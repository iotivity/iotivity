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
#include "caadapternetdtls.h"
#include "cacommon.h"
#include "caipinterface.h"
#include "dtls.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "global.h"
#include <netdb.h>

/**
 * @def NET_DTLS_TAG
 * @brief Logging tag for module name
 */
#define NET_DTLS_TAG "NET_DTLS"

/**
 * @var g_caDtlsContext
 * @brief global context which holds dtls context and cache list information.
 */
static stCADtlsContext_t *g_caDtlsContext = NULL;

/**
 * @var g_dtlsContextMutex
 * @brief Mutex to synchronize access to g_caDtlsContext.
 */
static ca_mutex g_dtlsContextMutex = NULL;

/**
 * @var g_getCredentialsCallback
 * @brief callback to get DTLS credentials
 */
static CAGetDTLSCredentialsHandler g_getCredentialsCallback = NULL;

static CAEndpoint_t *GetPeerInfo(const CAEndpoint_t *peer)
{
    uint32_t list_index = 0;
    uint32_t list_length = 0;

    if(NULL == peer)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "CAPeerInfoListContains invalid parameters");
        return NULL;
    }

    CAEndpoint_t *peerInfo;
    list_length = u_arraylist_length(g_caDtlsContext->peerInfoList);
    for (list_index = 0; list_index < list_length; list_index++)
    {
        peerInfo = (CAEndpoint_t *)u_arraylist_get(g_caDtlsContext->peerInfoList, list_index);
        if (NULL == peerInfo)
        {
            continue;
        }

        if((0 == strncmp(peer->addr, peerInfo->addr, MAX_ADDR_STR_SIZE_CA)) &&
                (peer->port == peerInfo->port))
        {
            return peerInfo;
        }
    }
    return NULL;
}

static CAResult_t CAAddIdToPeerInfoList(const char *peerAddr, uint32_t port,
                                    const unsigned char *id, uint16_t id_length)
{
    if(NULL == peerAddr
       || NULL == id
       || 0 == port
       || 0 == id_length
       || CA_MAX_ENDPOINT_IDENTITY_LEN < id_length)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "CAAddIdToPeerInfoList invalid parameters");
        return CA_STATUS_INVALID_PARAM;
    }

    CAEndpoint_t *peer = (CAEndpoint_t *)OICCalloc(1, sizeof (CAEndpoint_t));
    if (NULL == peer)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "peerInfo malloc failed!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    OICStrcpy(peer->addr, sizeof(peer->addr), peerAddr);
    peer->port = port;

    memcpy(peer->identity.id, id, id_length);
    peer->identity.id_length = id_length;

    if(NULL != GetPeerInfo(peer))
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "CAAddIdToPeerInfoList peer already exist");
        OICFree(peer);
        return CA_STATUS_FAILED;
    }

    CAResult_t result = u_arraylist_add(g_caDtlsContext->peerInfoList, (void *)peer);
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "u_arraylist_add failed!");
        OICFree(peer);
    }

    return result;
}

static void CAFreePeerInfoList()
{
    uint32_t list_length = u_arraylist_length(g_caDtlsContext->peerInfoList);
    for (uint32_t list_index = 0; list_index < list_length; list_index++)
    {
        CAEndpoint_t *peerInfo = (CAEndpoint_t *)u_arraylist_get(
                                     g_caDtlsContext->peerInfoList, list_index);
        OICFree(peerInfo);
    }
    u_arraylist_free(&(g_caDtlsContext->peerInfoList));
    g_caDtlsContext->peerInfoList = NULL;
}

static void CARemovePeerFromPeerInfoList(const char * addr, uint32_t port)
{
    if (NULL == addr || 0 >= port)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "CADTLSGetPeerPSKId invalid parameters");
        return;
    }

    uint32_t list_length = u_arraylist_length(g_caDtlsContext->peerInfoList);
    for (uint32_t list_index = 0; list_index < list_length; list_index++)
    {
        CAEndpoint_t *peerInfo = (CAEndpoint_t *)u_arraylist_get(
                                g_caDtlsContext->peerInfoList,list_index);
        if (NULL == peerInfo)
        {
            continue;
        }
        if((0 == strncmp(addr, peerInfo->addr, MAX_ADDR_STR_SIZE_CA)) &&
                (port == peerInfo->port))
        {
            OICFree(u_arraylist_remove(g_caDtlsContext->peerInfoList, list_index));
            return;
        }
    }
}

