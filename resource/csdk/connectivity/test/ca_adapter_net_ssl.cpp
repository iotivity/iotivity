/* *****************************************************************
 *
 * Copyright 2019 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * *****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <gtest/gtest.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "srmutility.h"
#include "../src/adapter_util/ca_adapter_net_ssl.c"
#include "mbedtls/pem.h"
#include "mbedtls_messages.h"

#undef TAG
#ifdef __cplusplus
}
#endif

#undef TAG
#define TAG "OCAA_UNITTEST"

using namespace std;

#define TESTDIR "resource/csdk/connectivity/test"

static ByteArray_t *ca  = NULL;
static ByteArray_t *crl = NULL;
static ByteArray_t *key = NULL;
static ByteArray_t *cert= NULL;

#if defined(__PEM2DER__)
#define PEM_BEGIN_CRT           "-----BEGIN CERTIFICATE-----"
#define PEM_END_CRT             "-----END CERTIFICATE-----"
static void pem2der(const unsigned char *crt, const char *begin, const char *end,
                    unsigned char **buf, size_t *len)
{
    size_t use_len;
    mbedtls_pem_context pem;
    mbedtls_pem_init(&pem);
    LOG_MBEDTLS_ERROR(mbedtls_pem_read_buffer(&pem, begin, end, crt, NULL, 0, &use_len));

    *buf = (unsigned char *)OICCalloc(1, pem.buflen);
    *len = pem.buflen;
    memcpy(*buf, pem.buf, pem.buflen);

    mbedtls_pem_free( &pem );
}

static void getCertDer(const unsigned char *crt, unsigned char **buf, size_t *len)
{
    pem2der(crt, PEM_BEGIN_CRT, PEM_END_CRT, buf, len);
}

static void getKeyDer(const unsigned char *crt, unsigned char **buf, size_t *len)
{
    pem2der(crt, "-----BEGIN RSA PRIVATE KEY-----", "-----END RSA PRIVATE KEY-----", buf, len);
}
#endif

static ByteArray_t *readFile(const char *fname)
{
    ByteArray_t *ret = NULL;
    size_t rb = 0;
    int fd = open(fname, O_RDONLY);
    if (0 > fd)
    {
        OIC_LOG_V(ERROR, TAG, "%s: open file %s", __func__, fname);
        return NULL;
    }

    struct stat st;
    memset(&st, 0, sizeof(struct stat));
    if (0 != fstat(fd, &st))
    {
        OIC_LOG_V(ERROR, TAG, "%s: fstat: (%d) %s", __func__, errno, strerror(errno));
        goto exit;
    }

    ret = (ByteArray_t *)OICCalloc(1, sizeof(ByteArray_t));
    if (!ret)
    {
        OIC_LOG_V(ERROR, TAG, "%s: alloc", __func__);
        goto exit;
    }
    ret->data = (unsigned char *)OICCalloc(st.st_size, sizeof(unsigned char));
    if (!ret->data)
    {
        OIC_LOG_V(ERROR, TAG, "%s: alloc", __func__);
        goto exit;
    }
    ret->len = st.st_size;
    rb = read(fd, ret->data,
#if !defined(__unix__)
              (unsigned)
#endif
              ret->len);
    if ((size_t)ret->len != rb)
    {
        OIC_LOG_V(ERROR, TAG, "%s: read %s (%lu)", __func__, fname, rb);
    }
exit:
    close(fd);
    return ret;
}

static oc_thread pInternalThread;
static bool pInternalWorking;

static void *__process__(void *p)
{
    OIC_LOG_V(DEBUG, TAG, "%s: IN", __func__);
    OC_UNUSED(p);
    struct timespec timeout = {0, 100000000L};
    pInternalWorking = true;
    while (pInternalWorking)
    {
        if (OCProcess() != OC_STACK_OK)
        {
            OIC_LOG(FATAL, TAG, "OCStack process error");
            return NULL;
        }
        nanosleep(&timeout, NULL);
    }
    OIC_LOG_V(DEBUG, TAG, "%s: OUT", __func__);
    return NULL;
}

static void PkixInfoHandler(PkiInfo_t *inf)
{
    OIC_LOG_V(DEBUG, TAG, "%s: IN", __func__);

    inf->crt.next = NULL;
    inf->crt.cert = (ByteArray_t *)OICCalloc(1, sizeof(ByteArray_t));
    inf->crt.cert->data = (unsigned char *)OICCalloc(cert->len, sizeof(unsigned char));
    inf->crt.cert->len = cert->len;
    memcpy(inf->crt.cert->data, cert->data, cert->len);

    inf->ca.next = NULL;
    inf->ca.cert = (ByteArray_t *)OICCalloc(1, sizeof(ByteArray_t));
    inf->ca.cert->data = (unsigned char *)OICCalloc(ca->len, sizeof(unsigned char));
    inf->ca.cert->len = ca->len;
    memcpy(inf->ca.cert->data, ca->data, ca->len);

    inf->crl.len = crl->len;
    inf->crl.data = (unsigned char *)OICCalloc(crl->len, sizeof(unsigned char));
    memcpy(inf->crl.data, crl->data, crl->len);

    inf->key.len = key->len;
    inf->key.data = (unsigned char *)OICCalloc(key->len, sizeof(unsigned char));
    memcpy(inf->key.data, key->data, key->len);

    OIC_LOG_V(DEBUG, TAG, "%s: OUT", __func__);
}

class OCAA : public ::testing::Test
{
    public:
        static void SetUpTestCase()
        {
            OIC_LOG_V(DEBUG, TAG, "%s: OUT", __func__);
            ca = readFile(TESTDIR"/cert/ca.der");
            crl = readFile(TESTDIR"/cert/crl.der");
            key = readFile(TESTDIR"/cert/key.der");
            cert = readFile(TESTDIR"/cert/crt.der");

            CAsetPkixInfoCallback(PkixInfoHandler);

            oc_thread_new(&pInternalThread, __process__, NULL);

            EXPECT_EQ(CA_STATUS_OK, CAinitSslAdapter());
            oc_mutex_lock(g_sslContextMutex);

            OIC_LOG_V(DEBUG, TAG, "%s: OUT", __func__);
        }

        static void TearDownTestCase()
        {
            OIC_LOG_V(DEBUG, TAG, "%s: OUT", __func__);
            pInternalWorking = false;
            oc_mutex_unlock(g_sslContextMutex);
            CAdeinitSslAdapter();
            OIC_LOG_V(DEBUG, TAG, "%s: OUT", __func__);
        }
};

TEST_F(OCAA, CAcloseSslConnection)
{
    CAEndpoint_t *ep = (CAEndpoint_t *)OICCalloc(1, sizeof(CAEndpoint_t));
    sprintf(ep->addr, "127.0.0.1");
    ep->port = 3335;
    ep->adapter = CA_ADAPTER_IP;

    SslContext_t *backup = g_caSslContext;
    g_caSslContext = NULL;
    CAcloseSslConnection(ep);
    g_caSslContext = backup;
    CAcloseSslConnection(ep);
}

TEST_F(OCAA, CAcloseSslConnectionAll)
{
    CAEndpoint_t *ep = (CAEndpoint_t *)OICCalloc(1, sizeof(CAEndpoint_t));
    sprintf(ep->addr, "127.0.0.1");
    ep->port = 3335;
    ep->adapter = CA_ADAPTER_IP;
    SslEndPoint_t *sep = NULL;
    sep = NewSslEndPoint(ep, &(ep->adapter == CA_ADAPTER_IP ?
                               g_caSslContext->clientDtlsConf : g_caSslContext->clientTlsConf));
    if (sep)
    {
        oc_mutex_lock(g_sslContextMutex);
        u_arraylist_add(g_caSslContext->peerList, (void *) sep);
        oc_mutex_unlock(g_sslContextMutex);
    }

    SslContext_t *backup = g_caSslContext;
    g_caSslContext = NULL;
    CAcloseSslConnectionAll(CA_ADAPTER_IP);
    g_caSslContext = backup;
    CAcloseSslConnectionAll(CA_ADAPTER_IP);
}

TEST_F(OCAA, GetAlertCode)
{
    EXPECT_EQ(0x2D, GetAlertCode(MBEDTLS_SSL_ALERT_MSG_CERT_EXPIRED));
    EXPECT_EQ(0x2D, GetAlertCode(MBEDTLS_X509_BADCERT_EXPIRED));
}

static CAResult_t peerCNVerifyCallbackOk(const unsigned char *cn, size_t cnLen)
{
    OC_UNUSED(cn);
    OC_UNUSED(cnLen);
    return CA_STATUS_OK;
}

static CAResult_t peerCNVerifyCallbackFailed(const unsigned char *cn, size_t cnLen)
{
    OC_UNUSED(cn);
    OC_UNUSED(cnLen);
    return CA_STATUS_FAILED;
}

TEST_F(OCAA, PeerCertExtractCN)
{
    mbedtls_x509_crt *crt = (mbedtls_x509_crt *)OICCalloc(1, sizeof(mbedtls_x509_crt));
    mbedtls_x509_crt_init(crt);

    ASSERT_TRUE(NULL != cert);

    unsigned ret = mbedtls_x509_crt_parse(crt, cert->data, cert->len);
    LOG_MBEDTLS_ERROR(ret);

    CAsetPeerCNVerifyCallback(peerCNVerifyCallbackOk);
    EXPECT_EQ(CA_STATUS_OK, PeerCertExtractCN(crt));

    CAsetPeerCNVerifyCallback(peerCNVerifyCallbackFailed);
    EXPECT_EQ(CA_STATUS_FAILED, PeerCertExtractCN(crt));

    CAsetPeerCNVerifyCallback(NULL);
    EXPECT_EQ(CA_STATUS_OK, PeerCertExtractCN(crt));

    mbedtls_x509_crt_free(crt);

    OICFree(crt);
}

TEST_F(OCAA, GetAdapterIndex)
{
    EXPECT_EQ(0, GetAdapterIndex(CA_ADAPTER_IP));
    EXPECT_EQ(1, GetAdapterIndex(CA_ADAPTER_TCP));
    EXPECT_EQ(2, GetAdapterIndex(CA_ADAPTER_GATT_BTLE));
    EXPECT_EQ(-1, GetAdapterIndex(CA_ALL_ADAPTERS));
}

TEST_F(OCAA, DeInitPkixInfo)
{
    PkiInfo_t *inf = NULL;
    DeInitPkixInfo(inf);
}

TEST_F(OCAA, SetCASecureEndpointAttribute)
{
    CAEndpoint_t *peer = NULL;
    SslContext_t *backup = g_caSslContext;
    g_caSslContext = NULL;
    EXPECT_FALSE(SetCASecureEndpointAttribute(peer, 0));
    g_caSslContext = backup;
    EXPECT_FALSE(SetCASecureEndpointAttribute(peer, 0));
}

void _CAgetIdentityHandler(UuidContext_t *list, unsigned char *p, size_t len)
{
    OC_UNUSED(list);
    OC_UNUSED(p);
    OC_UNUSED(len);
}

TEST_F(OCAA, verifyIdentity)
{
    EXPECT_EQ(MBEDTLS_ERR_X509_FEATURE_UNAVAILABLE, verifyIdentity(NULL, NULL, 1, NULL));
    CAsetIdentityCallback(_CAgetIdentityHandler);

    mbedtls_x509_crt *crt = (mbedtls_x509_crt *)OICCalloc(1, sizeof(mbedtls_x509_crt));
    mbedtls_x509_crt_init(crt);

    EXPECT_EQ(MBEDTLS_ERR_X509_BAD_INPUT_DATA, verifyIdentity(NULL, crt, 1, NULL));

    ASSERT_TRUE(NULL != cert);
    LOG_MBEDTLS_ERROR(mbedtls_x509_crt_parse(crt, cert->data, cert->len));

    EXPECT_EQ(0, verifyIdentity(NULL, crt, 1, NULL));
    EXPECT_EQ(-1, verifyIdentity(NULL, crt, 0, NULL));

    mbedtls_x509_crt_free(crt);
    OICFree(crt);
}

/*
TEST_F(OCAA, PKIX)
{
    InitPKIX(CA_ADAPTER_IP);
}
*/
TEST_F(OCAA, ConvertMbedtlsCodesToCAResult)
{
    EXPECT_EQ(CA_STATUS_OK, ConvertMbedtlsCodesToCAResult(0));
    EXPECT_EQ(CA_DTLS_AUTHENTICATION_FAILURE, ConvertMbedtlsCodesToCAResult(1));
}

