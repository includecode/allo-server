/**
 *  Project SNOWPACK by PAvel SAMENE TIAH; Device Device.h, GNU GENERAL PUBLIC LICENSE, May 2023
 * 
*/
#pragma once
#include "Common.h"
#include "Device.h"
#include <fstream>

using namespace std;

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

    virtual void run() = 0;
};