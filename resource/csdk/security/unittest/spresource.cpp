//******************************************************************
// Copyright 2018 Cable Television Laboratories, Inc.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific lan guage governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include <gtest/gtest.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "srmtestcommon.h"
#include "tools.h"
#include "ocserverrequest.h"
#undef TAG
#include "../src/spresource.c"

#ifdef __cplusplus
}
#endif

#ifdef TAG
#undef TAG
#endif

#define TAG "SRM-SP-UT"

#define SVR_DB_FILE_NAME TAG".dat"
#define PM_DB_FILE_NAME TAG".db"

static CAEndpoint_t *endpoint = NULL;

class SP : public ::testing::Test
{
    public:
        static void SetUpTestCase()
        {
            IOT_Init(PM_DB_FILE_NAME);
            endpoint = createEndpoint();
        }

        static void TearDownTestCase()
        {
            IOT_DeInit(PM_DB_FILE_NAME);
        }
};

//*****************************************************************************
// Test data
//*****************************************************************************

static char s_spBlackName[] = "1.3.6.1.4.1.51414.0.0.2.0";
static char s_spBlueName[] = "1.3.6.1.4.1.51414.0.0.3.0";
static char s_spPurpleName[] = "1.3.6.1.4.1.51414.0.0.4.0";
static char s_spBaselineName[] = "1.3.6.1.4.1.51414.0.0.1.0";

static char* s_profileArray[] = { NULL, NULL, NULL, NULL, NULL, };
static OicSecSp_t s_testSp =
{
    0,                      // supportedLen
    s_profileArray,         // supportedProfiles[0]
    NULL,                   // currentProfile
};

//*****************************************************************************
// Helper fxns
//*****************************************************************************
static void TestEncodeDecode(OicSecSp_t* spToTest, bool* propsToTest, bool checkForValidSp)
{
    OCStackResult ret = OC_STACK_OK;

    uint8_t *cborPayload = NULL;
    size_t cborSize = 0;
    OicSecSp_t *spDecoded = NULL;
    bool decodedProperties[SP_PROPERTY_COUNT];

    // encode
    ret = SpToCBORPayloadPartial(spToTest, &cborPayload, &cborSize, propsToTest);
    EXPECT_TRUE(OC_STACK_OK == ret);
    VERIFY_OR_LOG_AND_EXIT(TAG, (OC_STACK_OK == ret), "Failed to encode sp", ERROR);

    // decode
    ret = CBORPayloadToSp(cborPayload, cborSize, &spDecoded, decodedProperties);
    EXPECT_TRUE(OC_STACK_OK == ret);
    VERIFY_OR_LOG_AND_EXIT(TAG, (OC_STACK_OK == ret), "Failed to decode sp", ERROR);

    if ( checkForValidSp )
    {
        EXPECT_TRUE(true==RequiredSpPropsPresentAndValid(spDecoded, decodedProperties));
    }

    // compare
    EXPECT_TRUE(true == IsPropArraySame(propsToTest, decodedProperties));
    EXPECT_TRUE(true == IsSpSame(&s_testSp, spDecoded, propsToTest));

exit:
    OICFree(cborPayload);
    DeleteSpBinData(spDecoded);
    OICFree(spDecoded);
}


// can pass in spProps = NULL;
static void TestInit(OicSecSp_t* testSp, bool* spProps)
{
    testSp->supportedLen = 0;
    testSp->currentProfile = NULL;

    for (size_t i = 0; i < sizeof(s_profileArray)/sizeof(s_profileArray[0]); i++)
    {
        testSp->supportedProfiles[i] = NULL;
    }
    if (spProps != NULL)
    {
        SetAllSpProps(spProps, false);
    }
}

//*****************************************************************************
// Tests
//*****************************************************************************

TEST_F(SP, CBORFullEncodingPositive)
{
    bool propsToTest[SP_PROPERTY_COUNT];

    // all supported profiles
    TestInit(&s_testSp, propsToTest);
    SetAllSpProps(propsToTest, true);
    s_testSp.supportedLen = 4;
    s_testSp.supportedProfiles[0] = s_spBlueName;
    s_testSp.supportedProfiles[1] = s_spBaselineName;
    s_testSp.supportedProfiles[2] = s_spBlackName;
    s_testSp.supportedProfiles[3] = s_spPurpleName;
    s_testSp.currentProfile = s_spBlackName;
    TestEncodeDecode(&s_testSp, propsToTest, true);

    // default configuration
    TestInit(&s_testSp, propsToTest);
    SetAllSpProps(propsToTest, true);
    s_testSp.supportedLen = 1;
    s_testSp.supportedProfiles[0] = s_spBaselineName;
    s_testSp.currentProfile = s_spBaselineName;
    TestEncodeDecode(&s_testSp, propsToTest, true);

    // one non baseline
    TestInit(&s_testSp, propsToTest);
    SetAllSpProps(propsToTest, true);
    s_testSp.supportedLen = 2;
    s_testSp.supportedProfiles[0] = s_spBaselineName;
    s_testSp.supportedProfiles[1] = s_spBlackName;
    s_testSp.currentProfile = s_spBlackName;
    TestEncodeDecode(&s_testSp, propsToTest, true);
}

