/**
 *  Project SNOWPACK by PAvel SAMENE TIAH; Device Device.cpp, GNU GENERAL PUBLIC LICENSE, May 2023
 * 
*/

#include "Device.h"

Device::Device(deviceType_e type): type(type)
{
    this->socketFd = -1; //initialisation

    if(deviceType_e::PROXY == type)
    {
        this->portNumber = PROXY_PORT;
    }
    else
    {
        this->portNumber = PROXY_PORT +1;
    }
}

Device::~Device()
{
}


/**
 * @brief   : Open main socket and return it's file descriptor
 * @param   : Port number for this device
 * @return  : \c int Socket's file descriptor, or -1 on error 
*/
int Device::openSocket()
{
    int sockfd;
    struct sockaddr_in serv_addr, cli_addr;
    
    // create a socket
    sockfd =  socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        cout << "ERROR opening socket" << endl;
        return sockfd;
    }
   
    cout << "Port: " << this->portNumber <<  ", \tFD: " << sockfd << endl;

    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;  

    // automatically be filled with current host's IP address
    serv_addr.sin_addr.s_addr = INADDR_ANY;  

    // convert short integer value for port must be converted into network byte order
    serv_addr.sin_port = htons(this->portNumber);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        cout << "ERROR on binding" << endl;
    }

    return sockfd;
}

/**
 * @brief Sends message on this devices socket file descriptor
 * @param  socketFd : Socket file descriptor to send
 * @return number of bytes sent or error code if send error
*/
int Device::sendMessage(string message, int socketFd)
{
    /*if(this->type == deviceType_e::PROXY)
    return write(socketFd, message.c_str(), message.size());
    else*/
    return (send(socketFd, message.c_str(), message.size(), 0));
}

/**
 * @brief Sends message on this devices socket file descriptor
 * @param  socketFd : Socket file descriptor to receive from
 * @return number of bytes sent or error code if send error
*/
string Device::receiveMessage(int socketFd)
{
    char receiveBuff[MAX_MSG_SIZE] = {0x00};
    int n = read(socketFd, receiveBuff, 255);
    string ret = string("");

    if(n >= 0)
    {
        string strRemoveEmpty = string(receiveBuff);
        ret = string(strRemoveEmpty, 0, n); // Get a string matching the excat length (remove remaining empty buff)
        cout << "---------> " << ret << endl;
    }

    return ret;
}

/**
 * @brief           : Open main socket and return it's file descriptor
 * @param socketFd  : File descriptor of the socket to close
*/
void Device::closeSocket(int socketFd)
{
    if(socketFd > -1)
    {
        close(socketFd);
        cout << "Connection closed succesfully from sockefFD:" << socketFd << endl;
    }
}