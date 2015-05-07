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

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

#include "caadapterutils.h"

/**
 * @def IP_CLIENT_TAG
 * @brief Logging tag for module name
 */
#define IP_CLIENT_TAG "IP_CLIENT"
#define OC_MULTICAST_IP "224.0.1.187"

static uint32_t CASendData(const char *remoteAddress, uint16_t port, const void *data,
                           uint32_t dataLength, int sockfd)
{
    OIC_LOG(DEBUG, IP_CLIENT_TAG, "IN");

    VERIFY_NON_NULL_RET(remoteAddress, IP_CLIENT_TAG, "IP address is NULL", 0);
    VERIFY_NON_NULL_RET(data, IP_CLIENT_TAG, "data is NULL", 0);

    if (0 == dataLength)
    {
        OIC_LOG(ERROR, IP_CLIENT_TAG, "Data length is 0!");
        return 0;
    }

    if (0 > sockfd)
    {
        OIC_LOG(ERROR, IP_CLIENT_TAG, "Unicast Server is not running!");
        return 0;
    }

    struct sockaddr_in destAddr = { 0 };
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(port);

    int ret = inet_pton(AF_INET, remoteAddress, &(destAddr.sin_addr));
    if (1 != ret)
    {
        OIC_LOG(ERROR, IP_CLIENT_TAG, "inet_pton failed!");
        return 0;
    }

    ssize_t sendDataLength = sendto(sockfd, data, dataLength, 0, (struct sockaddr *) &destAddr,
                                    sizeof(destAddr));
    if (-1 == sendDataLength)
    {
        OIC_LOG_V(ERROR, IP_CLIENT_TAG, "sendto failed, Error code: %s",
                  strerror(errno));
        return 0;
    }

    OIC_LOG_V(INFO, IP_CLIENT_TAG, "Sending data is successful, sent bytes[%d] to ip[%s:%d]",
              sendDataLength, remoteAddress, port);
    OIC_LOG(DEBUG, IP_CLIENT_TAG, "OUT");
    return sendDataLength;
}

uint32_t CAIPSendData(const char *remoteAddress, uint16_t remotePort, const void *data,
                            uint32_t dataLength, bool isMulticast, bool isSecured)
{
    u_arraylist_t *tempServerInfoList = u_arraylist_create();
    if (!tempServerInfoList)
    {
        OIC_LOG(ERROR, IP_CLIENT_TAG, "u_arraylist_create failed");
        return 0;
    }

    CAResult_t res = CAGetIPServerInfoList(&tempServerInfoList);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, IP_CLIENT_TAG, "CAGetIPServerInfoList failed");
        CAClearServerInfoList(tempServerInfoList);
        return 0;
    }

    uint32_t len = 0;
    if (isMulticast || strcmp(remoteAddress, OC_MULTICAST_IP) == 0)
    {
        uint32_t listIndex = 0;
        uint32_t listLength = u_arraylist_length(tempServerInfoList);
        for (listIndex = 0; listIndex < listLength; listIndex++)
        {
            CAServerInfo_t *info = (CAServerInfo_t *) u_arraylist_get(tempServerInfoList,
                                                                      listIndex);
            if (!info || info->isMulticastServer || info->isSecured)
            {
                continue;
            }

            if (info->socketFd < 0)
            {
                OIC_LOG(ERROR, IP_CLIENT_TAG, "Invalid Socket Fd");
                continue;
            }

            OIC_LOG_V(DEBUG, IP_CLIENT_TAG,
                      "CA IP Multicast SendData with src ip %s port %d sockFd %d",
                      info->ipAddress, info->port, info->socketFd);
            len = CASendData(remoteAddress, remotePort, data, dataLength, info->socketFd);
        }
    }
    else
    {
        int sockFd = CAGetSocketFdForUnicastServer(tempServerInfoList, remoteAddress, isSecured,
                                                   isMulticast, CA_IPV4);

        if (sockFd < 0)
        {
            OIC_LOG(ERROR, IP_CLIENT_TAG, "Invalid Socket Fd");
            CAClearServerInfoList(tempServerInfoList);
            return len;
        }

        OIC_LOG_V(DEBUG, IP_CLIENT_TAG, "IP unicast SendData sockFd %d", sockFd);

        len = CASendData(remoteAddress, remotePort, data, dataLength, sockFd);

    }
    CAClearServerInfoList(tempServerInfoList);
    return len;
}

