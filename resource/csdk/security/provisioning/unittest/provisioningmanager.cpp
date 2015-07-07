/* *****************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * *****************************************************************/

#include "gtest/gtest.h"

#include "provisioningmanager.h"


static OicSecAcl_t acl;
SPTargetDeviceInfo_t list;
SPTargetDeviceInfo_t *ptr = &list;

TEST(SPProvisioningDiscoveryTest, NotNullList)
{
    EXPECT_EQ(SP_RESULT_INVALID_PARAM, SPProvisioningDiscovery(0, &ptr));
}

TEST(SPInitProvisionContextTest, NullDeviceInfo)
{
    EXPECT_EQ(SP_RESULT_INVALID_PARAM, SPInitProvisionContext(0, NULL));
}

TEST(SPProvisionACLTest, NullDeviceInfo)
{
    EXPECT_EQ(SP_RESULT_INVALID_PARAM, SPProvisionACL(0, NULL, &acl));
}

TEST(SPFinalizeProvisioningTest, NullDeviceInfo)
{
    EXPECT_EQ(SP_RESULT_INVALID_PARAM, SPFinalizeProvisioning(0, NULL));
}

TEST(SPTerminateProvisioningTest, ValidCase)
{
    EXPECT_EQ(SP_RESULT_SUCCESS, SPTerminateProvisioning());
}

TEST(SPProvisionCredentialsTest, NullList)
{
    EXPECT_EQ(SP_RESULT_INVALID_PARAM, SPProvisionCredentials(0, 0, NULL));
}
