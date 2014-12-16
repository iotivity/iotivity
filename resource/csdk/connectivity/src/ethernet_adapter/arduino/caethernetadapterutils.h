/******************************************************************
*
* Copyright 2014 Samsung Electronics All Rights Reserved.
*
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

/**
 * @file caethernetadapterutils.h
 * @brief This file provides APIs ethernet client/server/network monitor modules
 */

#ifndef _CA_ETHERNET_ADAPTER_UTILS_
#define _CA_ETHERNET_ADAPTER_UTILS_

#include <Arduino.h>
#include <Ethernet.h>
#include <socket.h>
#include <w5100.h>
#include <EthernetUdp.h>
#include <IPAddress.h>

#include "logger.h"
#include "cacommon.h"
#include "caadapterinterface.h"
#include "caethernetadapter_singlethread.h"
#include "caadapterutils.h"

#ifdef __cplusplus
extern "C"
{
#endif

CAResult_t CAArduinoInitUdpSocket(int16_t *port, int32_t *socketID);

CAResult_t CAArduinoInitMulticastUdpSocket(const char *mcastAddress, const int16_t *mport,
        const int16_t *lport, int32_t *socketID);

int16_t CAParseIPv4AddressLocal(unsigned char *ipAddrStr, uint8_t *ipAddr, uint16_t *port);

#ifdef __cplusplus
}
#endif

#endif //_CA_ETHERNET_ADAPTER_UTILS_

