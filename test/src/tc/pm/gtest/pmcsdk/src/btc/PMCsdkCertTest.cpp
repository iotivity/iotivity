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
 *      LICENSE-2.0" target="_blank">http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 ******************************************************************/

#include <iostream>
#include <string.h>
#include <oic_malloc.h>
#include "utlist.h"
#include "secureresourceprovider.h"
#include "cert_generator.h"
#include "ck_manager.h"
#include "pki.h"
#include "sn_store.h"
#include "der_dec.h"
#include "crl.h"
#include "crl_generator.h"
#include "ckm_info.h"

#include <gtest/gtest.h>
#include "CommonUtil.h"
#include "IotivityTest_Logger.h"

#define RUNS          1
#define MAX_LEN     1000
#define TEST_SN       50
#define READ_WRITE_BLOCK_N 1ul
#define N_LENGTH_BYTES 3
#define CRL_DEFAULT_CRL_ID           1
#define CRL_DEFAULT_THIS_UPDATE     "160601000000Z"
#define CRL_DEFAULT_CRL_DATA        "-"
#define NUMBER_OF_REVOKED 2
#define SIMPLE_CRT_LEN 469 // Length of test certificate

OCPersistentStorage ps =
{ NULL, NULL, NULL, NULL, NULL };
const char *CKMI_JSON_FILE_NAME = "CKMInfo.json";

FILE* ckm_fopen(const char * /*path*/, const char *mode)
{
    return fopen(CKMI_JSON_FILE_NAME, mode);
}

void SetPersistentHandler(OCPersistentStorage *ps)
{
    if (ps)
    {
        ps->open = ckm_fopen;
        ps->read = fread;
        ps->write = fwrite;
        ps->close = fclose;
        ps->unlink = unlink;
    }
}

class PMCsdkCertTest_btc: public ::testing::Test
{
protected:
    bool isSuccess;
    static CertificateX509 g_certificate;
    static const ByteArray g_caPublicKey;
    static const ByteArray g_derCode;
    static ByteArray g_serNum;

    virtual void SetUp()
    {
        CommonUtil::runCommonTCSetUpPart();

        ps =
        {   NULL, NULL, NULL, NULL, NULL};

        SetPersistentHandler(&ps);
        OCStackResult res = OCRegisterPersistentStorageHandler(&ps);

        if (res != OC_STACK_OK)
            SET_FAILURE("Error is occurred in SetUp");
    }

    virtual void TearDown()
    {
        CommonUtil::runCommonTCTearDownPart();
    }
public:

};

CertificateX509 PMCsdkCertTest_btc::g_certificate;

const ByteArray PMCsdkCertTest_btc::g_derCode =
{ (uint8_t[]
        )
        { 0x30, 0x82, 0x01, 0xd1, 0x30, 0x82, 0x01, 0x77, 0xa0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x09,
                0x00, 0xd7, 0x56, 0x8c, 0xfc, 0x53, 0x18, 0xb0, 0xab, 0x30, 0x0a, 0x06, 0x08, 0x2a,
                0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x02, 0x30, 0x45, 0x31, 0x0b, 0x30, 0x09, 0x06,
                0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x41, 0x55, 0x31, 0x13, 0x30, 0x11, 0x06, 0x03,
                0x55, 0x04, 0x08, 0x0c, 0x0a, 0x53, 0x6f, 0x6d, 0x65, 0x2d, 0x53, 0x74, 0x61, 0x74,
                0x65, 0x31, 0x21, 0x30, 0x1f, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x18, 0x49, 0x6e,
                0x74, 0x65, 0x72, 0x6e, 0x65, 0x74, 0x20, 0x57, 0x69, 0x64, 0x67, 0x69, 0x74, 0x73,
                0x20, 0x50, 0x74, 0x79, 0x20, 0x4c, 0x74, 0x64, 0x30, 0x1e, 0x17, 0x0d, 0x31, 0x35,
                0x30, 0x33, 0x31, 0x32, 0x31, 0x32, 0x32, 0x35, 0x31, 0x31, 0x5a, 0x17, 0x0d, 0x31,
                0x37, 0x30, 0x33, 0x31, 0x31, 0x31, 0x32, 0x32, 0x35, 0x31, 0x31, 0x5a, 0x30, 0x45,
                0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x41, 0x55, 0x31,
                0x13, 0x30, 0x11, 0x06, 0x03, 0x55, 0x04, 0x08, 0x0c, 0x0a, 0x53, 0x6f, 0x6d, 0x65,
                0x2d, 0x53, 0x74, 0x61, 0x74, 0x65, 0x31, 0x21, 0x30, 0x1f, 0x06, 0x03, 0x55, 0x04,
                0x0a, 0x0c, 0x18, 0x49, 0x6e, 0x74, 0x65, 0x72, 0x6e, 0x65, 0x74, 0x20, 0x57, 0x69,
                0x64, 0x67, 0x69, 0x74, 0x73, 0x20, 0x50, 0x74, 0x79, 0x20, 0x4c, 0x74, 0x64, 0x30,
                0x59, 0x30, 0x13, 0x06, 0x07, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x02, 0x01, 0x06, 0x08,
                0x2a, 0x86, 0x48, 0xce, 0x3d, 0x03, 0x01, 0x07, 0x03, 0x42, 0x00, 0x04, 0x8c, 0xc8,
                0x92, 0x1d, 0xaa, 0x7f, 0xf0, 0xe4, 0xb2, 0x75, 0xd6, 0x4a, 0xf1, 0xd5, 0x14, 0x3f,
                0x1a, 0x09, 0xc5, 0x3e, 0x52, 0xd6, 0xda, 0xa0, 0xbf, 0x90, 0x43, 0xd1, 0x6b, 0xfe,
                0xd1, 0xb3, 0x75, 0x5c, 0xdd, 0x69, 0xac, 0x42, 0xa1, 0xcb, 0x03, 0x16, 0xee, 0xa4,
                0x30, 0xa5, 0x8d, 0x36, 0x8f, 0xc5, 0x7b, 0xb4, 0xb5, 0x6a, 0x7d, 0x9b, 0x16, 0x04,
                0x46, 0xab, 0xae, 0xbb, 0x56, 0xa1, 0xa3, 0x50, 0x30, 0x4e, 0x30, 0x1d, 0x06, 0x03,
                0x55, 0x1d, 0x0e, 0x04, 0x16, 0x04, 0x14, 0x5c, 0x0e, 0x30, 0xa8, 0x8e, 0x7f, 0xc9,
                0x02, 0xcd, 0xa8, 0xed, 0x0d, 0x1a, 0x1b, 0xd9, 0x7d, 0xe6, 0xce, 0x2a, 0x59, 0x30,
                0x1f, 0x06, 0x03, 0x55, 0x1d, 0x23, 0x04, 0x18, 0x30, 0x16, 0x80, 0x14, 0x5c, 0x0e,
                0x30, 0xa8, 0x8e, 0x7f, 0xc9, 0x02, 0xcd, 0xa8, 0xed, 0x0d, 0x1a, 0x1b, 0xd9, 0x7d,
                0xe6, 0xce, 0x2a, 0x59, 0x30, 0x0c, 0x06, 0x03, 0x55, 0x1d, 0x13, 0x04, 0x05, 0x30,
                0x03, 0x01, 0x01, 0xff, 0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04,
                0x03, 0x02, 0x03, 0x48, 0x00, 0x30, 0x45, 0x02, 0x21, 0x00, 0xf6, 0x79, 0xed, 0x69,
                0xd5, 0xe5, 0xba, 0x42, 0x14, 0xfc, 0xce, 0x47, 0xf1, 0x61, 0x1c, 0x51, 0x11, 0x2b,
                0xba, 0x04, 0x70, 0x56, 0x78, 0xaf, 0xa9, 0xa6, 0x98, 0x8f, 0x4b, 0xa8, 0x11, 0x67,
                0x02, 0x20, 0x3a, 0xdf, 0xf1, 0x74, 0xc9, 0x2f, 0xfb, 0x84, 0x46, 0xde, 0xbc, 0x2d,
                0xda, 0xe3, 0x05, 0xb4, 0x81, 0x31, 0x45, 0xf7, 0x3d, 0x71, 0x46, 0x07, 0xa7, 0xd8,
                0xcb, 0xae, 0x1e, 0x1b, 0x1c, 0x5a }, SIMPLE_CRT_LEN };

const ByteArray PMCsdkCertTest_btc::g_caPublicKey =
{ (uint8_t[]
        )
        { 0x8c, 0xc8, 0x92, 0x1d, 0xaa, 0x7f, 0xf0, 0xe4, 0xb2, 0x75, 0xd6, 0x4a, 0xf1, 0xd5, 0x14,
                0x3f, 0x1a, 0x09, 0xc5, 0x3e, 0x52, 0xd6, 0xda, 0xa0, 0xbf, 0x90, 0x43, 0xd1, 0x6b,
                0xfe, 0xd1, 0xb3, 0x75, 0x5c, 0xdd, 0x69, 0xac, 0x42, 0xa1, 0xcb, 0x03, 0x16, 0xee,
                0xa4, 0x30, 0xa5, 0x8d, 0x36, 0x8f, 0xc5, 0x7b, 0xb4, 0xb5, 0x6a, 0x7d, 0x9b, 0x16,
                0x04, 0x46, 0xab, 0xae, 0xbb, 0x56, 0xa1 }, PUBLIC_KEY_SIZE };

ByteArray PMCsdkCertTest_btc::g_serNum =
{ (uint8_t[SERIAL_NUMBER_MAX_LEN]
        )
        { 0 }, SERIAL_NUMBER_MAX_LEN };

