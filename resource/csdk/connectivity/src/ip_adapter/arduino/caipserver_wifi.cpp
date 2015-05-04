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
#include <WiFi.h>
#include <WiFiUdp.h>
#include <SPI.h>
#include <utility/server_drv.h>
#include <utility/wifi_drv.h>
#include <IPAddress.h>

#include "logger.h"
#include "cacommon.h"
#include "cainterface.h"
#include "caadapterinterface.h"
#include "caipadapter_singlethread.h"
#include "caadapterutils.h"
#include "oic_malloc.h"

#define TAG "IPS"

// Length of the IP address decimal notation string
#define IPNAMESIZE (16)

// Start offsets based on end of received data buffer
#define IP_RECBUF_IPADDR_OFFSET  (6)
#define IP_RECBUF_PORT_OFFSET    (2)

#define IP_RECBUF_IPADDR_SIZE    (IP_RECBUF_IPADDR_OFFSET - IP_RECBUF_PORT_OFFSET)
#define IP_RECBUF_PORT_SIZE      (IP_RECBUF_PORT_OFFSET - 0)
#define IP_RECBUF_FOOTER_SIZE    (IP_RECBUF_IPADDR_SIZE + IP_RECBUF_PORT_SIZE)

static CAResult_t CAArduinoGetInterfaceAddress(char *address, int32_t addrLen);
static void CAArduinoCheckData();
static void CAPacketReceivedCallback(const char *ipAddress, const uint16_t port,
                                     const void *data, const uint32_t dataLength);

static CAIPPacketReceivedCallback gPacketReceivedCallback = NULL;
static int32_t gUnicastSocket = 0;
static bool gServerRunning = false;
static WiFiUDP Udp;

CAResult_t CAIPInitializeServer(void)
{
    /**
     * This API is to keep design in sync with other platforms.
     * The required implementation is done in Start() api's.
     */
    return CA_STATUS_OK;
}

void CAIPTerminateServer(void)
{
    /**
     * This API is to keep design in sync with other platforms.
     * The required implementation is done in Stop() api's.
     */
}

CAResult_t CAIPGetUnicastServerInfo(char **ipAddress, uint16_t *port, int *serverID)
{
    /*
     * This API is to keep design in sync with other platforms.
     * Will be implemented as and when CA layer wants this info.
     */
    return CA_STATUS_OK;
}

CAResult_t CAIPStartUnicastServer(const char *localAddress, uint16_t *port,
                                  bool forceStart, int *serverFD)
{
    OIC_LOG(DEBUG, TAG, "IN");
    VERIFY_NON_NULL(port, TAG, "port");

    if (gServerRunning)
    {
        // already running
        OIC_LOG(DEBUG, TAG, "Error");
        return CA_STATUS_FAILED;
    }

    if (WiFi.status() != WL_CONNECTED)
    {
        OIC_LOG(ERROR, TAG, "ERROR:No WIFI");
        return CA_STATUS_FAILED;
    }

    char localIpAddress[CA_IPADDR_SIZE];
    int32_t localIpAddressLen = sizeof(localIpAddress);
    CAArduinoGetInterfaceAddress(localIpAddress, localIpAddressLen);
    OIC_LOG_V(DEBUG, TAG, "address: %s", localIpAddress);
    OIC_LOG_V(DEBUG, TAG, "port: %d", *port);

    Udp.begin((uint16_t ) *port);
    gServerRunning = true;

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAIPStartMulticastServer(const char *localAddress, const char *multicastAddress,
                                      uint16_t multicastPort, int *serverFD)
{
    // wifi shield does not support multicast
    OIC_LOG(DEBUG, TAG, "IN");
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_NOT_SUPPORTED;
}

CAResult_t CAIPStopUnicastServer()
{
    OIC_LOG(DEBUG, TAG, "IN");
    Udp.stop();

    gServerRunning = false;
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAIPStopMulticastServer()
{
    return CAIPStopUnicastServer();
}

void CAPacketReceivedCallback(const char *ipAddress, const uint16_t port,
                              const void *data, const uint32_t dataLength)
{
    OIC_LOG(DEBUG, TAG, "IN");
    if (gPacketReceivedCallback)
    {
        gPacketReceivedCallback(ipAddress, port, data, dataLength);
        OIC_LOG(DEBUG, TAG, "Notified network packet");
    }
    OIC_LOG(DEBUG, TAG, "OUT");
}

void CAArduinoCheckData()
{
    OIC_LOG(DEBUG, TAG, "IN");
    char addr[IPNAMESIZE] = {0};
    uint16_t senderPort = 0;
    int16_t packetSize = Udp.parsePacket();
    OIC_LOG_V(DEBUG, TAG, "Rcv packet of size:%d ", packetSize);
    if (packetSize)
    {
        packetSize = packetSize > COAP_MAX_PDU_SIZE ? COAP_MAX_PDU_SIZE:packetSize;
        char *data = (char *)OICMalloc(packetSize + 1);
        if (NULL == data)
        {
            return;
        }
        IPAddress remoteIp = Udp.remoteIP();
        senderPort = Udp.remotePort();
        sprintf(addr, "%d.%d.%d.%d", remoteIp[0], remoteIp[1], remoteIp[2], remoteIp[3]);
        OIC_LOG_V(DEBUG, TAG, "remoteip: %s, port: %d", addr, senderPort);
        // read the packet into packetBufffer
        int32_t dataLen = Udp.read(data, COAP_MAX_PDU_SIZE);
        if (dataLen > 0)
        {
            data[dataLen] = 0;
        }
        CAPacketReceivedCallback(addr, senderPort, data, dataLen);
        OICFree(data);
    }
    OIC_LOG(DEBUG, TAG, "OUT");
}

void CAIPSetPacketReceiveCallback(CAIPPacketReceivedCallback callback)
{
    OIC_LOG(DEBUG, TAG, "IN");
    gPacketReceivedCallback = callback;
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

/// Retrieves the IP address assigned to Arduino WiFi shield
CAResult_t CAArduinoGetInterfaceAddress(char *address, int32_t addrLen)
{
    OIC_LOG(DEBUG, TAG, "IN");
    if (WiFi.status() != WL_CONNECTED)
    {
        OIC_LOG(DEBUG, TAG, "No WIFI");
        return CA_STATUS_FAILED;
    }

    VERIFY_NON_NULL(address, TAG, "Invalid address");
    if (addrLen < IPNAMESIZE)
    {
        OIC_LOG_V(ERROR, TAG, "AddrLen MUST be atleast %d", IPNAMESIZE);
        return CA_STATUS_FAILED;
    }

    IPAddress ip = WiFi.localIP();
    sprintf((char *)address, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

    OIC_LOG_V(DEBUG, TAG, "Wifi shield address is: %s", address);
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}



