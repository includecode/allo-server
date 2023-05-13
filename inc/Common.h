/**
 *  Project SNOWPACK by PAvel SAMENE TIAH; FILE Common.h, GNU GENERAL PUBLIC LICENSE, May 2023
 * 
*/
#pragma once
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

#define PROXY_PORT 1030
#define MAX_MSG_SIZE 1024

enum class deviceType_e
{
    PROXY,
    USER
};