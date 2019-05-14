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
#include <gtest/gtest.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "tools.h"
#undef TAG
#include "../src/certhelpers.c"
#include "utlist.h"
#include "occertutility.h"
#include "mbedtls/pem.h"

#ifdef __cplusplus
}
#endif

using namespace std;

#ifdef TAG
#undef TAG
#endif

#define TAG  "CERT-HLP-UT"

#define SVR_DB_FILE_NAME TAG".dat"
#define PM_DB_FILE_NAME TAG".db"

#define ISSUER_CERT "resource/csdk/security/unittest/issuer/cert.pem"
#define ISSUER_KEY "resource/csdk/security/unittest/issuer/key.pem"
#define ISSUER_CERT_DER "resource/csdk/security/unittest/issuer/cert.der"

#define PEM_BEGIN_CRT           "-----BEGIN CERTIFICATE-----\n"
#define PEM_END_CRT             "-----END CERTIFICATE-----\n"

static mbedtls_pk_context keyPair;
static uint8_t *certificate;
static size_t certificateLen;
static uint8_t *pubKey;
static size_t pubKeyLen;
static const char *publicKey;
static const char *issuerCert;
static uint8_t *issuerCertDer;
static size_t issuerCertDerLen;

static const char* certificateCAPem;
//static uint8_t* certificateCA;
static size_t certificateCALen;
static const char *privateKeyCA;
static const char *publicKeyCA;
static uint8_t *pubKeyCA;
static size_t pubKeyCALen;

class CERT_HLP : public ::testing::Test
{
    public:
        static void SetUpTestCase()
        {
            OicUuid_t *subjectUuid = NULL;
            const char *issuerPrivateKey = NULL;
            issuerCert = NULL;
            ASSERT_EQ(0, readTextFile(ISSUER_CERT, &issuerCert));
            ASSERT_NE(nullptr, issuerCert);
            ASSERT_EQ(0, readTextFile(ISSUER_KEY, &issuerPrivateKey));
            ASSERT_NE(nullptr, issuerPrivateKey);
            ASSERT_EQ(0, readFile(ISSUER_CERT_DER, &issuerCertDer, &issuerCertDerLen));
            ASSERT_NE(nullptr, issuerCertDer);

            subjectUuid = createUuid();
            ASSERT_NE(nullptr, subjectUuid);

            mbedtls_pk_context keyPairCA;
            EXPECT_GE(OCInternalGenerateKeyPair(&keyPairCA), 0);

            int ret = 0;
            size_t len = 2048;
            uint8_t *buf = (uint8_t*)OICCalloc(len, 1);
            ASSERT_NE(buf, nullptr);

            ret = 0;
            ret = mbedtls_pk_write_pubkey_der(&keyPairCA, buf, len);
            ASSERT_GE(ret, 0);
            pubKeyCA = (uint8_t*)OICCalloc(ret, 1);
            memcpy(pubKeyCA, buf + len - ret, ret);
            pubKeyCALen = ret;

            ret = 0;
            ret = mbedtls_pk_write_pubkey_pem(&keyPairCA, buf, len);
            ASSERT_EQ(ret, 0);
            ret = (int)strlen((const char*)buf);
            publicKeyCA = (char*)OICCalloc(ret + 1, 1);
            ASSERT_NE(publicKeyCA, nullptr);
            memcpy((char*)publicKeyCA, buf, ret);

            ret = 0;
            ret = mbedtls_pk_write_key_pem(&keyPairCA, buf, len);
            ASSERT_GE(ret, 0);
            size_t pkLen = strlen((const char*)buf);
            privateKeyCA = (char*)OICCalloc( pkLen+ 1, 1);
            ASSERT_NE(privateKeyCA, nullptr);
            memcpy((char*)privateKeyCA, buf, pkLen);
            OIC_LOG_V(INFO, TAG, "pk: len:%lu pem:\n%s", pkLen, privateKeyCA);

            const char *serial = NULL;
            size_t serialLen = 0;
            ASSERT_EQ(OC_STACK_OK,  OCGenerateRandomSerialNumber((char**)&serial, &serialLen));

            EXPECT_EQ(OC_STACK_OK, OCGenerateRootCACertificate(
                "C=UA, O=Samsung, CN=CA",
                publicKeyCA, issuerCert, issuerPrivateKey,
                serial, "20190101235959", "20210101235959",
                (char**)&certificateCAPem, &certificateCALen
            ));
            ASSERT_NE(nullptr, certificateCAPem);
            ASSERT_TRUE(certificateCALen > 0);

            mbedtls_pk_free(&keyPairCA);

            OIC_LOG_V(INFO, TAG, "ca: pem:\n%s", certificateCAPem);
            //OCSaveTrustCertChain

            mbedtls_pk_init(&keyPair);
            EXPECT_GE(OCInternalGenerateKeyPair(&keyPair), 0);

            ret = 0;
            ret = mbedtls_pk_write_pubkey_der(&keyPair, buf, len);
            ASSERT_GE(ret, 0);
            pubKey = (uint8_t*)OICCalloc(ret, 1);
            memcpy(pubKey, buf + len - ret, ret);
            pubKeyLen = ret;

            ret = 0;
            ret = mbedtls_pk_write_pubkey_pem(&keyPair, buf, len);
            ASSERT_EQ(ret, 0);
            ret = (int)strlen((const char*)buf);
            publicKey = (char*)OICCalloc(ret + 1, 1);
            ASSERT_NE(publicKey, nullptr);
            memcpy((char*)publicKey, buf, ret);

            ASSERT_EQ(OC_STACK_OK,  OCGenerateRandomSerialNumber((char**)&serial, &serialLen));

            EXPECT_EQ(OC_STACK_OK, OCGenerateRoleCertificate(
                subjectUuid, publicKey, certificateCAPem, privateKeyCA,
                serial, "20190101235959", "20210101235959", "1.3.6.1.4.1.44924.1.7", "samsung",
                (char**)&certificate, &certificateLen
            ));
            ASSERT_NE(nullptr, certificate);
            ASSERT_TRUE(certificateLen > 0);

            OICFree(buf);
        }
        static void TearDownTestCase()
        {
            mbedtls_pk_free(&keyPair);
        }
};

