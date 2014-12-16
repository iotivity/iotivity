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
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

#include "pdu.h"
#include "caadapterutils.h"
#ifdef __WITH_DTLS__
#include "caadapternetdtls.h"
#endif
#include "umutex.h"

/**
 * @def ETHERNET_SERVER_TAG
 * @brief Logging tag for module name
 */
#define ETHERNET_SERVER_TAG "ETHERNET_SERVER"

/**
 * @def CA_UDP_BIND_RETRY_COUNT
 * @brief Retry count in case of socket bind failure.
 */
#define CA_UDP_BIND_RETRY_COUNT 10

/**
 * @def IPNAMESIZE
 * @brief max length for ip
 */
#define IPNAMESIZE 16

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
 * @var gMutexSecureUnicastServer
 * @brief Mutex to synchronize secure unicast server
 */
static u_mutex gMutexSecureUnicastServer = NULL;

/**
 * @var gStopSecureUnicast
 * @brief Flag to control the unicast secure data receive thread
 */
static bool gStopSecureUnicast = false;
#endif

/**
 * @var gThreadPool
 * @brief ThreadPool for storing u_thread_pool_t handle passed from adapter
 */
static u_thread_pool_t gThreadPool = NULL;

/**
 * @var gMulticastServerInterface
 * @brief Local interface on which multicast server is running
 */
static char gMulticastServerInterface[IPNAMESIZE];

/**
 * @var gMulticastMemberReq
 * @brief ip_mreq structure passed to join a multicast group
 */
static struct ip_mreq gMulticastMemberReq;

/**
 * @var gPacketReceivedCallback
 * @brief Callback for notifying the upper layer on receival data from remote OIC device
 */
static CAEthernetPacketReceivedCallback gPacketReceivedCallback = NULL;

/**
 * @var gExceptionCallback
 * @brief Callback for notifying the upper layer when unicast/multicast server encounters exception
 */
static CAEthernetExceptionCallback gExceptionCallback = NULL;

/**
 @brief Thread context information for unicast, multicast and secured unicast server
 */
typedef struct
{
    bool *stopFlag;
    int32_t socket_fd;
    CAAdapterServerType_t type;
}CAAdapterReceiveThreadContext_t;