static int CASizeOfAddrInfo(stCADtlsAddrInfo_t *addrInfo)
{
    VERIFY_NON_NULL_RET(addrInfo, NET_DTLS_TAG, "addrInfo is NULL" , DTLS_FAIL);

    switch (addrInfo->addr.st.ss_family)
    {
    case AF_INET:
        {
            return sizeof (struct sockaddr_in);
        }
    case AF_INET6:
        {
            return sizeof (struct sockaddr_in6);
        }
    default:
        {
            break;
        }
    }
    return sizeof (struct sockaddr_storage);
}

static eDtlsRet_t CAAdapterNetDtlsEncryptInternal(const stCADtlsAddrInfo_t *dstSession,
        uint8_t *data, uint32_t dataLen)
{
    OIC_LOG(DEBUG, NET_DTLS_TAG, "IN");

    VERIFY_NON_NULL_RET(dstSession, NET_DTLS_TAG, "Param dstSession is NULL" , DTLS_FAIL);
    VERIFY_NON_NULL_RET(data, NET_DTLS_TAG, "Param data is NULL" , DTLS_FAIL);

    if (0 == dataLen)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Given Packet length is equal to zero.");
        return DTLS_FAIL;
    }

    if (NULL == g_caDtlsContext)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Context is NULL");
        return DTLS_FAIL;
    }

    int retLen = dtls_write(g_caDtlsContext->dtlsContext, (session_t *)dstSession, data,
                                dataLen);
    OIC_LOG_V(DEBUG, NET_DTLS_TAG, "dtls_write retun len [%d]", retLen);
    if (0 == retLen)
    {
        // A new DTLS session was initiated by tinyDTLS library and wait for callback.
        return DTLS_SESSION_INITIATED;
    }
    else if (dataLen == retLen)
    {
        OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT");
        return DTLS_OK;
    }
    OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT FAILURE");
    return DTLS_FAIL;
}

static eDtlsRet_t CAAdapterNetDtlsDecryptInternal(const stCADtlsAddrInfo_t *srcSession,
        uint8_t *buf, uint32_t bufLen)
{
    OIC_LOG(DEBUG, NET_DTLS_TAG, "IN");

    VERIFY_NON_NULL_RET(srcSession, NET_DTLS_TAG, "Param srcSession is NULL", DTLS_FAIL);
    VERIFY_NON_NULL_RET(buf, NET_DTLS_TAG, "Param buf is NULL", DTLS_FAIL);

    if (0 == bufLen)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Given Packet length is equal to zero.");
        return DTLS_FAIL;
    }

    eDtlsRet_t ret = DTLS_FAIL;

    if (dtls_handle_message(g_caDtlsContext->dtlsContext, (session_t *)srcSession, buf, bufLen) == 0)
    {
        OIC_LOG(DEBUG, NET_DTLS_TAG, "dtls_handle_message success");
        ret = DTLS_OK;
    }

    OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT");
    return ret;
}

static void CAFreeCacheMsg(stCACacheMessage_t *msg)
{
    OIC_LOG(DEBUG, NET_DTLS_TAG, "IN");
    VERIFY_NON_NULL_VOID(msg, NET_DTLS_TAG, "msg");

    OICFree(msg->data);
    OICFree(msg);

    OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT");
}

static void CAClearCacheList()
{
    OIC_LOG(DEBUG, NET_DTLS_TAG, "IN");
    uint32_t list_index = 0;
    uint32_t list_length = 0;
    if (NULL == g_caDtlsContext)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Dtls Context is NULL");
        return;
    }
    list_length = u_arraylist_length(g_caDtlsContext->cacheList);
    for (list_index = 0; list_index < list_length; list_index++)
    {
        stCACacheMessage_t *msg = (stCACacheMessage_t *)u_arraylist_get(g_caDtlsContext->cacheList,
                                  list_index);
        if (msg != NULL)
        {
            CAFreeCacheMsg(msg);
        }
    }
    u_arraylist_free(&g_caDtlsContext->cacheList);
    g_caDtlsContext->cacheList = NULL;
    OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT");
}

static CAResult_t CADtlsCacheMsg(stCACacheMessage_t *msg)
{
    OIC_LOG(DEBUG, NET_DTLS_TAG, "IN");

    if (NULL == g_caDtlsContext)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Dtls Context is NULL");
        return CA_STATUS_FAILED;
    }

    CAResult_t result = u_arraylist_add(g_caDtlsContext->cacheList, (void *)msg);
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "u_arraylist_add failed!");
    }

    OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT");
    return result;
}


