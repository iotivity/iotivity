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
#include "cawifiinterface.h"
#include "dtls.h"
#include "global.h"

/**
 * @def NET_DTLS_TAG
 * @brief Logging tag for module name
 */
#define NET_DTLS_TAG "NET_DTLS"

/**
 * @var gCaDtlsContext
 * @brief global context which holds dtls context and cache list information.
 */
static stCADtlsContext_t *gCaDtlsContext = NULL;

/**
 * @var gDtlsContextMutex
 * @brief Mutex to synchronize access to gCaDtlsContext.
 */
static u_mutex gDtlsContextMutex = NULL;

/**
 * @var gGetCredentialsCallback
 * @brief callback to get DTLS credentials
 */
static CAGetDTLSCredentialsHandler gGetCredentialsCallback = NULL;


static stCADtlsPeerInfo_t * GetPeerInfo(const char *peerAddr, const uint32_t port)
{
    uint32_t list_index = 0;
    uint32_t list_length = 0;

    if(NULL == peerAddr || 0 == port)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "CAPeerInfoListContains invalid parameters");
        return NULL;
    }

    stCADtlsPeerInfo_t *peerInfo;
    list_length = u_arraylist_length(gCaDtlsContext->peerInfoList);
    for (list_index = 0; list_index < list_length; list_index++)
    {
        peerInfo = (stCADtlsPeerInfo_t *)u_arraylist_get(gCaDtlsContext->peerInfoList, list_index);
        if (NULL == peerInfo)
        {
            continue;
        }

        if((0 == strncmp(peerAddr, peerInfo->address.IP.ipAddress, CA_IPADDR_SIZE)) &&
                (port == peerInfo->address.IP.port))
        {
            return peerInfo;
        }
    }
    return NULL;
}

static CAResult_t CAAddIdToPeerInfoList(const char *peerAddr, const uint32_t port,
       const unsigned char * id,uint16_t id_length)
{
    if(NULL == peerAddr || NULL == id || 0 == port || 0 == id_length)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "CAAddIdToPeerInfoList invalid parameters");
        return CA_STATUS_INVALID_PARAM;
    }

    if(NULL != GetPeerInfo(peerAddr, port))
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "CAAddIdToPeerInfoList peer already exist");
        return CA_STATUS_INVALID_PARAM;
    }

    stCADtlsPeerInfo_t *peerInfo = (stCADtlsPeerInfo_t *)
                                    OICMalloc(sizeof(stCADtlsPeerInfo_t));
    if (NULL == peerInfo)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "peerInfo malloc failed!");
        return CA_MEMORY_ALLOC_FAILED;
    }

    memset(peerInfo, 0x0, sizeof(stCADtlsPeerInfo_t));
    strncpy(peerInfo->address.IP.ipAddress, peerAddr, CA_IPADDR_SIZE);
    peerInfo->address.IP.port = port;
    memcpy(peerInfo->identity.id, id, id_length);
    peerInfo->identity.id_length = id_length;

    CAResult_t result = u_arraylist_add(gCaDtlsContext->peerInfoList, (void *)peerInfo);

    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "u_arraylist_add failed!");
        OICFree(peerInfo);
    }

    return result;
}


static void CAFreePeerInfoList()
{
    uint32_t list_index = 0;
    uint32_t list_length = 0;

    list_length = u_arraylist_length(gCaDtlsContext->peerInfoList);
    for (list_index = 0; list_index < list_length; list_index++)
    {
        stCADtlsPeerInfo_t * peerInfo = (stCADtlsPeerInfo_t *)u_arraylist_get(
                                gCaDtlsContext->peerInfoList, list_index);
        OICFree(peerInfo);
    }
    u_arraylist_free(&(gCaDtlsContext->peerInfoList));
    gCaDtlsContext->peerInfoList = NULL;
}

