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
#include "ocstack.h"
#include "cainterface.h"

using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

#include "policyengine.h"

// test parameters
PEContext_t g_peContext;

#ifdef __cplusplus
}
#endif

OicUuid_t g_subjectIdA = {"SubjectA"};
OicUuid_t g_subjectIdB = {"SubjectB"};
char g_resource1[] = "Resource1";
char g_resource2[] = "Resource2";

//Policy Engine Core Tests
TEST(PolicyEngineCore, InitPolicyEngine)
{
    EXPECT_EQ(OC_STACK_OK, InitPolicyEngine(&g_peContext));
}

TEST(PolicyEngineCore, CheckPermissionNoAcls)
{
    EXPECT_EQ(ACCESS_DENIED_SUBJECT_NOT_FOUND,
        CheckPermission(&g_peContext,
                        &g_subjectIdA,
                        g_resource1,
                        PERMISSION_READ));
    EXPECT_EQ(0, memcmp(g_peContext.subject,
                        &WILDCARD_SUBJECT_ID,
                        sizeof(OicUuid_t)));
    EXPECT_EQ(0, strcmp(g_peContext.resource, g_resource1));
}

TEST(PolicyEngineCore, DeInitPolicyEngine)
{
    DeInitPolicyEngine(&g_peContext);
    EXPECT_EQ(STOPPED, g_peContext.state);
    EXPECT_EQ(NULL, g_peContext.subject);
    EXPECT_EQ(NULL, g_peContext.resource);
    EXPECT_EQ((uint16_t)0, g_peContext.permission);
    EXPECT_EQ(0, g_peContext.matchingAclFound); // TODO 'false' => compiler warn
    EXPECT_EQ(ACCESS_DENIED_POLICY_ENGINE_ERROR, g_peContext.retVal);
}
