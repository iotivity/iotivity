/*****************************************************************j
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/if.h>
#include <errno.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <error.h>

#include "caciinterface.h"
#include "pdu.h"
#include "caadapterutils.h"
#include "camutex.h"
#include "oic_malloc.h"
#include "oic_string.h"

/**
 * Logging tag for module name.
 */
#define TAG "CI_SERVER"

/**
 * Server port number for local test.
 */
#define SERVER_PORT 8000

/**
 * Mutex to synchronize device object list.
 */
static ca_mutex g_mutexObjectList = NULL;

/**
 * Maintains the callback to be notified when data received from remote
 * Bluetooth device.
 */
static CACIPacketReceivedCallback g_packetReceivedCallback;

/**
 * Error callback to update error in CI.
 */
static CACIErrorHandleCallback g_ciErrorHandler = NULL;

static CAResult_t CAReceiveMessage();

/**
 * Destroy Mutex.
 */
static void CACIDestroyMutex()
{
    if (g_mutexObjectList)
    {
        ca_mutex_free(g_mutexObjectList);
        g_mutexObjectList = NULL;
    }
}

/*
 * Create Mutex.
 */
static CAResult_t CACICreateMutex()
{
    g_mutexObjectList = ca_mutex_new();
    if (!g_mutexObjectList)
    {
        OIC_LOG(ERROR, TAG, "Failed to created mutex!");

        CACIDestroyMutex();
        return CA_STATUS_FAILED;
    }

    return CA_STATUS_OK;
}

static void CACIDisconnectAll()
{
    OIC_LOG(DEBUG, TAG, "IN");

    ca_mutex_lock(g_mutexObjectList);
    uint32_t length = u_arraylist_length(caglobals.ci.svrlist);

    CACIServerInfo_t *svritem = NULL;
    for (size_t i = 0; i < length; i++)
    {
        svritem = (CACIServerInfo_t *) u_arraylist_get(caglobals.ci.svrlist, i);
        if (svritem && svritem->u4tcp.fd >= 0)
        {
            close(svritem->u4tcp.fd);
        }
    }
    u_arraylist_destroy(caglobals.ci.svrlist);

    ca_mutex_unlock(g_mutexObjectList);

    OIC_LOG(DEBUG, TAG, "OUT");
}

static void CAReceiveHandler(void *data)
{
    (void)data;
    OIC_LOG(DEBUG, TAG, "IN - CAReceiveHandler");

    while (!caglobals.ci.terminate)
    {
        CAReceiveMessage();
    }

    OIC_LOG(DEBUG, TAG, "OUT - CAReceiveHandler");
}

static CAResult_t CAReceiveMessage()
{
    uint32_t length = u_arraylist_length(caglobals.ci.svrlist);

    CACIServerInfo_t *svritem = NULL;
    for (size_t i = 0; i < length; i++)
    {
        svritem = (CACIServerInfo_t *) u_arraylist_get(caglobals.ci.svrlist, i);

        if (svritem->u4tcp.fd < 0)
        {
            continue;
        }

        char recvBuffer[COAP_TCP_MAX_PDU_SIZE] = { 0 };
        ssize_t recvLen = recv(svritem->u4tcp.fd, recvBuffer, sizeof (recvBuffer), 0);
        if (recvLen <= 0)
        {
            if(EAGAIN == errno)
            {
                continue;
            }
            OIC_LOG_V(ERROR, TAG, "Recvfrom failed %s", strerror(errno));
            close(svritem->u4tcp.fd);
            u_arraylist_remove(caglobals.ci.svrlist, i);
        }
        else
        {
            CAEndpoint_t ep = { .adapter = CA_ADAPTER_CLOUD_INTERFACE,
                                .flags = CA_IPV4_TCP,
                                .port = svritem->u4tcp.port };
            strncpy(ep.addr, svritem->addr, MAX_ADDR_STR_SIZE_CA);

            if (g_packetReceivedCallback)
            {
                g_packetReceivedCallback(&ep, recvBuffer, recvLen);
            }
        }
    }
    return CA_STATUS_OK;
}

