// Copyright 2015 Intel Mobile Communications GmbH All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
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
#include "securevirtualresourcetypes.h"
#include "srmresourcestrings.h"
#include "doxmresource.h"
#include "ocserverrequest.h"
#include "ocmalloc.h"

#ifdef __cplusplus
extern "C" {
#endif

//Declare Doxm resource methods for testing
OCStackResult CreateDoxmResource();
OCEntityHandlerResult DoxmEntityHandler (OCEntityHandlerFlag flag,
                OCEntityHandlerRequest * ehRequest);
char * BinToDoxmJSON(const OicSecDoxm_t * doxm);
OicSecDoxm_t * JSONToDoxmBin(const char * jsonStr);
void InitSecDoxmInstance(OicSecDoxm_t * doxm);
OCEntityHandlerResult HandleDoxmPostRequest (const OCEntityHandlerRequest * ehRequest);
void DeleteDoxmBinData(OicSecDoxm_t* doxm);
#ifdef __cplusplus
}
#endif

OicSecDoxm_t * getBinDoxm()
{
    OicSecDoxm_t * doxm = (OicSecDoxm_t*)OCCalloc(1, sizeof(OicSecDoxm_t));
    doxm->oxmTypeLen =  1;
    doxm->oxmType    = (OicUrn_t *)OCCalloc(doxm->oxmTypeLen, sizeof(char *));
    doxm->oxmType[0] = (char*)OCMalloc(strlen(OXM_JUST_WORKS) + 1);
    strcpy(doxm->oxmType[0], OXM_JUST_WORKS);
    doxm->oxmLen     = 1;
    doxm->oxm        = (OicSecOxm_t *)OCCalloc(doxm->oxmLen, sizeof(short));
    doxm->oxm[0]     = OIC_JUST_WORKS;
    doxm->oxmSel     = OIC_JUST_WORKS;
    doxm->owned      = true;
    //TODO: Need more clarification on deviceIDFormat field type.
    //doxm.deviceIDFormat = URN;
    strcpy((char *) doxm->deviceID.id, "deviceId");
    strcpy((char *)doxm->owner.id, "ownersId");
    return doxm;
}

 //InitDoxmResource Tests
TEST(InitDoxmResourceTest, InitDoxmResource)
{
    EXPECT_EQ(OC_STACK_ERROR, InitDoxmResource());
}

//DeInitDoxmResource Tests
TEST(DeInitDoxmResourceTest, DeInitDoxmResource)
{
    EXPECT_EQ(OC_STACK_ERROR, DeInitDoxmResource());
}

//CreateDoxmResource Tests
TEST(CreateDoxmResourceTest, CreateDoxmResource)
{
    EXPECT_EQ(OC_STACK_ERROR, CreateDoxmResource());
}

 //DoxmEntityHandler Tests
TEST(DoxmEntityHandlerTest, DoxmEntityHandlerWithDummyRequest)
{
    OCEntityHandlerRequest req;
    EXPECT_EQ(OC_EH_ERROR, DoxmEntityHandler(OCEntityHandlerFlag::OC_REQUEST_FLAG, &req));
}

TEST(DoxmEntityHandlerTest, DoxmEntityHandlerWithNULLRequest)
{
    EXPECT_EQ(OC_EH_ERROR, DoxmEntityHandler(OCEntityHandlerFlag::OC_REQUEST_FLAG, NULL));
}

TEST(DoxmEntityHandlerTest, DoxmEntityHandlerInvalidFlag)
{
    OCEntityHandlerRequest req;
    EXPECT_EQ(OC_EH_ERROR, DoxmEntityHandler(OCEntityHandlerFlag::OC_OBSERVE_FLAG, &req));
}

//BinToDoxmJSON Tests
TEST(BinToDoxmJSONTest, BinToDoxmJSONNullDoxm)
{
    char* value = BinToDoxmJSON(NULL);
    EXPECT_TRUE(value == NULL);
}

TEST(BinToDoxmJSONTest, BinToDoxmJSONValidDoxm)
{
    OicSecDoxm_t * doxm =  getBinDoxm();

    char * json = BinToDoxmJSON(doxm);
    printf("BinToDoxmJSON:%s\n", json);
    EXPECT_TRUE(json != NULL);

    DeleteDoxmBinData(doxm);
    OCFree(json);
}

//JSONToDoxmBin Tests
TEST(JSONToDoxmBinTest, JSONToDoxmBinValidJSON)
{
    OicSecDoxm_t * doxm1 =  getBinDoxm();
    char * json = BinToDoxmJSON(doxm1);
    EXPECT_TRUE(json != NULL);

    OicSecDoxm_t *doxm2 = JSONToDoxmBin(json);
    EXPECT_TRUE(doxm2 != NULL);

    DeleteDoxmBinData(doxm1);
    OCFree(json);
}

TEST(JSONToDoxmBinTest, JSONToDoxmBinNullJSON)
{
    OicSecDoxm_t *doxm = JSONToDoxmBin(NULL);
    EXPECT_TRUE(doxm == NULL);
}

//GetDoxmResourceData Test
TEST(DoxmGetResourceDataTest, GetDoxmResourceData)
{
    EXPECT_TRUE(NULL == GetDoxmResourceData());
}
#if 0
TEST(HandleDoxmPostRequestTest, HandleDoxmPostRequestValidInput)
{
    OCEntityHandlerRequest ehRequest = {};
    OCServerRequest svRequest = {};

    OicSecDoxm_t * doxm =  getBinDoxm();

    strcpy(svRequest.addressInfo.IP.ipAddress, "10.10.10.10");
    svRequest.addressInfo.IP.port = 2345;
    svRequest.connectivityType = CA_ETHERNET;

    ehRequest.reqJSONPayload = (unsigned char *) BinToDoxmJSON(doxm);
    ehRequest.requestHandle = (OCRequestHandle) &svRequest;

    EXPECT_EQ(OC_EH_ERROR, HandleDoxmPostRequest(&ehRequest));
    DeleteDoxmBinData(doxm);
    OCFree(ehRequest.reqJSONPayload);
}
#endif
