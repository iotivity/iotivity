/******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
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

#define _GNU_SOURCE

#include <stddef.h>
#include <stdbool.h>
#include "ca_adapter_net_tls.h"
#include "cacommon.h"
#include "caipinterface.h"
#include "oic_malloc.h"
#include "pkix/byte_array.h"
#include "octhread.h"

// headers required for mbed TLS
#include "mbedtls/platform.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/pkcs12.h"
#include "mbedtls/ssl_internal.h"

#ifndef NDEBUG
#include "mbedtls/debug.h"
#include "mbedtls/version.h"
#endif

#ifdef __unix__
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#include "pkix/byte_array.h"

/**
 * @def MBED_TLS_VERSION_LEN
 * @brief mbedTLS version string length
 */
#define MBED_TLS_VERSION_LEN (16)
/**
 * @def SEED
 * @brief Seed for initialization RNG
 */
#define SEED "IOTIVITY_RND"
/**
 * @def UUID_PREFIX
 * @brief uuid prefix in certificate subject field
 */
#define UUID_PREFIX "uuid:"
/**
 * @def USERID_PREFIX
 * @brief userid prefix in certificate alternative subject name field
 */
#define USERID_PREFIX "userid:"

/**
 * @def NET_TLS_TAG
 * @brief Logging tag for module name
 */
#define NET_TLS_TAG "OIC_CA_NET_TLS"
/**
 * @def MBED_TLS_TAG
 * @brief Logging tag for mbedTLS library
 */
#define MBED_TLS_TAG "OIC_MBED_TLS"
/**
 * @def MMBED_TLS_DEBUG_LEVEL
 * @brief Logging level for mbedTLS library
 */
#define MBED_TLS_DEBUG_LEVEL (4)

/**
 * @def TLS_MSG_BUF_LEN
 * @brief Buffer size for TLS record. A single TLS record may be up to 16384 octets in length
 */

#define TLS_MSG_BUF_LEN (16384)
/**
 * @def PSK_LENGTH
 * @brief PSK keys max length
 */
#define PSK_LENGTH (256/8)
/**
 * @def UUID_LENGTHPSK_LENGTH
 * @brief Identity max length
 */
#define UUID_LENGTH (128/8)
/**
 * @def MASTER_SECRET_LEN
 * @brief TLS master secret length
 */
#define MASTER_SECRET_LEN (48)

/**@def TLS_CLOSE_NOTIFY(peer, ret)
 *
 * Notifies of existing \a peer about closing TLS connection.
 *
 * @param[in] peer remote peer
 * @param[in] ret used internaly
 */
#define TLS_CLOSE_NOTIFY(peer, ret)                                                                \
do                                                                                                 \
{                                                                                                  \
    ret = mbedtls_ssl_close_notify(&peer->ssl);                                                    \
} while (MBEDTLS_ERR_SSL_WANT_WRITE == ret)

/**@def TLS_RET_HANDSHAKE_RES(peer)
 *
 * Sets handshake result for callback.
 *
 * @param[in] peer remote peer
 */
#define TLS_RET_HANDSHAKE_RES(peer)                                                                \
if (g_tlsHandshakeCallback)                                                                        \
{                                                                                                  \
    CAErrorInfo_t errorInfo = {.result = CA_STATUS_FAILED};                                        \
    g_tlsHandshakeCallback(&peer->sep.endpoint, &errorInfo);                                       \
}
/**@def TLS_CHECK_HANDSHAKE_FAIL(peer, ret, str, mutex, error, msg)
 *
 * Checks handshake result and send allert if needed.
 *
 * @param[in] peer remote peer
 * @param[in] ret error code
 * @param[in] str debug string
 * @param[in] mutex ca mutex
 * @param[in] return error code
 * @param[in] msg allert message
 */
#define TLS_CHECK_HANDSHAKE_FAIL(peer, ret, str, mutex, error, msg)                                \
if (0 != ret && MBEDTLS_ERR_SSL_WANT_READ != ret &&  MBEDTLS_ERR_SSL_WANT_WRITE != ret)            \
{                                                                                                  \
    OIC_LOG_V(ERROR, NET_TLS_TAG, "%s: -0x%x", str, -ret);                                         \
    if ((int) MBEDTLS_ERR_SSL_FATAL_ALERT_MESSAGE != (int) ret &&                                  \
        (int) MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY != (int) ret &&                                    \
        (int) MBEDTLS_ERR_SSL_BAD_HS_CLIENT_HELLO != (int) ret)                                    \
    {                                                                                              \
        mbedtls_ssl_send_alert_message(&peer->ssl, MBEDTLS_SSL_ALERT_LEVEL_FATAL, msg);            \
    }                                                                                              \
    TLS_RET_HANDSHAKE_RES(peer);                                                                   \
    removePeerFromList(&peer->sep.endpoint);                                                       \
    if (mutex)                                                                                     \
    {                                                                                              \
        oc_mutex_unlock(g_tlsContextMutex);                                                        \
    }                                                                                              \
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "Out %s", __func__);                                             \
    return error;                                                                                  \
}

typedef enum
{
    ADAPTER_TLS_RSA_WITH_AES_256_CBC_SHA,
    ADAPTER_TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8,
    ADAPTER_TLS_ECDH_ANON_WITH_AES_128_CBC_SHA_256,
    ADAPTER_TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256,
    ADAPTER_CIPHER_MAX
} AdapterCipher_t;

typedef enum
{
    ADAPTER_CURVE_SECP256R1,
    ADAPTER_CURVE_MAX
} AdapterCurve_t;

int tlsCipher[ADAPTER_CIPHER_MAX][2] =
{
    {MBEDTLS_TLS_RSA_WITH_AES_256_CBC_SHA, 0},
    {MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8, 0},
    {MBEDTLS_TLS_ECDH_ANON_WITH_AES_128_CBC_SHA256, 0},
    {MBEDTLS_TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256, 0}
};

static int g_cipherSuitesList[2 + 1];

mbedtls_ecp_group_id curve[ADAPTER_CURVE_MAX][2] =
{
    {MBEDTLS_ECP_DP_SECP256R1, 0}
};

static PkiInfo_t g_pkiInfo = {{NULL, 0}, {NULL, 0}, {NULL, 0}, {NULL, 0}};

typedef struct  {
    int code;
    int alert;
} CrtVerifyAlert_t;

static const CrtVerifyAlert_t crtVerifyAlerts[] = {
    {MBEDTLS_X509_BADCERT_EXPIRED,       MBEDTLS_SSL_ALERT_MSG_CERT_EXPIRED},
    {MBEDTLS_X509_BADCERT_REVOKED,       MBEDTLS_SSL_ALERT_MSG_CERT_REVOKED},
    {MBEDTLS_X509_BADCERT_CN_MISMATCH,   MBEDTLS_SSL_ALERT_MSG_CERT_UNKNOWN},
    {MBEDTLS_X509_BADCERT_NOT_TRUSTED,   MBEDTLS_SSL_ALERT_MSG_UNKNOWN_CA},
    {MBEDTLS_X509_BADCRL_NOT_TRUSTED,    MBEDTLS_SSL_ALERT_MSG_UNKNOWN_CA},
    {MBEDTLS_X509_BADCRL_EXPIRED,        MBEDTLS_SSL_ALERT_MSG_INSUFFICIENT_SECURITY},
    {MBEDTLS_X509_BADCERT_MISSING,       MBEDTLS_SSL_ALERT_MSG_NO_CERT},
    {MBEDTLS_X509_BADCERT_SKIP_VERIFY,   MBEDTLS_SSL_ALERT_MSG_INSUFFICIENT_SECURITY},
    {MBEDTLS_X509_BADCERT_OTHER,         MBEDTLS_SSL_ALERT_MSG_INTERNAL_ERROR},
    {MBEDTLS_X509_BADCERT_FUTURE,        MBEDTLS_SSL_ALERT_MSG_BAD_CERT},
    {MBEDTLS_X509_BADCRL_FUTURE,         MBEDTLS_SSL_ALERT_MSG_INSUFFICIENT_SECURITY},
    {MBEDTLS_X509_BADCERT_KEY_USAGE,     MBEDTLS_SSL_ALERT_MSG_BAD_CERT},
    {MBEDTLS_X509_BADCERT_EXT_KEY_USAGE, MBEDTLS_SSL_ALERT_MSG_BAD_CERT},
    {MBEDTLS_X509_BADCERT_NS_CERT_TYPE,  MBEDTLS_SSL_ALERT_MSG_BAD_CERT},
    {MBEDTLS_X509_BADCERT_BAD_MD,        MBEDTLS_SSL_ALERT_MSG_BAD_CERT},
    {MBEDTLS_X509_BADCERT_BAD_PK,        MBEDTLS_SSL_ALERT_MSG_BAD_CERT},
    {MBEDTLS_X509_BADCERT_BAD_KEY,       MBEDTLS_SSL_ALERT_MSG_BAD_CERT},
    {MBEDTLS_X509_BADCRL_BAD_MD,         MBEDTLS_SSL_ALERT_MSG_BAD_CERT},
    {MBEDTLS_X509_BADCRL_BAD_PK,         MBEDTLS_SSL_ALERT_MSG_BAD_CERT},
    {MBEDTLS_X509_BADCRL_BAD_KEY,        MBEDTLS_SSL_ALERT_MSG_BAD_CERT},
    {0, 0}
};

