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
#ifdef __cplusplus
}
#endif

const char* JSON_FILE_NAME = "oic_unittest.json";

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
#if 0
TEST(ACLResourceTest, InitACLResource)
{
    OCPersistentStorage *psi = (OCPersistentStorage*) OCMalloc(sizeof(OCPersistentStorage));
    psi->open = utopen;
    psi->read = utread;
    psi->write = utwrite;
    psi->close = utclose;
    psi->unlink = utunlink;
#if 1
    EXPECT_EQ(OC_STACK_OK, SRMRegisterPersistentStorageHandler(psi));
    EXPECT_EQ(OC_STACK_OK, InitACLResource());
    DeInitACLResource();
#else
    EXPECT_EQ(OC_STACK_OK, OCRegisterPersistentStorageHandler(psi));
    /* This should read SVR database and read ACL resource */
    EXPECT_EQ(OC_STACK_OK, OCInit(NULL, 0, OC_SERVER));
    EXPECT_EQ(OC_STACK_OK, OCStop());
#endif
    OCFree(psi);
}
#endif

TEST(ACLResourceTest, JSONMarshalliingTests)
{
    char *jsonStr1 = ReadFile(JSON_FILE_NAME);
    if (NULL != jsonStr1)
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
        EXPECT_STREQ(jsonStr1, jsonStr2);

        OCFree(jsonStr1);
        OCFree(jsonStr2);
    }
    else
    {
        printf("Please copy %s into unittest folder\n", JSON_FILE_NAME);
    }
}


