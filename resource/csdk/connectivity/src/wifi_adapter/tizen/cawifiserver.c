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
#ifdef __WITH_DTLS__
#include "caadapternetdtls.h"
#endif
#include "camutex.h"
#include "oic_string.h"
#include "oic_malloc.h"

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
 * @def IPNAMESIZE
 * @brief max length for ip
 */
#define IPNAMESIZE 16

/**
 * @var g_unicastServerSocketFD
 * @brief Unicast server socket descriptor
 */
static int g_unicastServerSocketFD = -1;

/**
 * @var g_mutexUnicastServer
 * @brief Mutex to synchronize unicast server
 */
static ca_mutex g_mutexUnicastServer = NULL;

/**
 * @var g_stopUnicast
 * @brief Flag to control the Receive Unicast Data Thread
 */
static bool g_stopUnicast = false;

/**
 * @var g_multicastServerSocketFD
 * @brief socket descriptor for multicast server
 */
static int g_multicastServerSocketFD = -1;

/**
 * @var g_mutexMulticastServer
 * @brief Mutex to synchronize secure multicast server
 */
static ca_mutex g_mutexMulticastServer = NULL;

/**
 * @var g_stopMulticast
 * @brief Flag to control the Receive Multicast Data Thread
 */
static bool g_stopMulticast = false;

#ifdef __WITH_DTLS__
/**
 * @var g_secureUnicastServerSocketFD
 * @brief Secure unicast server socket descriptor
 */
static int g_secureUnicastServerSocketFD = -1;

/**
 * @var g_mutexSecureUnicastServer
 * @brief Mutex to synchronize secure unicast server
 */
static ca_mutex g_mutexSecureUnicastServer = NULL;

/**
 * @var g_stopSecureUnicast
 * @brief Flag to control the unicast secure data receive thread
 */
static bool g_stopSecureUnicast = false;
#endif

/**
 * @var g_threadPool
 * @brief ThreadPool for storing ca_thread_pool_t handle passed from adapter
 */
static ca_thread_pool_t g_threadPool = NULL;

/**
 * @var g_multicastServerInterface
 * @brief Local interface on which multicast server is running
 */
static char g_multicastServerInterface[IPNAMESIZE];

/**
 * @var g_multicastMemberReq
 * @brief ip_mreq structure passed to join a multicast group
 */
static struct ip_mreq g_multicastMemberReq;

/**
 * @var g_packetReceivedCallback
 * @brief Callback for notifying the upper layer on receival data from remote OIC device
 */
static CAWiFiPacketReceivedCallback g_packetReceivedCallback = NULL;

/**
 * @var g_exceptionCallback
 * @brief Callback for notifying the upper layer when unicast/multicast server encounters exception
 */
static CAWiFiExceptionCallback g_exceptionCallback = NULL;

/**
 @brief Thread context information for unicast, multicast and secured unicast server
 */
typedef struct
{
    bool *stopFlag;
    int socket_fd;
    CAAdapterServerType_t type;
} CAAdapterReceiveThreadContext_t;