static int getAlertCode(uint32_t flags)
{
    const CrtVerifyAlert_t *cur;

    for (cur = crtVerifyAlerts; cur->alert != 0 ; cur++)
    {
        if (flags & cur->code)
        {
            return cur->alert;
        }
    }
    return 0;
}

#ifndef NDEBUG
/**
 * Pass a message to the OIC logger.
 *
 * @param[in] ctx  opaque context for the callback
 * @param[in] level  debug level
 * @param[in] file  file name
 * @param[in] line  line number
 * @param[in] str  message
 */
static void debugTls(void *ctx, int level, const char *file, int line, const char *str)
{
    ((void) level);
    ((void) file);
    ((void) line);

    OIC_LOG_V(DEBUG, MBED_TLS_TAG, "%s", str);
    fflush((FILE *) ctx);
}
#endif

/**
 * structure to holds the information of cache message and address info.
 */
typedef ByteArray TlsCacheMessage_t;


/**
 * Data structure for holding the send and recv callbacks.
 */
typedef struct TlsCallBacks
{
    CAPacketReceivedCallback recvCallback;  /**< Callback used to send data to upper layer. */
    CAPacketSendCallback sendCallback;      /**< Callback used to send data to socket layer. */
} TlsCallbacks_t;

/**
 * Data structure for holding the mbedTLS interface related info.
 */
typedef struct TlsContext
{
    u_arraylist_t *peerList;         /**< peer list which holds the mapping between
                                              peer id, it's n/w address and mbedTLS context. */
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context rnd;
    mbedtls_x509_crt ca;
    mbedtls_x509_crt crt;
    mbedtls_pk_context pkey;
    mbedtls_ssl_config clientConf;
    mbedtls_ssl_config serverConf;
    AdapterCipher_t cipher;
    TlsCallbacks_t adapterCallbacks[MAX_SUPPORTED_ADAPTERS];
    mbedtls_x509_crl crl;
    bool cipherFlag[2];
    int selectedCipher;

} TlsContext_t;

/**
 * @var g_caTlsContext
 * @brief global context which holds tls context and cache list information.
 */
static TlsContext_t * g_caTlsContext = NULL;

/**
 * @var g_getCredentialsCallback
 * @brief callback to get TLS credentials (same as for DTLS)
 */
static CAGetDTLSPskCredentialsHandler g_getCredentialsCallback = NULL;
/**
 * @var g_getCerdentilTypesCallback
 * @brief callback to get different credential types from SRM
 */
static CAgetCredentialTypesHandler g_getCredentialTypesCallback = NULL;
/**
 * @var g_getPkixInfoCallback
 *
 * @brief callback to get X.509-based Public Key Infrastructure
 */
static CAgetPkixInfoHandler g_getPkixInfoCallback = NULL;

/**
 * @var g_dtlsContextMutex
 * @brief Mutex to synchronize access to g_caTlsContext.
 */
static oc_mutex g_tlsContextMutex = NULL;

/**
 * @var g_tlsHandshakeCallback
 * @brief callback to deliver the TLS handshake result
 */
static CAErrorCallback g_tlsHandshakeCallback = NULL;

/**
 * Data structure for holding the data to be received.
 */
typedef struct TlsRecBuf
{
    uint8_t * buff;
    size_t len;
    size_t loaded;
} TlsRecBuf_t;

/**
 * Data structure for holding the data related to endpoint
 * and TLS session.
 */
typedef struct TlsEndPoint
{
    mbedtls_ssl_context ssl;
    CASecureEndpoint_t sep;
    u_arraylist_t * cacheList;
    TlsRecBuf_t recBuf;
    uint8_t master[MASTER_SECRET_LEN];
} TlsEndPoint_t;

void CAsetTlsCredentialsCallback(CAGetDTLSPskCredentialsHandler credCallback)
{
    // TODO Does this method needs protection of tlsContextMutex?
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "In %s", __func__);
    g_getCredentialsCallback = credCallback;
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "Out %s", __func__);
}

void CAsetPkixInfoCallback(CAgetPkixInfoHandler infoCallback)
{
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "In %s", __func__);
    g_getPkixInfoCallback = infoCallback;
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "Out %s", __func__);
}
void CAsetCredentialTypesCallback(CAgetCredentialTypesHandler credTypesCallback)
{
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "In %s", __func__);
    g_getCredentialTypesCallback = credTypesCallback;
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "Out %s", __func__);
}
/**
 * Write callback.
 *
 * @param[in]  tep    TLS endpoint
 * @param[in]  data    message
 * @param[in]  dataLen    message length
 *
 * @return  message length
 */
static int sendTls(void * tep, const unsigned char * data, size_t dataLen)
{
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "In %s", __func__);
    VERIFY_NON_NULL_RET(tep, NET_TLS_TAG, "secure endpoint is NULL", 0);
    VERIFY_NON_NULL_RET(data, NET_TLS_TAG, "data is NULL", 0);
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "Data len: %zu", dataLen);

    g_caTlsContext->adapterCallbacks[0].sendCallback(&(((TlsEndPoint_t * )tep)->sep.endpoint),
            (const void *) data, (uint32_t) dataLen);
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "Out %s", __func__);
    return dataLen;
}
/**
 * Read callback.
 *
 * @param[in]  tep    TLS endpoint
 * @param[in]  data    message
 * @param[in]  dataLen    message length
 *
 * @return  read length
 */
static int recvTls(void * tep, unsigned char * data, size_t dataLen)
{
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "In %s", __func__);
    VERIFY_NON_NULL_RET(tep, NET_TLS_TAG, "endpoint is NULL", 0);
    VERIFY_NON_NULL_RET(data, NET_TLS_TAG, "data is NULL", 0);

    TlsRecBuf_t *recBuf = &((TlsEndPoint_t *)tep)->recBuf;
    size_t retLen = (recBuf->len > recBuf->loaded ? recBuf->len - recBuf->loaded : 0);
    retLen = (retLen < dataLen ? retLen : dataLen);

    memcpy(data, recBuf->buff + recBuf->loaded, retLen);
    recBuf->loaded += retLen;

    OIC_LOG_V(DEBUG, NET_TLS_TAG, "Out %s", __func__);
    return (int)retLen;
}

/**
 * Parse chain of X.509 certificates.
 *
 * @param[out] crt     container for X.509 certificates
 * @param[in]  data    buffer with X.509 certificates. Certificates may be in either in PEM
                       or DER format in a jumble. Each PEM certificate must be NULL-terminated.
 * @param[in]  buflen  buffer length
 *
 * @return  0 on success, -1 on error
 */