static void CARemovePeerFromPeerInfoList(char * addr, uint32_t port)
{
    if(NULL == addr || 0 >= port)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "CADTLSGetPeerPSKId invalid parameters");
        return;
    }

    uint32_t list_index = 0;
    uint32_t list_length = 0;

    list_length = u_arraylist_length(gCaDtlsContext->peerInfoList);
    for (list_index = 0; list_index < list_length; list_index++)
    {
        stCADtlsPeerInfo_t * peerInfo = (stCADtlsPeerInfo_t *)u_arraylist_get(
                                gCaDtlsContext->peerInfoList,list_index);
        if (NULL == peerInfo)
        {
            continue;
        }
        if((0 == strncmp(addr, peerInfo->address.IP.ipAddress, CA_IPADDR_SIZE)) &&
                (port == peerInfo->address.IP.port))
        {
            OICFree(u_arraylist_remove(gCaDtlsContext->peerInfoList, list_index));
            return;
        }
    }
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

    if (NULL == gCaDtlsContext)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Context is NULL");
        return DTLS_FAIL;
    }

    int32_t retLen = dtls_write(gCaDtlsContext->dtlsContext, (session_t *)dstSession, data,
                                dataLen);
    OIC_LOG_V(DEBUG, NET_DTLS_TAG, "dtls_write retun len [%d]", retLen);
    if (0 == retLen)
    {
        // A new DTLS session was initiated by tinyDTLS library and wait for callback.
        return DTLS_SESSION_INITIATED;
    }
    if (dataLen == retLen)
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

    if (dtls_handle_message(gCaDtlsContext->dtlsContext, (session_t *)srcSession, buf, bufLen) == 0)
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

    OICFree(msg->destSession);
    OICFree(msg->data);
    OICFree(msg);

    OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT");
}

static void CAClearCacheList()
{
    OIC_LOG(DEBUG, NET_DTLS_TAG, "IN");
    uint32_t list_index = 0;
    uint32_t list_length = 0;
    if (NULL == gCaDtlsContext)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Dtls Context is NULL");
        return;
    }
    list_length = u_arraylist_length(gCaDtlsContext->cacheList);
    for (list_index = 0; list_index < list_length; list_index++)
    {
        stCACacheMessage_t *msg = (stCACacheMessage_t *)u_arraylist_get(gCaDtlsContext->cacheList,
                                  list_index);
        if (msg == NULL)
        {
            continue;
        }
        CAFreeCacheMsg(msg);
    }
    u_arraylist_free(&gCaDtlsContext->cacheList);
    gCaDtlsContext->cacheList = NULL;
    OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT");
}

static CAResult_t CADtlsCacheMsg(stCACacheMessage_t *msg)
{
    OIC_LOG(DEBUG, NET_DTLS_TAG, "IN");

    if (NULL == gCaDtlsContext)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Dtls Context is NULL");
        return CA_STATUS_FAILED;
    }

    CAResult_t result = u_arraylist_add(gCaDtlsContext->cacheList, (void *)msg);
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "u_arraylist_add failed!");
    }

    OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT");
    return result;
}


static CABool_t CAIsAddressMatching(const stCADtlsAddrInfo_t *a,  const stCADtlsAddrInfo_t *b)
{
    if (a->size != b->size || a->addr.sa.sa_family != b->addr.sa.sa_family)
        return CA_FALSE;

    if ((a->addr.sin.sin_port == b->addr.sin.sin_port) &&
        (memcmp(&a->addr.sin.sin_addr, &b->addr.sin.sin_addr,
                sizeof(struct in_addr)) == 0))
    {
        return CA_TRUE;
    }
    return CA_FALSE;
}