TEST_F(CERT_HLP, OCInternalGenerateKeyPair)
{
    EXPECT_EQ(-1, OCInternalGenerateKeyPair(NULL));
}

TEST_F(CERT_HLP, InvalidCertificateBuffer)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, OCInternalIsValidCertChain(NULL, 0));
}

TEST_F(CERT_HLP, GenerateCSRForKey)
{
    OCByteString *csr = (OCByteString *)OICCalloc(1, sizeof(OCByteString));
    EXPECT_EQ(-1, OCInternalCSRRequest(NULL, NULL, OIC_ENCODING_RAW, NULL));
    EXPECT_EQ(-1, GenerateCSRForKey("test", &keyPair, OIC_ENCODING_RAW, csr));
    EXPECT_GE(0, GenerateCSRForKey("test", &keyPair, OIC_ENCODING_PEM, csr));
    EXPECT_GE(0, GenerateCSRForKey("%$#!!===========,,,,,,,,,,", &keyPair, OIC_ENCODING_PEM, csr));
    EXPECT_GE(0, GenerateCSRForKey("C=UA,O=Samsung,CN=uuid:44444442-4444-4444-5555-666666666666", &keyPair, OIC_ENCODING_DER, csr));
    OICFree(csr);
}

TEST_F(CERT_HLP, OCInternalIsValidRoleCertificate)
{
    size_t _pubKeyLen = 2048;
    uint8_t* _pubKey = NULL;

    ByteArrayLL_t *crt = NULL, *tmp = NULL, *list = getCerts();
    LL_FOREACH_SAFE(list, crt, tmp)
    {
        EXPECT_EQ(OC_STACK_INVALID_PARAM, OCInternalIsValidRoleCertificate(crt->cert->data, crt->cert->len, &_pubKey,
                  &_pubKeyLen));
        LL_DELETE(list, crt);
        OICFree(crt->cert->data);
        OICFree(crt->cert);
        OICFree(crt);
    }
}

TEST_F(CERT_HLP, OCInternalVerifyRoleCertificate)
{
    OicSecKey_t *certificateChain = NULL;
    OicSecRole_t *roles = NULL;
    size_t rolesLength = 0;

    ByteArrayLL_t *trustedCaCerts = (ByteArrayLL_t*)OICCalloc(1, sizeof(ByteArrayLL_t));
    trustedCaCerts->next = NULL;
    trustedCaCerts->cert = (ByteArray_t*)OICCalloc(1, sizeof(ByteArray_t));
    trustedCaCerts->cert->data = (uint8_t*)certificateCAPem;
    trustedCaCerts->cert->len = certificateCALen;

    time_t rtime;
    time(&rtime);
    struct tm *notValidAfter =  localtime(&rtime);

    EXPECT_EQ(OC_STACK_INVALID_PARAM, OCInternalVerifyRoleCertificate(certificateChain, trustedCaCerts,
              &roles, &rolesLength, notValidAfter));

    certificateChain = (OicSecKey_t *)OICCalloc(1, sizeof(OicSecKey_t));
    certificateChain->data = certificate;
    certificateChain->len = certificateLen;
    certificateChain->encoding = OIC_ENCODING_DER;
    EXPECT_EQ(OC_STACK_OK, OCInternalVerifyRoleCertificate(certificateChain, trustedCaCerts, &roles,
              &rolesLength, notValidAfter));

    certificateChain->data[certificateChain->len - 1] = 1;
    EXPECT_EQ(OC_STACK_OK, OCInternalVerifyRoleCertificate(certificateChain, trustedCaCerts, &roles,
              &rolesLength, notValidAfter));
}