static int parseChain(mbedtls_x509_crt * crt, const unsigned char * buf, int buflen)
{
    VERIFY_NON_NULL_RET(crt, NET_TLS_TAG, "Param crt is NULL" , -1);
    VERIFY_NON_NULL_RET(buf, NET_TLS_TAG, "Param buf is NULL" , -1);

    char pemCertHeader[] = {
        0x2d, 0x2d, 0x2d, 0x2d, 0x2d, 0x42, 0x45, 0x47, 0x49, 0x4e, 0x20, 0x43, 0x45, 0x52,
        0x54, 0x49, 0x46, 0x49, 0x43, 0x41, 0x54, 0x45, 0x2d, 0x2d, 0x2d, 0x2d, 0x2d
    };
    char pemCertFooter[] = {
        0x2d, 0x2d, 0x2d, 0x2d, 0x2d, 0x45, 0x4e, 0x44, 0x20, 0x43, 0x45, 0x52, 0x54, 0x49,
        0x46, 0x49, 0x43, 0x41, 0x54, 0x45, 0x2d, 0x2d, 0x2d, 0x2d, 0x2d, 0x0a, 0x00
    };
    size_t pemCertHeaderLen = sizeof(pemCertHeader);
    size_t pemCertFooterLen = sizeof(pemCertFooter);

    int pos = 0;
    int len = 0;
    int ret = 0;
    while (pos < buflen)
    {
        if (0x30 == buf[pos] && 0x82 == buf[pos + 1] && pos + 3 < buflen)
        {
            len = (((int) buf[pos+2]) << 8) | buf[pos+3];
            if (pos + len < buflen)
            {
                ret = mbedtls_x509_crt_parse_der(crt, buf+pos, len + 4); 
                if( 0 != ret)
                {
                    OIC_LOG_V(ERROR, NET_TLS_TAG, "mbedtls_x509_crt_parse returned -0x%x", -ret);
                    return -1;
                }
            }
            pos += len + 4;
        }
        else if (0 == memcmp(buf + pos, pemCertHeader, pemCertHeaderLen))
        {
            void * endPos = NULL;
            endPos = memmem(&(buf[pos]), buflen - pos, pemCertFooter, pemCertFooterLen);
            if (NULL == endPos)
            {
                OIC_LOG(ERROR, NET_TLS_TAG, "Error: end of PEM certificate not found.");
                return -1;
            }
            len = (char*)endPos - ((char*)(buf + pos)) + pemCertFooterLen;
            ret = mbedtls_x509_crt_parse(crt, buf, len);
            pos += len;
        }
        else
        {
             OIC_LOG_V(ERROR, NET_TLS_TAG, "mbedtls_x509_crt_parse returned -0x%x", -ret);
             return -1;
        }
    }
    return 0;
}
//Loads PKIX related information from SRM
static int loadX509()
{
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "In %s", __func__);
    VERIFY_NON_NULL_RET(g_getPkixInfoCallback, NET_TLS_TAG, "PKIX info callback is NULL", -1);
    bool loadOwnCred = true;
    g_getPkixInfoCallback(&g_pkiInfo);

    // optional
    int ret = parseChain(&g_caTlsContext->crt, g_pkiInfo.crt.data, g_pkiInfo.crt.len);
    if (0 != ret)
    {
        OIC_LOG(WARNING, NET_TLS_TAG, "Own certificate chain parsing error");
        goto required;
    }
    ret =  mbedtls_pk_parse_key(&g_caTlsContext->pkey, g_pkiInfo.key.data, g_pkiInfo.key.len,
                                                                               NULL, 0);
    if (0 != ret)
    {
        OIC_LOG(WARNING, NET_TLS_TAG, "Key parsing error");
        goto required;
    }

    ret = mbedtls_ssl_conf_own_cert(&g_caTlsContext->serverConf, &g_caTlsContext->crt,
                                                                 &g_caTlsContext->pkey);
    if (0 != ret)
    {
        OIC_LOG(WARNING, NET_TLS_TAG, "Own certificate parsing error");
        goto required;
    }
    ret = mbedtls_ssl_conf_own_cert( &g_caTlsContext->clientConf, &g_caTlsContext->crt,
                                                                  &g_caTlsContext->pkey);
    if(0 != ret)
    {
        OIC_LOG(WARNING, NET_TLS_TAG, "Own certificate configuration error");
        goto required;
    }

    required:

    ret = parseChain(&g_caTlsContext->ca, g_pkiInfo.ca.data, g_pkiInfo.ca.len);
    if(0 != ret)
    {
        OIC_LOG(ERROR, NET_TLS_TAG, "CA chain parsing error");
        OIC_LOG_V(DEBUG, NET_TLS_TAG, "Out %s", __func__);
        return -1;
    }

    ret = mbedtls_x509_crl_parse_der(&g_caTlsContext->crl, g_pkiInfo.crl.data, g_pkiInfo.crl.len);
    if(0 != ret)
    {
        OIC_LOG(WARNING, NET_TLS_TAG, "CRL parsing error");
        mbedtls_ssl_conf_ca_chain(&g_caTlsContext->clientConf, &g_caTlsContext->ca, NULL);
        mbedtls_ssl_conf_ca_chain(&g_caTlsContext->serverConf, &g_caTlsContext->ca, NULL);
    }
    else
    {
        mbedtls_ssl_conf_ca_chain(&g_caTlsContext->clientConf, &g_caTlsContext->ca,
                                                                              &g_caTlsContext->crl);
        mbedtls_ssl_conf_ca_chain(&g_caTlsContext->serverConf, &g_caTlsContext->ca,
                                                                              &g_caTlsContext->crl);
    }

    OIC_LOG_V(DEBUG, NET_TLS_TAG, "Out %s", __func__);
    return 0;
}

/*
 * PSK callback.
 *
 * @param[in]  notUsed     opaque context
 * @param[in]  ssl    mbedTLS context
 * @param[in]  desc    identity
 * @param[in]  descLen    identity length
 *
 * @return  0 on success any other return value will result in a denied PSK identity
 */
static int getTlsCredentialsCallback(void * notUsed, mbedtls_ssl_context * ssl,
                                     const unsigned char * desc, size_t descLen)
{
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "In %s", __func__);
    VERIFY_NON_NULL_RET(g_getCredentialsCallback, NET_TLS_TAG, "Credential callback s NULL", -1);
    VERIFY_NON_NULL_RET(ssl, NET_TLS_TAG, "ssl pointer is NULL", -1);
    VERIFY_NON_NULL_RET(desc, NET_TLS_TAG, "desc pointer is NULL", -1);
    if (descLen > CA_MAX_ENDPOINT_IDENTITY_LEN)
    {
        OIC_LOG(ERROR, NET_TLS_TAG, "desc too long!");
        return -1;
    }
    (void) notUsed;
    uint8_t keyBuf[PSK_LENGTH] = {0};

    // Retrieve the credentials blob from security module
    int ret = g_getCredentialsCallback(2, desc, descLen, keyBuf, PSK_LENGTH);
    if (ret > 0)
    {
        memcpy(((TlsEndPoint_t *) ssl)->sep.identity.id, desc, descLen);
        ((TlsEndPoint_t *) ssl)->sep.identity.id_length = descLen;

        OIC_LOG_V(DEBUG, NET_TLS_TAG, "Out %s", __func__);
        return(mbedtls_ssl_set_hs_psk(ssl, keyBuf, PSK_LENGTH));
    }
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "Out %s", __func__);
    return -1;
}
/**
 * Gets session corresponding for endpoint.
 *
 * @param[in]  peer    remote address
 *
 * @return  TLS session or NULL
 */
static TlsEndPoint_t *getTlsPeer(const CAEndpoint_t *peer)
{
    uint32_t listIndex = 0;
    uint32_t listLength = 0;
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "In %s", __func__);
    VERIFY_NON_NULL_RET(peer, NET_TLS_TAG, "TLS peer is NULL", NULL);

    TlsEndPoint_t *tep = NULL;
    listLength = u_arraylist_length(g_caTlsContext->peerList);
    for (listIndex = 0; listIndex < listLength; listIndex++)
    {
        tep = (TlsEndPoint_t *) u_arraylist_get(g_caTlsContext->peerList, listIndex);
        if (NULL == tep)
        {
            continue;
        }
        OIC_LOG_V(DEBUG, NET_TLS_TAG, "Compare [%s] and [%s]", peer->addr, tep->sep.endpoint.addr);
        if((0 == strncmp(peer->addr, tep->sep.endpoint.addr, MAX_ADDR_STR_SIZE_CA))
                && (peer->port == tep->sep.endpoint.port))
        {
            OIC_LOG_V(DEBUG, NET_TLS_TAG, "Out %s", __func__);
            return tep;
        }
    }
    OIC_LOG(DEBUG, NET_TLS_TAG, "Return NULL");
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "Out %s", __func__);
    return NULL;
}
/**
 * Deletes cached message.
 *
 * @param[in]  msg    message
 */
static void deleteTlsCacheMessage(TlsCacheMessage_t * msg)
{
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "In %s", __func__);
    VERIFY_NON_NULL_VOID(msg, NET_TLS_TAG, "msg");

    OICFree(msg->data);
    OICFree(msg);

    OIC_LOG_V(DEBUG, NET_TLS_TAG, "Out %s", __func__);
}
/**
 * Deletes cached message list.
 *
 * @param[in] cacheList  list of cached messages
 */
