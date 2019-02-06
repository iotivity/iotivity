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

#include "ocprovisioningmanager.h"
#include "../src/credentialgenerator.c"
#include "tools.h"

#undef TAG
#ifdef __cplusplus
}
#endif

#undef TAG
#define TAG "CG_UNITTEST"

#define SVR_DB_FILE_NAME TAG".dat"
#define PM_DB_FILE_NAME TAG".db"

using namespace std;

class CG : public ::testing::Test
{
    public:
        static void SetUpTestCase()
        {
            IOT_Init(PM_DB_FILE_NAME);
        }

        static void TearDownTestCase()
        {
            IOT_DeInit(PM_DB_FILE_NAME);
        }

};

TEST_F(CG, PMGeneratePairWiseCredentialsInvalidFirstDevID)
{
    OicUuid_t *SecondDevID = (OicUuid_t *)OICMalloc(sizeof(OicUuid_t));
    if (SecondDevID)
    {
        SecondDevID->id[0] = 2;
    }
    OicSecCred_t *cred1 = NULL;
    OicSecCred_t *cred2 = NULL;
    size_t keySize = OWNER_PSK_LENGTH_128;
    EXPECT_EQ(OC_STACK_INVALID_PARAM, PMGeneratePairWiseCredentials(NO_SECURITY_MODE,
              keySize, NULL, SecondDevID, NULL, NULL, &cred1, &cred2));
    OICFree(SecondDevID);
}

TEST_F(CG, PMGeneratePairWiseCredentialsInvalidSecondDevID)
{
    OicUuid_t *firstDevID = (OicUuid_t *)OICMalloc(sizeof(OicUuid_t));
    if (firstDevID)
    {
        firstDevID->id[0] = 1;
    }
    OicUuid_t *provisioningDevID = (OicUuid_t *)OICMalloc(sizeof(OicUuid_t));
    if (provisioningDevID)
    {
        provisioningDevID->id[0] = 2;
    }
    OicSecCred_t *cred1 = NULL;
    OicSecCred_t *cred2 = NULL;
    size_t keySize = OWNER_PSK_LENGTH_128;
    EXPECT_EQ(OC_STACK_INVALID_PARAM, PMGeneratePairWiseCredentials(NO_SECURITY_MODE, keySize,
              firstDevID, NULL, NULL, NULL, &cred1, &cred2));
    OICFree(firstDevID);
    OICFree(provisioningDevID);
}

TEST_F(CG, PMGeneratePairWiseCredentialsInvalidCred)
{
    OicUuid_t *firstDevID = (OicUuid_t *)OICMalloc(sizeof(OicUuid_t));
    if (firstDevID)
    {
        firstDevID->id[0] = 1;
    }
    OicUuid_t *SecondDevID = (OicUuid_t *)OICMalloc(sizeof(OicUuid_t));
    if (SecondDevID)
    {
        SecondDevID->id[0] = 2;
    }
    OicUuid_t *provisioningDevID = (OicUuid_t *)OICMalloc(sizeof(OicUuid_t));
    if (provisioningDevID)
    {
        provisioningDevID->id[0] = 3;
    }
    size_t keySize = OWNER_PSK_LENGTH_128;
    EXPECT_EQ(OC_STACK_INVALID_PARAM, PMGeneratePairWiseCredentials(NO_SECURITY_MODE, keySize,
              firstDevID, SecondDevID, NULL, NULL, NULL, NULL));
    OICFree(firstDevID);
    OICFree(SecondDevID);
    OICFree(provisioningDevID);
}

TEST_F(CG, PMGeneratePairWiseCredentialsFull)
{
    OicUuid_t *firstDevID = (OicUuid_t *)OICMalloc(sizeof(OicUuid_t));
    EXPECT_TRUE(NULL != firstDevID);
    ConvertStrToUuid("11111111-1234-1234-1234-123456789011", firstDevID);
    OicUuid_t *secondDevID = (OicUuid_t *)OICMalloc(sizeof(OicUuid_t));
    EXPECT_TRUE(NULL != secondDevID);
    ConvertStrToUuid("11111111-1234-1234-1234-123456789012", secondDevID);
    OicSecRole_t *firstRole = (OicSecRole_t *)OICMalloc(sizeof(OicSecRole_t));
    OicSecRole_t *secondRole = (OicSecRole_t *)OICMalloc(sizeof(OicSecRole_t));
    OicSecCred_t *firstCred = NULL;
    OicSecCred_t *secondCred = NULL;

    EXPECT_EQ(OC_STACK_OK, PMGeneratePairWiseCredentials(SYMMETRIC_PAIR_WISE_KEY, OWNER_PSK_LENGTH_256,
              firstDevID, secondDevID, firstRole, secondRole, &firstCred, &secondCred));

    OICFree(firstCred->privateData.data);
    OICFree(secondCred->privateData.data);
    OICFree(secondCred);
    OICFree(firstCred);
    OICFree(secondRole);
    OICFree(firstRole);
    OICFree(firstDevID);
    OICFree(secondDevID);
}
