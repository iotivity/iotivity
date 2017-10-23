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

#include "cawsadapter_ssl.h"
#include "cacommonutil.h"
#include "cawsutil.h"
#include "oic_malloc.h"
#include "uarraylist.h"
#include "octhread.h"
#include "memory.h"
#include "experimental/logger.h"
#include "string.h"

#define TAG "OIC_CA_WS_ADAP_SSL"

#define UUID_LENGTH (128/8)
#define PSK_LENGTH (256/8)

static CAgetPkixInfoHandler g_getPkixInfoCallback = NULL;
static CAgetCredentialTypesHandler g_getCredentialTypesCallback = NULL;
static CAgetPskCredentialsHandler g_getCredentialsCallback = NULL;

static PkiInfo_t g_pkiInfo = {{NULL, 0}, {NULL, 0}, {NULL, 0}, {NULL, 0}};
bool g_cipherFlag[2];

static oc_mutex g_sslCtxMutex;
static SSL_CTX *g_clientSslCtx = NULL;
static SSL_CTX *g_serverSslCtx = NULL;

// Map from mbedTLS cipher to OpenSSL ciphers
typedef struct
{
    int mbedTLSCipher;
    char *cipher;
    int cipherLen;
} CiphersMapItem;

static CiphersMapItem g_cipherSuites[] =
{
    {0x3D, "AES256-SHA256", 13},
    {0x9C, "AES128-GCM-SHA256", 17},
    {0xC02B, "ECDHE-ECDSA-AES128-GCM-SHA256", 29},
    {0xC0AE, "ECDHE-ECDSA-AES128-CCM8", 23},
    {0xC0AC, "ECDHE-ECDSA-AES128-CCM", 22},
    {0xC023, "ECDHE-ECDSA-AES128-SHA256", 25},
    {0xC024, "ECDHE-ECDSA-AES256-SHA384", 25},
    {0xC02C, "ECDHE-ECDSA-AES256-GCM-SHA384", 29},
    //{0xC037, "ECDHE-PSK-AES128-CBC-SHA256", 27}, //Not defined in OpenSSL ?
    {0xC037, "PSK-AES128-CBC-SHA", 27},
    {0xC027, "ECDHE-RSA-AES128-SHA256", 23}
};

static int g_cipherSuitesSize = sizeof(g_cipherSuites) / sizeof(CiphersMapItem);
static int g_selectedCipherIndex = sizeof(g_cipherSuites) / sizeof(CiphersMapItem);

typedef struct
{
    SSL *ssl;
    uint16_t port;
    char addr[MAX_ADDR_STR_SIZE_CA];
    CARemoteId_t identity;
} SSLSession_t;

static u_arraylist_t *g_sessionList;

static SSLSession_t *getSession(char *addr, int port)
{
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);

    size_t listIndex = 0;
    size_t listLength = 0;
    SSLSession_t *session = NULL;

    listLength = u_arraylist_length(g_sessionList);
    for (listIndex = 0; listIndex < listLength; listIndex++)
    {
        session = (SSLSession_t *) u_arraylist_get(g_sessionList, listIndex);
        if (0 == strncmp(addr, session->addr, MAX_ADDR_STR_SIZE_CA)
            && (port == session->port))
        {
            return session;
        }
    }

    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
    return NULL;
}

static void deleteAllSessions()
{
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);

    size_t listIndex = 0;
    size_t listLength = 0;
    SSLSession_t *session = NULL;

    listLength = u_arraylist_length(g_sessionList);
    for (listIndex = 0; listIndex < listLength; listIndex++)
    {
        session = (SSLSession_t *) u_arraylist_get(g_sessionList, listIndex);
        OICFree(session);
    }

    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
}

/* Create cipher string for the given ciphers list */
static char *formCipherString(int *selectCiphers, int size)
{
    char *ciphersStr = NULL;
    int ciphersLen = 0;
    int i = 0;
    int pos = 0;

    if (size <= 0)
    {
        return NULL;
    }

    // Calculate size of cipherstring
    for (i = 0; i < size; i++)
    {
        ciphersLen += g_cipherSuites[selectCiphers[i]].cipherLen;
        ciphersLen += 1; //for ciphers separator ":"
    }

    // Construct cipher string
    ciphersStr = (char *) OICCalloc(sizeof(char), ciphersLen);
    if (!ciphersStr)
    {
        OIC_LOG(ERROR, TAG, "Memory allocation failed!");
        return NULL;
    }

    for (i = 0; i < size; i++)
    {
        int cipherIndex = selectCiphers[i];
        strncpy(ciphersStr + pos, g_cipherSuites[cipherIndex].cipher,
                g_cipherSuites[cipherIndex].cipherLen);
        pos += g_cipherSuites[cipherIndex].cipherLen;

        strncpy(ciphersStr + pos, ":", 1);
        pos += 1;
    }

    ciphersStr[pos - 1] = '\0';
    return ciphersStr;
}