static void deleteCacheList(u_arraylist_t * cacheList)
{
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "In %s", __func__);
    VERIFY_NON_NULL_VOID(cacheList, NET_TLS_TAG, "cacheList");
    uint32_t listIndex = 0;
    uint32_t listLength = 0;

    listLength = u_arraylist_length(cacheList);
    for (listIndex = 0; listIndex < listLength; listIndex++)
    {
        TlsCacheMessage_t * msg = (TlsCacheMessage_t *) u_arraylist_get(cacheList, listIndex);
        if (NULL != msg)
        {
            deleteTlsCacheMessage(msg);
        }
    }
    u_arraylist_free(&cacheList);

    OIC_LOG_V(DEBUG, NET_TLS_TAG, "Out %s", __func__);
}
/**
 * Deletes endpoint with session.
 *
 * @param[in]  tep    endpoint with session info
 */
static void deleteTlsEndPoint(TlsEndPoint_t * tep)
{
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "In %s", __func__);
    VERIFY_NON_NULL_VOID(tep, NET_TLS_TAG, "tep");

    mbedtls_ssl_free(&tep->ssl);
    deleteCacheList(tep->cacheList);
    OICFree(tep);
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "Out %s", __func__);
}
/**
 * Removes endpoint session from list.
 *
 * @param[in]  endpoint    remote address
 */
static void removePeerFromList(CAEndpoint_t * endpoint)
{
    uint32_t listLength = u_arraylist_length(g_caTlsContext->peerList);
    VERIFY_NON_NULL_VOID(endpoint, NET_TLS_TAG, "endpoint");
    for (uint32_t listIndex = 0; listIndex < listLength; listIndex++)
    {
        TlsEndPoint_t * tep = (TlsEndPoint_t *)u_arraylist_get(g_caTlsContext->peerList,listIndex);
        if (NULL == tep)
        {
            continue;
        }
        if(0 == strncmp(endpoint->addr, tep->sep.endpoint.addr, MAX_ADDR_STR_SIZE_CA)
                && (endpoint->port == tep->sep.endpoint.port))
        {
            u_arraylist_remove(g_caTlsContext->peerList, listIndex);
            deleteTlsEndPoint(tep);
            return;
        }
    }
}
/**
 * Deletes session list.
 */
static void deletePeerList()
{
    uint32_t listLength = u_arraylist_length(g_caTlsContext->peerList);
    for (uint32_t listIndex = 0; listIndex < listLength; listIndex++)
    {
        TlsEndPoint_t * tep = (TlsEndPoint_t *)u_arraylist_get(g_caTlsContext->peerList,listIndex);
        if (NULL == tep)
        {
            continue;
        }
        deleteTlsEndPoint(tep);
    }
    u_arraylist_free(&g_caTlsContext->peerList);
}

CAResult_t CAcloseTlsConnection(const CAEndpoint_t *endpoint)
{
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "In %s", __func__);
    VERIFY_NON_NULL_RET(endpoint, NET_TLS_TAG, "Param endpoint is NULL" , CA_STATUS_INVALID_PARAM);

    oc_mutex_lock(g_tlsContextMutex);
    if (NULL == g_caTlsContext)
    {
        OIC_LOG(ERROR, NET_TLS_TAG, "Context is NULL");
        oc_mutex_unlock(g_tlsContextMutex);
        return CA_STATUS_FAILED;
    }
    TlsEndPoint_t * tep = getTlsPeer(endpoint);
    if (NULL == tep)
    {
        OIC_LOG(ERROR, NET_TLS_TAG, "Session does not exist");
        oc_mutex_unlock(g_tlsContextMutex);
        return CA_STATUS_FAILED;
    }
    /* No error checking, the connection might be closed already */
    int ret = 0;
    do
    {
        ret = mbedtls_ssl_close_notify(&tep->ssl);
    }
    while (MBEDTLS_ERR_SSL_WANT_WRITE == ret);

    removePeerFromList(&tep->sep.endpoint);
    oc_mutex_unlock(g_tlsContextMutex);

    OIC_LOG_V(DEBUG, NET_TLS_TAG, "Out %s", __func__);
    return CA_STATUS_OK;
}
/**
 * Creates session for endpoint.
 *
 * @param[in]  endpoint    remote address
 * @param[in]  config    mbedTLS configuration info
 *
 * @return  TLS endpoint or NULL
 */
static TlsEndPoint_t * newTlsEndPoint(const CAEndpoint_t * endpoint, mbedtls_ssl_config * config)
{
    TlsEndPoint_t * tep = NULL;
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "In %s", __func__);
    VERIFY_NON_NULL_RET(endpoint, NET_TLS_TAG, "endpoint", NULL);
    VERIFY_NON_NULL_RET(config, NET_TLS_TAG, "config", NULL);

    tep = (TlsEndPoint_t *) OICCalloc(1, sizeof (TlsEndPoint_t));
    if (NULL == tep)
    {
        OIC_LOG(ERROR, NET_TLS_TAG, "Malloc failed!");
        return NULL;
    }

    tep->sep.endpoint = *endpoint;
    tep->sep.endpoint.flags |= CA_SECURE;

    if(0 != mbedtls_ssl_setup( &tep->ssl, config))
    {
        OIC_LOG(ERROR, NET_TLS_TAG, "Setup failed");
        OICFree(tep);
        OIC_LOG_V(DEBUG, NET_TLS_TAG, "Out %s", __func__);
        return NULL;
    }

    mbedtls_ssl_set_bio(&tep->ssl, tep, sendTls, recvTls, NULL);

    tep->cacheList = u_arraylist_create();
    if (NULL == tep->cacheList)
    {
        OIC_LOG(ERROR, NET_TLS_TAG, "cacheList initialization failed!");
        mbedtls_ssl_free(&tep->ssl);
        OICFree(tep);
        OIC_LOG_V(DEBUG, NET_TLS_TAG, "Out %s", __func__);
        return NULL;
    }
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "Out %s", __func__);
    return tep;
}
/**
 * Initializes PSK identity.
 *
 * @param[out]  config    client/server config to be updated
 *
 * @return  0 on success or -1 on error
 */
static int initPskIdentity(mbedtls_ssl_config * config)
{
    uint8_t idBuf[UUID_LENGTH] = {0};
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "In %s", __func__);
    VERIFY_NON_NULL_RET(config, NET_TLS_TAG, "Param config is NULL" , -1);

    if (0 > g_getCredentialsCallback(CA_DTLS_PSK_IDENTITY, NULL, 0, idBuf, UUID_LENGTH))
    {
        OIC_LOG(ERROR, NET_TLS_TAG, "Identity not found");
        OIC_LOG_V(DEBUG, NET_TLS_TAG, "Out %s", __func__);
        return -1;
    }
    if (0 != mbedtls_ssl_conf_psk(config, idBuf, 0, idBuf, UUID_LENGTH))
    {
        OIC_LOG(ERROR, NET_TLS_TAG, "Identity initialization failed!");
        OIC_LOG_V(DEBUG, NET_TLS_TAG, "Out %s", __func__);
        return -1;
    }
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "Out %s", __func__);
    return 0;
}
static void setupCipher(mbedtls_ssl_config * config)
{
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "In %s", __func__);
    if (NULL == g_getCredentialTypesCallback)
    {
        OIC_LOG(ERROR, NET_TLS_TAG, "Param callback is null");
        return;
    }

    g_getCredentialTypesCallback(g_caTlsContext->cipherFlag);
    // Retrieve the PSK credential from SRM
    // PIN OTM if (true == g_caTlsContext->cipherFlag[0] && 0 != initPskIdentity(config))
    if (0 != initPskIdentity(config))
    {
        OIC_LOG(ERROR, NET_TLS_TAG, "PSK identity initialization failed!");
    }

    // Retrieve the ECC credential from SRM
    if (true == g_caTlsContext->cipherFlag[1] || ADAPTER_TLS_RSA_WITH_AES_256_CBC_SHA == g_caTlsContext->cipher)
    {
        int ret = loadX509();
        if (0 != ret)
        {
            OIC_LOG(ERROR, NET_TLS_TAG, "Failed to init X.509");
        }
    }
    if (ADAPTER_CIPHER_MAX == g_caTlsContext->cipher)
    {
        int index = 0;
        memset(g_cipherSuitesList, 0, sizeof(g_cipherSuitesList));
        if (true == g_caTlsContext->cipherFlag[1])
        {
            g_cipherSuitesList[index] = MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8;
            index ++;
        }
        if (true == g_caTlsContext->cipherFlag[0])
        {
           g_cipherSuitesList[index] = MBEDTLS_TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256;
        }

        mbedtls_ssl_conf_ciphersuites(config, g_cipherSuitesList);
    }

    OIC_LOG_V(DEBUG, NET_TLS_TAG, "Out %s", __func__);
}
/**
 * Initiate TLS handshake with endpoint.
 *
 * @param[in]  endpoint    remote address
 *
 * @return  TLS endpoint or NULL
 */
