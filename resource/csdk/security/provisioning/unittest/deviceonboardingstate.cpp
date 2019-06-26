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
#include "../src/deviceonboardingstate.c"
#include "tools.h"

#undef TAG
#ifdef __cplusplus
}
#endif

#define TAG "DOBT_UNITTEST"

#define SVR_DB_FILE_NAME "oic_dobt.dat"
#define PM_DB_FILE_NAME "oic_dovt.db"

using namespace std;

class DOBT : public ::testing::Test
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

TEST_F(DOBT, DoStateChange)
{
    EXPECT_EQ(OC_STACK_OK, DoStateChange(DOS_RESET));
    EXPECT_EQ(OC_STACK_OK, DoStateChange(DOS_RFOTM));
    EXPECT_EQ(OC_STACK_FORBIDDEN_REQ, DoStateChange(DOS_RFPRO));
    EXPECT_EQ(OC_STACK_FORBIDDEN_REQ, DoStateChange(DOS_RFNOP));
    EXPECT_EQ(OC_STACK_FORBIDDEN_REQ, DoStateChange(DOS_SRESET));

    OicUuid_t *owner = createUuid();

    EXPECT_EQ(OC_STACK_OK, SetDoxmSelfOwnership(owner));

    EXPECT_EQ(OC_STACK_OK, DoStateChange(DOS_RESET));
    EXPECT_EQ(OC_STACK_OK, DoStateChange(DOS_RFOTM));
    EXPECT_EQ(OC_STACK_FORBIDDEN_REQ, DoStateChange(DOS_RFPRO));
    EXPECT_EQ(OC_STACK_FORBIDDEN_REQ, DoStateChange(DOS_RFNOP));
    EXPECT_EQ(OC_STACK_FORBIDDEN_REQ, DoStateChange(DOS_SRESET));

    freeUuid(owner);
}