/**
 * @since 2015-10-03
 * @see None
 * @objective Test OCRegisterPersistentStorageHandler with valid parameters
 * @target OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @test_data Regular data for OCRegisterPersistentStorageHandler
 * @pre_condition none
 * @procedure Call OCRegisterPersistentStorageHandler
 * @post_condition none
 * @expected OCRegisterPersistentStorageHandler will return OC_STACK_OK
 */
#if defined(__LINUX__)
TEST_F(PMCsdkCertTest_btc, RegisterPersistentStorage_P)
{
    SetPersistentHandler(&ps);

    if(OCRegisterPersistentStorageHandler(&ps) != OC_STACK_OK)
    SET_FAILURE("Failed");

    SUCCEED();
}
#endif

/**
 * @since 2015-10-03
 * @see None
 * @objective Check decoding predefined certificate positively
 * @target PKIError DecodeCertificate(ByteArray code, CertificateX509 *crt)
 * @test_data Regular data for DecodeCertificate
 * @pre_condition none
 * @procedure Call DecodeCertificate
 * @post_condition none
 * @expected DecodeCertificate returns PKI_SUCCESS
 */
#if defined(__LINUX__)
TEST_F(PMCsdkCertTest_btc, DecodeCertificate_P)
{
    ByteArray code = PMCsdkCertTest_btc::g_derCode;

    if(DecodeCertificate(code, &PMCsdkCertTest_btc::g_certificate) != PKI_SUCCESS)
    SET_FAILURE("Failed");

    SUCCEED();
}
#endif

/**
 * @since 2015-10-03
 * @see None
 * @objective Check decoding predefined certificate negatively
 * @target PKIError DecodeCertificate(ByteArray code, CertificateX509 *crt)
 * @test_data Regular data for DecodeCertificate
 * @pre_condition none
 * @procedure Call DecodeCertificate
 * @post_condition none
 * @expected DecodeCertificate doesn't return PKI_SUCCESS
 */
#if defined(__LINUX__)
TEST_F(PMCsdkCertTest_btc, DecodeCertificate_N)
{
    ByteArray code;
    code.data = NULL;

    if(DecodeCertificate(code, &PMCsdkCertTest_btc::g_certificate) == PKI_SUCCESS)
    SET_FAILURE("Failed");

    SUCCEED();
}
#endif

/**
 * @since 2015-10-03
 * @see None
 * @objective check decoding of random symbols sequence negatively
 * @target PKIError DecodeCertificate(ByteArray code, CertificateX509 *crt)
 * @test_data Regular data for DecodeCertificate
 * @pre_condition none
 * @procedure Call DecodeCertificate
 * @post_condition none
 * @expected DecodeCertificate doesn't return PKI_SUCCESS
 */
#if defined(__LINUX__)
TEST_F(PMCsdkCertTest_btc, RandomDecode_N)
{
    srand((unsigned int)time(NULL));
    ByteArray code;
    INIT_BYTE_ARRAY(code);

    for (unsigned int i = 0; i < RUNS; i++)
    {
        code.len = rand() % MAX_LEN;
        code.data = (uint8_t *)malloc(code.len * sizeof(uint8_t));

        if(code.data == (uint8_t *)NULL)
        {
            SET_FAILURE("Failed");
            break;
        }

        for (unsigned int j = 0; j < code.len; j++)
        code.data[j] = (uint8_t)(rand() % 128 + 1);

        if(DecodeCertificate(code, &PMCsdkCertTest_btc::g_certificate) == PKI_SUCCESS)
        SET_FAILURE("DecodeCertificate Failed");

        free(code.data);
    }
    SUCCEED();
}
#endif

/**
 * @since 2015-10-03
 * @see PKIError DecodeCertificate(ByteArray code, CertificateX509 *crt)
 * @objective validity check of predefined certificate negatively
 * @target PKIError CheckValidity(ByteArray dateFrom, ByteArray dateTo)
 * @test_data crt.validTo is less than crt.validFrom
 * @pre_condition none
 * @procedure 1. Call DecodeCertificate
 *            2. Call CheckValidity
 * @post_condition none
 * @expected CheckValidity returns PKI_CERT_DATE_INVALID
 */
#if defined(__LINUX__)
TEST_F(PMCsdkCertTest_btc, CheckValidity_N)
{
    CertificateX509 tempCrt;

    if(DecodeCertificate(PMCsdkCertTest_btc::g_derCode, &tempCrt) != PKI_SUCCESS)
    {
        SET_FAILURE("Failed");
        return;
    }

    ByteArray temp = tempCrt.validTo;
    tempCrt.validTo = tempCrt.validFrom;
    tempCrt.validFrom = temp;

    if(CheckValidity(tempCrt.validFrom, tempCrt.validTo) != PKI_CERT_DATE_INVALID)
    SET_FAILURE("CheckValidity Failed");

    SUCCEED();
}
#endif

/**
 * @since 2015-10-03
 * @see None
 * @objective Testing signature check of predefined certificate positively
 * @target PKIError CheckCertificate(ByteArray certDerCode, ByteArray caPublicKey)
 * @test_data Regular data for CheckCertificate
 * @pre_condition none
 * @procedure Call CheckCertificate
 * @post_condition none
 * @expected CheckCertificate returns PKI_SUCCESS
 */
#if defined(__LINUX__)
TEST_F(PMCsdkCertTest_btc, CheckSignature_P)
{
    ByteArray code = PMCsdkCertTest_btc::g_derCode;

    if(CheckCertificate(code, PMCsdkCertTest_btc::g_caPublicKey) != PKI_SUCCESS)
    SET_FAILURE("CheckCertificate Failed");

    SUCCEED();
}
#endif

/**
 * @since 2015-10-03
 * @see None
 * @objective Testing signature check of predefined certificate negatively
 * @target PKIError CheckCertificate(ByteArray certDerCode, ByteArray caPublicKey)
 * @test_data certDerCode = NULL
 * @pre_condition none
 * @procedure Call CheckCertificate
 * @post_condition none
 * @expected CheckCertificate doesn't return PKI_SUCCESS
 */
#if defined(__LINUX__)
TEST_F(PMCsdkCertTest_btc, CheckSignature_N)
{
    ByteArray code;
    code.data = NULL;

    if(CheckCertificate(code, PMCsdkCertTest_btc::g_caPublicKey) == PKI_SUCCESS)
    SET_FAILURE("CheckCertificate Failed");

    SUCCEED();
}
#endif

/**
 * @since 2015-10-03
 * @see None
 * @objective Test saving certificate into file
 * @target 1. PKIError InitCKMInfo(void)
 * @target 2. PKIError GenerateCAKeyPair (ByteArray *caPrivateKey, ByteArray *caPublicKey)
 * @target 3. PKIError SetSerialNumber (const long serNum)
 * @target 4. PKIError SetRootName (const ByteArray rootName)
 * @target 5. PKIError CKMIssueRootCertificate (const uint8_t *uint8NotBefore, const uint8_t *uint8NotAfter, ByteArray *issuedRootCertificate)
 * @target 6. PKIError CheckCertificate(ByteArray certDerCode, ByteArray caPublicKey)
 * @target 7. PKIError PKIError GenerateDERCertificateFile (const ByteArray *certificate, const char * const certFileName)
 * @target 8. PKIError PKIError CloseCKMInfo(void)
 * @test_data None
 * @pre_condition none
 * @procedure   1. Call InitCKMInfo
 *              2. Call GenerateCAKeyPair
 *              3. Call SetSerialNumber
 *              4. Call SetRootName
 *              5. Call CKMIssueRootCertificate
 *              6. Call CheckCertificate
 *              7. Call GenerateDERCertificateFile
 *              8. Call CloseCKMInfo
 * @post_condition none
 * @expected Saving certificate will be successful
 */
#if defined(__LINUX__)
TEST_F(PMCsdkCertTest_btc, DERCertificateFile_P)
{
    uint8_t derData[ISSUER_MAX_CERT_SIZE] =
    {   0};
    uint8_t caPubKey[PUBLIC_KEY_SIZE] =
    {   0};
    uint8_t caPrivKey[PRIVATE_KEY_SIZE] =
    {   0};

    ByteArray certDer = BYTE_ARRAY_INITIALIZER;
    ByteArray pubKeyIss = BYTE_ARRAY_INITIALIZER;
    ByteArray privKeyIss = BYTE_ARRAY_INITIALIZER;
    ByteArray rootName = BYTE_ARRAY_INITIALIZER;

    certDer.data = derData;
    certDer.len = ISSUER_MAX_CERT_SIZE;

    pubKeyIss.data = caPubKey;
    pubKeyIss.len = sizeof(caPubKey);
    privKeyIss.data = caPrivKey;
    privKeyIss.len = sizeof(caPrivKey);
    rootName.data = (uint8_t *)"ROOT1";
    rootName.len = strlen((char *)rootName.data);
    if(InitCKMInfo() != PKI_SUCCESS)
    {
        SET_FAILURE("Failed");
        return;
    }

    for (int i = 1; i <= RUNS; i++)
    {
        if(GenerateCAKeyPair(&privKeyIss, &pubKeyIss) != PKI_SUCCESS)
        {
            SET_FAILURE("Failed");
            return;
        }
        if(SetSerialNumber(i) != PKI_SUCCESS)
        {
            SET_FAILURE("Failed");
            return;
        }
        if(SetRootName(rootName) != PKI_SUCCESS)
        {
            SET_FAILURE("Failed");
            return;
        }
        if(CKMIssueRootCertificate(0, 0, &certDer) != PKI_SUCCESS)
        {
            SET_FAILURE("Failed");
            return;
        }
        if(CheckCertificate(certDer, pubKeyIss) != PKI_SUCCESS)
        {
            SET_FAILURE("Failed");
            return;
        }
        if(GenerateDERCertificateFile (&certDer, "der_cert") != PKI_SUCCESS)
        {
            SET_FAILURE("Failed");
            return;
        }
    }
    if(CloseCKMInfo() != PKI_SUCCESS)
    SET_FAILURE("Failed");

    SUCCEED();
}
#endif

