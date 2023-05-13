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

public:
    Device(deviceType_e type);
    ~Device();
    virtual void run() = 0;
    virtual bool waitRemoteUser(uint16_t timeout) = 0;
    virtual bool sendMessage(uint16_t timeout) = 0;
};