static void CASendCachedMsg(const stCADtlsAddrInfo_t *dstSession)
{
    OIC_LOG(DEBUG, NET_DTLS_TAG, "IN");

    ///TODO: if dstSession is coming as NULL, what we will do with cached msg.
       //(will be cleared in termination)
    VERIFY_NON_NULL_VOID(dstSession, NET_DTLS_TAG, "Param dstSession is NULL");

    uint32_t list_index = 0;
    uint32_t list_length = 0;
    list_length = u_arraylist_length(gCaDtlsContext->cacheList);
    for (list_index = 0; list_index < list_length;)
    {
        stCACacheMessage_t *msg = (stCACacheMessage_t *)u_arraylist_get(gCaDtlsContext->cacheList,
                                  list_index);
        if ((NULL != msg) && (CA_TRUE == CAIsAddressMatching(msg->destSession, dstSession)))
        {
            eDtlsRet_t ret = CAAdapterNetDtlsEncryptInternal(msg->destSession,
                             msg->data, msg->dataLen);
            if (ret == DTLS_OK)
            {
                OIC_LOG(DEBUG, NET_DTLS_TAG, "CAAdapterNetDtlsEncryptInternal success");
            }
            else
            {
                OIC_LOG(ERROR, NET_DTLS_TAG, "CAAdapterNetDtlsEncryptInternal failed.");
            }

            if (u_arraylist_remove(gCaDtlsContext->cacheList, list_index))
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

    char *remoteAddress = inet_ntoa(addrInfo->addr.sin.sin_addr);
    uint32_t port = ntohs(addrInfo->addr.sin.sin_port);
    eDtlsAdapterType_t type = (eDtlsAdapterType_t)addrInfo->ifIndex;

    if (NULL == gCaDtlsContext)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Context is NULL");
        return 0;
    }

    if ((0 <= type) && (MAX_SUPPORTED_ADAPTERS > type) &&
        (NULL != gCaDtlsContext->adapterCallbacks[type].recvCallback))
    {
        // Get identity of sthe source of packet
        stCADtlsPeerInfo_t * peerInfo = GetPeerInfo(remoteAddress, port);

        gCaDtlsContext->adapterCallbacks[type].recvCallback(remoteAddress, port,
                buf,  bufLen, CA_TRUE,
                (peerInfo) ? &(peerInfo->identity) : NULL);
    }
    else
    {
        OIC_LOG_V(DEBUG, NET_DTLS_TAG, "recvCallback Callback or adapter type is wrong [%d]", type );
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

    VERIFY_NON_NULL_RET(session, NET_DTLS_TAG, "Param Session is NULL", 0);
    VERIFY_NON_NULL_RET(buf, NET_DTLS_TAG, "Param buf is NULL", 0);

    if (0 == bufLen)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Encrypted Buffer length is equal to zero");
        return 0;
    }

    stCADtlsAddrInfo_t *addrInfo = (stCADtlsAddrInfo_t *)session;

    char *remoteAddress = inet_ntoa(addrInfo->addr.sin.sin_addr);
    uint32_t port = ntohs(addrInfo->addr.sin.sin_port);
    eDtlsAdapterType_t type = (eDtlsAdapterType_t)addrInfo->ifIndex;

    //Mutex is not required for gCaDtlsContext. It will be called in same thread.
    int32_t sentLen = 0;
    if ((0 <= type) && (MAX_SUPPORTED_ADAPTERS > type) &&
        (NULL != gCaDtlsContext->adapterCallbacks[type].sendCallback))
    {
        sentLen = gCaDtlsContext->adapterCallbacks[type].sendCallback(remoteAddress, port,
                  buf,  bufLen);
    }
    else
    {
        OIC_LOG_V(DEBUG, NET_DTLS_TAG, "send Callback or adapter type is wrong [%d]", type );
    }

    OIC_LOG_V(DEBUG, NET_DTLS_TAG, "sent buffer length [%d]", sentLen);

    OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT");
    return sentLen;
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

    VERIFY_NON_NULL_RET(gGetCredentialsCallback, NET_DTLS_TAG, "GetCredential callback", 0);

    OCDtlsPskCredsBlob *credInfo = NULL;

    //Retrieve the credentials blob from security module
     gGetCredentialsCallback(&credInfo);

    VERIFY_NON_NULL_RET(credInfo, NET_DTLS_TAG, "CAGetDtlsPskCredentials credInfo is NULL", 0);

    if ((type == DTLS_PSK_HINT) || (type == DTLS_PSK_IDENTITY))
    {
        if (DTLS_PSK_ID_LEN <= resultLen)
        {
            memcpy(result, credInfo->identity, DTLS_PSK_ID_LEN);
            ret = DTLS_PSK_ID_LEN;
        }
    }

    if ((DTLS_PSK_KEY == type) && (desc) && (DTLS_PSK_PSK_LEN == descLen))
    {
        //Check if we have the credentials for the device with which we
        //are trying to perform a handshake
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

    return ret;
}

void CADTLSSetAdapterCallbacks(CAPacketReceivedCallback recvCallback,
                               CAPacketSendCallback sendCallback, eDtlsAdapterType_t type)
{
    OIC_LOG(DEBUG, NET_DTLS_TAG, "IN");
    u_mutex_lock(gDtlsContextMutex);
    if (NULL == gCaDtlsContext)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Context is NULL");
        u_mutex_unlock(gDtlsContextMutex);
        return;
    }

    if ((0 <= type) && (MAX_SUPPORTED_ADAPTERS > type))
    {
        gCaDtlsContext->adapterCallbacks[type].recvCallback = recvCallback;
        gCaDtlsContext->adapterCallbacks[type].sendCallback = sendCallback;
    }

    u_mutex_unlock(gDtlsContextMutex);

    OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT");
}

void CADTLSSetCredentialsCallback(CAGetDTLSCredentialsHandler credCallback)
{
    // TODO Does this method needs protection of DtlsContextMutex ?
    OIC_LOG(DEBUG, NET_DTLS_TAG, "IN");
    gGetCredentialsCallback = credCallback;
    OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT");
}

CAResult_t CADtlsSelectCipherSuite(const dtls_cipher_t cipher)
{
    OIC_LOG(DEBUG, NET_DTLS_TAG, "IN CADtlsSelectCipherSuite");

    u_mutex_lock(gDtlsContextMutex);
    if (NULL == gCaDtlsContext)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Context is NULL");
        u_mutex_unlock(gDtlsContextMutex);
        return CA_STATUS_FAILED;
    }
    dtls_select_cipher(gCaDtlsContext->dtlsContext, cipher);
    u_mutex_unlock(gDtlsContextMutex);

    OIC_LOG_V(DEBUG, NET_DTLS_TAG, "Selected cipher suite is 0x%02X%02X\n",
        ((uint8_t*)(&cipher))[1], ((uint8_t*)(&cipher))[0]);
    OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT CADtlsSelectCipherSuite");

    return CA_STATUS_OK ;
}