/**
 * @since 2015-10-03
 * @see None
 * @objective Test checking time validity of generated certificate
 * @target      1. PKIError InitCKMInfo(void)
 * @target      2. PKIError GenerateCAKeyPair (ByteArray *caPrivateKey, ByteArray *caPublicKey)
 * @target      3. PKIError SetSerialNumber (const long serNum)
 * @target      4. PKIError SetRootName (const ByteArray rootName)
 * @target      5. PKIError CKMIssueRootCertificate (const uint8_t *uint8NotBefore, const uint8_t *uint8NotAfter, ByteArray *issuedRootCertificate)
 * @target      6. PKIError CheckCertificate(ByteArray certDerCode, ByteArray caPublicKey)
 * @target      7. PKIError PKIError CloseCKMInfo(void)
 * @test_data None
 * @pre_condition none
 * @procedure   1. Call InitCKMInfo
 *              2. Call GenerateCAKeyPair
 *              3. Call SetSerialNumber
 *              4. Call SetRootName
 *              5. Call CKMIssueRootCertificate
 *              6. Call CheckCertificate
 *              7. Call CKMIssueRootCertificate
 *              8. Call CheckCertificate
 *              9. Call CKMIssueRootCertificate
 *              10. Call CheckCertificate
 *              11. Call CloseCKMInfo
 * @post_condition none
 * @expected Saving certificate will be successful
 */
#if defined(__LINUX__)
TEST_F(PMCsdkCertTest_btc, TimeValidity_P)
{
    uint8_t derData[ISSUER_MAX_CERT_SIZE] =
    {   0};
    uint8_t caPubKey[PUBLIC_KEY_SIZE] =
    {   0};
    uint8_t caPrivKey[PRIVATE_KEY_SIZE] =
    {   0};

    ByteArray certDer = BYTE_ARRAY_INITIALIZER;
    ByteArray pubKey = BYTE_ARRAY_INITIALIZER;
    ByteArray privKey = BYTE_ARRAY_INITIALIZER;
    ByteArray rootName = BYTE_ARRAY_INITIALIZER;

    privKey.data = caPrivKey;
    privKey.len = sizeof(caPrivKey);

    certDer.data = derData;
    certDer.len = sizeof(derData);

    pubKey.data = caPubKey;
    pubKey.len = sizeof(caPubKey);

    rootName.data = (uint8_t *)"ROOT3";
    rootName.len = strlen((char *)rootName.data);
    if(InitCKMInfo() != PKI_SUCCESS)
    {
        SET_FAILURE("Failed");
        return;
    }
    for (int i = 1; i <= RUNS; i++)
    {
        if(GenerateCAKeyPair(&privKey, &pubKey) != PKI_SUCCESS)
        {
            SET_FAILURE("Failed");
            return;
        }
        if(SetSerialNumber(i) != PKI_SUCCESS)
        {
            SET_FAILURE("Failed");
            return;
        }
        if(SetRootName(rootName) != PKI_SUCCESS)
        {
            SET_FAILURE("Failed");
            return;
        }
        if(CKMIssueRootCertificate(0, 0, &certDer) != PKI_SUCCESS)
        {
            SET_FAILURE("Failed");
            return;
        }
        if(CheckCertificate(certDer, pubKey) != PKI_SUCCESS)
        {
            SET_FAILURE("Failed");
            return;
        }

        certDer.len = sizeof(derData);
        if(CKMIssueRootCertificate(0, (uint8_t *)"160101000000Z", &certDer) != PKI_SUCCESS)
        {
            SET_FAILURE("Failed");
            return;
        }

        if(CheckCertificate(certDer, pubKey) != PKI_CERT_DATE_INVALID)
        {
            SET_FAILURE("Failed");
            return;
        }

        certDer.len = sizeof(derData);
        if(CKMIssueRootCertificate((uint8_t *)"161001000000Z", 0, &certDer) != PKI_SUCCESS)
        {
            SET_FAILURE("Failed");
            return;
        }

        if(CheckCertificate(certDer, pubKey) != PKI_CERT_DATE_INVALID)
        {
            SET_FAILURE("Failed");
            return;
        }
    }
    if(CloseCKMInfo() != PKI_SUCCESS)
    SET_FAILURE("Failed");

    SUCCEED();
}
#endif

/**
 * @since 2015-10-03
 * @see None
 * @objective Testing certificate generation by certificate signing request
 * @target      1. PKIError InitCKMInfo(void)
 * @target      2. PKIError GenerateCAKeyPair (ByteArray *caPrivateKey, ByteArray *caPublicKey)
 * @target      3. PKIError SetSerialNumber (const long serNum)
 * @target      4. PKIError SetRootName (const ByteArray rootName)
 * @target      5. PKIError GenerateKeyPair (ByteArray *privateKey, ByteArray *publicKey)
 * @target      6. PKIError GenerateCSR (const uint8_t *uint8SubjectName, const uint8_t *uint8SubjectPublicKey, const uint8_t *uint8SubjectPrivateKey, ByteArray *encodedCSR);
 * @target      7. PKIError GenerateCertificateByCSR (const ByteArray *encodedCSR, ByteArray *issuedCertificate)
 * @target      8. PKIError CheckCertificate(ByteArray certDerCode, ByteArray caPublicKey)
 * @target      9. PKIError PKIError CloseCKMInfo(void)
 * @test_data None
 * @pre_condition none
 * @procedure   1. Call InitCKMInfo
 *              2. Call GenerateCAKeyPair
 *              3. Call SetSerialNumber
 *              4. Call SetRootName
 *              5. Call GenerateKeyPair
 *              6. Call GenerateCSR
 *              7. Call CKMIssueRootCertificate
 *              8. Call GenerateCertificateByCSR
 *              9. Call CKMIssueRootCertificate
 *              10. Call CheckCertificate
 *              11. Call CheckCertificate
 *              12. Call CheckCertificate
 *              13. Call CloseCKMInfo
 * @post_condition none
 * @expected Certificate generation by certificate signing request will be successful
 */
#if defined(__LINUX__)
TEST_F(PMCsdkCertTest_btc, CertificateSigningRequest_P)
{
    uint8_t certData[ISSUER_MAX_CERT_SIZE] =
    {   0};
    uint8_t csrData[CSR_MAX_SIZE] =
    {   0};
    uint8_t subjPubKey[PUBLIC_KEY_SIZE] =
    {   0};
    uint8_t subjPrivKey[PRIVATE_KEY_SIZE] =
    {   0};
    uint8_t caPubKey[PUBLIC_KEY_SIZE] =
    {   0};
    uint8_t caPrivKey[PRIVATE_KEY_SIZE] =
    {   0};
    uint8_t *subjName = (uint8_t *)"Subject05";

    ByteArray certDer = BYTE_ARRAY_INITIALIZER;
    ByteArray csrDer = BYTE_ARRAY_INITIALIZER;
    ByteArray pubKeyIss = BYTE_ARRAY_INITIALIZER;
    ByteArray privKeyIss = BYTE_ARRAY_INITIALIZER;
    ByteArray pubKeySubj = BYTE_ARRAY_INITIALIZER;
    ByteArray privKeySubj = BYTE_ARRAY_INITIALIZER;
    ByteArray rootName = BYTE_ARRAY_INITIALIZER;

    certDer.data = certData;
    certDer.len = sizeof(certData);
    csrDer.data = csrData;
    csrDer.len = CSR_MAX_SIZE;

    pubKeyIss.data = caPubKey;
    pubKeyIss.len = sizeof(caPubKey);
    privKeyIss.data = caPrivKey;
    privKeyIss.len = sizeof(caPrivKey);
    pubKeySubj.data = subjPubKey;
    pubKeySubj.len = sizeof(subjPubKey);
    privKeySubj.data = subjPrivKey;
    privKeySubj.len = sizeof(subjPrivKey);
    rootName.data = (uint8_t *)"ROOT2";
    rootName.len = strlen((char *)rootName.data);
    if(InitCKMInfo() != PKI_SUCCESS)
    SET_FAILURE("Failed");

    if(GenerateCAKeyPair(&privKeyIss, &pubKeyIss) != PKI_SUCCESS)
    {
        SET_FAILURE("Failed");
        return;
    }
    if(SetSerialNumber(1) != PKI_SUCCESS)
    {
        SET_FAILURE("Failed");
        return;
    }
    if(SetRootName(rootName) != PKI_SUCCESS)
    {
        SET_FAILURE("Failed");
        return;
    }

    for (int i = 1; i <= RUNS; i++)
    {
        if(GenerateKeyPair(&privKeySubj, &pubKeySubj) != PKI_SUCCESS)
        {
            SET_FAILURE("Failed");
            return;
        }
        if(GenerateCSR(subjName, subjPubKey, subjPrivKey, &csrDer) != PKI_SUCCESS)
        {
            SET_FAILURE("Failed");
            return;
        }
        if(GenerateCertificateByCSR(&csrDer, &certDer) != PKI_SUCCESS)
        {
            SET_FAILURE("Failed");
            return;
        }
        if(CheckCertificate(certDer, pubKeyIss) != PKI_SUCCESS)
        {
            SET_FAILURE("Failed");
            return;
        }
        certDer.data[0]++;
        if(CheckCertificate(certDer, pubKeyIss) == PKI_SUCCESS)
        {
            SET_FAILURE("Failed");
            return;
        }
        certDer.data[0]--;
        if(CheckCertificate(certDer, pubKeyIss) != PKI_SUCCESS)
        {
            SET_FAILURE("Failed");
            return;
        }
    }
    if(CloseCKMInfo() != PKI_SUCCESS)
    SET_FAILURE("Failed");

    SUCCEED();
}
#endif

