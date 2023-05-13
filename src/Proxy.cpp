/**
 *  Project SNOWPACK by PAvel SAMENE TIAH; FILE Proxy.cpp, GNU GENERAL PUBLIC LICENSE, May 2023
 * 
*/
#include "Proxy.h"

Proxy::Proxy():
    Device(deviceType_e::PROXY), nbUSersConnected(0)
{
}


Proxy::~Proxy()
{
}

void Proxy::run()
{
}

bool Proxy::waitRemoteUser(uint16_t timeout)
{
    return true;
}
bool Proxy::sendMessage(uint16_t timeout)
{
    return true;
}


/* Getters ans Setters */