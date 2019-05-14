/******************************************************************
*
* Copyright 2019 Samsung Electronics All Rights Reserved.
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
#include <gtest/gtest.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "tools.h"
#undef TAG
#include "../src/occertutility.c"

#ifdef __cplusplus
}
#endif

#ifdef TAG
#undef TAG
#endif

#define TAG  "CERTUTIL"

#define SVR_DB_FILE_NAME TAG".dat"
#define PM_DB_FILE_NAME TAG".db"

#define CSR "resource/csdk/security/unittest/csr/csr.pem"
#define ISSUER_CERT "resource/csdk/security/unittest/issuer/cert.pem"
#define ISSUER_KEY "resource/csdk/security/unittest/issuer/key.pem"

static const char* csr = NULL;
static const char *issuerCert = NULL;
static const char *issuerPrivateKey = NULL;
static OicUuid_t *subjectUuid = NULL;
static const char *subjectPublicKey = NULL;
static const char *serial = NULL;
static size_t serialLen = 0;
static const char *authority = NULL;
static const char *notValidBefore;
static const char *notValidAfter;
static const char *role;
static const char *subject;

class CERTUTIL : public ::testing::Test
{
    public:
        static void SetUpTestCase()
        {
            try
            {
            ASSERT_EQ(0, readTextFile(CSR, &csr));
            ASSERT_NE(nullptr, csr);
            ASSERT_EQ(0, readTextFile(ISSUER_CERT, &issuerCert));
            ASSERT_NE(nullptr, issuerCert);
            ASSERT_EQ(0, readTextFile(ISSUER_KEY, &issuerPrivateKey));
            ASSERT_NE(nullptr, issuerPrivateKey);

            subjectUuid = (OicUuid_t*)OICCalloc(1, sizeof(OicUuid_t));
            ASSERT_NE(nullptr, subjectUuid);

            ASSERT_EQ(OC_STACK_OK, OCGetUuidFromCSR(csr, subjectUuid));
            ASSERT_NE(nullptr, subjectUuid);

            ASSERT_EQ(OC_STACK_OK, OCGetPublicKeyFromCSR(csr, (char**)&subjectPublicKey));
            ASSERT_NE(nullptr, subjectPublicKey);

            ASSERT_EQ(OC_STACK_OK, OCGenerateRandomSerialNumber((char**)&serial, &serialLen));

            notValidBefore = "20190101235959";
            notValidAfter = "20210101235959";
            role = "1.3.6.1.4.1.44924.1.7";
            subject = "33333333-3333-3333-3333-222222222222";
            }
            catch(std::exception& e)
            {
                std::cout << "SetUpTestCase exception: " << e.what() << std::endl;
            }
        }
        static void TearDownTestCase()
        {
            OICFree((void*)serial);
            OICFree((void*)subjectPublicKey);
            OICFree(subjectUuid);
            OICFree((void*)csr);
            OICFree((void*)issuerCert);
            OICFree((void*)issuerPrivateKey);
        }
};

TEST_F(CERTUTIL, OCGenerateRandomSerialNumber)
{
    char* _serial = NULL;
    size_t _serialLen = 0;
    EXPECT_EQ(OC_STACK_OK, OCGenerateRandomSerialNumber(&_serial, &_serialLen));
    EXPECT_TRUE(_serialLen > 0);
    EXPECT_NE(nullptr, _serial);
}

TEST_F(CERTUTIL, OCGenerateKeyPair)
{
    char *publicKey = NULL;
    size_t publicKeyLen = 0;
    char *privateKey = NULL;
    size_t privateKeyLen = 0;
    EXPECT_EQ(OC_STACK_OK, OCGenerateKeyPair(&publicKey, &publicKeyLen, &privateKey, &privateKeyLen));
    EXPECT_TRUE(publicKeyLen > 0);
    EXPECT_NE(nullptr, publicKey);
    EXPECT_TRUE(privateKeyLen > 0);
    EXPECT_NE(nullptr, privateKey);
}

TEST_F(CERTUTIL, OCConvertDerCSRToPem)
{
    const char* derCSR = (const char*)certData();
    size_t derCSRLen = certDataLen();
    char* pemCSR = NULL;

    EXPECT_EQ(OC_STACK_OK, OCConvertDerCSRToPem(derCSR, derCSRLen, &pemCSR));
}


TEST_F(CERTUTIL, CSR_ALL)
{
    char* key = NULL;

    EXPECT_EQ(OC_STACK_OK, OCGetPublicKeyFromCSR(csr, &key));
    EXPECT_NE(nullptr, key);

    OicUuid_t* uuid = (OicUuid_t*)OICCalloc(1, sizeof(OicUuid_t));
    EXPECT_EQ(OC_STACK_OK, OCGetUuidFromCSR(csr, uuid));
    EXPECT_NE(nullptr, uuid);
    OICFree(uuid);

    EXPECT_EQ(OC_STACK_ERROR, OCVerifyCSRSignature(csr));
}

TEST_F(CERTUTIL, VerifyCSRSignature)
{
    mbedtls_x509_csr csrObj;

    mbedtls_x509_csr_init(&csrObj);
    EXPECT_EQ(0, mbedtls_x509_csr_parse(&csrObj, (const unsigned char*)csr, strlen(csr) + 1));

    EXPECT_EQ(-1, VerifyCSRSignature(&csrObj));

    csrObj.sig_md = MBEDTLS_MD_NONE;
    EXPECT_EQ(-1, VerifyCSRSignature(&csrObj));
    csrObj.sig_md = MBEDTLS_MD_SHA256;

    uint8_t *p = csrObj.cri.p;
    csrObj.cri.p = NULL;
    EXPECT_EQ(-1, VerifyCSRSignature(&csrObj));
    csrObj.cri.p = p;

    p = csrObj.sig.p;
    csrObj.sig.p = NULL;
    EXPECT_EQ(-1, VerifyCSRSignature(&csrObj));
    csrObj.sig.p = p;

    mbedtls_x509_csr_free(&csrObj);
}

TEST_F(CERTUTIL, OCGenerateRoleCertificate)
{
    char *certificate = NULL;
    size_t certificateLen = 0;

    EXPECT_EQ(OC_STACK_INVALID_PARAM, OCGenerateRoleCertificate(
        NULL, subjectPublicKey, issuerCert, issuerPrivateKey,
        serial, notValidBefore, notValidAfter, role, authority,
        &certificate, &certificateLen
    ));

    EXPECT_EQ(OC_STACK_INVALID_PARAM, OCGenerateRoleCertificate(
        subjectUuid, subjectPublicKey, issuerCert, issuerPrivateKey,
        serial, notValidBefore, notValidAfter, NULL, authority,
        &certificate, &certificateLen
    ));


    EXPECT_EQ(OC_STACK_OK, OCGenerateRoleCertificate(
        subjectUuid, subjectPublicKey, issuerCert, issuerPrivateKey,
        serial, notValidBefore, notValidAfter, role, authority,
        &certificate, &certificateLen
    ));
    ASSERT_NE(nullptr, certificate);
    ASSERT_TRUE(certificateLen > 0);
    OICFree(certificate);
}

TEST_F(CERTUTIL, OCGenerateIdentityCertificate)
{
    char *certificate = NULL;
    size_t certificateLen = 0;

    EXPECT_EQ(OC_STACK_INVALID_PARAM, OCGenerateIdentityCertificate(
        subjectUuid, subjectPublicKey, NULL, issuerPrivateKey,
        serial, notValidBefore, notValidAfter,
        &certificate, &certificateLen
    ));

    EXPECT_EQ(OC_STACK_OK, OCGenerateIdentityCertificate(
        subjectUuid, subjectPublicKey, issuerCert, issuerPrivateKey,
        serial, notValidBefore, notValidAfter,
        &certificate, &certificateLen
    ));
    ASSERT_NE(nullptr, certificate);
    ASSERT_TRUE(certificateLen > 0);
    OICFree(certificate);
}

TEST_F(CERTUTIL, OCGenerateIntermediateCACertificate)
{
    char *certificate = NULL;
    size_t certificateLen = 0;

    EXPECT_EQ(OC_STACK_OK, OCGenerateIntermediateCACertificate(
        subject, subjectPublicKey, issuerCert, issuerPrivateKey,
        serial, notValidBefore, notValidAfter,
        &certificate, &certificateLen
    ));
    ASSERT_NE(nullptr, certificate);
    ASSERT_TRUE(certificateLen > 0);
    OICFree(certificate);
}

TEST_F(CERTUTIL, OCGenerateRootCACertificate)
{
    char *certificate = NULL;
    size_t certificateLen = 0;

    EXPECT_EQ(OC_STACK_OK, OCGenerateRootCACertificate(
        subject, subjectPublicKey, issuerCert, issuerPrivateKey,
        serial, notValidBefore, notValidAfter,
        &certificate, &certificateLen
    ));
    ASSERT_NE(nullptr, certificate);
    ASSERT_TRUE(certificateLen > 0);
    OICFree(certificate);
}