static void CAReceiveHandler(void *data)
{
    OIC_LOG(DEBUG, ETHERNET_SERVER_TAG, "IN");

    // Input validation
    VERIFY_NON_NULL_VOID(data, ETHERNET_SERVER_TAG, "Invalid thread context");

    CAAdapterReceiveThreadContext_t *ctx = (CAAdapterReceiveThreadContext_t *)data;
    fd_set reads;
    struct timeval timeout;
    char recvBuffer[COAP_MAX_PDU_SIZE] = {0};

    while (true != *(ctx->stopFlag))
    {
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        FD_ZERO(&reads);
        FD_SET(ctx->socket_fd, &reads);

        int32_t ret = select(ctx->socket_fd + 1, &reads, NULL, NULL, &timeout);
        if (*(ctx->stopFlag) == true)
        {
            OIC_LOG_V(DEBUG, ETHERNET_SERVER_TAG, "Stop request received for [%d] server", ctx->type);
            break;
        }

        if (ret < 0)
        {
            OIC_LOG_V(FATAL, ETHERNET_SERVER_TAG, "select returned error %s", strerror(errno));
            continue;
        }

        if (!FD_ISSET(ctx->socket_fd, &reads))
        {
            continue;
        }

        memset(recvBuffer, 0, sizeof(recvBuffer));

        // Read data from socket
        struct sockaddr_in srcSockAddress;
        int32_t recvLen;
        socklen_t srcAddressLen = sizeof(srcSockAddress);
        if (-1 == (recvLen = recvfrom(ctx->socket_fd, recvBuffer,
                                      sizeof(recvBuffer), 0, (struct sockaddr *) &srcSockAddress,
                                      &srcAddressLen)))
        {
            OIC_LOG_V(DEBUG, ETHERNET_SERVER_TAG, "%s", strerror(errno));
            continue;
        }
        else if (0 == recvLen)
        {
            OIC_LOG_V(ERROR, ETHERNET_SERVER_TAG, "Server socket shutdown [%d]!", ctx->type);

            // Notify upper layer this exception
            if (gExceptionCallback)
            {
                gExceptionCallback(ctx->type);
            }
            OICFree(ctx);
            return;
        }

        const char *srcIPAddress = NULL;
        int32_t srcPort = -1;

        srcIPAddress = inet_ntoa(srcSockAddress.sin_addr);
        srcPort = ntohs(srcSockAddress.sin_port);

        OIC_LOG_V(DEBUG, ETHERNET_SERVER_TAG, "Received packet from %s:%d\n",
                  srcIPAddress, srcPort);
        OIC_LOG_V(DEBUG, ETHERNET_SERVER_TAG, "Data: %s\t, DataLength: %d\n",
                  recvBuffer, recvLen);

        char *netMask = NULL;
        if (CA_STATUS_OK != CAEthernetGetInterfaceSubnetMask(&netMask))
        {
            OIC_LOG_V(ERROR, ETHERNET_SERVER_TAG, "Failed to get ethernet subnet");
            continue;
        }

        if(!CAAdapterIsSameSubnet(gMulticastServerInterface, srcIPAddress, netMask))
        {
            OIC_LOG(DEBUG, ETHERNET_SERVER_TAG, "Packet received from different subnet, Ignore!");
            continue;
        }

        OICFree(netMask);
        switch(ctx->type)
        {
            case CA_UNICAST_SERVER:
            case CA_MULTICAST_SERVER:
                // Notify data to upper layer
                if (gPacketReceivedCallback)
                {
                    gPacketReceivedCallback(srcIPAddress, srcPort, recvBuffer, recvLen, false);
                }
                break;
#ifdef __WITH_DTLS__
            case CA_SECURED_UNICAST_SERVER:
                {
                    CAResult_t ret = CAAdapterNetDtlsDecrypt(srcIPAddress,
                                        srcPort,
                                        (uint8_t *)recvBuffer,
                                        recvLen, DTLS_ETHERNET);
                    OIC_LOG_V(DEBUG, ETHERNET_SERVER_TAG, "CAAdapterNetDtlsDecrypt returns [%d]", ret);
                }
                break;
#endif //__WITH_DTLS__
            default:
                // Should never occur
                OIC_LOG_V(DEBUG, ETHERNET_SERVER_TAG, "Invalid server type");
                OICFree(ctx);
                return;
        }
    }

    OIC_LOG(DEBUG, ETHERNET_SERVER_TAG, "OUT");
}

static CAResult_t CACreateSocket(int32_t* socketFD, const char *localIp, int16_t *port,
                                 const bool forceStart)
{
    int32_t sock = -1;
    // Create a UDP socket
    if (-1 == (sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)))
    {
        OIC_LOG_V(ERROR, ETHERNET_SERVER_TAG, "Failed to create Socket, Error code: %s",
                  strerror(errno));
        return CA_STATUS_FAILED;
    }

    // Make the socket non-blocking
    if (-1 == fcntl(sock, F_SETFL, O_NONBLOCK))
    {
        OIC_LOG_V(ERROR, ETHERNET_SERVER_TAG, "Failed to set non-block mode, Error code: %s",
                  strerror(errno));

        close(sock);
        return CA_STATUS_FAILED;
    }

    if (true == forceStart)
    {
        int32_t setOptionOn = 1;
        if (-1 == setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
                             (char *) &setOptionOn,
                             sizeof(setOptionOn)))
        {
            OIC_LOG_V(ERROR, ETHERNET_SERVER_TAG, "Failed to set SO_REUSEADDR! Error code: %s",
                      strerror(errno));

            close(sock);
            return CA_STATUS_FAILED;
        }
    }

    struct sockaddr_in sockAddr;
    bool isBound = false;
    int16_t serverPort = *port;

    memset((char *) &sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(serverPort);
    if(localIp)
    {
        sockAddr.sin_addr.s_addr = inet_addr(localIp);
    }
    else
    {
        sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    }

    int16_t i;
    for (i = 0; i < CA_UDP_BIND_RETRY_COUNT; i++)
    {
        if (-1 == bind(sock, (struct sockaddr *) &sockAddr,
                       sizeof(sockAddr)))
        {
            if (false == forceStart)
            {
                OIC_LOG_V(ERROR, ETHERNET_SERVER_TAG, "Failed to bind socket[%s]. Trying again..",
                          strerror(errno));

                //Set the port to next one
                serverPort += 1;
                sockAddr.sin_port = htons(serverPort);
                continue;
            }
            else
            {
                OIC_LOG_V(ERROR, ETHERNET_SERVER_TAG, "Failed to bind socket[%s]!",
                          strerror(errno));
                break;
            }
        }

        isBound = true;
        break;
    }

    if (false == isBound)
    {
        close(sock);
        return CA_STATUS_FAILED;
    }

    *port = serverPort;
    *socketFD = sock;
    return CA_STATUS_OK;
}

