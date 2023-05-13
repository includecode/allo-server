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
    void send_disconnect();
    
    /// @overload virtuals
    void run();
    
}; // Class User