static void CAReceiveHandler(void *data)
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");
    // Input validation
    VERIFY_NON_NULL_VOID(data, WIFI_SERVER_TAG, "Invalid thread context");

    CAAdapterReceiveThreadContext_t *ctx = (CAAdapterReceiveThreadContext_t *)data;


    char recvBuffer[COAP_MAX_PDU_SIZE] = {0};

    while (true != *(ctx->stopFlag))
    {
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        fd_set reads;
        FD_ZERO(&reads);
        FD_SET(ctx->socket_fd, &reads);

        int ret = select(ctx->socket_fd + 1, &reads, NULL, NULL, &timeout);
        if (*(ctx->stopFlag) == true)
        {
            OIC_LOG_V(DEBUG, WIFI_SERVER_TAG, "Stop request received for [%d] server", ctx->type);
            break;
        }
        if (ret < 0)
        {
            OIC_LOG_V(FATAL, WIFI_SERVER_TAG, "select returned error %s", strerror(errno));
            continue;
        }
        if (!FD_ISSET(ctx->socket_fd, &reads))
        {
            continue;
        }

        // Read data from socket
        struct sockaddr_in srcSockAddress = {0};
        int recvLen = 0;
        socklen_t srcAddressLen = sizeof(srcSockAddress);
        if (-1 == (recvLen = recvfrom(ctx->socket_fd, recvBuffer,
                                      sizeof(recvBuffer), 0, (struct sockaddr *) &srcSockAddress,
                                      &srcAddressLen)))
        {
            OIC_LOG_V(DEBUG, WIFI_SERVER_TAG, "%s", strerror(errno));
            continue;
        }
        else if (0 == recvLen)
        {
            OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Server socket shutdown [%d]!", ctx->type);

            // Notify upper layer this exception
            if (g_exceptionCallback)
            {
                g_exceptionCallback(ctx->type);
            }
            OICFree(ctx);
            return;
        }

        const char *srcIPAddress = NULL;
        uint16_t srcPort = 0;

        srcIPAddress = inet_ntoa(srcSockAddress.sin_addr);
        srcPort = ntohs(srcSockAddress.sin_port);

        OIC_LOG_V(DEBUG, WIFI_SERVER_TAG, "Received packet from %s:%d\n",
                  srcIPAddress, srcPort);
        OIC_LOG_V(DEBUG, WIFI_SERVER_TAG, "Data: %s\t, DataLength: %d\n",
                  recvBuffer, recvLen);

        char *netMask = NULL;
        if (CA_STATUS_OK != CAWiFiGetInterfaceSubnetMask(&netMask))
        {
            OIC_LOG(ERROR, WIFI_SERVER_TAG, "Failed to get ethernet subnet");
            continue;
        }

        if (!CAAdapterIsSameSubnet(g_multicastServerInterface, srcIPAddress, netMask))
        {
            OIC_LOG(DEBUG, WIFI_SERVER_TAG, "Packet received from different subnet, Ignore!");
            if (NULL != netMask)
            {
                OICFree(netMask);
            }
            continue;
        }

        OICFree(netMask);
        switch (ctx->type)
        {
            case CA_UNICAST_SERVER:
            case CA_MULTICAST_SERVER:
                // Notify data to upper layer
                if (g_packetReceivedCallback)
                {
                    g_packetReceivedCallback(srcIPAddress, srcPort, recvBuffer, recvLen, false);
                }
                break;
#ifdef __WITH_DTLS__
            case CA_SECURED_UNICAST_SERVER:
                {
                    CAResult_t ret = CAAdapterNetDtlsDecrypt(srcIPAddress,
                                     srcPort,
                                     (uint8_t *)recvBuffer,
                                     recvLen, DTLS_WIFI);
                    OIC_LOG_V(DEBUG, WIFI_SERVER_TAG, "CAAdapterNetDtlsDecrypt returns [%d]", ret);
                }
                break;
#endif //__WITH_DTLS__
            default:
                // Should never occur
                OIC_LOG(DEBUG, WIFI_SERVER_TAG, "Invalid server type");
                OICFree(ctx);
                return;
        }
    }

    // free context
    OICFree(ctx);

    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "OUT");
}