CAResult_t CADtlsEnablesAnonEcdh(const uint8_t enable)
{
    OIC_LOG(DEBUG, NET_DTLS_TAG, "IN CADtlsEnablesAnonEcdh");

    u_mutex_lock(gDtlsContextMutex);
    if (NULL == gCaDtlsContext)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Context is NULL");
        u_mutex_unlock(gDtlsContextMutex);
        return CA_STATUS_FAILED;
    }
    dtls_enables_anon_ecdh(gCaDtlsContext->dtlsContext, enable);
    u_mutex_unlock(gDtlsContextMutex);
    OIC_LOG_V(DEBUG, NET_DTLS_TAG, "TLS_ECDH_anon_WITH_AES_128_CBC_SHA  is %s",
        enable ? "enabled" : "disabled");

    OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT CADtlsEnablesAnonEcdh");

    return CA_STATUS_OK ;
}

CAResult_t CADtlsInitiateHandshake(const CAAddress_t* addrInfo,
                                 const CAConnectivityType_t connType)
{
    int res;
    stCADtlsAddrInfo_t dst;

    OIC_LOG(DEBUG, NET_DTLS_TAG, "IN CADtlsInitiateHandshake");

    if(!addrInfo)
    {
        return CA_STATUS_INVALID_PARAM;
    }

    memset(&dst, 0, sizeof(stCADtlsAddrInfo_t));

    if(inet_aton(addrInfo->IP.ipAddress, &dst.addr.sin.sin_addr) == 0)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Failed to convert from ASCII to Network Address");
        return CA_STATUS_FAILED;
    }
    dst.addr.sin.sin_family = AF_INET;
    dst.addr.sin.sin_port = htons(addrInfo->IP.port);
    dst.size = sizeof(dst.addr);

    u_mutex_lock(gDtlsContextMutex);
    if(NULL == gDtlsContextMutex)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Context is NULL");
        u_mutex_unlock(gDtlsContextMutex);
        return CA_STATUS_FAILED;
    }

    if(0 > dtls_connect(gCaDtlsContext->dtlsContext, &dst))
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Failed to connect");
        u_mutex_unlock(gDtlsContextMutex);
        return CA_STATUS_FAILED;
    }

    u_mutex_unlock(gDtlsContextMutex);

    OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT CADtlsInitiateHandshake");

    return CA_STATUS_OK;
}

