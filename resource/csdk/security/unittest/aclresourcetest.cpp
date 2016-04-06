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
#include "psinterface.h"
#include "ocpayload.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "cainterface.h"
#include "secureresourcemanager.h"
#include "securevirtualresourcetypes.h"
#include "srmresourcestrings.h"
#include "aclresource.h"
#include "srmtestcommon.h"
#include "srmutility.h"
#include "logger.h"
#include "doxmresource.h"
#include "ocpayload.h"
#include "ocpayloadcbor.h"
#include "payload_logging.h"
#include "security_internals.h"

using namespace std;

#define TAG  "SRM-ACL-UT"

// These paths match jenkins build configuration.
const char* DEFAULT_ACL_FILE_NAME = "/oic_unittest_default_acl.dat";
const char* ACL1_FILE_NAME = "/oic_unittest_acl1.dat";

#define NUM_ACE_FOR_WILDCARD_IN_ACL1_DAT (1)

TEST(ACLResourceTest, CBORDefaultACLConversion)
{
    OicSecAcl_t *defaultAcl = (OicSecAcl_t *) OICCalloc(1, sizeof(OicSecAcl_t));
    ASSERT_TRUE(defaultAcl != NULL);
    uint8_t defaultAclSub[] = { 0x2a };
    memcpy(defaultAcl->subject.id, defaultAclSub, sizeof(defaultAclSub));
    defaultAcl->permission = 2;
    const char *defaulAclRsrc[] = { "/oic/res", "/oic/d", "/oic/p", "/oic/res/types/d",
            "/oic/ad", "/oic/sec/acl", "/oic/sec/doxm", "/oic/sec/pstat"};
    defaultAcl->resourcesLen = 8;
    defaultAcl->resources = (char **)OICCalloc(defaultAcl->resourcesLen, sizeof(char *));
    ASSERT_TRUE(defaultAcl->resources != NULL);
    for (size_t i = 0 ; i < defaultAcl->resourcesLen; i++)
    {
        defaultAcl->resources[i] = OICStrdup(defaulAclRsrc[i]);
        ASSERT_TRUE(defaultAcl->resources[i] != NULL);
    }
    uint8_t defaultAclOwnrs[] = {0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32,
        0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32};
    memcpy(defaultAcl->rownerID.id, defaultAclOwnrs, sizeof(defaultAclOwnrs));

    size_t defaultAclSize = 0;
    uint8_t *defaultPsStorage = NULL;
    OCStackResult convRet = AclToCBORPayload(defaultAcl, &defaultPsStorage, &defaultAclSize);
    EXPECT_EQ(OC_STACK_OK, convRet);
    ASSERT_TRUE(defaultPsStorage != NULL);
    EXPECT_NE(0, defaultAclSize);

    OicSecAcl_t* convertedAcl = CBORPayloadToAcl(defaultPsStorage, defaultAclSize);
    ASSERT_TRUE(convertedAcl != NULL);

    EXPECT_EQ(defaultAcl->resourcesLen, convertedAcl->resourcesLen);
    for(int i = 0; i < convertedAcl->resourcesLen; i++)
    {
        EXPECT_EQ(0, strcmp(defaultAcl->resources[i], convertedAcl->resources[i]));
    }

    DeleteACLList(convertedAcl);
    DeleteACLList(defaultAcl);
    OICFree(defaultPsStorage);
}