static TlsEndPoint_t * initiateTlsHandshake(const CAEndpoint_t *endpoint)
{
    int ret = 0;
    TlsEndPoint_t * tep = NULL;

    OIC_LOG_V(DEBUG, NET_TLS_TAG, "In %s", __func__);
    VERIFY_NON_NULL_RET(endpoint, NET_TLS_TAG, "Param endpoint is NULL" , NULL);



    tep = newTlsEndPoint(endpoint, &g_caTlsContext->clientConf);
    if (NULL == tep)
    {
        OIC_LOG(ERROR, NET_TLS_TAG, "Malloc failed!");
        return NULL;
    }

    //Load allowed SVR suites from SVR DB
    setupCipher(&g_caTlsContext->clientConf);

    OIC_LOG_V(DEBUG, NET_TLS_TAG, "Add %s:%d", tep->sep.endpoint.addr, tep->sep.endpoint.port);
    ret = u_arraylist_add(g_caTlsContext->peerList, (void *) tep);
    if (!ret)
    {
        OIC_LOG(ERROR, NET_TLS_TAG, "u_arraylist_add failed!");
        deleteTlsEndPoint(tep);
        return NULL;
    }

    while (MBEDTLS_SSL_HANDSHAKE_OVER > tep->ssl.state)
    {
        ret = mbedtls_ssl_handshake_step(&tep->ssl);
        if (MBEDTLS_ERR_SSL_CONN_EOF == ret)
        {
            break;
        }
        TLS_CHECK_HANDSHAKE_FAIL(tep, ret, "Handshake error", 0, NULL, MBEDTLS_SSL_ALERT_MSG_HANDSHAKE_FAILURE);
    }
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "Out %s", __func__);
    return tep;
}

void CAdeinitTlsAdapter()
{
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "In %s", __func__);

    VERIFY_NON_NULL_VOID(g_caTlsContext, NET_TLS_TAG, "context is NULL");
    VERIFY_NON_NULL_VOID(g_tlsContextMutex, NET_TLS_TAG, "context mutex is NULL");

    //Lock tlsContext mutex
    oc_mutex_lock(g_tlsContextMutex);

    // Clear all lists
    deletePeerList();

    // De-initialize mbedTLS
    mbedtls_x509_crt_free(&g_caTlsContext->crt);
    mbedtls_pk_free(&g_caTlsContext->pkey);
    mbedtls_ssl_config_free(&g_caTlsContext->clientConf);
    mbedtls_ssl_config_free(&g_caTlsContext->serverConf);
    mbedtls_ctr_drbg_free(&g_caTlsContext->rnd);
    mbedtls_entropy_free(&g_caTlsContext->entropy);

    // De-initialize tls Context
    OICFree(g_caTlsContext);
    g_caTlsContext = NULL;

    // Unlock tlsContext mutex and de-initialize it
    oc_mutex_unlock(g_tlsContextMutex);
    oc_mutex_free(g_tlsContextMutex);
    g_tlsContextMutex = NULL;

    OIC_LOG_V(DEBUG, NET_TLS_TAG, "Out %s ", __func__);
}

CAResult_t CAinitTlsAdapter()
{
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "In %s", __func__);

    // Initialize mutex for tlsContext
    if (NULL == g_tlsContextMutex)
    {
        g_tlsContextMutex = oc_mutex_new();
        VERIFY_NON_NULL_RET(g_tlsContextMutex, NET_TLS_TAG, "malloc failed",
                            CA_MEMORY_ALLOC_FAILED);
    }
    else
    {
        OIC_LOG(ERROR, NET_TLS_TAG, "CAAdapterNettlsInit done already!");
        return CA_STATUS_OK;
    }

    // Lock tlsContext mutex and create tlsContext
    oc_mutex_lock(g_tlsContextMutex);
    g_caTlsContext = (TlsContext_t *)OICCalloc(1, sizeof(TlsContext_t));

    if (NULL == g_caTlsContext)
    {
        OIC_LOG(ERROR, NET_TLS_TAG, "Context malloc failed");
        oc_mutex_unlock(g_tlsContextMutex);
        oc_mutex_free(g_tlsContextMutex);
        g_tlsContextMutex = NULL;
        return CA_MEMORY_ALLOC_FAILED;
    }

    // Create peer list
    g_caTlsContext->peerList = u_arraylist_create();

    if(NULL == g_caTlsContext->peerList)
    {
        OIC_LOG(ERROR, NET_TLS_TAG, "peerList initialization failed!");
        OICFree(g_caTlsContext);
        g_caTlsContext = NULL;
        oc_mutex_unlock(g_tlsContextMutex);
        oc_mutex_free(g_tlsContextMutex);
        g_tlsContextMutex = NULL;
        return CA_STATUS_FAILED;
    }

    /* Initialize TLS library
     */
#ifndef NDEBUG
    char version[MBED_TLS_VERSION_LEN];
    mbedtls_version_get_string(version);
    OIC_LOG_V(INFO, NET_TLS_TAG, "mbed TLS version: %s", version);
#endif

    /* Entropy settings
     */
    mbedtls_entropy_init(&g_caTlsContext->entropy);
    mbedtls_ctr_drbg_init(&g_caTlsContext->rnd);

#ifdef __unix__
    unsigned char seed[sizeof(SEED)] = {0};
    int urandomFd = -2;
    urandomFd = open("/dev/urandom", O_RDONLY);
    if(urandomFd == -1)
    {
        OIC_LOG(ERROR, NET_TLS_TAG, "Fails open /dev/urandom!");
        oc_mutex_unlock(g_tlsContextMutex);
        CAdeinitTlsAdapter();
        return CA_STATUS_FAILED;
    }
    if(0 > read(urandomFd, seed, sizeof(seed)))
    {
        OIC_LOG(ERROR, NET_TLS_TAG, "Fails read from /dev/urandom!");
        close(urandomFd);
        oc_mutex_unlock(g_tlsContextMutex);
        CAdeinitTlsAdapter();
        return CA_STATUS_FAILED;
    }
    close(urandomFd);

#else
    unsigned char * seed = (unsigned char*) SEED;
#endif
    if(0 != mbedtls_ctr_drbg_seed(&g_caTlsContext->rnd, mbedtls_entropy_func,
                                  &g_caTlsContext->entropy, seed, sizeof(SEED)))
    {
        OIC_LOG(ERROR, NET_TLS_TAG, "Seed initialization failed!");
        oc_mutex_unlock(g_tlsContextMutex);
        CAdeinitTlsAdapter();
        return CA_STATUS_FAILED;
    }
    mbedtls_ctr_drbg_set_prediction_resistance(&g_caTlsContext->rnd, MBEDTLS_CTR_DRBG_PR_OFF);

    /* Client SSL configuration
     */
    mbedtls_ssl_config_init(&g_caTlsContext->clientConf);
    if (mbedtls_ssl_config_defaults(&g_caTlsContext->clientConf, MBEDTLS_SSL_IS_CLIENT,
                                    MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT) != 0)
    {
        OIC_LOG(ERROR, NET_TLS_TAG, "Client config initialization failed!");
        oc_mutex_unlock(g_tlsContextMutex);
        CAdeinitTlsAdapter();
        return CA_STATUS_FAILED;
    }

    OIC_LOG_V(DEBUG, NET_TLS_TAG, "Out %s", __func__);

    mbedtls_ssl_conf_psk_cb(&g_caTlsContext->clientConf, getTlsCredentialsCallback, NULL);
    mbedtls_ssl_conf_rng( &g_caTlsContext->clientConf, mbedtls_ctr_drbg_random,
                          &g_caTlsContext->rnd);
    mbedtls_ssl_conf_curves(&g_caTlsContext->clientConf, curve[ADAPTER_CURVE_SECP256R1]);
    mbedtls_ssl_conf_min_version(&g_caTlsContext->clientConf, MBEDTLS_SSL_MAJOR_VERSION_3,
                                 MBEDTLS_SSL_MINOR_VERSION_1);
    mbedtls_ssl_conf_renegotiation(&g_caTlsContext->clientConf, MBEDTLS_SSL_RENEGOTIATION_DISABLED);

    mbedtls_ssl_conf_authmode(&g_caTlsContext->clientConf, MBEDTLS_SSL_VERIFY_REQUIRED);
