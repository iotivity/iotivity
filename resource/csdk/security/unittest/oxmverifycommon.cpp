/* *****************************************************************
 *
 * Copyright 2019 Samsung Electronics All Rights Reserved.
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
#include <gtest/gtest.h>
#include "octypes.h"
#include "oxmverifycommon.h"

#define TAG  "OXM-VERIFY-UT"

static VerifyOptionBitmask_t gVerifyOption = (VerifyOptionBitmask_t)(DISPLAY_NUM | USER_CONFIRM);

static DisplayNumContext_t gDisplayNumContext = { .callback = NULL, .context = NULL };
static UserConfirmContext_t gUserConfirmContext = { .callback = NULL, .context = NULL };

TEST(SetDisplayNumCBTest, InvalidCallback)
{
    SetDisplayNumCB(NULL, NULL);
}

TEST(UnsetDisplayNumCBTest, Simple)
{
    UnsetDisplayNumCB();
}

TEST(UnsetUserConfirmCBTest, Simple)
{
    UnsetUserConfirmCB();
}

TEST(SetUserConfirmCBTest, InvalidCallback)
{
    SetUserConfirmCB(NULL, NULL);
}

TEST(SetVerifyOptionTest, Simple)
{
    SetVerifyOption(DISPLAY_NUM);
}

TEST(VerifyOwnershipTransferTest, InvalidDisplayNumCB)
{
    gDisplayNumContext.callback = NULL;
    gVerifyOption = (VerifyOptionBitmask_t)DISPLAY_NUM;
    EXPECT_EQ(OC_STACK_ERROR, VerifyOwnershipTransfer(NULL, (VerifyOptionBitmask_t)DISPLAY_NUM));
}

TEST(VerifyOwnershipTransferTest, InvalidUserConfirmCB)
{
    gUserConfirmContext.callback = NULL;
    gVerifyOption = (VerifyOptionBitmask_t)USER_CONFIRM;
    EXPECT_EQ(OC_STACK_ERROR, VerifyOwnershipTransfer(NULL, (VerifyOptionBitmask_t)USER_CONFIRM));
}