TEST(ACLResourceTest, CBORACLConversion)
{
    OicSecAcl_t *secAcl = (OicSecAcl_t *) OICCalloc(1, sizeof(OicSecAcl_t));
    ASSERT_TRUE(secAcl != NULL);
    uint8_t subjectBytes[] = { 0x2a };
    memcpy(secAcl->subject.id, subjectBytes, sizeof(subjectBytes));
    secAcl->permission = 2;
    const char *rsrc[] = { "/oic/res", "/oic/d", "/oic/p", "/oic/res/types/d",
                           "/oic/ad", "/oic/sec/acl"};
    secAcl->resourcesLen = 6;
    secAcl->resources = (char **)OICCalloc(secAcl->resourcesLen, sizeof(char *));
    ASSERT_TRUE(secAcl->resources != NULL);
    for (size_t i = 0 ; i < secAcl->resourcesLen; i++)
    {
        secAcl->resources[i] = OICStrdup(rsrc[i]);
        ASSERT_TRUE(secAcl->resources[i] != NULL);

    }

    uint8_t ownrs[] = {0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32,
        0x32, 0x32, 0x32, 0x32, 0x32, 0x32};
    memcpy(secAcl->rownerID.id, ownrs, sizeof(ownrs));

    OicSecAcl_t *secAcl1 = (OicSecAcl_t *) OICCalloc(1, sizeof(OicSecAcl_t));
    ASSERT_TRUE(secAcl1 != NULL);
    memcpy(secAcl1->subject.id, subjectBytes, sizeof(subjectBytes));
    secAcl1->permission = 6;
    const char *rsrc1[] = { "/oic/sec/doxm", "/oic/sec/pstat"};
    secAcl1->resourcesLen = 2;
    secAcl1->resources = (char **)OICCalloc(secAcl1->resourcesLen, sizeof(char *));
    ASSERT_TRUE(secAcl1->resources != NULL);
    for (size_t i = 0 ; i < secAcl1->resourcesLen; i++)
    {
        secAcl1->resources[i] = OICStrdup(rsrc1[i]);
        ASSERT_TRUE(secAcl1->resources[i] != NULL);
    }
    memcpy(secAcl1->rownerID.id, ownrs, sizeof(ownrs));
    secAcl->next = secAcl1;

    OicSecAcl_t *secAcl2 = (OicSecAcl_t *) OICCalloc(1, sizeof(OicSecAcl_t));
    ASSERT_TRUE(secAcl2 != NULL);
    uint8_t subjectBytes1[] = {0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31,
        0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31};
    memcpy(secAcl2->subject.id, subjectBytes1, sizeof(subjectBytes1));
    secAcl2->permission = 255;
    const char *rsrc2[] = {"/oic/light", "/oic/fan" };
    secAcl2->resourcesLen = 2;
    secAcl2->resources = (char **)OICCalloc(secAcl2->resourcesLen, sizeof(char *));
    ASSERT_TRUE(secAcl2->resources != NULL);
    for (size_t i = 0 ; i < secAcl2->resourcesLen; i++)
    {
        secAcl2->resources[i] = OICStrdup(rsrc2[i]);
        ASSERT_TRUE(secAcl2->resources[i] != NULL);
    }
    memcpy(secAcl2->rownerID.id, ownrs, sizeof(ownrs));
    secAcl1->next = secAcl2;

    OicSecAcl_t *secAcl3 = (OicSecAcl_t *) OICCalloc(1, sizeof(OicSecAcl_t));
    ASSERT_TRUE(secAcl3 != NULL);
    uint8_t subjectBytes2[] = {0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
                               0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33};
    memcpy(secAcl3->subject.id, subjectBytes2, sizeof(subjectBytes2));
    secAcl3->permission = 255;
    const char *rsrc3[] = {"/oic/light", "/oic/garage" };
    secAcl3->resourcesLen = 2;
    secAcl3->resources = (char **)OICCalloc(secAcl3->resourcesLen, sizeof(char *));
    ASSERT_TRUE(secAcl3->resources != NULL);
    for (size_t i = 0 ; i < secAcl3->resourcesLen; i++)
    {
        secAcl3->resources[i] = OICStrdup(rsrc3[i]);
        ASSERT_TRUE(secAcl3->resources[i] != NULL);
    }
    memcpy(secAcl3->rownerID.id, ownrs, sizeof(ownrs));
    secAcl2->next = secAcl3;
    secAcl3->next = NULL;

    size_t size = 0;
    uint8_t *psStorage = NULL;
    EXPECT_EQ(OC_STACK_OK, AclToCBORPayload(secAcl, &psStorage, &size));
    ASSERT_TRUE(NULL != psStorage);
    OicSecAcl_t *acl = CBORPayloadToAcl(psStorage, size);
    ASSERT_TRUE(NULL != acl);
    EXPECT_EQ(2, acl->permission);
    EXPECT_EQ(6 , acl->resourcesLen);
    EXPECT_STREQ("/oic/res", acl->resources[0]);
    DeleteACLList(acl);
    OICFree(psStorage);
    DeleteACLList(secAcl);
}

