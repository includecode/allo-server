/**
 *  Project SNOWPACK by Pavel SAMENE TIAH; FILE User.cpp, GNU GENERAL PUBLIC LICENSE, May 2023
 * 
*/

#include "User.h"
#include <thread>
#include <chrono>

User::User(string secret):
    Device(deviceType_e::USER), secret(secret)
{
}

User::~User()
{
}

void User::run()
{
    if(this->secret.size())
    {
        cout << "NOTE: User initialised with secret: ";
        cout << this->secret.at(0);
        for (size_t i = 1; i < this->secret.size(); i++)
        {
            cout << "*";
        }
        cout << endl;
    }

    this->socketFd = this->openSocket();

    if(this->socketFd < 0)
    {
        return;
    }

    struct sockaddr_in serv_addr;
    struct hostent *server;

    server = gethostbyname("localhost");
    if (server == NULL) 
    {
        cout << "ERROR, no such host" << endl;
        exit(0);
    }
 
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(PROXY_PORT); // We want to read from server's port

    if (connect(this->socketFd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        cout << "ERROR connecting, make sure host is running and restart" << endl;
        this->closeSocket(this->socketFd);
        return;
    }
    
    /* Wait for SYN_ACK */
    string message = this->receiveMessage(this->socketFd);
    messageType_e messageType = this->getMessageType(message);
    if(messageType != messageType_e::_COUNT)
    {
        this->processNewMessage(messageType, message.substr(1));
    }

    this->enableDisableSocketBlocking(this->socketFd, readBlockingType_e::NON_BLOCKING);

    this->waitForAnotherUser();

    this->sendMessage(std::to_string(static_cast<int>(messageType_e::DISCONNECT)), this->socketFd);

    this->closeSocket(this->socketFd);
}

/**
 * @brief Process a new incomming message from a \c User
 * @param messageType   : Type of the message
 * @param message       : Optional message for message with type \c messageType_e::SEND_MESSAGE or \c messageType_e::MESSAGE
*/
void User::processNewMessage(messageType_e messageType, string message)
{
    switch (messageType)
    {
    case messageType_e::SYN_ACK:
    {
        cout << "Successfuly connected to Proxy" << endl;

        // Share my secret with the Proxy, even if empty
        this->sendMessage(std::to_string(static_cast<int>(messageType_e::SHARE_SECRET)) + this->secret, this->socketFd);
        
        break;
    }
    case messageType_e::REMOTE_USER_OK:
    {
        this->isConnectedToAnotherUser = true;
        cout << "---------- Successfuly connected to a user wich has FD: " << message << " ------------" << endl;
        break;
    }
    case messageType_e::REMOTE_USER_KO:
    {
        this->isConnectedToAnotherUser = false;
        cout << "No user is available to pair" << endl;
        break;            
    }
    case messageType_e::MESSAGE:
    {
        bool isEchoReply = false;
        size_t pos = message.find(" ") ;
        if(pos != std::string::npos)
        {
            string toFind = string("ECHOREPLY");

            // search for "ECHOREPLY" in the received message, 9 first chars
            if(message.substr(0, toFind.size()).find(toFind) != std::string::npos)
            {
                isEchoReply = true;
            }
        }

        message = message.substr(pos +1);
        cout << message << endl;
        if(isEchoReply)
        {
            // Resend the same message to the User
            this->sendMessage(std::to_string(static_cast<int>(messageType_e::MESSAGE)) + message, this->socketFd);
        }
        
        break;            
    }
    
    default:
        break;
    }
}

/**
 * @brief Read commands from user input and send to Proxy
*/
void User::sendCommands()
{
    std::string line;
    cout << "-------- @@@ AT ANY TIME @@@, type a message and press enter -----------" << endl;
    while(1)
    {
        std::getline(std::cin, line);
        this->sendMessage(string(std::to_string(static_cast<int>(messageType_e::MESSAGE))) + line, this->socketFd);
    }
}

/**
 * @brief Wait for a message from Proxy saying another user has joined
*/
void User::waitForAnotherUser()
{
    std::condition_variable cv;
    std::mutex mutexTimerExpiredVar;
 
    std::unique_lock<std::mutex> lk(mutexTimerExpiredVar);
    auto now = std::chrono::system_clock::now();

    // Call reader thread
    std::thread reader([&] {this->socketReadThread(cv, mutexTimerExpiredVar);});
    reader.detach(); // Fire and forget

    // Wait for reader thread until timeout
    if(cv.wait_until(lk, now + USER_PAIR_TIME_OUT * 1000ms, [&](){return this->isConnectedToAnotherUser == true;}))
    {
        // Thread has notify()'d before timeout expired => another user has joined
        lk.unlock();
        this->sendCommands();
    }
    else
    {
        // Stop waiting the thread "socketReadThread"
        cout << USER_PAIR_TIME_OUT << " s Timeout expired: Exiting..." << endl;
        lk.unlock();
        mutexTimerExpiredVar.lock(); 
        this->isTimerExpired = true;
        mutexTimerExpiredVar.unlock();
    }
}

/**
 * @brief Wait for a message from Proxy
 * @param cv                    : Conditional variable shared with waiter thread
 * @param mutexTimerExpiredVar  : Mutex shared with waiter thread, used to end this thread
*/
void User::socketReadThread(std::condition_variable& cv, std::mutex& mutexTimerExpiredVar)
{
    bool firstMessage = true;
    while (1)
    {
        // Manage timer case, flag updated by caller thread
        mutexTimerExpiredVar.lock(); 
        if(this->isTimerExpired)
        {
            mutexTimerExpiredVar.unlock();
            break;
        }
        mutexTimerExpiredVar.unlock();

        // Listen for messages from Proxy
        string message = this->receiveMessage(this->socketFd);
        if(message.size())
        {
            messageType_e messageType = this->getMessageType(message);
            if(messageType != messageType_e::_COUNT)
            {
                this->processNewMessage(messageType, message.substr(1));
                if(messageType_e::REMOTE_USER_OK == messageType)
                {
                    cv.notify_all();
                }
            }

            // Remote User disconnected
            if(messageType == messageType_e::REMOTE_USER_KO && (!firstMessage))
            {
                this->isConnectedToAnotherUser = false;
            }
        }

        // Tempo
        this_thread::sleep_for(chrono::milliseconds(WAIT_USER_RELEX_MS));
        firstMessage = false;
    }
}