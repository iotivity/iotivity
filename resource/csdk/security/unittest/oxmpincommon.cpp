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
#include "pinoxmcommon.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "../src/oxmpincommon.c"

#undef TAG
#ifdef __cplusplus
}
#endif

#define TAG  "OXM-PIN-UT"

static void OC_CALL inputPinCB(char* pin, size_t len)
{
    if(NULL == pin || OXM_RANDOM_PIN_MIN_SIZE > len || len > OXM_RANDOM_PIN_MAX_SIZE)
    {
        OIC_LOG(ERROR, TAG, "inputPinCB invalid parameters");
        return;
    }

    printf("   > INPUT PIN: ");
    for(int ret=0; 1!=ret; )
    {
        ret = scanf("%32s", pin);
        for( ; 0x20<=getchar(); );  // for removing overflow garbages
                                    // '0x20<=code' is character region
    }
}

static void OC_CALL inputPinWithCtxCB(OicUuid_t deviceId, char *pin, size_t len, void *context)
{
    OC_UNUSED(deviceId);
    OC_UNUSED(context);

    if(NULL == pin || OXM_RANDOM_PIN_MIN_SIZE > len || len > OXM_RANDOM_PIN_MAX_SIZE)
    {
        OIC_LOG(ERROR, TAG, "inputPinCB invalid parameters");
        return;
    }

    printf("   > INPUT PIN: ");
    for(int ret=0; 1!=ret; )
    {
        ret = scanf("%32s", pin);
        for( ; 0x20<=getchar(); );  // for removing overflow garbages
                                    // '0x20<=code' is character region
    }
}

static void OC_CALL generatePinCB(char* pin, size_t pinSize)
{
    if (NULL == pin || pinSize <= 0)
    {
        OIC_LOG(INFO, TAG, "Invalid PIN");
        return;
    }

    OIC_LOG(INFO, TAG, "============================");
    OIC_LOG_V(INFO, TAG, "    PIN CODE : %s", pin);
    OIC_LOG(INFO, TAG, "============================");
}

static void OC_CALL displayPinCB(char *pin, size_t pinSize, void *context)
{
    OC_UNUSED(context);

    if(NULL == pin || pinSize <= 0)
    {
        OIC_LOG(INFO, TAG, "Invalid PIN");
        return;
    }

    OIC_LOG(INFO, TAG, "============================");
    OIC_LOG_V(INFO, TAG, "    PIN CODE : %s", pin);
    OIC_LOG(INFO, TAG, "============================");
}

static void OC_CALL closePinDisplayCB(void)
{
    OIC_LOG(INFO, TAG, "============================");
    OIC_LOG(INFO, TAG, "    PIN DISPLAY CLOSED.");
    OIC_LOG(INFO, TAG, "============================");
}

TEST(SetRandomPinPolicyTest, InvalidMinPinSize)
{
    size_t pinSize = OXM_RANDOM_PIN_MIN_SIZE - 1;
    EXPECT_EQ(OC_STACK_INVALID_PARAM, SetRandomPinPolicy(pinSize, NUM_PIN));
}

TEST(SetRandomPinPolicyTest, InvalidMaxPinSize)
{
    size_t pinSize = OXM_RANDOM_PIN_MAX_SIZE + 1;
    EXPECT_EQ(OC_STACK_INVALID_PARAM, SetRandomPinPolicy(pinSize, NUM_PIN));
}

TEST(SetRandomPinPolicyTest, InvalidPinType)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, SetRandomPinPolicy(OXM_RANDOM_PIN_MIN_SIZE, (OicSecPinType_t)(NUM_PIN + 1)));
}

TEST(SetInputPinCBTest, InvalidCallback)
{
    SetInputPinCB(NULL);
}

TEST(SetInputPinCBTest, Full)
{
    InputPinCallback callBack = inputPinCB;
    InputPinCallbackWithContext callBackWithCtx = inputPinWithCtxCB;
    g_inputPinCallbacks.contextCallback = callBackWithCtx;
    SetInputPinCB(callBack);

    g_inputPinCallbacks.contextCallback = NULL;
    SetInputPinCB(callBack);
}

TEST(SetInputPinWithContextCBTest, InvalidCallback)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, SetInputPinWithContextCB(NULL, NULL));
}

TEST(SetInputPinWithContextCBTest, Full)
{
    InputPinCallback callBack = inputPinCB;
    InputPinCallbackWithContext callBackWithCtx = inputPinWithCtxCB;
    g_inputPinCallbacks.callback = callBack;
    g_inputPinCallbacks.contextCallback = callBackWithCtx;
    EXPECT_EQ(OC_STACK_DUPLICATE_REQUEST, SetInputPinWithContextCB(callBackWithCtx, NULL));

    g_inputPinCallbacks.callback = NULL;
    g_inputPinCallbacks.contextCallback = NULL;
    EXPECT_EQ(OC_STACK_OK, SetInputPinWithContextCB(callBackWithCtx, NULL));
}

TEST(SetGeneratePinCBTest, Full)
{
    SetGeneratePinCB(NULL);

    GeneratePinCallback pinCB = generatePinCB;
    DisplayPinCallbackWithContext ctxPinCB = displayPinCB;
    g_displayPinCallbacks.callback = pinCB;
    g_displayPinCallbacks.contextCallback = ctxPinCB;
    SetGeneratePinCB(pinCB);

    g_displayPinCallbacks.callback = NULL;
    g_displayPinCallbacks.contextCallback = NULL;
    SetGeneratePinCB(pinCB);
}

TEST(SetDisplayPinWithContextCBTest, Full)
{
    EXPECT_EQ(OC_STACK_INVALID_PARAM, SetDisplayPinWithContextCB(NULL, NULL));

    GeneratePinCallback pinCB = generatePinCB;
    DisplayPinCallbackWithContext ctxPinCB = displayPinCB;
    g_displayPinCallbacks.callback = pinCB;
    g_displayPinCallbacks.contextCallback = ctxPinCB;
    EXPECT_EQ(OC_STACK_DUPLICATE_REQUEST, SetDisplayPinWithContextCB(ctxPinCB, NULL));

    g_displayPinCallbacks.callback = NULL;
    g_displayPinCallbacks.contextCallback = NULL;
    EXPECT_EQ(OC_STACK_OK, SetDisplayPinWithContextCB(ctxPinCB, NULL));
}

TEST(SetClosePinDisplayCBTest, Full)
{
    SetClosePinDisplayCB(NULL);

    ClosePinDisplayCallback closeCB = closePinDisplayCB;
    g_displayPinCallbacks.closePinDisplayCallback = closeCB;
    SetClosePinDisplayCB(closeCB);

    g_displayPinCallbacks.closePinDisplayCallback = NULL;
    SetClosePinDisplayCB(closeCB);
}

TEST(UnsetInputPinWithContextCBTest, Simple)
{
    UnsetInputPinWithContextCB();
}

TEST(UnsetDisplayPinWithContextCBTest, Simple)
{
    UnsetDisplayPinWithContextCB();
}

TEST(UnsetClosePinDisplayCBTest, Simple)
{
    UnsetClosePinDisplayCB();
}

TEST(ClosePinDisplayTest, Simple)
{
    ClosePinDisplay();
}
