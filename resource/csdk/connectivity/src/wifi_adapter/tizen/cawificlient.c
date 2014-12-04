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
#include "cawificlient.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "caadapterutils.h"

/**
 * @def WIFI_CLIENT_TAG
 * @brief Logging tag for module name
 */
#define WIFI_CLIENT_TAG "WIFI_CLIENT"

/**
 * @var gClientSendQueueHandle
 * @brief Queue Handle for send Data
 */
static CAAdapterMessageQueue_t *gClientSendQueueHandle = NULL;

/**
 * @var gSendDataFlag
 * @brief Flag to check if Termination is Called
 */
static CABool_t gSendDataFlag = CA_FALSE;

/**
 * @var gClientSendDataCond
 * @brief Condition for Send Data
 */
static u_cond gClientSendDataCond = NULL;

/**
 * @var gMulticastIP
 * @brief Multicast IP address
 */
static char gMulticastIP[CA_IPADDR_SIZE];

/**
 * @var gMulticastPort
 * @brief Multicast Port
 */
static uint32_t gMulticastPort;

/**
 * @var gUnicastServerSocketDescClient
 * @brief socket descriptor for unicast server
 */
static int gUnicastServerSocketDescClient = -1;

void CASetUnicastSocketDescriptor(int unicastSocketDesc)
{
    OIC_LOG(DEBUG, WIFI_CLIENT_TAG, "IN");
    gUnicastServerSocketDescClient = unicastSocketDesc;
    OIC_LOG(DEBUG, WIFI_CLIENT_TAG, "OUT");
}

void CASetSendQueueHandle(CAAdapterMessageQueue_t *gSendQueueHandle, CABool_t flag, u_cond cond)
{
    OIC_LOG(DEBUG, WIFI_CLIENT_TAG, "IN");
    gClientSendQueueHandle = gSendQueueHandle;
    gSendDataFlag = flag;
    if (NULL == cond)
    {
        u_cond_signal(gClientSendDataCond);
    }
    gClientSendDataCond = cond;
    OIC_LOG(DEBUG, WIFI_CLIENT_TAG, "OUT");
}

void CASetMulticastData(char *mAddr, uint32_t mPort)
{
    OIC_LOG(DEBUG, WIFI_CLIENT_TAG, "IN");
    strncpy(gMulticastIP, mAddr, CA_IPADDR_SIZE - 1);
    gMulticastPort = mPort;
    OIC_LOG(DEBUG, WIFI_CLIENT_TAG, "OUT");
}

CAResult_t CAStartSendDataThread(u_thread_pool_t threadHandle)
{
    OIC_LOG(DEBUG, WIFI_CLIENT_TAG, "IN");
    VERIFY_NON_NULL(threadHandle, WIFI_CLIENT_TAG, "Invalid argument : threadHandle is NULL");
    if (CA_STATUS_OK != u_thread_pool_add_task(threadHandle, (void *) CAStartThreadForSendData,
            (void *) NULL))
    {
        OIC_LOG(ERROR, WIFI_CLIENT_TAG, "Failed to add task in thread pool");
        return CA_STATUS_FAILED;
    }
    OIC_LOG(DEBUG, WIFI_CLIENT_TAG, "OUT");
    return CA_STATUS_OK;
}

uint32_t CAWIFISendData(const char *remoteIpAddress, const uint32_t port, const char *data,
                        uint32_t dataLen, int16_t isMulticast)
{
    OIC_LOG(DEBUG, WIFI_CLIENT_TAG, "IN");

    VERIFY_NON_NULL_RET(remoteIpAddress, WIFI_CLIENT_TAG, "Invalid argument : remoteIpAddress is NULL",
                        0);
    VERIFY_NON_NULL_RET(data, WIFI_CLIENT_TAG, "Invalid argument : data is NULL", 0);

    OIC_LOG_V(DEBUG, WIFI_CLIENT_TAG, "remoteIPAddress is %s", remoteIpAddress);
    OIC_LOG_V(DEBUG, WIFI_CLIENT_TAG, "PortNumber is %d", port);
    OIC_LOG_V(DEBUG, WIFI_CLIENT_TAG, "Data[%d] to send is %s", dataLen, data);

    uint32_t sendDataLength = 0;

    if (dataLen <= 0)
    {
        OIC_LOG(ERROR, WIFI_CLIENT_TAG, "Invalid Arguemnet : Datalength is <= 0");
        return sendDataLength;
    }

    if (-1 == gUnicastServerSocketDescClient)
    {
        OIC_LOG(ERROR, WIFI_CLIENT_TAG, "Unicast Server is not running");
        return sendDataLength;
    }

    struct sockaddr_in destAddr;
    memset((char *)&destAddr, 0, sizeof(destAddr));
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(port);

    // Conversion from ASCII format to Network format
    if (inet_aton(remoteIpAddress, &destAddr.sin_addr) == 0)
    {
        OIC_LOG(ERROR, WIFI_CLIENT_TAG, "Failed to convert from ASCII to Network Address");
        return sendDataLength;
    }

    sendDataLength = sendto(gUnicastServerSocketDescClient, data, dataLen, 0,
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

void *CAStartThreadForSendData()
{
    OIC_LOG(DEBUG, WIFI_CLIENT_TAG, "IN");
    if (NULL == gClientSendQueueHandle)
    {
        OIC_LOG(ERROR, WIFI_CLIENT_TAG, "Queue Handle is NULL");
        return NULL;
    }

    u_mutex sendDataMutex = u_mutex_new();
    CAAdapterMessage_t *message;

    while (gSendDataFlag)
    {
        OIC_LOG(DEBUG, WIFI_CLIENT_TAG, "Inside The while loop");
        u_mutex_lock(sendDataMutex);
        u_cond_wait(gClientSendDataCond, sendDataMutex);
        while (CA_STATUS_OK == CAAdapterDequeueMessage(gClientSendQueueHandle, &message))
        {
            if (NULL != message->remoteEndpoint)
            {
                OIC_LOG(DEBUG, WIFI_CLIENT_TAG, "Send Unicast Data is called");
                CAWIFISendData(message->remoteEndpoint->addressInfo.IP.ipAddress,
                               message->remoteEndpoint->addressInfo.IP.port, message->data, message->dataLen, 0);
            }
            else
            {
                OIC_LOG(DEBUG, WIFI_CLIENT_TAG, "Send Multicast Data is called");
                CAWIFISendData(gMulticastIP, gMulticastPort, message->data, message->dataLen, 1);
            }
            CAAdapterFreeMessage(message);
        }
        u_mutex_unlock(sendDataMutex);
    }

    OIC_LOG(DEBUG, WIFI_CLIENT_TAG, "OUT");
    return NULL;
}
