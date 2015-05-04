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
#include "caadapterinterface.h"
#include "caipadapter_singlethread.h"
#include "caipadapterutils_eth.h"
#include "caadapterutils.h"
#include "oic_malloc.h"

#define TAG "IPC"

static int g_sockID = 0;

/**
 * @var g_unicastPort
 * @brief Unicast Port
 */
static uint16_t g_unicastPort = 0;

void CAIPSetUnicastSocket(int socketID)
{
    OIC_LOG(DEBUG, TAG, "IN");
    if (0 < socketID)
    {
        g_sockID = socketID;
    }
    else
    {
        OIC_LOG(ERROR, TAG, "sock err");
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return;
}

void CAIPSetUnicastPort(uint16_t port)
{
    OIC_LOG(DEBUG, TAG, "IN");
    g_unicastPort = port;
    OIC_LOG(DEBUG, TAG, "OUT");
    return;
}

uint32_t CAIPSendData(const char *remoteAddress, uint16_t port,
                      const char *buf, uint32_t bufLen, bool isMulticast)
{
    if (!isMulticast && 0 == g_unicastPort)
    {
        OIC_LOG(ERROR, TAG, "port 0");
        return 0;
    }

    VERIFY_NON_NULL(buf, TAG, "buf");
    VERIFY_NON_NULL(remoteAddress, TAG, "address");

    int socketID = 0;
    if (isMulticast)
    {
        if (CAArduinoInitMulticastUdpSocket(remoteAddress, port, g_unicastPort, &socketID)
            != CA_STATUS_OK)
        {
            OIC_LOG(ERROR, TAG, "init mcast err");
            return 0;
        }
        OIC_LOG_V(DEBUG, TAG, "MPORT:%d", port);
        OIC_LOG_V(DEBUG, TAG, "LPORT:%d", g_unicastPort);
        OIC_LOG_V(DEBUG, TAG, "SOCKET ID:%d", socketID);
    }
    else
    {
        if (0 == g_sockID)
        {
            if (CAArduinoInitUdpSocket(&port, &socketID) != CA_STATUS_OK)
            {
                OIC_LOG(ERROR, TAG, "init ucast err");
                return 0;
            }
        }
        else
        {
            socketID = g_sockID;
        }
    }

    uint32_t ret;
    uint8_t ipAddr[4] = { 0 };
    uint16_t parsedPort = 0;
    if (CAParseIPv4AddressInternal(remoteAddress, ipAddr, sizeof(ipAddr),
                                   &parsedPort) != CA_STATUS_OK)
    {
        OIC_LOG(ERROR, TAG, "parse fail");
        return 0;
    }

    if (bufLen > 65535) // Max value for uint16_t
    {
        // This will never happen as max buffer size we are dealing with is COAP_MAX_PDU_SIZE
        OIC_LOG(ERROR, TAG, "Size exceeded");
        return 0;
    }

    ret = sendto(socketID, (const uint8_t *)buf, (uint16_t)bufLen, ipAddr, port);
    if (g_sockID != socketID)
    {
        close(socketID);
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return ret;
}

