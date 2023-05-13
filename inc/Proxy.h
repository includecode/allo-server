/**
 *  Project SNOWPACK by PAvel SAMENE TIAH; FILE Proxy.h, GNU GENERAL PUBLIC LICENSE, May 2023
 * 
*/
#pragma once
#include "Common.h"
#include "Device.h"

using namespace std;

class Proxy : public Device
{
private:
    uint8_t nbUSersConnected;

public:
    Proxy();
    ~Proxy();

    /// @overload virtuals
    void run();
    bool waitRemoteUser(uint16_t timeout);
    bool sendMessage(uint16_t timeout);

}; // Class Proxy
