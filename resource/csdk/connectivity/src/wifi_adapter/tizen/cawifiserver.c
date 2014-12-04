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
#include "cawifiserver.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <fcntl.h>
#include <net/if.h>
#include <errno.h>
#include <ifaddrs.h>
#include <unistd.h>
#include "caadapterutils.h"
#include "umutex.h"
#include "cawificlient.h"

/**
 * @def WIFI_SERVER_TAG
 * @brief Logging tag for module name
 */
#define WIFI_SERVER_TAG "WIFI_SERVER"

/**
 * @def CA_BUFFER_LEN
 * @brief Length of buffer for receiving data
 */
#define CA_BUFFER_LEN 512  // Max length of buffer

/**
 * @def CA_UDP_BIND_RETRY_COUNT
 * @brief Retry count in case of socket bind failure.
 */
#define CA_UDP_BIND_RETRY_COUNT 10


/**
 * @var gUnicastServerSocketDescriptor
 * @brief socket descriptor for unicast server
 */
static int gUnicastServerSocketDescriptor = -1;

/**
 * @var gMutexUnicastServerSocketDescriptor
 * @brief Mutex for socket descriptor for unicast server
 */
static u_mutex gMutexUnicastServerSocketDescriptor = NULL;
/**
 * @var gMulticastServerSocketDescriptor
 * @brief socket descriptor for multicast server
 */
static int gMulticastServerSocketDescriptor = -1;

/**
 * @var gMutexMulticastServerSocketDescriptor
 * @brief Mutex for socket descriptor for Multicast server
 */
static u_mutex gMutexMulticastServerSocketDescriptor = NULL;

/**
 * @var gThreadPool
 * @brief ThreadPool for storing u_thread_pool_t handle passed from adapter
 */
static u_thread_pool_t gThreadPool = NULL;

/**
 * @var gMReq
 * @brief ip_mreq structure passed to join a multicast group
 */
static struct ip_mreq gMReq;

/**
 * @var gStopUnicast
 * @brief Flag to control the Receive Unicast Data Thread
 */
static bool gStopUnicast = false;

/**
 * @var gMutexStopUnicast
 * @brief Mutex for gStopUnicast
 */
static u_mutex gMutexStopUnicast = NULL;

/**
 * @var gStopMulticast
 * @brief Flag to control the Receive Multicast Data Thread
 */
static bool gStopMulticast = false;

/**
 * @var gMutexStopMulticast
 * @brief Mutex for gStopMulticast
 */
static u_mutex gMutexStopMulticast = NULL;

/**
 * @var gServerRecvQueueHandle
 * @brief Queue Handle for Receive Data
 */
static CAAdapterMessageQueue_t *gServerRecvQueueHandle = NULL;

/* Skip Queue */
/**
 * @var gNetworkPacketCallback
 * @brief receiving request response callback
 */
static CANetworkPacketReceivedCallback gNetworkPacketCallback = NULL;

void CAInitializeServerMutex()
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");
    gMutexUnicastServerSocketDescriptor = u_mutex_new();
    gMutexMulticastServerSocketDescriptor = u_mutex_new();
    gMutexStopUnicast = u_mutex_new();
    gMutexStopMulticast = u_mutex_new();
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "OUT");
}

void CASetRecvQueueHandle(CAAdapterMessageQueue_t *recvQueueHandle)
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");
    gServerRecvQueueHandle = recvQueueHandle;
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "OUT");
}

void CASetThreadHandle(u_thread_pool_t handle)
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");
    gThreadPool = handle;
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "OUT");
}

