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
#include "../src/crlresource.c"
#include "srmtestcommon.h"
#include "ocserverrequest.h"

#ifdef __cplusplus
}
#endif

#ifdef TAG
#undef TAG
#endif

#define TAG  "SRM-CRL-UT"

#define SVR_DB_FILE_NAME TAG".dat"
#define PM_DB_FILE_NAME TAG".db"

class CRL : public ::testing::Test
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

OCStackResult responseHandler(OCEntityHandlerResponse * ehResponse)
{
    OC_UNUSED(ehResponse);
    OIC_LOG_V(DEBUG, TAG, "%s run", __func__);
    return OC_STACK_OK;
}

//GetCRLResource Tests
TEST_F(CRL, GetCRLResource)
{
    OicSecCrl_t *nullCrl = GetCRLResource();
    EXPECT_NE((OicSecCrl_t*)NULL, nullCrl);
    DeleteCrl(nullCrl);
}

//CrlToCBORPayload Tests
TEST_F(CRL, CrlToCBORPayload)
{
    uint8_t *payload = NULL;
    size_t size;
    OicSecCrl_t *crl = GetCRLResource();
    ASSERT_TRUE(NULL != crl);
    size = 0;
    EXPECT_EQ(OC_STACK_INVALID_PARAM, CrlToCBORPayload(NULL, &payload, &size, NULL));
    EXPECT_EQ(OC_STACK_OK, CrlToCBORPayload(crl, &payload, &size, NULL));
    DeleteCrl(crl);
    OICFree(payload);
}

//CBORPaylodToCrl Tests
TEST_F(CRL, CBORPayloadToCrl)
{
    uint8_t *payload = NULL;
    size_t size;
    OicSecCrl_t *crl = GetCRLResource();
    ASSERT_TRUE(NULL != crl);
    size = 0;
    EXPECT_EQ(OC_STACK_OK, CrlToCBORPayload(crl, &payload, &size, NULL));
    DeleteCrl(crl);
    crl = NULL;
    EXPECT_EQ(OC_STACK_INVALID_PARAM, CBORPayloadToCrl(NULL, size, &crl));
    EXPECT_EQ(OC_STACK_OK, CBORPayloadToCrl(payload, size, &crl));
    DeleteCrl(crl);
    OICFree(payload);
}

//GetDerCrl Tests
TEST_F(CRL, GetDerCrl)
{
    ByteArray crlArray = {NULL, 0};
    GetDerCrl(&crlArray);
    EXPECT_NE(static_cast<size_t>(0), crlArray.len);
    OICFree(crlArray.data);
}

TEST_F(CRL, copyCrl)
{
    OicSecCrl_t *src = GetCRLResource();
    OicSecCrl_t *dst = (OicSecCrl_t*)OICCalloc(1, sizeof(OicSecCrl_t));
    EXPECT_TRUE(copyCrl(src, dst));
}

TEST_F(CRL, getCurrentUTCTime)
{
    char *time = (char*)OICCalloc(512,1);
    getCurrentUTCTime(time, 512);
    ASSERT_STRNE(NULL, time);
    OICFree(time);
}

TEST_F(CRL, UpdateCRLResource)
{
    OicSecCrl_t *src = GetCRLResource();
    EXPECT_EQ(OC_STACK_OK, UpdateCRLResource(src));
}

TEST_F(CRL, ValidateQuery)
{
    char *q = (char*)OICCalloc(512,1);
    EXPECT_TRUE(ValidateQuery(q));
    snprintf(q, 512, "%s=lo",OC_RSRVD_INTERFACE);
    EXPECT_FALSE(ValidateQuery(q));
    OICFree(q);
}

TEST_F(CRL, HandleCRLGetRequest)
{
    OCServerRequest *srvReq = (OCServerRequest*)OICCalloc(1, sizeof(OCServerRequest));
    srvReq->ehResponseHandler = responseHandler;
    OCEntityHandlerRequest ehReq = OCEntityHandlerRequest();
    ehReq.method = OC_REST_GET;
    ehReq.requestHandle = (OCRequestHandle) srvReq;
    EXPECT_EQ(OC_EH_OK,HandleCRLGetRequest(&ehReq));
    ehReq.query = (char*)TAG;
    EXPECT_EQ(OC_EH_OK,HandleCRLGetRequest(&ehReq));
    OicSecCrl_t *p = gCrl;
    gCrl = NULL;
    EXPECT_EQ(OC_EH_ERROR,HandleCRLGetRequest(&ehReq));
    gCrl = p;
}

TEST_F(CRL, HandleCRLPostRequest)
{
    OicSecCrl_t *crl = GetCRLResource();
    uint8_t *payload = NULL;
    size_t size = 0;
    char *lastUpdate = NULL;

    ASSERT_EQ(OC_STACK_OK, CrlToCBORPayload(crl, &payload, &size, lastUpdate));
    OCSecurityPayload *securityPayload = OCSecurityPayloadCreate(payload, size);
    ASSERT_TRUE(NULL != securityPayload);

    OCServerRequest *srvReq = (OCServerRequest*)OICCalloc(1, sizeof(OCServerRequest));
    srvReq->ehResponseHandler = responseHandler;

    OCEntityHandlerRequest ehReq = OCEntityHandlerRequest();
    ehReq.method = OC_REST_POST;
    ehReq.payload = (OCPayload *) securityPayload;
    ehReq.requestHandle = (OCRequestHandle) srvReq;
    EXPECT_EQ(OC_EH_CHANGED, HandleCRLPostRequest(&ehReq));
}

TEST_F(CRL, CRLEntityHandler)
{
    OicSecCrl_t *crl = GetCRLResource();
    uint8_t *payload = NULL;
    size_t size = 0;
    char *lastUpdate = NULL;

    EXPECT_EQ(OC_STACK_INVALID_PARAM, getLastUpdateFromDB(NULL));
    EXPECT_EQ(OC_STACK_OK, getLastUpdateFromDB(&lastUpdate));
    ASSERT_EQ(OC_STACK_OK, CrlToCBORPayload(crl, &payload, &size, lastUpdate));
    OCSecurityPayload *securityPayload = OCSecurityPayloadCreate(payload, size);
    ASSERT_TRUE(NULL != securityPayload);

    OCServerRequest *srvReq = (OCServerRequest*)OICCalloc(1, sizeof(OCServerRequest));
    srvReq->ehResponseHandler = responseHandler;

    OCEntityHandlerRequest ehReq = OCEntityHandlerRequest();
    ehReq.method = OC_REST_POST;
    ehReq.payload = (OCPayload *) securityPayload;
    ehReq.requestHandle = (OCRequestHandle) srvReq;
    EXPECT_EQ(OC_EH_CHANGED, CRLEntityHandler(OC_REQUEST_FLAG, &ehReq, NULL));
    ehReq.method = OC_REST_GET;
    EXPECT_EQ(OC_EH_OK, CRLEntityHandler(OC_REQUEST_FLAG, &ehReq, NULL));
    ehReq.method = OC_REST_NOMETHOD;
    EXPECT_EQ(OC_EH_ERROR, CRLEntityHandler(OC_REQUEST_FLAG, &ehReq, NULL));
}