CAResult_t CADtlsGenerateOwnerPSK(const CAAddress_t* addrInfo,
                    const CAConnectivityType_t connType,
                    const uint8_t* label, const size_t labelLen,
                    const uint8_t* rsrcServerDeviceID, const size_t rsrcServerDeviceIDLen,
                    const uint8_t* provServerDeviceID, const size_t provServerDeviceIDLen,
                    uint8_t* ownerPSK, const size_t ownerPSKSize)
{
    OIC_LOG(DEBUG, NET_DTLS_TAG, "IN CADtlsGenerateOwnerPSK");

    if(!addrInfo || !label || 0 == labelLen || !ownerPSK || 0 == ownerPSKSize)
    {
        return CA_STATUS_INVALID_PARAM;
    }

    stCADtlsAddrInfo_t dst;
    memset(&dst, 0, sizeof(stCADtlsAddrInfo_t));

    if(inet_aton(addrInfo->IP.ipAddress, &dst.addr.sin.sin_addr) == 0)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Failed to convert from ASCII to Network Address");
        return CA_STATUS_FAILED;
    }
    dst.addr.sin.sin_family = AF_INET;
    dst.addr.sin.sin_port = htons(addrInfo->IP.port);
    dst.size = sizeof(dst.addr);

    u_mutex_lock(gDtlsContextMutex);
    if (NULL == gCaDtlsContext)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Context is NULL");
        u_mutex_unlock(gDtlsContextMutex);
        return CA_STATUS_FAILED;
    }

    if( 0 == dtls_prf_with_current_keyblock(gCaDtlsContext->dtlsContext, &dst, label, labelLen,
                                  rsrcServerDeviceID, rsrcServerDeviceIDLen,
                                  provServerDeviceID, provServerDeviceIDLen,
                                  ownerPSK, ownerPSKSize))
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Failed to DTLS PRF");
        u_mutex_unlock(gDtlsContextMutex);
        return CA_STATUS_FAILED;
    }
    u_mutex_unlock(gDtlsContextMutex);
    OIC_LOG(DEBUG, NET_DTLS_TAG, "[OwnerPSK]");
    OIC_LOG_BUFFER(DEBUG, NET_DTLS_TAG, ownerPSK, ownerPSKSize);

    OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT CADtlsGenerateOwnerPSK");

    return CA_STATUS_OK;
}

CAResult_t CAAdapterNetDtlsInit()
{
    OIC_LOG(DEBUG, NET_DTLS_TAG, "IN");

    // Initialize mutex for DtlsContext
    if (NULL == gDtlsContextMutex)
    {
        u_mutex_init();
        gDtlsContextMutex = u_mutex_new();
        VERIFY_NON_NULL_RET(gDtlsContextMutex, NET_DTLS_TAG, "malloc failed",
                CA_MEMORY_ALLOC_FAILED);
    }
    else
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "CAAdapterNetDtlsInit done already!");
        return CA_STATUS_OK;
    }

    // Lock DtlsContext mutex and create DtlsContext
    u_mutex_lock(gDtlsContextMutex);
    gCaDtlsContext = (stCADtlsContext_t *)OICMalloc(sizeof(stCADtlsContext_t));

    if (NULL == gCaDtlsContext)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Context malloc failed");
        u_mutex_unlock(gDtlsContextMutex);
        u_mutex_free(gDtlsContextMutex);
        return CA_MEMORY_ALLOC_FAILED;
    }

    memset(gCaDtlsContext, 0x0, sizeof(stCADtlsContext_t));

    // Create PeerInfoList and CacheList
    gCaDtlsContext->peerInfoList = u_arraylist_create();
    gCaDtlsContext->cacheList = u_arraylist_create();

    if( (NULL == gCaDtlsContext->peerInfoList) ||
        (NULL == gCaDtlsContext->peerInfoList))
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "peerInfoList or cacheList initialization failed!");
        CAClearCacheList();
        CAFreePeerInfoList();
        OICFree(gCaDtlsContext);
        gCaDtlsContext = NULL;
        u_mutex_unlock(gDtlsContextMutex);
        u_mutex_free(gDtlsContextMutex);
        return CA_STATUS_FAILED;
    }

    // Initialize clock, crypto and other global vars in tinyDTLS library
    dtls_init();

    // Create tinydtls Context
    gCaDtlsContext->dtlsContext = dtls_new_context(gCaDtlsContext);

    if (NULL ==  gCaDtlsContext->dtlsContext)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "dtls_new_context failed");
        u_mutex_unlock(gDtlsContextMutex);
        CAAdapterNetDtlsDeInit();
        return CA_STATUS_FAILED;
    }

    gCaDtlsContext->callbacks.write = CASendSecureData;
    gCaDtlsContext->callbacks.read  = CAReadDecryptedPayload;
    gCaDtlsContext->callbacks.event = CAHandleSecureEvent;
    gCaDtlsContext->callbacks.get_psk_info = CAGetPskCredentials;

    dtls_set_handler(gCaDtlsContext->dtlsContext, &(gCaDtlsContext->callbacks));
    u_mutex_unlock(gDtlsContextMutex);
    OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT");
    return CA_STATUS_OK;
}

