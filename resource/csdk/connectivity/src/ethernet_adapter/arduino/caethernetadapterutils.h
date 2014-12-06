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

#ifndef _ETHERNET_ADAPTER_ARDUINO_H_
#define _ETHERNET_ADAPTER_ARDUINO_H_

#include "caadapterinterface.h"

#ifdef __cplusplus
extern "C" {
#endif

CAResult_t CAStartUnicastServer(const char *localAddress, int16_t *port);

// Retrieves the IP address assigned to Arduino Ethernet shield
CAResult_t CAArduinoGetInterfaceAddress(char *address, int32_t addrLen);

CAResult_t CAArduinoInitUdpSocket(int16_t *port, int32_t *socketID);

CAResult_t CAArduinoInitMulticastUdpSocket(const char *mcastAddress, const int16_t *mport,
        const int16_t *lport, int32_t *socketID);

/**
 * @brief API to start multicast server.
 * @param mcastAddress - IP address to join multicast group.
 * @param localAddress - Local Unicast IP address to bind the socket.
 * @param port - multicast port number where socket will listen for incoming request.
 * @return - Error Code
 */
CAResult_t CAStartMulticastServer(const char *mcastAddress, const char *localAddress,
                                  const int16_t *port);

CAResult_t CAStopUnicastServer();

void CAArduoinoCheckServerData();

CAResult_t CAStopMulticastServer();

uint32_t CAEthernetSendData(const char *remoteIpAddress, const int16_t port, const char *buf,
                            uint32_t bufLen,
                            int16_t isMulticast);

/// Send data to requested end-point using UDP socket
uint32_t CAArduinoSendData(int32_t sockFd, const uint8_t *buf, uint32_t bufLen,
                           const char *ipAddress,
                           int32_t port);

void CACheckData();

void CANotifyCallback(void *data, int32_t dataLen, const char *senderIp, int32_t senderPort);

/// Retrieve any available data from UDP socket. This is a non-blocking call.
int32_t CAArduinoRecvData(int32_t sockFd, uint8_t *buf, uint32_t bufLen, uint8_t *senderAddr,
                          uint16_t *senderPort);

void CASetEthernetNetworkPacketCallback(CANetworkPacketReceivedCallback callback);

int16_t CAParseIPv4AddressLocal(unsigned char *ipAddrStr, uint8_t *ipAddr, uint16_t *port);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif


