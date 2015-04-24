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
#include "ocmalloc.h"
#include "cJSON.h"
#include "cainterface.h"
#include "securityresourcemanager.h"
#include "securevirtualresourcetypes.h"
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
#ifdef __cplusplus
}
#endif

const char* JSON_FILE_NAME = "oic_unittest.json";
const char* DEFAULT_ACL_JSON_FILE_NAME = "oic_unittest_default_acl.json";
const char* ACL1_JSON_FILE_NAME = "oic_unittest_acl1.json";

char* ReadFile(const char* filename)
{

    FILE *fp = NULL;
    char *data = NULL;
    struct stat st;

    fp = fopen(filename, "r");
    if (fp)
    {
        if (stat(filename, &st) == 0)
        {
            data = (char*)OCMalloc(st.st_size);
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


//ACLResource Tests
TEST(ACLResourceTest, JSONMarshalliingTests)
{
    char *jsonStr1 = ReadFile(ACL1_JSON_FILE_NAME);
    EXPECT_TRUE(NULL != jsonStr1);
    if (NULL == jsonStr1)
    {
        printf("Please copy %s into unittest folder\n", ACL1_JSON_FILE_NAME);
    }

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

    if( jsonStr1 && jsonStr2)
    {
        EXPECT_STREQ(jsonStr1, jsonStr2);
    }

    OCFree(jsonStr1);
    OCFree(jsonStr2);
    DeleteACLList(acl);
}


TEST(ACLResourceTest, GetDefaultACLTests)
{
    char *jsonStr = ReadFile(DEFAULT_ACL_JSON_FILE_NAME);
    EXPECT_TRUE(NULL != jsonStr);
    if (NULL == jsonStr)
    {
        printf("Please copy %s into unittest folder\n", DEFAULT_ACL_JSON_FILE_NAME);
    }

    /* Read from the file */
    OicSecAcl_t * acl = JSONToAclBin(jsonStr);
    EXPECT_TRUE(NULL != acl);

    /* Invoke API to generate default ACL */
    OicSecAcl_t * defaultAcl = NULL;
    OCStackResult ret = GetDefaultACL(&defaultAcl);

    EXPECT_TRUE(NULL != defaultAcl);
    EXPECT_TRUE(OC_STACK_OK == ret);

    /* Verify if the source code generated what is stored in file */
    if (acl && defaultAcl)
    {
        EXPECT_TRUE(memcmp(&(acl->subject), &(defaultAcl->subject), sizeof(OicUuid_t)) == 0);
        EXPECT_EQ(acl->resourcesLen, defaultAcl->resourcesLen);
        for (size_t i =0; i < acl->resourcesLen; i++)
        {
            EXPECT_EQ(strlen(acl->resources[i]), strlen(defaultAcl->resources[i]));
            EXPECT_TRUE(memcmp(acl->resources[i], defaultAcl->resources[i], strlen(acl->resources[i])) == 0);
        }
        EXPECT_EQ(acl->permission, defaultAcl->permission);
    }

    /* Perform cleanup */
    DeleteACLList(acl);
    DeleteACLList(defaultAcl);
    OCFree(jsonStr);
}


