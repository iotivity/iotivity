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
 * @var gUnicastServerSocketFD
 * @brief Unicast server socket descriptor
 */
static int32_t gUnicastServerSocketFD = -1;

/**
 * @var gMutexUnicastServer
 * @brief Mutex to synchronize unicast server
 */
static u_mutex gMutexUnicastServer = NULL;

/**
 * @var gStopUnicast
 * @brief Flag to control the Receive Unicast Data Thread
 */
static bool gStopUnicast = false;

/**
 * @var gMulticastServerSocketFD
 * @brief socket descriptor for multicast server
 */
static int32_t gMulticastServerSocketFD = -1;

/**
 * @var gMutexMulticastServer
 * @brief Mutex to synchronize secure multicast server
 */
static u_mutex gMutexMulticastServer = NULL;

/**
 * @var gStopMulticast
 * @brief Flag to control the Receive Multicast Data Thread
 */
static bool gStopMulticast = false;

#ifdef __WITH_DTLS__
/**
 * @var gSecureUnicastServerSocketFD
 * @brief Secure unicast server socket descriptor
 */
static int32_t gSecureUnicastServerSocketFD = -1;

/**
 * @var gMutexUnicastServer
 * @brief Mutex to synchronize secure unicast server
 */
static u_mutex gMutexSecureUnicastServer = NULL;

/**
 * @var gStopSecureUnicast
 * @brief Flag to control the unicast secure data receive thread
 */
static bool gStopSecureUnicast = false;

/**
 * @var gSecureUnicastRecvBuffer
 * @brief Character buffer used for receiving unicast data from network
 */
static char gSecureUnicastRecvBuffer[COAP_MAX_PDU_SIZE] = {0};
#endif

/**
 * @var gThreadPool
 * @brief ThreadPool for storing u_thread_pool_t handle passed from adapter
 */
static u_thread_pool_t gThreadPool = NULL;

/**
 * @var gMulticastMemberReq
 * @brief ip_mreq structure passed to join a multicast group
 */
static struct ip_mreq gMulticastMemberReq;

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

static void CAUnicastReceiveHandler(void *data)
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "[CAUnicastReceiveHandler] IN");

    fd_set reads;
    struct timeval timeout;

    while (!gStopUnicast)
    {
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        FD_ZERO(&reads);
        FD_SET(gUnicastServerSocketFD, &reads);

        int32_t ret = select(gUnicastServerSocketFD + 1, &reads, NULL, NULL, &timeout);
        if (gStopUnicast)
        {
            OIC_LOG(DEBUG, WIFI_SERVER_TAG, "Stop request already received for unicast server");
            break;
        }
        if (ret < 0)
        {
            OIC_LOG_V(FATAL, WIFI_SERVER_TAG, "select returned error %s", strerror(errno));
            continue;
        }
        if (!FD_ISSET(gUnicastServerSocketFD, &reads))
        {
            continue;
        }

        memset(gUnicastRecvBuffer, 0, sizeof(char)*COAP_MAX_PDU_SIZE);

        // Read data from socket
        struct sockaddr_in srcSockAddress;
        int32_t recvLen;
        socklen_t srcAddressLen = sizeof(srcSockAddress);
        if (-1 == (recvLen = recvfrom(gUnicastServerSocketFD, gUnicastRecvBuffer,
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
            return;
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
            gPacketReceivedCallback(srcIPAddress, srcPort, gUnicastRecvBuffer, recvLen, false);
        }
    }

    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "[CAUnicastReceiveHandler] OUT");
}

