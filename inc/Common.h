/**
 *  Project SNOWPACK by Pavel SAMENE TIAH; FILE Common.h, GNU GENERAL PUBLIC LICENSE, May 2023
 * 
*/
#pragma once

// STL
#include <iostream>
#include <vector>
#include <memory>
#include <cstring>
#include "stdint.h"

// C sockets
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>

// User defined
#include "ExceptionHandler.h"

#define LOG_TO_FILE(msg) ExceptionHandler::writeTologFile(std::string( __PRETTY_FUNCTION__).append(" ") + msg)