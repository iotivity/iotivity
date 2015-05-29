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
#include "oic_malloc.h"

/**
 * @def IP_SERVER_TAG
 * @brief Logging tag for module name
 */
#define IP_SERVER_TAG "IP_SERVER"

/**
 * @def CA_UDP_BIND_RETRY_COUNT
 * @brief Retry count in case of socket bind failure.
 */
#define CA_UDP_BIND_RETRY_COUNT 10

/**
 * @def IPNAMESIZE
 * @brief Max length for ip address.
 */
#define IPNAMESIZE 16

/**
 * @def SOCKETOPTION
 * @brief Socket option.
 */
#define SOCKETOPTION 1

/**
 * @var g_packetHandlerStopFlag
 * @brief Flag for stopping packet handler thread.
 */
static bool g_packetHandlerStopFlag = false;

/**
 * @var CAAdapterIPServerContext_t
 * @brief Thread context information for callbacks and threadpool.
 */
typedef struct
{
    ca_thread_pool_t threadPool;
    CAIPPacketReceivedCallback packetReceivedCallback;
    CAIPExceptionCallback exceptionCallback;
} CAAdapterIPServerContext_t;

/**
 * @var g_serverInfoList
 * @brief Mutex to synchronize ethenet adapter context.
 */
static u_arraylist_t *g_serverInfoList = NULL;

/**
 * @var g_mutexServerInfoList
 * @brief Mutex to synchronize Server Information.
 */
static ca_mutex g_mutexServerInfoList = NULL;

/**
 * @var g_adapterEthServerContext
 * @brief Mutex to synchronize ethenet adapter context.
 */
static CAAdapterIPServerContext_t *g_adapterEthServerContext = NULL;

/**
 * @var g_mutexAdapterServerContext
 * @brief Mutex to synchronize unicast server
 */
static ca_mutex g_mutexAdapterServerContext = NULL;