//InitResource Tests
TEST(ACLResourceTest, InitAclResource)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, InitACLResource());
    EXPECT_EQ(OC_STACK_INVALID_PARAM, DeInitACLResource());
}

// Default ACL tests
TEST(ACLResourceTest, GetDefaultACLTests)
{
    uint8_t *payload = NULL;
    size_t size = 0;

    ASSERT_TRUE(ReadCBORFile(DEFAULT_ACL_FILE_NAME, OIC_JSON_ACL_NAME, &payload, &size));
    ASSERT_TRUE(payload != NULL);

    OicSecAcl_t *psAcl = CBORPayloadToAcl(payload, size);
    ASSERT_TRUE(psAcl != NULL);

    OicSecAcl_t *acl = NULL;
    EXPECT_EQ(OC_STACK_OK, GetDefaultACL(&acl));
    ASSERT_TRUE(acl != NULL);

    // Verify if the SRM generated default ACL matches with unit test default
    if (acl && psAcl)
    {
        EXPECT_TRUE(strcmp((char*)acl->subject.id, (char*)psAcl->subject.id) == 0);
        EXPECT_EQ(acl->resourcesLen, psAcl->resourcesLen);
        for (size_t i = 0; i < acl->resourcesLen; i++)
        {
            EXPECT_EQ(strlen(acl->resources[i]), strlen(psAcl->resources[i]));
            EXPECT_TRUE(memcmp(acl->resources[i], psAcl->resources[i],
                            strlen(acl->resources[i])) == 0);
        }
        EXPECT_EQ(acl->permission, psAcl->permission);
    }

    DeleteACLList(psAcl);
    DeleteACLList(acl);
    DeInitACLResource();
    OICFree(payload);
}

// 'POST' ACL tests
TEST(ACLResourceTest, ACLPostTest)
{
    // Read an ACL from the file
    uint8_t *payload = NULL;
    size_t size = 0;

    ASSERT_TRUE(ReadCBORFile(ACL1_FILE_NAME, OIC_JSON_ACL_NAME, &payload, &size));
    ASSERT_TRUE(NULL != payload);

    OCSecurityPayload *securityPayload = OCSecurityPayloadCreate(payload, size);
    ASSERT_TRUE(NULL != securityPayload);

    static OCPersistentStorage ps = OCPersistentStorage();
    SetPersistentHandler(&ps, true);

    // Create Entity Handler POST request payload
    OCEntityHandlerRequest ehReq = OCEntityHandlerRequest();
    ehReq.method = OC_REST_POST;
    ehReq.payload = (OCPayload *) securityPayload;

    ACLEntityHandler(OC_REQUEST_FLAG, &ehReq, NULL);

    OicSecAcl_t *acl = CBORPayloadToAcl(payload, size);
    ASSERT_TRUE(NULL != acl);

    // Verify if SRM contains ACL for the subject
    OicSecAcl_t *savePtr = NULL;
    const OicSecAcl_t* subjectAcl = GetACLResourceData(&(acl->subject), &savePtr);
    ASSERT_TRUE(NULL != subjectAcl);

    // Perform cleanup
    OICFree(payload);
    OCPayloadDestroy((OCPayload *) securityPayload);
    DeInitACLResource();
    DeleteACLList(acl);
}

// GetACLResource tests
TEST(ACLResourceTest, GetACLResourceTests)
{
    // Read an ACL from the file
    static OCPersistentStorage ps = OCPersistentStorage();
    SetPersistentHandler(&ps, true);

    uint8_t *payload = NULL;
    size_t size = 0;

    ASSERT_TRUE(ReadCBORFile(ACL1_FILE_NAME, OIC_JSON_ACL_NAME, &payload, &size));
    ASSERT_TRUE(payload != NULL);

    OicSecAcl_t *defaultPsAcl = CBORPayloadToAcl(payload, size);
    ASSERT_TRUE(defaultPsAcl != NULL);

    OicSecAcl_t *acl1 = NULL;
    EXPECT_EQ(OC_STACK_OK, GetDefaultACL(&acl1));
    ASSERT_TRUE(acl1 != NULL);
    EXPECT_EQ(OC_STACK_OK, SetDefaultACL(acl1));

    // Verify that ACL file contains 2 ACE entries for 'WILDCARD' subject
    const OicSecAcl_t *acl = NULL;
    OicSecAcl_t *savePtr = NULL;
    OicUuid_t subject = WILDCARD_SUBJECT_ID;
    int count = 0;

    do
    {
        acl = GetACLResourceData(&subject, &savePtr);
        count = (NULL != acl) ? count + 1 : count;
    } while (acl != NULL);

    EXPECT_EQ(count, NUM_ACE_FOR_WILDCARD_IN_ACL1_DAT);

    /* Perform cleanup */
    OICFree(payload);
    DeleteACLList(defaultPsAcl);
    DeInitACLResource();
}