static CAResult_t CAWiFiCreateSocket(int *socketFD, const char *localIp, uint16_t *port,
                                      bool forceStart)
{
    VERIFY_NON_NULL(socketFD, WIFI_SERVER_TAG, "socketFD is NULL");
    VERIFY_NON_NULL(port, WIFI_SERVER_TAG, "port is NULL");
    VERIFY_NON_NULL(localIp, WIFI_SERVER_TAG, "localIp is NULL");

    int sock = -1;
    // Create a UDP socket
    if (-1 == (sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)))
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed to create Socket, Error code: %s",
                  strerror(errno));
        return CA_STATUS_FAILED;
    }

    // Make the socket non-blocking
    if (-1 == fcntl(sock, F_SETFL, O_NONBLOCK))
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed to set non-block mode, Error code: %s",
                  strerror(errno));

        close(sock);
        return CA_STATUS_FAILED;
    }

    if (true == forceStart)
    {
        int setOptionOn = 1;
        if (-1 == setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
                             (char *) &setOptionOn,
                             sizeof(setOptionOn)))
        {
            OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed to set SO_REUSEADDR! Error code: %s",
                      strerror(errno));

            close(sock);
            return CA_STATUS_FAILED;
        }
    }

    struct sockaddr_in sockAddr = {0};
    bool isBound = false;
    uint16_t serverPort = *port;

    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(serverPort);
    if (localIp)
    {
        sockAddr.sin_addr.s_addr = inet_addr(localIp);
    }
    else
    {
        sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    }

    int16_t i = 0;
    for (i = 0; i < CA_UDP_BIND_RETRY_COUNT; i++)
    {
        if (-1 == bind(sock, (struct sockaddr *) &sockAddr,
                       sizeof(sockAddr)))
        {
            if (false == forceStart)
            {
                OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed to bind socket[%s]. Trying again..",
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
        close(sock);
        return CA_STATUS_FAILED;
    }

    *port = serverPort;
    *socketFD = sock;
    return CA_STATUS_OK;
}

static CAResult_t CAWiFiCloseSocket(int *socketFD)
{
    VERIFY_NON_NULL(socketFD, WIFI_SERVER_TAG, "socketFD is NULL");

    if (-1 == *socketFD)
    {
        OIC_LOG(INFO, WIFI_SERVER_TAG, "Server not running");
        return CA_SERVER_NOT_STARTED;
    }

    // close the socket
    if (-1 == close(*socketFD))
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed to close the socket, Error code: %s\n",
                  strerror(errno));
        return CA_STATUS_FAILED;
    }

    *socketFD = -1;
    return CA_STATUS_OK;
}

static CAResult_t CAStartUnicastServer(const char *localAddress, uint16_t *port,
                                        bool forceStart, bool isSecured, int *serverFD)
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");

    VERIFY_NON_NULL(localAddress, WIFI_SERVER_TAG, "localAddress is NULL");
    VERIFY_NON_NULL(serverFD, WIFI_SERVER_TAG, "socketFD is NULL");
    VERIFY_NON_NULL(port, WIFI_SERVER_TAG, "port is NULL");

    CAResult_t ret = CAWiFiCreateSocket(serverFD, localAddress, port, forceStart);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, WIFI_SERVER_TAG, "Failed to create unicast socket");
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
    if (!ctx)
    {
        OIC_LOG(ERROR, WIFI_SERVER_TAG, "Out of memory!");
        close(*serverFD);
        return CA_MEMORY_ALLOC_FAILED;
    }

    ctx->stopFlag = &g_stopUnicast;
    ctx->socket_fd = *serverFD;
    ctx->type = isSecured ? CA_SECURED_UNICAST_SERVER : CA_UNICAST_SERVER;
    if (CA_STATUS_OK != ca_thread_pool_add_task(g_threadPool, CAReceiveHandler, (void *)ctx))
    {
        OIC_LOG(ERROR, WIFI_SERVER_TAG, "Failed to create read thread!");
        OICFree(ctx);
        close(*serverFD);
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}

static void CAWiFiServerDestroyMutex(void)
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");

    if (g_mutexUnicastServer)
    {
        ca_mutex_free(g_mutexUnicastServer);
        g_mutexUnicastServer = NULL;
    }

#ifdef __WITH_DTLS__
    if (g_mutexSecureUnicastServer)
    {
        ca_mutex_free(g_mutexSecureUnicastServer);
        g_mutexSecureUnicastServer = NULL;
    }
