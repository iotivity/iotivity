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

#include <oic_malloc.h>
#include <gtest/gtest.h>
#include "crl_generator.h"

static const ByteArray privateKey = {(uint8_t[])
{
	0xd6, 0xc8, 0x92, 0x16, 0x36, 0x7f, 0xf0, 0xe4, 0xb2, 0x75, 0xd6, 0x4a, 0xf1, 0x3f, 0x14, 0x30,
    0x1a, 0x69, 0xc5, 0x3e, 0x52, 0xd6, 0xda, 0xa0, 0xbf, 0xae, 0x43, 0xd1, 0x6b, 0xfe, 0xd1, 0x36
}, PRIVATE_KEY_SIZE };

//GenerateCRL test
TEST(CRLGeneratorTest, GenerateCRL)
{
    uint8_t *uint8ThisUpdateTime = (uint8_t *)"130101000000Z";
    uint32_t numberOfRevoked = 2;
    uint32_t revokedNumbers[2];
    const uint8_t *revocationDates[2];
    CertificateRevocationInfo_t *certificateRevocationInfo = NULL;
    UTF8String_t *issuerName = NULL;
    UTCTime_t *thisUpdateTime = NULL;
    ByteArray encodedCRL = BYTE_ARRAY_INITIALIZER;
    BIT_STRING_t issuerPrivateKey;

    revokedNumbers[0] = 100; // serial number of first revoked certificate
    revokedNumbers[1] = 200; // serial number of second revoked certificate

    revocationDates[0] = (const uint8_t *)"130101000001Z";
    revocationDates[1] = (const uint8_t *)"130101000002Z";

    certificateRevocationInfo = (CertificateRevocationInfo_t *)OICCalloc(numberOfRevoked,
            sizeof(CertificateRevocationInfo_t));
    EXPECT_TRUE(NULL != certificateRevocationInfo);

    issuerName = (UTF8String_t *)OICCalloc(1, sizeof(UTF8String_t));
    EXPECT_TRUE(NULL != issuerName);

    thisUpdateTime = (UTCTime_t *)OICCalloc(1, sizeof(UTCTime_t));
    EXPECT_TRUE(NULL != thisUpdateTime);

    encodedCRL.data = (uint8_t *)OICCalloc(1,
                (CRL_MIN_SIZE + numberOfRevoked * (sizeof(CertificateRevocationInfo_t) + 4)));
    EXPECT_TRUE(NULL != encodedCRL.data);
    encodedCRL.len = (CRL_MIN_SIZE + numberOfRevoked * (sizeof(CertificateRevocationInfo_t) + 4));

    issuerPrivateKey.size = PRIVATE_KEY_SIZE + 1;
    issuerPrivateKey.buf = (uint8_t *)OICCalloc((issuerPrivateKey.size), sizeof(uint8_t));
    EXPECT_TRUE(NULL != issuerPrivateKey.buf);

    if(NULL != certificateRevocationInfo && NULL != issuerName &&
            NULL != thisUpdateTime && NULL != encodedCRL.data && NULL != issuerPrivateKey.buf)
    {
        ByteArray issName = BYTE_ARRAY_INITIALIZER;
        issName.data = (uint8_t *)"Issuer";
        issName.len = strlen((char *)issName.data);

        issuerName->buf  = issName.data;
        issuerName->size = issName.len;

        for (size_t i = 0; i < numberOfRevoked; i++ )
        {
            certificateRevocationInfo[i].userCertificate = revokedNumbers[i];
            certificateRevocationInfo[i].revocationDate.buf = (uint8_t *)revocationDates[i];
            certificateRevocationInfo[i].revocationDate.size =
                     strlen((const char *)revocationDates[i]);
        }

        memcpy((issuerPrivateKey.buf) + 1, privateKey.data, PRIVATE_KEY_SIZE);

        EXPECT_EQ(PKI_SUCCESS, GenerateCRL(issuerName, thisUpdateTime, numberOfRevoked,
                certificateRevocationInfo, &issuerPrivateKey, &encodedCRL));
    }

    OICFree(certificateRevocationInfo);
    OICFree(issuerName);
    OICFree(thisUpdateTime);
}