TEST_F(OCAA, checkSslOperation)
{
    SslEndPoint_t *peer = (SslEndPoint_t *)OICCalloc(1, sizeof(SslEndPoint_t));
    mbedtls_ssl_init( &peer->ssl );
    peer->ssl.conf = (mbedtls_ssl_config *)OICCalloc(1, sizeof(mbedtls_ssl_config));
    mbedtls_ssl_config_defaults((mbedtls_ssl_config *)peer->ssl.conf,
                                MBEDTLS_SSL_IS_SERVER, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);

    EXPECT_TRUE(checkSslOperation(peer, MBEDTLS_ERR_SSL_WANT_READ, NULL, 0));
    EXPECT_TRUE(checkSslOperation(peer, MBEDTLS_ERR_SSL_NON_FATAL, NULL, 0));
    EXPECT_FALSE(checkSslOperation(peer, MBEDTLS_ERR_SSL_CONN_EOF, NULL, 0));

    OICFree((void *)peer->ssl.conf);
    OICFree(peer);
}

TEST_F(OCAA, CA_EncDeCryptSsl)
{
    CASecureEndpoint_t *sep = NULL;
    uint8_t *data = NULL;
    size_t dataLen = 0;
    CAEndpoint_t *ep = NULL;

    EXPECT_EQ(CA_STATUS_INVALID_PARAM, CAdecryptSsl(sep, data, dataLen));
    EXPECT_EQ(CA_STATUS_INVALID_PARAM, CAencryptSsl(ep, data, dataLen));

    ep = (CAEndpoint_t *)OICCalloc(1, sizeof(CAEndpoint_t));
    sprintf(ep->addr, "127.0.0.1");
    ep->port = 3333;
    ep->adapter = CA_ADAPTER_IP;

    data = (uint8_t *)OICCalloc(512, 1);

    EXPECT_EQ(CA_STATUS_FAILED, CAencryptSsl(ep, data, dataLen));
    dataLen = 512;

    SslContext_t *backup = g_caSslContext;
    g_caSslContext = NULL;
    EXPECT_EQ(CA_STATUS_FAILED, CAencryptSsl(ep, data, dataLen));
    EXPECT_EQ(CA_STATUS_FAILED, CAdecryptSsl(sep, data, dataLen));
    g_caSslContext = backup;

    EXPECT_EQ(CA_STATUS_INVALID_PARAM, CAdecryptSsl(sep, data, dataLen));
}

