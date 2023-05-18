/**
 *  Project SNOWPACK by Pavel SAMENE TIAH; FILE Proxy.h, GNU GENERAL PUBLIC LICENSE, May 2023
 * 
*/
#pragma once
#include "Common.h"
#include "Device.h"
#include "User.h"
#include <map>
#include <set>

using namespace std;

class Proxy : public Device
{
private:
    static std::map<int, shared_ptr<User>> users; // all users connected to the Proxy, keys are thier socketFD

public:
    Proxy();
    ~Proxy();
    void processNewMessage(string message);
    void manageSocketInThread(int socketFd);
    bool connectToAnotherUSer(int newCommerFd);
    void processNewMessage(messageType_e messageType, std::shared_ptr<User> &sender, string message = string(""));
    void disconnectUser(int socketFd);
    void removeUser(int userFD);

    /// @overload virtuals
    void run();

}; // Class Proxy