static CAResult_t CACloseSocket(int32_t *socketFD)
{
    if (-1 == *socketFD)
    {
        OIC_LOG(INFO, ETHERNET_SERVER_TAG, "Server not running");
        return CA_SERVER_NOT_STARTED;
    }

    // close the socket
    if (-1 == close(*socketFD))
    {
        OIC_LOG_V(ERROR, ETHERNET_SERVER_TAG, "Failed to close the socket, Error code: %s\n",
                  strerror(errno));
        return CA_STATUS_FAILED;
    }

    *socketFD = -1;
    return CA_STATUS_OK;
}

static CAResult_t CAStartUnicastServer(const char *localAddress, int16_t *port,
                                       const bool forceStart, bool isSecured, int32_t *serverFD)
{
    OIC_LOG(DEBUG, ETHERNET_SERVER_TAG, "IN");

    CAResult_t ret = CACreateSocket(serverFD, localAddress, port, forceStart);
    if(CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, ETHERNET_SERVER_TAG, "Failed to create unicast socket");
        return ret;
    }

    /**
      * The task to listen for data from unicast socket is added to the thread pool.
      * This is a blocking call is made where we try to receive some data..
      * We will keep waiting until some data is received.
      * This task will be terminated when thread pool is freed on stopping the adapters.
      * Thread context will be freed by thread on exit.
      */
    CAAdapterReceiveThreadContext_t *ctx = (CAAdapterReceiveThreadContext_t *)
                                            OICMalloc(sizeof(CAAdapterReceiveThreadContext_t));
    if(!ctx)
    {
        OIC_LOG(ERROR, ETHERNET_SERVER_TAG, "Out of memory!");
        close(*serverFD);
        return CA_MEMORY_ALLOC_FAILED;
    }

    ctx->stopFlag = &gStopUnicast;
    ctx->socket_fd = *serverFD;
    ctx->type = isSecured ? CA_SECURED_UNICAST_SERVER:CA_UNICAST_SERVER;
    if (CA_STATUS_OK != u_thread_pool_add_task(gThreadPool, CAReceiveHandler, (void *)ctx))
    {
        OIC_LOG(ERROR, ETHERNET_SERVER_TAG, "Failed to create read thread!");
        OICFree((void *)ctx);
        close(*serverFD);
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, ETHERNET_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}

static void CAEthernetServerDestroyMutex(void)
{
    OIC_LOG(DEBUG, ETHERNET_SERVER_TAG, "IN");

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

    OIC_LOG(DEBUG, ETHERNET_SERVER_TAG, "OUT");
}

