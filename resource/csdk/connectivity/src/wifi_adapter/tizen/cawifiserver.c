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
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

#include "pdu.h"
#include "caadapterutils.h"
#include "umutex.h"

/**
 * @def WIFI_SERVER_TAG
 * @brief Logging tag for module name
 */
#define WIFI_SERVER_TAG "WIFI_SERVER"

/**
 * @def CA_UDP_BIND_RETRY_COUNT
 * @brief Retry count in case of socket bind failure.
 */
#define CA_UDP_BIND_RETRY_COUNT 10

/**
 * @var gUnicastServerSocketDescriptor
 * @brief socket descriptor for unicast server
 */
static int32_t gUnicastServerSocketDescriptor = -1;

/**
 * @var gUnicastServerSocketDescriptor
 * @brief socket descriptor for unicast server
 */
static char *gUnicastServerAddress = NULL;

/**
 * @var gUnicastServerSocketDescriptor
 * @brief socket descriptor for unicast server
 */
static int16_t gUnicastServerPort = -1;

/**
 * @var gMutexUnicastServerSocketDescriptor
 * @brief Mutex for socket descriptor for unicast server
 */
static u_mutex gMutexUnicastServerSocketDescriptor = NULL;
/**
 * @var gMulticastServerSocketDescriptor
 * @brief socket descriptor for multicast server
 */
static int32_t gMulticastServerSocketDescriptor = -1;

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
 * @var gPacketReceivedCallback
 * @brief Callback for notifying the upper layer on receival data from remote OIC device
 */
static CAWiFiPacketReceivedCallback gPacketReceivedCallback = NULL;

/**
 * @var gExceptionCallback
 * @brief Callback for notifying the upper layer when unicast/multicast server encounters exception
 */
static CAWiFiExceptionCallback gExceptionCallback = NULL;

/**
 * @var gUnicastRecvBuffer
 * @brief Character buffer used for receiving unicast data from network
 */
static char gUnicastRecvBuffer[COAP_MAX_PDU_SIZE] = {0};

/**
 * @var gMulticastRecvBuffer
 * @brief Character buffer used for receiving multicast data from network
 */
static char gMulticastRecvBuffer[COAP_MAX_PDU_SIZE] = {0};

/**
 * @fn CAWiFiServerCreateMutex
 * @brief Creates and initializes mutex
 */
static CAResult_t CAWiFiServerCreateMutex(void);

/**
 * @fn CAWiFiServerDestroyMutex
 * @brief Releases all created mutex
 */
static void CAWiFiServerDestroyMutex(void);

/**
 * @fn CAReceiveThreadForMulticast
 * @brief Handler thread for receiving data on multicast server
 */
static void *CAReceiveThreadForMulticast(void *data);

/**
 * @fn CAWiFiReceiveThreadForUnicast
 * @brief Handler thread for receiving data on unicast server
 */
static void *CAWiFiReceiveThreadForUnicast(void *data);

CAResult_t CAWiFiInitializeServer(const u_thread_pool_t threadPool)
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");

    // Input validation
    VERIFY_NON_NULL(threadPool, WIFI_SERVER_TAG, "Thread pool handle is NULL");

    // Initialize mutex
    if (CA_STATUS_OK != CAWiFiServerCreateMutex())
    {
        OIC_LOG(ERROR, WIFI_SERVER_TAG, "Failed to create mutex!");
        return CA_STATUS_FAILED;
    }

    gThreadPool = threadPool;

    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CAWiFiTerminateServer(void)
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");

    gThreadPool = NULL;

    // Destroy mutex
    CAWiFiServerDestroyMutex();

    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "OUT");
}

