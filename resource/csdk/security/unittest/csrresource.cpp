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
#include "../src/csrresource.c"
#include "srmtestcommon.h"
#include "ocserverrequest.h"

#ifdef __cplusplus
}
#endif

#ifdef TAG
#undef TAG
#endif

#define TAG  "CSR"

#define SVR_DB_FILE_NAME TAG".dat"
#define PM_DB_FILE_NAME TAG".db"

class CSR : public ::testing::Test
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

static OCStackResult responseHandler(OCEntityHandlerResponse * ehResponse)
{
    OC_UNUSED(ehResponse);
    OIC_LOG_V(DEBUG, TAG, "%s run", __func__);
    return OC_STACK_OK;
}

TEST_F(CSR, StoreKeyPair)
{
    mbedtls_pk_context pk;
    mbedtls_pk_init(&pk);
    EXPECT_GE(OCInternalGenerateKeyPair(&pk), 0);

    OicUuid_t *uuid = createUuid();

    EXPECT_EQ(OC_STACK_OK, StoreKeyPair(&pk, uuid));

    freeUuid(uuid);
}

TEST_F(CSR, HandleCsrGetRequest)
{
    OCServerRequest *srvReq = (OCServerRequest*)OICCalloc(1, sizeof(OCServerRequest));
    srvReq->ehResponseHandler = responseHandler;
    OCEntityHandlerRequest ehReq = OCEntityHandlerRequest();
    ehReq.method = OC_REST_GET;
    ehReq.requestHandle = (OCRequestHandle) srvReq;
    EXPECT_EQ(OC_EH_OK,HandleCsrGetRequest(&ehReq));
    ehReq.query = (char*)TAG;
    EXPECT_EQ(OC_EH_OK,HandleCsrGetRequest(&ehReq));
}


TEST_F(CSR, CredCsrEntityHandler)
{
    OCServerRequest *srvReq = (OCServerRequest*)OICCalloc(1, sizeof(OCServerRequest));
    srvReq->ehResponseHandler = responseHandler;

    OCEntityHandlerRequest ehReq = OCEntityHandlerRequest();
    ehReq.method = OC_REST_POST;
    ehReq.requestHandle = (OCRequestHandle) srvReq;
    EXPECT_EQ(OC_EH_ERROR, CredCsrEntityHandler(OC_REQUEST_FLAG, &ehReq, NULL));
    ehReq.method = OC_REST_GET;
    EXPECT_EQ(OC_EH_OK, CredCsrEntityHandler(OC_REQUEST_FLAG, &ehReq, NULL));
    ehReq.method = OC_REST_NOMETHOD;
    EXPECT_EQ(OC_EH_ERROR, CredCsrEntityHandler(OC_REQUEST_FLAG, &ehReq, NULL));
}