#ifndef NDEBUG
    mbedtls_ssl_conf_dbg( &g_caTlsContext->clientConf, debugTls, NULL);
#endif

    /* Server SSL configuration
     */
    mbedtls_ssl_config_init(&g_caTlsContext->serverConf);
    if (mbedtls_ssl_config_defaults(&g_caTlsContext->serverConf, MBEDTLS_SSL_IS_SERVER,
                                    MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT) != 0)
    {
        OIC_LOG(ERROR, NET_TLS_TAG, "Server config initialization failed!");
        oc_mutex_unlock(g_tlsContextMutex);
        CAdeinitTlsAdapter();
        return CA_STATUS_FAILED;
    }

    mbedtls_ssl_conf_psk_cb(&g_caTlsContext->serverConf, getTlsCredentialsCallback, NULL);
    mbedtls_ssl_conf_rng( &g_caTlsContext->serverConf, mbedtls_ctr_drbg_random,
                          &g_caTlsContext->rnd);
    mbedtls_ssl_conf_curves(&g_caTlsContext->serverConf, curve[ADAPTER_CURVE_SECP256R1]);
    mbedtls_ssl_conf_min_version(&g_caTlsContext->serverConf, MBEDTLS_SSL_MAJOR_VERSION_3,
                                 MBEDTLS_SSL_MINOR_VERSION_1);
    mbedtls_ssl_conf_renegotiation(&g_caTlsContext->serverConf, MBEDTLS_SSL_RENEGOTIATION_DISABLED);
    mbedtls_ssl_conf_authmode(&g_caTlsContext->serverConf, MBEDTLS_SSL_VERIFY_REQUIRED);

#ifndef NDEBUG
    mbedtls_ssl_conf_dbg( &g_caTlsContext->serverConf, debugTls, NULL);
    mbedtls_debug_set_threshold(MBED_TLS_DEBUG_LEVEL);
#endif

    // set default cipher
    g_caTlsContext->cipher = ADAPTER_CIPHER_MAX;

    // init X.509
    mbedtls_x509_crt_init(&g_caTlsContext->ca);
    mbedtls_x509_crt_init(&g_caTlsContext->crt);
    mbedtls_pk_init(&g_caTlsContext->pkey);
    mbedtls_x509_crl_init(&g_caTlsContext->crl);

    oc_mutex_unlock(g_tlsContextMutex);

    OIC_LOG_V(DEBUG, NET_TLS_TAG, "Out %s", __func__);
    return CA_STATUS_OK;
}
/**
 * Creates cache message structure and fills with data.
 *
 * @param[in]  data    data array
 * @param[in]  dataLen    data array length
 *
 * @return  message or NULL
 */
TlsCacheMessage_t *  newTlsCacheMessage(uint8_t * data, size_t dataLen)
{
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "In %s", __func__);
    VERIFY_NON_NULL_RET(data, NET_TLS_TAG, "Param data is NULL" , NULL);
    if (0 == dataLen)
    {
        OIC_LOG(ERROR, NET_TLS_TAG, "dataLen is equal to zero");
        OIC_LOG_V(DEBUG, NET_TLS_TAG, "Out %s", __func__);
        return NULL;
    }
    TlsCacheMessage_t * message = (TlsCacheMessage_t *) OICCalloc(1, sizeof(TlsCacheMessage_t));
    if (NULL == message)
    {
        OIC_LOG(ERROR, NET_TLS_TAG, "calloc failed!");
        OIC_LOG_V(DEBUG, NET_TLS_TAG, "Out %s", __func__);
        return NULL;
    }

    message->data = (uint8_t *)OICCalloc(dataLen, sizeof(uint8_t));
    if (NULL == message->data)
    {
        OIC_LOG(ERROR, NET_TLS_TAG, "calloc failed!");
        OICFree(message);
        OIC_LOG_V(DEBUG, NET_TLS_TAG, "Out %s", __func__);
        return NULL;
    }
    memcpy(message->data, data, dataLen);
    message->len = dataLen;
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "Out %s", __func__);
    return message;
}

/* Send data via TLS connection.
 */
CAResult_t CAencryptTls(const CAEndpoint_t *endpoint,
                        void *data, uint32_t dataLen)
{
    int ret = 0;

    OIC_LOG_V(DEBUG, NET_TLS_TAG, "In %s ", __func__);

    VERIFY_NON_NULL_RET(endpoint, NET_TLS_TAG,"Remote address is NULL", CA_STATUS_INVALID_PARAM);
    VERIFY_NON_NULL_RET(data, NET_TLS_TAG, "Data is NULL", CA_STATUS_INVALID_PARAM);

    if (0 == dataLen)
    {
        OIC_LOG_V(ERROR, NET_TLS_TAG, "dataLen is zero [%d]", dataLen);
        return CA_STATUS_FAILED;
    }

    OIC_LOG_V(DEBUG, NET_TLS_TAG, "Data to be encrypted dataLen [%d]", dataLen);

    oc_mutex_lock(g_tlsContextMutex);
    if(NULL == g_caTlsContext)
    {
        OIC_LOG(ERROR, NET_TLS_TAG, "Context is NULL");
        oc_mutex_unlock(g_tlsContextMutex);
        return CA_STATUS_FAILED;
    }

    TlsEndPoint_t * tep = getTlsPeer(endpoint);
    if (NULL == tep)
    {
        tep = initiateTlsHandshake(endpoint);
    }
    if (NULL == tep)
    {
        OIC_LOG(ERROR, NET_TLS_TAG, "TLS handshake failed");
        oc_mutex_unlock(g_tlsContextMutex);
        return CA_STATUS_FAILED;
    }

    if (MBEDTLS_SSL_HANDSHAKE_OVER == tep->ssl.state)
    {
        ret = mbedtls_ssl_write(&tep->ssl, (unsigned char *) data, dataLen);

        if(ret < 0)
        {
            OIC_LOG_V(ERROR, NET_TLS_TAG, "mbedTLS write returned %d", ret);
            if (g_tlsHandshakeCallback)
            {
                CAErrorInfo_t errorInfo = {.result = CA_STATUS_FAILED};
                g_tlsHandshakeCallback(&tep->sep.endpoint, &errorInfo);
            }
            removePeerFromList(&tep->sep.endpoint);
            oc_mutex_unlock(g_tlsContextMutex);
            return CA_STATUS_FAILED;
        }
    }
    else
    {
        TlsCacheMessage_t * msg = newTlsCacheMessage(data, dataLen);
        if (NULL == msg || !u_arraylist_add(tep->cacheList, (void *) msg))
        {
            OIC_LOG(ERROR, NET_TLS_TAG, "u_arraylist_add failed!");
            oc_mutex_unlock(g_tlsContextMutex);
            return CA_STATUS_FAILED;
        }
    }

    oc_mutex_unlock(g_tlsContextMutex);

    OIC_LOG_V(DEBUG, NET_TLS_TAG, "Out %s", __func__);
    return CA_STATUS_OK;
}
/**
 * Sends cached messages via TLS connection.
 *
 * @param[in]  tep    remote address with session info
 */
static void sendCacheMessages(TlsEndPoint_t * tep)
{
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "In %s", __func__);
    VERIFY_NON_NULL_VOID(tep, NET_TLS_TAG, "Param tep is NULL");

    uint32_t listIndex = 0;
    uint32_t listLength = 0;
    listLength = u_arraylist_length(tep->cacheList);
    for (listIndex = 0; listIndex < listLength;)
    {
        int ret = 0;
        TlsCacheMessage_t * msg = (TlsCacheMessage_t *) u_arraylist_get(tep->cacheList, listIndex);
        if (NULL != msg && NULL != msg->data && 0 != msg->len)
        {
            do
            {
                ret = mbedtls_ssl_write(&tep->ssl, (unsigned char *) msg->data, msg->len);
            }
            while(MBEDTLS_ERR_SSL_WANT_WRITE == ret);

            if(ret < 0)
            {
                OIC_LOG_V(ERROR, NET_TLS_TAG,"mbedTLS write returned %d", ret );
            }
            if (u_arraylist_remove(tep->cacheList, listIndex))
            {
                deleteTlsCacheMessage(msg);
                // Reduce list length by 1 as we removed one element.
                listLength--;
            }
            else
            {
                OIC_LOG(ERROR, NET_TLS_TAG, "u_arraylist_remove failed.");
                break;
            }
        }
        else
        {
            // Move to the next element
            ++listIndex;
        }
    }
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "Out %s", __func__);
}