/**
 * @since 2015-10-03
 * @see None
 * @objective Test public key structure parsing
 * @target      PKIError ParsePublicKey(ByteArray *caPublicKey)
 * @test_data None
 * @pre_condition none
 * @procedure   Call ParsePublicKey for different data
 * @post_condition none
 * @expected ParsePublicKey will return appropriate PKIError
 */
#if defined(__LINUX__)
TEST_F(PMCsdkCertTest_btc, ParsePublicKey_P)
{
    if(ParsePublicKey((ByteArray*)&PMCsdkCertTest_btc::g_caPublicKey) != PKI_SUCCESS)
    SET_FAILURE("Failed");

    size_t length = 3;
    uint8_t shortAr[length];
    ByteArray shortArray =
    {   shortAr, length};
    if(ParsePublicKey(&shortArray) != PKI_WRONG_ARRAY_LEN)
    {
        SET_FAILURE("Failed");
        return;
    }

    uint8_t uncompressed[PUBLIC_KEY_SIZE + 2];
    uncompressed[0] = 0;
    uncompressed[1] = ASN1_UNCOMPRESSED_KEY;
    memcpy(&uncompressed[2], PMCsdkCertTest_btc::g_caPublicKey.data, PUBLIC_KEY_SIZE);
    ByteArray uncomprArr =
    {   uncompressed, PUBLIC_KEY_SIZE+2};
    ParsePublicKey(&uncomprArr);
    if(uncomprArr.len != (size_t)PUBLIC_KEY_SIZE)
    {
        SET_FAILURE("Failed");
        return;
    }
    if(memcmp(uncomprArr.data, PMCsdkCertTest_btc::g_caPublicKey.data, PUBLIC_KEY_SIZE) != 0)
    SET_FAILURE("Failed");

    SUCCEED();
}
#endif

/**
 * @since 2015-10-03
 * @see None
 * @objective Test checking of certificate generated by OpenSSL
 * @target 1. PKIError DecodeCertificateList(ByteArray code, CertificateList *crl, ByteArray caPubKey)
 * @target 2. PKIError CheckCertificate(ByteArray certDerCode, ByteArray caPublicKey)
 * @test_data None
 * @pre_condition none
 * @procedure   1. Call DecodeCertificateList
 *              2. Call CheckCertificate
 * @post_condition none
 * @expected DecodeCertificateList and  CheckCertificate  return PKI_SUCCESS
 */
#if defined(__LINUX__)
TEST_F(PMCsdkCertTest_btc, VerifyOpenSslCertSign_P)
{
    ByteArray crtDer = BYTE_ARRAY_INITIALIZER;
    CertificateX509 certificate;

    FILE *fileCert = fopen("01.der", "rb");
    if(fileCert == NULL)
    {
        SET_FAILURE("Failed");
        return;
    }

    //get the length
    fseek(fileCert, 0, SEEK_END);
    crtDer.len = ftell(fileCert);
    fseek(fileCert, 0, SEEK_SET);
    //allocate memory
    crtDer.data = (uint8_t*)malloc(crtDer.len+1);
    //read the content
    if(fread(crtDer.data, crtDer.len, READ_WRITE_BLOCK_N, fileCert) != READ_WRITE_BLOCK_N)
    SET_FAILURE("Failed");
    fclose(fileCert);

    ByteArray pubKey = BYTE_ARRAY_INITIALIZER;
    FILE * fileKey = fopen("capub.der", "rb");
    if(fileKey == NULL)
    {
        SET_FAILURE("Failed");
        return;
    }

    fseek(fileKey, 0, SEEK_END);
    pubKey.len = ftell(fileKey);
    fseek(fileKey, 0, SEEK_SET);
    //openssl generates a public key that is longer than 64 bytes
    //with additional 27 bytes prepending the actual key
    if(pubKey.len > PUBLIC_KEY_SIZE)
    {
        fseek(fileKey, (pubKey.len - PUBLIC_KEY_SIZE), SEEK_SET);
        pubKey.len = PUBLIC_KEY_SIZE;
    }
    pubKey.data = (uint8_t*)malloc(pubKey.len+1);
    //read the content
    if(fread(pubKey.data, pubKey.len, READ_WRITE_BLOCK_N, fileKey) != READ_WRITE_BLOCK_N)
    SET_FAILURE("Failed");
    fclose(fileKey);

    if(DecodeCertificate(crtDer, &certificate) != PKI_SUCCESS)
    SET_FAILURE("Failed");
    if(CheckCertificate(crtDer, pubKey) != PKI_SUCCESS)
    SET_FAILURE("Failed");

    free(crtDer.data);
    free(pubKey.data);

    SUCCEED();
}
#endif

/**
 * @since 2015-10-03
 * @see None
 * @objective Test parsing of certificate chain generated by OpenSSL
 * @target 1. PKIError LoadCertificateChain (ByteArray msg, ByteArray *chain, uint8_t *chainLength)
 * @target 2. PKIError ParseCertificateChain (ByteArray *chainDerCode, CertificateX509 *chainCrt, uint8_t chainLen);
 * @test_data None
 * @pre_condition none
 * @procedure   1. Call LoadCertificateChain
 *              2. Call ParseCertificateChain
 * @post_condition none
 * @expected Parsing of certificate chain generated by OpenSSL will be successful
 */
#if defined(__LINUX__)
TEST_F(PMCsdkCertTest_btc, LoadCertificateChain_P)
{
    ByteArray crtChainDer[MAX_CHAIN_LEN] =
    {
        {   0,0},};
    CertificateX509 crtChain[MAX_CHAIN_LEN] =
    {
        {
            {   0,0},
            {   0,0},
            {   0,0},
            {   0,0},
            {   0,0},
            {   0,0},
            {   0,0},
            {   0,0},
            {   0,0}},};
    ByteArray msg = BYTE_ARRAY_INITIALIZER;
    uint8_t chainLength;

    FILE *file = fopen("cert_chain.dat", "rb");

    if(file == NULL)
    {
        SET_FAILURE("Failed");
        return;
    }

    while (!feof (file))
    {
        msg.data = (uint8_t *) realloc (msg.data, msg.len + 1);
        msg.data[msg.len] = fgetc (file);
        msg.len++;
    }
    msg.len--;
    fclose (file);
    INC_BYTE_ARRAY(msg, 3);

    if(LoadCertificateChain (msg, crtChainDer, &chainLength) != PKI_SUCCESS)
    SET_FAILURE("Failed");
    if(ParseCertificateChain (crtChainDer, crtChain, chainLength) != PKI_UNKNOWN_OID)
    SET_FAILURE("Failed");

    free(msg.data - 3);

    SUCCEED();
}
#endif

/**
 * @since 2015-10-03
 * @see None
 * @objective Test checking CA certificate generated by OpenSSL
 * @target PKIError DecodeCertificate(ByteArray code, CertificateX509 *crt)
 * @test_data None
 * @pre_condition none
 * @procedure Call DecodeCertificate
 * @post_condition none
 * @expected DecodeCertificate returns PKI_SUCCESS
 */
#if defined(__LINUX__)
TEST_F(PMCsdkCertTest_btc, OpenSSLCertificate_P)
{
    ByteArray crtDer = BYTE_ARRAY_INITIALIZER;
    FILE *fileCert = fopen("cacert.der", "rb");
    if(fileCert == NULL)
    {
        SET_FAILURE("Failed");
        return;
    }

    //get the length
    fseek(fileCert, 0, SEEK_END);
    crtDer.len = ftell(fileCert);
    fseek(fileCert, 0, SEEK_SET);
    //allocate memory
    crtDer.data = (uint8_t*)malloc(crtDer.len+1);
    //read the content
    if(fread(crtDer.data, crtDer.len, READ_WRITE_BLOCK_N, fileCert) != READ_WRITE_BLOCK_N)
    SET_FAILURE("Failed");

    fclose(fileCert);

    if(DecodeCertificate(crtDer, &PMCsdkCertTest_btc::g_certificate) != PKI_SUCCESS)
    SET_FAILURE("Failed");
    free(crtDer.data);

    SUCCEED();
}
#endif

/**
 * @since 2015-10-03
 * @see None
 * @objective Test signatures checking of certificate chain generated by OpenSSL
 * @target 1. PKIError LoadCertificateChain (ByteArray msg, ByteArray *chain, uint8_t *chainLength)
 * @target 2. PKIError ParseCertificateChain (ByteArray *chainDerCode, CertificateX509 *chainCrt, uint8_t chainLen);
 * @target 3. PKIError CheckCertificateChain (CertificateX509 *chainCrt, uint8_t chainLen, ByteArray caPubKey)
 * @test_data None
 * @pre_condition none
 * @procedure   1. Call LoadCertificateChain
 *              2. Call ParseCertificateChain
 *              3. Call CheckCertificateChain
 * @post_condition none
 * @expected Signatures checking of certificate chain generated by OpenSSL will be succesful
 */