static void CAReceiveHandler(void *data)
{
    OIC_LOG(DEBUG, IP_SERVER_TAG, "IN");

    fd_set readFds;
    int maxSd = 0;
    struct timeval timeout;
    char recvBuffer[COAP_MAX_PDU_SIZE] = { 0 };

    while (true != g_packetHandlerStopFlag)
    {
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        FD_ZERO(&readFds);

        ca_mutex_lock(g_mutexServerInfoList);
        uint32_t listIndex = 0;
        uint32_t listLength = u_arraylist_length(g_serverInfoList);

        u_arraylist_t *tempServerInfoList = u_arraylist_create();
        if (!tempServerInfoList)
        {
            OIC_LOG(ERROR, IP_SERVER_TAG, "u_arraylist_create failed");
            ca_mutex_unlock(g_mutexServerInfoList);
            return;
        }

        for (listIndex = 0; listIndex < listLength; listIndex++)
        {
            CAServerInfo_t *info = (CAServerInfo_t *) u_arraylist_get(g_serverInfoList, listIndex);
            if (!info)
            {
                listIndex++;
                continue;
            }

            int sd = info->socketFd;
            //if valid socket descriptor then add to read list
            if (sd > 0)
            {
                FD_SET(sd, &readFds);
            }

            //highest file descriptor number, need it for the select function
            if (sd > maxSd)
            {
                maxSd = sd;
            }

            CAServerInfo_t *newInfo = (CAServerInfo_t *) OICMalloc(sizeof(CAServerInfo_t));
            if (!newInfo)
            {
                OIC_LOG(ERROR, IP_SERVER_TAG, "Malloc failed");
                CAClearServerInfoList(tempServerInfoList);
                ca_mutex_unlock(g_mutexServerInfoList);
                return;
            }

            memcpy(newInfo, info, sizeof(CAServerInfo_t));

            CAResult_t result = u_arraylist_add(tempServerInfoList, (void *) newInfo);
            if (CA_STATUS_OK != result)
            {
                OIC_LOG(ERROR, IP_SERVER_TAG, "u_arraylist_add failed!Thread exit");
                CAClearServerInfoList(tempServerInfoList);
                ca_mutex_unlock(g_mutexServerInfoList);
                return;
            }
        }

        ca_mutex_unlock(g_mutexServerInfoList);

        int ret = select(maxSd + 1, &readFds, NULL, NULL, &timeout);
        if (g_packetHandlerStopFlag)
        {
            OIC_LOG_V(DEBUG, IP_SERVER_TAG,
                      "Packet receiver handler Stop request received. Thread exited");
            CAClearServerInfoList(tempServerInfoList);
            break;
        }
        if (ret < 0)
        {
            OIC_LOG_V(FATAL, IP_SERVER_TAG, "select returned error %s", strerror(errno));
            CAClearServerInfoList(tempServerInfoList);
            continue;
        }

        listLength = u_arraylist_length(tempServerInfoList);
        for (listIndex = 0; listIndex < listLength; listIndex++)
        {
            CAServerInfo_t *info = (CAServerInfo_t *) u_arraylist_get(tempServerInfoList,
                                                                      listIndex);
            if (!info)
            {
                continue;
            }

            int sd = info->socketFd;
            if (FD_ISSET(sd , &readFds))
            {
                OIC_LOG_V(ERROR, IP_SERVER_TAG,
                          "data Received server information ip %s, port %d socket %d",
                          info->ipAddress, info->port, sd);
                memset(recvBuffer, 0, sizeof(recvBuffer));

                struct sockaddr_in srcSockAddress = { 0 };
                socklen_t srcAddressLen = sizeof(srcSockAddress);

                //Reading from socket
                ssize_t recvLen = recvfrom(sd, recvBuffer, sizeof(recvBuffer), 0,
                                           (struct sockaddr *) &srcSockAddress, &srcAddressLen);
                if (-1 == recvLen)
                {
                    OIC_LOG_V(ERROR, IP_SERVER_TAG, "Recvfrom failed %s", strerror(errno));
                    continue;
                }
                else if (0 == recvLen)
                {
                    OIC_LOG_V(ERROR, IP_SERVER_TAG, "Server socket shutdown sock fd[%d]", sd);
                    ca_mutex_lock(g_mutexAdapterServerContext);
                    // Notify upper layer this exception
                    if (g_adapterEthServerContext->exceptionCallback)
                    {
                        // need to make proper exception callback.
                        //g_adapterEthServerContext->exceptionCallback(ctx->type);
                    }
                    ca_mutex_unlock(g_mutexAdapterServerContext);
                }

                char srcIPAddress[CA_IPADDR_SIZE] = { 0 };
                if (!inet_ntop(AF_INET, &srcSockAddress.sin_addr.s_addr, srcIPAddress,
                               sizeof(srcIPAddress)))
                {

                    OIC_LOG(ERROR, IP_SERVER_TAG, "inet_ntop is failed!");
                    continue;
                }

                uint16_t srcPort = ntohs(srcSockAddress.sin_port);

                OIC_LOG_V(DEBUG, IP_SERVER_TAG, "Received packet from %s:%d len %d",
                          srcIPAddress, srcPort, recvLen);

                char *netMask = NULL;
                if (CA_STATUS_OK != CAIPGetInterfaceSubnetMask(info->ifAddr, &netMask))
                {
                    OIC_LOG(ERROR, IP_SERVER_TAG, "Failed to get IP subnet");
                    continue;
                }

                if (!CAAdapterIsSameSubnet(info->ifAddr, srcIPAddress, netMask))
                {
                    OIC_LOG(DEBUG, IP_SERVER_TAG,
                            "Packet received from different subnet, Ignore!");
                    OICFree(netMask);
                    continue;
                }
                OICFree(netMask);

                if (info->isSecured)
                {
#ifdef __WITH_DTLS__
                    CAResult_t ret = CAAdapterNetDtlsDecrypt(srcIPAddress, srcPort,
                                                             (uint8_t *)recvBuffer, recvLen,
                                                             DTLS_IP);
                    OIC_LOG_V(DEBUG, IP_SERVER_TAG,
                              "CAAdapterNetDtlsDecrypt returns [%d]", ret);
#endif
                }
                else //both multicast and unicast
                {
                    ca_mutex_lock(g_mutexAdapterServerContext);

                    if (g_adapterEthServerContext->packetReceivedCallback)
                    {
                        g_adapterEthServerContext->packetReceivedCallback(srcIPAddress, srcPort,
                                                                          recvBuffer, recvLen,
                                                                          false);
                    }

                    ca_mutex_unlock(g_mutexAdapterServerContext);
                }
            }
        }
        CAClearServerInfoList(tempServerInfoList);
    }
    OIC_LOG(DEBUG, IP_SERVER_TAG, "OUT");
}