#ifdef __WITH_DTLS__
static void CASecureUnicastReceiveHandler(void *data)
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "[CASecureUnicastReceiveHandler] IN");

    fd_set reads;
    struct timeval timeout;

    while (!gStopSecureUnicast)
    {
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        FD_ZERO(&reads);
        FD_SET(gSecureUnicastServerSocketFD, &reads);

        int32_t ret = select(gSecureUnicastServerSocketFD + 1, &reads, NULL, NULL, &timeout);
        if (gStopSecureUnicast)
        {
            OIC_LOG(DEBUG, WIFI_SERVER_TAG, "Stop request already received for secure server");
            break;
        }

        if (ret < 0)
        {
            OIC_LOG_V(FATAL, WIFI_SERVER_TAG, "select returned error %s", strerror(errno));
            continue;
        }

        if (!FD_ISSET(gSecureUnicastServerSocketFD, &reads))
        {
            continue;
        }

        memset(gUnicastRecvBuffer, 0, sizeof(char)*COAP_MAX_PDU_SIZE);

        // Read data from socket
        struct sockaddr_in srcSockAddress;
        int32_t recvLen;
        socklen_t srcAddressLen = sizeof(srcSockAddress);
        if (-1 == (recvLen = recvfrom(gSecureUnicastServerSocketFD, gSecureUnicastRecvBuffer,
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
            return;
        }

        const char *srcIPAddress = NULL;
        int32_t srcPort = -1;

        srcIPAddress = inet_ntoa(srcSockAddress.sin_addr);
        srcPort = ntohs(srcSockAddress.sin_port);

        OIC_LOG_V(DEBUG, WIFI_SERVER_TAG, "Received packet from %s:%d\n",
                  srcIPAddress, srcPort);
        OIC_LOG_V(DEBUG, WIFI_SERVER_TAG, "Data: %s\t, DataLength: %d\n",
                  gSecureUnicastRecvBuffer, recvLen);

        CAResult_t result = CAAdapterNetDtlsDecrypt(srcIPAddress,
                            srcPort,
                            gSecureUnicastRecvBuffer,
                            recvLen);

        OIC_LOG_V(DEBUG, WIFI_SERVER_TAG, " CAAdapterNetDtlsDecrypt returns [%d]", result);
    }

    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "[CASecureUnicastReceiveHandler] OUT");
}
#endif

static void CAMulticastReceiveHandler(void *data)
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");

    fd_set reads;
    struct timeval timeout;

    while (!gStopMulticast)
    {
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        FD_ZERO(&reads);
        FD_SET(gMulticastServerSocketFD, &reads);

        int32_t ret = select(gMulticastServerSocketFD + 1, &reads, NULL, NULL, &timeout);
        if (gStopMulticast)
        {
            OIC_LOG(DEBUG, WIFI_SERVER_TAG, "Stop request already received for multicast server");
            break;
        }
        if ( ret < 0)
        {
            OIC_LOG_V(FATAL, WIFI_SERVER_TAG, "select returned error %s", strerror(errno));
            continue;
        }
        if (!FD_ISSET(gMulticastServerSocketFD, &reads))
        {
            continue;
        }

        memset(gMulticastRecvBuffer, 0, sizeof(char)*COAP_MAX_PDU_SIZE);

        // Read data from socket
        struct sockaddr_in srcSockAddress;
        int32_t recvLen;
        socklen_t srcAddressLen = sizeof(srcSockAddress);
        if (-1 == (recvLen = recvfrom(gMulticastServerSocketFD, gMulticastRecvBuffer,
                                      COAP_MAX_PDU_SIZE, 0, (struct sockaddr *) &srcSockAddress,
                                      &srcAddressLen)))
        {
            OIC_LOG_V(DEBUG, WIFI_SERVER_TAG, "%s", strerror(errno));
            continue;
        }
        else if (0 == recvLen)
        {
            OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Multicast socket is shutdown!\n");

            // Notify upper layer this exception
            if (gExceptionCallback)
            {
                gExceptionCallback(CA_MULTICAST_SERVER);
            }
            return;
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
            gPacketReceivedCallback(srcIPAddress, srcPort, gMulticastRecvBuffer, recvLen, false);
        }
    }

    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "OUT");
}