static int CACICreateTCPSocket(int family, CACIServerInfo_t *ciServerInfo)
{
    // create tcp socket
    int fd = socket(family, SOCK_STREAM, IPPROTO_TCP);
    if (-1 == fd)
    {
        OIC_LOG_V(ERROR, TAG, "create socket failed: %s", strerror(errno));
        goto exit;
    }

    // set non-blocking socket
    unsigned long nonblock = 1;
    if (ioctl(fd, FIONBIO, &nonblock))
    {
        OIC_LOG(ERROR, TAG, "set non-blocking socket error!");
    }

    struct sockaddr_in sa = { .sin_family = family };
    inet_pton(family, ciServerInfo->addr, &sa.sin_addr.s_addr);
    sa.sin_port = htons(ciServerInfo->u4tcp.port);

    // connect to CI server
    socklen_t socklen = sizeof(sa);
    if (connect(fd, (struct sockaddr *)&sa, socklen) != 0)
    {
        OIC_LOG(ERROR, TAG, "first connect, error");
    }

    // update the max file descriptor
    if (fd > caglobals.ci.maxfd)
    {
        caglobals.ci.maxfd = fd;
    }

    struct timeval timeout = { .tv_sec = caglobals.ci.selectTimeout / 1000,
                               .tv_usec = (caglobals.ci.selectTimeout % 1000) * 1000};

    fd_set writeFd;
    FD_ZERO(&writeFd);
    FD_SET(caglobals.ci.maxfd, &writeFd);

    int error = 0;
    int state = select(caglobals.ci.maxfd + 1, NULL, &writeFd, NULL, &timeout);
    switch (state)
    {
        case -1:
            OIC_LOG(ERROR, TAG, "select() error");
            goto exit;
            break;
        case 0:
            OIC_LOG(ERROR, TAG, "time out error");
            goto exit;
            break;
        default:
            if (FD_ISSET(fd, &writeFd))
            {
                socklen_t len = sizeof(error);
                if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len))
                {
                    OIC_LOG(ERROR, TAG, "getsockopt error");
                }
            }
            else
            {
                OIC_LOG(ERROR, TAG, "select error: sockfd not set");
            }
            OIC_LOG(ERROR, TAG, "connect socket success");
            break;
    }

    // set blocking socket
    nonblock = 0;
    if (ioctl(fd, FIONBIO, &nonblock))
    {
        OIC_LOG(ERROR, TAG, "set blocking socket error!");
    }

    return fd;

exit:
    if (fd >= 0)
    {
        close(fd);
    }
    return -1;
}

#define CHECKFD(FD) \
    if (FD > caglobals.ci.maxfd) \
        caglobals.ci.maxfd = FD;
#define NEWTCPSOCKET(FAMILY, NAME, SERVER)  \
    SERVER->NAME.fd = CACICreateTCPSocket(FAMILY, SERVER);

static void CACIConvertAddrToString(struct sockaddr_in *addr, char *addr_str)
{
    inet_ntop(AF_INET, &(addr->sin_addr), addr_str, INET_ADDRSTRLEN);
}