#if defined(__LINUX__)
TEST_F(PMCsdkCertTest_btc, ParseAndCheckCertificateChain_P)
{
    ByteArray crtChainDer[MAX_CHAIN_LEN] =
    {
        {   0,0},};
    CertificateX509 crtChain[MAX_CHAIN_LEN] =
    {
        {
            {   0,0},
            {   0,0},
            {   0,0},
            {   0,0},
            {   0,0},
            {   0,0},
            {   0,0},
            {   0,0},
            {   0,0}},};
    ByteArray msg = BYTE_ARRAY_INITIALIZER;
    uint8_t chainLength;

    const char* chainPath =
    {   "chain.der"};
    FILE *fileChain = fopen(chainPath, "rb");
    if(fileChain == NULL)
    {
        SET_FAILURE("Failed");
        return;
    }

    //get the length
    fseek(fileChain, 0, SEEK_END);
    msg.len = ftell(fileChain);
    fseek(fileChain, 0, SEEK_SET);
    //allocate memory
    msg.data = (uint8_t*)malloc(msg.len+1);
    //read the content
    if(fread(msg.data, msg.len, READ_WRITE_BLOCK_N, fileChain) != READ_WRITE_BLOCK_N)
    SET_FAILURE("Failed");

    fclose (fileChain);

    INC_BYTE_ARRAY(msg, 3);
    if(LoadCertificateChain(msg, crtChainDer, &chainLength) != PKI_SUCCESS)
    SET_FAILURE("Failed");
    if(chainLength != 3)
    SET_FAILURE("Failed");

    if(ParseCertificateChain(crtChainDer, crtChain, chainLength) != PKI_SUCCESS)
    SET_FAILURE("Failed");

    ByteArray caPubKey = BYTE_ARRAY_INITIALIZER;

    const char* caPubKeyPath =
    {   "capub.der"};
    FILE *fileCaPubKey = fopen(caPubKeyPath, "rb");
    if(fileCaPubKey == NULL)
    {
        SET_FAILURE("Failed");
        return;
    }

    fseek(fileCaPubKey, 0, SEEK_END);
    caPubKey.len = ftell(fileCaPubKey);
    fseek(fileCaPubKey, 0, SEEK_SET);
    if(caPubKey.len > PUBLIC_KEY_SIZE)
    {
        fseek(fileCaPubKey, (caPubKey.len - PUBLIC_KEY_SIZE), SEEK_SET);
        caPubKey.len = PUBLIC_KEY_SIZE;
    }
    caPubKey.data = (uint8_t*)malloc(caPubKey.len+1);
    //read the content
    if(fread(caPubKey.data, caPubKey.len, READ_WRITE_BLOCK_N, fileCaPubKey) != READ_WRITE_BLOCK_N)
    SET_FAILURE("Failed");
    fclose(fileCaPubKey);

    if(CheckCertificateChain(crtChain, chainLength, caPubKey) != PKI_SUCCESS)
    SET_FAILURE("Failed");

    free(msg.data - 3);
    free(caPubKey.data);

    SUCCEED();
}
#endif

/**
 * @since 2015-10-03
 * @see None
 * @objective Testing correctness of decoding certificate length from ASN.1 structure
 * @target      PKIError DecodeLength(ByteArray *code, size_t *length)
 * @test_data None
 * @pre_condition none
 * @procedure   Call DecodeLength
 * @post_condition none
 * @expected Testing correctness of decoding certificate length from ASN.1 structure will be successful
 */
#if defined(__LINUX__)
TEST_F(PMCsdkCertTest_btc, CheckDecodeLength_P)
{
    ByteArray cert = BYTE_ARRAY_INITIALIZER;
    size_t length(0);
    if(DecodeLength(&cert, &length) != PKI_NULL_PASSED)
    SET_FAILURE("Failed");

    //a simple DER
    size_t derLength = (size_t)rand() % LEN_LONG;
    cert.len = derLength + 2;
    uint8_t *certData = (uint8_t*)malloc(cert.len);
    cert.data = certData;
    cert.data[0] = (uint8_t)0x30;//mixed types
    cert.data[1] = (uint8_t)(derLength & 0xff);
    if(DecodeLength(&cert, &length) != PKI_SUCCESS)
    SET_FAILURE("Failed");
    if(length != derLength)
    SET_FAILURE("Failed");
    free(certData);

    SUCCEED();
}
#endif

/**
 * @since 2015-10-03
 * @see None
 * @objective Testing serial number storage
 * @target      1. PKIError StoreSerialNumber(const ByteArray sn)
 * @target      2. PKIError CheckSerialNumber(const ByteArray sn)
 * @test_data None
 * @pre_condition none
 * @procedure   1. Call StoreSerialNumber
 *              2. Call CheckSerialNumber
 * @post_condition none
 * @expected Testing serial number storage will be successful
 */
#if defined(__LINUX__)
TEST_F(PMCsdkCertTest_btc, StoreSerialNumber_P)
{
    uint8_t data[10] =
    {   0x01, 0x82, 0x01, 0xd1, 0x30, 0x82, 0x01, 0x77, 0xa0, 0x03};
    const ByteArray sn =
    {   data, sizeof(data) / sizeof(uint8_t)};
    int i;
    for (i = 0; i < 400; i++)
    {
        sn.data[0] = i % 20;
        if(StoreSerialNumber(sn) != PKI_SUCCESS)
        {
            SET_FAILURE("Failed");
            return;
        }
    }
    if(CheckSerialNumber(sn) != PKI_CERT_REVOKED)
    {
        SET_FAILURE("Failed");
        return;
    }

    sn.data[1] = 0x01;
    if(CheckSerialNumber(sn) != PKI_SUCCESS)
    {
        SET_FAILURE("Failed");
        return;
    }

    FreeSNStore();

    SUCCEED();
}
#endif

/**
 * @since 2015-10-03
 * @see None
 * @objective Testing next certificate serial number handling by "CKM info" unit
 * @target     1. PKIError InitCKMInfo(void)
 * @target     2. PKIError SetSerialNumber (const long serNum)
 * @target     3. PKIError GetNextSerialNumber (long *nextSN)
 * @target     4. PKIError CloseCKMInfo(void)
 * @test_data None
 * @pre_condition none
 * @procedure   1. Call InitCKMInfo
 *              2. Call InitCKMInfo
 *              3. Call SetSerialNumber
 *              4. Call GetNextSerialNumber
 *              5. Call CloseCKMInfo
 * @post_condition none
 * @expected Testing next certificate serial number handling by "CKM info" unit will be successful
 */
#if defined(__LINUX__)
TEST_F(PMCsdkCertTest_btc, CAInitAndSerialNum_P)
{
    if(InitCKMInfo() != PKI_SUCCESS)
    {
        SET_FAILURE("Failed");
        return;
    }
    long serialNum = rand() % (MAX_LEN - 1) + 1;
    if(InitCKMInfo() != PKI_SUCCESS)
    {
        SET_FAILURE("Failed");
        return;
    }
    //all the serials should start from
    if(SetSerialNumber(serialNum) != PKI_SUCCESS)
    {
        SET_FAILURE("Failed");
        return;
    }
    long nextSerial;
    if(GetNextSerialNumber(&nextSerial) != PKI_SUCCESS)
    {
        SET_FAILURE("Failed");
        return;
    }
    if(serialNum != nextSerial)
    {
        SET_FAILURE("Failed");
        return;
    }
    if(CloseCKMInfo() != PKI_SUCCESS)
    {
        SET_FAILURE("Failed");
        return;
    }

    SUCCEED();
}
#endif

/**
 * @since 2015-10-03
 * @see None
 * @objective Testing CA name handling by "CKM info" unit
 * @target     1. PKIError InitCKMInfo(void)
 * @target     2. PKIError SetRootName (const ByteArray rootName)
 * @target     3. PKIError GetCAName (ByteArray *CAName)
 * @target     4. PKIError CloseCKMInfo(void)
 * @test_data None
 * @pre_condition none
 * @procedure   1. Call InitCKMInfo
 *              2. Call SetRootName
 *              3. Call GetCAName
 *              4. Call CloseCKMInfo
 * @post_condition none
 * @expected Testing CA name handling by "CKM info" unit will be succesful
 */
#if defined(__LINUX__)
TEST_F(PMCsdkCertTest_btc, CAName_P)
{
    ByteArray caName = BYTE_ARRAY_INITIALIZER;
    caName.len = ((size_t)rand() % (ISSUER_MAX_NAME_SIZE - 1) + 1);
    caName.data = (uint8_t*)malloc(caName.len);
    size_t i;
    for(i = 0; i < caName.len; i++)
    {
        caName.data[i] = (uint8_t)(rand() % 128);
    }
    if(InitCKMInfo() != PKI_SUCCESS)
    SET_FAILURE("Failed");
    if(SetRootName(caName) != PKI_SUCCESS)
    SET_FAILURE("Failed");
    ByteArray getName = BYTE_ARRAY_INITIALIZER;
    uint8_t uint8CAName[ISSUER_MAX_NAME_SIZE] =
    {   0};
    getName.data = uint8CAName;
    getName.len = ISSUER_MAX_NAME_SIZE;
    if(GetCAName(&getName) != PKI_SUCCESS)
    SET_FAILURE("Failed");
    if(memcmp(caName.data, getName.data, caName.len) != 0)
    SET_FAILURE("Failed");
    free(caName.data);
    if(CloseCKMInfo() != PKI_SUCCESS)
    {
        SET_FAILURE("Failed");
        return;
    }

    SUCCEED();
}
#endif

