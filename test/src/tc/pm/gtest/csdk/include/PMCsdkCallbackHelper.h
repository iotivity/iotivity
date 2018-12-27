/******************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      LICENSE-2.0" target="_blank">http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 ******************************************************************/

#ifndef PMCsdkCallbackHelper_H_
#define PMCsdkCallbackHelper_H_

#include "PMCsdkUtilityHelper.h"

/**
 * Helper Class for Provisioning Manager
 */
class PMCsdkCallbackHelper
{

public:

    static bool s_CBInvoked;

    PMCsdkCallbackHelper();
    static void callbackWrapper(bool hasError, size_t nOfRes, void* ctx, OCProvisionResult_t* arr);
    static void provisionGetCB(void* ctx, size_t nOfRes, OCProvisionResult_t* arr,
    bool hasError);
    static void provisionPutCB(void* ctx, size_t nOfRes, OCProvisionResult_t* arr,
    bool hasError);
    static void provisionPostCB(void* ctx, size_t nOfRes, OCProvisionResult_t* arr,
    bool hasError);
    static void provisionDeleteCB(void* ctx, size_t nOfRes, OCProvisionResult_t* arr,
    bool hasError);
};
#endif
