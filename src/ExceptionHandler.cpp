/**
 *  Project SNOWPACK by Pavel SAMENE TIAH; FILE ExceptionHandler.cpp, GNU GENERAL PUBLIC LICENSE, May 2023
 * 
*/

#include <fstream>
#include "Common.h"

ExceptionHandler::ExceptionHandler(string msg, exceptionType_e type)
{
    this->msg = std::string(__PRETTY_FUNCTION__);
    this->msg.append(": " + msg);

    if(exceptionType_e::WARNING == type)
    {
        this->msg.append(" : (WARNING)");
    }
    else
    {
        this->msg.append(" : (CRITICAL)");
    }
}

/**
 * @brief override what() with a custom message taken from constructor
*/
const char * ExceptionHandler::what () const throw ()
{
    return msg.c_str();
}

/**
 * @brief Write exception to log file
*/
void ExceptionHandler::writeTologFile(const string &message)
{
    ofstream file("logs/log.log");

    if(file.is_open())
    {
        file << message;
        file.close();
    }
    else
    {
        cout << "Unable to open log file" << endl;
    }
}