static bool CAIsAddressMatching(const stCADtlsAddrInfo_t *a,  const stCADtlsAddrInfo_t *b)
{
    if (a->size != b->size)
    {
        return false;
    }
    if (memcmp(&a->addr, &b->addr, a->size))
    {
        return false;
    }
    return true;
}

static void CASendCachedMsg(const stCADtlsAddrInfo_t *dstSession)
{
    OIC_LOG(DEBUG, NET_DTLS_TAG, "IN");
    VERIFY_NON_NULL_VOID(dstSession, NET_DTLS_TAG, "Param dstSession is NULL");

    uint32_t list_index = 0;
    uint32_t list_length = 0;
    list_length = u_arraylist_length(g_caDtlsContext->cacheList);
    for (list_index = 0; list_index < list_length;)
    {
        stCACacheMessage_t *msg = (stCACacheMessage_t *)u_arraylist_get(g_caDtlsContext->cacheList,
                                  list_index);
        if ((NULL != msg) && (true == CAIsAddressMatching(&(msg->destSession), dstSession)))
        {
            eDtlsRet_t ret = CAAdapterNetDtlsEncryptInternal(&(msg->destSession),
                             msg->data, msg->dataLen);
            if (ret == DTLS_OK)
            {
                OIC_LOG(DEBUG, NET_DTLS_TAG, "CAAdapterNetDtlsEncryptInternal success");
            }
            else
            {
                OIC_LOG(ERROR, NET_DTLS_TAG, "CAAdapterNetDtlsEncryptInternal failed.");
            }

            if (u_arraylist_remove(g_caDtlsContext->cacheList, list_index))
            {
                CAFreeCacheMsg(msg);
                // Reduce list length by 1 as we removed one element.
                list_length--;
            }
            else
            {
                OIC_LOG(ERROR, NET_DTLS_TAG, "u_arraylist_remove failed.");
                break;
            }
        }
        else
        {
            // Move to the next element
            ++list_index;
        }
    }

    OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT");
}

static int32_t CAReadDecryptedPayload(dtls_context_t *dtlsContext,
                                      session_t *session,
                                      uint8_t *buf,
                                      size_t bufLen )
{
    OIC_LOG(DEBUG, NET_DTLS_TAG, "IN");

    VERIFY_NON_NULL_RET(session, NET_DTLS_TAG, "Param Session is NULL", 0);
    OIC_LOG_V(DEBUG, NET_DTLS_TAG, "Decrypted buf len [%d]", bufLen);

    stCADtlsAddrInfo_t *addrInfo = (stCADtlsAddrInfo_t *)session;

    CAEndpoint_t endpoint = { 0 };
    CAConvertAddrToName(&(addrInfo->addr.st), endpoint.addr, &endpoint.port);
    endpoint.flags = addrInfo->addr.st.ss_family == AF_INET ? CA_IPV4 : CA_IPV6;
    endpoint.flags |= CA_SECURE;
    endpoint.adapter = CA_ADAPTER_IP;
    int type = 0;

    if (NULL == g_caDtlsContext)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Context is NULL");
        return 0;
    }

    if ((0 <= type) && (MAX_SUPPORTED_ADAPTERS > type) &&
        (NULL != g_caDtlsContext->adapterCallbacks[type].recvCallback))
    {
        // Get identity of the source of packet
        CAEndpoint_t *peerInfo = GetPeerInfo(&endpoint);
        if (peerInfo)
        {
            endpoint.identity = peerInfo->identity;
        }

        g_caDtlsContext->adapterCallbacks[type].recvCallback(&endpoint, buf, bufLen);
    }
    else
    {
        OIC_LOG_V(DEBUG, NET_DTLS_TAG, "recvCallback Callback or adapter type is wrong [%d]", type);
    }

    OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT");
    return 0;
}