#endif

    if (g_mutexMulticastServer)
    {
        ca_mutex_free(g_mutexMulticastServer);
        g_mutexMulticastServer = NULL;
    }

    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "OUT");
}

static CAResult_t CAWiFiServerCreateMutex(void)
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");

    if(!g_mutexUnicastServer)
    {
        g_mutexUnicastServer = ca_mutex_new();
        if (!g_mutexUnicastServer)
        {
            OIC_LOG(ERROR, WIFI_SERVER_TAG, "Failed to created mutex!");
            return CA_STATUS_FAILED;
        }
    }

#ifdef __WITH_DTLS__
    if(!g_mutexSecureUnicastServer)
    {
        g_mutexSecureUnicastServer = ca_mutex_new();
        if (!g_mutexSecureUnicastServer)
        {
            OIC_LOG(ERROR, WIFI_SERVER_TAG, "Failed to created mutex!");

            CAWiFiServerDestroyMutex();
            return CA_STATUS_FAILED;
        }
    }
#endif

    if(!g_mutexMulticastServer)
    {
        g_mutexMulticastServer = ca_mutex_new();
        if (!g_mutexMulticastServer)
        {
            OIC_LOG(ERROR, WIFI_SERVER_TAG, "Failed to created mutex!");

            CAWiFiServerDestroyMutex();
            return CA_STATUS_FAILED;
        }
    }
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAWiFiInitializeServer(const ca_thread_pool_t threadPool)
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

    g_threadPool = threadPool;

    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CAWiFiTerminateServer(void)
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");

    g_threadPool = NULL;

    // Destroy mutex
    CAWiFiServerDestroyMutex();

    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "OUT");
}

CAResult_t CAWiFiStartUnicastServer(const char *localAddress, uint16_t *port,
                                    bool forceStart, bool isSecured,
                                    int *serverFD)
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");

    // Input validation
    VERIFY_NON_NULL(localAddress, WIFI_SERVER_TAG, "localAddress");
    VERIFY_NON_NULL(port, WIFI_SERVER_TAG, "port");
    VERIFY_NON_NULL(serverFD, WIFI_SERVER_TAG, "server socket FD");

    if (0 >= *port)
    {
        OIC_LOG(ERROR, WIFI_SERVER_TAG, "Invalid input: port is invalid!");
        return CA_STATUS_INVALID_PARAM;
    }

    *serverFD = -1;
    if (false == isSecured)
    {
        ca_mutex_lock(g_mutexUnicastServer);
        if (-1 != g_unicastServerSocketFD)
        {
            OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Unicast Server is Started Already!",
                      CA_SERVER_STARTED_ALREADY);

            *serverFD = g_unicastServerSocketFD;
            ca_mutex_unlock(g_mutexUnicastServer);
            return CA_SERVER_STARTED_ALREADY;
        }

        g_stopUnicast = false;
        if (CA_STATUS_OK != CAStartUnicastServer(localAddress, port, forceStart, isSecured,
                &g_unicastServerSocketFD))
        {
            OIC_LOG(ERROR, WIFI_SERVER_TAG, "Failed to start unicast server!");
            g_unicastServerSocketFD = -1;
            ca_mutex_unlock(g_mutexUnicastServer);
            return CA_STATUS_FAILED;
        }

        *serverFD = g_unicastServerSocketFD;
        ca_mutex_unlock(g_mutexUnicastServer);
    }
