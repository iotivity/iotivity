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
 * @var g_dtlsListMutex
 * @brief Mutex to synchronize access to DTLS Cache.
 */
static ca_mutex g_dtlsListMutex = NULL;

/**
 * @var g_getCredentialsCallback
 * @brief callback to get DTLS credentials
 */
static CAGetDTLSCredentialsHandler g_getCredentialsCallback = NULL;

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

    ca_mutex_lock(g_dtlsContextMutex);
    if (NULL == g_caDtlsContext)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Context is NULL");
        ca_mutex_unlock(g_dtlsContextMutex);
        return DTLS_FAIL;
    }

    int retLen = dtls_write(g_caDtlsContext->dtlsContext, (session_t *)dstSession, data,
                                dataLen);
    OIC_LOG_V(DEBUG, NET_DTLS_TAG, "dtls_write retun len [%d]", retLen);
    ca_mutex_unlock(g_dtlsContextMutex);

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

    ///  TODO: how to protect g_caDtlsContext as dtls_handle_message is blocking call
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
    ca_mutex_lock(g_dtlsListMutex);
    if (NULL == g_caDtlsContext)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Dtls Context is NULL");
        ca_mutex_unlock(g_dtlsListMutex);
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
    ca_mutex_unlock(g_dtlsListMutex);
    OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT");
}

static CAResult_t CADtlsCacheMsg(stCACacheMessage_t *msg)
{
    OIC_LOG(DEBUG, NET_DTLS_TAG, "IN");

    ca_mutex_lock(g_dtlsListMutex);
    if (NULL == g_caDtlsContext)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Dtls Context is NULL");
        ca_mutex_unlock(g_dtlsListMutex);
        return CA_STATUS_FAILED;
    }

    CAResult_t result = u_arraylist_add(g_caDtlsContext->cacheList, (void *)msg);
    if (CA_STATUS_OK != result)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "u_arraylist_add failed!");
    }
    ca_mutex_unlock(g_dtlsListMutex);

    OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT");
    return result;
}


static bool CAIsAddressMatching(const stCADtlsAddrInfo_t *a,  const stCADtlsAddrInfo_t *b)
{
    return (a->size == b->size) &&
           (a->addr.sa.sa_family == b->addr.sa.sa_family) &&
           (a->addr.sin.sin_port == b->addr.sin.sin_port) &&
           memcmp(&a->addr.sin.sin_addr, &b->addr.sin.sin_addr, sizeof(struct in_addr)) == 0;
}

static void CASendCachedMsg(const stCADtlsAddrInfo_t *dstSession)
{
    OIC_LOG(DEBUG, NET_DTLS_TAG, "IN");
    VERIFY_NON_NULL_VOID(dstSession, NET_DTLS_TAG, "Param dstSession is NULL");

    uint32_t list_index = 0;
    uint32_t list_length = 0;
    ca_mutex_lock(g_dtlsListMutex);
    list_length = u_arraylist_length(g_caDtlsContext->cacheList);
    for (list_index = 0; list_index < list_length;)
    {
        stCACacheMessage_t *msg = (stCACacheMessage_t *)u_arraylist_get(g_caDtlsContext->cacheList,
                                  list_index);
        if ((NULL != msg) && (true == CAIsAddressMatching(msg->destSession, dstSession)))
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
    ca_mutex_unlock(g_dtlsListMutex);

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

    ca_mutex_lock(g_dtlsContextMutex);
    if (NULL == g_caDtlsContext)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Context is NULL");
        ca_mutex_unlock(g_dtlsContextMutex);
        return 0;
    }

    if ((0 <= type) && (MAX_SUPPORTED_ADAPTERS > type) &&
        (NULL != g_caDtlsContext->adapterCallbacks[type].recvCallback))
    {
        g_caDtlsContext->adapterCallbacks[type].recvCallback(remoteAddress, port,
                buf,  bufLen, true);
    }
    else
    {
        OIC_LOG_V(DEBUG, NET_DTLS_TAG, "recvCallback Callback or adapter type is wrong [%d]", type);
    }
    ca_mutex_unlock(g_dtlsContextMutex);

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

    char *remoteAddress = inet_ntoa(addrInfo->addr.sin.sin_addr);
    uint16_t port = ntohs(addrInfo->addr.sin.sin_port);
    eDtlsAdapterType_t type = (eDtlsAdapterType_t)addrInfo->ifIndex;

    //Mutex is not required for g_caDtlsContext. It will be called in same thread.
    int32_t sentLen = 0;
    if ((0 <= type) && (MAX_SUPPORTED_ADAPTERS > type) &&
        (NULL != g_caDtlsContext->adapterCallbacks[type].sendCallback))
    {
        sentLen = g_caDtlsContext->adapterCallbacks[type].sendCallback(remoteAddress, port,
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
                               CAPacketSendCallback sendCallback, eDtlsAdapterType_t type)
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
        g_caDtlsContext->adapterCallbacks[type].recvCallback = recvCallback;
        g_caDtlsContext->adapterCallbacks[type].sendCallback = sendCallback;
    }

    ca_mutex_unlock(g_dtlsContextMutex);

    OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT");
}

void CADTLSSetCredentialsCallback(CAGetDTLSCredentialsHandler credCallback)
{
    OIC_LOG(DEBUG, NET_DTLS_TAG, "IN");
    g_getCredentialsCallback = credCallback;
    OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT");
}