void CAsetTlsHandshakeCallback(CAErrorCallback tlsHandshakeCallback)
{
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "In %s", __func__);
    g_tlsHandshakeCallback = tlsHandshakeCallback;
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "Out %s", __func__);
}
// TODO move ConvertStrToUuid function to common module
/*
 * Converts string UUID to CARemoteId_t
 *
 * @param strUuid Device UUID in string format
 * @param uuid converted UUID in CARemoteId_t format
 *
 * @return 0 for success.
 * */
static int ConvertStrToUuid(const char* strUuid, CARemoteId_t* uuid)
{
    if(NULL == strUuid || NULL == uuid)
    {
        OIC_LOG(ERROR, NET_TLS_TAG, "ConvertStrToUuid : Invalid param");
        return -1;
    }

    size_t urnIdx = 0;
    size_t uuidIdx = 0;
    size_t strUuidLen = 0;
    char convertedUuid[UUID_LENGTH * 2] = {0};

    strUuidLen = strlen(strUuid);
    if(0 == strUuidLen)
    {
        OIC_LOG(INFO, NET_TLS_TAG, "The empty string detected, The UUID will be converted to "\
                           "\"00000000-0000-0000-0000-000000000000\"");
    }
    else if(UUID_LENGTH * 2 + 4 == strUuidLen)
    {
        for(uuidIdx=0, urnIdx=0; uuidIdx < UUID_LENGTH ; uuidIdx++, urnIdx+=2)
        {
            if(*(strUuid + urnIdx) == '-')
            {
                urnIdx++;
            }
            sscanf(strUuid + urnIdx, "%2hhx", &convertedUuid[uuidIdx]);
        }
    }
    else
    {
        OIC_LOG(ERROR, NET_TLS_TAG, "Invalid string uuid format");
        return -1;
    }

    memcpy(uuid->id, convertedUuid, UUID_LENGTH);
    uuid->id_length = UUID_LENGTH;
    return 0;
}

/* Read data from TLS connection
 */
CAResult_t CAdecryptTls(const CASecureEndpoint_t *sep, uint8_t *data, uint32_t dataLen)
{
    int ret = 0;
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "In %s", __func__);
    VERIFY_NON_NULL_RET(sep, NET_TLS_TAG, "endpoint is NULL" , CA_STATUS_INVALID_PARAM);
    VERIFY_NON_NULL_RET(data, NET_TLS_TAG, "Param data is NULL" , CA_STATUS_INVALID_PARAM);

    oc_mutex_lock(g_tlsContextMutex);
    if (NULL == g_caTlsContext)
    {
        OIC_LOG(ERROR, NET_TLS_TAG, "Context is NULL");
        oc_mutex_unlock(g_tlsContextMutex);
        return CA_STATUS_FAILED;
    }


    TlsEndPoint_t * peer = getTlsPeer(&sep->endpoint);
    if (NULL == peer)
    {
        peer = newTlsEndPoint(&sep->endpoint, &g_caTlsContext->serverConf);
        if (NULL == peer)
        {
            OIC_LOG(ERROR, NET_TLS_TAG, "Malloc failed!");
            oc_mutex_unlock(g_tlsContextMutex);
            return CA_STATUS_FAILED;
        }
        //Load allowed SVR suites from SVR DB
        setupCipher(&g_caTlsContext->serverConf);

        ret = u_arraylist_add(g_caTlsContext->peerList, (void *) peer);
        if (!ret)
        {
            OIC_LOG(ERROR, NET_TLS_TAG, "u_arraylist_add failed!");
            OICFree(peer);
            oc_mutex_unlock(g_tlsContextMutex);
            return CA_STATUS_FAILED;
        }
    }

    peer->recBuf.buff = data;
    peer->recBuf.len = dataLen;
    peer->recBuf.loaded = 0;

    OIC_LOG(DEBUG, NET_TLS_TAG, "Call mbedTLS handshake steps");

    while (MBEDTLS_SSL_HANDSHAKE_OVER > peer->ssl.state)
    {
        ret = mbedtls_ssl_handshake_step(&peer->ssl);
        if (MBEDTLS_ERR_SSL_CONN_EOF == ret)
        {
            break;
        }
        uint32_t flags = mbedtls_ssl_get_verify_result(&peer->ssl);
        if (0 != flags)
        {
            OIC_LOG_BUFFER(ERROR, NET_TLS_TAG, &flags, sizeof(flags));
            TLS_CHECK_HANDSHAKE_FAIL(peer, flags, "Cert verification failed", 1,
                                                     CA_STATUS_FAILED, getAlertCode(flags));
        }
        TLS_CHECK_HANDSHAKE_FAIL(peer, ret, "Handshake error", 1, CA_STATUS_FAILED, MBEDTLS_SSL_ALERT_MSG_HANDSHAKE_FAILURE);
        if (MBEDTLS_SSL_CLIENT_CHANGE_CIPHER_SPEC == peer->ssl.state)
        {
            memcpy(peer->master, peer->ssl.session_negotiate->master, sizeof(peer->master));
            g_caTlsContext->selectedCipher = peer->ssl.session_negotiate->ciphersuite;
        }

        if (MBEDTLS_SSL_HANDSHAKE_OVER == peer->ssl.state)
        {
            if (MBEDTLS_SSL_IS_CLIENT == peer->ssl.conf->endpoint)
            {
                sendCacheMessages(peer);
                if (g_tlsHandshakeCallback)
                {
                    CAErrorInfo_t errorInfo = {.result = CA_STATUS_OK};
                    g_tlsHandshakeCallback(&peer->sep.endpoint, &errorInfo);
                }
            }
            else
            {
                if (MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8 == g_caTlsContext->selectedCipher)
                {
                    char uuid[UUID_LENGTH * 2 + 5] = {0};
                    void * uuidPos = NULL;
                    void * userIdPos = NULL;
                    const mbedtls_x509_crt * peerCert = mbedtls_ssl_get_peer_cert(&peer->ssl);
                    ret = (NULL == peerCert ? -1 : 0);
                    TLS_CHECK_HANDSHAKE_FAIL(peer, ret, "Failed to retrieve cert", 1,
                                                CA_STATUS_FAILED, MBEDTLS_SSL_ALERT_MSG_NO_CERT);
                    uuidPos = memmem((void *) peerCert->subject_raw.p, peerCert->subject_raw.len,
                                                     (void *) UUID_PREFIX, sizeof(UUID_PREFIX) - 1);

                    ret = (NULL == uuidPos ? -1 : 0);
                    TLS_CHECK_HANDSHAKE_FAIL(peer, ret, "Failed to retrieve subject", 1,
                                        CA_STATUS_FAILED, MBEDTLS_SSL_ALERT_MSG_UNSUPPORTED_CERT);

                    memcpy(uuid, uuidPos + sizeof(UUID_PREFIX) - 1, UUID_LENGTH * 2 + 4);
                    ret = ConvertStrToUuid(uuid, &peer->sep.identity);
                    TLS_CHECK_HANDSHAKE_FAIL(peer, ret, "Failed to convert subject", 1,
                                        CA_STATUS_FAILED, MBEDTLS_SSL_ALERT_MSG_UNSUPPORTED_CERT);

                    userIdPos = memmem((void *) peerCert->subject_raw.p, peerCert->subject_raw.len,
                                                 (void *) USERID_PREFIX, sizeof(USERID_PREFIX) - 1);
                    if (NULL != userIdPos)
                    {
                        memcpy(uuid, userIdPos + sizeof(USERID_PREFIX) - 1, UUID_LENGTH * 2 + 4);
                        ret = ConvertStrToUuid(uuid, &peer->sep.userId);
                        TLS_CHECK_HANDSHAKE_FAIL(peer, ret, "Failed to convert subject alt name", 1,
                                          CA_STATUS_FAILED, MBEDTLS_SSL_ALERT_MSG_UNSUPPORTED_CERT);
                    }
                    else
                    {
                        OIC_LOG(WARNING, NET_TLS_TAG, "Subject alternative name not found");
                    }
                }
            }
            oc_mutex_unlock(g_tlsContextMutex);
            OIC_LOG_V(DEBUG, NET_TLS_TAG, "Out %s", __func__);
            return CA_STATUS_OK;
        }
    }

    if (MBEDTLS_SSL_HANDSHAKE_OVER == peer->ssl.state)
    {
        uint8_t decryptBuffer[TLS_MSG_BUF_LEN] = {0};
        do
        {
            ret = mbedtls_ssl_read(&peer->ssl, decryptBuffer, TLS_MSG_BUF_LEN);
        } while (MBEDTLS_ERR_SSL_WANT_READ == ret);
        if (MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY == ret)
        {
            OIC_LOG(INFO, NET_TLS_TAG, "Connection was closed gracefully");
            removePeerFromList(&peer->sep.endpoint);
            oc_mutex_unlock(g_tlsContextMutex);
            return CA_STATUS_OK;
        }

        if (0 >= ret)
        {
            OIC_LOG_V(ERROR, NET_TLS_TAG, "mbedtls_ssl_read returned -0x%x", -ret);
            if (g_tlsHandshakeCallback)
            {
                CAErrorInfo_t errorInfo = {.result = CA_STATUS_FAILED};
                g_tlsHandshakeCallback(&peer->sep.endpoint, &errorInfo);
            }
            removePeerFromList(&peer->sep.endpoint);
            oc_mutex_unlock(g_tlsContextMutex);
            return CA_STATUS_FAILED;
        }

        g_caTlsContext->adapterCallbacks[0].recvCallback(&peer->sep, decryptBuffer, ret);
    }

    oc_mutex_unlock(g_tlsContextMutex);
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "Out %s", __func__);
    return CA_STATUS_OK;
}

