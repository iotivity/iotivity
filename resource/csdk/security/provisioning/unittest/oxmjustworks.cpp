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
#include "../src/oxmjustworks.c"
#include "tools.h"

#undef TAG
#ifdef __cplusplus
}
#endif

#define TAG "OXMJW_UNITTEST"

#define SVR_DB_FILE_NAME "oic_oxmjustworks.dat"
#define PM_DB_FILE_NAME "oic_oxmjustworks.db"

using namespace std;

class OXMJW : public ::testing::Test
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

        OCProvisionDev_t *g_ownedDevices = NULL;
};

TEST_F(OXMJW, CreateMVJustWorksSelectOxmPayload)
{
    uint8_t *payload = NULL;
    size_t size = 0;
    bool propertiesToInclude[DOXM_PROPERTY_COUNT];
    memset(propertiesToInclude, 0, sizeof(propertiesToInclude));
    propertiesToInclude[DOXM_OXMS] = true;
    propertiesToInclude[DOXM_OXMSEL] = true;
    propertiesToInclude[DOXM_SCT] = true;
    propertiesToInclude[DOXM_OWNED] = true;
    propertiesToInclude[DOXM_DEVICEUUID] = true;
    propertiesToInclude[DOXM_DEVOWNERUUID] = true;

    OTMContext_t *otmCtx = createOTMContext();

    EXPECT_EQ(OC_STACK_OK, CreateMVJustWorksSelectOxmPayload(otmCtx, &payload, &size));
    EXPECT_TRUE(payload != NULL);

    freeOTMContext(otmCtx);
}

