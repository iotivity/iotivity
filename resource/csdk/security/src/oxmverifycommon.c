//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
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

#include "ocstack.h"
#include "logger.h"
#include "base64.h"
#include "securevirtualresourcetypes.h"
#include "srmresourcestrings.h"
#include "cainterface.h"
#include "oxmverifycommon.h"

#define TAG "OIC_VERIFY_COMMON"

static VerifyOptionBitmask_t gVerifyOption = (DISPLAY_NUM | USER_CONFIRM);

static DisplayNumContext_t gDisplayNumContext = { .callback = NULL, .context = NULL };
static UserConfirmContext_t gUserConfirmContext = { .callback = NULL, .context = NULL };

void SetDisplayNumCB(void * ptr, DisplayNumCallback displayNumCB)
{
    if (NULL == displayNumCB)
    {
        OIC_LOG(ERROR, TAG, "Failed to set callback for displaying mutualVerifNum");
        return;
    }
    gDisplayNumContext.callback = displayNumCB;
    gDisplayNumContext.context = ptr;
}

void UnsetDisplayNumCB()
{
    gDisplayNumContext.callback = NULL;
}

void SetUserConfirmCB(void * ptr, UserConfirmCallback userConfirmCB)
{
    if (NULL == userConfirmCB)
    {
        OIC_LOG(ERROR, TAG, "Failed to set callback to confirm mutualVerifNum");
        return;
    }
    gUserConfirmContext.callback = userConfirmCB;
    gUserConfirmContext.context = ptr;
}

void UnsetUserConfirmCB()
{
    gUserConfirmContext.callback = NULL;
}

void SetVerifyOption(VerifyOptionBitmask_t verifyOption)
{
    gVerifyOption = verifyOption;
}

OCStackResult VerifyOwnershipTransfer(uint8_t mutualVerifNum [MUTUAL_VERIF_NUM_LEN],
                                    VerifyOptionBitmask_t verifyOption)
{
    verifyOption = (VerifyOptionBitmask_t)(verifyOption & gVerifyOption);
    if (verifyOption & DISPLAY_NUM)
    {
        if (!gDisplayNumContext.callback)
        {
            OIC_LOG(ERROR, TAG, "Callback for displaying verification PIN not registered");
            return OC_STACK_ERROR;
        }
        if (OC_STACK_OK != gDisplayNumContext.callback(gDisplayNumContext.context, mutualVerifNum))
        {
            OIC_LOG(ERROR, TAG, "Failed to display verification PIN");
            return OC_STACK_ERROR;
        }
    }
    if (verifyOption & USER_CONFIRM)
    {
        if (!gUserConfirmContext.callback)
        {
            OIC_LOG(ERROR, TAG, "Callback to get user confirmation not registered");
            return OC_STACK_ERROR;
        }
        if (OC_STACK_OK != gUserConfirmContext.callback(gUserConfirmContext.context))
        {
            OIC_LOG(ERROR, TAG, "Failed to get user confirmation");
            return OC_STACK_USER_DENIED_REQ;
        }
    }
    return OC_STACK_OK;
}
