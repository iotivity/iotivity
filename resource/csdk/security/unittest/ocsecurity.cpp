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
#include <gtest/gtest.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "utlist.h"
#include "tools.h"
#undef TAG
#include "../src/ocsecurity.c"

#ifdef __cplusplus
}
#endif

#ifdef TAG
#undef TAG
#endif

#define TAG  "OC-SEC-UT"

#define SVR_DB_FILE_NAME TAG".dat"
#define PM_DB_FILE_NAME TAG".db"

using namespace std;

class OC_SEC_UT : public ::testing::Test
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

static void resultCallback (void* ctx, bool hasError)
{
    OC_UNUSED(ctx);
    OC_UNUSED(hasError);

    OIC_LOG_V(INFO, TAG, "%s: has error: %d", __func__, hasError);
}

TEST_F(OC_SEC_UT, OCAssertRolesTest)
{
    OCDevAddr* devAddr = setAddr("127.0.0.1", 4444, (OCTransportAdapter)CT_ADAPTER_IP);

    EXPECT_EQ(OC_STACK_INVALID_CALLBACK, OCAssertRoles(NULL, devAddr, NULL));
    EXPECT_EQ(OC_STACK_INCONSISTENT_DB, OCAssertRoles(NULL, devAddr, resultCallback));

    OicSecCred_t *cred = getSignedAsymKeyCred();

    OICFree(cred->credUsage);
    cred->credUsage = (char*)OICCalloc(strlen(ROLE_CERT) + 1, 1);
    strncpy(cred->credUsage, ROLE_CERT, strlen(ROLE_CERT));
    AddCredential(cred);

    EXPECT_EQ(OC_STACK_OK, OCAssertRoles(NULL, devAddr, resultCallback));
    OICFree(devAddr);

    devAddr = setAddr("::1", 4444, (OCTransportAdapter)CT_ADAPTER_IP);
    devAddr->flags = (OCTransportFlags)(CT_FLAG_SECURE | CT_IP_USE_V6);
    EXPECT_EQ(OC_STACK_OK, OCAssertRoles(NULL, devAddr, resultCallback));

    devAddr->flags = (OCTransportFlags)(CT_FLAG_SECURE);
    EXPECT_EQ(OC_STACK_ERROR, OCAssertRoles(NULL, devAddr, resultCallback));
    OICFree(devAddr);

    devAddr = setAddr("127.0.0.1", 4444, (OCTransportAdapter)CT_ADAPTER_TCP);
    EXPECT_EQ(OC_STACK_OK, OCAssertRoles(NULL, devAddr, resultCallback));
    OICFree(devAddr);

    devAddr = setAddr("127.0.0.1", 4444, (OCTransportAdapter)CT_ADAPTER_GATT_BTLE);
    EXPECT_EQ(OC_STACK_OK, OCAssertRoles(NULL, devAddr, resultCallback));
    OICFree(devAddr);

    devAddr = setAddr("127.0.0.1", 4444, (OCTransportAdapter)CT_ADAPTER_RFCOMM_BTEDR);
    EXPECT_EQ(OC_STACK_ERROR, OCAssertRoles(NULL, devAddr, resultCallback));

    devAddr->adapter = (OCTransportAdapter)CT_DEFAULT;
    EXPECT_EQ(OC_STACK_ERROR, OCAssertRoles(NULL, devAddr, resultCallback));
    OICFree(devAddr);

}

TEST_F(OC_SEC_UT, OCAssertRolesCBHandler)
{
    OCClientResponse *response = NULL;

    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, OCAssertRolesCBHandler(NULL, NULL, response));

    response = (OCClientResponse *)OICCalloc(1, sizeof(*response));
    response->payload = (OCPayload *)OCRepPayloadCreate();
    response->payload->type = PAYLOAD_TYPE_REPRESENTATION;

    AssertRolesData_t *assertRolesData = (AssertRolesData_t*)OICCalloc(1, sizeof(AssertRolesData_t));;
    assertRolesData->ctx = NULL;
    assertRolesData->resultCallback = resultCallback;

    EXPECT_EQ(OC_STACK_DELETE_TRANSACTION, OCAssertRolesCBHandler((void*)assertRolesData, NULL, response));
}
