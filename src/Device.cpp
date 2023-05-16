/**
 *  Project SNOWPACK by Pavel SAMENE TIAH; Device Device.cpp, GNU GENERAL PUBLIC LICENSE, May 2023
 * 
*/

#include "Device.h"
#include "poll.h"
#include "sys/ioctl.h"

Device::Device(deviceType_e type): type(type)
{
    this->socketFd = -1; //initialisation

    if(deviceType_e::PROXY == type)
    {
        this->portNumber = PROXY_PORT;
    }
    else
    {
        // Providing a seed value
	    srand((unsigned) time(NULL));
        // Ports from 0-1023 are reserved
        int random =  1023 + (rand() % 64000 );
        this->portNumber = random;
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

    // Prevent kernel from keeping socketFD alive in a waiting state when process is killed
    const int enable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    {
        cout << "Failed to set 'SO_REUSEADDR'" << endl;
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
        this->closeSocket(sockfd);
        cout << "ERROR on binding, please restart, errno: " << errno << endl;
        return -1;
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
    /// cout << "Debug:<---------" << message << " to " << socketFd <<endl; // "to FD:" << socketFd << endl;
    int ret = send(socketFd, message.c_str(), message.size(), 0);
    if (ret < 0)
    {
        cout << "ERROR writing to socket" << endl;
    }
    return ret;
}

/**
 * @brief Sends message on this devices socket file descriptor
 * @param  socketFd     : Socket file descriptor to receive from
 * @return \c string    : data read
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
        ///if(n > 0)
        ///{
        ///    cout << "Debug---------> " << ret << endl;
        ///}
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
    }
}

/**
 * @brief       : Set if reading from socket should be blocking until data received
 * @param type  : Blocking type for the socket
 * NOTE: if type is \c readBlockingType_e::BLOCKING , then reading is aborted when socket is closed
*/
void Device::enableDisableSocketBlocking(int socketFd, readBlockingType_e type)
{
    int readBlocking = static_cast<int>(type);
    ioctl(socketFd, FIONBIO, &readBlocking);
}

/**
 * @brief           : Get message type
 * @param message   : message
 * @param timeoutSec: Receive timeout in seconds
*/
messageType_e Device::getMessageType(const string message)
{
    messageType_e messageType = messageType_e::_COUNT;

    if(message.size())
    {
        int msgType = static_cast<int>(message.at(0) - '0');
        if(msgType < static_cast<int>(messageType_e::_COUNT) && msgType > 0)
        {
            messageType = static_cast<messageType_e>(msgType);
        }
    }

    return messageType;
}