void CAAdapterNetDtlsDeInit()
{
    OIC_LOG(DEBUG, NET_DTLS_TAG, "IN");

    VERIFY_NON_NULL_VOID(gCaDtlsContext, NET_DTLS_TAG, "context is NULL");
    VERIFY_NON_NULL_VOID(gDtlsContextMutex, NET_DTLS_TAG, "context mutex is NULL");

    //Lock DtlsContext mutex
    u_mutex_lock(gDtlsContextMutex);

    // Clear all lists
    CAFreePeerInfoList();
    CAClearCacheList();

    // De-initialize tinydtls context
    dtls_free_context(gCaDtlsContext->dtlsContext);
    gCaDtlsContext->dtlsContext = NULL;

    // De-initialize DtlsContext
    OICFree(gCaDtlsContext);
    gCaDtlsContext = NULL;

    // Unlock DtlsContext mutex and de-initialize it
    u_mutex_unlock(gDtlsContextMutex);
    u_mutex_free(gDtlsContextMutex);
    gDtlsContextMutex = NULL;

    OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT");
}

CAResult_t CAAdapterNetDtlsEncrypt(const char *remoteAddress,
                                   const uint32_t port,
                                   const void *data,
                                   uint32_t dataLen,
                                   uint8_t *cacheFlag,
                                   eDtlsAdapterType_t adapterType)
{

    OIC_LOG(DEBUG, NET_DTLS_TAG, "IN");

    VERIFY_NON_NULL_RET(remoteAddress, NET_DTLS_TAG,"Param remoteAddress is NULL",CA_STATUS_FAILED);

    VERIFY_NON_NULL_RET(data, NET_DTLS_TAG, "Param data is NULL" , CA_STATUS_FAILED);

    if (0 == dataLen)
    {
        OIC_LOG_V(ERROR, NET_DTLS_TAG, "dataLen is less than or equal zero [%d]", dataLen);
        return CA_STATUS_FAILED;
    }

    OIC_LOG_V(DEBUG, NET_DTLS_TAG, " Data to be encrypted dataLen [%d]", dataLen);

    stCADtlsAddrInfo_t *addrInfo = (stCADtlsAddrInfo_t *)OICMalloc(sizeof(stCADtlsAddrInfo_t));

    VERIFY_NON_NULL_RET(addrInfo, NET_DTLS_TAG, "malloc failed" , CA_MEMORY_ALLOC_FAILED);

    memset(addrInfo, 0x0, sizeof(stCADtlsAddrInfo_t));

    addrInfo->addr.sin.sin_family = AF_INET;
    addrInfo->addr.sin.sin_port = htons(port);
    // Conversion from ASCII format to Network format
    if (inet_aton(remoteAddress, &addrInfo->addr.sin.sin_addr) == 0)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Failed to convert from ASCII to Network Address");
        OICFree(addrInfo);
        return CA_STATUS_FAILED;
    }
    addrInfo->size = sizeof(addrInfo->addr);
    addrInfo->ifIndex = adapterType;

    ///TODO: we need to check this memory allocation is needed or not
    uint8_t *buf = (uint8_t *)OICMalloc(sizeof(uint8_t) * (dataLen + 1));
    if (NULL == buf)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "malloc failed!");
        OICFree(addrInfo);
        return CA_MEMORY_ALLOC_FAILED;
    }
    memset(buf, 0x0, sizeof(uint8_t) * (dataLen + 1));
    memcpy(buf, data, dataLen);

    // TODO: Error handling in this method is really a landmine of bugs.
    // This needs to be re-factored when merge to master happens.
    u_mutex_lock(gDtlsContextMutex);
    eDtlsRet_t ret = CAAdapterNetDtlsEncryptInternal(addrInfo, buf, dataLen);
    if (ret == DTLS_SESSION_INITIATED)
    {
        stCACacheMessage_t *message = (stCACacheMessage_t *)OICMalloc(sizeof(stCACacheMessage_t));
        if (NULL == message)
        {
            OIC_LOG(ERROR, NET_DTLS_TAG, "malloc failed!");
            OICFree(addrInfo);
            OICFree(buf);
            u_mutex_unlock(gDtlsContextMutex);
            return CA_MEMORY_ALLOC_FAILED;
        }
        memset(message, 0x0, sizeof(stCACacheMessage_t));

        message->data = buf;
        message->dataLen = dataLen;
        message->destSession = addrInfo;

        CAResult_t result = CADtlsCacheMsg(message);
        if (CA_STATUS_OK == result)
        {
            if (cacheFlag)
            {
                *cacheFlag = 1;
            }
        }
        else
        {
            OIC_LOG(DEBUG, NET_DTLS_TAG, "CADtlsCacheMsg failed!");
            CAFreeCacheMsg(message);
        }
        OIC_LOG_V(DEBUG, NET_DTLS_TAG, "OUT Initiating Dtls session [%d]", result);
        u_mutex_unlock(gDtlsContextMutex);
        return result;
    }

    u_mutex_unlock(gDtlsContextMutex);

    if (ret == DTLS_OK)
    {
        OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT");
        OICFree(addrInfo);
        OICFree(buf);
        return CA_STATUS_OK;
    }
    OICFree(addrInfo);
    OICFree(buf);
    OIC_LOG(ERROR, NET_DTLS_TAG, "OUT FAILURE");
    return CA_STATUS_FAILED;
}




