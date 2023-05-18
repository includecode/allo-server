/**
 *  Project SNOWPACK by Pavel SAMENE TIAH; FILE User.h, GNU GENERAL PUBLIC LICENSE, May 2023
 * 
*/
#pragma once
#include "Common.h"
#include "Device.h"
#include <atomic>
#include <condition_variable>

#define USER_PAIR_TIME_OUT      30 // 30 s max to recieve a connection pair with another User
#define DEFAULT_READ_TIME_OUT   3600 // 1H max to recieve a message
#define WAIT_USER_RELEX_MS   400 // sleep for 400 ms

using namespace std;

class User : public Device
{
private:
    string secret;
    int pairedUserFd = -1;
    std::atomic<bool> isTimerExpired{false}; // Timer to wait for another user
    std::atomic<bool> isConnectedToAnotherUser{false};


public:
    User(string secret);
    ~User();
    void sendCommands();
    void waitForAnotherUser();
    void socketReadThread(std::condition_variable& cv, std::mutex &mutexTimerExpiredVar);
    void processNewMessage(messageType_e messageType, string message = string(""));
    
    /// @overload virtuals
    void run();
    
    // Getters and setters
    void setSocketFD(int socketFD);
    int getSocketFD();
    string getSecret();
    void setSecret(string secret);
    void setPairedUserFD(int paredFd);
    int getPairedUserFD();
    
}; // Class User