#ifdef __WITH_DTLS__
    else // Start unicast server for secured communication
    {
        ca_mutex_lock(g_mutexSecureUnicastServer);
        if (-1 != g_secureUnicastServerSocketFD)
        {
            OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Unicast Server is Started Already!",
                      CA_SERVER_STARTED_ALREADY);

            *serverFD = g_secureUnicastServerSocketFD;
            ca_mutex_unlock(g_mutexSecureUnicastServer);
            return CA_SERVER_STARTED_ALREADY;
        }

        g_stopSecureUnicast = false;
        if (CA_STATUS_OK != CAStartUnicastServer(localAddress, port, forceStart, isSecured,
                &g_secureUnicastServerSocketFD))
        {
            OIC_LOG(ERROR, WIFI_SERVER_TAG, "Failed to start unicast server!");
            g_secureUnicastServerSocketFD = -1;
            ca_mutex_unlock(g_mutexSecureUnicastServer);
            return CA_STATUS_FAILED;
        }

        *serverFD = g_secureUnicastServerSocketFD;
        ca_mutex_unlock(g_mutexSecureUnicastServer);
    }
#endif
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAWiFiStartMulticastServer(const char *localAddress, const char *multicastAddress,
                                       uint16_t multicastPort, int *serverFD)
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");

    // Input validation
    VERIFY_NON_NULL(localAddress, WIFI_SERVER_TAG, "localAddress");
    VERIFY_NON_NULL(multicastAddress, WIFI_SERVER_TAG, "port");
    VERIFY_NON_NULL(serverFD, WIFI_SERVER_TAG, "server socket FD");

    uint16_t port = multicastPort;
    if (0 >= port)
    {
        OIC_LOG(ERROR, WIFI_SERVER_TAG, "Invalid input: Multicast port is invalid!");
        return CA_STATUS_INVALID_PARAM;
    }

    ca_mutex_lock(g_mutexMulticastServer);

    if (g_multicastServerSocketFD != -1)
    {
        OIC_LOG(ERROR, WIFI_SERVER_TAG, "Multicast Server is already running!");
        ca_mutex_unlock(g_mutexMulticastServer);
        return CA_SERVER_STARTED_ALREADY;
    }

    CAResult_t ret = CAWiFiCreateSocket(&g_multicastServerSocketFD, multicastAddress, &port, true);
    if (ret != CA_STATUS_OK)
    {
        OIC_LOG(ERROR, WIFI_SERVER_TAG, "Failed to create multicast socket");
        ca_mutex_unlock(g_mutexMulticastServer);
        return ret;
    }

    // Add membership to receiving socket (join group)
    memset(&g_multicastMemberReq, 0, sizeof(struct ip_mreq));
    g_multicastMemberReq.imr_interface.s_addr = inet_addr(localAddress);
    inet_aton(multicastAddress, &g_multicastMemberReq.imr_multiaddr);

    if (-1 == setsockopt(g_multicastServerSocketFD, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                         (char *) &g_multicastMemberReq,
                         sizeof(struct ip_mreq)))
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed to add to multicast group, Error code: %s\n",
                  strerror(errno));
        close(g_multicastServerSocketFD);
        g_multicastServerSocketFD = -1;
        ca_mutex_unlock(g_mutexMulticastServer);
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
    if (!ctx)
    {
        OIC_LOG(ERROR, WIFI_SERVER_TAG, "Out of memory!");
        close(g_multicastServerSocketFD);
        g_multicastServerSocketFD = -1;
        return CA_MEMORY_ALLOC_FAILED;
    }

    ctx->stopFlag = &g_stopMulticast;
    ctx->socket_fd = g_multicastServerSocketFD;
    ctx->type = CA_MULTICAST_SERVER;

    g_stopMulticast = false;
    if (CA_STATUS_OK != ca_thread_pool_add_task(g_threadPool, CAReceiveHandler, (void *)ctx))
    {
        OIC_LOG(ERROR, WIFI_SERVER_TAG, "thread_pool_add_task failed!");

        close(g_multicastServerSocketFD);
        g_multicastServerSocketFD = -1;
        g_stopMulticast = true;
        ca_mutex_unlock(g_mutexMulticastServer);
        return CA_STATUS_FAILED;
    }

    *serverFD = g_multicastServerSocketFD;
    strncpy(g_multicastServerInterface, localAddress, sizeof(g_multicastServerInterface));
    ca_mutex_unlock(g_mutexMulticastServer);

    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAWiFiStopUnicastServer()
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");

    ca_mutex_lock(g_mutexUnicastServer);
    g_stopUnicast = true;
    CAResult_t ret = CAWiFiCloseSocket(&g_unicastServerSocketFD);
    ca_mutex_unlock(g_mutexUnicastServer);

    OIC_LOG_V(INFO, WIFI_SERVER_TAG, "Unicast server stopped [%d]", ret);
    return ret;
}

