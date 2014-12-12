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
#include "caethernetinterface.h"

#include <Arduino.h>
#include <Ethernet.h>
#include <socket.h>
#include <w5100.h>
#include <EthernetUdp.h>
#include <IPAddress.h>
#include <TimedAction.h>

#include "logger.h"
#include "cacommon.h"
#include "caadapterinterface.h"
#include "caethernetadapter.h"
#include "caethernetadapterutils.h"
#include "caadapterutils.h"
#include "oic_malloc.h"

/// This is the max buffer size between Arduino and WiFi Shield
#define ARDUINO_ETHERNET_SPI_RECV_BUFFERSIZE (64)

#define MOD_NAME "EC"

static int32_t gSockID = 0;

/**
 * @var gUnicastPort
 * @brief Unicast Port
 */
static int16_t gUnicastPort = 0;

void CAEthernetSetUnicastSocket(const int32_t socketID)
{
    OIC_LOG(DEBUG, MOD_NAME, "IN");
    if (0 < socketID)
    {
        gSockID = socketID;
        return;
    }

    OIC_LOG(DEBUG, MOD_NAME, "OUT");
    return;
}

void CAEthernetSetUnicastPort(const int32_t port)
{
    OIC_LOG(DEBUG, MOD_NAME, "IN");
    if (0 < port)
    {
        gUnicastPort = port;
        return;
    }

    OIC_LOG(DEBUG, MOD_NAME, "OUT");
    return;
}

uint32_t CAEthernetSendData(const char *remoteAddress, const int16_t port,
                            const char *buf, const uint32_t bufLen, bool isMulticast)
{
    if (!isMulticast && 0 == gUnicastPort)
    {
        OIC_LOG(ERROR, MOD_NAME, "Failed");
        return 0;
    }

    int32_t socketID = 0;
    if (isMulticast)
    {
        if (CAArduinoInitMulticastUdpSocket(remoteAddress, &port, &gUnicastPort, &socketID) != CA_STATUS_OK)
        {
            OIC_LOG(ERROR, MOD_NAME, "multicast");
            return 0;
        }
        OIC_LOG_V(DEBUG, MOD_NAME, "MPORT:%d", port);
        OIC_LOG_V(DEBUG, MOD_NAME, "LPORT:%d", gUnicastPort);
        OIC_LOG_V(DEBUG, MOD_NAME, "SOCKET ID:%d", socketID);
    }
    else
    {
        if (0 == gSockID)
        {
            if (CAArduinoInitUdpSocket((int16_t *)&port, &socketID) != CA_STATUS_OK)
            {
                OIC_LOG(ERROR, MOD_NAME, "unicast");
                return 0;
            }
        }
        else
        {
            socketID = gSockID;
        }
    }

    uint32_t ret;
    VERIFY_NON_NULL(buf, MOD_NAME, "buf");
    VERIFY_NON_NULL(remoteAddress, MOD_NAME, "address");

    uint8_t ipAddr[4] = { 0 };
    uint16_t parsedPort = 0;
    if (!CAParseIPv4AddressLocal((unsigned char *) remoteAddress, ipAddr, &parsedPort))
    {
        OIC_LOG(ERROR, MOD_NAME, "failed");
        return 0;
    }

    ret = sendto(socketID, (const uint8_t *)buf, (uint16_t)bufLen, (uint8_t *)ipAddr, port);
    delay(10);
    if (gSockID != socketID)
    {
        close(socketID);
    }

    OIC_LOG(DEBUG, MOD_NAME, "OUT");
    return ret;
}