TEST_F(OCAA, GetCASecureEndpointData)
{
    CASecureEndpoint_t *sep = NULL;
    CAEndpoint_t *ep = NULL;
    SslContext_t *backup = g_caSslContext;
    g_caSslContext = NULL;
    EXPECT_EQ(CA_STATUS_NOT_INITIALIZED, GetCASecureEndpointData(ep, sep));
    g_caSslContext = backup;
    EXPECT_EQ(CA_STATUS_INVALID_PARAM, GetCASecureEndpointData(ep, sep));

}

static void *process(void *p)
{
    OIC_LOG_V(DEBUG, TAG, "%s: IN", __func__);
    OC_UNUSED(p);

    uint32_t attr;
    CAEndpoint_t *ep = (CAEndpoint_t *)OICCalloc(1, sizeof(CAEndpoint_t));
    sprintf(ep->addr, "127.0.0.1");
    ep->port = 3333;

    SslContext_t *backup = g_caSslContext;
    g_caSslContext = NULL;
    EXPECT_FALSE(GetCASecureEndpointAttributes(ep, &attr));
    g_caSslContext = backup;

    EXPECT_FALSE(GetCASecureEndpointAttributes(ep, &attr));

    OIC_LOG_V(DEBUG, TAG, "%s: OUT", __func__);
    return NULL;
}

