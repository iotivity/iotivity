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
#include "credresource.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "logger.h"

#define TAG PCF("SRM-CRED-UT")

#ifdef __cplusplus
extern "C" {
#endif

//Declare Cred resource methods for testing
OCStackResult CreateCredResource();
OCEntityHandlerResult CredEntityHandler (OCEntityHandlerFlag flag,
                OCEntityHandlerRequest * ehRequest);
char * BinToCredJSON(const OicSecCred_t * pstat);
OicSecCred_t * JSONToCredBin(const char * jsonStr);
void InitSecCredInstance(OicSecCred_t * cred);
void DeleteCredList(OicSecCred_t* cred);
const OicSecCred_t* GetCredResourceData(const OicUuid_t* subject);
#ifdef __cplusplus
}
#endif

OicSecCred_t * getCredList()
{
    OicSecCred_t * cred = (OicSecCred_t*)OICCalloc(1, sizeof(OicSecCred_t));
    cred->credId = 1234;
    OICStrcpy((char *)cred->subject.id, sizeof(cred->subject.id), "subject1");

#if 0
    cred->roleIdsLen = 2;
    cred->roleIds = (OicSecRole_t *)OICCalloc(cred->roleIdsLen, sizeof(OicSecRole_t));
    OICStrcpy((char *)cred->roleIds[0].id, sizeof(cred->roleIds[0].id), "role11");
    OICStrcpy((char *)cred->roleIds[1].id, sizeof(cred->roleIds[1].id), "role12");
#endif

    cred->credType = 1;
    cred->ownersLen = 1;
    cred->owners = (OicUuid_t*)OICCalloc(cred->ownersLen, sizeof(OicUuid_t));
    OICStrcpy((char *)cred->owners[0].id, sizeof(cred->owners[0].id), "ownersId11");

    cred->next = (OicSecCred_t*)OICCalloc(1, sizeof(OicSecCred_t));
    cred->next->credId = 5678;
    OICStrcpy((char *)cred->next->subject.id, sizeof(cred->next->subject.id), "subject2");
#if 0
    cred->next->roleIdsLen = 0;
#endif
    cred->next->credType = 1;
    size_t data_size = strlen("My private Key21") + 1;
    cred->next->privateData.data = (char *)OICCalloc(1, data_size);
    OICStrcpy(cred->next->privateData.data, data_size,"My private Key21");
#if 0
    cred->next->publicData.data = (char *)OICCalloc(1, strlen("My Public Key123") + 1);
    OICStrcpy(cred->next->publicData.data, sizeof(cred->next->publicData.data),"My Public Key123");
#endif
    cred->next->ownersLen = 2;
    cred->next->owners = (OicUuid_t*)OICCalloc(cred->next->ownersLen, sizeof(OicUuid_t));
    OICStrcpy((char *)cred->next->owners[0].id, sizeof(cred->next->owners[0].id), "ownersId21");
    OICStrcpy((char *)cred->next->owners[1].id, sizeof(cred->next->owners[1].id), "ownersId22");
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
    EXPECT_EQ(OC_STACK_INVALID_PARAM, InitCredResource());
}

//DeInitCredResource Tests
TEST(DeInitCredResourceTest, DeInitCredResource)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, DeInitCredResource());
}

//CreateCredResource Tests
TEST(CreateCredResourceTest, CreateCredResource)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, CreateCredResource());
}

 //CredEntityHandler Tests
TEST(CredEntityHandlerTest, CredEntityHandlerWithDummyRequest)
{
    OCEntityHandlerRequest req;
    EXPECT_EQ(OC_EH_ERROR, CredEntityHandler(OCEntityHandlerFlag::OC_REQUEST_FLAG, &req));
}

TEST(CredEntityHandlerTest, CredEntityHandlerWithNULLRequest)
{
    EXPECT_EQ(OC_EH_ERROR, CredEntityHandler(OCEntityHandlerFlag::OC_REQUEST_FLAG, NULL));
}

