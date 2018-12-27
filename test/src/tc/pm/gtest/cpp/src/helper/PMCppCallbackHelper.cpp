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

#include "PMCppHelper.h"
#include "PMCppCallbackHelper.h"
#include "PMCppUtilityHelper.h"

int PMCppCallbackHelper::s_isCBInvoked = CALLBACK_NOT_INVOKED;


void PMCppCallbackHelper::callbackWrapper(PMResultList_t *result, int hasError)
{
    if (hasError)
    {
        IOTIVITYTEST_LOG(ERROR, "[PMCppHelper] Provisioning ERROR %d!!!", hasError);
    }
    else
    {
        IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] Received provisioning results: ");

        for (unsigned int i = 0; i < result->size(); i++)
        {
            std::cout << "Result is = " << result->at(i).res << " for device ";
            PMCppUtilityHelper::printUuid(result->at(i).deviceId);
        }
        PMCppCallbackHelper::s_isCBInvoked = CALLBACK_INVOKED;
        delete result;
    }
}

void PMCppCallbackHelper::provisionGetCB(PMResultList_t *result, int hasError)
{
    __FUNC_IN__

    PMCppCallbackHelper::callbackWrapper(result, hasError);

    __FUNC_OUT__
}

void PMCppCallbackHelper::provisionPutCB(PMResultList_t *result, int hasError)
{
    __FUNC_IN__

    PMCppCallbackHelper::callbackWrapper(result, hasError);

    __FUNC_OUT__
}

void PMCppCallbackHelper::provisionPostCB(PMResultList_t *result, int hasError)
{
    __FUNC_IN__

    PMCppCallbackHelper::callbackWrapper(result, hasError);

    __FUNC_OUT__
}

void PMCppCallbackHelper::provisionDeleteCB(PMResultList_t *result, int hasError)
{
    __FUNC_IN__

    PMCppCallbackHelper::callbackWrapper(result, hasError);

    __FUNC_OUT__
}


