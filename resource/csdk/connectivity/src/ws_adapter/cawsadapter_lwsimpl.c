/* ****************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
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

#include <errno.h>
#include "cainterface.h"
#include "caipnwmonitor.h"
#include "cawsinterface.h"
#include "caqueueingthread.h"
#include "caadapterutils.h"
#include "cawsutil.h"
#include "octhread.h"
#include "uarraylist.h"
#include "caremotehandler.h"
#include "experimental/logger.h"
#include "oic_malloc.h"

#ifdef __WITH_TLS__
#include "cawsadapter_ssl.h"
#endif

#include "libwebsockets.h"

#define TAG "OIC_CA_WS_LWS_IMPL"

static bool g_continueLWSService = false;

static oc_mutex g_continueLWSServiceMutex = NULL;

static oc_cond g_LWSServiceStoppedCond = NULL;

static WSConnInfoList *g_connInfoList = NULL;

static oc_mutex g_connInfoListMutex = NULL;

static CANetworkPacketReceivedCallback g_networkPacketCallback = NULL;

static CAConnectionChangeCallback g_connectionChangeCallback = NULL;

static CAErrorHandleCallback g_errorHandleCallback = NULL;

void CAWSSetPacketReceivedCallback(CANetworkPacketReceivedCallback networkPacketCallback)
{
    g_networkPacketCallback = networkPacketCallback;
}

void CAWSSetConnectionChangeCallback(CAConnectionChangeCallback connCallback)
{
    g_connectionChangeCallback = connCallback;
}

void CAWSSetErrorHandleCallback(CAErrorHandleCallback errorCallback)
{
    g_errorHandleCallback = errorCallback;
}

static int CALWSCallback(struct lws *wsi, enum lws_callback_reasons reason,
                         void *user, void *in, size_t len);

static struct lws_protocols protocols[] =
{
    { "coap", CALWSCallback, 0 },
    { NULL, NULL, 0 }
};

static struct lws_context *g_context;

static int CALWSCallback(struct lws *wsi, enum lws_callback_reasons reason,
                         void *user, void *in, size_t len)
{
    WSConnInfo *connInfo;

    switch (reason)
    {
        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
            oc_mutex_lock(g_connInfoListMutex);
            connInfo = (WSConnInfo *)user;

            OIC_LOG_V(ERROR, TAG, "[LWS] Could not connect to server [%s]",
                      connInfo->secureEndpoint.endpoint.addr);

            if (NULL != in)
            {
                // TODO: is in NULL terminated?
                OIC_LOG_V(ERROR, TAG, "[LWS] Connection error: [%s]", in);
            }

            CAWSRemoveWSConnInfoFromList(&g_connInfoList, connInfo);
            oc_mutex_unlock(g_connInfoListMutex);

            while (connInfo->pendingDataList)
            {
                WSData *wsData = CAWSRemoveWSDataFromList(&(connInfo->pendingDataList));
                g_errorHandleCallback(&(connInfo->secureEndpoint.endpoint),
                                      wsData->data + LWS_PRE, wsData->dataLength, CA_SEND_FAILED);
                CAWSDestroyWSData(wsData);
            }
            CAWSDestroyWSConnInfo(connInfo);
            return 0;

        case LWS_CALLBACK_CLOSED:
            oc_mutex_lock(g_connInfoListMutex);

            if (caglobals.client)
            {
                connInfo = (WSConnInfo *)user;
            }
            else
            {
                connInfo = *((WSConnInfo **)user);
            }

            OIC_LOG_V(DEBUG, TAG, "[LWS] Websocket session ended for [%s]",
                      connInfo->secureEndpoint.endpoint.addr);

            CAWSRemoveWSConnInfoFromList(&g_connInfoList, connInfo);
            oc_mutex_unlock(g_connInfoListMutex);
            g_connectionChangeCallback(&(connInfo->secureEndpoint.endpoint), false);
            CAWSDestroyWSConnInfo(connInfo);
            return 0;

        case LWS_CALLBACK_CLIENT_ESTABLISHED:
            oc_mutex_lock(g_connInfoListMutex);
            connInfo = (WSConnInfo *)user;

            OIC_LOG_V(DEBUG, TAG, "[LWS] Connected to server [%s]",
                      connInfo->secureEndpoint.endpoint.addr);

            connInfo->status = WS_CONN_STATUS_CONNECTED;
            if (connInfo->pendingDataList)
            {
                lws_callback_on_writable(wsi);
            }

            g_connectionChangeCallback(&(connInfo->secureEndpoint.endpoint), true);
            oc_mutex_unlock(g_connInfoListMutex);
            return 0;

        case LWS_CALLBACK_ESTABLISHED:
            OIC_LOG(DEBUG, TAG, "[LWS] New connection accepted!");

            // Create endpoint for peer device
            CAEndpoint_t endpoint;
            memset(&endpoint, 0, sizeof(endpoint));
            endpoint.adapter = CA_ADAPTER_WS;

            CAWSGetPeerAddress(lws_get_socket_fd(wsi),
                               endpoint.addr, &endpoint.port, &endpoint.flags);

            OIC_LOG_V(DEBUG, TAG, "[LWS] Connected client is %s:%d",
                      endpoint.addr, endpoint.port);

            oc_mutex_lock(g_connInfoListMutex);
            connInfo = CAWSFindWSConnInfoFromList(g_connInfoList, &endpoint);

            if (!connInfo)
            {
                connInfo = (WSConnInfo *)OICCalloc(1, sizeof(WSConnInfo));

                if (!connInfo)
                {
                    oc_mutex_unlock(g_connInfoListMutex);
                    OIC_LOG(ERROR, TAG, "Could not allocate websocket connection info");
                    return 1;
                }

                connInfo->secureEndpoint.endpoint = endpoint;
                connInfo->status = WS_CONN_STATUS_CONNECTED;
                connInfo->wsi = wsi;

#ifdef __WITH_TLS__
                // Add Client's identity information
                connInfo->secureEndpoint.identity = CAWSGetPeerIdentity(&endpoint);
#endif
                if (CA_STATUS_OK != CAWSAddWSConnInfoToList(&g_connInfoList, connInfo))
                {
                    oc_mutex_unlock(g_connInfoListMutex);
                    OICFree(connInfo);
                    OIC_LOG(ERROR, TAG, "Could not add websocket connection info to list");
                    return 1;
                }
            }

            *((WSConnInfo **)user) = connInfo;
            g_connectionChangeCallback(&(connInfo->secureEndpoint.endpoint), true);
            oc_mutex_unlock(g_connInfoListMutex);
            return 0;

        case LWS_CALLBACK_SERVER_WRITEABLE:
            oc_mutex_lock(g_connInfoListMutex);
            connInfo = *(WSConnInfo **)user;

        case LWS_CALLBACK_CLIENT_WRITEABLE:
            if (caglobals.client)
            {
                oc_mutex_lock(g_connInfoListMutex);
                connInfo = (WSConnInfo *)user;
            }

            OIC_LOG_V(DEBUG, TAG, "[LWS] Writeable callback for client: \
                      [%d], server: [%d], addr= [%s]", caglobals.client,
                      caglobals.server, connInfo->secureEndpoint.endpoint.addr);

            if (!connInfo->pendingDataList)
            {
                oc_mutex_unlock(g_connInfoListMutex);
                OIC_LOG(DEBUG, TAG, "[LWS] Nothing to write");
                return 0;
            }

            WSData *wsData = CAWSRemoveWSDataFromList(&(connInfo->pendingDataList));
            if (!wsData)
            {
                oc_mutex_unlock(g_connInfoListMutex);
                OIC_LOG(ERROR, TAG, "Could not get data from list");
                return 0;
            }

            int n = lws_write(wsi, (uint8_t *)wsData->data + LWS_PRE, wsData->dataLength,
                              LWS_WRITE_BINARY);

            if (n < 0)
            {
                g_errorHandleCallback(&(connInfo->secureEndpoint.endpoint),
                                      wsData->data + LWS_PRE, wsData->dataLength, CA_SEND_FAILED);
                oc_mutex_unlock(g_connInfoListMutex);
                OIC_LOG(ERROR, TAG, "Fatal error while writing data");
                CAWSDestroyWSData(wsData);
                return 0;
            }
            else if (n < wsData->dataLength)
            {
                g_errorHandleCallback(&(connInfo->secureEndpoint.endpoint),
                                      wsData->data + LWS_PRE, wsData->dataLength, CA_SEND_FAILED);
                oc_mutex_unlock(g_connInfoListMutex);
                OIC_LOG(ERROR, TAG, "Partial write");
                CAWSDestroyWSData(wsData);
                return 0;
            }
            CAWSDestroyWSData(wsData);

            if (connInfo->pendingDataList)
            {
                lws_callback_on_writable(wsi);
            }

            oc_mutex_unlock(g_connInfoListMutex);
            return 0;

        case LWS_CALLBACK_RECEIVE:
            oc_mutex_lock(g_connInfoListMutex);
            connInfo = *(WSConnInfo **)user;

        case LWS_CALLBACK_CLIENT_RECEIVE:
            if (caglobals.client)
            {
                oc_mutex_lock(g_connInfoListMutex);
                connInfo = (WSConnInfo *)user;
            }

            OIC_LOG_V(DEBUG, TAG, "[LWS] Receive callback for client: \
                      [%d], server: [%d]", caglobals.client, caglobals.server);

            if (!connInfo->recvData)
            {
                connInfo->recvData = (WSData *)OICCalloc(1, sizeof(WSData));
                if (!connInfo->recvData)
                {
                    oc_mutex_unlock(g_connInfoListMutex);
                    OIC_LOG(ERROR, TAG, "Could not allocate recvData");
                    return 1;
                }
                connInfo->recvData->data = OICMalloc(len + lws_remaining_packet_payload(wsi));
                if (!connInfo->recvData->data)
                {
                    OICFree(connInfo->recvData);
                    oc_mutex_unlock(g_connInfoListMutex);
                    OIC_LOG(ERROR, TAG, "Could not allocate recvData->data");
                    return 1;
                }
            }
            memcpy((uint8_t *)connInfo->recvData->data + connInfo->recvData->dataLength, in, len);
            connInfo->recvData->dataLength += len;

            if (lws_is_final_fragment(wsi))
            {
                WSData *data = connInfo->recvData;
                connInfo->recvData = NULL;
                OIC_LOG(DEBUG, TAG, "Completed receiving a packet");
                OIC_LOG_BUFFER(DEBUG, TAG, data->data, data->dataLength);
                g_networkPacketCallback(&(connInfo->secureEndpoint), data->data, data->dataLength);
                CAWSDestroyWSData(data);
            }
            oc_mutex_unlock(g_connInfoListMutex);
            return 0;

        case LWS_CALLBACK_CLIENT_RECEIVE_PONG:
            connInfo = (WSConnInfo *)user;
            OIC_LOG_V(DEBUG, TAG, "[LWS] Received pong from server [%s]",
                      connInfo->secureEndpoint.endpoint.addr);
            return 0;

#ifdef __WITH_TLS__
        case LWS_CALLBACK_OPENSSL_LOAD_EXTRA_CLIENT_VERIFY_CERTS:
            OIC_LOG_V(DEBUG, TAG, "[LWS] Received event LWS_CALLBACK_OPENSSL_LOAD_EXTRA_CLIENT_VERIFY_CERTS");
            CAWSInitSSLClient(user);
            return 0;

        case LWS_CALLBACK_OPENSSL_LOAD_EXTRA_SERVER_VERIFY_CERTS:
            OIC_LOG_V(DEBUG, TAG, "[LWS] Received event LWS_CALLBACK_OPENSSL_LOAD_EXTRA_SERVER_VERIFY_CERTS");
            CAWSInitSSLServer(user);
            return 0;
#endif

        default:
            return 0;
    }
}

static CAResult_t CAInitMutexVariables()
{
    OIC_LOG_V(DEBUG, TAG, "%s IN", __func__);

    if (NULL == g_continueLWSServiceMutex)
    {
        g_continueLWSServiceMutex = oc_mutex_new();
        if (NULL == g_continueLWSServiceMutex)
        {
            OIC_LOG(ERROR, TAG, "oc_mutex_new failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == g_LWSServiceStoppedCond)
    {
        g_LWSServiceStoppedCond = oc_cond_new();
        if (NULL == g_LWSServiceStoppedCond)
        {
            OIC_LOG(ERROR, TAG, "oc_cond_new failed");
            return CA_STATUS_FAILED;
        }
    }

    if (NULL == g_connInfoListMutex)
    {
        g_connInfoListMutex = oc_mutex_new();
        if (NULL == g_connInfoListMutex)
        {
            OIC_LOG(ERROR, TAG, "oc_mutex_new failed");
            return CA_STATUS_FAILED;
        }
    }

    return CA_STATUS_OK;
}

static void CATerminateMutexVariables()
{
    OIC_LOG_V(DEBUG, TAG, "%s IN", __func__);

    if (g_continueLWSServiceMutex)
    {
        oc_mutex_free(g_continueLWSServiceMutex);
        g_continueLWSServiceMutex = NULL;
    }

    if (g_LWSServiceStoppedCond)
    {
        oc_cond_free(g_LWSServiceStoppedCond);
        g_LWSServiceStoppedCond = NULL;
    }

    if (g_connInfoListMutex)
    {
        oc_mutex_free(g_connInfoListMutex);
        g_connInfoListMutex = NULL;
    }
}

static CAResult_t CALWSInitClient()
{
    OIC_LOG_V(DEBUG, TAG, "%s IN", __func__);

    struct lws_context_creation_info contextInfo;
    memset(&contextInfo, 0, sizeof(contextInfo));

    contextInfo.port = CONTEXT_PORT_NO_LISTEN;
    contextInfo.protocols = protocols;
    contextInfo.ws_ping_pong_interval = 60;
#ifdef __WITH_TLS__
    contextInfo.options |= LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
#endif

    g_context = lws_create_context(&contextInfo);
    if (!g_context)
    {
        OIC_LOG(ERROR, TAG, "Could not create client context");
        return CA_STATUS_FAILED;
    }
    return CA_STATUS_OK;
}

static CAResult_t CALWSInitServer()
{
    OIC_LOG_V(DEBUG, TAG, "%s IN", __func__);

    protocols[0].per_session_data_size = sizeof(WSConnInfo *);

    struct lws_context_creation_info contextInfo;
    memset(&contextInfo, 0, sizeof(contextInfo));

    uint16_t port = caglobals.ws.u4;
#ifdef __WITH_TLS__
    port = caglobals.ws.u4s;
    contextInfo.options |= LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;

    // TODO: This option is not available in libwebsockets.
    // Patch for this is under review in libwebsockets repo.
    contextInfo.options |= LWS_SERVER_OPTION_SKIP_LOADING_CERT;
#endif

    if (!caglobals.ws.ipv4wsenabled)
    {
        OIC_LOG(DEBUG, TAG, "IPv4 disabled, enabling only ipv6");
        contextInfo.options |= LWS_SERVER_OPTION_IPV6_V6ONLY_VALUE;
    }

    if (!caglobals.ws.ipv6wsenabled)
    {
        OIC_LOG(DEBUG, TAG, "IPv6 disabled");
        contextInfo.options |= LWS_SERVER_OPTION_DISABLE_IPV6;
    }

    contextInfo.port = port;
    contextInfo.protocols = protocols;
    g_context = lws_create_context(&contextInfo);
    if (!g_context)
    {
        OIC_LOG_V(DEBUG, TAG, "Could not create server context with port=%u",
                  port);
        OIC_LOG(DEBUG, TAG, "Try to create context with port 0");

        contextInfo.port = 0;
        g_context = lws_create_context(&contextInfo);
        if (!g_context)
        {
            OIC_LOG(ERROR, TAG, "Could not create server context on port 0");
            return CA_STATUS_FAILED;
        }
    }
#ifdef __WITH_TLS__
    caglobals.ws.u4s = caglobals.ws.u6s = contextInfo.port;
#else
    caglobals.ws.u4 = caglobals.ws.u6 = contextInfo.port;
#endif
    OIC_LOG_V(DEBUG, TAG, "Created server context on port=%u", contextInfo.port);

    return CA_STATUS_OK;
}

static void CALWSConnect(WSConnInfo *connInfo)
{
    OIC_LOG_V(DEBUG, TAG, "%s IN", __func__);

    struct lws_client_connect_info lwsConnInfo;
    memset(&lwsConnInfo, 0, sizeof(lwsConnInfo));

    lwsConnInfo.port = connInfo->secureEndpoint.endpoint.port;
    lwsConnInfo.address = connInfo->secureEndpoint.endpoint.addr;
    lwsConnInfo.path = "/.well-known/coap";
    lwsConnInfo.context = g_context;
    lwsConnInfo.host = "localhost";
    lwsConnInfo.origin = "";
    lwsConnInfo.protocol = "coap";
    lwsConnInfo.userdata = connInfo;
    lwsConnInfo.pwsi = &(connInfo->wsi);

#ifdef __WITH_TLS__
    if (connInfo->secured)
    {
        lwsConnInfo.ssl_connection = 1;
        CAWSSetupCipher(connInfo->secureEndpoint.endpoint.remoteId);
    }
#endif

    lws_client_connect_via_info(&lwsConnInfo);
}

static void CALWSService(void *data)
{
    OIC_LOG_V(DEBUG, TAG, "%s IN", __func__);

    while (true)
    {
        oc_mutex_lock(g_continueLWSServiceMutex);
        if (!g_continueLWSService)
        {
            oc_mutex_unlock(g_continueLWSServiceMutex);
            break;
        }
        oc_mutex_unlock(g_continueLWSServiceMutex);
        lws_service(g_context, 50);
    }
    oc_cond_signal(g_LWSServiceStoppedCond);

    OIC_LOG(DEBUG, TAG, "Stopped LWSService");
}

CAResult_t CAWSStart(const ca_thread_pool_t threadPool)
{
    OIC_LOG_V(DEBUG, TAG, "%s IN", __func__);

    CAResult_t ret = CAInitMutexVariables();
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, TAG, "Failed to initialize mutex variables");
        CATerminateMutexVariables();
        return ret;
    }

#ifdef __WITH_TLS__
    ret = CAWSInitSSLAdapter();
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, TAG, "Failed to initialize security!");
        CATerminateMutexVariables();
        return ret;
    }
#endif

    if (caglobals.client)
    {
        OIC_LOG(DEBUG, TAG, "WS client mode");
        ret = CALWSInitClient();
        if (CA_STATUS_OK != ret)
        {
            OIC_LOG(ERROR, TAG, "Failed to initialize client");
            CATerminateMutexVariables();
            return ret;
        }
    }
    else
    {
        OIC_LOG(DEBUG, TAG, "WS server mode");
        ret = CALWSInitServer();
        if (CA_STATUS_OK != ret)
        {
            OIC_LOG(ERROR, TAG, "Failed to initialize server");
            CATerminateMutexVariables();
            return ret;
        }
    }

    g_continueLWSService = true;
    ret = ca_thread_pool_add_task(threadPool, CALWSService, NULL);
    if (CA_STATUS_OK != ret)
    {
        OIC_LOG(ERROR, TAG, "Could not add CALWSService task to theadPool");
        g_continueLWSService = false;
        CATerminateMutexVariables();
        lws_context_destroy(g_context);
        g_context = NULL;
        return ret;
    }

    return CA_STATUS_OK;
}

CAResult_t CAWSSendData(const CAEndpoint_t *endpoint, const void *data,
                        uint32_t dataLength)
{
    OIC_LOG_V(DEBUG, TAG, "%s IN", __func__);
    OIC_LOG_V(DEBUG, TAG, "CAWSSendData called with addr: %s, port: %u", endpoint->addr,
              endpoint->port);

    CAResult_t ret = CA_STATUS_OK;

    if (!data)
    {
        OIC_LOG(DEBUG, TAG, "data is NULL");
        return CA_STATUS_OK;
    }

    oc_mutex_lock(g_connInfoListMutex);
    WSConnInfo *connInfo = CAWSFindWSConnInfoFromList(g_connInfoList, endpoint);

    if (!caglobals.client && !connInfo)
    {
        oc_mutex_unlock(g_connInfoListMutex);
        OIC_LOG(ERROR, TAG, "Could not find connection information for this endpoint");
        return CA_STATUS_FAILED;
    }

    if (!connInfo)
    {
        connInfo = (WSConnInfo *)OICCalloc(1, sizeof(WSConnInfo));

        if (!connInfo)
        {
            oc_mutex_unlock(g_connInfoListMutex);
            OIC_LOG(ERROR, TAG, "Could not allocate websocket connection info");
            return CA_MEMORY_ALLOC_FAILED;
        }

        connInfo->secureEndpoint.endpoint = *endpoint;
        connInfo->status = WS_CONN_STATUS_DISCONNECTED;

#ifdef __WITH_TLS__
        if (endpoint->flags & CA_SECURE)
        {
            OIC_LOG(DEBUG, TAG, "Its secure endpoint!");
            connInfo->secured = 1;
        }
#endif

        ret = CAWSAddWSConnInfoToList(&g_connInfoList, connInfo);
        if (CA_STATUS_OK != ret)
        {
            oc_mutex_unlock(g_connInfoListMutex);
            OICFree(connInfo);
            OIC_LOG(ERROR, TAG, "Could not add websocket connection info to list");
            return ret;
        }
    }

    WSData *wsData = CAWSPrepareWSData(data, dataLength);

    if (!wsData)
    {
        oc_mutex_unlock(g_connInfoListMutex);
        OIC_LOG(ERROR, TAG, "Could not prepare send data");
        return CA_STATUS_FAILED;
    }

    ret = CAWSAddWSDataToList(&(connInfo->pendingDataList), wsData);
    if (CA_STATUS_OK != ret)
    {
        oc_mutex_lock(g_connInfoListMutex);
        CAWSDestroyWSData(wsData);
        OIC_LOG(ERROR, TAG, "Could not add pending data to list");
        return ret;
    }

    if (connInfo->status == WS_CONN_STATUS_DISCONNECTED && caglobals.client)
    {
        connInfo->status = WS_CONN_STATUS_PENDING;
        CALWSConnect(connInfo);
    }
    else
    {
        lws_callback_on_writable(connInfo->wsi);
    }

    oc_mutex_unlock(g_connInfoListMutex);
    return CA_STATUS_OK;
}

CAResult_t CAWSStop()
{
    OIC_LOG_V(DEBUG, TAG, "%s IN", __func__);

    oc_mutex_lock(g_continueLWSServiceMutex);
    if (g_continueLWSService)
    {
        g_continueLWSService = false;
        lws_cancel_service(g_context);
        oc_cond_wait(g_LWSServiceStoppedCond, g_continueLWSServiceMutex);
    }
    oc_mutex_unlock(g_continueLWSServiceMutex);

    if (NULL != g_context)
    {
        lws_context_destroy(g_context);
        g_context = NULL;
    }

    oc_mutex_lock(g_connInfoListMutex);
    CAWSDestroyWSConnInfoList(g_connInfoList);
    g_connInfoList = NULL;
    oc_mutex_unlock(g_connInfoListMutex);

    CATerminateMutexVariables();

#ifdef __WITH_TLS__
    CAWSDeInitSSLAdapter();
#endif

    return CA_STATUS_OK;
}

CAResult_t CAWSGetInterfaceInformation(CAEndpoint_t **info, size_t *size)
{
    VERIFY_NON_NULL(info, TAG, "info is NULL");
    VERIFY_NON_NULL(size, TAG, "size is NULL");

    u_arraylist_t *iflist = CAIPGetInterfaceInformation(0);
    if (!iflist)
    {
        OIC_LOG_V(ERROR, TAG, "get interface info failed: %s", strerror(errno));
        return CA_STATUS_FAILED;
    }

#ifdef __WITH_TLS__
    const size_t endpointsPerInterface = 2;
#else
    const size_t endpointsPerInterface = 1;
#endif

    size_t interfaces = u_arraylist_length(iflist);
    for (size_t i = 0; i < u_arraylist_length(iflist); i++)
    {
        CAInterface_t *ifitem = (CAInterface_t *)u_arraylist_get(iflist, i);
        if (!ifitem)
        {
            continue;
        }

        if ((ifitem->family == AF_INET6 && !caglobals.ip.ipv6enabled) ||
            (ifitem->family == AF_INET && !caglobals.ip.ipv4enabled))
        {
            interfaces--;
        }
    }

    if (!interfaces)
    {
        OIC_LOG(DEBUG, TAG, "network interface size is zero");
        return CA_STATUS_OK;
    }

    size_t totalEndpoints = interfaces * endpointsPerInterface;
    CAEndpoint_t *ep = (CAEndpoint_t *)OICCalloc(totalEndpoints, sizeof (CAEndpoint_t));
    if (!ep)
    {
        OIC_LOG(ERROR, TAG, "Malloc Failed");
        u_arraylist_destroy(iflist);
        return CA_MEMORY_ALLOC_FAILED;
    }

    for (size_t i = 0, j = 0; i < u_arraylist_length(iflist); i++)
    {
        CAInterface_t *ifitem = (CAInterface_t *)u_arraylist_get(iflist, i);
        if (!ifitem)
        {
            continue;
        }

        if ((ifitem->family == AF_INET6 && !caglobals.ip.ipv6enabled) ||
            (ifitem->family == AF_INET && !caglobals.ip.ipv4enabled))
        {
            continue;
        }
        ep[j].adapter = CA_ADAPTER_WS;
        ep[j].ifindex = ifitem->index;

        if (ifitem->family == AF_INET6)
        {
            ep[j].flags = CA_IPV6;
            ep[j].port = caglobals.ws.u6;
        }
        else if (ifitem->family == AF_INET)
        {
            ep[j].flags = CA_IPV4;
            ep[j].port = caglobals.ws.u4;
        }
        else
        {
            continue;
        }
        OICStrcpy(ep[j].addr, sizeof(ep[j].addr), ifitem->addr);

#ifdef __WITH_TLS__
        j++;

        ep[j].adapter = CA_ADAPTER_WS;
        ep[j].ifindex = ifitem->index;

        if (ifitem->family == AF_INET6)
        {
            ep[j].flags = CA_IPV6 | CA_SECURE;
            ep[j].port = caglobals.ws.u6s;
        }
        else
        {
            ep[j].flags = CA_IPV4 | CA_SECURE;
            ep[j].port = caglobals.ws.u4s;
        }
        OICStrcpy(ep[j].addr, sizeof(ep[j].addr), ifitem->addr);
#endif
        j++;
    }

    *info = ep;
    *size = totalEndpoints;

    u_arraylist_destroy(iflist);

    return CA_STATUS_OK;
}