/**
 * @since 2015-10-03
 * @see None
 * @objective Testing CA name handling by "CKM info" unit
 * @target      1. PKIError InitCKMInfo(void)
 * @target      2. PKIError GenerateCAKeyPair (ByteArray *caPrivateKey, ByteArray *caPublicKey)
 * @target      3. PKIError GetCAPrivateKey (ByteArray *CAPrivateKey)
 * @target      4. PKIError GetCAPublicKey (ByteArray *CAPublicKey)
 * @target      5. PKIError GenerateKeyPair (ByteArray *privateKey, ByteArray *publicKey)
 * @target      6. PKIError CloseCKMInfo(void)
 * @test_data None
 * @pre_condition none
 * @procedure   1. Call InitCKMInfo
 *              2. Call SetRootName
 *              3. Call GetCAPrivateKey
 *              4. Call GetCAPublicKey
 *              5. Call GenerateKeyPair
 *              6. Call InitCKMInfo
 *              7. Call GetCAPrivateKey
 *              8. Call GetCAPublicKey
 *              9. Call CloseCKMInfo
 * @post_condition none
 * @expected Testing CA name handling by "CKM info" unit will be succesful
 */
#if defined(__LINUX__)
TEST_F(PMCsdkCertTest_btc, KeyPair_P)
{
    ByteArray rootName = BYTE_ARRAY_INITIALIZER;
    rootName.data = (uint8_t *)"ROOT";
    rootName.len = strlen((char *)rootName.data);
    SetRootName(rootName);

    //first test the GenerateCAKeyPair - this writes to the CA storage
    ByteArray privKeyIss = BYTE_ARRAY_INITIALIZER;
    privKeyIss.len = PRIVATE_KEY_SIZE;
    uint8_t caPrivKey[PRIVATE_KEY_SIZE] =
    {   0};
    privKeyIss.data = caPrivKey;

    ByteArray pubKeyIss = BYTE_ARRAY_INITIALIZER;
    pubKeyIss.len = PUBLIC_KEY_SIZE;
    uint8_t caPubKey[PUBLIC_KEY_SIZE] =
    {   0};
    pubKeyIss.data = caPubKey;

    if(InitCKMInfo() != PKI_SUCCESS)
    {
        SET_FAILURE("Failed");
        return;
    }
    if(GenerateCAKeyPair(&privKeyIss, &pubKeyIss) != PKI_SUCCESS)
    {
        SET_FAILURE("Failed");
        return;
    }

    ByteArray keyCheck = BYTE_ARRAY_INITIALIZER;
    keyCheck.len = PUBLIC_KEY_SIZE;
    uint8_t keyCheckData[PUBLIC_KEY_SIZE] =
    {   0};
    keyCheck.data = keyCheckData;
    if(GetCAPrivateKey(&keyCheck) != PKI_SUCCESS)
    {
        SET_FAILURE("Failed");
        return;
    }
    if(memcmp(keyCheck.data, privKeyIss.data, PRIVATE_KEY_SIZE) != 0)
    {
        SET_FAILURE("Failed");
        return;
    }

    if(GetCAPublicKey(&keyCheck) != PKI_SUCCESS)
    {
        SET_FAILURE("Failed");
        return;
    }
    if(memcmp(keyCheck.data, pubKeyIss.data, PUBLIC_KEY_SIZE) != 0)
    {
        SET_FAILURE("Failed");
        return;
    }

    //now test the GenerateKeyPair - does not write to the CA storage
    if(GenerateKeyPair(&privKeyIss, &pubKeyIss) != PKI_SUCCESS)
    {
        SET_FAILURE("Failed");
        return;
    }

    if(InitCKMInfo() != PKI_SUCCESS)
    {
        SET_FAILURE("Failed");
        return;
    }
    if(GetCAPrivateKey(&keyCheck) != PKI_SUCCESS)
    {
        SET_FAILURE("Failed");
        return;
    }
    if(memcmp(keyCheck.data, privKeyIss.data, PRIVATE_KEY_SIZE) == 0)
    {
        SET_FAILURE("Failed");
        return;
    }

    if(GetCAPublicKey(&keyCheck) != PKI_SUCCESS)
    {
        SET_FAILURE("Failed");
        return;
    }
    if(memcmp(keyCheck.data, pubKeyIss.data, PUBLIC_KEY_SIZE) == 0)
    {
        SET_FAILURE("Failed");
        return;
    }
    if(CloseCKMInfo() != PKI_SUCCESS)
    {
        SET_FAILURE("Failed");
        return;
    }

    SUCCEED();
}
#endif

/**
 * @since 2015-10-03
 * @see None
 * @objective Testing CRL encoding
 * @target      1. PKIError InitCKMInfo(void)
 * @target      2. PKIError SetRootName (const ByteArray rootName)
 * @target      3. PKIError GenerateCAKeyPair (ByteArray *caPrivateKey, ByteArray *caPublicKey)
 * @target      4. PKIError CKMIssueCRL (const uint8_t *uint8ThisUpdateTime, const uint32_t nuberOfRevoked, const uint32_t *revokedNumbers, const uint8_t **revocationDates, ByteArray *encodedCRL)
 * @target      5. PKIError DecodeCertificateList(ByteArray code, CertificateList *crl, ByteArray caPubKey)
 * @target      6. PKIError CloseCKMInfo(void)
 * @test_data None
 * @pre_condition none
 * @procedure   1. Call InitCKMInfo
 *              2. Call SetRootName
 *              3. Call GenerateCAKeyPair
 *              4. Call CKMIssueCRL
 *              5. Call DecodeCertificateList
 *              6. Call CloseCKMInfo
 * @post_condition none
 * @expected Testing CRL encoding will be succesful
 */
#if defined(__LINUX__)
TEST_F(PMCsdkCertTest_btc, EncodeCRL_P)
{
    CertificateList crl;

    uint8_t *uint8ThisUpdateTime = (uint8_t *)"130101000000Z";
    uint32_t numberOfRevoked = 0;
    uint32_t revokedNumbers[2];
    const uint8_t *revocationDates[2];

    ByteArray code = BYTE_ARRAY_INITIALIZER;
    ByteArray pubKeyIss = BYTE_ARRAY_INITIALIZER;
    ByteArray privKeyIss = BYTE_ARRAY_INITIALIZER;
    ByteArray rootName = BYTE_ARRAY_INITIALIZER;

    uint8_t caPubKey[PUBLIC_KEY_SIZE] =
    {   0};
    uint8_t caPrivKey[PRIVATE_KEY_SIZE] =
    {   0};

    pubKeyIss.data = caPubKey;
    pubKeyIss.len = PUBLIC_KEY_SIZE;
    privKeyIss.data = caPrivKey;
    privKeyIss.len = PRIVATE_KEY_SIZE;

    numberOfRevoked = 2;

    revokedNumbers[0] = 100; // serial number of first revoked certificate
    revokedNumbers[1] = 200;// serial number of second revoked certificate
    revocationDates[0] = (const uint8_t *)"130101000001Z";
    revocationDates[1] = (const uint8_t *)"130101000002Z";

    rootName.data = (uint8_t *)"ROOT2";
    rootName.len = strlen((char *)rootName.data);
    if(InitCKMInfo() != PKI_SUCCESS)
    {
        SET_FAILURE("Failed");
        return;
    }
    if(SetRootName(rootName) != PKI_SUCCESS)
    {
        SET_FAILURE("Failed");
        return;
    }
    if(GenerateCAKeyPair(&privKeyIss, &pubKeyIss) != PKI_SUCCESS)
    {
        SET_FAILURE("Failed");
        return;
    }

    code.data = (uint8_t *)calloc(1,
            (CRL_MIN_SIZE + numberOfRevoked * (sizeof(CertificateRevocationInfo_t) + 4)));
    code.len = (CRL_MIN_SIZE + numberOfRevoked * (sizeof(CertificateRevocationInfo_t) + 4));

    if(CKMIssueCRL(uint8ThisUpdateTime, numberOfRevoked, revokedNumbers, revocationDates,&code) != PKI_SUCCESS)
    SET_FAILURE("Failed");
    if(DecodeCertificateList (code, &crl, pubKeyIss) != PKI_SUCCESS)
    SET_FAILURE("Failed");

    FreeSNStore();
    free(code.data);
    if(CloseCKMInfo() != PKI_SUCCESS)
    {
        SET_FAILURE("Failed");
        return;
    }

    SUCCEED();
}
#endif

/**
 * @since 2015-10-03
 * @see None
 * @objective Check correctness of certificate revocation by CKMIssueCRL() and CKMRevocateCertificate()
 * @target      1. PKIError InitCKMInfo(void)
 * @target      4. PKIError SetRootName (const ByteArray rootName)
 * @target      5. PKIError GenerateCAKeyPair (ByteArray *caPrivateKey, ByteArray *caPublicKey)
 * @target      6. PKIError CKMIssueCRL (const uint8_t *uint8ThisUpdateTime, const uint32_t nuberOfRevoked, const uint32_t *revokedNumbers, const uint8_t **revocationDates, ByteArray *encodedCRL)
 * @target      7. PKIError DecodeCertificateList(ByteArray code, CertificateList *crl, ByteArray caPubKey)
 * @target      8. PKIError CKMRevocateCertificate (const uint8_t *uint8ThisUpdateTime, const long revokedNumber, ByteArray *encodedCRL)
 * @target      9. PKIError CloseCKMInfo(void)
 * @test_data None
 * @pre_condition none
 * @procedure   1. Call InitCKMInfo
 *              2. Call SetRootName
 *              3. Call GenerateCAKeyPair
 *              4. Call CKMIssueCRL
 *              5. Call DecodeCertificateList
 *              6. Call CKMRevocateCertificate
 *              7. Call DecodeCertificateList
 *              8. Call CloseCKMInfo
 * @post_condition none
 * @expected Certificate revocation by CKMIssueCRL() and CKMRevocateCertificate() will be successful
 */