/* Returns ordinal of given cipher in preferred ciphers list */
static int getCipherIndex(int cipher)
{
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);

    int i = 0;
    for (i = 0; i < g_cipherSuitesSize; i++)
    {
        if (g_cipherSuites[i].mbedTLSCipher == cipher)
        {
            break;
        }
    }

    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
    return i;
}

/* Convert PEM formatted binary data to X509 structure */
static X509 *pemToX509(uint8_t *buf, size_t bufLen)
{
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);

    BIO *in = NULL;
    X509 *x = NULL;

    in = BIO_new_mem_buf(buf, bufLen);
    VERIFY_NON_NULL_RET(in, TAG, "Failed to create buffer BIO!", NULL);

    x = PEM_read_bio_X509(in, NULL, NULL, NULL);
    if (!x)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to read cert from buffer!");
        goto exit;
    }

exit:
    BIO_free(in);

    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
    return x;
}

/* Convert DER formatted binary data to X509 structure */
static X509 *derToX509(uint8_t *buf, size_t bufLen)
{
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);

    BIO *in = NULL;
    X509 *x = NULL;

    in = BIO_new_mem_buf(buf, bufLen);
    VERIFY_NON_NULL_RET(in, TAG, "Failed to create buffer BIO!", NULL);

    x = d2i_X509_bio(in, NULL);
    if (!x)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to read cert from buffer!");
        goto exit;
    }

exit:
    BIO_free(in);

    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
    return x;
}

/* Get PEM/DER certificate length */
static size_t asn1GetLength(uint8_t *start, uint8_t *end)
{
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);
    int len = 0;

    BIO *in = BIO_new_mem_buf(start, end - start);
    VERIFY_NON_NULL_RET(in, TAG, "Failed to create buffer BIO!", 0);

    X509 *x = d2i_X509_bio(in, NULL);
    if (!x)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to read DER format data from buffer!");
        goto exit;
    }

    unsigned char *der = NULL;
    len = ASN1_item_i2d((void *) x, &der, ASN1_ITEM_rptr(X509));
    if (!der)
    {
        goto exit;
    }

    OPENSSL_free(der);

exit:
    X509_free(x);
    BIO_free(in);

    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
    return len;
}