static CAResult_t CAEthernetServerCreateMutex(void)
{
    OIC_LOG(DEBUG, ETHERNET_SERVER_TAG, "IN");

    gMutexUnicastServer = u_mutex_new();
    if (!gMutexUnicastServer)
    {
        OIC_LOG(ERROR, ETHERNET_SERVER_TAG, "Failed to created mutex!");
        return CA_STATUS_FAILED;
    }

#ifdef __WITH_DTLS__
    gMutexSecureUnicastServer = u_mutex_new();
    if (!gMutexSecureUnicastServer)
    {
        OIC_LOG(ERROR, ETHERNET_SERVER_TAG, "Failed to created mutex!");

        CAEthernetServerDestroyMutex();
        return CA_STATUS_FAILED;
    }
#endif

    gMutexMulticastServer = u_mutex_new();
    if (!gMutexMulticastServer)
    {
        OIC_LOG(ERROR, ETHERNET_SERVER_TAG, "Failed to created mutex!");

        CAEthernetServerDestroyMutex();
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, ETHERNET_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAEthernetInitializeServer(const u_thread_pool_t threadPool)
{
    OIC_LOG(DEBUG, ETHERNET_SERVER_TAG, "IN");

    // Input validation
    VERIFY_NON_NULL(threadPool, ETHERNET_SERVER_TAG, "Thread pool handle is NULL");

    // Initialize mutex
    if (CA_STATUS_OK != CAEthernetServerCreateMutex())
    {
        OIC_LOG(ERROR, ETHERNET_SERVER_TAG, "Failed to create mutex!");
        return CA_STATUS_FAILED;
    }

    gThreadPool = threadPool;

    OIC_LOG(DEBUG, ETHERNET_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CAEthernetTerminateServer(void)
{
    OIC_LOG(DEBUG, ETHERNET_SERVER_TAG, "IN");

    gThreadPool = NULL;

    // Destroy mutex
    CAEthernetServerDestroyMutex();

    OIC_LOG(DEBUG, ETHERNET_SERVER_TAG, "OUT");
}

CAResult_t CAEthernetStartUnicastServer(const char *localAddress, int16_t *port,
                                        const bool forceStart, const bool isSecured,
                                        int32_t *serverFD)
{
    OIC_LOG(DEBUG, ETHERNET_SERVER_TAG, "IN");

    // Input validation
    VERIFY_NON_NULL(localAddress, ETHERNET_SERVER_TAG, "localAddress");
    VERIFY_NON_NULL(port, ETHERNET_SERVER_TAG, "port");
    VERIFY_NON_NULL(serverFD, ETHERNET_SERVER_TAG, "server socket FD");

    if (0 >= *port)
    {
        OIC_LOG_V(ERROR, ETHERNET_SERVER_TAG, "Invalid input: port is invalid!");
        return CA_STATUS_INVALID_PARAM;
    }

    *serverFD = -1;
    if (CA_FALSE == isSecured)
    {
        u_mutex_lock(gMutexUnicastServer);
        if (-1 != gUnicastServerSocketFD)
        {
            OIC_LOG_V(ERROR, ETHERNET_SERVER_TAG, "Unicast Server is Started Already!",
                      CA_SERVER_STARTED_ALREADY);

            *serverFD = gUnicastServerSocketFD;
            u_mutex_unlock(gMutexUnicastServer);
            return CA_SERVER_STARTED_ALREADY;
        }

        gStopUnicast = false;
        if (CA_STATUS_OK != CAStartUnicastServer(localAddress, port, forceStart, isSecured,
                                                 &gUnicastServerSocketFD))
        {
            OIC_LOG_V(ERROR, ETHERNET_SERVER_TAG, "Failed to start unicast server!");
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
            OIC_LOG_V(ERROR, ETHERNET_SERVER_TAG, "Unicast Server is Started Already!",
                      CA_SERVER_STARTED_ALREADY);

            *serverFD = gSecureUnicastServerSocketFD;
            u_mutex_unlock(gMutexSecureUnicastServer);
            return CA_SERVER_STARTED_ALREADY;
        }

        gStopSecureUnicast = false;
        if (CA_STATUS_OK != CAStartUnicastServer(localAddress, port, forceStart, isSecured,
                                                 &gSecureUnicastServerSocketFD))
        {
            OIC_LOG_V(ERROR, ETHERNET_SERVER_TAG, "Failed to start unicast server!");
            gSecureUnicastServerSocketFD = -1;
            u_mutex_unlock(gMutexSecureUnicastServer);
            return CA_STATUS_FAILED;
        }

        *serverFD = gSecureUnicastServerSocketFD;
        u_mutex_unlock(gMutexSecureUnicastServer);
    }
#endif
    OIC_LOG(DEBUG, ETHERNET_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAEthernetStartMulticastServer(const char *localAddress, const char *multicastAddress,
        const int16_t multicastPort, int32_t *serverFD)
{
    OIC_LOG(DEBUG, ETHERNET_SERVER_TAG, "IN");

    // Input validation
    VERIFY_NON_NULL(localAddress, ETHERNET_SERVER_TAG, "Local address is NULL");
    VERIFY_NON_NULL(multicastAddress, ETHERNET_SERVER_TAG, "Multicast address is NULL");

    int16_t port = multicastPort;
    if (0 >= port)
    {
        OIC_LOG_V(ERROR, ETHERNET_SERVER_TAG, "Invalid input: Multicast port is invalid!");
        return CA_STATUS_INVALID_PARAM;
    }

    u_mutex_lock(gMutexMulticastServer);

    if (gMulticastServerSocketFD != -1)
    {
        OIC_LOG_V(ERROR, ETHERNET_SERVER_TAG, "Multicast Server is already running!");
        u_mutex_unlock(gMutexMulticastServer);
        return CA_SERVER_STARTED_ALREADY;
    }

    CAResult_t ret = CACreateSocket(&gMulticastServerSocketFD, multicastAddress, &port, true);
    if(ret != CA_STATUS_OK)
    {
        OIC_LOG(ERROR, ETHERNET_SERVER_TAG, "Failed to create multicast socket");
        u_mutex_unlock(gMutexMulticastServer);
        return ret;
    }

    // Add membership to receiving socket (join group)
    memset(&gMulticastMemberReq, 0, sizeof(struct ip_mreq));
    gMulticastMemberReq.imr_interface.s_addr = inet_addr(localAddress);
    inet_aton(multicastAddress, &gMulticastMemberReq.imr_multiaddr);

    if (-1 == setsockopt(gMulticastServerSocketFD, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                         (char *) &gMulticastMemberReq,
                         sizeof(struct ip_mreq)))
    {
        OIC_LOG_V(ERROR, ETHERNET_SERVER_TAG, "Failed to add to multicast group, Error code: %s\n",
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
      * Thread context will be freed by thread on exit.
      */
    CAAdapterReceiveThreadContext_t *ctx = (CAAdapterReceiveThreadContext_t *)
                                            OICMalloc(sizeof(CAAdapterReceiveThreadContext_t));
    if(!ctx)
    {
        OIC_LOG(ERROR, ETHERNET_SERVER_TAG, "Out of memory!");
        close(gMulticastServerSocketFD);
        gMulticastServerSocketFD = -1;
        return CA_MEMORY_ALLOC_FAILED;
    }

    ctx->stopFlag = &gStopMulticast;
    ctx->socket_fd = gMulticastServerSocketFD;
    ctx->type = CA_MULTICAST_SERVER;

    gStopMulticast = false;
    if (CA_STATUS_OK != u_thread_pool_add_task(gThreadPool, CAReceiveHandler, (void *)ctx))
    {
        OIC_LOG(ERROR, ETHERNET_SERVER_TAG, "thread_pool_add_task failed!");

        close(gMulticastServerSocketFD);
        gMulticastServerSocketFD = -1;
        gStopMulticast = true;
        u_mutex_unlock(gMutexMulticastServer);
        return CA_STATUS_FAILED;
    }

    *serverFD = gMulticastServerSocketFD;
    strncpy(gMulticastServerInterface, localAddress, sizeof(gMulticastServerInterface));
    u_mutex_unlock(gMutexMulticastServer);

    OIC_LOG(DEBUG, ETHERNET_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAEthernetStopUnicastServer()
{
    OIC_LOG(DEBUG, ETHERNET_SERVER_TAG, "IN");

    u_mutex_lock(gMutexUnicastServer);
    gStopUnicast = true;
    CAResult_t ret = CACloseSocket(&gUnicastServerSocketFD);
    u_mutex_unlock(gMutexUnicastServer);

    OIC_LOG_V(INFO, ETHERNET_SERVER_TAG, "Unicast server stopped [%d]", ret);
    return ret;
}

#ifdef __WITH_DTLS__
CAResult_t CAEthernetStopSecureUnicastServer()
{
    OIC_LOG(DEBUG, ETHERNET_SERVER_TAG, "IN");

    u_mutex_lock(gMutexSecureUnicastServer);
    gStopSecureUnicast = true;
    CAResult_t ret = CACloseSocket(&gSecureUnicastServerSocketFD);
    u_mutex_unlock(gMutexSecureUnicastServer);

    OIC_LOG_V(INFO, ETHERNET_SERVER_TAG, "Secured unicast server stopped [%d]", ret);
    return ret;
}
#endif

CAResult_t CAEthernetStopMulticastServer(void)
{
    OIC_LOG(DEBUG, ETHERNET_SERVER_TAG, "IN");

    u_mutex_lock(gMutexMulticastServer);

    if (gMulticastServerSocketFD == -1)
    {
        OIC_LOG(INFO, ETHERNET_SERVER_TAG, "Multicast server is not yet started");
        u_mutex_unlock(gMutexMulticastServer);
        return CA_SERVER_NOT_STARTED;
    }

    gStopMulticast = true;

    // leave the group after you are done
    if (-1 == setsockopt(gMulticastServerSocketFD, IPPROTO_IP, IP_DROP_MEMBERSHIP,
                         (char *)&gMulticastMemberReq,
                         sizeof(struct ip_mreq)))
    {
        OIC_LOG_V(ERROR, ETHERNET_SERVER_TAG, "Failed to leave multicast group, Error code: %s\n",
                  strerror(errno));
    }

    CAResult_t ret = CACloseSocket(&gMulticastServerSocketFD);
    u_mutex_unlock(gMutexMulticastServer);

    OIC_LOG_V(INFO, ETHERNET_SERVER_TAG, "Multicast server stopped [%d]", ret);
    return ret;
}

CAResult_t CAEthernetGetUnicastServerInfo(const bool isSecured, char **ipAddress, int16_t *port,
                                          int32_t *serverFD)
{
    OIC_LOG(DEBUG, ETHERNET_SERVER_TAG, "IN");

    // Input validation
    VERIFY_NON_NULL(ipAddress, ETHERNET_SERVER_TAG, "IP address");
    VERIFY_NON_NULL(port, ETHERNET_SERVER_TAG, "Port");
    VERIFY_NON_NULL(serverFD, ETHERNET_SERVER_TAG, "Server ID");

    struct sockaddr_in sockAddr;
    socklen_t len = sizeof(struct sockaddr_in);
    if (-1 == getsockname(gUnicastServerSocketFD, (struct sockaddr *)&sockAddr, &len))
    {
        OIC_LOG_V(ERROR, ETHERNET_SERVER_TAG, "Failed in getsockname [%s]!", strerror(errno));
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

    OIC_LOG(DEBUG, ETHERNET_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CAEthernetSetPacketReceiveCallback(CAEthernetPacketReceivedCallback callback)
{
    OIC_LOG(DEBUG, ETHERNET_SERVER_TAG, "IN");
    gPacketReceivedCallback = callback;
}

void CAEthernetSetExceptionCallback(CAEthernetExceptionCallback callback)
{
    OIC_LOG(DEBUG, ETHERNET_SERVER_TAG, "IN");
    gExceptionCallback = callback;
}