TEST_F(SP, CBORPartialEncodingPositive)
{
    bool propsToTest[SP_PROPERTY_COUNT];

    // supported profiles only
    TestInit(&s_testSp, propsToTest);
    propsToTest[SP_SUPPORTED_PROFILES] = true;
    s_testSp.supportedLen = 1;
    s_testSp.supportedProfiles[0] = s_spBaselineName;
    TestEncodeDecode(&s_testSp, propsToTest, false);

    // current profile only
    TestInit(&s_testSp, propsToTest);
    propsToTest[SP_CURRENT_PROFILE] = true;
    s_testSp.currentProfile = s_spBlueName;
    TestEncodeDecode(&s_testSp, propsToTest, false);

    // supported profiles and current profile
    TestInit(&s_testSp, propsToTest);
    propsToTest[SP_SUPPORTED_PROFILES] = true;
    propsToTest[SP_CURRENT_PROFILE] = true;
    s_testSp.supportedLen = 2;
    s_testSp.supportedProfiles[0] = s_spBaselineName;
    s_testSp.supportedProfiles[1] = s_spBlueName;
    s_testSp.currentProfile = s_spBlueName;
    TestEncodeDecode(&s_testSp, propsToTest, false);
}

TEST_F(SP, ValidateQuery)
{
    EXPECT_TRUE(ValidateQuery("q=35"));
    EXPECT_FALSE(ValidateQuery("if=eth0"));
}

static OCStackResult responseHandler(OCEntityHandlerResponse * ehResponse)
{
    OC_UNUSED(ehResponse);
    OIC_LOG_V(DEBUG, TAG, "%s run", __func__);
    return OC_STACK_OK;
}

TEST_F(SP, HandleSpGetRequest)
{
    OCServerRequest *srvReq = (OCServerRequest*)OICCalloc(1, sizeof(OCServerRequest));
    srvReq->ehResponseHandler = responseHandler;
    OCEntityHandlerRequest req = OCEntityHandlerRequest();
    req.method = OC_REST_GET;
    req.requestHandle = (OCRequestHandle) srvReq;
    req.devAddr.port = endpoint->port;
    strncpy(req.devAddr.addr, endpoint->addr, MAX_ADDR_STR_SIZE);
    req.devAddr.flags = (OCTransportFlags)endpoint->flags;
    req.devAddr.adapter = (OCTransportAdapter)endpoint->adapter;
    req.query = (char*)OICCalloc(256, 1);
    strncpy(req.query, "coaps://127.0.0.1:3333/?if=eth0", 256);

    EXPECT_EQ(OC_EH_OK, HandleSpGetRequest(&req));
}

TEST_F(SP, HandleSpPostRequest)
{
    OCServerRequest *srvReq = (OCServerRequest*)OICCalloc(1, sizeof(OCServerRequest));
    srvReq->ehResponseHandler = responseHandler;
    OCEntityHandlerRequest req = OCEntityHandlerRequest();
    req.method = OC_REST_POST;
    req.requestHandle = (OCRequestHandle) srvReq;
    req.devAddr.port = endpoint->port;
    strncpy(req.devAddr.addr, endpoint->addr, MAX_ADDR_STR_SIZE);
    req.devAddr.flags = (OCTransportFlags)endpoint->flags;
    req.devAddr.adapter = (OCTransportAdapter)endpoint->adapter;

    uint8_t *payload = NULL;
    size_t size = 0;
    OicSecSp_t *sp = &gDefaultSp;//(OicSecSp_t*)OICCalloc(1, sizeof(OicSecSp_t));
    ASSERT_EQ(OC_STACK_OK, SpToCBORPayload(sp, &payload, &size));
    OCSecurityPayload *securityPayload = OCSecurityPayloadCreate(payload, size);
    ASSERT_TRUE(NULL != securityPayload);
    req.payload = (OCPayload *) securityPayload;

    EXPECT_EQ(OC_EH_OK, HandleSpPostRequest(&req));
}