void CAsetTlsAdapterCallbacks(CAPacketReceivedCallback recvCallback,
                              CAPacketSendCallback sendCallback,
                              CATransportAdapter_t type)
{
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "In %s", __func__);
    VERIFY_NON_NULL_VOID(sendCallback, NET_TLS_TAG, "sendCallback is NULL");
    VERIFY_NON_NULL_VOID(recvCallback, NET_TLS_TAG, "recvCallback is NULL");
    oc_mutex_lock(g_tlsContextMutex);
    if (NULL == g_caTlsContext)
    {
        OIC_LOG(ERROR, NET_TLS_TAG, "Context is NULL");
        oc_mutex_unlock(g_tlsContextMutex);
        return;
    }

    if (MAX_SUPPORTED_ADAPTERS > type)
    {
        // TODO: change the zeros to better values.
        g_caTlsContext->adapterCallbacks[0].recvCallback = recvCallback;
        g_caTlsContext->adapterCallbacks[0].sendCallback = sendCallback;
    }

    oc_mutex_unlock(g_tlsContextMutex);
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "Out %s", __func__);
}

CAResult_t CAsetTlsCipherSuite(const uint32_t cipher)
{
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "In %s", __func__);
    VERIFY_NON_NULL_RET(g_getCredentialTypesCallback, NET_TLS_TAG, "Param callback is null", CA_STATUS_FAILED);
    g_getCredentialTypesCallback(g_caTlsContext->cipherFlag);
    switch(cipher)
    {
        case MBEDTLS_TLS_RSA_WITH_AES_256_CBC_SHA:
        {
            //todo check that Cred with RSA cert exists
            mbedtls_ssl_conf_ciphersuites(&g_caTlsContext->clientConf,
                                         tlsCipher[ADAPTER_TLS_RSA_WITH_AES_256_CBC_SHA]);
            mbedtls_ssl_conf_ciphersuites(&g_caTlsContext->serverConf,
                                         tlsCipher[ADAPTER_TLS_RSA_WITH_AES_256_CBC_SHA]);
            g_caTlsContext->cipher = ADAPTER_TLS_RSA_WITH_AES_256_CBC_SHA;
            break;
        }
        case MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8:
        {
            if (false == g_caTlsContext->cipherFlag[1])
            {
                OIC_LOG(ERROR, NET_TLS_TAG, "No Credential for ECC");
                return CA_STATUS_FAILED;
            }
            mbedtls_ssl_conf_ciphersuites(&g_caTlsContext->clientConf,
                                         tlsCipher[ADAPTER_TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8]);
            mbedtls_ssl_conf_ciphersuites(&g_caTlsContext->serverConf,
                                         tlsCipher[ADAPTER_TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8]);
            g_caTlsContext->cipher = ADAPTER_TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8;
            break;
        }
        case MBEDTLS_TLS_ECDH_ANON_WITH_AES_128_CBC_SHA256:
        {
            mbedtls_ssl_conf_ciphersuites(&g_caTlsContext->clientConf,
                                         tlsCipher[ADAPTER_TLS_ECDH_ANON_WITH_AES_128_CBC_SHA_256]);
            mbedtls_ssl_conf_ciphersuites(&g_caTlsContext->serverConf,
                                         tlsCipher[ADAPTER_TLS_ECDH_ANON_WITH_AES_128_CBC_SHA_256]);
            g_caTlsContext->cipher = ADAPTER_TLS_ECDH_ANON_WITH_AES_128_CBC_SHA_256;
            break;
        }
        case MBEDTLS_TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256:
        {
#if 0 // PIN OTM
            if (false == g_caTlsContext->cipherFlag[0])
            {
                OIC_LOG(ERROR, NET_TLS_TAG, "No Credential for PSK");
                return CA_STATUS_FAILED;
            }
#endif
            mbedtls_ssl_conf_ciphersuites(&g_caTlsContext->clientConf,
                                          tlsCipher[ADAPTER_TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256]);
            mbedtls_ssl_conf_ciphersuites(&g_caTlsContext->serverConf,
                                          tlsCipher[ADAPTER_TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256]);
            g_caTlsContext->cipher = ADAPTER_TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256;
            break;
        }
        default:
        {
            OIC_LOG(ERROR, NET_TLS_TAG, "Unknown cipher");
            return CA_STATUS_FAILED;
        }
    }
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "Selected cipher: 0x%x", cipher);
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "Out %s", __func__);
    return CA_STATUS_OK;
}

CAResult_t CAinitiateTlsHandshake(const CAEndpoint_t *endpoint)
{
    CAResult_t res = CA_STATUS_OK;
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "In %s", __func__);
    VERIFY_NON_NULL_RET(endpoint, NET_TLS_TAG, "Param endpoint is NULL" , CA_STATUS_INVALID_PARAM);
    oc_mutex_lock(g_tlsContextMutex);
    if (NULL == initiateTlsHandshake(endpoint))
    {
        OIC_LOG(ERROR, NET_TLS_TAG, "TLS handshake failed");
        res = CA_STATUS_FAILED;
    }
    oc_mutex_unlock(g_tlsContextMutex);
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "Out %s", __func__);
    return res;
}

CAResult_t CAtlsGenerateOwnerPSK(const CAEndpoint_t *endpoint,
                                 uint8_t* ownerPSK, const size_t ownerPSKSize,
                                 const uint8_t* deviceID, const size_t deviceIDLen)
{
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "In %s", __func__);

    VERIFY_NON_NULL_RET(ownerPSK, NET_TLS_TAG, "ownerPSK is NULL", CA_STATUS_INVALID_PARAM);
    VERIFY_NON_NULL_RET(deviceID, NET_TLS_TAG, "rsrcID is NULL", CA_STATUS_INVALID_PARAM);

    oc_mutex_lock(g_tlsContextMutex);
    if (NULL == g_caTlsContext)
    {
        OIC_LOG(ERROR, NET_TLS_TAG, "Context is NULL");
        oc_mutex_unlock(g_tlsContextMutex);
        OIC_LOG_V(DEBUG, NET_TLS_TAG, "Out %s", __func__);
        return CA_STATUS_FAILED;
    }
    TlsEndPoint_t * tep = getTlsPeer(endpoint);
    if (NULL == tep)
    {
        OIC_LOG(ERROR, NET_TLS_TAG, "Session does not exist");
        oc_mutex_unlock(g_tlsContextMutex);
        return CA_STATUS_FAILED;
    }

    if (0 != mbedtls_pkcs12_derivation(ownerPSK, ownerPSKSize,
                                       tep->master, sizeof(tep->master),
                                       deviceID, deviceIDLen,
                                       MBEDTLS_MD_SHA1, MBEDTLS_PKCS12_DERIVE_KEY, 1))
    {
        OIC_LOG(ERROR, NET_TLS_TAG, "Failed to generate key");
        oc_mutex_unlock(g_tlsContextMutex);
        OIC_LOG_V(DEBUG, NET_TLS_TAG, "Out %s", __func__);
        return CA_STATUS_FAILED;
    }
    OIC_LOG_V(DEBUG, NET_TLS_TAG, "PSK: ");
    OIC_LOG_BUFFER(DEBUG, NET_TLS_TAG, tep->master, sizeof(tep->master));
    oc_mutex_unlock(g_tlsContextMutex);

    OIC_LOG_V(DEBUG, NET_TLS_TAG, "Out %s", __func__);
    return CA_STATUS_OK;
}
