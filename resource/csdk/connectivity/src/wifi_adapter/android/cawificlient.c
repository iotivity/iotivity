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
#include "cawifiinterface.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

#include "caadapterutils.h"

/**
 * @def WIFI_CLIENT_TAG
 * @brief Logging tag for module name
 */
#define WIFI_CLIENT_TAG "WIFI_CLIENT"

/**
 * @var gUnicastServerSocketDescClient
 * @brief socket descriptor for unicast server
 */
static int gUnicastServerSocketDescClient = -1;

void CAWiFiSetUnicastSocket(const int32_t socketFD)
{
    OIC_LOG(DEBUG, WIFI_CLIENT_TAG, "IN");

    gUnicastServerSocketDescClient = socketFD;
}

uint32_t CAWiFiSendData(const char *remoteAddress, const uint32_t port,
                        const void *data, const uint32_t dataLength, bool isMulticast)
{
    OIC_LOG(DEBUG, WIFI_CLIENT_TAG, "IN");

    VERIFY_NON_NULL_RET(remoteAddress, WIFI_CLIENT_TAG, "IP address is NULL", 0);
    VERIFY_NON_NULL_RET(data, WIFI_CLIENT_TAG, "data is NULL", 0);

    if (dataLength == 0)
    {
        OIC_LOG(ERROR, WIFI_CLIENT_TAG, "Data length is 0 !");
        return 0;
    }

    if (0 > gUnicastServerSocketDescClient)
    {
        OIC_LOG(ERROR, WIFI_CLIENT_TAG, "Unicast Server is not running !");
        return 0;
    }

    struct sockaddr_in destAddr;
    memset((char *)&destAddr, 0, sizeof(destAddr));
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(port);

    // Conversion from ASCII format to Network format
    if (inet_aton(remoteAddress, &destAddr.sin_addr) == 0)
    {
        OIC_LOG(ERROR, WIFI_CLIENT_TAG, "Failed to convert from ASCII to Network Address");
        return 0;
    }

    int32_t sendDataLength = sendto(gUnicastServerSocketDescClient, data, dataLength, 0,
                                    (struct sockaddr *)&destAddr, sizeof(destAddr));
    if (sendDataLength == -1)
    {
        OIC_LOG_V(ERROR, WIFI_CLIENT_TAG, "Failed to Send Data, Error code: %s", strerror(errno));
        return 0;
    }

    OIC_LOG_V(INFO, WIFI_CLIENT_TAG, "Sending data is successful, sent bytes[%d]", sendDataLength);

    OIC_LOG(DEBUG, WIFI_CLIENT_TAG, "OUT");
    return sendDataLength;
}

