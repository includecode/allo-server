/**
 *  Project SNOWPACK by PAvel SAMENE TIAH; FILE Proxy.cpp, GNU GENERAL PUBLIC LICENSE, May 2023
 * 
*/
#include "Proxy.h"
#include <thread>
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

     // This listen() call tells the socket to listen to the incoming connections.
     // The listen() function places all incoming connection into a backlog queue
     // until accept() call accepts the connection.
     // Here, we set the maximum size for the backlog queue to 5.

    std::cout << "Waiting for clients..." << std::endl;
    while (1)
    {
        if(this->users.size() >= MAX_USERS)
        {
            std::cout << "Max users coount reached, Proxy does not accept more users" << std::endl;
            this_thread::sleep_for(chrono::seconds(5));
        }    

        listen(this->socketFd ,2);

        // The accept() call actually accepts an incoming connection
        clilen = sizeof(cli_addr);

        // This accept() function will write the connecting client's address info 
        // into the the address structure and the size of that structure is clilen.
        // The accept() returns a new socket file descriptor for the accepted connection.
        // So, the original socket file descriptor can continue to be used 
        // for accepting new connections while the new socker file descriptor is used for
        // communicating with the connected client.
        int newsockfd = accept(this->socketFd, 
                    (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) 
            printf("ERROR on accept\n");

        printf("server: got connection from %s port %d\n",
            inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));

        // If connected user does not exist, insert him in map
        if (this->users.find(newsockfd) == this->users.end())
        {
            shared_ptr<userInfo_t> userInfo = make_shared<userInfo_t>();
            userInfo->socketFd = newsockfd;
            this->users[newsockfd] = userInfo;

            // Send ACK
            this->processNewMessage(messageType_e::SYN_CONNECT, this->users[newsockfd], string(""));

            // Manage new User in its own thread
            std::thread newUserThread( [&] { this->spawnSocketInThread(newsockfd); } );
            newUserThread.detach(); // Fire and forget
        }

        /* Attempt to connect this new User to an existing User */
        if(this->connectToAnotherUSer(newsockfd))
        {
            // Inform both users that they are connected
            this->sendMessage(string(std::to_string(static_cast<int>(messageType_e::REMOTE_USER_OK)) + std::to_string(this->users[newsockfd]->pairedUserFd)), newsockfd);
            this->sendMessage(string(std::to_string(static_cast<int>(messageType_e::REMOTE_USER_OK)) + std::to_string(newsockfd)), this->users[newsockfd]->pairedUserFd);
        }
        else
        {
            // Inform new User that he is not connected
            this->sendMessage(string(std::to_string(static_cast<int>(messageType_e::REMOTE_USER_KO))), newsockfd);
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
void Proxy::processNewMessage(messageType_e messageType, std::shared_ptr<userInfo_t> &sender, string message)
{
    switch (messageType)
    {
    case messageType_e::SYN_CONNECT:
    {
        // Send ACK to User that he is conneted
        int ret = this->sendMessage(string(std::to_string(static_cast<int>(messageType_e::SYN_ACK))), sender->socketFd);
        if (ret < 0)
        {
            cout << "ERROR writing to socket" << endl;
        }
        break;
    }
    case messageType_e::SHARE_SECRET:
    {
        // Store Users secret
        std::map<int, shared_ptr<userInfo_t>>::iterator it;
        it = this->users.find(sender->socketFd);
        if(it != this->users.end())
        {
            this->users.at(sender->socketFd)->secret = message;
        }

        break;
    }
    case messageType_e::MESSAGE:
    {
        if(sender->pairedUserFd > -1)
        {
            // Forward message to user
            int ret = this->sendMessage(message, sender->pairedUserFd);
            if (ret < 0)
            {
                cout << "ERROR writing to socket" << endl;
            }
        }
        else
        {
            cout << "No user connected to forward this message" << endl;
        }
        
        break;
    }
    case messageType_e::DISCONNECT:
    {
        this->removeUser(sender->socketFd);
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
void Proxy::spawnSocketInThread(int socketFd)
{
    std::map<int, shared_ptr<userInfo_t>>::iterator it;
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
        std::map<int, shared_ptr<userInfo_t>>::iterator it;
        for (it = this->users.begin(); it != this->users.end(); it++)
        {
            if (it->first != newCommerFd)
            {
                it->second->pairedUserFd = newCommerFd;
                this->users[newCommerFd]->pairedUserFd = it->first;
                connected = true;
                break; // Only 2 users max
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
    // Find if some users where connected to him, and invalidate the connection
    std::map<int, shared_ptr<userInfo_t>>::iterator it;
    for(it = this->users.begin(); it != this->users.end(); it++)
    {
        if (it->second->pairedUserFd == userFD)
        {
            this->sendMessage(string(std::to_string(static_cast<int>(messageType_e::REMOTE_USER_KO))), it->first);
            it->second->pairedUserFd = -1;
        }
    }

    // Delete sender
    it = this->users.find(userFD);
    if(it != this->users.end())
    {
        this->users.erase(it);
    }
}
/* Getters ans Setters */