CAResult_t CAWiFiStartMulticastServer(const char *localAddress, const char *multicastAddress,
                                      const int16_t multicastPort, int32_t *serverFD)
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");

    if (gMulticastServerSocketDescriptor != -1)
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Multicast Server is already running!");
        return CA_SERVER_STARTED_ALREADY;
    }

    VERIFY_NON_NULL(localAddress, WIFI_SERVER_TAG, "Local address is NULL");
    VERIFY_NON_NULL(multicastAddress, WIFI_SERVER_TAG, "Multicast address is NULL");

    if (0 >= multicastPort)
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Invalid input: Multicast port is invalid!");
        return CA_STATUS_INVALID_PARAM;
    }

    // Create a datagram socket on which to recv/send.
    u_mutex_lock(gMutexStopMulticast);
    gStopMulticast = false;
    u_mutex_unlock(gMutexStopMulticast);

    u_mutex_lock(gMutexMulticastServerSocketDescriptor);

    // create a UDP socket
    if ((gMulticastServerSocketDescriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed to Create Socket, Error code: %s",
                  strerror(errno));
        u_mutex_unlock(gMutexMulticastServerSocketDescriptor);
        return CA_SOCKET_OPERATION_FAILED;
    }

    // Make the socket non-blocking
    if (-1 == fcntl(gMulticastServerSocketDescriptor, F_SETFL, O_NONBLOCK))
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed to set non-block mode, Error code: %s",
                  strerror(errno));
        close(gMulticastServerSocketDescriptor);
        gMulticastServerSocketDescriptor = -1;
        u_mutex_unlock(gMutexMulticastServerSocketDescriptor);
        return CA_STATUS_FAILED;
    }

    OIC_LOG(INFO, WIFI_SERVER_TAG, "socket creation success");

    int32_t setOptionOn = 1;
    if (-1 == setsockopt(gMulticastServerSocketDescriptor, SOL_SOCKET, SO_REUSEADDR,
                         (char *) &setOptionOn,
                         sizeof(setOptionOn)))
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed to setsockopt for SO_REUSEADDR, Error code: %s",
                  strerror(errno));
        close(gMulticastServerSocketDescriptor);
        gMulticastServerSocketDescriptor = -1;
        u_mutex_unlock(gMutexMulticastServerSocketDescriptor);
        return CA_SOCKET_OPERATION_FAILED;
    }

    struct sockaddr_in sockAddr;
    memset((char *) &sockAddr, 0, sizeof(sockAddr));

    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(multicastPort);
    sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // bind socket to multicast port
    if (-1 == bind(gMulticastServerSocketDescriptor, (struct sockaddr *) &sockAddr,
                   sizeof(sockAddr)))
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed to Bind Socket! Return Code[%d]",
                  CA_SOCKET_OPERATION_FAILED);
        close(gMulticastServerSocketDescriptor);
        gMulticastServerSocketDescriptor = -1;
        u_mutex_unlock(gMutexMulticastServerSocketDescriptor);
        return CA_SOCKET_OPERATION_FAILED;
    }

    OIC_LOG(INFO, WIFI_SERVER_TAG, "socket bind success");

    // Add membership to receiving socket (join group)
    memset(&gMReq, 0, sizeof(struct ip_mreq));
    gMReq.imr_interface.s_addr = htonl(INADDR_ANY);
    inet_aton(multicastAddress, &gMReq.imr_multiaddr);

    if (-1 == setsockopt(gMulticastServerSocketDescriptor, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                         (char *) &gMReq,
                         sizeof(struct ip_mreq)))
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed to add to multicast group, Error code: %s\n",
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

    *serverFD = gMulticastServerSocketDescriptor;
    u_mutex_unlock(gMutexMulticastServerSocketDescriptor);

    OIC_LOG(INFO, WIFI_SERVER_TAG, "thread_pool_add_task done");
    OIC_LOG(INFO, WIFI_SERVER_TAG, "Multicast Server Started Successfully");

    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "OUT");
    return CA_STATUS_OK;

}