TEST_F(OCAA, GetCASecureEndpointAttributes)
{
    oc_thread p1;
    oc_mutex_unlock(g_sslContextMutex);
    oc_thread_new(&p1, process, NULL);
    oc_thread_wait(p1);
    oc_mutex_lock(g_sslContextMutex);
}

TEST_F(OCAA, CAsslGenerateOwnerPsk)
{
    CAEndpoint_t *ep = (CAEndpoint_t *)OICCalloc(1, sizeof(CAEndpoint_t));
    sprintf(ep->addr, "127.0.0.1");
    ep->port = 3333;
    ep->adapter = CA_ADAPTER_IP;
    SslEndPoint_t *sep = NULL;
    sep = NewSslEndPoint(ep, &(ep->adapter == CA_ADAPTER_IP ?
                               g_caSslContext->clientDtlsConf : g_caSslContext->clientTlsConf));
    if (sep)
    {
        oc_mutex_lock(g_sslContextMutex);
        u_arraylist_add(g_caSslContext->peerList, (void *) sep);
        oc_mutex_unlock(g_sslContextMutex);
    }

    size_t labelLen = 32;
    unsigned char *label = (unsigned char *)OICCalloc(labelLen, 1);
    size_t random1Len = 64;
    unsigned char *random1 = (unsigned char *)OICCalloc(random1Len, 1);
    size_t random2Len = 64;
    unsigned char *random2 = (unsigned char *)OICCalloc(random2Len, 1);
    size_t ownerPskSize = 512;
    uint8_t *ownerPsk = (unsigned char *)OICCalloc(ownerPskSize, 1);
    SslContext_t *backup = g_caSslContext;
    g_caSslContext = NULL;
    EXPECT_EQ(CA_STATUS_FAILED, CAsslGenerateOwnerPsk(ep, label, labelLen, random1, random1Len, random2,
              random2Len, ownerPsk, ownerPskSize));
    g_caSslContext = backup;
    g_caSslContext->selectedCipher = MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_CCM;
    EXPECT_EQ(CA_STATUS_OK, CAsslGenerateOwnerPsk(ep, label, labelLen, random1, random1Len, random2,
              random2Len, ownerPsk, ownerPskSize));
    g_caSslContext->selectedCipher = MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256;
    EXPECT_EQ(CA_STATUS_OK, CAsslGenerateOwnerPsk(ep, label, labelLen, random1, random1Len, random2,
              random2Len, ownerPsk, ownerPskSize));

    g_caSslContext->selectedCipher = MBEDTLS_TLS_RSA_WITH_AES_256_CBC_SHA256;
    EXPECT_EQ(CA_STATUS_OK, CAsslGenerateOwnerPsk(ep, label, labelLen, random1, random1Len, random2,
              random2Len, ownerPsk, ownerPskSize));

    g_caSslContext->selectedCipher = MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384;
    EXPECT_EQ(CA_STATUS_OK, CAsslGenerateOwnerPsk(ep, label, labelLen, random1, random1Len, random2,
              random2Len, ownerPsk, ownerPskSize));
    g_caSslContext->selectedCipher = MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384;
    EXPECT_EQ(CA_STATUS_OK, CAsslGenerateOwnerPsk(ep, label, labelLen, random1, random1Len, random2,
              random2Len, ownerPsk, ownerPskSize));
}

