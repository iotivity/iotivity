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

 ******************************************************************/
#include <gtest/gtest.h>
#include "ck_manager.h"
#include "crl_generator.h"
#include "pki.h"
#include "oic_malloc.h"
#include "ckm_info.h"
#define ASN1_UNCOMPRESSED_KEY_ID   (0x04)

//EncodeDecodeCSR test
TEST(CSRGeneratorTest, EncodeDecodeCSR)
{
    uint8_t csrData[CSR_MAX_SIZE] = {0};
    uint8_t subjPubKey[PUBLIC_KEY_SIZE] = {0};
    uint8_t subjPrivKey[PRIVATE_KEY_SIZE] = {0};
    ByteArray pubKeySubj = BYTE_ARRAY_INITIALIZER;
    ByteArray privKeySubj = BYTE_ARRAY_INITIALIZER;
    uint8_t *subjName = (uint8_t *)"Subject05";
    UTF8String_t* subjectName = NULL;
    BIT_STRING_t* subjectPublicKey = NULL;
    BIT_STRING_t* subjectPrivateKey = NULL;
    ByteArray csrDer = BYTE_ARRAY_INITIALIZER;
    BIT_STRING_t* subjectPublicKeyOut = NULL;
    UTF8String_t* subjectNameOut = NULL;
    uint8_t uint8SubjectName[ISSUER_MAX_NAME_SIZE];
    uint8_t uint8SubjectPublicKey[PUBLIC_KEY_SIZE + 1];

    csrDer.data = csrData;
    csrDer.len = CSR_MAX_SIZE;

    pubKeySubj.data = subjPubKey;
    pubKeySubj.len = sizeof(subjPubKey);
    privKeySubj.data = subjPrivKey;
    privKeySubj.len = sizeof(subjPrivKey);

    ASSERT_EQ(PKI_SUCCESS, GenerateKeyPair(&privKeySubj, &pubKeySubj));

    subjectName = (UTF8String_t*)OICCalloc(1, sizeof(UTF8String_t));
    EXPECT_TRUE(NULL != subjectName);

    subjectPublicKey = (BIT_STRING_t*)OICCalloc(1, sizeof(BIT_STRING_t));
    EXPECT_TRUE(NULL != subjectPublicKey);

    subjectPrivateKey = (BIT_STRING_t*)OICCalloc(1, sizeof(BIT_STRING_t));
    EXPECT_TRUE(NULL != subjectPrivateKey);

    if(NULL != subjectName && NULL != subjectPublicKey && NULL != subjectPrivateKey)
    {
        subjectName->buf  = (uint8_t *)subjName;
        subjectName->size = strlen((const char *)subjectName->buf);

        subjectPrivateKey->size = PRIVATE_KEY_SIZE + 1;
        subjectPrivateKey->buf = (uint8_t *)OICCalloc((subjectPrivateKey->size), sizeof(uint8_t));
        EXPECT_TRUE(NULL != subjectPrivateKey->buf);

        subjectPublicKey->size = PUBLIC_KEY_SIZE + 1;
        subjectPublicKey->buf = (uint8_t *)OICCalloc(subjectPublicKey->size, sizeof(uint8_t));
        EXPECT_TRUE(NULL != subjectPublicKey->buf);

        if(NULL!= subjectPublicKey->buf && NULL != subjectPrivateKey->buf)
        {
            memcpy((subjectPrivateKey->buf) + 1, subjPrivKey, PRIVATE_KEY_SIZE);
            memcpy((subjectPublicKey->buf) + 1, subjPubKey, PUBLIC_KEY_SIZE);

            EXPECT_EQ(PKI_SUCCESS, EncodeCSR(subjectName, subjectPublicKey, subjectPrivateKey, &csrDer));
        }
    }

    subjectPublicKeyOut = (BIT_STRING_t*)OICCalloc(1, sizeof(BIT_STRING_t));
    EXPECT_TRUE(NULL != subjectPublicKey);

    subjectNameOut = (UTF8String_t*)OICCalloc(1, sizeof(UTF8String_t));
    EXPECT_TRUE(NULL != subjectNameOut);

    if(NULL != subjectNameOut && NULL != subjectPublicKeyOut)
    {
        subjectNameOut->buf = uint8SubjectName;
        subjectPublicKeyOut->buf = uint8SubjectPublicKey;
        EXPECT_EQ(PKI_SUCCESS, DecodeCSR(&csrDer, subjectNameOut, subjectPublicKeyOut));
    }

    OICFree(subjectName);
    OICFree(subjectPublicKey);
    OICFree(subjectNameOut);
    OICFree(subjectPublicKeyOut);
    if (subjectPrivateKey)
    {
        OICFree(subjectPrivateKey->buf);
        OICFree(subjectPrivateKey);
    }
}