/*
* Parse CA cert chain from buffer.
* NOTE: buf might contain certificates in PEM as well as DER format.
*/
static size_t parseCertChain(X509_STORE *certStore, uint8_t *buf, size_t bufLen, int *errNum)
{
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);

    *errNum = 0;
    VERIFY_NON_NULL_RET(certStore, TAG, "CertStore is NULL!", 0);

    if (!buf || 1 > bufLen)
    {
        OIC_LOG(ERROR, TAG, "Cert Buffer is empty!");
        return 0;
    }

    char pemCertHeader[] =
    {
        0x2d, 0x2d, 0x2d, 0x2d, 0x2d, 0x42, 0x45, 0x47, 0x49, 0x4e, 0x20, 0x43, 0x45, 0x52,
        0x54, 0x49, 0x46, 0x49, 0x43, 0x41, 0x54, 0x45, 0x2d, 0x2d, 0x2d, 0x2d, 0x2d
    };
    char pemCertFooter[] =
    {
        0x2d, 0x2d, 0x2d, 0x2d, 0x2d, 0x45, 0x4e, 0x44, 0x20, 0x43, 0x45, 0x52, 0x54, 0x49,
        0x46, 0x49, 0x43, 0x41, 0x54, 0x45, 0x2d, 0x2d, 0x2d, 0x2d, 0x2d
    };
    size_t pemCertHeaderLen = sizeof(pemCertHeader);
    size_t pemCertFooterLen = sizeof(pemCertFooter);

    size_t len = 0;
    size_t count = 0;
    size_t pos = 0;

    *errNum = 0;
    while (pos < bufLen)
    {
        if (buf[pos] == 0x30 && buf[pos + 1] == 0x82)
        {
            len = asn1GetLength(buf + pos, buf + bufLen);
            if (!len)
            {
                OIC_LOG(ERROR, TAG, "Bad DER data!");
                goto exit;
            }

            if (pos + len <= bufLen)
            {
                // Convert DER to X509 certificate
                X509 *x = derToX509(buf + pos, len);
                if (x)
                {
                    // Add certificate to cert store
                    int ret = X509_STORE_add_cert(certStore, x);
                    if (ret)
                    {
                        count++;
                    }
                    else
                    {
                        OIC_LOG_V(ERROR, TAG, "Failed to add cert to store!");
                        X509_free(x);
                        (*errNum)++;
                    }
                }
                else
                {
                    (*errNum)++;
                }
            }
            pos += len;
        }
        else if ((buf + pos + pemCertHeaderLen < buf + bufLen) &&
                 (0 == memcmp(buf + pos, pemCertHeader, pemCertHeaderLen)))
        {
            void *endPos = NULL;
            endPos = memmem(&(buf[pos]), bufLen - pos, pemCertFooter, pemCertFooterLen);
            if (NULL == endPos)
            {
                OIC_LOG(ERROR, TAG, "end of PEM certificate not found.");
                goto exit;
            }

            len = (char *)endPos - ((char *)buf + pos) + pemCertFooterLen;
            if (pos + len + 1 <= bufLen)
            {
                char con = buf[pos + len];
                buf[pos + len] = 0x00;

                // Convert PEM to X509 certificate
                X509 *x = pemToX509(buf + pos, len + 1);
                if (x)
                {
                    // Add certificate to cert store
                    int ret = X509_STORE_add_cert(certStore, x);
                    if (ret)
                    {
                        count++;
                    }
                    else
                    {
                        OIC_LOG_V(ERROR, TAG, "Failed to add cert to store!");
                        X509_free(x);
                        (*errNum)++;
                    }
                }
                else
                {
                    (*errNum)++;
                }

                buf[pos + len] = con;
            }
            else
            {
                unsigned char *lastCert = (unsigned char *)OICMalloc((len + 1) * sizeof(unsigned char));
                if (NULL == lastCert)
                {
                    OIC_LOG_V(ERROR, TAG, "Out of memory!");
                    goto exit;
                }

                memcpy(lastCert, buf + pos, len);
                lastCert[len] = 0x00;

                // Convert PEM to X509 certificate
                X509 *x = pemToX509(lastCert, len + 1);
                if (x)
                {
                    // Add certificate to cert store
                    int ret = X509_STORE_add_cert(certStore, x);
                    if (ret)
                    {
                        count++;
                    }
                    else
                    {
                        OIC_LOG_V(ERROR, TAG, "Failed to add cert to store!");
                        X509_free(x);
                        (*errNum)++;
                    }
                }
                else
                {
                    (*errNum)++;
                }

                OICFree(lastCert);
            }
            pos += len;
        }
        else
        {
            pos++;
        }
    }

    OIC_LOG_V(DEBUG, TAG, "%s successfully parsed %d certificates", __func__, count);

exit:
    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
    return count;
}

/*
* Parse CRL from buffer.
* NOTE: CRL content will be given in DER format only.
*/
static size_t parseCRL(X509_STORE *certStore, uint8_t *buf, size_t bufLen, int *errNum)
{
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);

    *errNum = 0;
    VERIFY_NON_NULL_RET(certStore, TAG, "CertStore is NULL!", 0);

    if (!buf || 1 > bufLen)
    {
        OIC_LOG(ERROR, TAG, "CRL Buffer is NULL!");
        return 0;
    }

    size_t count = 0;
    BIO *in = BIO_new_mem_buf(buf, bufLen);
    for (;;)
    {
        // Read certificate from buffer BIO
        X509_CRL *x = d2i_X509_CRL_bio(in, NULL);
        if (!x)
        {
            OIC_LOG_V(ERROR, TAG, "Failed to read CRL cert from buffer! [Count: %d]", count);
            break;
        }

        // Add certificate to cert crl store
        int ret = X509_STORE_add_crl(certStore, x);
        if (!ret)
        {
            OIC_LOG_V(ERROR, TAG, "Failed to add CRL cert to store! [Count: %d]", count);
            X509_CRL_free(x);
            break;
        }

        count++;
        X509_CRL_free(x);
    }

    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
    return count;
}

