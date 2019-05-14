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

#include "oic_malloc.h"
#include "tools.h"
#undef TAG
#include "../src/pkix_interface.c"

#ifdef __cplusplus
}
#endif

#ifdef TAG
#undef TAG
#endif

#define TAG  "PKIX"

#define SVR_DB_FILE_NAME TAG".dat"
#define PM_DB_FILE_NAME TAG".db"


class PKIX : public ::testing::Test
{
    public:
        static void SetUpTestCase()
        {
        }
        static void TearDownTestCase()
        {
        }
};

TEST_F(PKIX, GetManufacturerPkixInfo)
{
    PkiInfo_t* inf = NULL;
    bool list = false;

    GetPkixInfo(inf);
    GetManufacturerPkixInfo(inf);
    InitManufacturerCipherSuiteList(&list, "33333333-3333-3333-3333-111111111111");

    inf = (PkiInfo_t*)OICCalloc(1, sizeof(PkiInfo_t));
    inf->key.data = keyData();
    inf->key.len = keyDataLen();

    GetPkixInfo(inf);
    GetManufacturerPkixInfo(inf);

    OICFree(inf->key.data);
    OICFree(inf);
}