static void CAAcceptHandler(void *data)
{
    (void)data;
    OIC_LOG(DEBUG, TAG, "IN - CAAcceptHandler");

    int reuse = 1;
    struct sockaddr_in server;

    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);

    int svrsockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (svrsockfd < 0)
    {
        OIC_LOG(ERROR, TAG, "Failed to create socket");
    }

    if (setsockopt(svrsockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
    {
        OIC_LOG(ERROR, TAG, "setsockopt SO_REUSEADDR");
    }

    int serverlen = sizeof(server);
    if (bind(svrsockfd, (struct sockaddr *)&server, serverlen) < 0)
    {
        OIC_LOG(ERROR, TAG, "bind() error");
    }

    if (listen(svrsockfd, 3) != 0)
    {
        OIC_LOG(ERROR, TAG, "listen() error");
    }

    struct sockaddr clientaddr;
    socklen_t clientlen = sizeof(clientaddr);

    while (!caglobals.ci.terminate)
    {
        int sockfd = accept(svrsockfd, (struct sockaddr *)&clientaddr, &clientlen);
        if (sockfd != -1)
        {
            CACIServerInfo_t *svritem = (CACIServerInfo_t *) OICMalloc(sizeof (CACIServerInfo_t));
            if (!svritem)
            {
                OIC_LOG(ERROR, TAG, "Out of memory");
                return;
            }
            svritem->u4tcp.fd = sockfd;
            svritem->u4tcp.port = SERVER_PORT;
            CACIConvertAddrToString((struct sockaddr_in *)&clientaddr, (char *) &svritem->addr);

            ca_mutex_lock(g_mutexObjectList);
            bool res = u_arraylist_add(caglobals.ci.svrlist, svritem);
            if (!res)
            {
                OIC_LOG(ERROR, TAG, "u_arraylist_add failed.");
                OICFree(svritem);
                ca_mutex_unlock(g_mutexObjectList);
                return;
            }
            ca_mutex_unlock(g_mutexObjectList);
        }
    }

    OIC_LOG(DEBUG, TAG, "OUT - CAAcceptHandler");
}

CAResult_t CACIStartServer(const ca_thread_pool_t threadPool)
{
    if (caglobals.ci.started)
    {
        return CA_STATUS_OK;
    }

    if (!caglobals.ci.ipv4tcpenabled)
    {
        caglobals.ci.ipv4tcpenabled = true;
    }

    CACICreateMutex();

    ca_mutex_lock(g_mutexObjectList);
    if (!caglobals.ci.svrlist)
    {
        caglobals.ci.svrlist = u_arraylist_create();
    }
    ca_mutex_unlock(g_mutexObjectList);

    caglobals.ci.terminate = false;

    CAResult_t res = ca_thread_pool_add_task(threadPool, CAAcceptHandler, NULL);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "thread_pool_add_task failed");
        return res;
    }
    OIC_LOG(DEBUG, TAG, "CAAcceptHandler thread started successfully.");

    res = ca_thread_pool_add_task(threadPool, CAReceiveHandler, NULL);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "thread_pool_add_task failed");
        return res;
    }
    OIC_LOG(DEBUG, TAG, "CAReceiveHandler thread started successfully.");

    caglobals.ci.started = true;

    return CA_STATUS_OK;
}

void CACIStopServer()
{
    OIC_LOG(DEBUG, TAG, "IN");

    caglobals.ci.terminate = true;

    CACIDisconnectAll();

    CACIDestroyMutex();

    OIC_LOG(DEBUG, TAG, "OUT");
}

void CACISetPacketReceiveCallback(CACIPacketReceivedCallback callback)
{
    OIC_LOG(DEBUG, TAG, "IN");

    g_packetReceivedCallback = callback;

    OIC_LOG(DEBUG, TAG, "OUT");
}

static void sendData(const CAEndpoint_t *endpoint,
                     const void *data, uint32_t dlen,
                     const char *cast, const char *fam)
{
    uint32_t index = 0;
    CACIServerInfo_t *svritem = CAGetCIServerInfoFromList(endpoint->addr, endpoint->port,
                                                          &index);
    if (!svritem)
    {
        OIC_LOG(ERROR, TAG, "Failed to get TCP Server Info");
        return;
    }

    if (svritem->u4tcp.fd < 0)
    {
        return;
    }

    struct timeval timeout = { .tv_sec = caglobals.ci.selectTimeout / 1000,
                               .tv_usec = (caglobals.ci.selectTimeout % 1000) * 1000};

    if (setsockopt(svritem->u4tcp.fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) == -1)
    {
        OIC_LOG(ERROR, TAG, "Failed to set socket option");
        return;
    }

    ssize_t len = send(svritem->u4tcp.fd, data, dlen, 0);
    if (-1 == len)
    {
        OIC_LOG_V(ERROR, TAG, "%s %s sendTo failed: %s", cast, fam, strerror(errno));
        g_ciErrorHandler(endpoint, data, dlen, CA_SEND_FAILED);
    }
    else
    {
        OIC_LOG_V(INFO, TAG, "%s %s sendTo is successful: %d bytes", cast, fam, len);
    }
}