#if defined(__LINUX__)
TEST_F(PMCsdkCertTest_btc, RevocateCertificate_P)
{
    CertificateList crl;

    uint8_t *uint8ThisUpdateTime = (uint8_t *)"130101000000Z";
    uint32_t numberOfRevoked = 0;
    uint32_t revokedNumbers[2];
    const uint8_t *revocationDates[2];

    ByteArray code = BYTE_ARRAY_INITIALIZER;
    ByteArray pubKeyIss = BYTE_ARRAY_INITIALIZER;
    ByteArray privKeyIss = BYTE_ARRAY_INITIALIZER;
    ByteArray rootName = BYTE_ARRAY_INITIALIZER;

    uint8_t caPubKey[PUBLIC_KEY_SIZE] =
    {   0};
    uint8_t caPrivKey[PRIVATE_KEY_SIZE] =
    {   0};

    pubKeyIss.data = caPubKey;
    pubKeyIss.len = sizeof(caPubKey);
    privKeyIss.data = caPrivKey;
    privKeyIss.len = sizeof(caPrivKey);

    numberOfRevoked = 2;

    revokedNumbers[0] = 100; // serial number of first revoked certificate
    revokedNumbers[1] = 200;// serial number of second revoked certificate
    revocationDates[0] = (const uint8_t *)"130101000001Z";
    revocationDates[1] = (const uint8_t *)"130101000002Z";

    rootName.data = (uint8_t *)"ROOT2";
    rootName.len = strlen((char *)rootName.data);
    if(InitCKMInfo() != PKI_SUCCESS)
    {
        SET_FAILURE("Failed");
        return;
    }
    if(SetRootName(rootName) != PKI_SUCCESS)
    {
        SET_FAILURE("Failed");
        return;
    }
    if(GenerateCAKeyPair(&privKeyIss, &pubKeyIss) != PKI_SUCCESS)
    {
        SET_FAILURE("Failed");
        return;
    }

    code.len = CRL_MIN_SIZE + numberOfRevoked * (sizeof(CertificateRevocationInfo_t) + 4);
    code.data = (uint8_t *)calloc(1, code.len);

    if(CKMIssueCRL (uint8ThisUpdateTime, numberOfRevoked, revokedNumbers, revocationDates, &code) != PKI_SUCCESS)
    SET_FAILURE("Failed");
    if(DecodeCertificateList (code, &crl, pubKeyIss) != PKI_SUCCESS)
    SET_FAILURE("Failed");
    free(code.data);
    numberOfRevoked++;
    code.len = CRL_MIN_SIZE + numberOfRevoked * (sizeof(CertificateRevocationInfo_t) + 4);
    code.data = (uint8_t *)calloc(1, code.len);
    if(CKMRevocateCertificate (uint8ThisUpdateTime, 50, &code) != PKI_SUCCESS)
    SET_FAILURE("Failed");
    if(DecodeCertificateList (code, &crl, pubKeyIss) != PKI_SUCCESS)
    SET_FAILURE("Failed");

    FreeSNStore();
    free(code.data);
    if(CloseCKMInfo() != PKI_SUCCESS)
    {
        SET_FAILURE("Failed");
        return;
    }

    SUCCEED();
}
#endif

/**
 * @since 2015-10-03
 * @see None
 * @objective Check correctness of saving root certificate to binary file
 * @target      1. PKIError InitCKMInfo(void)
 * @target      2. PKIError GenerateCAKeyPair (ByteArray *caPrivateKey, ByteArray *caPublicKey)
 * @target      3. PKIError SetSerialNumber (const long serNum)
 * @target      4. PKIError SetRootName (const ByteArray rootName)
 * @target      5. PKIError CKMIssueRootCertificate (const uint8_t *uint8NotBefore, const uint8_t *uint8NotAfter, ByteArray *issuedRootCertificate)
 * @target      6. PKIError CKMIssueCRL (const uint8_t *uint8ThisUpdateTime, const uint32_t nuberOfRevoked, const uint32_t *revokedNumbers, const uint8_t **revocationDates, ByteArray *encodedCRL)
 * @target      7. PKIError DecodeCertificateList(ByteArray code, CertificateList *crl, ByteArray caPubKey)
 * @target      8. PKIError GetCertificateRevocationList (ByteArray *certificateRevocationList)
 * @target      9. PKIError CloseCKMInfo(void)
 * @test_data None
 * @pre_condition none
 * @procedure   1. Call InitCKMInfo
 *              2. Call GenerateCAKeyPair
 *              3. Call SetSerialNumber
 *              4. Call SetRootName
 *              5. Call CKMIssueRootCertificate
 *              6. Call CKMIssueCRL
 *              7. Call DecodeCertificateList
 *              8. Call GetCertificateRevocationList
 *              9. Call DecodeCertificateList
 *              10. Call CloseCKMInfo
 * @post_condition none
 * @expected Saving root certificate to binary file will be successful
 */
#if defined(__LINUX__)
TEST_F(PMCsdkCertTest_btc, StoreCKMInfo_P)
{
    if(InitCKMInfo() != PKI_SUCCESS)
    {
        SET_FAILURE("Failed");
        return;
    }
    uint8_t derData[ISSUER_MAX_CERT_SIZE] =
    {   0};
    uint8_t caPubKey[PUBLIC_KEY_SIZE] =
    {   0};
    uint8_t caPrivKey[PRIVATE_KEY_SIZE] =
    {   0};
    const long serNum = 48598490;
    CertificateList crl;
    uint8_t *uint8ThisUpdateTime = (uint8_t *)"130101000000Z";
    uint32_t numberOfRevoked = 0;
    uint32_t revokedNumbers[2];
    const uint8_t *revocationDates[2];

    ByteArray certDer = BYTE_ARRAY_INITIALIZER;
    ByteArray pubKeyIss = BYTE_ARRAY_INITIALIZER;
    ByteArray privKeyIss = BYTE_ARRAY_INITIALIZER;
    ByteArray rootName = BYTE_ARRAY_INITIALIZER;
    ByteArray code = BYTE_ARRAY_INITIALIZER;

    certDer.data = derData;
    certDer.len = ISSUER_MAX_CERT_SIZE;
    pubKeyIss.data = caPubKey;
    pubKeyIss.len = PUBLIC_KEY_SIZE;
    privKeyIss.data = caPrivKey;
    privKeyIss.len = PRIVATE_KEY_SIZE;
    rootName.data = (uint8_t *)"ROOT";
    rootName.len = strlen((char *)rootName.data);

    //generate CA Certificate
    if(GenerateCAKeyPair(&privKeyIss, &pubKeyIss) != PKI_SUCCESS)
    {
        SET_FAILURE("Failed");
        return;
    }
    if(SetSerialNumber(serNum) != PKI_SUCCESS)
    {
        SET_FAILURE("Failed");
        return;
    }
    if(SetRootName(rootName) != PKI_SUCCESS)
    {
        SET_FAILURE("Failed");
        return;
    }
    if(CKMIssueRootCertificate(0, 0, &certDer) != PKI_SUCCESS)
    {
        SET_FAILURE("Failed");
        return;
    }

    //generate CRL
    numberOfRevoked = NUMBER_OF_REVOKED;

    revokedNumbers[0] = 100;// serial number of first revoked certificate
    revokedNumbers[1] = 200;// serial number of second revoked certificate
    revocationDates[0] = (const uint8_t *)"130101000001Z";
    revocationDates[1] = (const uint8_t *)"130101000002Z";

    code.data = (uint8_t *)calloc(1,
            (CRL_MIN_SIZE + numberOfRevoked * (sizeof(CertificateRevocationInfo_t) + 4)));
    code.len = (CRL_MIN_SIZE + numberOfRevoked * (sizeof(CertificateRevocationInfo_t) + 4));

    if(CKMIssueCRL (uint8ThisUpdateTime, numberOfRevoked, revokedNumbers,revocationDates, &code) != PKI_SUCCESS)
    {
        SET_FAILURE("Failed");
        return;
    }

    // Check Certificate file
    CertificateX509 certificate;
    ByteArray crtDer = BYTE_ARRAY_INITIALIZER;
    FILE *filePtr = fopen(CA_STORAGE_CRT_FILE , "rb");
    if(filePtr == NULL)
    {
        SET_FAILURE("Failed");
        return;
    }

    //get the length
    fseek(filePtr, 0, SEEK_END);
    crtDer.len = ftell(filePtr);
    fseek(filePtr, 0, SEEK_SET);
    //allocate memory
    crtDer.data = (uint8_t*)malloc(crtDer.len+1);
    //read the content
    if(fread(crtDer.data, crtDer.len, READ_WRITE_BLOCK_N, filePtr) != READ_WRITE_BLOCK_N)
    SET_FAILURE("Failed");
    fclose(filePtr);
    ByteArray crtCheck;
    crtCheck.data = crtDer.data + 3;//now file contains length of certificate
    crtCheck.len = crtDer.len - 3;
    if(DecodeCertificate(crtCheck, &certificate) != PKI_SUCCESS)
    SET_FAILURE("Failed");

    //check CRL
    ByteArray crlDer = BYTE_ARRAY_INITIALIZER;
    crlDer.len = (CRL_MIN_SIZE + numberOfRevoked * (sizeof(CertificateRevocationInfo_t) + 4));
    crlDer.data = (uint8_t *)malloc(crlDer.len);

    if(GetCertificateRevocationList(&crlDer) != PKI_SUCCESS)
    SET_FAILURE("Failed");

    if(DecodeCertificateList (crlDer, &crl, pubKeyIss) != PKI_SUCCESS)
    SET_FAILURE("Failed");

    if(CloseCKMInfo() != PKI_SUCCESS)
    SET_FAILURE("Failed");
    free(crlDer.data);
    free(code.data);
    free(crtDer.data);

    SUCCEED();
}
#endif