CAResult_t CAStartUnicastServer(const char *localAddress, int16_t *port)
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");

    if (gUnicastServerSocketDescriptor != -1)
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Unicast Server is Started Already! Return Code[%d]",
                  CA_SERVER_STARTED_ALREADY);
        return CA_SERVER_STARTED_ALREADY;
    }

    VERIFY_NON_NULL(localAddress, WIFI_SERVER_TAG, "Invalid argument : localAddress is NULL");
    VERIFY_NON_NULL(port, WIFI_SERVER_TAG, "Invalid argument : port is NULL");

    struct sockaddr_in sockAddr;
    int16_t isBound = 0;
    int16_t i = 0;
    int16_t result = 0;
    int16_t status = 0;
    int setOptionOn = 1;
    u_mutex_lock(gMutexStopUnicast);
    gStopUnicast = false;
    u_mutex_unlock(gMutexStopUnicast);

    u_mutex_lock(gMutexUnicastServerSocketDescriptor);
    // Create a UDP socket
    if ((gUnicastServerSocketDescriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed to Create Socket, Error code: %s", strerror(errno));
        u_mutex_unlock(gMutexUnicastServerSocketDescriptor);
        return CA_SOCKET_OPERATION_FAILED;
    }

    // Make the socket non-blocking
    if ((status = fcntl(gUnicastServerSocketDescriptor, F_SETFL, O_NONBLOCK)) < 0)
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "fcntl to make the socket non-blocking failed, Error code: %s",
                  strerror(errno));
        close(gUnicastServerSocketDescriptor);
        gUnicastServerSocketDescriptor = -1;
        u_mutex_unlock(gMutexUnicastServerSocketDescriptor);
        return CA_STATUS_FAILED;
    }

    OIC_LOG(INFO, WIFI_SERVER_TAG, "socket creation success");

    if ((result = setsockopt(gUnicastServerSocketDescriptor, SOL_SOCKET, SO_REUSEADDR,
                             (char *) &setOptionOn,
                             sizeof(setOptionOn))) < 0)
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed to setsockopt for SO_REUSEADDR! Error code: %s",
                  strerror(errno));
        close(gUnicastServerSocketDescriptor);
        gUnicastServerSocketDescriptor = -1;
        u_mutex_unlock(gMutexUnicastServerSocketDescriptor);
        return CA_SOCKET_OPERATION_FAILED;
    }

    // zero out the structure
    memset((char *) &sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(*port);

    // TODO: Assign the Particular Address on Multiple Interfaces
    sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Trying for bind in a loop
    for (i = 0; i < CA_UDP_BIND_RETRY_COUNT; i++)
    {
        // bind socket to port
        if (bind(gUnicastServerSocketDescriptor, (struct sockaddr *) &sockAddr, sizeof(sockAddr)) == -1)
        {
            OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed to bind socket[%s]. Trying again... ", strerror(errno));
            continue;
        }
        isBound = 1;
        break;
    }

    if (!isBound)
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed to bind Socket! Return code[%d]",
                  CA_SOCKET_OPERATION_FAILED);
        close(gUnicastServerSocketDescriptor);
        gUnicastServerSocketDescriptor = -1;
        u_mutex_unlock(gMutexUnicastServerSocketDescriptor);
        return CA_SOCKET_OPERATION_FAILED;
    }

    OIC_LOG(INFO, WIFI_SERVER_TAG, "socket bind success");

    socklen_t len = sizeof(sockAddr);

    if (getsockname(gUnicastServerSocketDescriptor, (struct sockaddr *)&sockAddr, &len) == -1)
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed to getsockname, Error code: %s", strerror(errno));
    }
    else
    {
        OIC_LOG_V(DEBUG, WIFI_SERVER_TAG, "port number %d", ntohs(sockAddr.sin_port));
        *port = ntohs(sockAddr.sin_port);
    }

    /**
      * The task to listen for data from unicast socket is added to the thread pool.
      * This is a blocking call is made where we try to receive some data.. We will keep waiting until some data is received.
      * This task will be terminated when thread pool is freed on stopping the adapters.
      */
    if (CA_STATUS_OK != u_thread_pool_add_task(gThreadPool, (void *) CAReceiveThreadForUnicast,
            (void *) NULL))
    {
        OIC_LOG(ERROR, WIFI_SERVER_TAG, "[testThreadPool] thread_pool_add_task failed!");

        close(gUnicastServerSocketDescriptor);
        gUnicastServerSocketDescriptor = -1;
        u_mutex_unlock(gMutexUnicastServerSocketDescriptor);
        return CA_STATUS_FAILED;
    }
    u_mutex_unlock(gMutexUnicastServerSocketDescriptor);

    // Set Unicast Socket Descriptor in cawificlient
    CASetUnicastSocketDescriptor(gUnicastServerSocketDescriptor);

    OIC_LOG(INFO, WIFI_SERVER_TAG, "thread_pool_add_task done");
    OIC_LOG(INFO, WIFI_SERVER_TAG, "Unicast Server Started Successfully");

    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAStartMulticastServer(const char *mcastAddress, const char *localAddress,
                                  int16_t *port)
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");

    if (gMulticastServerSocketDescriptor != -1)
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Multicast Server is Started Already! Return Code[%d]",
                  CA_SERVER_STARTED_ALREADY);
        return CA_SERVER_STARTED_ALREADY;
    }

    VERIFY_NON_NULL(mcastAddress, WIFI_SERVER_TAG, "Invalid argument : mcastAddress is NULL");
    VERIFY_NON_NULL(localAddress, WIFI_SERVER_TAG, "Invalid argument : localAddress is NULL");
    VERIFY_NON_NULL(port, WIFI_SERVER_TAG, "Invalid argument : port is NULL");

    // Create a datagram socket on which to recv/send.
    struct sockaddr_in sockAddr;
    int16_t result = 0;
    int16_t i = 0;
    int16_t status = 0;

    int setOptionOn = 1;
    u_mutex_lock(gMutexStopMulticast);
    gStopMulticast = false;
    u_mutex_unlock(gMutexStopMulticast);

    u_mutex_lock(gMutexMulticastServerSocketDescriptor);

    // create a UDP socket
    if ((gMulticastServerSocketDescriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed to Create Socket, Error code: %s", strerror(errno));
        u_mutex_unlock(gMutexMulticastServerSocketDescriptor);
        return CA_SOCKET_OPERATION_FAILED;
    }

    // Make the socket non-blocking
    if ((status = fcntl(gMulticastServerSocketDescriptor, F_SETFL, O_NONBLOCK)) < 0)
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "fcntl to make the socket non-blocking failed, Error code: %s",
                  strerror(errno));
        close(gMulticastServerSocketDescriptor);
        gMulticastServerSocketDescriptor = -1;
        u_mutex_unlock(gMutexMulticastServerSocketDescriptor);
        return CA_STATUS_FAILED;
    }

    OIC_LOG(INFO, WIFI_SERVER_TAG, "socket creation success");

    if ((result = setsockopt(gMulticastServerSocketDescriptor, SOL_SOCKET, SO_REUSEADDR,
                             (char *) &setOptionOn,
                             sizeof(setOptionOn))) < 0)
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed to setsockopt for SO_REUSEADDR, Error code: %s",
                  strerror(errno));
        close(gMulticastServerSocketDescriptor);
        gMulticastServerSocketDescriptor = -1;
        u_mutex_unlock(gMutexMulticastServerSocketDescriptor);
        return CA_SOCKET_OPERATION_FAILED;
    }

    // zero out the structure
    memset((char *) &sockAddr, 0, sizeof(sockAddr));

    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(*port);
    // TODO: Assign the Particular Address on Multiple Interfaces
    sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Trying for bind in a loop
    for (i = 0; i < CA_UDP_BIND_RETRY_COUNT; i++)
    {
        // bind socket to multicast port
        if (bind(gMulticastServerSocketDescriptor, (struct sockaddr *) &sockAddr, sizeof(sockAddr)) == -1)
        {
            OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed to bind socket[%s]. Trying again...", strerror(errno));
            continue;
        }
        result = 1;
        break;
    }
    if (!result)
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed to Bind Socket! Return Code[%d]",
                  CA_SOCKET_OPERATION_FAILED);
        close(gMulticastServerSocketDescriptor);
        gMulticastServerSocketDescriptor = -1;
        u_mutex_unlock(gMutexMulticastServerSocketDescriptor);
        return CA_SOCKET_OPERATION_FAILED;
    }

    OIC_LOG(INFO, WIFI_SERVER_TAG, "socket bind success");

    socklen_t len = sizeof(sockAddr);

    if (getsockname(gMulticastServerSocketDescriptor, (struct sockaddr *)&sockAddr, &len) == -1)
    {
        OIC_LOG_V(DEBUG, WIFI_SERVER_TAG, "getsockname failed, Error code: %s", strerror(errno));
    }
    else
    {
        OIC_LOG_V(DEBUG, WIFI_SERVER_TAG, "port number %d\n", ntohs(sockAddr.sin_port));
        *port = ntohs(sockAddr.sin_port);
    }

    // add membership to receiving socket (join group)
    memset(&gMReq, 0, sizeof(struct ip_mreq));
    gMReq.imr_interface.s_addr = htonl(INADDR_ANY);
    inet_aton(mcastAddress, &gMReq.imr_multiaddr);

    if ((result = setsockopt(gMulticastServerSocketDescriptor, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                             (char *) &gMReq,
                             sizeof(struct ip_mreq))) < 0)
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "setsockopt API for IP_ADD_MEMBERSHIP failed, Error code: %s\n",
                  strerror(errno));
        close(gMulticastServerSocketDescriptor);
        gMulticastServerSocketDescriptor = -1;
        u_mutex_unlock(gMutexMulticastServerSocketDescriptor);
        return CA_SOCKET_OPERATION_FAILED;
    }

    /**
      * The task to listen to data from multicastcast socket is added to the thread pool.
      * This is a blocking call is made where we try to receive some data.. We will keep waiting until some data is received.
      * This task will be terminated when thread pool is freed on stopping the adapters.
      */
    if (CA_STATUS_OK != u_thread_pool_add_task(gThreadPool, (void *) CAReceiveThreadForMulticast,
            (void *)NULL))
    {
        OIC_LOG(ERROR, WIFI_SERVER_TAG, "[testThreadPool] thread_pool_add_task failed!");

        close(gMulticastServerSocketDescriptor);
        gMulticastServerSocketDescriptor = -1;
        u_mutex_unlock(gMutexMulticastServerSocketDescriptor);
        return CA_STATUS_FAILED;
    }

    u_mutex_unlock(gMutexMulticastServerSocketDescriptor);

    OIC_LOG(INFO, WIFI_SERVER_TAG, "thread_pool_add_task done");
    OIC_LOG(INFO, WIFI_SERVER_TAG, "Multicast Server Started Successfully");

    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAStopUnicastServer()
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");
    u_mutex_lock(gMutexUnicastServerSocketDescriptor);

    if (gUnicastServerSocketDescriptor == -1)
    {
        OIC_LOG(INFO, WIFI_SERVER_TAG, "Unicast Server is not yet Started");
        u_mutex_unlock(gMutexUnicastServerSocketDescriptor);
        return CA_SERVER_NOT_STARTED;
    }
    u_mutex_lock(gMutexStopUnicast);
    gStopUnicast = true;

    // close the socket
    int16_t ret = close(gUnicastServerSocketDescriptor);

    if (-1 == ret)
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Unicast Server socket close failed, Error code: %s\n",
                  strerror(errno));
        u_mutex_unlock(gMutexUnicastServerSocketDescriptor);
        u_mutex_unlock(gMutexStopUnicast);
        return CA_SOCKET_OPERATION_FAILED;
    }

    u_mutex_unlock(gMutexStopUnicast);
    gUnicastServerSocketDescriptor = -1;
    // UnSet Unicast Socket Descriptor in cawificlient
    CASetUnicastSocketDescriptor(gUnicastServerSocketDescriptor);
    u_mutex_unlock(gMutexUnicastServerSocketDescriptor);

    OIC_LOG(INFO, WIFI_SERVER_TAG, "Unicast Server Stopped Successfully");

    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAStopMulticastServer()
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");

    u_mutex_lock(gMutexMulticastServerSocketDescriptor);

    if (gMulticastServerSocketDescriptor == -1)
    {
        OIC_LOG(INFO, WIFI_SERVER_TAG, "Multicast Server is not yet Started");
        u_mutex_unlock(gMutexMulticastServerSocketDescriptor);
        return CA_SERVER_NOT_STARTED;
    }

    u_mutex_lock(gMutexStopMulticast);
    gStopMulticast = true;

    // leave the group after you are done
    int16_t result = setsockopt(gMulticastServerSocketDescriptor, IPPROTO_IP, IP_DROP_MEMBERSHIP,
                                (char *)&gMReq,
                                sizeof(struct ip_mreq));
    if (result < 0)
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "cannot leave multicast group, Error code: %s\n",
                  strerror(errno));
    }

    // close the socket
    result = close(gMulticastServerSocketDescriptor);
    if (-1 == result)
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Multicast Server socket close failed, Error code: %s\n",
                  strerror(errno));
        u_mutex_unlock(gMutexMulticastServerSocketDescriptor);
        u_mutex_unlock(gMutexStopMulticast);
        return CA_SOCKET_OPERATION_FAILED;
    }

    u_mutex_unlock(gMutexStopMulticast);

    gMulticastServerSocketDescriptor = -1;
    u_mutex_unlock(gMutexMulticastServerSocketDescriptor);

    OIC_LOG(INFO, WIFI_SERVER_TAG, "Multicast Server Stopped Successfully");

    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}

