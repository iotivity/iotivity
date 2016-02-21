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
#include "oxmjustworks.h"
#include "oxmrandompin.h"
#include "ownershiptransfermanager.h"
#include "ocstack.h"
#include "utlist.h"

using namespace std;

TEST(JustWorksOxMTest, NullParam)
{
    OTMContext_t* otmCtx = NULL;
    uint8_t *payloadRes = NULL;
    size_t size = 0;

    //LoadSecretJustWorksCallback always returns OC_STACK_OK.
    EXPECT_EQ(OC_STACK_OK, LoadSecretJustWorksCallback(otmCtx));

    EXPECT_EQ(OC_STACK_INVALID_PARAM, CreateSecureSessionJustWorksCallback(otmCtx));

    EXPECT_EQ(OC_STACK_INVALID_PARAM, CreateJustWorksSelectOxmPayload(otmCtx, &payloadRes, &size));
    EXPECT_TRUE(NULL == payloadRes);

    EXPECT_EQ(OC_STACK_INVALID_PARAM, CreateJustWorksOwnerTransferPayload(otmCtx, &payloadRes, &size));
    EXPECT_TRUE(NULL == payloadRes);

    OTMContext_t otmCtx2;
    otmCtx2.selectedDeviceInfo = NULL;

    //LoadSecretJustWorksCallback always returns OC_STACK_OK.
    EXPECT_EQ(OC_STACK_OK, LoadSecretJustWorksCallback(&otmCtx2));

    EXPECT_EQ(OC_STACK_INVALID_PARAM, CreateSecureSessionJustWorksCallback(&otmCtx2));

    EXPECT_EQ(OC_STACK_INVALID_PARAM, CreateJustWorksSelectOxmPayload(&otmCtx2, &payloadRes, &size));
    EXPECT_TRUE(NULL == payloadRes);

    EXPECT_EQ(OC_STACK_INVALID_PARAM, CreateJustWorksOwnerTransferPayload(&otmCtx2, &payloadRes, &size));
    EXPECT_TRUE(NULL == payloadRes);
}

TEST(RandomPinOxMTest, NullParam)
{
    OTMContext_t* otmCtx = NULL;
    uint8_t *payloadRes = NULL;
    size_t size = 0;

    //LoadSecretJustWorksCallback always returns OC_STACK_OK.
    EXPECT_EQ(OC_STACK_INVALID_PARAM, InputPinCodeCallback(otmCtx));

    EXPECT_EQ(OC_STACK_INVALID_PARAM, CreateSecureSessionRandomPinCallback(otmCtx));

    EXPECT_EQ(OC_STACK_INVALID_PARAM, CreatePinBasedSelectOxmPayload(otmCtx, &payloadRes, &size));
    EXPECT_TRUE(NULL == payloadRes);

    EXPECT_EQ(OC_STACK_INVALID_PARAM, CreatePinBasedOwnerTransferPayload(otmCtx, &payloadRes, &size));
    EXPECT_TRUE(NULL == payloadRes);

    OTMContext_t otmCtx2;
    otmCtx2.selectedDeviceInfo = NULL;

    //LoadSecretJustWorksCallback always returns OC_STACK_OK.
    EXPECT_EQ(OC_STACK_INVALID_PARAM, InputPinCodeCallback(&otmCtx2));

    EXPECT_EQ(OC_STACK_INVALID_PARAM, CreateSecureSessionRandomPinCallback(&otmCtx2));

    EXPECT_EQ(OC_STACK_INVALID_PARAM, CreatePinBasedSelectOxmPayload(&otmCtx2, &payloadRes, &size));
    EXPECT_TRUE(NULL == payloadRes);

    EXPECT_EQ(OC_STACK_INVALID_PARAM, CreatePinBasedOwnerTransferPayload(&otmCtx2, &payloadRes, &size));
    EXPECT_TRUE(NULL == payloadRes);
}