CAResult_t CAAdapterNetDtlsDecrypt(const char *remoteAddress,
                                   const uint32_t port,
                                   uint8_t *data,
                                   uint32_t dataLen,
                                   eDtlsAdapterType_t adapterType)
{
    OIC_LOG(DEBUG, NET_DTLS_TAG, "IN");

    stCADtlsAddrInfo_t *addrInfo = (stCADtlsAddrInfo_t *)OICMalloc(sizeof(stCADtlsAddrInfo_t));

    VERIFY_NON_NULL_RET(addrInfo, NET_DTLS_TAG, "malloc failed" , CA_MEMORY_ALLOC_FAILED);

    memset(addrInfo, 0x0, sizeof(stCADtlsAddrInfo_t));

    addrInfo->addr.sin.sin_family = AF_INET;
    addrInfo->addr.sin.sin_port = htons(port);
    // Conversion from ASCII format to Network format
    if (inet_aton(remoteAddress, &addrInfo->addr.sin.sin_addr) == 0)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Failed to convert from ASCII to Network Address");
        OICFree(addrInfo);
        return CA_STATUS_FAILED;
    }
    addrInfo->size = sizeof(addrInfo->addr);
    addrInfo->ifIndex = adapterType;

    uint8_t *buf = (uint8_t *)OICMalloc(sizeof(uint8_t) * (dataLen + 1));
    if (NULL == buf)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "malloc failed!");
        OICFree(addrInfo);
        return CA_MEMORY_ALLOC_FAILED;
    }
    memset(buf, 0x0, sizeof(uint8_t) * (dataLen + 1));
    memcpy(buf, data, dataLen);

    u_mutex_lock(gDtlsContextMutex);
    eDtlsRet_t ret = CAAdapterNetDtlsDecryptInternal(addrInfo, buf, dataLen);
    u_mutex_unlock(gDtlsContextMutex);

    if (DTLS_OK == ret || DTLS_HS_MSG == ret)
    {
        OIC_LOG_V(DEBUG, NET_DTLS_TAG, "Successfully Decrypted or Handshake msg recvd [%d]", ret);
        OICFree(buf);
        OICFree(addrInfo);
        OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT");
        return CA_STATUS_OK;
    }

    OICFree(buf);
    OICFree(addrInfo);
    OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT FAILURE");
    return CA_STATUS_FAILED;
}

