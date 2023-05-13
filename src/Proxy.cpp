/**
 *  Project SNOWPACK by PAvel SAMENE TIAH; FILE Proxy.cpp, GNU GENERAL PUBLIC LICENSE, May 2023
 * 
*/
#include "Proxy.h"

Proxy::Proxy():
    Device(deviceType_e::PROXY), nbUSersConnected(0)
{
}


Proxy::~Proxy()
{
}

void Proxy::run()
{
    this->socketFd = this->openSocket();
    struct sockaddr_in cli_addr;
    socklen_t clilen;
    char buffer[256] = {0x00};

     // This listen() call tells the socket to listen to the incoming connections.
     // The listen() function places all incoming connection into a backlog queue
     // until accept() call accepts the connection.
     // Here, we set the maximum size for the backlog queue to 5.

    std::cout << "Waiting for clients..." << std::endl;
    
    listen(this->socketFd ,5);

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

    int ret = this->sendMessage(string("Hello from Proxy"), newsockfd);
    if (ret < 0)
    {
        cout << "ERROR writing to socket" << endl;
    }

     //int n = read(newsockfd,buffer,255);
     string response = this->receiveMessage(newsockfd);
     if (!response.size())
     {
        cout << "No response recieved " << response << endl;
     }

     this->closeSocket(newsockfd);

     this->closeSocket(this->socketFd);
}

/* Getters ans Setters */