void *CAReceiveThreadForUnicast(void *data)
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");
    if (NULL == gServerRecvQueueHandle)
    {
        OIC_LOG(ERROR, WIFI_SERVER_TAG, "QueueHandle is not Initialized");
        return NULL;
    }

    uint32_t recvLen;
    int32_t ret = 0;
    struct sockaddr_in siOther;
    socklen_t sLen = sizeof(siOther);
    fd_set reads;
    struct timeval timeout;
    CARemoteEndpoint_t *endPointUnicast = (CARemoteEndpoint_t *) OICMalloc (sizeof(CARemoteEndpoint_t));
    if (NULL == endPointUnicast)
    {
        OIC_LOG(ERROR, WIFI_SERVER_TAG, "Memory Allocation failed");
        return NULL;
    }
    memset (endPointUnicast, 0, sizeof(CARemoteEndpoint_t));



    // keep listening for data
    while (!gStopUnicast)
    {
        OIC_LOG(DEBUG, WIFI_SERVER_TAG, "Waiting for data..");

        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        FD_ZERO(&reads);
        // Use select for polling the socket fd
        FD_SET(gUnicastServerSocketDescriptor, &reads);

        ret = select(gUnicastServerSocketDescriptor + 1, &reads, NULL, NULL, &timeout);
        if ( ret < 0)
        {
            OIC_LOG_V(FATAL, WIFI_SERVER_TAG, "select API failed");
            continue;
        }
        if (!FD_ISSET(gUnicastServerSocketDescriptor, &reads))
        {
            OIC_LOG(DEBUG, WIFI_SERVER_TAG, "No data to read");
            continue;
        }

        // Allocate Memory for COAP Buffer
        char *buf = (char *) OICMalloc (CA_BUFFER_LEN);
        if (NULL == buf)
        {
            OIC_LOG(ERROR, WIFI_SERVER_TAG, "Memory Allocation failed");
            return (void *) NULL;
        }

        memset(buf, 0, sizeof(char) * CA_BUFFER_LEN);

        CARemoteEndpoint_t *endPointUnicast = (CARemoteEndpoint_t *) OICMalloc (sizeof(CARemoteEndpoint_t));
        if (NULL == endPointUnicast)
        {
            OIC_LOG(ERROR, WIFI_SERVER_TAG, "Memory Allocation failed");
            OICFree(buf);
            return NULL;
        }
        memset (endPointUnicast, 0, sizeof(CARemoteEndpoint_t));


        // try to receive some data
        if ((recvLen = recvfrom(gUnicastServerSocketDescriptor, buf, CA_BUFFER_LEN, 0,
                                (struct sockaddr *) &siOther, &sLen)) == -1)
        {
            OIC_LOG_V(DEBUG, WIFI_SERVER_TAG, "%s\n", strerror(errno));
            OICFree(buf);
            OICFree(endPointUnicast);
            continue;
        }
        else if (0 == recvLen)
        {
            OIC_LOG(ERROR, WIFI_SERVER_TAG, "Unicast socket is shutdown, returning from thread\n");
            OICFree(buf);
            OICFree(endPointUnicast);
            return (void *) NULL;
        }

        // print details of the client/peer and the data received
        OIC_LOG_V(DEBUG, WIFI_SERVER_TAG, "Received packet from %s:%d\n", inet_ntoa(siOther.sin_addr),
                  ntohs(siOther.sin_port));
        OIC_LOG_V(DEBUG, WIFI_SERVER_TAG, "Data: %s, DataLength: %d\n", buf, sizeof(buf));

        endPointUnicast->resourceUri = NULL; // will be filled by upper layer
        strncpy((char *)endPointUnicast->addressInfo.IP.ipAddress, inet_ntoa(siOther.sin_addr),
                CA_IPADDR_SIZE);
        endPointUnicast->addressInfo.IP.port = ntohs(siOther.sin_port);
        endPointUnicast->connectivityType = CA_WIFI;

        // Enqueue the Received Message in the Queue
#if 0 /* Skip Queue */
        CAAdapterEnqueueMessage(gServerRecvQueueHandle, &endPointUnicast, buf, recvLen);
#else
        if (gNetworkPacketCallback)
        {
            gNetworkPacketCallback(endPointUnicast, buf, recvLen);
        }
        else
        {
            OICFree(buf);
            OICFree(endPointUnicast);
        }
        // Currently, endPointUnicast and buf is freed in gNetworkPacketCallback
#endif //#if 0
    }
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "stopUnicastServer is called, Breaking from while loop\n");
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "OUT");
    return (void *) NULL;
}