/*
* Parse private key from buffer.
* NOTE: Private key data might be in PEM or DER format.
*/
static int parsePrivateKey(SSL_CTX *ctx, uint8_t *buf, size_t bufLen)
{
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);

    BIO *in = BIO_new_mem_buf(buf, bufLen);
    if (!in)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to create BIO!");
        return 0;
    }

    EVP_PKEY *key = PEM_read_bio_PrivateKey(in, NULL, NULL, NULL);
    if (!key)
    {
        // Check for DER format
        OIC_LOG_V(DEBUG, TAG, "Private key is not in PEM format!. Checking if it is DER");
        BIO_free(in);

        in = BIO_new_mem_buf(buf, bufLen);
        key = d2i_PrivateKey_bio(in, NULL);
    }

    if (!key)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to read private key from buffer!");
        BIO_free(in);
        return 0;
    }

    int ret = SSL_CTX_use_PrivateKey(ctx, key);
    if (1 != ret)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to set private key to context!");
        EVP_PKEY_free(key);
        return 0;
    }

    EVP_PKEY_free(key);
    BIO_free(in);

    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
    return 1;
}

/* set Cert, Private key, CA chain and CRL to ssl context. */
static CAResult_t setCertificates(SSL_CTX *ctx)
{
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);

    VERIFY_NON_NULL(ctx, TAG, "SSL Context is NULL");
    VERIFY_NON_NULL(g_getPkixInfoCallback, TAG, "PkixInfoHandler is NULL");

    // load pk key, cert, trust chain and crl
    g_getPkixInfoCallback(&g_pkiInfo);

    // Get X509_STORE from context
    X509_STORE *certStore = SSL_CTX_get_cert_store(ctx);
    if (!certStore)
    {
        OIC_LOG(ERROR, TAG, "Failed to fetch cert store from SSL context!");
        return CA_STATUS_FAILED;
    }

    int count = 0;
    int errNum = 0;

    // Store own certificate chain
    if (g_pkiInfo.crt.data != NULL)
    {
        count = parseCertChain(certStore, g_pkiInfo.crt.data, g_pkiInfo.crt.len, &errNum);
        OIC_LOG_V(DEBUG, TAG, "Own cert chain parse result - [%d/%d]", count, errNum);
        if (0 == count || 0 < errNum)
        {
            goto required;
        }
    }

    // Store private key
    if (g_pkiInfo.key.data != NULL)
    {
        if (parsePrivateKey(ctx, g_pkiInfo.key.data, g_pkiInfo.key.len))
        {
            OIC_LOG_V(DEBUG, TAG, "Private key stored!");
        }
        else
        {
            OIC_LOG_V(ERROR, TAG, "Failed ot store private key!");
        }
    }

required:
    // Store CA chain certificate
    if (g_pkiInfo.ca.data != NULL)
    {
        count = parseCertChain(certStore, g_pkiInfo.ca.data, g_pkiInfo.ca.len, &errNum);
        OIC_LOG_V(ERROR, TAG, "CA cert chain parse result - [%d/%d]", count, errNum);
    }

    // Store CRL
    if (g_pkiInfo.crl.data != NULL)
    {
        count = parseCRL(certStore, g_pkiInfo.crl.data, g_pkiInfo.crl.len, &errNum);
        OIC_LOG_V(ERROR, TAG, "CRL parse result - [%d/%d]", count, errNum);
    }

    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
    return CA_STATUS_OK;
}