static CAResult_t CAStartUnicastServer(const char *localAddress, int16_t *port,
                                       const bool forceStart, u_thread_func receiveThread,
                                       int32_t *serverFD)
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");

    int32_t socketFD = -1;

    // Create a UDP socket
    if (-1 == (socketFD = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)))
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed to create Socket, Error code: %s",
                  strerror(errno));
        return CA_STATUS_FAILED;
    }

    // Make the socket non-blocking
    if (-1 == fcntl(socketFD, F_SETFL, O_NONBLOCK))
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed to set non-block mode, Error code: %s",
                  strerror(errno));

        close(socketFD);
        return CA_STATUS_FAILED;
    }

    if (true == forceStart)
    {
        int32_t setOptionOn = 1;
        if (-1 == setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR,
                             (char *) &setOptionOn,
                             sizeof(setOptionOn)))
        {
            OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed to set SO_REUSEADDR! Error code: %s",
                      strerror(errno));

            close(socketFD);
            return CA_STATUS_FAILED;
        }
    }

    struct sockaddr_in sockAddr;
    bool isBound = false;
    int16_t serverPort = *port;

    memset((char *) &sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(serverPort);
    sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int16_t i;
    for (i = 0; i < CA_UDP_BIND_RETRY_COUNT; i++)
    {
        if (-1 == bind(socketFD, (struct sockaddr *) &sockAddr,
                       sizeof(sockAddr)))
        {
            if (false == forceStart)
            {
                OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed to bind socket[%s]. Trying again... ",
                          strerror(errno));

                //Set the port to next one
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
        close(socketFD);
        return CA_STATUS_FAILED;
    }

    *port = serverPort;
    *serverFD = socketFD;

    /**
      * The task to listen for data from unicast socket is added to the thread pool.
      * This is a blocking call is made where we try to receive some data..
      * We will keep waiting until some data is received.
      * This task will be terminated when thread pool is freed on stopping the adapters.
      */
    if (CA_STATUS_OK != u_thread_pool_add_task(gThreadPool, receiveThread, NULL))
    {
        OIC_LOG(ERROR, WIFI_SERVER_TAG, "Failed to create read thread!");

        close(socketFD);
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}

static void CAWiFiServerDestroyMutex(void)
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");

    if (gMutexUnicastServer)
    {
        u_mutex_free(gMutexUnicastServer);
        gMutexUnicastServer = NULL;
    }

#ifdef __WITH_DTLS__
    if (gMutexSecureUnicastServer)
    {
        u_mutex_free(gMutexSecureUnicastServer);
        gMutexSecureUnicastServer = NULL;
    }
#endif

    if (gMutexMulticastServer)
    {
        u_mutex_free(gMutexMulticastServer);
        gMutexMulticastServer = NULL;
    }

    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "OUT");
}

static CAResult_t CAWiFiServerCreateMutex(void)
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");

    gMutexUnicastServer = u_mutex_new();
    if (!gMutexUnicastServer)
    {
        OIC_LOG(ERROR, WIFI_SERVER_TAG, "Failed to created mutex!");
        return CA_STATUS_FAILED;
    }

#ifdef __WITH_DTLS__
    gMutexSecureUnicastServer = u_mutex_new();
    if (!gMutexSecureUnicastServer)
    {
        OIC_LOG(ERROR, WIFI_SERVER_TAG, "Failed to created mutex!");
        return CA_STATUS_FAILED;
    }
#endif

    gMutexMulticastServer = u_mutex_new();
    if (!gMutexMulticastServer)
    {
        OIC_LOG(ERROR, WIFI_SERVER_TAG, "Failed to created mutex!");

        CAWiFiServerDestroyMutex();
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}

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

CAResult_t CAWiFiStartUnicastServer(const char *localAddress, int16_t *port,
                                    const bool forceStart, const CABool_t isSecured,
                                    int32_t *serverFD)
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");

    // Input validation
    VERIFY_NON_NULL(localAddress, WIFI_SERVER_TAG, "localAddress");
    VERIFY_NON_NULL(port, WIFI_SERVER_TAG, "port");
    VERIFY_NON_NULL(serverFD, WIFI_SERVER_TAG, "server socket FD");

    if (0 >= *port)
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Invalid input: port is invalid!");
        return CA_STATUS_INVALID_PARAM;
    }

    *serverFD = -1;
    if (CA_FALSE == isSecured)
    {
        u_mutex_lock(gMutexUnicastServer);
        if (-1 != gUnicastServerSocketFD)
        {
            OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Unicast Server is Started Already!",
                      CA_SERVER_STARTED_ALREADY);

            *serverFD = gUnicastServerSocketFD;
            u_mutex_unlock(gMutexUnicastServer);
            return CA_SERVER_STARTED_ALREADY;
        }

        gStopUnicast = false;
        if (CA_STATUS_OK != CAStartUnicastServer(localAddress, port, forceStart,
                CAUnicastReceiveHandler, &gUnicastServerSocketFD))
        {
            OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed to start unicast server!");
            gUnicastServerSocketFD = -1;
            u_mutex_unlock(gMutexUnicastServer);
            return CA_STATUS_FAILED;
        }

        *serverFD = gUnicastServerSocketFD;
        u_mutex_unlock(gMutexUnicastServer);
    }
#ifdef __WITH_DTLS__
    else // Start unicast server for secured communication
    {
        u_mutex_lock(gMutexSecureUnicastServer);
        if (-1 != gSecureUnicastServerSocketFD)
        {
            OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Unicast Server is Started Already!",
                      CA_SERVER_STARTED_ALREADY);

            *serverFD = gSecureUnicastServerSocketFD;
            u_mutex_unlock(gMutexSecureUnicastServer);
            return CA_SERVER_STARTED_ALREADY;
        }

        gStopSecureUnicast = false;
        if (CA_STATUS_OK != CAStartUnicastServer(localAddress, port, forceStart,
                CASecureUnicastReceiveHandler, &gSecureUnicastServerSocketFD))
        {
            OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed to start unicast server!");
            gSecureUnicastServerSocketFD = -1;
            u_mutex_unlock(gMutexSecureUnicastServer);
            return CA_STATUS_FAILED;
        }

        *serverFD = gSecureUnicastServerSocketFD;
        u_mutex_unlock(gMutexSecureUnicastServer);
    }