#ifdef __WITH_DTLS__
CAResult_t CAWiFiStopSecureUnicastServer()
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");

    ca_mutex_lock(g_mutexSecureUnicastServer);
    g_stopSecureUnicast = true;
    CAResult_t ret = CAWiFiCloseSocket(&g_secureUnicastServerSocketFD);
    ca_mutex_unlock(g_mutexSecureUnicastServer);

    OIC_LOG_V(INFO, WIFI_SERVER_TAG, "OUT Secured unicast server stopped [%d]", ret);
    return ret;
}
#endif

CAResult_t CAWiFiStopMulticastServer(void)
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");

    ca_mutex_lock(g_mutexMulticastServer);

    if (g_multicastServerSocketFD == -1)
    {
        OIC_LOG(INFO, WIFI_SERVER_TAG, "Multicast server is not yet started");
        ca_mutex_unlock(g_mutexMulticastServer);
        return CA_SERVER_NOT_STARTED;
    }

    g_stopMulticast = true;

    // leave the group after you are done
    if (-1 == setsockopt(g_multicastServerSocketFD, IPPROTO_IP, IP_DROP_MEMBERSHIP,
                         (char *)&g_multicastMemberReq,
                         sizeof(struct ip_mreq)))
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed to leave multicast group, Error code: %s\n",
                  strerror(errno));
    }

    CAResult_t ret = CAWiFiCloseSocket(&g_multicastServerSocketFD);
    ca_mutex_unlock(g_mutexMulticastServer);

    OIC_LOG_V(INFO, WIFI_SERVER_TAG, " OUT Multicast server stopped [%d]", ret);
    return ret;
}

CAResult_t CAWiFiGetUnicastServerInfo(bool isSecured, char **ipAddress,
                                    uint16_t *port, int *serverFD)
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");

    // Input validation
    VERIFY_NON_NULL(ipAddress, WIFI_SERVER_TAG, "IP address");
    VERIFY_NON_NULL(port, WIFI_SERVER_TAG, "Port");
    VERIFY_NON_NULL(serverFD, WIFI_SERVER_TAG, "Server ID");

    struct sockaddr_in sockAddr = {0};
    socklen_t len = sizeof(struct sockaddr_in);
    if (-1 == getsockname(g_unicastServerSocketFD, (struct sockaddr *)&sockAddr, &len))
    {
        OIC_LOG_V(ERROR, WIFI_SERVER_TAG, "Failed in getsockname [%s]!", strerror(errno));
        return CA_STATUS_FAILED;
    }

    const char *serverAddress = inet_ntoa(sockAddr.sin_addr);
    *ipAddress = (serverAddress) ? OICStrdup(serverAddress) : NULL;
    *port = ntohs(sockAddr.sin_port);
#ifdef __WITH_DTLS__
    *serverFD = (true == isSecured) ? g_secureUnicastServerSocketFD : g_unicastServerSocketFD;
#else
    *serverFD = g_unicastServerSocketFD;
#endif
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CAWiFiSetPacketReceiveCallback(CAWiFiPacketReceivedCallback callback)
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");

    g_packetReceivedCallback = callback;

    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "OUT");
}

void CAWiFiSetExceptionCallback(CAWiFiExceptionCallback callback)
{
    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "IN");

    g_exceptionCallback = callback;

    OIC_LOG(DEBUG, WIFI_SERVER_TAG, "OUT");
}