static OCStackResult  populateAcl(OicSecAcl_t *acl,  int numRsrc)
{
     OCStackResult ret = OC_STACK_ERROR;
    memcpy(acl->subject.id, "2222222222222222", sizeof(acl->subject.id));
    acl->resourcesLen = (size_t)numRsrc;
    acl->resources = (char**)OICCalloc(acl->resourcesLen, sizeof(char*));
    VERIFY_NON_NULL(TAG, acl->resources, ERROR);
    acl->resources[0] = (char*)OICMalloc(strlen("/a/led")+1);
    VERIFY_NON_NULL(TAG, acl->resources[0], ERROR);
    OICStrcpy(acl->resources[0], strlen("/a/led")+1, "/a/led");
    if(numRsrc == 2)
    {
        acl->resources[1] = (char*)OICMalloc(strlen("/a/fan")+1);
        VERIFY_NON_NULL(TAG, acl->resources[1], ERROR);
        OICStrcpy(acl->resources[1], strlen("/a/fan")+1, "/a/fan");
    }
    acl->permission = 6;
    memcpy(acl->rownerID.id, "1111111111111111", sizeof(acl->rownerID.id));

    ret = OC_STACK_OK;
exit:
    return ret;

}

//'DELETE' ACL test
TEST(ACLResourceTest, ACLDeleteWithSingleResourceTest)
{
    //Populate ACL
    OicSecAcl_t acl = OicSecAcl_t();
    EXPECT_EQ(OC_STACK_OK, populateAcl(&acl, 1));

    //GET CBOR POST payload
    size_t size = 0;
    uint8_t  *payload = NULL;
    EXPECT_EQ(OC_STACK_OK, AclToCBORPayload(&acl, &payload, &size));
    ASSERT_TRUE(NULL != payload);

    // Security Payload
    OCSecurityPayload *securityPayload = OCSecurityPayloadCreate(payload, size);
    ASSERT_TRUE(NULL != securityPayload);

    static OCPersistentStorage ps = OCPersistentStorage();
    SetPersistentHandler(&ps, true);

    // Create Entity Handler POST request payload
    OCEntityHandlerRequest ehReq = OCEntityHandlerRequest();
    ehReq.payload = (OCPayload *) securityPayload;
    ehReq.method = OC_REST_POST;
    ACLEntityHandler(OC_REQUEST_FLAG, &ehReq, NULL);

    // Verify if SRM contains ACE for the subject
    OicSecAcl_t* savePtr = NULL;
    const OicSecAcl_t* subjectAcl1 = GetACLResourceData(&acl.subject, &savePtr);
    ASSERT_TRUE(NULL != subjectAcl1);

    // Create Entity Handler DELETE request
    ehReq.method = OC_REST_DELETE;
    char query[] = "subjectuuid=2222222222222222;resources=/a/led";
    ehReq.query = (char *)OICMalloc(strlen(query)+1);
    ASSERT_TRUE(NULL !=  ehReq.query);
    OICStrcpy(ehReq.query, strlen(query)+1, query);
    ACLEntityHandler(OC_REQUEST_FLAG, &ehReq, NULL);

    // Verify if SRM has deleted ACE for the subject
    savePtr = NULL;
    const OicSecAcl_t* subjectAcl2 = GetACLResourceData(&acl.subject, &savePtr);
    ASSERT_TRUE(NULL == subjectAcl2);

    // Perform cleanup
    DeInitACLResource();
    OICFree(ehReq.query);
    OCPayloadDestroy((OCPayload *)securityPayload);
    OICFree(payload);
}

