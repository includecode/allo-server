/**
 *  Project SNOWPACK by Pavel SAMENE TIAH; FILE Proxy.cpp, GNU GENERAL PUBLIC LICENSE, May 2023
 * 
*/
#include "Proxy.h"
#include <thread>
std::map<int, shared_ptr<User>> Proxy::users; // all users connected to the Proxy, keys are thier socketFD

Proxy::Proxy():
    Device(deviceType_e::PROXY)
{
}


Proxy::~Proxy()
{
}

void Proxy::run()
{
    this->socketFd = this->openSocket();

    if(this->socketFd < 0)
    {
        return;
    }

    struct sockaddr_in cli_addr;
    socklen_t clilen;

    std::cout << "Waiting for clients..." << std::endl;
    while (1)
    {
        if(this->users.size() >= MAX_USERS)
        {
            /// std::cout << "Max users coount reached, Proxy does not accept more users" << std::endl;
            this_thread::sleep_for(chrono::seconds(5));
        }    

        listen(this->socketFd ,MAX_USERS);

        // The accept() call actually accepts an incoming connection
        clilen = sizeof(cli_addr);

        int newsockfd = accept(this->socketFd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0)
        {
            cout << "ERROR on accept" << endl;
        }

        cout << "server: got connection from " << inet_ntoa(cli_addr.sin_addr) << "port " << ntohs(cli_addr.sin_port) << endl;

        // If connected user does not exist, insert him in map
        if (this->users.find(newsockfd) == this->users.end())
        {
            shared_ptr<User> userInfo = make_shared<User>("");
            userInfo->setSocketFD(newsockfd);
            this->users[newsockfd] = userInfo;

            // Send ACK
            this->processNewMessage(messageType_e::SYN_CONNECT, this->users[newsockfd], string(""));

            // Manage new User in its own thread
            std::thread newUserThread( [&] { this->manageSocketInThread(newsockfd); } );
            newUserThread.detach(); // Fire and forget
        }
    }

     this->closeSocket(this->socketFd);
}

/**
 * @brief Process a new incomming message from a \c User
 * @param messageType   : Type of the message
 * @param sender        : User who sent the \c message
 * @param message       : Optional message for message with type \c messageType_e::SEND_MESSAGE or \c messageType_e::MESSAGE
*/
void Proxy::processNewMessage(messageType_e messageType, std::shared_ptr<User> &sender, string message)
{
    // Server also prints the received message
    if(message.size() && (messageType_e::MESSAGE == messageType))
    {
        cout << message << endl;
    }

    switch (messageType)
    {
    case messageType_e::SYN_CONNECT:
    {
        // Send ACK to User that he is conneted
        this->sendMessage(string(std::to_string(static_cast<int>(messageType_e::SYN_ACK))), sender->getSocketFD());

        break;
    }
    case messageType_e::SHARE_SECRET:
    {
        // Store Users secret
        std::map<int, shared_ptr<User>>::iterator it;
        it = this->users.find(sender->getSocketFD());
        if(it != this->users.end())
        {
            this->users.at(sender->getSocketFD())->setSecret(message);

            /* Attempt to connect this new User to an existing User */
            if(this->connectToAnotherUSer(sender->getSocketFD()))
            {
                // Inform both users that they are connected
                this->sendMessage(string(std::to_string(static_cast<int>(messageType_e::REMOTE_USER_OK)) \
                    + std::to_string(this->users[sender->getSocketFD()]->getPairedUserFD())), sender->getSocketFD());

                this->sendMessage(string(std::to_string(static_cast<int>(messageType_e::REMOTE_USER_OK)) \
                    + std::to_string(sender->getSocketFD())), this->users[sender->getSocketFD()]->getPairedUserFD());
            }
            else
            {
                // Inform new User that he is not connected
                this->sendMessage(string(std::to_string(static_cast<int>(messageType_e::REMOTE_USER_KO))), sender->getSocketFD());
            }
        }

        break;
    }
    case messageType_e::MESSAGE:
    {
        if(sender->getPairedUserFD() > -1)
        {
            // Forward message to user
            this->sendMessage(string(std::to_string(static_cast<int>(messageType_e::MESSAGE))) +  message, sender->getPairedUserFD());
        }
        else
        {
            cout << "No second user found to receive the message" << endl;
        }
        
        break;
    }
    case messageType_e::DISCONNECT:
    {
        this->removeUser(sender->getSocketFD());
        break;            
    }
    
    default:
        break;
    }
}

/**
 * @brief this function is an endless manager for one single socket, it keeps reading from the socket's file descriptor
 * an proceeds to the execution on the receive message. This funtion should be passed in a thread
 * @param socketFD  : Socket file descriptor to manage I/O
*/
void Proxy::manageSocketInThread(int socketFd)
{
    std::map<int, shared_ptr<User>>::iterator it;
    while (1)
    {
        string message = this->receiveMessage(socketFd);

        // If connected user does not exist, exit
        it = this->users.find(socketFd);
        if (it == this->users.end() || !message.size())
        {
            cout << "Error: User: [FD:" << socketFd << "] unexpectedly closed the connection" << endl;
            this->removeUser(socketFd); //  Usefull when message is empty (Client closed socket)
            break;
        }

        messageType_e messageType = this->getMessageType(message);
        if(messageType != messageType_e::_COUNT)
        {
            this->processNewMessage(messageType, this->users[socketFd], message.substr(1));
        }
    }
    this->closeSocket(socketFd);
}

/**
 * @brief Attempt to connect a new user to an existing user
 * @param newCommerFd   : File descriptor of the new user
 * @return \c bool \c true if both users are connected, false otherwise
*/
bool Proxy::connectToAnotherUSer(int newCommerFd)
{
    bool connected = false;
    if(this->users.size() > 1)
    {
        std::map<int, shared_ptr<User>>::iterator it;
        for (it = this->users.begin(); it != this->users.end(); it++)
        {
            if (it->first != newCommerFd)
            {
                if(it->second->getSecret() == this->users[newCommerFd]->getSecret())
                {
                    it->second->setPairedUserFD(newCommerFd);
                    this->users[newCommerFd]->setPairedUserFD(it->first);
                    connected = true;
                }
                else
                {
                    cout << "User not paired because of secret mismatch" << endl;
                }
            }
        }
    }

    return connected;
}

/**
 * @brief Remove a User from known Users
 * @param userFD   : File descriptor of the new user to remove
*/
void Proxy::removeUser(int userFD)
{
    // Find if some users where connected to him, invalidate the connection and send them a notification
    std::map<int, shared_ptr<User>>::iterator it;
    for(it = this->users.begin(); it != this->users.end(); it++)
    {
        if (it->second->getPairedUserFD() == userFD)
        {
            this->sendMessage(string(std::to_string(static_cast<int>(messageType_e::REMOTE_USER_KO))), it->first);
            it->second->setPairedUserFD(-1);
        }
    }

    // Delete sender
    it = this->users.find(userFD);
    if(it != this->users.end())
    {
        this->users.erase(it);
    }
}