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
#include "../src/oxmmanufacturercert.c"
#include "tools.h"

#undef TAG
#ifdef __cplusplus
}
#endif

#define TAG "OXMMCERT_UNITTEST"

#define SVR_DB_FILE_NAME TAG".dat"
#define PM_DB_FILE_NAME TAG".db"

using namespace std;

class OXMMCERT : public ::testing::Test
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

TEST_F(OXMMCERT, CreateMCertificateBasedSelectOxmPayload)
{
    OTMContext_t *ctx = createOTMContext();
    uint8_t *payload = NULL;
    size_t size = 0;
    EXPECT_EQ(OC_STACK_OK, CreateMCertificateBasedSelectOxmPayload(ctx, &payload, &size));
    freeOTMContext(ctx);
}

TEST_F(OXMMCERT, CreateConMCertificateBasedSelectOxmPayload)
{
    OTMContext_t *ctx = createOTMContext();
    uint8_t *payload = NULL;
    size_t size = 0;
    EXPECT_EQ(OC_STACK_INVALID_PARAM, CreateConMCertificateBasedSelectOxmPayload(NULL, &payload,
              &size));
    EXPECT_EQ(OC_STACK_OK, CreateConMCertificateBasedSelectOxmPayload(ctx, &payload, &size));
    freeOTMContext(ctx);
}

TEST_F(OXMMCERT, CreateMCertificateBasedOwnerTransferPayload)
{
    OTMContext_t *ctx = createOTMContext();
    uint8_t *payload = NULL;
    size_t size = 0;
    EXPECT_EQ(OC_STACK_OK, CreateMCertificateBasedOwnerTransferPayload(ctx, &payload, &size));
    freeOTMContext(ctx);
}

TEST_F(OXMMCERT, PrepareMCertificateCallback)
{
    OTMContext_t *ctx = createOTMContext();
    EXPECT_EQ(OC_STACK_OK, PrepareMCertificateCallback(ctx));
    freeOTMContext(ctx);
}

TEST_F(OXMMCERT, CreateSecureSessionMCertificateCallback)
{
    OTMContext_t *ctx = createOTMContext();
    EXPECT_EQ(OC_STACK_OK, CreateSecureSessionMCertificateCallback(ctx));
    freeOTMContext(ctx);
}