TEST(ACLResourceTest, ACLDeleteWithMultiResourceTest)
{
    //Populate ACL
    OicSecAcl_t acl = OicSecAcl_t();
    EXPECT_EQ(OC_STACK_OK, populateAcl(&acl, 2));

    //GET CBOR POST payload
    size_t size = 0;
    uint8_t *payload = NULL;
    EXPECT_EQ(OC_STACK_OK, AclToCBORPayload(&acl, &payload, &size));
    ASSERT_TRUE(NULL != payload);

    // Security Payload
    OCSecurityPayload *securityPayload = OCSecurityPayloadCreate(payload, size);
    ASSERT_TRUE(NULL!= securityPayload);

    static OCPersistentStorage ps = OCPersistentStorage();
    SetPersistentHandler(&ps, true);

    // Create Entity Handler POST request payload
    OCEntityHandlerRequest ehReq = OCEntityHandlerRequest();
    ehReq.method = OC_REST_POST;
    ehReq.payload = (OCPayload *)securityPayload;
    ACLEntityHandler(OC_REQUEST_FLAG, &ehReq, NULL);

    // Verify if SRM contains ACE for the subject with two resources
    OicSecAcl_t* savePtr = NULL;
    const OicSecAcl_t* subjectAcl1 = GetACLResourceData(&acl.subject, &savePtr);
    ASSERT_TRUE(NULL != subjectAcl1);
    EXPECT_EQ(2, subjectAcl1->resourcesLen);

    // Create Entity Handler DELETE request
    ehReq.method = OC_REST_DELETE;
    char query[] = "subjectuuid=2222222222222222;resources=/a/led";
    ehReq.query = (char *)OICMalloc(strlen(query)+1);
    ASSERT_TRUE(NULL != ehReq.query);
    OICStrcpy(ehReq.query, strlen(query)+1, query);

    ACLEntityHandler(OC_REQUEST_FLAG, &ehReq, NULL);

    // Verify if SRM contains ACL for the subject but only with one resource
    savePtr = NULL;
    const OicSecAcl_t* subjectAcl2 = GetACLResourceData(&acl.subject, &savePtr);
    ASSERT_TRUE(NULL != subjectAcl2);
    EXPECT_EQ(1, subjectAcl2->resourcesLen);

    // Perform cleanup
    OCPayloadDestroy((OCPayload *)securityPayload);
    DeInitACLResource();
    OICFree(ehReq.query);
    OICFree(payload);
}

//'GET' with query ACL test
TEST(ACLResourceTest, ACLGetWithQueryTest)
{
    //Populate ACL
    OicSecAcl_t acl = OicSecAcl_t();
    EXPECT_EQ(OC_STACK_OK, populateAcl(&acl, 1));

    //GET CBOR POST payload
    size_t size = 0;
    uint8_t *payload = NULL;
    EXPECT_EQ(OC_STACK_OK, AclToCBORPayload(&acl, &payload, &size));
    ASSERT_TRUE(NULL != payload);

    // Security Payload
    OCSecurityPayload *securityPayload = OCSecurityPayloadCreate(payload, size);
    ASSERT_TRUE(NULL != securityPayload);

    static OCPersistentStorage ps = OCPersistentStorage();
    SetPersistentHandler(&ps, true);

    //Create Entity Handler POST request payload
    OCEntityHandlerRequest ehReq = OCEntityHandlerRequest();
    ehReq.method = OC_REST_POST;
    ehReq.payload = (OCPayload *)securityPayload;
    ACLEntityHandler(OC_REQUEST_FLAG, &ehReq, NULL);

    //Create Entity Handler GET request wit query
    ehReq.method = OC_REST_GET;
    char query[] = "subjectuuid=2222222222222222;resources=/a/led";
    ehReq.query = (char*)OICMalloc(strlen(query)+1);
    ASSERT_TRUE(NULL != ehReq.query);
    OICStrcpy(ehReq.query, strlen(query)+1, query);

    ACLEntityHandler(OC_REQUEST_FLAG, &ehReq, NULL);

    // Perform cleanup
    OCPayloadDestroy((OCPayload *)securityPayload);
    DeInitACLResource();
    OICFree(ehReq.query);
    OICFree(payload);
}
