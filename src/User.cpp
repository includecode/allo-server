/**
 *  Project SNOWPACK by PAvel SAMENE TIAH; FILE User.cpp, GNU GENERAL PUBLIC LICENSE, May 2023
 * 
*/

#include "User.h"
#include <cmath>

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
}

bool User::waitRemoteUser(uint16_t timeout)
{
    return true;
}
bool User::sendMessage(uint16_t timeout)
{
    return true;
}