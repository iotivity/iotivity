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

/**
 * @brief Initialize Unicast UDP socket
 * @param   port        [IN]   Port to start the unicast server
 * @param   socketID    [IN]   Unicast socket ID
 * @return  #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAArduinoInitUdpSocket(int16_t *port, int32_t *socketID);

/**
 * @brief Initialize Multicast UDP socket
 * @param   mcastAddress    [IN] Port to start the unicast server
 * @param   mport           [IN] Multicast port
 * @param   lport           [IN] Local port on which the server is started
 * @param   socketID        [IN] Multicast socket ID
 * @return  #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAArduinoInitMulticastUdpSocket(const char *mcastAddress, const int16_t *mport,
                                           const int16_t *lport, int32_t *socketID);

/**
 * @brief   To parse the IP address
 * @param   ipAddrStr   [IN]   IP address to be parsed
 * @param   ipAddr      [OUT]  Parsed IP address
 * @param   port        [OUT]  Parsed Port number
 * @return  1 on Success otherwise failed to parse the address
 */
int16_t CAParseIPv4AddressLocal(unsigned char *ipAddrStr, uint8_t *ipAddr, uint16_t *port);

#ifdef __cplusplus
}
#endif

#endif //_CA_ETHERNET_ADAPTER_UTILS_