TEST_F(OCAA, CAinitiateSslHandshake)
{
    CAEndpoint_t *ep = (CAEndpoint_t *)OICCalloc(1, sizeof(CAEndpoint_t));
    sprintf(ep->addr, "127.0.0.7");
    ep->port = 8833;
    ep->adapter = CA_ADAPTER_IP;
    SslEndPoint_t *sep = NULL;
    sep = NewSslEndPoint(ep, &(ep->adapter == CA_ADAPTER_IP ?
                               g_caSslContext->clientDtlsConf : g_caSslContext->clientTlsConf));
    if (sep)
    {
        oc_mutex_lock(g_sslContextMutex);
        u_arraylist_add(g_caSslContext->peerList, (void *) sep);
        oc_mutex_unlock(g_sslContextMutex);
    }

    g_caSslContext->cipherFlag[1] = false;
    EXPECT_EQ(CA_STATUS_OK, CAinitiateSslHandshake(ep));
}

TEST_F(OCAA, CAsetTlsCipherSuite)
{
    SslContext_t *backup = g_caSslContext;
    g_caSslContext = NULL;
    EXPECT_EQ(CA_STATUS_NOT_INITIALIZED, CAsetTlsCipherSuite(SSL_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256));
    g_caSslContext = backup;
}

TEST_F(OCAA, GetCipherIndex)
{
    for (int i = 0; i < 0xffff; i++)
    {
        GetCipherIndex(i);
    }
}

TEST_F(OCAA, StartRetransmit)
{
    SslContext_t *backup = g_caSslContext;
    g_caSslContext = NULL;
    StartRetransmit(NULL);
    g_caSslContext = backup;
}


