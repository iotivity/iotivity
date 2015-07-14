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
#include "ocpayload.h"
#include "resourcemanager.h"
#include "securevirtualresourcetypes.h"
#include "credresource.h"
#include "doxmresource.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "srmtestcommon.h"
#include "logger.h"
#include "base64.h"
#include "ocrandom.h"
#include "psinterface.h"
#include "ocpayload.h"

#define TAG PCF("SRM-CRED-UT")

extern OicSecCred_t *gCred;
extern OicSecDoxm_t *gDoxm;
OicSecCred_t * getCredList()
{

    OicSecCred_t * cred = NULL;

    cred = (OicSecCred_t*)OICCalloc(1, sizeof(OicSecCred_t));
    if(!cred)
    {
        return NULL;
    }
    cred->credId = 1234;
    OCFillRandomMem(cred->subject.id, sizeof(cred->subject.id));

#if WITH_ROLEID
    cred->roleIdsLen = 2;
    cred->roleIds = (OicSecRole_t *)OICCalloc(cred->roleIdsLen, sizeof(OicSecRole_t));
    VERIFY_NON_NULL(TAG, cred->roleIds, ERROR);
    OICStrcpy((char *)cred->roleIds[0].id, sizeof(cred->roleIds[0].id), "role11");
    OICStrcpy((char *)cred->roleIds[1].id, sizeof(cred->roleIds[1].id), "role12");

#endif

    cred->credType = 1;
    cred->ownersLen = 2;
    cred->owners = (OicUuid_t*)OICCalloc(cred->ownersLen, sizeof(OicUuid_t));
    if(!cred->owners)
    {
        DeleteCredList(cred);
        return NULL;
    }
    OCFillRandomMem(cred->owners[0].id, sizeof(cred->owners[0].id));
    OCFillRandomMem(cred->owners[1].id, sizeof(cred->owners[1].id));

    //next item

    cred->next = (OicSecCred_t*)OICCalloc(1, sizeof(OicSecCred_t));
    if (!cred->next)
    {
        DeleteCredList(cred);
        return NULL;
    }
    cred->next->credId = 5678;
    OCFillRandomMem(cred->next->subject.id, sizeof(cred->next->subject.id));

#if WITH_ROLEID
    cred->next->roleIdsLen = 0;
#endif
    cred->next->credType = 1;
    size_t data_size = strlen("My private Key21") + 1;
    cred->next->privateData.data = (char *)OICCalloc(1, data_size);
    if (!cred->next->privateData.data )
    {
        DeleteCredList(cred);
        return NULL;
    }
   OICStrcpy(cred->next->privateData.data, data_size,"My private Key21");
#if WITH_ROLEID
    cred->next->publicData.data = (char *)OICCalloc(1, strlen("My Public Key123") + 1);
    OICStrcpy(cred->next->publicData.data, sizeof(cred->next->publicData.data),"My Public Key123");
#endif
    cred->next->ownersLen = 2;
    cred->next->owners = (OicUuid_t*)OICCalloc(cred->next->ownersLen, sizeof(OicUuid_t));
    if(!cred->next->owners)
    {
        DeleteCredList(cred);
        return NULL;
    }
    OCFillRandomMem(cred->next->owners[0].id, sizeof(cred->owners[0].id));
    OCFillRandomMem(cred->next->owners[1].id, sizeof(cred->owners[1].id));

    return cred;
}

static void printCred(const OicSecCred_t * cred)
{
    EXPECT_TRUE(NULL != cred);

    const OicSecCred_t *credTmp1 = NULL;
    for(credTmp1 = cred; credTmp1; credTmp1 = credTmp1->next)
    {
        OC_LOG_V(INFO, TAG, PCF("\ncred->credId = %d"), credTmp1->credId);
        OC_LOG_V(INFO, TAG, PCF("cred->subject.id = %s"), credTmp1->subject.id);
        OC_LOG_V(INFO, TAG, PCF("cred->credType = %d"), credTmp1->credType);
        if(credTmp1->privateData.data)
        {
            OC_LOG_V(INFO, TAG, PCF("cred->privateData.data = %s"), credTmp1->privateData.data);
        }
        if(credTmp1->publicData.data)
        {
           OC_LOG_V(INFO, TAG, PCF("cred->publicData.data = %s"), credTmp1->publicData.data);
        }
        OC_LOG_V(INFO, TAG, PCF("cred->ownersLen = %zd"), credTmp1->ownersLen);
        for(size_t i = 0; i < cred->ownersLen; i++)
        {
            OC_LOG_V(INFO, TAG, PCF("cred->owners[%zd].id = %s"), i, credTmp1->owners[i].id);
        }
    }
}

 //InitCredResource Tests
TEST(InitCredResourceTest, InitCredResource)
{
    EXPECT_NE(OC_STACK_OK, InitCredResource(NULL, 0));
}

//DeInitCredResource Tests
TEST(DeInitCredResourceTest, DeInitCredResource)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, DeInitCredResource());
}

//CredEntityHandler Tests
TEST(CredEntityHandlerTest, CredEntityHandlerWithDummyRequest)
{
    OCEntityHandlerRequest* req = new OCEntityHandlerRequest();
    if(req)
    {
        EXPECT_EQ(OC_EH_ERROR,
                CredEntityHandler(OCEntityHandlerFlag::OC_REQUEST_FLAG, req, NULL));
        delete req;
    }
}