static CAResult_t CACreateSocket(int *socketFD, const char *localIp, uint16_t *port,
                                 bool forceBindStart)
{
    VERIFY_NON_NULL(socketFD, IP_SERVER_TAG, "socketFD is NULL");
    VERIFY_NON_NULL(localIp, IP_SERVER_TAG, "localIp is NULL");
    VERIFY_NON_NULL(port, IP_SERVER_TAG, "port is NULL");
    // Create a UDP socket
    int sock = -1;

#ifdef SOCK_CLOEXEC
    sock = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, IPPROTO_UDP);
#endif

    if (-1 == sock)
    {
        sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    }

    if (-1 == sock)
    {
        OIC_LOG_V(ERROR, IP_SERVER_TAG, "Failed to create Socket, Error code: %s",
                  strerror(errno));
        return CA_STATUS_FAILED;
    }

    // Make the socket non-blocking
    if (-1 == fcntl(sock, F_SETFL, O_NONBLOCK))
    {
        OIC_LOG_V(ERROR, IP_SERVER_TAG, "Failed to set non-block mode, Error code: %s",
                  strerror(errno));

        close(sock);
        return CA_STATUS_FAILED;
    }

    if (true == forceBindStart)
    {
        int setOptionOn = SOCKETOPTION;
        if (-1 == setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *) &setOptionOn,
                             sizeof(setOptionOn)))
        {
            OIC_LOG_V(ERROR, IP_SERVER_TAG, "Failed to set SO_REUSEADDR! Error code: %s",
                      strerror(errno));
            close(sock);
            return CA_STATUS_FAILED;
        }
    }

    struct sockaddr_in sockAddr = { 0 };
    uint16_t serverPort = *port;
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(serverPort);
    if (localIp)
    {
        sockAddr.sin_addr.s_addr = inet_addr(localIp);
    }

    int16_t i = 0;
    bool isBound = false;
    for (i = 0; i < CA_UDP_BIND_RETRY_COUNT; i++)
    {
        if (-1 == bind(sock, (struct sockaddr *) &sockAddr, sizeof(sockAddr)))
        {
            if (false == forceBindStart)
            {
                OIC_LOG_V(ERROR, IP_SERVER_TAG, "Failed to bind socket[%s]. Trying again..",
                          strerror(errno));

                //Set the port to next one
                serverPort += 1;
                sockAddr.sin_port = htons(serverPort);
                continue;
            }
            else
            {
                OIC_LOG_V(ERROR, IP_SERVER_TAG, "Failed to bind socket[%s]!",
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

static void CACloseSocket(int socketFD)
{
    if (-1 == socketFD)
    {
        OIC_LOG(ERROR, IP_SERVER_TAG, "Invalid Socket Fd");
        return;
    }

    // close the socket
    if (-1 == close(socketFD))
    {
        OIC_LOG_V(ERROR, IP_SERVER_TAG, "Failed to close the socket, Error code: %s\n",
                  strerror(errno));
    }
}

static CAResult_t CAStartUnicastServer(const char *localAddress, uint16_t *port,
                                       bool forceBindStart, bool isSecured, int *serverFD)
{
    OIC_LOG(DEBUG, IP_SERVER_TAG, "IN");

    VERIFY_NON_NULL(serverFD, IP_SERVER_TAG, "serverFD");
    VERIFY_NON_NULL(localAddress, IP_SERVER_TAG, "localAddress");
    VERIFY_NON_NULL(port, IP_SERVER_TAG, "port");

    CAResult_t ret = CACreateSocket(serverFD, localAddress, port, forceBindStart);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, IP_SERVER_TAG, "Failed to create unicast socket");
    }

    OIC_LOG(DEBUG, IP_SERVER_TAG, "OUT");
    return ret;
}

static CAResult_t CAIPStartPacketReceiverHandler()
{
    OIC_LOG(DEBUG, IP_SERVER_TAG, "IN");

    ca_mutex_lock(g_mutexServerInfoList);

    uint32_t listLength = u_arraylist_length(g_serverInfoList);

    ca_mutex_unlock(g_mutexServerInfoList);

    ca_mutex_lock(g_mutexAdapterServerContext);

    if (!g_adapterEthServerContext)
    {
        OIC_LOG(ERROR, IP_SERVER_TAG, "g_adapterEthServerContext NULL");
        ca_mutex_unlock(g_mutexAdapterServerContext);
        return CA_STATUS_FAILED;
    }

    if (1 == listLength) //Its first time.
    {
        g_packetHandlerStopFlag = false;
        if (CA_STATUS_OK != ca_thread_pool_add_task(g_adapterEthServerContext->threadPool,
                                                   CAReceiveHandler, NULL ))
        {
            OIC_LOG(ERROR, IP_SERVER_TAG, "thread_pool_add_task failed!");
            ca_mutex_unlock(g_mutexAdapterServerContext);
            return CA_STATUS_FAILED;
        }
        OIC_LOG(DEBUG, IP_SERVER_TAG, "CAReceiveHandler thread started successfully.");
    }
    else
    {
        OIC_LOG(DEBUG, IP_SERVER_TAG, "CAReceiveHandler thread already is running");
    }
    ca_mutex_unlock(g_mutexAdapterServerContext);

    OIC_LOG(DEBUG, IP_SERVER_TAG, "OUT");

    return CA_STATUS_OK;
}

static void CAIPServerDestroyMutex(void)
{
    OIC_LOG(DEBUG, IP_SERVER_TAG, "IN");

    if (g_mutexServerInfoList)
    {
        ca_mutex_free(g_mutexServerInfoList);
        g_mutexServerInfoList = NULL;
    }

    if (g_mutexAdapterServerContext)
    {
        ca_mutex_free(g_mutexAdapterServerContext);
        g_mutexAdapterServerContext = NULL;
    }

    OIC_LOG(DEBUG, IP_SERVER_TAG, "OUT");
}

static CAResult_t CAIPServerCreateMutex(void)
{
    OIC_LOG(DEBUG, IP_SERVER_TAG, "IN");

    g_mutexServerInfoList = ca_mutex_new();
    if (!g_mutexServerInfoList)
    {
        OIC_LOG(DEBUG, IP_SERVER_TAG, "OUT");
        return CA_MEMORY_ALLOC_FAILED;
    }

    g_mutexAdapterServerContext = ca_mutex_new();
    if (!g_mutexAdapterServerContext)
    {
        OIC_LOG(ERROR, IP_SERVER_TAG, "Failed to created mutex!");
        ca_mutex_free(g_mutexServerInfoList);
        g_mutexServerInfoList = NULL;
        return CA_MEMORY_ALLOC_FAILED;
    }

    OIC_LOG(DEBUG, IP_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAIPInitializeServer(const ca_thread_pool_t threadPool)
{
    OIC_LOG(DEBUG, IP_SERVER_TAG, "IN");

    // Input validation
    VERIFY_NON_NULL(threadPool, IP_SERVER_TAG, "Thread pool handle is NULL");

    // Initialize mutex
    if (CA_STATUS_OK != CAIPServerCreateMutex())
    {
        OIC_LOG(ERROR, IP_SERVER_TAG, "Failed to create mutex!");
        return CA_STATUS_FAILED;
    }

    ca_mutex_lock(g_mutexAdapterServerContext);
    g_adapterEthServerContext = (CAAdapterIPServerContext_t *) OICCalloc(1,
                                 sizeof(CAAdapterIPServerContext_t));

    if (!g_adapterEthServerContext)
    {
        OIC_LOG(ERROR, IP_SERVER_TAG, "Malloc failed");
        ca_mutex_unlock(g_mutexAdapterServerContext);
        return CA_MEMORY_ALLOC_FAILED;
    }

    g_adapterEthServerContext->threadPool = threadPool;

    ca_mutex_unlock(g_mutexAdapterServerContext);

    ca_mutex_lock(g_mutexServerInfoList);

    g_serverInfoList = u_arraylist_create();
    if (!g_serverInfoList)
    {
        OIC_LOG(ERROR, IP_SERVER_TAG, "u_arraylist_create failed");
        ca_mutex_unlock(g_mutexServerInfoList);
        return CA_MEMORY_ALLOC_FAILED;
    }
    ca_mutex_unlock(g_mutexServerInfoList);
    OIC_LOG(DEBUG, IP_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CAIPTerminateServer()
{
    OIC_LOG(DEBUG, IP_SERVER_TAG, "IN");
    ca_mutex_lock(g_mutexAdapterServerContext);
    if (!g_adapterEthServerContext)
    {
        OIC_LOG(ERROR, IP_SERVER_TAG, "g_adapterEthServerContext NULL");
        ca_mutex_unlock(g_mutexAdapterServerContext);
        return;
    }

    OICFree(g_adapterEthServerContext);
    g_adapterEthServerContext = NULL;

    ca_mutex_unlock(g_mutexAdapterServerContext);

    ca_mutex_lock(g_mutexServerInfoList);

    CAClearServerInfoList(g_serverInfoList);
    g_serverInfoList = NULL;

    ca_mutex_unlock(g_mutexServerInfoList);
    // Destroy mutex
    CAIPServerDestroyMutex();

    OIC_LOG(DEBUG, IP_SERVER_TAG, "OUT");

}

CAResult_t CAIPStartUnicastServer(const char *localAddress, uint16_t *port,
                                        bool forceBindStart, bool isSecured)
{
    OIC_LOG(DEBUG, IP_SERVER_TAG, "IN");

    // Input validation
    VERIFY_NON_NULL(localAddress, IP_SERVER_TAG, "localAddress");
    VERIFY_NON_NULL(port, IP_SERVER_TAG, "port");

    if (0 >= *port)
    {
        OIC_LOG(ERROR, IP_SERVER_TAG, "Invalid input: port is invalid!");
        return CA_STATUS_INVALID_PARAM;
    }

    ca_mutex_lock(g_mutexServerInfoList);
    bool isUnicastServerStarted = CAIsUnicastServerStarted(g_serverInfoList, localAddress, *port);
    if (!isUnicastServerStarted)
    {
        int unicastServerFd = -1;
        if (CA_STATUS_OK != CAStartUnicastServer(localAddress, port, forceBindStart, isSecured,
                                                 &unicastServerFd))
        {
            OIC_LOG(ERROR, IP_SERVER_TAG, "Failed to start unicast server!");
            ca_mutex_unlock(g_mutexServerInfoList);
            return CA_STATUS_FAILED;
        }

        CAServerInfo_t *info = (CAServerInfo_t *) OICCalloc(1, sizeof(CAServerInfo_t));
        if (!info)
        {
            OIC_LOG(ERROR, IP_SERVER_TAG, "Malloc failed");
            close(unicastServerFd);
            ca_mutex_unlock(g_mutexServerInfoList);
            return CA_MEMORY_ALLOC_FAILED;
        }

        char *netMask = NULL;
        if (CA_STATUS_OK != CAIPGetInterfaceSubnetMask(localAddress, &netMask))
        {
            OIC_LOG(ERROR, IP_SERVER_TAG, "Failed to get IP subnet");
        }
        if (netMask)
        {
            strncpy(info->subNetMask, netMask, sizeof(info->subNetMask) - 1);
            info->subNetMask[sizeof(info->subNetMask)-1] = '\0';
            OICFree(netMask);
        }
        strncpy(info->ipAddress, localAddress, sizeof(info->ipAddress) - 1);
        info->ipAddress[sizeof(info->ipAddress) - 1] = '\0';
        info->port = *port;
        info->socketFd = unicastServerFd;
        info->isSecured = isSecured;
        info->isServerStarted = true;
        info->isMulticastServer = false;
        strncpy(info->ifAddr, localAddress, sizeof(info->ifAddr) - 1);
        info->ifAddr[sizeof(info->ifAddr) - 1] = '\0';

        CAResult_t res = CAAddServerInfo(g_serverInfoList, info);
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, IP_SERVER_TAG, "CAAddServerInfo failed!");
            close(unicastServerFd);
            ca_mutex_unlock(g_mutexServerInfoList);
            return res;
        }
        ca_mutex_unlock(g_mutexServerInfoList);

        res = CAIPStartPacketReceiverHandler();
        if (CA_STATUS_OK != res)
        {
            OIC_LOG(ERROR, IP_SERVER_TAG, "CAIPStartPacketReceiverHandler failed!");
            close(unicastServerFd);
            return res;
        }
    }
    else
    {
        OIC_LOG_V(DEBUG, IP_SERVER_TAG, "Already Unicast Server Started ip [%s] port [%d]",
                  localAddress, *port);
        ca_mutex_unlock(g_mutexServerInfoList);
    }

    OIC_LOG(DEBUG, IP_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAIPStartMulticastServer(const char *localAddress, const char *multicastAddress,
                                          uint16_t multicastPort)
{
    OIC_LOG(DEBUG, IP_SERVER_TAG, "IN");

    // Input validation
    VERIFY_NON_NULL(localAddress, IP_SERVER_TAG, "localAddress");
    VERIFY_NON_NULL(multicastAddress, IP_SERVER_TAG, "port");

    uint16_t port = multicastPort;
    if (0 >= port)
    {
        OIC_LOG(ERROR, IP_SERVER_TAG, "Invalid input: Multicast port is invalid!");
        return CA_STATUS_INVALID_PARAM;
    }

    ca_mutex_lock(g_mutexServerInfoList);
    bool isMulticastServerStarted = CAIsMulticastServerStarted(g_serverInfoList, localAddress,
                                                               multicastAddress, port);
    if (!isMulticastServerStarted)
    {
        int mulicastServerFd = -1;
        CAResult_t ret = CACreateSocket(&mulicastServerFd, multicastAddress, &port, true);
        if (ret != CA_STATUS_OK)
        {
            OIC_LOG(ERROR, IP_SERVER_TAG, "Failed to create multicast socket");
            ca_mutex_unlock(g_mutexServerInfoList);
            return ret;
        }

        struct ip_mreq multicastMemberReq = {.imr_interface.s_addr = inet_addr(localAddress)};
        inet_aton(multicastAddress, &multicastMemberReq.imr_multiaddr);

        if (-1 == setsockopt(mulicastServerFd, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                             (char *) &multicastMemberReq, sizeof(struct ip_mreq)))
        {
            OIC_LOG_V(ERROR, IP_SERVER_TAG,
                      "Failed to add to multicast group, Error code: %s\n", strerror(errno));
            close(mulicastServerFd);
            ca_mutex_unlock(g_mutexServerInfoList);
            return CA_STATUS_FAILED;
        }

        CAServerInfo_t *info = (CAServerInfo_t *) OICCalloc(1, sizeof(CAServerInfo_t));
        if (!info)
        {
            OIC_LOG(ERROR, IP_SERVER_TAG, "Malloc failed");
            close(mulicastServerFd);
            ca_mutex_unlock(g_mutexServerInfoList);
            return CA_MEMORY_ALLOC_FAILED;
        }

        char *netMask = NULL;
        if (CA_STATUS_OK != CAIPGetInterfaceSubnetMask(localAddress, &netMask))
        {
            OIC_LOG(ERROR, IP_SERVER_TAG, "Failed to get IP subnet");
        }
        if (netMask)
        {
            strncpy(info->subNetMask, netMask, sizeof(info->subNetMask) - 1);
            info->subNetMask[sizeof(info->subNetMask) -1] = '\0';
            OICFree(netMask);
        }

        strncpy(info->ipAddress, multicastAddress, sizeof(info->ipAddress) - 1);
        info->ipAddress[sizeof(info->ipAddress) -1] = '\0';
        info->port = multicastPort;
        info->socketFd = mulicastServerFd;
        info->isSecured = false;
        info->isServerStarted = true;
        info->isMulticastServer = true;
        strncpy(info->ifAddr, localAddress, sizeof(info->ifAddr)-1);
        info->ifAddr[sizeof(info->ifAddr) -1] = '\0';

        ret = CAAddServerInfo(g_serverInfoList, info);

        if (CA_STATUS_OK != ret)
        {
            OIC_LOG(ERROR, IP_SERVER_TAG, "CAAddServerInfo failed!");
            close(mulicastServerFd);
            ca_mutex_unlock(g_mutexServerInfoList);
            return ret;
        }
        ca_mutex_unlock(g_mutexServerInfoList);

        ret = CAIPStartPacketReceiverHandler();
        if (CA_STATUS_OK != ret)
        {
            OIC_LOG(ERROR, IP_SERVER_TAG, "CAIPStartPacketReceiverHandler failed!");
            close(mulicastServerFd);
            return ret;
        }
    }
    else
    {
        OIC_LOG_V(DEBUG, IP_SERVER_TAG,
                  "Multicast Server is already started on interface addr[%s]", localAddress);
        ca_mutex_unlock(g_mutexServerInfoList);
    }

    OIC_LOG(DEBUG, IP_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAIPStopServer(const char *interfaceAddress)
{
    OIC_LOG(DEBUG, IP_SERVER_TAG, "IN");

    VERIFY_NON_NULL(interfaceAddress, IP_SERVER_TAG, "interfaceAddress is NULL");

    ca_mutex_lock(g_mutexServerInfoList);
    uint32_t listIndex = 0;
    uint32_t listLength = u_arraylist_length(g_serverInfoList);

    for (listIndex = 0; listIndex < listLength;)
    {
        CAServerInfo_t *info = (CAServerInfo_t *) u_arraylist_get(g_serverInfoList, listIndex);
        if (!info)
        {
            listIndex++;
            continue;
        }

        if (info->isMulticastServer && strncmp(interfaceAddress, info->ifAddr, strlen(info->ifAddr))
                == 0)
        {
            if (u_arraylist_remove(g_serverInfoList, listIndex))
            {
                struct ip_mreq multicastMemberReq = { { 0 }, { 0 } };

                multicastMemberReq.imr_interface.s_addr = inet_addr(info->ifAddr);
                inet_aton(info->ipAddress, &multicastMemberReq.imr_multiaddr);
                if (-1 == setsockopt(info->socketFd, IPPROTO_IP, IP_DROP_MEMBERSHIP,
                                     (char *) &multicastMemberReq, sizeof(struct ip_mreq)))
                {
                    OIC_LOG_V(ERROR, IP_SERVER_TAG,
                              "Failed to leave multicast group, Error code: %s", strerror(errno));
                }
                CACloseSocket(info->socketFd);
                OICFree(info);
                OIC_LOG(DEBUG, IP_SERVER_TAG, "Multicast server is stopped successfully.");
                // Reduce list length by 1 as we removed one element.
                listLength--;
            }
            else
            {
                OIC_LOG(ERROR, IP_SERVER_TAG, "u_arraylist_remove failed.");
                ca_mutex_unlock(g_mutexServerInfoList);
                return CA_STATUS_FAILED;
            }
        }
        else if (strncmp(interfaceAddress, info->ipAddress, strlen(info->ipAddress)) == 0)
        {
            if (u_arraylist_remove(g_serverInfoList, listIndex))
            {
                CACloseSocket(info->socketFd);
                OICFree(info);
                OIC_LOG(DEBUG, IP_SERVER_TAG, "Unicast server is stopped successfully.");
                // Reduce list length by 1 as we removed one element.
                listLength--;
            }
            else
            {
                OIC_LOG(ERROR, IP_SERVER_TAG, "u_arraylist_remove failed.");
                ca_mutex_unlock(g_mutexServerInfoList);
                return CA_STATUS_FAILED;
            }
        }
        else
        {
            listIndex++;
        }
    }

    OIC_LOG(DEBUG, IP_SERVER_TAG, "OUT");
    ca_mutex_unlock(g_mutexServerInfoList);
    return CA_STATUS_OK;
}

CAResult_t CAIPStopAllServers()
{
    OIC_LOG(DEBUG, IP_SERVER_TAG, "IN");

    g_packetHandlerStopFlag = true;

    ca_mutex_lock(g_mutexServerInfoList);

    uint32_t listIndex = 0;
    uint32_t listLength = u_arraylist_length(g_serverInfoList);
    for (listIndex = 0; listIndex < listLength;)
    {
        CAServerInfo_t *info = (CAServerInfo_t *) u_arraylist_get(g_serverInfoList, listIndex);
        if (!info)
        {
            listIndex++;
            continue;
        }
        if (u_arraylist_remove(g_serverInfoList, listIndex))
        {
            if (info->isMulticastServer)
            {
                struct ip_mreq multicastMemberReq = { { 0 }, { 0 } };

                multicastMemberReq.imr_interface.s_addr = inet_addr(info->ifAddr);
                inet_aton(info->ipAddress, &multicastMemberReq.imr_multiaddr);
                if (-1 == setsockopt(info->socketFd, IPPROTO_IP, IP_DROP_MEMBERSHIP,
                                     (char *) &multicastMemberReq, sizeof(struct ip_mreq)))
                {
                    OIC_LOG_V(ERROR, IP_SERVER_TAG,
                              "Failed to leave multicast group, Error code: %s", strerror(errno));
                }
            }
            CACloseSocket(info->socketFd);
            //Freeing server info.
            OICFree(info);
            // Reduce list length by 1 as we removed one element.
            listLength--;
        }
        else
        {
            OIC_LOG(ERROR, IP_SERVER_TAG, "u_arraylist_remove failed.");
            ca_mutex_unlock(g_mutexServerInfoList);
            return CA_STATUS_FAILED;
        }
    }

    ca_mutex_unlock(g_mutexServerInfoList);

    OIC_LOG(DEBUG, IP_SERVER_TAG, "All Server stopped successfully. OUT");
    return CA_STATUS_OK;
}

uint16_t CAGetServerPortNum(const char *ipAddress, bool isSecured)
{
    ca_mutex_lock(g_mutexServerInfoList);

    uint16_t port = CAGetServerPort(g_serverInfoList, ipAddress, isSecured);

    ca_mutex_unlock(g_mutexServerInfoList);

    return port;
}

CAResult_t CAGetIPServerInfoList(u_arraylist_t **serverInfoList)
{
    OIC_LOG(DEBUG, IP_SERVER_TAG, "IN");
    ca_mutex_lock(g_mutexServerInfoList);

    uint32_t list_index = 0;
    uint32_t list_length = u_arraylist_length(g_serverInfoList);
    for (list_index = 0; list_index < list_length; list_index++)
    {
        CAServerInfo_t *info = (CAServerInfo_t *) u_arraylist_get(g_serverInfoList, list_index);
        if (!info)
        {
            continue;
        }

        CAServerInfo_t *newNetinfo = (CAServerInfo_t *) OICMalloc(sizeof(CAServerInfo_t));
        if (!newNetinfo)
        {
            OIC_LOG(ERROR, IP_SERVER_TAG, "Malloc failed!");
            ca_mutex_unlock(g_mutexServerInfoList);
            return CA_MEMORY_ALLOC_FAILED;
        }

        memcpy(newNetinfo, info, sizeof(*info));

        CAResult_t result = u_arraylist_add(*serverInfoList, (void *) newNetinfo);
        if (CA_STATUS_OK != result)
        {
            OIC_LOG(ERROR, IP_SERVER_TAG, "u_arraylist_add failed!");
            ca_mutex_unlock(g_mutexServerInfoList);
            return CA_STATUS_FAILED;
        }
    }
    ca_mutex_unlock(g_mutexServerInfoList);
    OIC_LOG(DEBUG, IP_SERVER_TAG, "OUT");
    return CA_STATUS_OK;
}

void CAIPSetPacketReceiveCallback(CAIPPacketReceivedCallback callback)
{
    OIC_LOG(DEBUG, IP_SERVER_TAG, "IN");

    ca_mutex_lock(g_mutexAdapterServerContext);

    if (!g_adapterEthServerContext)
    {
        OIC_LOG(ERROR, IP_SERVER_TAG, "g_adapterEthServerContext NULL");
        ca_mutex_unlock(g_mutexAdapterServerContext);
        return;
    }
    g_adapterEthServerContext->packetReceivedCallback = callback;

    ca_mutex_unlock(g_mutexAdapterServerContext);

    OIC_LOG(DEBUG, IP_SERVER_TAG, "OUT");
}

void CAIPSetExceptionCallback(CAIPExceptionCallback callback)
{
    OIC_LOG(DEBUG, IP_SERVER_TAG, "IN");
    ca_mutex_lock(g_mutexAdapterServerContext);

    if (!g_adapterEthServerContext)
    {
        OIC_LOG(ERROR, IP_SERVER_TAG, "g_adapterEthServerContext NULL");
        ca_mutex_unlock(g_mutexAdapterServerContext);
        return;
    }
    g_adapterEthServerContext->exceptionCallback = callback;

    ca_mutex_unlock(g_mutexAdapterServerContext);

    OIC_LOG(DEBUG, IP_SERVER_TAG, "OUT");
}

