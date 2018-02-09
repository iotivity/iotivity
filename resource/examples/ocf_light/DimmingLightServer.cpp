/*
 * DimmingLightServer.cpp
 */
#include <iostream>

#include "DimmingLightServer.h"
#include "OCPlatform.h"

using namespace OC;

DimmingLightServer::DimmingLightServer(void):
m_binaryswitchInstance(),
m_dimmingInstance()
{
    std::cout << "Running DimmingLightServer constructor" << std::endl;
}

DimmingLightServer::~DimmingLightServer(void)
{
    std::cout << "Running DimmingLightServer destructor" << std::endl;
}