void *CAReceiveThreadForMulticast(void *data)
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");

    if (NULL == gServerRecvQueueHandle)
    {
        OIC_LOG(ERROR, WIFI_SERVER_TAG, "QueueHandle is not Initialized");
        return NULL;
    }

    int recvLen;
    struct sockaddr_in siOther;
    int32_t ret = 0;
    socklen_t sLen = sizeof(siOther);
    fd_set reads;
    struct timeval timeout;

    // keep listening for data
    while (!gStopMulticast)
    {
        OIC_LOG(DEBUG, WIFI_SERVER_TAG, "Waiting for data...\n");

        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        FD_ZERO(&reads);
        // Use select for polling the socket fd
        FD_SET(gMulticastServerSocketDescriptor, &reads);

        ret = select(gMulticastServerSocketDescriptor + 1, &reads, NULL, NULL, &timeout);
        if ( ret < 0)
        {
            OIC_LOG_V(FATAL, WIFI_SERVER_TAG, "select API failed");
            continue;
        }
        if (!FD_ISSET(gMulticastServerSocketDescriptor, &reads))
        {
            OIC_LOG_V(DEBUG, WIFI_SERVER_TAG, "No data to read");
            continue;
        }

        // Allocate Memory for COAP Buffer
        char *buf = (char *) OICMalloc (CA_BUFFER_LEN);
        if (NULL == buf)
        {
            OIC_LOG(ERROR, WIFI_SERVER_TAG, "Memory Allocation failed");
            return (void *) NULL;
        }

        memset(buf, 0, sizeof(char) * CA_BUFFER_LEN);

        CARemoteEndpoint_t *endPointMulticast = (CARemoteEndpoint_t *) OICMalloc (sizeof(
                CARemoteEndpoint_t));
        if (NULL == endPointMulticast)
        {
            OIC_LOG(ERROR, WIFI_SERVER_TAG, "Memory Allocation failed");
            OICFree(buf);
            return NULL;
        }
        memset (endPointMulticast, 0, sizeof(CARemoteEndpoint_t));

        // try to receive some data
        if ((recvLen = recvfrom(gMulticastServerSocketDescriptor, buf, CA_BUFFER_LEN, 0,
                                (struct sockaddr *) &siOther, &sLen)) == -1)
        {
            OIC_LOG_V(DEBUG, WIFI_SERVER_TAG, "%s\n", strerror(errno));
            OICFree(buf);
            OICFree(endPointMulticast);
            continue;
        }
        else if (0 == recvLen)
        {
            OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Multicast socket is shutdown, returning from thread\n");
            OICFree(buf);
            OICFree(endPointMulticast);
            return (void *) NULL;
        }

        // print details of the client/peer and the data received
        OIC_LOG_V(DEBUG, WIFI_SERVER_TAG, "Received packet from %s:%d\n",
                  inet_ntoa(siOther.sin_addr), ntohs(siOther.sin_port));
        OIC_LOG_V(DEBUG, WIFI_SERVER_TAG, "Data: %s\t, DataLength: %d\n", buf, recvLen);

        endPointMulticast->resourceUri = NULL; // will be filled by upper layer
        strncpy((char *)endPointMulticast->addressInfo.IP.ipAddress, inet_ntoa(siOther.sin_addr),
                (CA_IPADDR_SIZE-1));
        endPointMulticast->addressInfo.IP.port = ntohs(siOther.sin_port);
        endPointMulticast->connectivityType = CA_WIFI;
        if (gNetworkPacketCallback)
        {
            gNetworkPacketCallback(endPointMulticast, buf, recvLen);
        }
        else
        {
            OICFree(buf);
            OICFree(endPointMulticast);
        }
        // Currently, endpointMulticast and buf is freed in gNetworkPacketCallback
    }
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "stopMulticastServer is called, Breaking from while loop\n");

    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "OUT");
    return (void *) NULL;
}

/* Skip Queue */
void CASetWIFINetworkPacketCallback(CANetworkPacketReceivedCallback callback)
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");
    gNetworkPacketCallback = callback;
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "OUT");
}
