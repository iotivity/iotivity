/******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
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
#include "../include/PMCsdkAppMotHelper.h"

#if defined(__MOT__)
OCPersistentStorage g_pstMot;
int g_motDevCount = 0;
int g_motOwnedDevCount = 0;
int g_motCbInvoked = CALLBACK_NOT_INVOKED;

FILE* fopenMotClient(const char* path, const char* mode)
{
    OC_UNUSED(path);
    return fopen(MOT_CBOR_FILE, mode);
}

bool initMotClient()
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] initMotClient IN");

    // initialize persistent storage for SVR DB
    static OCPersistentStorage g_pstMot;

    g_pstMot.open = fopenMotClient;
    g_pstMot.read = fread;
    g_pstMot.write = fwrite;
    g_pstMot.close = fclose;
    g_pstMot.unlink = unlink;

    if (OC_STACK_OK != OCRegisterPersistentStorageHandler(&g_pstMot))
    {
        IOTIVITYTEST_LOG(ERROR, "OCRegisterPersistentStorageHandler Failed");
        return false;
    }

    // initialize OC stack and provisioning manager
    if (OC_STACK_OK != OCInit(NULL, 0, OC_CLIENT_SERVER))
    {
        IOTIVITYTEST_LOG(ERROR, "OCStack Initialization Failed");
        return false;
    }

    if (access(MOT_PRVN_DB_FILE_NAME, F_OK) != -1)
    {
        IOTIVITYTEST_LOG(INFO, "************************************************************");
        IOTIVITYTEST_LOG(INFO, "************Provisioning DB file already exists.************");
        IOTIVITYTEST_LOG(INFO, "************************************************************");
    }
    else
    {
        IOTIVITYTEST_LOG(INFO, "************************************************************");
        IOTIVITYTEST_LOG(INFO, "************No provisioning DB file, creating new************");
        IOTIVITYTEST_LOG(INFO, "************************************************************");
    }

    if (OC_STACK_OK != OCInitPM(MOT_PRVN_DB_FILE_NAME))
    {
        IOTIVITYTEST_LOG(ERROR, "Multiple OwnerShip Initialization Failed");
        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] initProvisionClient OUT");
    return true;
}

bool provisionPreconfPin(void* ctx, OCProvisionDev_t *targetDeviceInfo,
        const char * preconfPin, size_t preconfPinLen, OCProvisionResultCB resultCallback,
        OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] OCProvisionPreconfPin IN");

    g_motCbInvoked = false;

    OCStackResult res = OCProvisionPreconfigPin(ctx, targetDeviceInfo, preconfPin, preconfPinLen,
     resultCallback);
    IOTIVITYTEST_LOG(INFO, "[PMHelper] OCProvisionPreconfPin API returns: %s\n",
            getOCStackResult(res));

    if (expectedResult != res)
    {
        IOTIVITYTEST_LOG(ERROR, "Expected Result Mismatch");
        return false;
    }

    if (OC_STACK_OK == res)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRetMot())
        {
            IOTIVITYTEST_LOG(ERROR, "Callback not Invoked");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] OCProvisionPreconfPin OUT");
    return true;
}

bool changeMOTMode(void *ctx, const OCProvisionDev_t *targetDeviceInfo,
        const OicSecMomType_t momType, OCProvisionResultCB resultCallback,
        OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] OCChangeMOTMode IN");

    g_motCbInvoked = false;

    OCStackResult res = OCChangeMOTMode(ctx, targetDeviceInfo, momType, resultCallback);
    IOTIVITYTEST_LOG(INFO, "[PMHelper]  OCChangeMOTMode API returns: %s\n", getOCStackResult(res));

    if (expectedResult != res)
    {
        IOTIVITYTEST_LOG(ERROR, "Expected Result Mismatch");
        return false;
    }

    if (OC_STACK_OK == res)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRetMot())
        {
            IOTIVITYTEST_LOG(ERROR, "Callback not Invoked");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] OCChangeMOTMode OUT");
    return true;
}

bool selectMOTMethod(void *ctx, const OCProvisionDev_t *targetDeviceInfo,
        const OicSecOxm_t oxmSelValue, OCProvisionResultCB resultCallback,
        OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] OCSelectMOTMethod IN");

    g_motCbInvoked = false;

    OCStackResult res = OCSelectMOTMethod(ctx, targetDeviceInfo, oxmSelValue, resultCallback);
    IOTIVITYTEST_LOG(INFO, "[PMHelper] OCSelectMOTMethod API returns: %s\n", getOCStackResult(res));

    if (expectedResult != res)
    {
        IOTIVITYTEST_LOG(ERROR, "Expected Result Mismatch");
        return false;
    }

    if (OC_STACK_OK == res)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRetMot())
        {
            IOTIVITYTEST_LOG(ERROR, "Callback not Invoked");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] OCSelectMOTMethod OUT");
    return true;
}

void provisionPreconfPinCB(void* ctx, int nOfRes, OCProvisionResult_t* arr, bool hasError)
{
    if (!hasError)
    {
        IOTIVITYTEST_LOG(INFO, "provisionPreconfPinCB SUCCEEDED - ctx: %s", (char* ) ctx);
    }
    else
    {
        IOTIVITYTEST_LOG(ERROR, "provisionPreconfPinCB - ctx: %s", (char* ) ctx);
        printResultList((const OCProvisionResult_t*) arr, nOfRes);
    }

    g_motCbInvoked = true;
}

void changeMOTModeCB(void* ctx, int nOfRes, OCProvisionResult_t* arr, bool hasError)
{
    if (!hasError)
    {
        IOTIVITYTEST_LOG(INFO, "changeMOTModeCB SUCCEEDED - ctx: %s", (char* ) ctx);
    }
    else
    {
        IOTIVITYTEST_LOG(ERROR, "changeMOTModeCB FAILED - ctx: %s", (char* ) ctx);
        printResultList((const OCProvisionResult_t*) arr, nOfRes);
    }

    g_motCbInvoked = true;
}

void selectMOTMethodCB(void* ctx, int nOfRes, OCProvisionResult_t* arr, bool hasError)
{
    if (!hasError)
    {
        IOTIVITYTEST_LOG(INFO, "selectMOTMethodCB SUCCEEDED - ctx: %s", (char* ) ctx);
    }
    else
    {
        IOTIVITYTEST_LOG(ERROR, "selectMOTMethodCB FAILED - ctx: %s", (char* ) ctx);
        printResultList((const OCProvisionResult_t*) arr, nOfRes);
    }

    g_motCbInvoked = true;
}

bool discoverMultipleOwnerEnabledDevices(int nTime, OCProvisionDev_t** motdev_list,
        OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] discoverMultipleOwnerEnabledDevices IN");

    OCStackResult res = OCDiscoverMultipleOwnerEnabledDevices(nTime, motdev_list);
    IOTIVITYTEST_LOG(INFO, "[PMHelper] OCDiscoverMultipleOwnerEnabledDevices API returns: %s\n",
            getOCStackResult(res));

    if (expectedResult != res)
    {
        IOTIVITYTEST_LOG(ERROR, "Expected Result Mismatch");
        return false;
    }

    IOTIVITYTEST_LOG(INFO, "[PMHelper] Discovered Multiple Owner Enabled Devices List :");

    if (motdev_list != NULL)
    {
        g_motDevCount = printDevList(*motdev_list);
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] discoverMultipleOwnerEnabledDevices OUT");
    return true;
}

bool discoverMultipleOwnedDevices(int nTime, OCProvisionDev_t** motOnwedDev_list,
        OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] discoverMultipleOwnedDevices IN");

    OCStackResult res = OCDiscoverMultipleOwnedDevices(nTime, motOnwedDev_list);
    IOTIVITYTEST_LOG(INFO, "[PMHelper] OCDiscoverMultipleOwnerEnabledDevices API returns: %s\n",
            getOCStackResult(res));

    if (expectedResult != res)
    {
        IOTIVITYTEST_LOG(ERROR, "Expected Result Mismatch");
        return false;
    }

    IOTIVITYTEST_LOG(INFO, "[PMHelper] Discovered Multiple Owner Enabled Devices List :");

    if (motOnwedDev_list != NULL)
    {
        g_motOwnedDevCount = printDevList(*motOnwedDev_list);
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] discoverMultipleOwnedDevices OUT");
    return true;
}

bool addPreconfigPIN(const OCProvisionDev_t *targetDeviceInfo,
        const char* preconfPIN, size_t preconfPINLen, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] addPreconfigPIN IN");

    OCStackResult res = OCAddPreconfigPin(targetDeviceInfo, preconfPIN, preconfPINLen);
    IOTIVITYTEST_LOG(INFO, "[PMHelper] OCAddPreconfigPin API returns: %s\n",
            getOCStackResult(res));

    if (expectedResult != res)
    {
        IOTIVITYTEST_LOG(ERROR, "Expected Result Mismatch");
        return false;
    }
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] addPreconfigPIN OUT");
    return true;
}

bool doMultipleOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices,
        OCProvisionResultCB resultCallback, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] doMultipleOwnershipTransfer IN");

    IOTIVITYTEST_LOG(INFO, "[PMHelper] Printing MOT Device List to be owned IN");
    g_motDevCount = printDevList(targetDevices);

    g_motCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult res = OCDoMultipleOwnershipTransfer(ctx, targetDevices, resultCallback);
    IOTIVITYTEST_LOG(INFO, "[PMHelper] OCDoMultipleOwnershipTransfer API returns: %s",
            getOCStackResult(res));

    if (expectedResult != res)
    {
        IOTIVITYTEST_LOG(ERROR, "Expected Result Mismatch");
        return false;
    }

    if (OC_STACK_OK == res)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRetMot())
        {
            IOTIVITYTEST_LOG(ERROR, "Callback not Invoked");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] doMultipleOwnershipTransfer OUT");
    return true;
}

void multipleOwnershipTransferCB(void* ctx, int nOfRes, OCProvisionResult_t* arr, bool hasError)
{
    if (!hasError)
    {
        IOTIVITYTEST_LOG(INFO, "Multiple Ownership Transfer SUCCEEDED - ctx: %s", (char* ) ctx);
    }
    else
    {
        IOTIVITYTEST_LOG(ERROR, "Ownership Transfer FAILED - ctx: %s", (char* ) ctx);
        printResultList((const OCProvisionResult_t*) arr, nOfRes);
    }

    g_motCbInvoked = CALLBACK_INVOKED;
}

int waitCallbackRetMot(void)
{
    IOTIVITYTEST_LOG(DEBUG, "waitCallbackRetMot IN");

    for (int i = 0; CALLBACK_TIMEOUT > i; ++i)
    {
        IOTIVITYTEST_LOG(INFO, "waitCallbackRetMot Loop = %d", i);

        if (CALLBACK_INVOKED == g_motCbInvoked)
        {
            return CALLBACK_INVOKED;
        }

        CommonUtil::waitInSecond(DELAY_SHORT);

        if (OC_STACK_OK != OCProcess())
        {
            IOTIVITYTEST_LOG(ERROR, "OCStack Process Failed");
            return CALLBACK_NOT_INVOKED;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "waitCallbackRetMot OUT");
    return CALLBACK_NOT_INVOKED;
}

#endif/*#if defined(__MOT__)*/
