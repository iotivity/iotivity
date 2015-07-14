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
#include <pwd.h>
#include <grp.h>
#include <linux/limits.h>
#include <sys/stat.h>
#include "ocstack.h"
#include "ocpayload.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "cJSON.h"
#include "cainterface.h"
#include "secureresourcemanager.h"
#include "securevirtualresourcetypes.h"
#include "srmresourcestrings.h"
#include "aclresource.h"
#include "srmtestcommon.h"
#include "srmutility.h"
#include "logger.h"
#include "doxmresource.h"
#include "psinterface.h"
#include "ocpayload.h"

extern OicSecDoxm_t *gDoxm;
extern OicSecAcl_t *gAcl;

//InitResource Tests
TEST(InitAclResourceTest, InitAclResource)
{
    EXPECT_EQ(OC_STACK_ERROR, InitACLResource(NULL, 0));
}

//DeinitResource Tests
TEST(DeInitAclResourceTest, DeInitAclResource)
{
    EXPECT_EQ(OC_STACK_ERROR, InitACLResource(NULL, 0));
    EXPECT_EQ(OC_STACK_INVALID_PARAM, DeInitACLResource());
}

//Conversion Tests
TEST(AclConversionTest,AclConversionWithValidValues)
{
    gDoxm = GetDoxmDefault();
    OicSecAcl_t* acl = NULL;
    EXPECT_EQ(OC_STACK_OK, GetDefaultACL(&acl));
    ASSERT_TRUE(acl != NULL);
    if(acl)
    {
        OCRepPayload* payload = AclToPayload(acl);
        ASSERT_TRUE(payload != NULL);
        if(payload)
        {
            OicSecAcl_t *acl1 = PayloadToAcl(payload);
            ASSERT_TRUE(acl1 != NULL);
            if(acl1)
            {
                EXPECT_TRUE(acl1->permission == acl->permission);
                EXPECT_TRUE(acl1->resourcesLen == acl->resourcesLen);
                DeleteACLList(acl1);
            }
        }
        OCRepPayloadDestroy(payload);
    }
    DeleteACLList(acl);
}

TEST(AclEntityHandlerTest, AclEntityPutHandlerValidRequest)
{
    OCEntityHandlerRequest* req = new OCEntityHandlerRequest();
    ASSERT_TRUE(req != NULL);
    if(req)
    {
        OicSecAcl_t* acl =  NULL;
        EXPECT_EQ(OC_STACK_OK, GetDefaultACL(&acl));
        OCRepPayload* payload = AclToPayload(acl);
        ASSERT_TRUE(payload != NULL);
        if(payload)
        {
            req->payload = (OCPayload*)payload;
            req->payload->type = PAYLOAD_TYPE_REPRESENTATION;
            EXPECT_EQ(OC_EH_ERROR, ACLEntityHandler(OCEntityHandlerFlag::OC_REQUEST_FLAG, req, NULL));
            OCRepPayloadDestroy(payload);
        }
        delete req;
        DeleteACLList(acl);
    }
}

TEST(AclReadWriteTest, AclReadWriteData)
{
    static OCPersistentStorage ps = {fopen, fread, fwrite, fclose, unlink};
    ps.open = fopen;
    ps.read = fread;
    ps.write = fwrite;
    ps.close = fclose;
    ps.unlink = unlink;
    EXPECT_EQ(OC_STACK_OK,
            OCRegisterPersistentStorageHandler(&ps));
    EXPECT_EQ(OC_STACK_OK, GetDefaultACL(&gAcl));
    ASSERT_TRUE(gAcl != NULL);
    if(gAcl)
    {
        EXPECT_EQ(OC_STACK_OK, UpdateSVRData());
        OicSvr_t svr = {0, 0, 0, 0, 0, 0, 0, 0};
        EXPECT_EQ(OC_STACK_OK, ReadSVDataFromPS(&svr));
        OICFree(svr.aclPayload);
        OICFree(svr.credPayload);
        OICFree(svr.doxmPayload);
        OICFree(svr.pstatPayload);
        OicSecAcl_t* acl1 = NULL;
        EXPECT_EQ(OC_STACK_OK, GetDefaultACL(&acl1));
        ASSERT_TRUE(acl1 != NULL);
        if(acl1)
        {
            EXPECT_TRUE(gAcl->permission == acl1->permission);
            EXPECT_TRUE(gAcl->resourcesLen == acl1->resourcesLen);
            DeleteACLList(acl1);
        }
    }
    DeInitACLResource();

}