static int32_t CASendSecureData(dtls_context_t *dtlsContext,
                                session_t *session,
                                uint8_t *buf,
                                size_t bufLen)
{
    OIC_LOG(DEBUG, NET_DTLS_TAG, "IN");

    VERIFY_NON_NULL_RET(session, NET_DTLS_TAG, "Param Session is NULL", -1);
    VERIFY_NON_NULL_RET(buf, NET_DTLS_TAG, "Param buf is NULL", -1);

    if (0 == bufLen)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Encrypted Buffer length is equal to zero");
        return 0;
    }

    stCADtlsAddrInfo_t *addrInfo = (stCADtlsAddrInfo_t *)session;

    CAEndpoint_t endpoint;
    CAConvertAddrToName(&(addrInfo->addr.st), endpoint.addr, &endpoint.port);
    endpoint.flags = addrInfo->addr.st.ss_family == AF_INET ? CA_IPV4 : CA_IPV6;
    endpoint.flags |= CA_SECURE;
    endpoint.adapter = CA_ADAPTER_IP;
    int type = 0;

    //Mutex is not required for g_caDtlsContext. It will be called in same thread.
    if ((0 <= type) && (MAX_SUPPORTED_ADAPTERS > type) &&
        (NULL != g_caDtlsContext->adapterCallbacks[type].sendCallback))
    {
        g_caDtlsContext->adapterCallbacks[type].sendCallback(&endpoint, buf, bufLen);
    }
    else
    {
        OIC_LOG_V(DEBUG, NET_DTLS_TAG, "send Callback or adapter type is wrong [%d]", type );
    }

    OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT");
    return bufLen;
}

static int32_t CAHandleSecureEvent(dtls_context_t *dtlsContext,
                                   session_t *session,
                                   dtls_alert_level_t level,
                                   unsigned short code)
{
    OIC_LOG(DEBUG, NET_DTLS_TAG, "IN");

    VERIFY_NON_NULL_RET(session, NET_DTLS_TAG, "Param Session is NULL", 0);

    OIC_LOG_V(DEBUG, NET_DTLS_TAG, "level [%d] code [%u]", level, code);

    if (!level && (code == DTLS_EVENT_CONNECTED))
    {
        OIC_LOG(DEBUG, NET_DTLS_TAG, "Received DTLS_EVENT_CONNECTED. Sending Cached data");
        CASendCachedMsg((stCADtlsAddrInfo_t *)session);
    }

    if(DTLS_ALERT_LEVEL_FATAL == level && DTLS_ALERT_CLOSE_NOTIFY == code)
    {
        OIC_LOG(INFO, NET_DTLS_TAG, "Peer closing connection");

        stCADtlsAddrInfo_t *addrInfo = (stCADtlsAddrInfo_t *)session;
        char *peerAddr = inet_ntoa(addrInfo->addr.sin.sin_addr);
        uint32_t port = ntohs(addrInfo->addr.sin.sin_port);

        CARemovePeerFromPeerInfoList(peerAddr, port);
    }

    OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT");
    return 0;
}


