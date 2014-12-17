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

#include "caethernetinterface_singlethread.h"

#include <Arduino.h>
#include <Ethernet.h>
#include <socket.h>
#include <w5100.h>
#include <EthernetUdp.h>
#include <IPAddress.h>
#include <TimedAction.h>

#include "logger.h"
#include "cacommon.h"
#include "cainterface.h"
#include "caadapterinterface.h"
#include "caethernetadapter_singlethread.h"
#include "caethernetadapterutils.h"
#include "caadapterutils.h"
#include "oic_malloc.h"

#define COAP_MAX_PDU_SIZE 320
#define MOD_NAME "ES"

// Length of the IP address decimal notation string
#define IPNAMESIZE (16)

CAResult_t CAEthernetStartUnicastServer(const char *localAddress, int16_t *port,
                                        const bool forceStart, int32_t *serverFD);
static int32_t CAArduinoRecvData(int32_t sockFd, uint8_t *buf, uint32_t bufLen,
                                 uint8_t *senderAddr, uint16_t *senderPort);
static CAResult_t CAArduinoGetInterfaceAddress(char *address, int32_t addrLen);
static void CAArduinoCheckData();
void CAPacketReceivedCallback(const char *ipAddress, const uint32_t port,
                              const void *data, const uint32_t dataLength);

static CAEthernetPacketReceivedCallback gPacketReceivedCallback = NULL;
static int32_t gUnicastSocket = 0;
static int32_t gMulticastSocket = 0;
static bool gServerRunning = false;
static TimedAction gRcvAction = TimedAction(3000, CAArduinoCheckData);

/**
 * @var gUnicastPort
 * @brief Unicast Port
 */
int16_t gUnicastPort = 0;

CAResult_t CAEthernetInitializeServer(void)
{
    return CA_STATUS_OK;
}

void CAEthernetTerminateServer(void)
{
    return;
}

CAResult_t CAEthernetGetUnicastServerInfo(char **ipAddress, int *port, int32_t *serverID)
{
    return CA_STATUS_OK;
}

