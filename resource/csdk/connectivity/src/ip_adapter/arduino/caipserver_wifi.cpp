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

#include "caipinterface.h"

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
#include "caipadapter.h"
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

static void CAArduinoCheckData();
static void CAPacketReceivedCallback(const char *ipAddress, const uint16_t port,
                                     const void *data, const uint32_t dataLength);

static CAIPPacketReceivedCallback gPacketReceivedCallback = NULL;
static int32_t gUnicastSocket = 0;
static bool gServerRunning = false;
static WiFiUDP Udp;
/**
 * @var g_unicastPort
 * @brief Unicast Port
 */
static uint16_t g_unicastPort = 0;

CAResult_t CAIPInitializeServer(const ca_thread_pool_t threadPool)
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

uint16_t CAGetServerPortNum(const char *ipAddress, bool isSecured)
{
    return g_unicastPort;
}

CAResult_t CAIPStartUnicastServer(const char *localAddress, uint16_t *port,
                                  bool secured)
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

    OIC_LOG_V(DEBUG, TAG, "port: %d", *port);

    Udp.begin((uint16_t ) *port);
    gServerRunning = true;
    g_unicastPort = *port;
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAIPStartMulticastServer(const char *localAddress, const char *multicastAddress,
                                      uint16_t multicastPort)
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

CAResult_t CAIPStopServer(const char *interfaceAddress)
{
    /* For arduino, Server will be running in only one interface */
    return CAIPStopAllServers();
}

CAResult_t CAIPStopAllServers()
{
    OIC_LOG(DEBUG, TAG, "IN");
    CAResult_t result = CAIPStopUnicastServer();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, TAG, "stop ucast srv fail:%d", result);
        return result;
    }
    CAIPSetUnicastSocket(-1);
    CAIPSetUnicastPort(0);

    result = CAIPStopMulticastServer();
    if (CA_STATUS_OK != result)
    {
        OIC_LOG_V(ERROR, TAG, "stop mcast srv fail:%d", result);
    }
    OIC_LOG(DEBUG, TAG, "OUT");
    return result;
}

void CAPacketReceivedCallback(const char *ipAddress, const uint16_t port,
                              const void *data, const uint32_t dataLength)
{
    OIC_LOG(DEBUG, TAG, "IN");
    if (gPacketReceivedCallback)
    {
        CAEndpoint_t ep;
        strncpy(ep.addr, ipAddress, MAX_ADDR_STR_SIZE_CA);
        ep.port = port;
        ep.flags = CA_IPV4;
        ep.adapter = CA_ADAPTER_IP;
        gPacketReceivedCallback(&ep, data, dataLength);
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

void CAIPSetErrorHandleCallback(CAIPErrorHandleCallback ipErrorCallback)
{
    OIC_LOG(DEBUG, TAG, "IN");
    OIC_LOG(DEBUG, TAG, "OUT");
}

void CAIPPullData()
{
    CAArduinoCheckData();
}
