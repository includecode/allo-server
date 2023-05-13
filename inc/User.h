/**
 *  Project SNOWPACK by PAvel SAMENE TIAH; FILE User.h, GNU GENERAL PUBLIC LICENSE, May 2023
 * 
*/
#pragma once
#include "Common.h"
#include "Device.h"

using namespace std;

class User : public Device
{
private:
    string secret;

public:
    User(string secret);
    ~User();
    bool send_SYN_connect(uint16_t timeout);
    
    /// @overload virtuals
    void run();
    bool waitRemoteUser(uint16_t timeout);
    bool sendMessage(uint16_t timeout);
    
}; // Class User