CAResult_t CAWiFiStartUnicastServer(const char *localAddress, int16_t *port,
                                    const bool forceStart, int32_t *serverFD)
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

    if (0 >= port)
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Invalid input: port is invalid!");
        return CA_STATUS_INVALID_PARAM;
    }

    u_mutex_lock(gMutexStopUnicast);
    gStopUnicast = false;
    u_mutex_unlock(gMutexStopUnicast);

    u_mutex_lock(gMutexUnicastServerSocketDescriptor);
    // Create a UDP socket
    if ((gUnicastServerSocketDescriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed to Create Socket, Error code: %s",
                  strerror(errno));
        u_mutex_unlock(gMutexUnicastServerSocketDescriptor);
        return CA_SOCKET_OPERATION_FAILED;
    }

    // Make the socket non-blocking
    if (-1 == fcntl(gUnicastServerSocketDescriptor, F_SETFL, O_NONBLOCK))
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed to set non-block mode, Error code: %s",
                  strerror(errno));
        close(gUnicastServerSocketDescriptor);
        gUnicastServerSocketDescriptor = -1;
        u_mutex_unlock(gMutexUnicastServerSocketDescriptor);
        return CA_STATUS_FAILED;
    }

    OIC_LOG(INFO, WIFI_SERVER_TAG, "socket creation success");

    if (true == forceStart)
    {
        int32_t setOptionOn = 1;
        if (-1 == setsockopt(gUnicastServerSocketDescriptor, SOL_SOCKET, SO_REUSEADDR,
                             (char *) &setOptionOn,
                             sizeof(setOptionOn)))
        {
            OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed to set SO_REUSEADDR! Error code: %s",
                      strerror(errno));
            close(gUnicastServerSocketDescriptor);
            gUnicastServerSocketDescriptor = -1;
            u_mutex_unlock(gMutexUnicastServerSocketDescriptor);
            return CA_SOCKET_OPERATION_FAILED;
        }
    }

    struct sockaddr_in sockAddr;
    bool isBound = false;
    int16_t serverPort = *port;

    memset((char *) &sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(serverPort);
    sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Trying for bind in a loop
    int16_t i;
    for (i = 0; i < CA_UDP_BIND_RETRY_COUNT; i++)
    {
        if (-1 == bind(gUnicastServerSocketDescriptor, (struct sockaddr *) &sockAddr,
                       sizeof(sockAddr)))
        {
            if (false == forceStart)
            {
                OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed to bind socket[%s]. Trying again... ",
                          strerror(errno));

                // Set the port to next one
                serverPort += 1;
                sockAddr.sin_port = htons(serverPort);
                continue;
            }
            else
            {
                OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed to bind socket[%s]!",
                          strerror(errno));
                break;
            }
        }

        isBound = true;
        break;
    }

    if (false == isBound)
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed to bind Socket! Return code[%d]",
                  CA_SOCKET_OPERATION_FAILED);
        close(gUnicastServerSocketDescriptor);
        gUnicastServerSocketDescriptor = -1;
        u_mutex_unlock(gMutexUnicastServerSocketDescriptor);
        return CA_SOCKET_OPERATION_FAILED;
    }

    OIC_LOG(INFO, WIFI_SERVER_TAG, "socket bind success");

    socklen_t len;
    char *serverAddress = NULL;
    if (-1 != getsockname(gUnicastServerSocketDescriptor, (struct sockaddr *)&sockAddr, &len))
    {
        serverPort = ntohs(sockAddr.sin_port);
        serverAddress = inet_ntoa(sockAddr.sin_addr);
    }

    /**
      * The task to listen for data from unicast socket is added to the thread pool.
      * This is a blocking call is made where we try to receive some data..
      * We will keep waiting until some data is received.
      * This task will be terminated when thread pool is freed on stopping the adapters.
      */
    if (CA_STATUS_OK != u_thread_pool_add_task(gThreadPool, (void *) CAWiFiReceiveThreadForUnicast,
            (void *) NULL))
    {
        OIC_LOG(ERROR, WIFI_SERVER_TAG, "[testThreadPool] thread_pool_add_task failed!");

        close(gUnicastServerSocketDescriptor);
        gUnicastServerSocketDescriptor = -1;
        u_mutex_unlock(gMutexUnicastServerSocketDescriptor);
        return CA_STATUS_FAILED;
    }

    // Free the server address previously stored
    OICFree(gUnicastServerAddress);
    gUnicastServerAddress = NULL;
    gUnicastServerPort = serverPort;
    gUnicastServerAddress = (serverAddress) ? strndup(serverAddress, strlen(serverAddress)) :
                            NULL;
    *port = serverPort;
    *serverFD = gUnicastServerSocketDescriptor;
    u_mutex_unlock(gMutexUnicastServerSocketDescriptor);

    OIC_LOG(INFO, WIFI_SERVER_TAG, "Unicast Server Started Successfully");
    return CA_STATUS_OK;
}

CAResult_t CAWiFiStopMulticastServer(void)
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
    if (-1 == setsockopt(gMulticastServerSocketDescriptor, IPPROTO_IP, IP_DROP_MEMBERSHIP,
                         (char *)&gMReq,
                         sizeof(struct ip_mreq)))
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "cannot leave multicast group, Error code: %s\n",
                  strerror(errno));
    }

    // close the socket
    if (-1 == close(gMulticastServerSocketDescriptor))
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