/* Sets pre-shared key for the SSL connection based on hint. */
static unsigned int pskClientCallback(SSL *ssl, const char *hint,
                                      char *identity,
                                      unsigned int maxIdentityLen,
                                      unsigned char *psk,
                                      unsigned int maxPskLen)
{
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);

    VERIFY_NON_NULL_RET(ssl, TAG, "ssl pointer is NULL", 0);
    VERIFY_NON_NULL_RET(hint, TAG, "Hint is NULL", 0);

    OIC_LOG_BUFFER(DEBUG, TAG, (uint8_t*)hint, strlen(hint));

    // Set client's idetity
    int len;
    uint8_t idBuf[UUID_LENGTH + 1] = {0}; // Extra byte for null character
    len = g_getCredentialsCallback(CA_DTLS_PSK_IDENTITY, NULL, 0, idBuf, UUID_LENGTH);
    if (len < 0)
    {
        OIC_LOG(ERROR, TAG, "Failed to retrieve Identity!");
        return 0;
    }

    len = BIO_snprintf(identity, maxIdentityLen, "%s", idBuf);
    if (len < 0 || (unsigned int)len > maxIdentityLen)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to fill client's identity!");
        return 0;
    }

    OIC_LOG_V(DEBUG, TAG, "Identity:");
    OIC_LOG_BUFFER(DEBUG, TAG, (uint8_t*)identity, len);

    // Set client's pre-shared key
    uint8_t keyBuf[PSK_LENGTH] = {0};
    len = g_getCredentialsCallback(CA_DTLS_PSK_KEY, (uint8_t*)hint, strlen(hint), keyBuf, PSK_LENGTH);
    if (len < 0)
    {
        OIC_LOG(ERROR, TAG, "Failed to retrieve pre-shared key!");
        return 0;
    }

    OIC_LOG_V(DEBUG, TAG, "PSK:");
    OIC_LOG_BUFFER(DEBUG, TAG, (uint8_t*)keyBuf, len);

    if ((unsigned int)len > maxPskLen)
    {
        OIC_LOG_V(ERROR, TAG, "PSK key buffer is small (%d) for key (%d)!", maxPskLen, len);
        return 0;
    }

    memcpy(psk, keyBuf, len);

    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
    return len;
}

static int initPskIdentity(SSL_CTX *ctx)
{
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);

    SSL_CTX_set_psk_client_callback(ctx, pskClientCallback);

    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
    return 0;
}

/* Sets device UUID as server's hint for PSK cipher */
static void setServerPSKHint()
{
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);

    int ret;
    uint8_t idBuf[UUID_LENGTH + 1] = {0}; // Extra byte for null character
    ret = g_getCredentialsCallback(CA_DTLS_PSK_IDENTITY, NULL, 0, idBuf, UUID_LENGTH);
    if (ret > 0)
    {
        OIC_LOG(DEBUG, TAG, "Server Hint:");
        OIC_LOG_BUFFER(DEBUG, TAG, idBuf, ret);
        SSL_CTX_use_psk_identity_hint(g_serverSslCtx, (char*)idBuf);
    }

    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
}

/* Sets pre-shared key for the incoming SSL connection based on its identity. */
static unsigned int pskServerCallback(SSL *ssl, const char *identity,
                                      unsigned char *psk,
                                      unsigned int maxPskLen)
{
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);

    VERIFY_NON_NULL_RET(ssl, TAG, "ssl pointer is NULL", 0);
    VERIFY_NON_NULL_RET(identity, TAG, "identity is NULL", 0);

    OIC_LOG(DEBUG, TAG, "identity:");
    OIC_LOG_BUFFER(DEBUG, TAG, (uint8_t*)identity, strlen(identity));

    int pskLen;
    uint8_t keyBuf[PSK_LENGTH] = {0};
    pskLen = g_getCredentialsCallback(CA_DTLS_PSK_KEY, (uint8_t*)identity, strlen(identity), keyBuf, PSK_LENGTH);
    if (pskLen < 0)
    {
        OIC_LOG(ERROR, TAG, "PSK key not found!");
        return 0;
    }

    OIC_LOG(DEBUG, TAG, "PSK:");
    OIC_LOG_BUFFER(DEBUG, TAG, (uint8_t*)keyBuf, pskLen);

    if (pskLen > (int)maxPskLen)
    {
        OIC_LOG_V(ERROR, TAG, "PSK key buffer is small (%d) for key (%d)!", maxPskLen, pskLen);
        return 0;
    }

    memcpy(psk, keyBuf, pskLen);

    // Update identity of client
    uint16_t port;
    char addr[MAX_ADDR_STR_SIZE_CA];
    CAWSGetPeerAddress(SSL_get_fd(ssl), addr, &port, NULL);

    oc_mutex_lock(g_sslCtxMutex);
    SSLSession_t *session = getSession(addr, port);
    if (!session)
    {
        session = (SSLSession_t *) OICCalloc(1, sizeof (SSLSession_t));
        if (NULL == session)
        {
            OIC_LOG(ERROR, TAG, "Memory allocation failed!");
            oc_mutex_unlock(g_sslCtxMutex);
            return 0;
        }

        memcpy(session->addr, addr, MAX_ADDR_STR_SIZE_CA);
        session->port = port;
        session->ssl = ssl;

        u_arraylist_add(g_sessionList, (void *) session);
    }
    oc_mutex_unlock(g_sslCtxMutex);

    memcpy(session->identity.id, identity, strlen(identity));
    session->identity.id_length = strlen(identity);

    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
    return pskLen;
}

