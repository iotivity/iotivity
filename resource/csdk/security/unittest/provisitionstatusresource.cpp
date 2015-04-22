//******************************************************************
//
// Copyright 2015 Intel Mobile Communications GmbH All Rights Reserved.
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
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include "gtest/gtest.h"
#include "ocstack.h"
#include "resourcemanager.h"
#include "pstatresource.h"
#include "ocmalloc.h"
#include "cJSON.h"
#include "base64.h"
#include "cainterface.h"
#include "securityresourcemanager.h"
#include <unistd.h>
#ifdef __cplusplus
extern "C" {
#endif
//Declare Provision status resource methods for testing
OCStackResult CreatePstatResource();
OCEntityHandlerResult PstatEntityHandler (OCEntityHandlerFlag flag,
                                        OCEntityHandlerRequest * ehRequest);
char * BinToPstatJSON(const OicSecPstat_t * pstat);
OicSecPstat_t * JSONToPstatBin(const char * jsonStr);
char* ReadFile(const char* filename);
extern char* JSON_FILE_NAME;
#ifdef __cplusplus
}
#endif

//InitPstatResource Tests
TEST(InitPstatResourceTest, InitPstatResource)
{
    EXPECT_EQ(OC_STACK_ERROR,  InitPstatResource());
}


//DeInitPstatResource Tests
TEST(DeInitPstatResourceTest, DeInitPstatResource)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, DeInitPstatResource());
}

//CreatePstatResource Tests
TEST(CreatePstatResourceTest, CreatePstatResource)
{
    EXPECT_EQ(OC_STACK_ERROR,  CreatePstatResource());
}

//PstatEntityHandler Tests
TEST(PstatEntityHandlerTest, PstatEntityHandlerWithDummyRequest)
{
    OCEntityHandlerRequest req;
    EXPECT_EQ(OC_EH_ERROR, PstatEntityHandler(OCEntityHandlerFlag::OC_REQUEST_FLAG, &req));
}

TEST(PstatEntityHandlerTest, PstatEntityHandlerWithPostRequest)
{
    OCEntityHandlerRequest req;
    req.method = OC_REST_POST;
    req.reqJSONPayload = (unsigned char*)"{ \"pstat\": { \"tm\": 0, \"om\": 3 }}";
    EXPECT_EQ(OC_EH_ERROR, PstatEntityHandler(OCEntityHandlerFlag::OC_REQUEST_FLAG, &req));
}

TEST(PstatEntityHandlerTest, PstatEntityHandlerInvalidRequest)
{
    EXPECT_EQ(OC_EH_ERROR, PstatEntityHandler(OCEntityHandlerFlag::OC_OBSERVE_FLAG, NULL));
}

//BinToJSON Tests
TEST(BinToJSONTest, BinToNullJSON)
{
    char* value = BinToPstatJSON(NULL);
    EXPECT_TRUE(value == NULL);
}

TEST(JSONToBinTest, NullJSONToBin)
{
    OicSecPstat_t *pstat1 = JSONToPstatBin(NULL);
    EXPECT_TRUE(pstat1 == NULL);
}

//TODO: update valid values when the OicSecPstat_t is finalized
TEST(MarshalingAndUnMarshalingTest, BinToPstatJSONAndJSONToPstatBin)
{
    const char* id = "ZGV2aWNlaWQAAAAAABhanw==";
    OicSecPstat_t pstat;
    pstat.cm = NORMAL;
    pstat.commitHash = 1234;
    uint32_t outLen = 0;
    unsigned char base64Buff[sizeof(((OicUuid_t*) 0)->id)] = {};
    EXPECT_EQ(B64_OK, b64Decode(id, strlen(id), base64Buff, sizeof(base64Buff), &outLen));
    memcpy(pstat.deviceID.id, base64Buff, outLen);
    pstat.isOp = true;
    pstat.smsLen = 2;
    pstat.tm = NORMAL;
    pstat.om = SINGLE_SERVICE_CLIENT_DRIVEN;
    pstat.sms = (OicSecDpom_t*)OCCalloc(pstat.smsLen, sizeof(OicSecDpom_t));
    if(NULL != pstat.sms)
    {
        for(size_t i =0; i< pstat.smsLen; i++)
        {
            pstat.sms[i] = (OicSecDpom_t)i;
        }
    }
    char* jsonPstat = BinToPstatJSON(&pstat);
    printf("BinToJSON Dump:\n%s\n\n", jsonPstat);
    EXPECT_TRUE(jsonPstat != NULL);
    OicSecPstat_t *pstat1 = JSONToPstatBin(jsonPstat);
    EXPECT_TRUE(pstat1 != NULL);
    OCFree(pstat1->sms);
    OCFree(pstat1);
    OCFree(jsonPstat);
}

TEST(PstatTests, JSONMarshalliingTests)
{
    char *jsonStr1 = ReadFile(JSON_FILE_NAME);
    if (NULL != jsonStr1)
    {
        cJSON_Minify(jsonStr1);
        /* Workaround : cJSON_Minify does not remove all the unwanted characters
         from the end. Here is an attempt to remove those characters */
        int len = strlen(jsonStr1);
        while (len > 0)
        {
            if (jsonStr1[--len] == '}')
            {
                break;
            }
        }
        jsonStr1[len + 1] = 0;

        OicSecPstat_t* pstat = JSONToPstatBin(jsonStr1);
        EXPECT_TRUE(NULL != pstat);

        char* jsonStr2 = BinToPstatJSON(pstat);
        printf("BinToPstatJSON Dump:\n%s\n\n", jsonStr2);
        EXPECT_STRNE(jsonStr1, jsonStr2);

        OCFree(jsonStr1);
        OCFree(jsonStr2);
        OCFree(pstat);
   }
    else
    {
        printf("Please copy %s into unittest folder\n", JSON_FILE_NAME);
    }
}