#endif
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAWiFiStartMulticastServer(const char *localAddress, const char *multicastAddress,
                                      const int16_t multicastPort, int32_t *serverFD)
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");

    // Input validation
    VERIFY_NON_NULL(localAddress, WIFI_SERVER_TAG, "Local address is NULL");
    VERIFY_NON_NULL(multicastAddress, WIFI_SERVER_TAG, "Multicast address is NULL");

    if (0 >= multicastPort)
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Invalid input: Multicast port is invalid!");
        return CA_STATUS_INVALID_PARAM;
    }

    u_mutex_lock(gMutexMulticastServer);

    if (gMulticastServerSocketFD != -1)
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Multicast Server is already running!");
        return CA_SERVER_STARTED_ALREADY;
    }

    if ((gMulticastServerSocketFD = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed to Create Socket, Error code: %s",
                  strerror(errno));
        u_mutex_unlock(gMutexMulticastServer);
        return CA_STATUS_FAILED;
    }

    // Make the socket non-blocking
    if (-1 == fcntl(gMulticastServerSocketFD, F_SETFL, O_NONBLOCK))
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed to set non-block mode, Error code: %s",
                  strerror(errno));
        close(gMulticastServerSocketFD);
        gMulticastServerSocketFD = -1;
        u_mutex_unlock(gMutexMulticastServer);
        return CA_STATUS_FAILED;
    }

    OIC_LOG(INFO, WIFI_SERVER_TAG, "socket creation success");

    int32_t setOptionOn = 1;
    if (-1 == setsockopt(gMulticastServerSocketFD, SOL_SOCKET, SO_REUSEADDR,
                         (char *) &setOptionOn,
                         sizeof(setOptionOn)))
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed to setsockopt for SO_REUSEADDR, Error code: %s",
                  strerror(errno));
        close(gMulticastServerSocketFD);
        gMulticastServerSocketFD = -1;
        u_mutex_unlock(gMutexMulticastServer);
        return CA_STATUS_FAILED;
    }

    struct sockaddr_in sockAddr;
    memset((char *) &sockAddr, 0, sizeof(sockAddr));

    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(multicastPort);
    sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // bind socket to multicast port
    if (-1 == bind(gMulticastServerSocketFD, (struct sockaddr *) &sockAddr,
                   sizeof(sockAddr)))
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed to Bind Socket!");
        close(gMulticastServerSocketFD);
        gMulticastServerSocketFD = -1;
        u_mutex_unlock(gMutexMulticastServer);
        return CA_STATUS_FAILED;
    }

    // Add membership to receiving socket (join group)
    memset(&gMulticastMemberReq, 0, sizeof(struct ip_mreq));
    gMulticastMemberReq.imr_interface.s_addr = htonl(INADDR_ANY);
    inet_aton(multicastAddress, &gMulticastMemberReq.imr_multiaddr);

    if (-1 == setsockopt(gMulticastServerSocketFD, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                         (char *) &gMulticastMemberReq,
                         sizeof(struct ip_mreq)))
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed to add to multicast group, Error code: %s\n",
                  strerror(errno));
        close(gMulticastServerSocketFD);
        gMulticastServerSocketFD = -1;
        u_mutex_unlock(gMutexMulticastServer);
        return CA_STATUS_FAILED;
    }

    /**
      * The task to listen to data from multicastcast socket is added to the thread pool.
      * This is a blocking call is made where we try to receive some data.
      * We will keep waiting until some data is received.
      * This task will be terminated when thread pool is freed on stopping the adapters.
      */
    gStopMulticast = false;
    if (CA_STATUS_OK != u_thread_pool_add_task(gThreadPool, CAMulticastReceiveHandler, NULL))
    {
        OIC_LOG(ERROR, WIFI_SERVER_TAG, "thread_pool_add_task failed!");

        close(gMulticastServerSocketFD);
        gMulticastServerSocketFD = -1;
        gStopMulticast = true;
        u_mutex_unlock(gMutexMulticastServer);
        return CA_STATUS_FAILED;
    }

    *serverFD = gMulticastServerSocketFD;
    u_mutex_unlock(gMutexMulticastServer);

    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAWiFiStopUnicastServer()
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");

    u_mutex_lock(gMutexUnicastServer);
    if (-1 == gUnicastServerSocketFD)
    {
        OIC_LOG(INFO, WIFI_SERVER_TAG, "Unicast server is not yet started");
        u_mutex_unlock(gMutexUnicastServer);
        return CA_SERVER_NOT_STARTED;
    }

    gStopUnicast = true;

    // close the socket
    if (-1 == close(gUnicastServerSocketFD))
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed to close socket, Error code: %s\n",
                  strerror(errno));
        u_mutex_unlock(gMutexUnicastServer);
        return CA_STATUS_FAILED;
    }

    gUnicastServerSocketFD = -1;
    u_mutex_unlock(gMutexUnicastServer);

    OIC_LOG(INFO, WIFI_SERVER_TAG, "Unicast server stopped successfully");
    return CA_STATUS_OK;
}

