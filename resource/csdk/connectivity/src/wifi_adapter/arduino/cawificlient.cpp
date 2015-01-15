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

#include "logger.h"
#include "cacommon.h"
#include "caadapterinterface.h"
#include "cawifiadapter_singlethread.h"
#include "caadapterutils.h"

/// This is the max buffer size between Arduino and WiFi Shield
#define ARDUINO_WIFI_SPI_RECV_BUFFERSIZE (64)

#define MOD_NAME "WC"

static WiFiUDP Udp;

void CAWiFiSetUnicastSocket(const int32_t socketID)
{

}

uint32_t CAWiFiSendData(const char *remoteAddress, const uint32_t port,
                        const void *data, const uint32_t dataLength, bool isMulticast)
{
    OIC_LOG(DEBUG, MOD_NAME, "IN");

    VERIFY_NON_NULL(data, MOD_NAME, "data");
    VERIFY_NON_NULL(remoteAddress, MOD_NAME, "address");

    int32_t ret = 1;
    OIC_LOG_V(DEBUG, MOD_NAME, "remoteip: %s", remoteAddress);
    OIC_LOG_V(DEBUG, MOD_NAME, "port: %d", port);
    Udp.beginPacket(remoteAddress, (uint16_t)port);
    ret = (int32_t)Udp.write((char *)data);
    Udp.endPacket();
    OIC_LOG(DEBUG, MOD_NAME, "OUT");
    return ret;
}
