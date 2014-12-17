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

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

#include "caadapterutils.h"

/**
 * @def ETHERNET_CLIENT_TAG
 * @brief Logging tag for module name
 */
#define ETHERNET_CLIENT_TAG "ETHERNET_CLIENT"

/**
 * @var gUnicastServerSocketDescClient
 * @brief socket descriptor for unicast server
 */
static int32_t gUnicastServerSocketDescClient = -1;

#ifdef __WITH_DTLS__
/**
 * @var gUnicastServerSocketDescClient
 * @brief socket descriptor for secure unicast server
 */
static int32_t gUnicastServerSecureSocketDescClient = -1;
#endif

static uint32_t CASendData(const char *remoteAddress, const uint32_t port,
                           const void *data, const uint32_t dataLength, int32_t sockfd)
{
    OIC_LOG(DEBUG, ETHERNET_CLIENT_TAG, "IN");

    VERIFY_NON_NULL_RET(remoteAddress, ETHERNET_CLIENT_TAG, "IP address is NULL", 0);
    VERIFY_NON_NULL_RET(data, ETHERNET_CLIENT_TAG, "data is NULL", 0);

    if (0 == dataLength)
    {
        OIC_LOG(ERROR, ETHERNET_CLIENT_TAG, "Data length is 0 !");
        return 0;
    }

    if (0 > sockfd)
    {
        OIC_LOG(ERROR, ETHERNET_CLIENT_TAG, "Unicast Server is not running !");
        return 0;
    }

    struct sockaddr_in destAddr;
    memset((char *)&destAddr, 0, sizeof(destAddr));
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(port);

    // Conversion from ASCII format to Network format
    if (0 == inet_aton(remoteAddress, &destAddr.sin_addr))
    {
        OIC_LOG(ERROR, ETHERNET_CLIENT_TAG, "Failed to convert from ASCII to Network Address");
        return 0;
    }

    int32_t sendDataLength = sendto(sockfd, data, dataLength, 0,
                                    (struct sockaddr *)&destAddr, sizeof(destAddr));
    if (-1 == sendDataLength)
    {
        OIC_LOG_V(ERROR, ETHERNET_CLIENT_TAG, "Failed to Send Data, Error code: %s", strerror(errno));
        return 0;
    }

    OIC_LOG_V(INFO, ETHERNET_CLIENT_TAG, "Sending data is successful, sent bytes[%d]", sendDataLength);
    return sendDataLength;
}

void CAEthernetSetUnicastSocket(const int32_t socketFD)
{
    OIC_LOG(DEBUG, ETHERNET_CLIENT_TAG, "IN");

    gUnicastServerSocketDescClient = socketFD;
}

#ifdef __WITH_DTLS__
void CAEthernetSetSecureUnicastSocket(const int32_t socketFD)
{
    OIC_LOG(DEBUG, ETHERNET_CLIENT_TAG, "IN");

    gUnicastServerSecureSocketDescClient = socketFD;
}
#endif

uint32_t CAEthernetSendData(const char *remoteAddress, const uint32_t port,
                            const void *data, const uint32_t dataLength,
                            CABool_t isMulticast, CABool_t isSecured)
{
    uint32_t len = 0;

#ifdef __WITH_DTLS__
    if (CA_TRUE == isSecured)
    {
        len  = CASendData(remoteAddress, port,
                          data, dataLength, gUnicastServerSecureSocketDescClient);
    }
    else
    {
#endif
        len =  CASendData(remoteAddress, port,
                          data, dataLength, gUnicastServerSocketDescClient);
#ifdef __WITH_DTLS__
    }
#endif
    return len;
}