#ifdef __WITH_DTLS__
CAResult_t CAWiFiStopSecureUnicastServer()
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");

    u_mutex_lock(gMutexSecureUnicastServer);
    if (-1 == gSecureUnicastServerSocketFD)
    {
        OIC_LOG(INFO, WIFI_SERVER_TAG, "Secure unicast server is not yet started");
        u_mutex_unlock(gMutexSecureUnicastServer);
        return CA_SERVER_NOT_STARTED;
    }

    gStopSecureUnicast = true;

    // close the socket
    if (-1 == close(gSecureUnicastServerSocketFD))
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed to close the socket, Error code: %s\n",
                  strerror(errno));
        u_mutex_unlock(gMutexSecureUnicastServer);
        return CA_STATUS_FAILED;
    }

    gSecureUnicastServerSocketFD = -1;
    u_mutex_unlock(gMutexSecureUnicastServer);

    OIC_LOG(INFO, WIFI_SERVER_TAG, "Secure unicast server stopped successfully");
    return CA_STATUS_OK;
}
#endif

CAResult_t CAWiFiStopMulticastServer(void)
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");

    u_mutex_lock(gMutexMulticastServer);

    if (gMulticastServerSocketFD == -1)
    {
        OIC_LOG(INFO, WIFI_SERVER_TAG, "Multicast server is not yet started");
        u_mutex_unlock(gMutexMulticastServer);
        return CA_SERVER_NOT_STARTED;
    }

    gStopMulticast = true;

    // leave the group after you are done
    if (-1 == setsockopt(gMulticastServerSocketFD, IPPROTO_IP, IP_DROP_MEMBERSHIP,
                         (char *)&gMulticastMemberReq,
                         sizeof(struct ip_mreq)))
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed to leave multicast group, Error code: %s\n",
                  strerror(errno));
    }

    // close the socket
    if (-1 == close(gMulticastServerSocketFD))
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed to close the socket, Error code: %s\n",
                  strerror(errno));
        u_mutex_unlock(gMutexMulticastServer);
        return CA_SOCKET_OPERATION_FAILED;
    }

    gMulticastServerSocketFD = -1;
    u_mutex_unlock(gMutexMulticastServer);

    OIC_LOG(INFO, WIFI_SERVER_TAG, "Multicast server stopped successfully");
    return CA_STATUS_OK;
}

CAResult_t CAWiFiGetUnicastServerInfo(const CABool_t isSecured, char **ipAddress,
                                      int16_t *port, int32_t *serverFD)
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");

    // Input validation
    VERIFY_NON_NULL(ipAddress, WIFI_SERVER_TAG, "IP address");
    VERIFY_NON_NULL(port, WIFI_SERVER_TAG, "Port");
    VERIFY_NON_NULL(serverFD, WIFI_SERVER_TAG, "Server ID");

    struct sockaddr_in sockAddr;
    socklen_t len = sizeof(struct sockaddr_in);
    if (-1 == getsockname(gUnicastServerSocketFD, (struct sockaddr *)&sockAddr, &len))
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed in getsockname [%s]!", strerror(errno));
        return CA_STATUS_FAILED;
    }


    const char *serverAddress = inet_ntoa(sockAddr.sin_addr);
    *ipAddress = (serverAddress) ? strndup(serverAddress, strlen(serverAddress)) : NULL;
    *port = ntohs(sockAddr.sin_port);
#ifdef __WITH_DTLS__
    *serverFD = (CA_TRUE == isSecured) ? gSecureUnicastServerSocketFD : gUnicastServerSocketFD;
#else
    *serverFD = gUnicastServerSocketFD;
#endif
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


