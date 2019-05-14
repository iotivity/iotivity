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
#include "../src/deviceonboardingstate.c"

#ifdef __cplusplus
}
#endif

#ifdef TAG
#undef TAG
#endif

#define TAG  "DOS"

#define SVR_DB_FILE_NAME TAG".dat"
#define PM_DB_FILE_NAME TAG".db"

class DOS : public ::testing::Test
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

TEST_F(DOS, IsReadyToEnterSRESET)
{
    EXPECT_FALSE(IsReadyToEnterSRESET());
}

TEST_F(DOS, EnterSRESET)
{
    EXPECT_TRUE(EnterSRESET());
}

TEST_F(DOS, SetDosState)
{
    EXPECT_FALSE(SetDosState(DOS_RESET));
}

