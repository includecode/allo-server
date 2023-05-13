/**
 *  Project SNOWPACK by PAvel SAMENE TIAH; FILE User.cpp, GNU GENERAL PUBLIC LICENSE, May 2023
 * 
*/

#include "User.h"

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
        cout << "User initialised with secret: " << this->secret << endl;
    }

    this->socketFd = this->openSocket();
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
    serv_addr.sin_port = htons(PROXY_PORT); // We can to read from server's port

    if (connect(this->socketFd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        cout << "ERROR connecting, make sure host is running and restart" << endl;
    }

    int ret = this->sendMessage(string("Hello from User"), this->socketFd);
    if (ret < 0)
    {
         cout << "ERROR writing to socket\n" << endl;
    }
    
    string response = this->receiveMessage(this->socketFd);
    if (!response.size())
    {
        cout << "No response recieved " << response << endl;
    }

    this->closeSocket(this->socketFd);
}