TEST(CredEntityHandlerTest, CredEntityHandlerWithNULLRequest)
{
    EXPECT_EQ(OC_EH_ERROR, CredEntityHandler(OCEntityHandlerFlag::OC_REQUEST_FLAG, NULL, NULL));
}

TEST(CredEntityHandlerTest, CredEntityHandlerInvalidFlag)
{
    OCEntityHandlerRequest* req = new OCEntityHandlerRequest();
    if(req)
    {
        EXPECT_EQ(OC_EH_ERROR,
                CredEntityHandler(OCEntityHandlerFlag::OC_OBSERVE_FLAG, req, NULL));
        delete req;
    }
}

//GetCredResourceData Test
TEST(CredGetResourceDataTest, GetCredResourceDataNULLSubject)
{
    EXPECT_TRUE(NULL == GetCredResourceData(NULL));
}

TEST(CredGenerateCredentialTest, GenerateCredentialValidInput)
{
    OicUuid_t owners[1];
    OICStrcpy((char *)owners[0].id, sizeof(owners[0].id), "ownersId21");

    OicUuid_t subject = {{0}};
    OICStrcpy((char *)subject.id, sizeof(subject.id), "subject11");

    char privateKey[] = "My private Key11";

    OicSecCred_t * cred  = NULL;

    cred = GenerateCredential(&subject, SYMMETRIC_PAIR_WISE_KEY, NULL,
                             privateKey, 1, owners);
    printCred(cred);

    EXPECT_TRUE(NULL != cred);
    DeleteCredList(cred);
}

TEST(GenerateAndAddCredentialTest, GenerateAndAddCredentialValidInput)
{
    OicUuid_t owners[1];
    OICStrcpy((char *)owners[0].id, sizeof(owners[0].id), "ownersId11");

    OicUuid_t subject = {{0}};
    OICStrcpy((char *)subject.id, sizeof(subject.id), "subject11");

    char privateKey[] = "My private Key11";

    OicSecCred_t * cred1  = NULL;
    OicSecCred_t * headCred = NULL;

    cred1 = GenerateCredential(&subject, SYMMETRIC_PAIR_WISE_KEY, NULL,
                                 privateKey, 1, owners);
    gCred = NULL;
    gDoxm = NULL;
    EXPECT_EQ(OC_STACK_OK, AddCredential(cred1));
    headCred = cred1;

    OICStrcpy((char *)owners[0].id, sizeof(owners[0].id), "ownersId22");
    OICStrcpy((char *)subject.id, sizeof(subject.id), "subject22");
    cred1 = GenerateCredential(&subject, SYMMETRIC_PAIR_WISE_KEY, NULL,
                                     privateKey, 1, owners);
    EXPECT_EQ(OC_STACK_OK, AddCredential(cred1));

    OICStrcpy((char *)owners[0].id, sizeof(owners[0].id), "ownersId33");
    OICStrcpy((char *)subject.id, sizeof(subject.id), "subject33");
    cred1 = GenerateCredential(&subject, SYMMETRIC_PAIR_WISE_KEY, NULL,
                                     privateKey, 1, owners);
    EXPECT_EQ(OC_STACK_OK, AddCredential(cred1));

    const OicSecCred_t* credList = GetCredResourceData(&headCred->subject);

    printCred(credList);

    DeleteCredList(headCred);

}
TEST(CredConversionTest,CredConversionWithValidValues)
{
    OicSecCred_t *cred = getCredList();
    OCRepPayload* payload = CredToPayload(cred);
    ASSERT_TRUE(payload != NULL);
    if(payload)
    {
        OicSecCred_t *cred1 = PayloadToCred(payload);
        if(cred1 && cred)
        {
            EXPECT_TRUE(cred1->credType == cred->credType);
            EXPECT_TRUE(cred1->ownersLen == cred->ownersLen);
        }
        DeleteCredList(cred1);
    }
    OCRepPayloadDestroy(payload);
    DeleteCredList(cred);
}

TEST(CredReadWriteTest, CredReadWriteData)
{
    static OCPersistentStorage ps = {fopen, fread, fwrite, fclose, unlink};
    ps.open = fopen;
    ps.read = fread;
    ps.write = fwrite;
    ps.close = fclose;
    ps.unlink = unlink;
    EXPECT_EQ(OC_STACK_OK, OCRegisterPersistentStorageHandler(&ps));
    gCred = getCredList();
    EXPECT_EQ(OC_STACK_OK, UpdateSVRData());
    OicSvr_t svr = {0, 0, 0, 0, 0, 0, 0, 0};
    EXPECT_EQ(OC_STACK_OK, ReadSVDataFromPS(&svr));
    OICFree(svr.aclPayload);
    OICFree(svr.credPayload);
    OICFree(svr.doxmPayload);
    OICFree(svr.pstatPayload);
    OicSecCred_t *cred = getCredList();
    ASSERT_TRUE(cred != NULL);
    if(cred && gCred)
    {
        EXPECT_TRUE(gCred->credType == cred->credType);
        EXPECT_TRUE(gCred->ownersLen == cred->ownersLen);
    }
    DeleteCredList(cred);
}
