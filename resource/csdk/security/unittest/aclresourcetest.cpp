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
#include "oic_malloc.h"
#include "cJSON.h"
#include "cainterface.h"
#include "secureresourcemanager.h"
#include "securevirtualresourcetypes.h"
#include "srmresourcestrings.h"
#include "aclresource.h"

using namespace std;

#ifdef __cplusplus
extern "C" {
#endif
extern char * BinToAclJSON(const OicSecAcl_t * acl);
extern OicSecAcl_t * JSONToAclBin(const char * jsonStr);
char* ReadFile(const char* filename);
extern void DeleteACLList(OicSecAcl_t* acl);
OCStackResult  GetDefaultACL(OicSecAcl_t** defaultAcl);
OCEntityHandlerResult ACLEntityHandler (OCEntityHandlerFlag flag,
                                        OCEntityHandlerRequest * ehRequest);
#ifdef __cplusplus
}
#endif

const char* JSON_FILE_NAME = "oic_unittest.json";
const char* DEFAULT_ACL_JSON_FILE_NAME = "oic_unittest_default_acl.json";
const char* ACL1_JSON_FILE_NAME = "oic_unittest_acl1.json";

#define NUM_ACE_FOR_WILDCARD_IN_ACL1_JSON (2)

char* ReadFile(const char* filename)
{

    FILE *fp = NULL;
    char *data = NULL;
    struct stat st;
    // TODO: Find the location of the executable and concatenate the SVR file name
    // before opening it.
    fp = fopen(filename, "r");
    if (fp)
    {
        if (stat(filename, &st) == 0)
        {
            data = (char*)OICMalloc(st.st_size);
            if (data)
            {
                if (fread(data, 1, st.st_size, fp) != (size_t)st.st_size)
                {
                    printf("Error in reading file %s", filename);
                }
            }
        }
        fclose(fp);
    }
    else
    {
        printf("Unable to open %s file", filename);
    }

    return data;
}

void SetPersistentHandler(OCPersistentStorage *ps, bool set)
{
    if (set)
    {
        ps->open = fopen;
        ps->read = fread;
        ps->write = fwrite;
        ps->close = fclose;
        ps->unlink = unlink;
    }
    else
    {
        memset(ps, 0, sizeof(OCPersistentStorage));
    }
    EXPECT_EQ(OC_STACK_OK,
            OCRegisterPersistentStorageHandler(ps));
}

// JSON Marshalling Tests
TEST(ACLResourceTest, JSONMarshallingTests)
{
    char *jsonStr1 = ReadFile(ACL1_JSON_FILE_NAME);
    if (jsonStr1)
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

        OicSecAcl_t * acl = JSONToAclBin(jsonStr1);
        EXPECT_TRUE(NULL != acl);

        char * jsonStr2 = BinToAclJSON(acl);
        EXPECT_TRUE(NULL != jsonStr2);

        EXPECT_STREQ(jsonStr1, jsonStr2);

        OICFree(jsonStr1);
        OICFree(jsonStr2);
        DeleteACLList(acl);
    }
}

// Default ACL tests
TEST(ACLResourceTest, GetDefaultACLTests)
{
    // Read default ACL from the file
    char *jsonStr = ReadFile(DEFAULT_ACL_JSON_FILE_NAME);
    if (jsonStr)
    {
        OicSecAcl_t * acl = JSONToAclBin(jsonStr);
        EXPECT_TRUE(NULL != acl);

        // Invoke API to generate default ACL
        OicSecAcl_t * defaultAcl = NULL;
        OCStackResult ret = GetDefaultACL(&defaultAcl);
        EXPECT_TRUE(NULL == defaultAcl);

        EXPECT_TRUE(OC_STACK_ERROR == ret);

        // Verify if the SRM generated default ACL matches with unit test default
        if (acl && defaultAcl)
        {
            EXPECT_TRUE(memcmp(&(acl->subject), &(defaultAcl->subject), sizeof(OicUuid_t)) == 0);
            EXPECT_EQ(acl->resourcesLen, defaultAcl->resourcesLen);
            for (size_t i = 0; i < acl->resourcesLen; i++)
            {
                EXPECT_EQ(strlen(acl->resources[i]), strlen(defaultAcl->resources[i]));
                EXPECT_TRUE(
                        memcmp(acl->resources[i], defaultAcl->resources[i],
                                strlen(acl->resources[i])) == 0);
            }
            EXPECT_EQ(acl->permission, defaultAcl->permission);
        }

        // Perform cleanup
        DeleteACLList(acl);
        DeleteACLList(defaultAcl);
        OICFree(jsonStr);
    }
}


// 'POST' ACL tests
TEST(ACLResourceTest, ACLPostTest)
{
    OCEntityHandlerRequest ehReq = {};

    // Read an ACL from the file
    char *jsonStr = ReadFile(ACL1_JSON_FILE_NAME);
    if (jsonStr)
    {
        static OCPersistentStorage ps =
        { };
        SetPersistentHandler(&ps, true);

        // Create Entity Handler POST request payload
        ehReq.method = OC_REST_POST;
        ehReq.payload = (OCPayload*)calloc(1, sizeof(OCSecurityPayload));
        ehReq.payload->type = PAYLOAD_TYPE_SECURITY;
        ((OCSecurityPayload*)ehReq.payload)->securityData = jsonStr;

        OCEntityHandlerResult ehRet = ACLEntityHandler(OC_REQUEST_FLAG, &ehReq);
        EXPECT_TRUE(OC_EH_ERROR == ehRet);

        // Convert JSON into OicSecAcl_t for verification
        OicSecAcl_t * acl = JSONToAclBin(jsonStr);
        EXPECT_TRUE(NULL != acl);

        // Verify if SRM contains ACL for the subject
        OicSecAcl_t* savePtr = NULL;
        const OicSecAcl_t* subjectAcl = GetACLResourceData(&(acl->subject), &savePtr);
        EXPECT_TRUE(NULL != subjectAcl);

        // Perform cleanup
        DeleteACLList(acl);
        DeInitACLResource();
        OICFree(jsonStr);
    }
}


// GetACLResource tests
TEST(ACLResourceTest, GetACLResourceTests)
{
    // gAcl is a pointer to the the global ACL used by SRM
    extern OicSecAcl_t  *gAcl;

    // Read an ACL from the file
    char *jsonStr = ReadFile(ACL1_JSON_FILE_NAME);
    if (jsonStr)
    {
        gAcl = JSONToAclBin(jsonStr);
        EXPECT_TRUE(NULL != gAcl);

        // Verify that ACL file contains 2 ACE entries for 'WILDCARD' subject
        const OicSecAcl_t* acl = NULL;
        OicSecAcl_t* savePtr = NULL;
        OicUuid_t subject = WILDCARD_SUBJECT_ID;
        int count = 0;

        do
        {
            acl = GetACLResourceData(&subject, &savePtr);
            count = (NULL != acl) ? count + 1 : count;
        } while (acl != NULL);

        EXPECT_EQ(count, NUM_ACE_FOR_WILDCARD_IN_ACL1_JSON);

        /* Perform cleanup */
        DeleteACLList(gAcl);
        gAcl = NULL;
        OICFree(jsonStr);
    }
}

