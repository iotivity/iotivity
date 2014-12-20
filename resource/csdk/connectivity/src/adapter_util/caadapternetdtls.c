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
 * @var gDtlsListMutex
 * @brief Mutex to synchronize access to DTLS Cache.
 */
static u_mutex gDtlsListMutex = NULL;

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

    u_mutex_lock(gDtlsContextMutex);
    if (NULL == gCaDtlsContext)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Context is NULL");
        u_mutex_unlock(gDtlsContextMutex);
        return DTLS_FAIL;
    }

    int32_t retLen = dtls_write(gCaDtlsContext->dtlsContext, (session_t *)dstSession, data, 
                                dataLen);
    OIC_LOG_V(DEBUG, NET_DTLS_TAG, "dtls_write retun len [%d]", retLen);
    if (0 == retLen)
    {
        // A new DTLS session was initiated by tinyDTLS library and wait for callback.
        u_mutex_unlock(gDtlsContextMutex);
        return DTLS_SESSION_INITIATED;
    }
    u_mutex_unlock(gDtlsContextMutex);
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

    ///  TODO: how to protect gCaDtlsContext as dtls_handle_message is blocking call
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
    u_mutex_lock(gDtlsListMutex);
    if (NULL == gCaDtlsContext)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Dtls Context is NULL");
        u_mutex_unlock(gDtlsListMutex);
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
    u_arraylist_free(gCaDtlsContext->cacheList);
    gCaDtlsContext->cacheList = NULL;
    u_mutex_unlock(gDtlsListMutex);
    OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT");
}

static CAResult_t CADtlsCacheMsg(stCACacheMessage_t *msg)
{
    OIC_LOG(DEBUG, NET_DTLS_TAG, "IN");

    u_mutex_lock(gDtlsListMutex);
    if (NULL == gCaDtlsContext)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Dtls Context is NULL");
        u_mutex_unlock(gDtlsListMutex);
        return CA_STATUS_FAILED;
    }

    CAResult_t result = u_arraylist_add(gCaDtlsContext->cacheList, (void *)msg);
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "u_arraylist_add failed!");
    }
    u_mutex_unlock(gDtlsListMutex);

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
    u_mutex_lock(gDtlsListMutex);
    list_length = u_arraylist_length(gCaDtlsContext->cacheList);
    for (list_index = 0; list_index < list_length; list_index++)
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
            u_arraylist_remove(gCaDtlsContext->cacheList, list_index);
            CAFreeCacheMsg(msg);
            break;
        }

    }
    u_mutex_unlock(gDtlsListMutex);

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

    u_mutex_lock(gDtlsContextMutex);
    if (NULL == gCaDtlsContext)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Context is NULL");
        u_mutex_unlock(gDtlsContextMutex);
        return 0;
    }

    if ((0 <= type) && (MAX_SUPPORTED_ADAPTERS > type) &&
        (NULL != gCaDtlsContext->adapterCallbacks[type].recvCallback))
    {
        gCaDtlsContext->adapterCallbacks[type].recvCallback(remoteAddress, port,
                buf,  bufLen, CA_TRUE);
    }
    else
    {
        OIC_LOG_V(DEBUG, NET_DTLS_TAG, "recvCallback Callback or adapter type is wrong [%d]", type );
    }
    u_mutex_unlock(gDtlsContextMutex);

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
    OCDtlsPskCredsBlob *credInfo = NULL;

    //Retrieve the credentials blob from security module
    OCGetDtlsPskCredentials(&credInfo);

    VERIFY_NON_NULL_RET(credInfo, NET_DTLS_TAG, "OCGetDtlsPskCredentials credInfo is NULL", ret);

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
        //Check if we have the credentials for the device with which we
        //are trying to perform a handshake
        int index = 0;
        for (; index < credInfo->num; index++)
        {
            if (memcmp(desc, credInfo->creds[index].id, DTLS_PSK_ID_LEN) == 0)
            {
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

CAResult_t CAAdapterNetDtlsInit()
{
    OIC_LOG(DEBUG, NET_DTLS_TAG, "IN");

    if (NULL == gDtlsContextMutex)
    {
        u_mutex_init();
        gDtlsContextMutex = u_mutex_new();
        VERIFY_NON_NULL_RET(gDtlsContextMutex, NET_DTLS_TAG, "malloc failed", CA_MEMORY_ALLOC_FAILED);
    }
    else
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "CAAdapterNetDtlsInit done already!");
        return CA_STATUS_OK;
    }

    if (NULL == gDtlsListMutex)
    {
        gDtlsListMutex = u_mutex_new();
        if (NULL == gDtlsListMutex)
        {
            OIC_LOG(ERROR, NET_DTLS_TAG, "gDtlsListMutex malloc failed");
            u_mutex_free(gDtlsContextMutex);
            return CA_MEMORY_ALLOC_FAILED;
        }
    }

    u_mutex_lock(gDtlsContextMutex);
    gCaDtlsContext = (stCADtlsContext_t *)OICMalloc(sizeof(stCADtlsContext_t));

    if (NULL == gCaDtlsContext)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Context malloc failed");
        u_mutex_unlock(gDtlsContextMutex);
        u_mutex_free(gDtlsContextMutex);
        u_mutex_free(gDtlsListMutex);
        return CA_MEMORY_ALLOC_FAILED;
    }

    memset(gCaDtlsContext, 0x0, sizeof(stCADtlsContext_t));

    u_mutex_lock(gDtlsListMutex);
    gCaDtlsContext->cacheList = u_arraylist_create();

    if (NULL == gCaDtlsContext->cacheList)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "cacheList initialization failed!");
        u_mutex_unlock(gDtlsListMutex);
        u_mutex_unlock(gDtlsContextMutex);
        u_mutex_free(gDtlsContextMutex);
        u_mutex_free(gDtlsListMutex);
        OICFree(gCaDtlsContext);
        gCaDtlsContext = NULL;
        return CA_STATUS_FAILED;
    }
    u_mutex_unlock(gDtlsListMutex);
    // Initialize clock, crypto and other global vars in tinyDTLS library
    dtls_init();

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

    u_mutex_lock(gDtlsContextMutex);
    CAClearCacheList();
    dtls_free_context(gCaDtlsContext->dtlsContext);
    gCaDtlsContext->dtlsContext = NULL;
    OICFree(gCaDtlsContext);
    gCaDtlsContext = NULL;
    u_mutex_unlock(gDtlsContextMutex);

    u_mutex_free(gDtlsContextMutex);
    gDtlsContextMutex = NULL;
    u_mutex_free(gDtlsListMutex);
    gDtlsListMutex = NULL;
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

    eDtlsRet_t ret = CAAdapterNetDtlsEncryptInternal(addrInfo, buf, dataLen);
    if (ret == DTLS_SESSION_INITIATED)
    {
        stCACacheMessage_t *message = (stCACacheMessage_t *)OICMalloc(sizeof(stCACacheMessage_t));
        if (NULL == message)
        {
            OIC_LOG(ERROR, NET_DTLS_TAG, "malloc failed!");
            OICFree(addrInfo);
            OICFree(buf);
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
        return result;
    }

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

    eDtlsRet_t ret = CAAdapterNetDtlsDecryptInternal(addrInfo, buf, dataLen);
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