CAResult_t CAWSInitSSLAdapter()
{
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);

    if (NULL == g_sslCtxMutex)
    {
        g_sslCtxMutex = oc_mutex_new();
        if (NULL == g_sslCtxMutex)
        {
            OIC_LOG(ERROR, TAG, "Failed to create SSL context mutex!");
            return CA_STATUS_FAILED;
        }
    }
    else
    {
        OIC_LOG(INFO, TAG, "Initialization is already done!");
        return CA_STATUS_OK;
    }

    oc_mutex_lock(g_sslCtxMutex);
    if (!g_sessionList)
    {
        g_sessionList = u_arraylist_create();
        if (NULL == g_sessionList)
        {
            OIC_LOG(ERROR, TAG, "Failed to create session list!");
            goto error;
        }
    }

    g_clientSslCtx = NULL;
    g_serverSslCtx = NULL;

    oc_mutex_unlock(g_sslCtxMutex);
    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
    return CA_STATUS_OK;

error:
    u_arraylist_free(&g_sessionList);

    oc_mutex_unlock(g_sslCtxMutex);
    oc_mutex_free(g_sslCtxMutex);
    g_sslCtxMutex = NULL;
    return CA_STATUS_FAILED;
}

void CAWSDeInitSSLAdapter()
{
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);

    VERIFY_NON_NULL_VOID(g_sslCtxMutex, TAG, "SSL context mutex is NULL!");

    oc_mutex_lock(g_sslCtxMutex);
    if (g_sessionList)
    {
        deleteAllSessions();
        u_arraylist_free(&g_sessionList);
    }

    oc_mutex_unlock(g_sslCtxMutex);
    oc_mutex_free(g_sslCtxMutex);
    g_sslCtxMutex = NULL;

    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
}

void CAWSSetPkixInfoCallback(CAgetPkixInfoHandler infoCallback)
{
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);
    g_getPkixInfoCallback = infoCallback;
    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
}

void CAWSSetCredentialTypesCallback(CAgetCredentialTypesHandler credTypesCallback)
{
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);
    g_getCredentialTypesCallback = credTypesCallback;
    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
}

void CAWSSetPskCredentialsCallback(CAgetPskCredentialsHandler credCallback)
{
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);

    g_getCredentialsCallback = credCallback;

    oc_mutex_lock(g_sslCtxMutex);
    if (g_serverSslCtx && g_getCredentialsCallback)
    {
        setServerPSKHint();
    }

    oc_mutex_unlock(g_sslCtxMutex);
    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
}

/* Client methods. */
void CAWSInitSSLClient(SSL_CTX *ctx)
{
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);

    VERIFY_NON_NULL_VOID(ctx, TAG, "context is NULL!");

    oc_mutex_lock(g_sslCtxMutex);
    g_clientSslCtx = ctx;
    oc_mutex_unlock(g_sslCtxMutex);

    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
}

