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
#include "cawifiinterface_singlethread.h"

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <SPI.h>
#include <utility/server_drv.h>
#include <utility/wifi_drv.h>
#include <IPAddress.h>
#include <TimedAction.h>

#include "logger.h"
#include "cacommon.h"
#include "caadapterinterface.h"
#include "cawifiadapter_singlethread.h"
#include "caadapterutils.h"

/// This is the max buffer size between Arduino and WiFi Shield
#define ARDUINO_WIFI_SPI_RECV_BUFFERSIZE (64)

#define MOD_NAME "WC"

static int16_t CAParseIPv4AddressLocal(unsigned char *ipAddrStr, uint8_t *ipAddr, uint16_t *port);

static int32_t gSockID = 0;

static WiFiUDP Udp;

void CAWiFiSetUnicastSocket(const int32_t socketID)
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

uint32_t CAWiFiSendData(const char *remoteAddress, const uint32_t port,
                        const void *data, const uint32_t dataLength, bool isMulticast)
{
    OIC_LOG(DEBUG, MOD_NAME, "IN");

    uint32_t ip;
    uint16_t rem, send;

    VERIFY_NON_NULL(data, MOD_NAME, "data");
    VERIFY_NON_NULL(remoteAddress, MOD_NAME, "address");

    int32_t ret = 1;
    OIC_LOG_V(DEBUG, MOD_NAME, "remoteip: %s", remoteAddress);
    OIC_LOG_V(DEBUG, MOD_NAME, "sending to port: %d", port);
    Udp.beginPacket(remoteAddress, port);
    ret = (int32_t)Udp.write((char *)data);
    Udp.endPacket();
    OIC_LOG(DEBUG, MOD_NAME, "OUT");
    return ret;
}

int16_t CAParseIPv4AddressLocal(unsigned char *ipAddrStr, uint8_t *ipAddr, uint16_t *port)
{
    size_t index = 0;
    unsigned char *itr;
    uint8_t dotCount = 0;

    ipAddr[index] = 0;
    *port = 0;
    itr = ipAddrStr;
    if (!isdigit((unsigned char) *ipAddrStr))
    {
        return -1;
    }
    ipAddrStr = itr;

    while (*ipAddrStr)
    {
        if (isdigit((unsigned char) *ipAddrStr))
        {
            ipAddr[index] *= 10;
            ipAddr[index] += *ipAddrStr - '0';
        }
        else if ((unsigned char) *ipAddrStr == '.')
        {
            index++;
            dotCount++;
            ipAddr[index] = 0;
        }
        else
        {
            break;
        }
        ipAddrStr++;
    }
    if (*ipAddrStr == ':')
    {
        ipAddrStr++;
        while (*ipAddrStr)
        {
            if (isdigit((unsigned char) *ipAddrStr))
            {
                *port *= 10;
                *port += *ipAddrStr - '0';
            }
            else
            {
                break;
            }
            ipAddrStr++;
        }
    }
    if (ipAddr[0] < 255 && ipAddr[1] < 255 && ipAddr[2] < 255 && ipAddr[3] < 255
        && dotCount == 3)
    {
        return 1;
    }
    return 0;
}