static int32_t CAGetPskCredentials(dtls_context_t *ctx,
                                   const session_t *session,
                                   dtls_credentials_type_t type,
                                   const unsigned char *desc, size_t descLen,
                                   unsigned char *result, size_t resultLen)
{
    OIC_LOG(DEBUG, NET_DTLS_TAG, "IN");

    int32_t ret  = -1;
    if(NULL == ctx || NULL == session || NULL == result)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "CAGetPskCredentials invalid parameters");
        return ret;
    }

    VERIFY_NON_NULL_RET(g_getCredentialsCallback, NET_DTLS_TAG, "GetCredential callback", -1);
    VERIFY_NON_NULL_RET(result, NET_DTLS_TAG, "result", -1);

    CADtlsPskCredsBlob_t *credInfo = NULL;

    // Retrieve the credentials blob from security module
    g_getCredentialsCallback(&credInfo);

    VERIFY_NON_NULL_RET(credInfo, NET_DTLS_TAG, "credInfo is NULL", -1);
    if(NULL == credInfo->creds)
    {
        OIC_LOG(DEBUG, NET_DTLS_TAG, "credentials are NULL");
        memset(credInfo, 0, sizeof(CADtlsPskCredsBlob_t));
        OICFree(credInfo);
        return -1;
    }

    if ((type == DTLS_PSK_HINT) || (type == DTLS_PSK_IDENTITY))
    {
        if (DTLS_PSK_ID_LEN <= resultLen)
        {
            memcpy(result, credInfo->identity, DTLS_PSK_ID_LEN);
            ret = DTLS_PSK_ID_LEN;
        }
    }

    if ((type == DTLS_PSK_KEY) && (desc) && (descLen == DTLS_PSK_PSK_LEN))
    {
        // Check if we have the credentials for the device with which we
        // are trying to perform a handshake
        int index = 0;
        for (index = 0; index < credInfo->num; index++)
        {
            if (memcmp(desc, credInfo->creds[index].id, DTLS_PSK_ID_LEN) == 0)
            {
                if(NULL != ctx->peers && DTLS_SERVER == ctx->peers->role )
                {
                    // TODO SRM needs identity of the remote end-point with every data packet to
                    // perform access control management. tinyDTLS 'frees' the handshake parameters
                    // data structure when handshake completes. Therefore, currently this is a
                    // workaround to cache remote end-point identity when tinyDTLS asks for PSK.
                    stCADtlsAddrInfo_t *addrInfo = (stCADtlsAddrInfo_t *)session;
                    char *peerAddress = inet_ntoa(addrInfo->addr.sin.sin_addr);
                    uint32_t port = ntohs(addrInfo->addr.sin.sin_port);

                    CAResult_t result = CAAddIdToPeerInfoList(peerAddress, port, desc, descLen);
                    if(CA_STATUS_OK != result )
                    {
                        OIC_LOG(ERROR, NET_DTLS_TAG, "Fail to add peer id to gDtlsPeerInfoList");
                    }
                }
                memcpy(result, credInfo->creds[index].psk, DTLS_PSK_PSK_LEN);
                ret = DTLS_PSK_PSK_LEN;
            }
        }
    }

    // Erase sensitive data before freeing.
    memset(credInfo->creds, 0, sizeof(OCDtlsPskCreds) * (credInfo->num));
    OICFree(credInfo->creds);

    memset(credInfo, 0, sizeof(CADtlsPskCredsBlob_t));
    OICFree(credInfo);
    credInfo = NULL;

    return ret;
}

void CADTLSSetAdapterCallbacks(CAPacketReceivedCallback recvCallback,
                               CAPacketSendCallback sendCallback,
                               CATransportAdapter_t type)
{
    OIC_LOG(DEBUG, NET_DTLS_TAG, "IN");
    ca_mutex_lock(g_dtlsContextMutex);
    if (NULL == g_caDtlsContext)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Context is NULL");
        ca_mutex_unlock(g_dtlsContextMutex);
        return;
    }

    if ((0 <= type) && (MAX_SUPPORTED_ADAPTERS > type))
    {
        // TODO: change the zeros to better values.
        g_caDtlsContext->adapterCallbacks[0].recvCallback = recvCallback;
        g_caDtlsContext->adapterCallbacks[0].sendCallback = sendCallback;
    }

    ca_mutex_unlock(g_dtlsContextMutex);

    OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT");
}

void CADTLSSetCredentialsCallback(CAGetDTLSCredentialsHandler credCallback)
{
    // TODO Does this method needs protection of DtlsContextMutex ?
    OIC_LOG(DEBUG, NET_DTLS_TAG, "IN");
    g_getCredentialsCallback = credCallback;
    OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT");
}

CAResult_t CADtlsSelectCipherSuite(const dtls_cipher_t cipher)
{
    OIC_LOG(DEBUG, NET_DTLS_TAG, "IN CADtlsSelectCipherSuite");

    ca_mutex_lock(g_dtlsContextMutex);
    if (NULL == g_caDtlsContext)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Context is NULL");
        ca_mutex_unlock(g_dtlsContextMutex);
        return CA_STATUS_FAILED;
    }
    dtls_select_cipher(g_caDtlsContext->dtlsContext, cipher);
    ca_mutex_unlock(g_dtlsContextMutex);

    OIC_LOG_V(DEBUG, NET_DTLS_TAG, "Selected cipher suite is 0x%02X%02X\n",
        ((uint8_t*)(&cipher))[1], ((uint8_t*)(&cipher))[0]);
    OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT CADtlsSelectCipherSuite");

    return CA_STATUS_OK ;
}

CAResult_t CADtlsEnableAnonECDHCipherSuite(const bool enable)
{
    OIC_LOG(DEBUG, NET_DTLS_TAG, "IN CADtlsEnablesAnonEcdh");

    ca_mutex_lock(g_dtlsContextMutex);
    if (NULL == g_caDtlsContext)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Context is NULL");
        ca_mutex_unlock(g_dtlsContextMutex);
        return CA_STATUS_FAILED;
    }
    dtls_enables_anon_ecdh(g_caDtlsContext->dtlsContext,
        enable == true ? DTLS_CIPHER_ENABLE : DTLS_CIPHER_DISABLE);
    ca_mutex_unlock(g_dtlsContextMutex);
    OIC_LOG_V(DEBUG, NET_DTLS_TAG, "TLS_ECDH_anon_WITH_AES_128_CBC_SHA  is %s",
        enable ? "enabled" : "disabled");

    OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT CADtlsEnablesAnonEcdh");

    return CA_STATUS_OK ;
}

