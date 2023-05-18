/**
 *  Project SNOWPACK by Pavel SAMENE TIAH; FILE Main.cpp, GNU GENERAL PUBLIC LICENSE, May 2023
 * 
*/
#include "Device.h"
#include "User.h"
#include "Proxy.h"

void runProxy();
void runUser(string secret);
bool checkArguments(const int argc, char const **argv, string& mode, string& secret);
void displayHelp();

int main(int argc, char const **argv)
{
    string mode("");
    string secret("");

    bool argError = checkArguments(argc, argv, mode, secret);
    if(argError)
    {
        displayHelp();
    }
    else
    {
        if(mode == string("Proxy"))
        {
            runProxy();
        }
        else
        {
            runUser(secret);
        }
    }

    return 0;
}

/**
 * @brief run Proxy server
*/
void runProxy()
{
    shared_ptr<Proxy> proxy = make_shared<Proxy>();
    proxy->run();
}

/**
 * @brief run User client
*/
void runUser(string secret)
{
    shared_ptr<User> user = make_shared<User>(secret);
    user->run();
}

/**
 * @brief check command line arguments from user
 * @return \c bool True if arguments are good, false otherwise
*/
bool checkArguments(const int argc, char const **argv, string& mode, string& secret)
{
    bool argError = false;
    if(argc > 1)
    {
        mode = string(argv[1]);
        if(mode != string("Proxy") && mode != string("User"))
        {
            argError = true;
            std::cout << "Unknown device type: "<< mode << std::endl;
        }
    }

    if(argc > 2)
    {
        secret = string(argv[2]);
        if(secret.size() <= 0)
        {
            argError = true;
            std::cout << "Secret is empty"<< mode << std::endl;
        }
    }

    if(argc > 3 || argc < 2)
    {
        argError = true;
        std::cout << "Too many or few arguments" << std::endl;
    }

    return argError;
}


/**
 * @brief Display CLI usage
*/
void displayHelp()
{
    std::cout << "\n\n**********" << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "\t./app   <mode>   [secret]" << std::endl;
    std::cout << "mode          : Proxy / User" << std::endl;
    std::cout << "secret        : Optional secret for this (User) device" << std::endl;
    std::cout << "Example       : ./app   User  Sn0wp@ck" << std::endl;
    std::cout << "This example will run as User device with its secret value Sn0wp@ck" << std::endl;
    std::cout << "Secret supplied for Proxy devices are ignored" << std::endl;
    std::cout << "**********" << std::endl;
}