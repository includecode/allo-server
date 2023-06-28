/**
 *  Project SNOWPACK by Pavel SAMENE TIAH; ExceptionHandler.h, GNU GENERAL PUBLIC LICENSE, May 2023
 * 
*/
#pragma once
#include <exception>
#include <string>

using namespace std;

enum class exceptionType_e
{
    WARNING,
    CRITICAL
};

class ExceptionHandler : public exception {

private:
    std::string msg = std::string("");

public:
    ExceptionHandler(string msg, exceptionType_e type);

    const char * what () const throw ();

    static void writeTologFile(const string &message);
};