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
#include "psinterface.h"
#include "securevirtualresourcetypes.h"
#include "srmresourcestrings.h"
#include "doxmresource.h"
#include "ocserverrequest.h"
#include "oic_string.h"
#include "oic_malloc.h"
#include "logger.h"
#include "ocpayload.h"

#define TAG  PCF("SRM-DOXM")

extern OicSecDoxm_t *gDoxm;

OicSecDoxm_t * getBinDoxm()
{
    OicSecDoxm_t * doxm = (OicSecDoxm_t*)OICCalloc(1, sizeof(OicSecDoxm_t));
    if(!doxm)
    {
        return NULL;
    }
    doxm->oxmTypeLen =  1;
    doxm->oxmType    = (OicUrn_t *)OICCalloc(doxm->oxmTypeLen, sizeof(char *));
    if(!doxm->oxmType)
    {
        DeleteDoxmBinData(doxm);
        return NULL;
    }
    doxm->oxmType[0] = (char*)OICMalloc(strlen(OXM_JUST_WORKS) + 1);
    if(!doxm->oxmType[0])
    {
        DeleteDoxmBinData(doxm);
        return NULL;
    }
    strcpy(doxm->oxmType[0], OXM_JUST_WORKS);
    doxm->oxmLen     = 1;
    doxm->oxm        = (OicSecOxm_t *)OICCalloc(doxm->oxmLen, sizeof(OicSecOxm_t));
    if(!doxm->oxm)
    {
        DeleteDoxmBinData(doxm);
        return NULL;
    }
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
    EXPECT_NE(OC_STACK_OK, InitDoxmResource(NULL, 0));
}

//DeInitDoxmResource Tests
TEST(DeInitDoxmResourceTest, DeInitDoxmResource)
{
    EXPECT_EQ(OC_STACK_ERROR, DeInitDoxmResource());
}

 //DoxmEntityHandler Tests
TEST(DoxmEntityHandlerTest, DoxmEntityHandlerWithDummyRequest)
{
    OCEntityHandlerRequest* req = new OCEntityHandlerRequest();
    ASSERT_TRUE(req != NULL);
    if(req)
    {
        EXPECT_EQ(OC_EH_ERROR,
                DoxmEntityHandler(OCEntityHandlerFlag::OC_REQUEST_FLAG, req, NULL));
        delete req;
    }
}

TEST(DoxmEntityHandlerTest, DoxmEntityHandlerWithNULLRequest)
{
    EXPECT_EQ(OC_EH_ERROR, DoxmEntityHandler(OCEntityHandlerFlag::OC_REQUEST_FLAG, NULL, NULL));
}

TEST(DoxmEntityHandlerTest, DoxmEntityHandlerInvalidFlag)
{
    OCEntityHandlerRequest* req = new OCEntityHandlerRequest();
    if(req)
    {
        EXPECT_EQ(OC_EH_ERROR,
                DoxmEntityHandler(OCEntityHandlerFlag::OC_OBSERVE_FLAG, req, NULL));
    }
}

TEST(DoxmEntityHandlerTest, DoxmEntityHandlerValidRequest)
{
    EXPECT_NE(OC_STACK_OK, InitDoxmResource(NULL, 0));
    OCEntityHandlerRequest* req = new OCEntityHandlerRequest();
    ASSERT_TRUE(req != NULL);
    if(req)
    {
        req->method = OC_REST_GET;
        EXPECT_EQ(OC_EH_ERROR, DoxmEntityHandler(OCEntityHandlerFlag::OC_REQUEST_FLAG, req, NULL));
        OICFree(req->query);
        delete req;
    }

}

TEST(DoxmConversionTest, DoxmConversionWithValidValues)
{
    OicSecDoxm_t * doxm =  getBinDoxm();
    if(doxm)
    {
        OCRepPayload* payload = DoxmToPayload(doxm);
        if(payload)
        {
            OicSecDoxm_t *doxm1 = PayloadToDoxm(payload);
            if(doxm1)
            {
                EXPECT_TRUE(doxm1->owned == doxm->owned);
                EXPECT_TRUE(doxm1->oxmSel == doxm->oxmSel);
                DeleteDoxmBinData(doxm1);
            }
        }
        OCRepPayloadDestroy(payload);
    }
    DeleteDoxmBinData(doxm);
}

TEST(DoxmEntityHandlerTest, DoxmEntityPutHandlerValidRequest)
{
    EXPECT_NE(OC_STACK_OK, InitDoxmResource(NULL, 0));
    OCEntityHandlerRequest* req = new OCEntityHandlerRequest();
    ASSERT_TRUE(req != NULL);
    if(req)
    {
        req->method = OC_REST_PUT;
        OicSecDoxm_t * doxm = getBinDoxm();
        ASSERT_TRUE(doxm != NULL);
        if(doxm)
        {
            doxm->owned = false;
            OCRepPayload* payload = DoxmToPayload(doxm);
            ASSERT_TRUE(payload != NULL);
            if(payload)
            {
                req->payload = (OCPayload*)payload;
                req->payload->type = PAYLOAD_TYPE_REPRESENTATION;
                EXPECT_EQ(OC_EH_ERROR, DoxmEntityHandler(OCEntityHandlerFlag::OC_REQUEST_FLAG, req, NULL));
            }
            OCRepPayloadDestroy(payload);
        }
        DeleteDoxmBinData(doxm);
        delete req;
    }
}

TEST(DoxmReadWriteTest, DoxmReadWriteData)
{
    static OCPersistentStorage ps = {fopen, fread, fwrite, fclose, unlink};
    EXPECT_EQ(OC_STACK_OK,
            OCRegisterPersistentStorageHandler(&ps));
    gDoxm = getBinDoxm();
    EXPECT_EQ(OC_STACK_OK, UpdateSVRData());
    OicSvr_t svr = {0, 0, 0, 0, 0, 0, 0, 0};
    EXPECT_EQ(OC_STACK_OK, ReadSVDataFromPS(&svr));
    OICFree(svr.aclPayload);
    OICFree(svr.credPayload);
    OICFree(svr.doxmPayload);
    OICFree(svr.pstatPayload);
    OicSecDoxm_t *doxm1 = getBinDoxm();
    ASSERT_TRUE(doxm1 != NULL);
    if(doxm1 && gDoxm)
    {
        EXPECT_TRUE(gDoxm->oxmSel == doxm1->oxmSel);
    }
    DeleteDoxmBinData(doxm1);
    DeleteDoxmBinData(gDoxm);

}