CAResult_t CAWiFiStopUnicastServer()
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
    if (-1 == close(gUnicastServerSocketDescriptor))
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Unicast Server socket close failed, Error code: %s\n",
                  strerror(errno));
        u_mutex_unlock(gMutexUnicastServerSocketDescriptor);
        u_mutex_unlock(gMutexStopUnicast);
        return CA_SOCKET_OPERATION_FAILED;
    }

    u_mutex_unlock(gMutexStopUnicast);
    gUnicastServerSocketDescriptor = -1;

    u_mutex_unlock(gMutexUnicastServerSocketDescriptor);

    OIC_LOG(INFO, WIFI_SERVER_TAG, "Unicast Server Stopped Successfully");
    return CA_STATUS_OK;
}

CAResult_t CAWiFiGetUnicastServerInfo(char **ipAddress, int16_t *port, int32_t *serverFD)
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");

    // Input validation
    VERIFY_NON_NULL(ipAddress, WIFI_SERVER_TAG, "ipAddress holder is NULL");
    VERIFY_NON_NULL(port, WIFI_SERVER_TAG, "port holder is NULL");
    VERIFY_NON_NULL(serverFD, WIFI_SERVER_TAG, "serverID holder is NULL");

    *ipAddress = gUnicastServerAddress;
    *port = gUnicastServerPort;
    *serverFD = gUnicastServerSocketDescriptor;

    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CAWiFiSetPacketReceiveCallback(CAWiFiPacketReceivedCallback callback)
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");

    gPacketReceivedCallback = callback;
}

void CAWiFiSetExceptionCallback(CAWiFiExceptionCallback callback)
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");

    gExceptionCallback = callback;
}

CAResult_t CAWiFiServerCreateMutex(void)
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");

    gMutexUnicastServerSocketDescriptor = u_mutex_new();
    if (!gMutexUnicastServerSocketDescriptor)
    {
        OIC_LOG(ERROR, WIFI_SERVER_TAG, "Failed to created mutex!");
        return CA_STATUS_FAILED;
    }

    gMutexMulticastServerSocketDescriptor = u_mutex_new();
    if (!gMutexMulticastServerSocketDescriptor)
    {
        OIC_LOG(ERROR, WIFI_SERVER_TAG, "Failed to created mutex!");

        CAWiFiServerDestroyMutex();
        return CA_STATUS_FAILED;
    }

    gMutexStopUnicast = u_mutex_new();
    if (!gMutexStopUnicast)
    {
        OIC_LOG(ERROR, WIFI_SERVER_TAG, "Failed to created mutex!");

        CAWiFiServerDestroyMutex();
        return CA_STATUS_FAILED;
    }

    gMutexStopMulticast = u_mutex_new();
    if (!gMutexStopMulticast)
    {
        OIC_LOG(ERROR, WIFI_SERVER_TAG, "Failed to created mutex!");

        CAWiFiServerDestroyMutex();
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CAWiFiServerDestroyMutex(void)
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");

    if (gMutexUnicastServerSocketDescriptor)
    {
        u_mutex_free(gMutexUnicastServerSocketDescriptor);
        gMutexUnicastServerSocketDescriptor = NULL;
    }

    if (gMutexMulticastServerSocketDescriptor)
    {
        u_mutex_free(gMutexMulticastServerSocketDescriptor);
        gMutexMulticastServerSocketDescriptor = NULL;
    }

    if (gMutexStopUnicast)
    {
        u_mutex_free(gMutexStopUnicast);
        gMutexStopUnicast = NULL;
    }

    if (gMutexStopMulticast)
    {
        u_mutex_free(gMutexStopMulticast);
        gMutexStopMulticast = NULL;
    }

    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "OUT");
}