CAResult_t CADtlsInitiateHandshake(const CAEndpoint_t *endpoint)
{
    stCADtlsAddrInfo_t dst = {};

    OIC_LOG(DEBUG, NET_DTLS_TAG, "IN CADtlsInitiateHandshake");

    if(!endpoint)
    {
        return CA_STATUS_INVALID_PARAM;
    }

    CAConvertNameToAddr(endpoint->addr, endpoint->port, &(dst.addr.st));
    dst.ifIndex = 0;
    dst.size = CASizeOfAddrInfo(&dst);

    ca_mutex_lock(g_dtlsContextMutex);
    if(NULL == g_caDtlsContext)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Context is NULL");
        ca_mutex_unlock(g_dtlsContextMutex);
        return CA_STATUS_FAILED;
    }

    if(0 > dtls_connect(g_caDtlsContext->dtlsContext, (session_t*)(&dst)))
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Failed to connect");
        ca_mutex_unlock(g_dtlsContextMutex);
        return CA_STATUS_FAILED;
    }

    ca_mutex_unlock(g_dtlsContextMutex);

    OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT CADtlsInitiateHandshake");

    return CA_STATUS_OK;
}

CAResult_t CADtlsClose(const CAEndpoint_t *endpoint)
{
    stCADtlsAddrInfo_t dst = {};

    OIC_LOG(DEBUG, NET_DTLS_TAG, "IN CADtlsDisconnect");

    if(!endpoint)
    {
        return CA_STATUS_INVALID_PARAM;
    }

    CAConvertNameToAddr(endpoint->addr, endpoint->port, &(dst.addr.st));
    dst.ifIndex = 0;
    dst.size = CASizeOfAddrInfo(&dst);

    ca_mutex_lock(g_dtlsContextMutex);
    if (NULL == g_caDtlsContext)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Context is NULL");
        ca_mutex_unlock(g_dtlsContextMutex);
        return CA_STATUS_FAILED;
    }

    if (0 > dtls_close(g_caDtlsContext->dtlsContext, (session_t*)(&dst)))
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Failed to close the session");
        ca_mutex_unlock(g_dtlsContextMutex);
        return CA_STATUS_FAILED;
    }

    ca_mutex_unlock(g_dtlsContextMutex);

    OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT CADtlsDisconnect");

    return CA_STATUS_OK;
}

CAResult_t CADtlsGenerateOwnerPSK(const CAEndpoint_t *endpoint,
                    const uint8_t* label, const size_t labelLen,
                    const uint8_t* rsrcServerDeviceID, const size_t rsrcServerDeviceIDLen,
                    const uint8_t* provServerDeviceID, const size_t provServerDeviceIDLen,
                    uint8_t* ownerPSK, const size_t ownerPSKSize)
{
    OIC_LOG(DEBUG, NET_DTLS_TAG, "IN CADtlsGenerateOwnerPSK");

    if(!endpoint || !label || 0 == labelLen || !ownerPSK || 0 == ownerPSKSize)
    {
        return CA_STATUS_INVALID_PARAM;
    }

    stCADtlsAddrInfo_t dst = {};

    CAConvertNameToAddr(endpoint->addr, endpoint->port, &(dst.addr.st));
    dst.ifIndex = 0;
    dst.size = CASizeOfAddrInfo(&dst);

    ca_mutex_lock(g_dtlsContextMutex);
    if (NULL == g_caDtlsContext)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Context is NULL");
        ca_mutex_unlock(g_dtlsContextMutex);
        return CA_STATUS_FAILED;
    }

    if( 0 == dtls_prf_with_current_keyblock(g_caDtlsContext->dtlsContext, (session_t*)(&dst),
                 label, labelLen, rsrcServerDeviceID, rsrcServerDeviceIDLen,
                 provServerDeviceID, provServerDeviceIDLen, ownerPSK, ownerPSKSize))
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Failed to DTLS PRF");
        ca_mutex_unlock(g_dtlsContextMutex);
        return CA_STATUS_FAILED;
    }
    ca_mutex_unlock(g_dtlsContextMutex);

    OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT CADtlsGenerateOwnerPSK");

    return CA_STATUS_OK;
}