/**
 * @since 2015-10-03
 * @see None
 * @objective Check correctness of root device certificate generation
 * @target      1. PKIError InitCKMInfo(void)
 * @target      2. PKIError GenerateCAKeyPair (ByteArray *caPrivateKey, ByteArray *caPublicKey)
 * @target      3. PKIError SetSerialNumber (const long serNum)
 * @target      4. PKIError SetRootName (const ByteArray rootName)
 * @target      5. PKIError CKMIssueRootCertificate (const uint8_t *uint8NotBefore, const uint8_t *uint8NotAfter, ByteArray *issuedRootCertificate)
 * @target      6. PKIError CheckCertificate(ByteArray certDerCode, ByteArray caPublicKey) * @target      7. PKIError GenerateCertificateByCSR (const ByteArray *encodedCSR, ByteArray *issuedCertificate)
 * @target      7. PKIError CloseCKMInfo(void)
 * @test_data None
 * @pre_condition none
 * @procedure   1. Call InitCKMInfo
 *              2. Call GenerateCAKeyPair
 *              3. Call SetSerialNumber
 *              4. Call SetRootName
 *              5. Call CKMIssueRootCertificate
 *              6. Call CheckCertificate
 *              7. Call CheckCertificate
 *              8. Call CheckCertificate
 *              9. Call CloseCKMInfo
 * @post_condition none
 * @expected Root device certificate generation will be correct
 */
#if defined(__LINUX__)
TEST_F(PMCsdkCertTest_btc, GenerateRootCertificate_P)
{
    uint8_t derData[ISSUER_MAX_CERT_SIZE] =
    {   0};
    uint8_t caPubKey[PUBLIC_KEY_SIZE] =
    {   0};
    uint8_t caPrivKey[PRIVATE_KEY_SIZE] =
    {   0};

    ByteArray certDer = BYTE_ARRAY_INITIALIZER;
    ByteArray pubKeyIss = BYTE_ARRAY_INITIALIZER;
    ByteArray privKeyIss = BYTE_ARRAY_INITIALIZER;
    ByteArray rootName = BYTE_ARRAY_INITIALIZER;

    certDer.data = derData;
    certDer.len = sizeof(derData);

    pubKeyIss.data = caPubKey;
    pubKeyIss.len = sizeof(caPubKey);
    privKeyIss.data = caPrivKey;
    privKeyIss.len = sizeof(caPrivKey);
    rootName.data = (uint8_t *)"ROOT";
    rootName.len = strlen((char *)rootName.data);
    if(InitCKMInfo() != PKI_SUCCESS)
    {
        SET_FAILURE("Failed");
        return;
    }

    for (int i = 1; i <= RUNS; i++)
    {
        if(GenerateCAKeyPair(&privKeyIss, &pubKeyIss) != PKI_SUCCESS)
        {
            SET_FAILURE("Failed");
            return;
        }
        if(SetSerialNumber(i) != PKI_SUCCESS)
        {
            SET_FAILURE("Failed");
            return;
        }
        if(SetRootName(rootName) != PKI_SUCCESS)
        {
            SET_FAILURE("Failed");
            return;
        }
        if(CKMIssueRootCertificate(0, 0, &certDer) != PKI_SUCCESS)
        {
            SET_FAILURE("Failed");
            return;
        }

        if(CheckCertificate(certDer, pubKeyIss) != PKI_SUCCESS)
        {
            SET_FAILURE("Failed");
            return;
        }
        certDer.data[0]++;
        if(CheckCertificate(certDer, pubKeyIss) == PKI_SUCCESS)
        {
            SET_FAILURE("Failed");
            return;
        }

        certDer.data[0]--;
        if(CheckCertificate(certDer, pubKeyIss) != PKI_SUCCESS)
        {
            SET_FAILURE("Failed");
            return;
        }
    }
    if(CloseCKMInfo() != PKI_SUCCESS)
    {
        SET_FAILURE("Failed");
        return;
    }
    SUCCEED();
}
#endif

/**
 * @since 2015-10-03
 * @see None
 * @objective Check correctness of ordinal device certificate generation
 * @target      1. PKIError InitCKMInfo(void)
 * @target      2. PKIError GenerateCAKeyPair (ByteArray *caPrivateKey, ByteArray *caPublicKey)
 * @target      3. PKIError GenerateKeyPair (ByteArray *privateKey, ByteArray *publicKey)
 * @target      3. PKIError SetSerialNumber (const long serNum)
 * @target      4. PKIError SetRootName (const ByteArray rootName)
 * @target      5. PKIError CKMIssueRootCertificate (const uint8_t *uint8NotBefore, const uint8_t *uint8NotAfter, ByteArray *issuedRootCertificate)
 * @target      6. PKIError GenerateCSR (const uint8_t *uint8SubjectName, const uint8_t *uint8SubjectPublicKey, const uint8_t *uint8SubjectPrivateKey, ByteArray *encodedCSR);
 * @target      7. PKIError GenerateCertificateByCSR (const ByteArray *encodedCSR, ByteArray *issuedCertificate)
 * @target      8. PKIError CheckCertificate(ByteArray certDerCode, ByteArray caPublicKey)
 * @target      9. PKIError CloseCKMInfo(void)
 * @test_data None
 * @pre_condition none
 * @procedure   1. Call InitCKMInfo
 *              2. Call GenerateCAKeyPair
 *              3. Call SetSerialNumber
 *              4. Call SetRootName
 *              5. Call GenerateKeyPair
 *              6. Call GenerateCSR
 *              7. Call CKMIssueRootCertificate
 *              8. Call GenerateCertificateByCSR
 *              9. Call CKMIssueRootCertificate
 *              10. Call CheckCertificate
 *              11. Call CheckCertificate
 *              12. Call CheckCertificate
 *              13. Call CloseCKMInfo
 * @post_condition none
 * @expected Certificate generation by certificate signing request will be successful
 */
#if defined(__LINUX__)
TEST_F(PMCsdkCertTest_btc, GenerateDeviceCertificate_P)
{
    uint8_t derData[ISSUER_MAX_CERT_SIZE] =
    {   0};
    uint8_t subjPubKey[PUBLIC_KEY_SIZE] =
    {   0};
    uint8_t subjPrivKey[PRIVATE_KEY_SIZE] =
    {   0};
    uint8_t caPubKey[PUBLIC_KEY_SIZE] =
    {   0};
    uint8_t caPrivKey[PRIVATE_KEY_SIZE] =
    {   0};
    uint8_t *subjName = (uint8_t *)"Subject Name";

    ByteArray certDer = BYTE_ARRAY_INITIALIZER;
    ByteArray pubKeyIss = BYTE_ARRAY_INITIALIZER;
    ByteArray privKeyIss = BYTE_ARRAY_INITIALIZER;
    ByteArray pubKeySubj = BYTE_ARRAY_INITIALIZER;
    ByteArray privKeySubj = BYTE_ARRAY_INITIALIZER;
    ByteArray rootName = BYTE_ARRAY_INITIALIZER;

    certDer.data = derData;
    certDer.len = ISSUER_MAX_CERT_SIZE;

    pubKeyIss.data = caPubKey;
    pubKeyIss.len = sizeof(caPubKey);
    privKeyIss.data = caPrivKey;
    privKeyIss.len = sizeof(caPrivKey);
    pubKeySubj.data = subjPubKey;
    pubKeySubj.len = sizeof(subjPubKey);
    privKeySubj.data = subjPrivKey;
    privKeySubj.len = sizeof(subjPrivKey);
    rootName.data = (uint8_t *)"ROOT2";
    rootName.len = strlen((char *)rootName.data);
    if(InitCKMInfo() != PKI_SUCCESS)
    {
        SET_FAILURE("Failed");
        return;
    }

    ASSERT_EQ(GenerateCAKeyPair(&privKeyIss, &pubKeyIss), PKI_SUCCESS);
    for (int i = 1; i <= RUNS; i++)
    {
        if(GenerateKeyPair(&privKeySubj, &pubKeySubj) != PKI_SUCCESS)
        {
            SET_FAILURE("Failed");
            return;
        }
        if(SetSerialNumber(i) != PKI_SUCCESS)
        {
            SET_FAILURE("Failed");
            return;
        }
        if(SetRootName(rootName) != PKI_SUCCESS)
        {
            SET_FAILURE("Failed");
            return;
        }
        if(CKMIssueDeviceCertificate(subjName, 0, 0, subjPubKey, &certDer) != PKI_SUCCESS)
        {
            SET_FAILURE("Failed");
            return;
        }

        if(CheckCertificate(certDer, pubKeyIss) != PKI_SUCCESS)
        {
            SET_FAILURE("Failed");
            return;
        }
        certDer.data[0]++;
        if(CheckCertificate(certDer, pubKeyIss) == PKI_SUCCESS)
        {
            SET_FAILURE("Failed");
            return;
        }

        certDer.data[0]--;
        if(CheckCertificate(certDer, pubKeyIss) != PKI_SUCCESS)
        {
            SET_FAILURE("Failed");
            return;
        }
    }
    if(CloseCKMInfo() != PKI_SUCCESS)
    SET_FAILURE("Failed");
    SUCCEED();
}
#endif
