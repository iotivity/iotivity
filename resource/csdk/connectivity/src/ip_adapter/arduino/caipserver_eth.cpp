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

#include "caipinterface_singlethread.h"

#include <Arduino.h>
#include <Ethernet.h>
#include <socket.h>
#include <w5100.h>
#include <EthernetUdp.h>
#include <IPAddress.h>

#include "logger.h"
#include "cacommon.h"
#include "cainterface.h"
#include "caadapterinterface.h"
#include "caipadapter_singlethread.h"
#include "caipadapterutils_eth.h"
#include "caadapterutils.h"
#include "oic_malloc.h"

#define TAG "IPS"

// Length of the IP address decimal notation string
#define IPNAMESIZE (16)

CAResult_t CAIPStartUnicastServer(const char *localAddress, uint16_t *port,
                                        const bool forceStart, int32_t *serverFD);
static CAResult_t CAArduinoRecvData(int32_t sockFd);
static CAResult_t CAArduinoGetInterfaceAddress(char *address, int32_t addrLen);
static void CAArduinoCheckData();
static void CAPacketReceivedCallback(const char *ipAddress, const uint16_t port,
                              const void *data, const uint32_t dataLength);

static CAIPPacketReceivedCallback g_packetReceivedCallback = NULL;
static int g_unicastSocket = 0;
static int g_multicastSocket = 0;

/**
 * @var g_unicastPort
 * @brief Unicast Port
 */
static uint16_t g_unicastPort = 0;

CAResult_t CAIPInitializeServer(void)
{
    return CA_STATUS_OK;
}

void CAIPTerminateServer(void)
{
    return;
}

CAResult_t CAIPGetUnicastServerInfo(char **ipAddress, uint16_t *port,
                                          int *serverID)
{
    return CA_STATUS_OK;
}

