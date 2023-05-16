/**
 *  Project SNOWPACK by PAvel SAMENE TIAH; FILE Proxy.h, GNU GENERAL PUBLIC LICENSE, May 2023
 * 
*/
#pragma once
#include "Common.h"
#include "Device.h"
#include <map>
#include <set>

using namespace std;

typedef struct userInfo_t
{
    int socketFd = -1;
    int pairedUserFd = -1; // socketFD of User that is already connected to this User
    string secret = string("");
} userInfo_t;


class Proxy : public Device
{
private:
    static std::map<int, shared_ptr<userInfo_t>> users; // all users connected to the Proxy, keys are thier socketFD

public:
    Proxy();
    ~Proxy();
    void processNewMessage(string message);
    void manageSocketInThread(int socketFd);
    bool connectToAnotherUSer(int newCommerFd);
    void processNewMessage(messageType_e messageType, std::shared_ptr<userInfo_t> &sender, string message = string(""));
    void disconnectUser(int socketFd);
    void removeUser(int userFD);

    /// @overload virtuals
    void run();

}; // Class Proxy
