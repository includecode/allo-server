/**
 *  Project SNOWPACK by PAvel SAMENE TIAH; Device Device.h, GNU GENERAL PUBLIC LICENSE, May 2023
 * 
*/
#pragma once
#include "Common.h"
#include "Device.h"
#include <fstream>

#define PROXY_PORT 1030
#define MAX_MSG_SIZE 1024
#define MAX_USERS 2 // Max two users

using namespace std;

enum class deviceType_e
{
    PROXY,
    USER
};

enum class messageType_e
{
    SYN_CONNECT,
    SYN_ACK,
    SHARE_SECRET,
    REMOTE_USER_OK,
    REMOTE_USER_KO,
    DISCONNECT,
    MESSAGE,
    _COUNT
};

class Device
{
protected:
    deviceType_e type;
    int socketFd; // Socket file descriptor
    int portNumber;

public:
    Device(deviceType_e type);
    ~Device();
    int openSocket();
    int sendMessage(string message, int socketFd);
    string receiveMessage(int socketFd);
    void closeSocket(int socketFd);
    void setSocketReceiveTimeOut(int socketFD, int timeoutSec);
    messageType_e getMessageType(const string message);

    virtual void run() = 0;
};