CAResult_t CAAdapterNetDtlsInit()
{
    OIC_LOG(DEBUG, NET_DTLS_TAG, "IN");

    // Initialize mutex for DtlsContext
    if (NULL == g_dtlsContextMutex)
    {
        g_dtlsContextMutex = ca_mutex_new();
        VERIFY_NON_NULL_RET(g_dtlsContextMutex, NET_DTLS_TAG, "malloc failed",
            CA_MEMORY_ALLOC_FAILED);
    }
    else
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "CAAdapterNetDtlsInit done already!");
        return CA_STATUS_OK;
    }

    // Lock DtlsContext mutex and create DtlsContext
    ca_mutex_lock(g_dtlsContextMutex);
    g_caDtlsContext = (stCADtlsContext_t *)OICCalloc(1, sizeof(stCADtlsContext_t));

    if (NULL == g_caDtlsContext)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Context malloc failed");
        ca_mutex_unlock(g_dtlsContextMutex);
        ca_mutex_free(g_dtlsContextMutex);
        return CA_MEMORY_ALLOC_FAILED;
    }


    // Create PeerInfoList and CacheList
    g_caDtlsContext->peerInfoList = u_arraylist_create();
    g_caDtlsContext->cacheList = u_arraylist_create();

    if( (NULL == g_caDtlsContext->peerInfoList) ||
        (NULL == g_caDtlsContext->cacheList))
    {
    OIC_LOG(ERROR, NET_DTLS_TAG, "peerInfoList or cacheList initialization failed!");
        CAClearCacheList();
        CAFreePeerInfoList();
        OICFree(g_caDtlsContext);
        g_caDtlsContext = NULL;
        ca_mutex_unlock(g_dtlsContextMutex);
        ca_mutex_free(g_dtlsContextMutex);
        return CA_STATUS_FAILED;
    }

    // Initialize clock, crypto and other global vars in tinyDTLS library
    dtls_init();

    // Create tinydtls Context
    g_caDtlsContext->dtlsContext = dtls_new_context(g_caDtlsContext);

    if (NULL ==  g_caDtlsContext->dtlsContext)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "dtls_new_context failed");
        ca_mutex_unlock(g_dtlsContextMutex);
        CAAdapterNetDtlsDeInit();
        return CA_STATUS_FAILED;
    }

    g_caDtlsContext->callbacks.write = CASendSecureData;
    g_caDtlsContext->callbacks.read  = CAReadDecryptedPayload;
    g_caDtlsContext->callbacks.event = CAHandleSecureEvent;
    g_caDtlsContext->callbacks.get_psk_info = CAGetPskCredentials;

    dtls_set_handler(g_caDtlsContext->dtlsContext, &(g_caDtlsContext->callbacks));
    ca_mutex_unlock(g_dtlsContextMutex);
    OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT");
    return CA_STATUS_OK;
}

void CAAdapterNetDtlsDeInit()
{
    OIC_LOG(DEBUG, NET_DTLS_TAG, "IN");

    VERIFY_NON_NULL_VOID(g_caDtlsContext, NET_DTLS_TAG, "context is NULL");
    VERIFY_NON_NULL_VOID(g_dtlsContextMutex, NET_DTLS_TAG, "context mutex is NULL");

    //Lock DtlsContext mutex
    ca_mutex_lock(g_dtlsContextMutex);

    // Clear all lists
    CAFreePeerInfoList();
    CAClearCacheList();

    // De-initialize tinydtls context
    dtls_free_context(g_caDtlsContext->dtlsContext);
    g_caDtlsContext->dtlsContext = NULL;

    // De-initialize DtlsContext
    OICFree(g_caDtlsContext);
    g_caDtlsContext = NULL;

    // Unlock DtlsContext mutex and de-initialize it
    ca_mutex_unlock(g_dtlsContextMutex);
    ca_mutex_free(g_dtlsContextMutex);
    g_dtlsContextMutex = NULL;

    OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT");
}