CAResult_t CAAdapterNetDtlsInit()
{
    OIC_LOG(DEBUG, NET_DTLS_TAG, "IN");

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

    if (NULL == g_dtlsListMutex)
    {
        g_dtlsListMutex = ca_mutex_new();
        if (NULL == g_dtlsListMutex)
        {
            OIC_LOG(ERROR, NET_DTLS_TAG, "g_dtlsListMutex malloc failed");
            ca_mutex_free(g_dtlsContextMutex);
            return CA_MEMORY_ALLOC_FAILED;
        }
    }

    ca_mutex_lock(g_dtlsContextMutex);
    g_caDtlsContext = (stCADtlsContext_t *)OICCalloc(1, sizeof(stCADtlsContext_t));

    if (NULL == g_caDtlsContext)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "Context malloc failed");
        ca_mutex_free(g_dtlsListMutex);
        ca_mutex_unlock(g_dtlsContextMutex);
        ca_mutex_free(g_dtlsContextMutex);
        return CA_MEMORY_ALLOC_FAILED;
    }

    ca_mutex_lock(g_dtlsListMutex);
    g_caDtlsContext->cacheList = u_arraylist_create();

    if (NULL == g_caDtlsContext->cacheList)
    {
        OIC_LOG(ERROR, NET_DTLS_TAG, "cacheList initialization failed!");
        ca_mutex_unlock(g_dtlsListMutex);
        ca_mutex_free(g_dtlsListMutex);
        ca_mutex_unlock(g_dtlsContextMutex);
        ca_mutex_free(g_dtlsContextMutex);
        OICFree(g_caDtlsContext);
        g_caDtlsContext = NULL;
        return CA_STATUS_FAILED;
    }
    ca_mutex_unlock(g_dtlsListMutex);
    // Initialize clock, crypto and other global vars in tinyDTLS library
    dtls_init();

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

    ca_mutex_lock(g_dtlsContextMutex);
    CAClearCacheList();
    dtls_free_context(g_caDtlsContext->dtlsContext);
    g_caDtlsContext->dtlsContext = NULL;
    OICFree(g_caDtlsContext);
    g_caDtlsContext = NULL;
    ca_mutex_unlock(g_dtlsContextMutex);

    ca_mutex_free(g_dtlsContextMutex);
    g_dtlsContextMutex = NULL;
    ca_mutex_free(g_dtlsListMutex);
    g_dtlsListMutex = NULL;
    OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT");
}

CAResult_t CAAdapterNetDtlsEncrypt(const char *remoteAddress,
                                   const uint16_t port,
                                   void *data,
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

    OIC_LOG_V(DEBUG, NET_DTLS_TAG, "Data to be encrypted dataLen [%d]", dataLen);

    stCADtlsAddrInfo_t *addrInfo = (stCADtlsAddrInfo_t *)OICCalloc(1, sizeof(stCADtlsAddrInfo_t));

    VERIFY_NON_NULL_RET(addrInfo, NET_DTLS_TAG, "malloc failed" , CA_MEMORY_ALLOC_FAILED);

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

    eDtlsRet_t ret = CAAdapterNetDtlsEncryptInternal(addrInfo, data, dataLen);
    if (ret == DTLS_SESSION_INITIATED)
    {
        stCACacheMessage_t *message = (stCACacheMessage_t *)OICCalloc(1, sizeof(stCACacheMessage_t));
        if (NULL == message)
        {
            OIC_LOG(ERROR, NET_DTLS_TAG, "calloc failed!");
            OICFree(addrInfo);
            return CA_MEMORY_ALLOC_FAILED;
        }

        message->data = (uint8_t *)OICCalloc(dataLen + 1, sizeof(uint8_t));
        if (NULL == message->data)
        {
            OIC_LOG(ERROR, NET_DTLS_TAG, "calloc failed!");
            OICFree(addrInfo);
            OICFree(message);
            return CA_MEMORY_ALLOC_FAILED;
        }
        memcpy(message->data, data, dataLen);
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

    OICFree(addrInfo);

    if (ret == DTLS_OK)
    {
        OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT");
        return CA_STATUS_OK;
    }

    OIC_LOG(ERROR, NET_DTLS_TAG, "OUT FAILURE");
    return CA_STATUS_FAILED;
}


CAResult_t CAAdapterNetDtlsDecrypt(const char *remoteAddress,
                                   const uint16_t port,
                                   uint8_t *data,
                                   uint32_t dataLen,
                                   eDtlsAdapterType_t adapterType)
{
    OIC_LOG(DEBUG, NET_DTLS_TAG, "IN");

    stCADtlsAddrInfo_t *addrInfo = (stCADtlsAddrInfo_t *)OICCalloc(1, sizeof(stCADtlsAddrInfo_t));

    VERIFY_NON_NULL_RET(addrInfo, NET_DTLS_TAG, "calloc failed" , CA_MEMORY_ALLOC_FAILED);

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

    eDtlsRet_t ret = CAAdapterNetDtlsDecryptInternal(addrInfo, data, dataLen);

    OICFree(addrInfo);
    if (DTLS_OK == ret || DTLS_HS_MSG == ret)
    {
        OIC_LOG_V(DEBUG, NET_DTLS_TAG, "Successfully Decrypted or Handshake msg recvd [%d]", ret);
        OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT");
        return CA_STATUS_OK;
    }

    OIC_LOG(DEBUG, NET_DTLS_TAG, "OUT FAILURE");
    return CA_STATUS_FAILED;
}