CAResult_t CAEthernetStartUnicastServer(const char *localAddress, int16_t *port,
                                        const bool forceStart, int32_t *serverFD)
{
    OIC_LOG(DEBUG, MOD_NAME, "IN");
    VERIFY_NON_NULL(port, MOD_NAME, "port");

    // Note: ****Update the MAC address here with your shield's MAC address****
    uint8_t ETHERNET_MAC[] = {0x90, 0xA2, 0xDA, 0x0E, 0xC4, 0x05};
    uint8_t error = Ethernet.begin(ETHERNET_MAC);
    if (error  == 0)
    {
        OIC_LOG_V(ERROR, MOD_NAME, "FAILED:%d", error);
        return CA_STATUS_FAILED;
    }
    OIC_LOG(DEBUG, MOD_NAME, "success");

    uint8_t rawIPAddr[4];
    char address[16];
    W5100.getIPAddress(rawIPAddr);
    sprintf(address, "%d.%d.%d.%d", rawIPAddr[0], rawIPAddr[1], rawIPAddr[2], rawIPAddr[3]);
    OIC_LOG_V(DEBUG, MOD_NAME, "address:%s", address);

    //if (CAArduinoInitUdpSocket(port, serverFD) != CA_STATUS_OK)
    if (CAArduinoInitUdpSocket(port, serverFD) != CA_STATUS_OK)
    {
        OIC_LOG(DEBUG, MOD_NAME, "failed");
        return CA_STATUS_FAILED;
    }

    gUnicastPort = *port;
    gUnicastSocket = *serverFD;
    OIC_LOG_V(DEBUG, MOD_NAME, "gUnicastPort: %d", gUnicastPort);
    OIC_LOG_V(DEBUG, MOD_NAME, "gUnicastSocket: %d", gUnicastSocket);

    // start thread to monitor socket here
    if (!gServerRunning)
    {
        gRcvAction.enable();
        gServerRunning = true;
    }
    OIC_LOG(DEBUG, MOD_NAME, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAEthernetStartMulticastServer(const char *localAddress, const char *multicastAddress,
                                          const int16_t multicastPort, int32_t *serverFD)
{
    OIC_LOG(DEBUG, MOD_NAME, "IN");
    if (CAArduinoInitMulticastUdpSocket(multicastAddress, &multicastPort, &multicastPort,
                                        serverFD) != CA_STATUS_OK)
    {
        OIC_LOG(DEBUG, MOD_NAME, "failed");
        return CA_STATUS_FAILED;
    }

    gMulticastSocket = *serverFD;
    OIC_LOG_V(DEBUG, MOD_NAME, "gMulticastPort: %d", multicastPort);
    OIC_LOG_V(DEBUG, MOD_NAME, "gMulticastSocket: %d", gMulticastSocket);
    // start thread to monitor socket here
    if (!gServerRunning)
    {
        gRcvAction.enable();
        gServerRunning = true;
    }

    OIC_LOG(DEBUG, MOD_NAME, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAEthernetStopUnicastServer()
{
    OIC_LOG(DEBUG, MOD_NAME, "IN");
    // terminate server thread
    // Stop thread if both server stopped
    if (gMulticastSocket == 0)
    {
        gRcvAction.disable();
        gServerRunning = false;
    }

    close(gUnicastSocket);
    gUnicastSocket = 0;
    OIC_LOG(DEBUG, MOD_NAME, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAEthernetStopMulticastServer()
{
    OIC_LOG(DEBUG, MOD_NAME, "IN");
    // terminate server thread
    // Stop thread if both server stopped
    if (gUnicastSocket == 0)
    {
        gRcvAction.disable();
        gServerRunning = false;
    }

    close(gMulticastSocket);
    gMulticastSocket = 0;
    OIC_LOG(DEBUG, MOD_NAME, "OUT");
    return CA_STATUS_OK;
}

void CAPacketReceivedCallback(const char *ipAddress, const uint32_t port,
                              const void *data, const uint32_t dataLength)
{
    OIC_LOG(DEBUG, MOD_NAME, "IN");
    if (gPacketReceivedCallback)
    {
        gPacketReceivedCallback(ipAddress, port, data, dataLength);
        OIC_LOG(DEBUG, MOD_NAME, "NNP");
    }
    OIC_LOG(DEBUG, MOD_NAME, "OUT");
}

void CAArduinoCheckData()
{
    void *data = malloc(COAP_MAX_PDU_SIZE);
    if (NULL == data)
    {
        return;
    }
    memset(data, 0, COAP_MAX_PDU_SIZE);
    int32_t dataLen = 0;
    uint8_t senderAddr[4] = { 0 };
    char addr[IPNAMESIZE] = {0};
    uint16_t senderPort = 0;

    if (gUnicastSocket)
    {
        dataLen = CAArduinoRecvData(gUnicastSocket, (uint8_t *)data, COAP_MAX_PDU_SIZE, senderAddr,
                                    &senderPort);
        if (dataLen < 0)
        {
            OIC_LOG(ERROR, MOD_NAME, "FAILED");
            CAEthernetStopUnicastServer();
        }
        else if (dataLen > 0)
        {
            OIC_LOG(DEBUG, MOD_NAME, "Data recvd on unicast server");
            sprintf(addr, "%d.%d.%d.%d", senderAddr[0], senderAddr[1], senderAddr[2], senderAddr[3]);
            CAPacketReceivedCallback(addr, senderPort, data, dataLen);
        }
        else
        {
            OIC_LOG(DEBUG, MOD_NAME, "No data");
        }
    }

    if (gMulticastSocket)
    {
        dataLen = CAArduinoRecvData(gMulticastSocket, (uint8_t *)data, COAP_MAX_PDU_SIZE, senderAddr,
                                    &senderPort);
        if (dataLen < 0)
        {
            CAEthernetStopMulticastServer();
        }
        else if (dataLen > 0)
        {
            OIC_LOG(DEBUG, MOD_NAME, "multicast data recvd");
            sprintf(addr, "%d.%d.%d.%d", senderAddr[0], senderAddr[1], senderAddr[2], senderAddr[3]);
            CAPacketReceivedCallback(addr, senderPort, data, dataLen);
        }
    }
    OICFree(data);
}

/// Retrieve any available data from UDP socket. This is a non-blocking call.
int32_t CAArduinoRecvData(int32_t sockFd, uint8_t *buf, uint32_t bufLen, uint8_t *senderAddr,
                          uint16_t *senderPort)
{
    OIC_LOG(DEBUG, MOD_NAME, "IN");
    /**Bug : When there are multiple UDP packets in Wiznet buffer, W5100.getRXReceivedSize
     * will not return correct length of the first packet.
     * Fix : Use the patch provided for arduino/libraries/Ethernet/utility/socket.cpp
     */

    VERIFY_NON_NULL(buf, MOD_NAME, "Invalid buf");
    VERIFY_NON_NULL(senderAddr, MOD_NAME, "Invalid senderAddr");
    VERIFY_NON_NULL(senderPort, MOD_NAME, "Invalid senderPort");
    int32_t ret = 0;
    uint16_t recvLen = W5100.getRXReceivedSize(sockFd);
    if (recvLen == 0)
    {
        OIC_LOG_V(DEBUG, MOD_NAME, "rcvd %d", recvLen);
        return recvLen;
    }

    // Read available data.
    ret = recvfrom(sockFd, buf, bufLen, senderAddr, senderPort);
    OIC_LOG(DEBUG, MOD_NAME, "OUT");
    return ret;
}

void CAEthernetSetPacketReceiveCallback(CAEthernetPacketReceivedCallback callback)
{
    OIC_LOG(DEBUG, MOD_NAME, "IN");
    gPacketReceivedCallback = callback;
    OIC_LOG(DEBUG, MOD_NAME, "OUT");
}

void CAEthernetSetExceptionCallback(CAEthernetExceptionCallback callback)
{
    // TODO
}

void CAEthernetPullData()
{
    gRcvAction.check();
}

/// Retrieves the IP address assigned to Arduino Ethernet shield
CAResult_t CAArduinoGetInterfaceAddress(char *address, int32_t addrLen)
{
    OIC_LOG(DEBUG, MOD_NAME, "IN");
    VERIFY_NON_NULL(address, MOD_NAME, "address");

    //TODO : Fix this for scenarios when this API is invoked when device is not connected
    uint8_t rawIPAddr[4];
    if (addrLen < IPNAMESIZE)
    {
        OIC_LOG(ERROR, MOD_NAME, "Invalid addrLen");
        return CA_STATUS_FAILED;
    }

    W5100.getIPAddress(rawIPAddr);
    sprintf(address, "%d.%d.%d.%d", rawIPAddr[0], rawIPAddr[1], rawIPAddr[2], rawIPAddr[3]);

    OIC_LOG_V(DEBUG, MOD_NAME, "address:%s", address);
    OIC_LOG(DEBUG, MOD_NAME, "OUT");
    return CA_STATUS_OK;
}