CAResult_t CAWSSetupCipher(const char *deviceId)
{
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);

    VERIFY_NON_NULL(g_getCredentialTypesCallback, TAG, "CredentialTypes callback is NULL");

    oc_mutex_lock(g_sslCtxMutex);
    if (NULL == g_clientSslCtx)
    {
        OIC_LOG(ERROR, TAG, "Client context is NULL!");
        oc_mutex_unlock(g_sslCtxMutex);
        return CA_STATUS_FAILED;
    }

    //Resetting cipherFlag
    g_cipherFlag[0] = false;
    g_cipherFlag[1] = false;

    g_getCredentialTypesCallback(g_cipherFlag, deviceId);

    // Retrieve the PSK credential from SRM or use PIN based generation
    if ((8 == g_selectedCipherIndex || true == g_cipherFlag[0])
        && 0 != initPskIdentity(g_clientSslCtx))
    {
        OIC_LOG(ERROR, TAG, "PSK identity initialization failed!");
        /* Don't return error, the connection may work with another cred type */
    }

    if (true == g_cipherFlag[1])
    {
        if (CA_STATUS_OK != setCertificates(g_clientSslCtx))
        {
            OIC_LOG(ERROR, TAG, "Failed to init X.509");
            /* Don't return error, the connection may work with another cred type */
        }
    }

    //Update the cipher suite based on credtype
    int cipherList[g_cipherSuitesSize];
    int index = 0;
    if (g_cipherSuitesSize != g_selectedCipherIndex)
    {
        cipherList[index++] = g_selectedCipherIndex;
    }

    if (true == g_cipherFlag[0] && 8 != g_selectedCipherIndex)
    {
        cipherList[index++] = 8;
    }

    if (true == g_cipherFlag[1])
    {
        int i = 0;
        for (i = 0; i < g_cipherSuitesSize; i++)
        {
            if (i != g_selectedCipherIndex
                && i != 8)
            {
                cipherList[index++] = i;
            }
        }
    }

    char *cipherStr = formCipherString(cipherList, index);
    if (cipherStr)
    {
        OIC_LOG(INFO, TAG, "Cipher String is: ");
        OIC_LOG_V(INFO, TAG, "%s", cipherStr);

        if (!SSL_CTX_set_cipher_list(g_clientSslCtx, cipherStr))
        {
            OIC_LOG(ERROR, TAG, "Failed to set ciphers list!");
            /* Don't return error, the connection may work with default ciphers list */
        }
    }

    oc_mutex_unlock(g_sslCtxMutex);

    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
    return CA_STATUS_OK;
}

CAResult_t CAWSSetCipherSuite(const uint16_t cipher)
{
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);

    int cipherIndex = getCipherIndex(cipher);
    if (g_cipherSuitesSize == cipherIndex)
    {
        OIC_LOG(ERROR, TAG, "Unsupported cipher!");
        return CA_STATUS_FAILED;
    }

    int cipherList[1];
    cipherList[0] = cipherIndex;

    char *cipherStr = formCipherString(cipherList, 1);
    if (!cipherStr)
    {
        OIC_LOG(ERROR, TAG, "Failed to form cipher string");
        return CA_STATUS_FAILED;
    }

    OIC_LOG(INFO, TAG, "Cipher String is: ");
    OIC_LOG_V(INFO, TAG, "%s", cipherStr);

    oc_mutex_lock(g_sslCtxMutex);
    if (g_clientSslCtx && !SSL_CTX_set_cipher_list(g_clientSslCtx, cipherStr))
    {
        OIC_LOG(ERROR, TAG, "Failed to set ciphers list to server ctx!");
        oc_mutex_unlock(g_sslCtxMutex);
        return CA_STATUS_FAILED;
    }

    if (g_serverSslCtx && !SSL_CTX_set_cipher_list(g_serverSslCtx, cipherStr))
    {
        OIC_LOG(ERROR, TAG, "Failed to set ciphers list to server ctx!");
        oc_mutex_unlock(g_sslCtxMutex);
        return CA_STATUS_FAILED;
    }

    g_selectedCipherIndex = cipherIndex;
    oc_mutex_unlock(g_sslCtxMutex);

    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
    return CA_STATUS_OK;
}

/* Server methods. */
void CAWSInitSSLServer(SSL_CTX *ctx)
{
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);

    VERIFY_NON_NULL_VOID(ctx, TAG, "context is NULL!");

    oc_mutex_lock(g_sslCtxMutex);
    g_serverSslCtx = ctx;
    SSL_CTX_set_psk_server_callback(g_serverSslCtx, pskServerCallback);
    if (g_getCredentialsCallback)
    {
        setServerPSKHint();
    }

    oc_mutex_unlock(g_sslCtxMutex);

    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
}

CARemoteId_t CAWSGetPeerIdentity(CAEndpoint_t *endpoint)
{
    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);

    CARemoteId_t identity = {0};
    SSLSession_t *session = NULL;

    oc_mutex_lock(g_sslCtxMutex);
    session = getSession(endpoint->addr, endpoint->port);
    if (!session)
    {
        OIC_LOG_V(ERROR, TAG, "No SSl session!. Something is wrong!");
        oc_mutex_unlock(g_sslCtxMutex);
        return identity;
    }

    oc_mutex_unlock(g_sslCtxMutex);
    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
    return session->identity;
}