CAResult_t CAIPStartUnicastServer(const char *localAddress, uint16_t *port,
                                        const bool forceStart, int *serverFD)
{
    OIC_LOG(DEBUG, TAG, "IN");
    VERIFY_NON_NULL(port, TAG, "port");

    uint8_t rawIPAddr[4];
    char address[16];
    W5100.getIPAddress(rawIPAddr);
    sprintf(address, "%d.%d.%d.%d", rawIPAddr[0], rawIPAddr[1], rawIPAddr[2], rawIPAddr[3]);
    OIC_LOG_V(DEBUG, TAG, "address:%s", address);

    if (CAArduinoInitUdpSocket(port, serverFD) != CA_STATUS_OK)
    {
        OIC_LOG(DEBUG, TAG, "failed");
        return CA_STATUS_FAILED;
    }

    g_unicastPort = *port;
    g_unicastSocket = *serverFD;
    OIC_LOG_V(DEBUG, TAG, "g_unicastPort: %d", g_unicastPort);
    OIC_LOG_V(DEBUG, TAG, "g_unicastSocket: %d", g_unicastSocket);
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAIPStartMulticastServer(const char *localAddress, const char *multicastAddress,
                                    uint16_t multicastPort, int *serverFD)
{
    OIC_LOG(DEBUG, TAG, "IN");
    if (CAArduinoInitMulticastUdpSocket(multicastAddress, multicastPort, multicastPort,
                                        serverFD) != CA_STATUS_OK)
    {
        OIC_LOG(DEBUG, TAG, "failed");
        return CA_STATUS_FAILED;
    }

    g_multicastSocket = *serverFD;
    OIC_LOG_V(DEBUG, TAG, "gMulticastPort: %d", multicastPort);
    OIC_LOG_V(DEBUG, TAG, "g_multicastSocket: %d", g_multicastSocket);
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAIPStopUnicastServer()
{
    OIC_LOG(DEBUG, TAG, "IN");
    close(g_unicastSocket);
    g_unicastSocket = 0;
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAIPStopMulticastServer()
{
    OIC_LOG(DEBUG, TAG, "IN");
    close(g_multicastSocket);
    g_multicastSocket = 0;
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

void CAPacketReceivedCallback(const char *ipAddress, const uint16_t port,
                              const void *data, const uint32_t dataLength)
{
    OIC_LOG(DEBUG, TAG, "IN");
    if (g_packetReceivedCallback)
    {
        g_packetReceivedCallback(ipAddress, port, data, dataLength);
    }
    OIC_LOG(DEBUG, TAG, "OUT");
}

void CAArduinoCheckData()
{
    if (g_unicastSocket)
    {
        if (CAArduinoRecvData(g_unicastSocket) != CA_STATUS_OK)
        {
            OIC_LOG(ERROR, TAG, "rcv fail");
            CAIPStopUnicastServer();
        }
    }

    if (g_multicastSocket)
    {
        if (CAArduinoRecvData(g_multicastSocket) != CA_STATUS_OK)
        {
            OIC_LOG(ERROR, TAG, "rcv fail");
            CAIPStopMulticastServer();
        }
    }
}

/** Retrieve any available data from UDP socket and call callback.
 *  This is a non-blocking call.
 */
CAResult_t CAArduinoRecvData(int32_t sockFd)
{
    /**Bug : When there are multiple UDP packets in Wiznet buffer, W5100.getRXReceivedSize
     * will not return correct length of the first packet.
     * Fix : Use the patch provided for arduino/libraries/Ethernet/utility/socket.cpp
     */

    void *data = NULL;
    uint8_t senderAddr[4] = { 0 };
    char addr[IPNAMESIZE] = {0};
    uint16_t senderPort = 0;

    uint16_t recvLen = W5100.getRXReceivedSize(sockFd);
    if (recvLen == 0)
    {
        // No data available on socket
        return CA_STATUS_OK;
    }

    OIC_LOG_V(DEBUG, TAG, "rcvd %d", recvLen);
    recvLen = recvLen > COAP_MAX_PDU_SIZE ? COAP_MAX_PDU_SIZE:recvLen;

    data = OICCalloc(recvLen + 1, 1);
    if (NULL == data)
    {
        OIC_LOG(DEBUG, TAG, "Out of memory!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    // Read available data.
    int32_t ret = recvfrom(sockFd, (uint8_t *)data, recvLen + 1, senderAddr, &senderPort);
    if (ret < 0)
    {
        OIC_LOG(ERROR, TAG, "rcv fail");
        OICFree(data);
        return CA_STATUS_FAILED;
    }
    else if (ret > 0)
    {
        OIC_LOG(DEBUG, TAG, "data recvd");
        snprintf(addr, sizeof(addr), "%d.%d.%d.%d", senderAddr[0], senderAddr[1], senderAddr[2],
                 senderAddr[3]);
        CAPacketReceivedCallback(addr, senderPort, data, ret);
    }

    OICFree(data);

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

void CAIPSetPacketReceiveCallback(CAIPPacketReceivedCallback callback)
{
    OIC_LOG(DEBUG, TAG, "IN");
    g_packetReceivedCallback = callback;
    OIC_LOG(DEBUG, TAG, "OUT");
}

void CAIPSetExceptionCallback(CAIPExceptionCallback callback)
{
    // TODO
}

void CAIPPullData()
{
    CAArduinoCheckData();
}

/// Retrieves the IP address assigned to Arduino Ethernet shield
CAResult_t CAArduinoGetInterfaceAddress(char *address, int32_t addrLen)
{
    OIC_LOG(DEBUG, TAG, "IN");
    VERIFY_NON_NULL(address, TAG, "address");

    //TODO : Fix this for scenarios when this API is invoked when device is not connected
    uint8_t rawIPAddr[4];
    if (addrLen < IPNAMESIZE)
    {
        OIC_LOG(ERROR, TAG, "Invalid addrLen");
        return CA_STATUS_FAILED;
    }

    W5100.getIPAddress(rawIPAddr);
    snprintf(address, sizeof(address), "%d.%d.%d.%d", rawIPAddr[0], rawIPAddr[1], rawIPAddr[2],
             rawIPAddr[3]);

    OIC_LOG_V(DEBUG, TAG, "address:%s", address);
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

