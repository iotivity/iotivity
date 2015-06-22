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
#ifdef __cplusplus
}
#endif

OicSecCred_t * getCredList()
{
    OicSecCred_t * cred = (OicSecCred_t*)OICCalloc(1, sizeof(OicSecCred_t));
    cred->credId = 1234;
    strcpy((char *)cred->subject.id, "subject1");

#if 0
    cred->roleIdsLen = 2;
    cred->roleIds = (OicSecRole_t *)OICCalloc(cred->roleIdsLen, sizeof(OicSecRole_t));
    strcpy((char *)cred->roleIds[0].id, "role11");
    strcpy((char *)cred->roleIds[1].id, "role12");
#endif

    cred->credType = 1;
    cred->ownersLen = 1;
    cred->owners = (OicUuid_t*)OICCalloc(cred->ownersLen, sizeof(OicUuid_t));
    strcpy((char *)cred->owners[0].id, "ownersId11");

    cred->next = (OicSecCred_t*)OICCalloc(1, sizeof(OicSecCred_t));
    cred->next->credId = 5678;
    strcpy((char *)cred->next->subject.id, "subject2");
#if 0
    cred->next->roleIdsLen = 0;
#endif
    cred->next->credType = 1;
    cred->next->privateData.data = (char *)OICCalloc(1, strlen("My private Key21") + 1);
    strcpy(cred->next->privateData.data, "My private Key21");
#if 0
    cred->next->publicData.data = (char *)OICCalloc(1, strlen("My Public Key123") + 1);
    strcpy(cred->next->publicData.data, "My Public Key123");
#endif
    cred->next->ownersLen = 2;
    cred->next->owners = (OicUuid_t*)OICCalloc(cred->next->ownersLen, sizeof(OicUuid_t));
    strcpy((char *)cred->next->owners[0].id, "ownersId21");
    strcpy((char *)cred->next->owners[1].id, "ownersId22");
    return cred;
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
   strcpy((char *)owners[0].id, "ownersId21");

    OicUuid_t subject = {};
    strcpy((char *)subject.id, "subject11");

    char privateKey[] = "My private Key11";

    OicSecCred_t * cred  = NULL;

    cred = GenerateCredential(&subject, SYMMETRIC_PAIR_WISE_KEY, NULL,
                             privateKey, 1, owners);
    printf("cred->credId = %d\n", cred->credId);
    printf("cred->subject.id = %s\n", cred->subject.id);
    printf("cred->credType = %d\n", cred->credType);
    printf("cred->privateData.data = %s\n", cred->privateData.data);
    printf("cred->ownersLen = %zd\n", cred->ownersLen);
    printf("cred->owners[0].id = %s\n", cred->owners[0].id);

    EXPECT_TRUE(NULL != cred);
    DeleteCredList(cred);
}

TEST(CredAddCredentialTest, GenerateCredentialValidInput)
{
    OicUuid_t owners[1];
    strcpy((char *)owners[0].id, "ownersId21");

    OicUuid_t subject = {};
    strcpy((char *)subject.id, "subject11");

    char privateKey[] = "My private Key11";

    OicSecCred_t * cred  = NULL;

    cred = GenerateCredential(&subject, SYMMETRIC_PAIR_WISE_KEY, NULL,
                                 privateKey, 1, owners);
    EXPECT_TRUE(NULL != cred);

    EXPECT_EQ(OC_STACK_ERROR, AddCredential(cred));

    DeleteCredList(cred);

}
#if 0
TEST(CredGetResourceDataTest, GetCredResourceDataValidSubject)
{
    OicSecCred_t* cred = getCredList();
    EXPECT_TRUE(NULL != GetCredResourceData(cred->subject));
}
#endif