CAResult_t CAAdapterNetDtlsEncrypt(const CAEndpoint_t *endpoint,
                                   void *data, uint32_t dataLen)
{

    OIC_LOG(DEBUG, NET_DTLS_TAG, "IN");

    VERIFY_NON_NULL_RET(endpoint, NET_DTLS_TAG,"Param remoteAddress is NULL",
                        CA_STATUS_INVALID_PARAM);
    VERIFY_NON_NULL_RET(data, NET_DTLS_TAG, "Param data is NULL" ,
                        CA_STATUS_INVALID_PARAM);

    if (0 == dataLen)
    {
        OIC_LOG_V(ERROR, NET_DTLS_TAG, "dataLen is less than or equal zero [%d]", dataLen);
        return CA_STATUS_FAILED;
    }

    OIC_LOG_V(DEBUG, NET_DTLS_TAG, "Data to be encrypted dataLen [%d]", dataLen);

    stCADtlsAddrInfo_t addrInfo = {};

    CAConvertNameToAddr(endpoint->addr, endpoint->port, &(addrInfo.addr.st));
    addrInfo.ifIndex = 0;
    addrInfo.size = CASizeOfAddrInfo(&addrInfo);

    ca_mutex_lock(g_dtlsContextMutex);
    if(NULL == g_caDtlsContext)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Context is NULL");
        ca_mutex_unlock(g_dtlsContextMutex);
        return CA_STATUS_FAILED;
    }

    eDtlsRet_t ret = CAAdapterNetDtlsEncryptInternal(&addrInfo, data, dataLen);
    if (ret == DTLS_SESSION_INITIATED)
    {
        stCACacheMessage_t *message = (stCACacheMessage_t *)OICCalloc(1, sizeof(stCACacheMessage_t));
        if (NULL == message)
        {
            OIC_LOG(ERROR, NET_DTLS_TAG, "calloc failed!");
            ca_mutex_unlock(g_dtlsContextMutex);
            return CA_MEMORY_ALLOC_FAILED;
        }

        message->data = (uint8_t *)OICCalloc(dataLen + 1, sizeof(uint8_t));
        if (NULL == message->data)
        {
            OIC_LOG(ERROR, NET_DTLS_TAG, "calloc failed!");
            OICFree(message);
            ca_mutex_unlock(g_dtlsContextMutex);
            return CA_MEMORY_ALLOC_FAILED;
        }
        memcpy(message->data, data, dataLen);
        message->dataLen = dataLen;
        message->destSession = addrInfo;

        CAResult_t result = CADtlsCacheMsg(message);
        if (CA_STATUS_OK != result)
        {
            OIC_LOG(DEBUG, NET_DTLS_TAG, "CADtlsCacheMsg failed!");
            CAFreeCacheMsg(message);
        }
        OIC_LOG_V(DEBUG, NET_DTLS_TAG, "OUT Initiating Dtls session [%d]", result);
        ca_mutex_unlock(g_dtlsContextMutex);
        return result;
    }

    ca_mutex_unlock(g_dtlsContextMutex);

    if (ret != DTLS_OK)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "OUT FAILURE");
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CAAdapterNetDtlsDecrypt(const CAEndpoint_t *endpoint,
                                   uint8_t *data, uint32_t dataLen)
{
    OIC_LOG(DEBUG, NET_DTLS_TAG, "IN");
    VERIFY_NON_NULL_RET(endpoint, NET_DTLS_TAG, "endpoint is NULL" , CA_STATUS_INVALID_PARAM);

    stCADtlsAddrInfo_t addrInfo = {};

    CAConvertNameToAddr(endpoint->addr, endpoint->port, &(addrInfo.addr.st));
    addrInfo.ifIndex = 0;
    addrInfo.size = CASizeOfAddrInfo(&addrInfo);

    ca_mutex_lock(g_dtlsContextMutex);
    if (NULL == g_caDtlsContext)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Context is NULL");
        ca_mutex_unlock(g_dtlsContextMutex);
        return CA_STATUS_FAILED;
    }

    eDtlsRet_t ret = CAAdapterNetDtlsDecryptInternal(&addrInfo, data, dataLen);
    ca_mutex_unlock(g_dtlsContextMutex);

    if (DTLS_OK == ret || DTLS_HS_MSG == ret)
    {
        OIC_LOG_V(DEBUG, NET_DTLS_TAG, "Successfully Decrypted or Handshake msg recvd [%d]", ret);
        OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT");
        return CA_STATUS_OK;
    }

    OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT FAILURE");
    return CA_STATUS_FAILED;
}