void CACISendData(CAEndpoint_t *endpoint, const void *data, uint32_t datalen,
                                                            bool isMulticast)
{
    VERIFY_NON_NULL_VOID(endpoint, TAG, "endpoint is NULL");
    VERIFY_NON_NULL_VOID(data, TAG, "data is NULL");

    if (!isMulticast)
    {
        if (caglobals.ci.ipv4tcpenabled && (endpoint->flags & CA_IPV4_TCP))
        {
            sendData(endpoint, data, datalen, "unicast", "ipv4tcp");
        }
    }
}

CAResult_t CAGetCIInterfaceInformation(CAEndpoint_t **info, uint32_t *size)
{
    OIC_LOG(DEBUG, TAG, "IN");

    VERIFY_NON_NULL(info, TAG, "info is NULL");
    VERIFY_NON_NULL(size, TAG, "size is NULL");

    return CA_NOT_SUPPORTED;
}

CAResult_t CAConnectToCIServer(CACIServerInfo_t *ciServerInfo)
{
    VERIFY_NON_NULL(ciServerInfo, TAG, "CI server info is NULL");

    // create the socket and connect to CI server
    if (caglobals.ci.ipv4tcpenabled)
    {
        NEWTCPSOCKET(AF_INET, u4tcp, ciServerInfo);
        if (-1 == ciServerInfo->u4tcp.fd)
        {
            return CA_STATUS_FAILED;
        }
    }

    // add TCP connection info to list
    ca_mutex_lock(g_mutexObjectList);
    if (!caglobals.ci.svrlist)
    {
        caglobals.ci.svrlist = u_arraylist_create();
    }

    bool res = u_arraylist_add(caglobals.ci.svrlist, ciServerInfo);
    if (!res)
    {
        OIC_LOG(ERROR, TAG, "u_arraylist_add failed.");
        ca_mutex_unlock(g_mutexObjectList);
        return CA_STATUS_FAILED;
    }
    ca_mutex_unlock(g_mutexObjectList);

    return CA_STATUS_OK;
}

CAResult_t CADisconnectFromCIServer(const CACIServerInfo_t *ciServerInfo)
{
    VERIFY_NON_NULL(ciServerInfo, TAG, "CI server info is NULL");

    // #1. get server info
    uint32_t index = 0;
    ca_mutex_lock(g_mutexObjectList);
    CACIServerInfo_t *svritem = CAGetCIServerInfoFromList(ciServerInfo->addr,
                                                          ciServerInfo->u4tcp.port,
                                                          &index);
    if (!svritem)
    {
        OIC_LOG(ERROR, TAG, "there is no connection info");
        ca_mutex_unlock(g_mutexObjectList);
        return CA_STATUS_FAILED;
    }

    // #2. close the socket and remove CI connection info in list
    close(svritem->u4tcp.fd);
    u_arraylist_remove(caglobals.ci.svrlist, index);
    ca_mutex_unlock(g_mutexObjectList);

    return CA_STATUS_OK;
}

CACIServerInfo_t *CAGetCIServerInfoFromList(const char *addr, const uint16_t port,
                                            uint32_t *index)
{
    VERIFY_NON_NULL_RET(addr, TAG, "addr is NULL", NULL);
    VERIFY_NON_NULL_RET(index, TAG, "index is NULL", NULL);

    // get connection info from list
    uint32_t length = u_arraylist_length(caglobals.ci.svrlist);

    CACIServerInfo_t *svritem = NULL;
    for (size_t i = 0; i < length; i++)
    {
        svritem = (CACIServerInfo_t *) u_arraylist_get(caglobals.ci.svrlist, i);

        if (svritem->addr)
        {
            if (!strncmp(svritem->addr, addr, MAX_ADDR_STR_SIZE_CA)
                    && (svritem->u4tcp.port == port))
            {
                *index = i;
                return svritem;
            }
        }
    }

    return NULL;
}

void CACISetErrorHandler(CACIErrorHandleCallback errorHandleCallback)
{
    g_ciErrorHandler = errorHandleCallback;
}