void *CAWiFiReceiveThreadForUnicast(void *data)
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");

    fd_set reads;
    struct timeval timeout;

    // keep listening for data
    while (!gStopUnicast)
    {
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        FD_ZERO(&reads);
        FD_SET(gUnicastServerSocketDescriptor, &reads);

        int32_t ret = select(gUnicastServerSocketDescriptor + 1, &reads, NULL, NULL, &timeout);
        if (gStopUnicast)
        {
            OIC_LOG(DEBUG, WIFI_SERVER_TAG, "Stop Unicast is called");
            break;
        }
        if (ret < 0)
        {
            OIC_LOG_V(FATAL, WIFI_SERVER_TAG, "select returned error %s", strerror(errno));
            continue;
        }
        if (!FD_ISSET(gUnicastServerSocketDescriptor, &reads))
        {
            continue;
        }

        memset(gUnicastRecvBuffer, 0, sizeof(char)*COAP_MAX_PDU_SIZE);

        // Read data from socket
        struct sockaddr_in srcSockAddress;
        int32_t recvLen;
        socklen_t srcAddressLen = sizeof(srcSockAddress);
        if (-1 == (recvLen = recvfrom(gUnicastServerSocketDescriptor, gUnicastRecvBuffer,
                                      COAP_MAX_PDU_SIZE, 0, (struct sockaddr *) &srcSockAddress,
                                      &srcAddressLen)))
        {
            OIC_LOG_V(DEBUG, WIFI_SERVER_TAG, "%s", strerror(errno));
            continue;
        }
        else if (0 == recvLen)
        {
            OIC_LOG(ERROR, WIFI_SERVER_TAG, "Unicast server socket is shutdown !");

            // Notify upper layer this exception
            if (gExceptionCallback)
            {
                gExceptionCallback(CA_UNICAST_SERVER);
            }
            return NULL;
        }

        const char *srcIPAddress = NULL;
        int32_t srcPort = -1;

        srcIPAddress = inet_ntoa(srcSockAddress.sin_addr);
        srcPort = ntohs(srcSockAddress.sin_port);

        OIC_LOG_V(DEBUG, WIFI_SERVER_TAG, "Received packet from %s:%d\n",
                  srcIPAddress, srcPort);
        OIC_LOG_V(DEBUG, WIFI_SERVER_TAG, "Data: %s\t, DataLength: %d\n",
                  gUnicastRecvBuffer, recvLen);

        // Notify data to upper layer
        if (gPacketReceivedCallback)
        {
            gPacketReceivedCallback(srcIPAddress, srcPort, gUnicastRecvBuffer, recvLen);
        }
    }

    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "OUT");
    return NULL;
}

void *CAReceiveThreadForMulticast(void *data)
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");

    fd_set reads;
    struct timeval timeout;

    // keep listening for data
    while (!gStopMulticast)
    {
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        FD_ZERO(&reads);
        FD_SET(gMulticastServerSocketDescriptor, &reads);

        int32_t ret = select(gMulticastServerSocketDescriptor + 1, &reads, NULL, NULL, &timeout);
        if (gStopMulticast)
        {
            OIC_LOG(DEBUG, WIFI_SERVER_TAG, "Stop Multicast is called");
            break;
        }
        if ( ret < 0)
        {
            OIC_LOG_V(FATAL, WIFI_SERVER_TAG, "select returned error %s", strerror(errno));
            continue;
        }
        if (!FD_ISSET(gMulticastServerSocketDescriptor, &reads))
        {
            continue;
        }

        memset(gMulticastRecvBuffer, 0, sizeof(char)*COAP_MAX_PDU_SIZE);

        // Read data from socket
        struct sockaddr_in srcSockAddress;
        int32_t recvLen;
        socklen_t srcAddressLen = sizeof(srcSockAddress);
        if (-1 == (recvLen = recvfrom(gMulticastServerSocketDescriptor, gMulticastRecvBuffer,
                                      COAP_MAX_PDU_SIZE, 0, (struct sockaddr *) &srcSockAddress,
                                      &srcAddressLen)))
        {
            OIC_LOG_V(DEBUG, WIFI_SERVER_TAG, "%s", strerror(errno));
            continue;
        }
        else if (0 == recvLen)
        {
            OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Multicast socket is shutdown, returning from \
                thread\n");

            // Notify upper layer this exception
            if (gExceptionCallback)
            {
                gExceptionCallback(CA_MULTICAST_SERVER);
            }
            return NULL;
        }

        const char *srcIPAddress = NULL;
        int32_t srcPort = -1;

        srcIPAddress = inet_ntoa(srcSockAddress.sin_addr);
        srcPort = ntohs(srcSockAddress.sin_port);

        OIC_LOG_V(DEBUG, WIFI_SERVER_TAG, "Received packet from %s:%d\n",
                  srcIPAddress, srcPort);
        OIC_LOG_V(DEBUG, WIFI_SERVER_TAG, "Data: %s\t, DataLength: %d\n",
                  gMulticastRecvBuffer, recvLen);


        // Notify data to upper layer
        if (gPacketReceivedCallback)
        {
            gPacketReceivedCallback(srcIPAddress, srcPort, gMulticastRecvBuffer, recvLen);
        }
    }

    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "OUT");
    return NULL;
}


