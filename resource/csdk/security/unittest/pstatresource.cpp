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
#include "oic_malloc.h"
#include "cainterface.h"
#include "secureresourcemanager.h"
#include "srmtestcommon.h"
#include "ocpayload.h"
#include <unistd.h>
#include "ocrandom.h"
#include "psinterface.h"
#include "ocpayload.h"
#include <unistd.h>

extern OicSecPstat_t *gPstat;
OicSecPstat_t pstat;

//InitPstatResource Tests
TEST(InitPstatResourceTest, InitPstatResource)
{
    EXPECT_NE(OC_STACK_OK,  InitPstatResource(NULL, 0));
}


//DeInitPstatResource Tests
TEST(DeInitPstatResourceTest, DeInitPstatResource)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, DeInitPstatResource());
}

//PstatEntityHandler Tests
TEST(PstatEntityHandlerTest, PstatEntityHandlerWithDummyRequest)
{
    OCEntityHandlerRequest* req = new OCEntityHandlerRequest();
    ASSERT_TRUE(req != NULL);
    if(req)
    {
        EXPECT_EQ(OC_EH_ERROR, PstatEntityHandler(OCEntityHandlerFlag::OC_REQUEST_FLAG, req, NULL));
        delete req;
    }
}

TEST(PstatEntityHandlerTest, PstatEntityHandlerInvalidRequest)
{
    EXPECT_EQ(OC_EH_ERROR, PstatEntityHandler(OCEntityHandlerFlag::OC_OBSERVE_FLAG, NULL, NULL));
}

TEST(PstatConversionTest, PstatConversionWithInvalidValues)
{
    OCRepPayload* payload = PstatToPayload(NULL);
    EXPECT_TRUE(NULL == payload);
    OicSecPstat_t *pstat1 = PayloadToPstat(payload);
    EXPECT_TRUE(pstat1 == NULL);
}

TEST(PstatConversionTest, PstatConversionWithValidValues)
{
    pstat.cm = NORMAL;
    pstat.commitHash = 0;
    OCFillRandomMem(pstat.deviceID.id, sizeof(pstat.deviceID.id));
    pstat.isOp = false;
    pstat.tm = NORMAL;
    pstat.om = SINGLE_SERVICE_CLIENT_DRIVEN;
    pstat.smLen = 2;
    pstat.sm = (OicSecDpom_t*)OICCalloc(pstat.smLen, sizeof(OicSecDpom_t));
    if(pstat.sm)
    {
        pstat.sm[0] = SINGLE_SERVICE_CLIENT_DRIVEN;
        pstat.sm[1] = SINGLE_SERVICE_SERVER_DRIVEN;
    }
    OCRepPayload* payload = PstatToPayload(&pstat);
    ASSERT_TRUE(payload != NULL);
    if(payload)
    {
        OicSecPstat_t *pstat1 = PayloadToPstat(payload);
        ASSERT_TRUE(pstat1 != NULL);
        if(pstat1 && pstat.sm)
        {
            EXPECT_TRUE(pstat1->cm == pstat.cm);
            EXPECT_TRUE(pstat1->sm[1] == pstat.sm[1]);
        }
        OICFree(pstat1);
    }
    OCRepPayloadDestroy(payload);

}

TEST(PstatReadWriteTests, PstatReadWriteData)
{
    static OCPersistentStorage ps = {fopen, fread, fwrite, fclose, unlink};
    EXPECT_EQ(OC_STACK_OK,
            OCRegisterPersistentStorageHandler(&ps));
    gPstat = &pstat;
    uint8_t *pstatPayload;
    size_t pstatSize;
    UpdateSVRData();
    EXPECT_EQ(OC_STACK_OK, ConvertPstatData(&pstatPayload, &pstatSize));
    OicSvr_t svr = {0, 0, 0, 0, 0, 0, 0, 0};
    EXPECT_EQ(OC_STACK_OK, ReadSVDataFromPS(&svr));
    OICFree(svr.aclPayload);
    OICFree(svr.credPayload);
    OICFree(svr.doxmPayload);
    OICFree(svr.pstatPayload);
    EXPECT_TRUE(pstat.cm == gPstat->cm);
    EXPECT_TRUE(pstat.sm[1] == gPstat->sm[1]);
}

TEST(PstatEntityHandlerTest, PstatEntityHandlerWithPutRequest)
{
    OCEntityHandlerRequest* req = new OCEntityHandlerRequest();
    ASSERT_TRUE(req != NULL);
    if(req)
    {
        req->method = OC_REST_PUT;
        pstat.isOp = true;
        pstat.cm = NORMAL;
        pstat.commitHash = 0;
        OCFillRandomMem(pstat.deviceID.id, sizeof(pstat.deviceID.id));
        OCRepPayload* payload = PstatToPayload(&pstat);
        ASSERT_TRUE(payload != NULL);
        if(payload)
        {
            req->payload = (OCPayload*)payload;
            req->payload->type = PAYLOAD_TYPE_REPRESENTATION;
            EXPECT_EQ(OC_EH_ERROR, PstatEntityHandler(OCEntityHandlerFlag::OC_REQUEST_FLAG, req, NULL));
            OCRepPayloadDestroy(payload);
        }
        delete req;
    }
}