TEST(CredEntityHandlerTest, CredEntityHandlerInvalidFlag)
{
    OCEntityHandlerRequest req;
    EXPECT_EQ(OC_EH_ERROR, CredEntityHandler(OCEntityHandlerFlag::OC_OBSERVE_FLAG, &req));
}

//BinToCredJSON Tests
TEST(BinToCredJSONTest, BinToCredJSONNullCred)
{
    char* value = BinToCredJSON(NULL);
    EXPECT_TRUE(value == NULL);
}

TEST(BinToCredJSONTest, BinToCredJSONValidCred)
{
    char* json = NULL;
    OicSecCred_t * cred = getCredList();

    json = BinToCredJSON(cred);

    printf("BinToCredJSON:%s\n", json);
    EXPECT_TRUE(json != NULL);
    DeleteCredList(cred);
    OICFree(json);
}

//JSONToCredBin Tests
TEST(JSONToCredBinTest, JSONToCredBinValidJSON)
{
    OicSecCred_t* cred1 = getCredList();
    char* json = BinToCredJSON(cred1);

    EXPECT_TRUE(json != NULL);
    OicSecCred_t *cred2 = JSONToCredBin(json);
    EXPECT_TRUE(cred2 == NULL);
    DeleteCredList(cred1);
    DeleteCredList(cred2);
    OICFree(json);
}

TEST(JSONToCredBinTest, JSONToCredBinNullJSON)
{
    OicSecCred_t *cred = JSONToCredBin(NULL);
    EXPECT_TRUE(cred == NULL);
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

    OicUuid_t subject = {};
    OICStrcpy((char *)subject.id, sizeof(subject.id), "subject11");

    char privateKey[] = "My private Key11";

    OicSecCred_t * cred  = NULL;

    cred = GenerateCredential(&subject, SYMMETRIC_PAIR_WISE_KEY, NULL,
                             privateKey, 1, owners);
    printCred(cred);

    DeleteCredList(cred);
}

TEST(GenerateAndAddCredentialTest, GenerateAndAddCredentialValidInput)
{
    OicUuid_t owners[1];
    OICStrcpy((char *)owners[0].id, sizeof(owners[0].id), "ownersId11");

    OicUuid_t subject = {};
    OICStrcpy((char *)subject.id, sizeof(subject.id), "subject11");

    char privateKey[] = "My private Key11";

    OicSecCred_t * cred1  = NULL;
    OicSecCred_t * headCred = NULL;

    cred1 = GenerateCredential(&subject, SYMMETRIC_PAIR_WISE_KEY, NULL,
                                 privateKey, 1, owners);

    EXPECT_EQ(OC_STACK_ERROR, AddCredential(cred1));
    headCred = cred1;

    OICStrcpy((char *)owners[0].id, sizeof(owners[0].id), "ownersId22");
    OICStrcpy((char *)subject.id, sizeof(subject.id), "subject22");
    cred1 = GenerateCredential(&subject, SYMMETRIC_PAIR_WISE_KEY, NULL,
                                     privateKey, 1, owners);
    EXPECT_EQ(OC_STACK_ERROR, AddCredential(cred1));

    OICStrcpy((char *)owners[0].id, sizeof(owners[0].id), "ownersId33");
    OICStrcpy((char *)subject.id, sizeof(subject.id), "subject33");
    cred1 = GenerateCredential(&subject, SYMMETRIC_PAIR_WISE_KEY, NULL,
                                     privateKey, 1, owners);
    EXPECT_EQ(OC_STACK_ERROR, AddCredential(cred1));

    const OicSecCred_t* credList = GetCredResourceData(&headCred->subject);

    printCred(credList);

    DeleteCredList(headCred);

}

#if 0
TEST(CredGetResourceDataTest, GetCredResourceDataValidSubject)
{
    OicSecCred_t* cred = getCredList();
    EXPECT_TRUE(NULL != GetCredResourceData(cred->subject));
}
#endif


