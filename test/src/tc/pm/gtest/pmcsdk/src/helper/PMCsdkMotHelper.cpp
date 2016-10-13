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
#include "PMCsdkMotHelper.h"
#include "PMCsdkUtilityHelper.h"

#if defined(__MOT__)
OCPersistentStorage g_pstMot;
int g_motDevCount = 0;
int g_motOwnedDevCount = 0;
int g_motCbInvoked = CALLBACK_NOT_INVOKED;

PMCsdkMotHelper::PMCsdkMotHelper()
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] Constructor");
}

FILE* PMCsdkMotHelper::fopenMotClient(const char* path, const char* mode)
{
    return fopen(MOT_CBOR_FILE, mode);
}

bool PMCsdkMotHelper::initMotClient()
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] initMotClient IN");

    // initialize persistent storage for SVR DB
    static OCPersistentStorage g_pstMot;

    g_pstMot.open = PMCsdkMotHelper::fopenMotClient;
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
        printf("************************************************************\n");
        printf("************Provisioning DB file already exists.************\n");
        printf("************************************************************\n");
    }
    else
    {
        printf("*************************************************************\n");
        printf("************No provisioning DB file, creating new************\n");
        printf("*************************************************************\n");
    }

    if (OC_STACK_OK != OCInitPM(MOT_PRVN_DB_FILE_NAME))
    {
        IOTIVITYTEST_LOG(ERROR, "Multiple OwnerShip Initialization Failed");
        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] initProvisionClient OUT");
    return true;
}

bool PMCsdkMotHelper::discoverMultipleOwnerEnabledDevices(int nTime, OCProvisionDev_t** motdev_list,
        OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] discoverMultipleOwnerEnabledDevices IN");

    OCStackResult res = OCDiscoverMultipleOwnerEnabledDevices(nTime, motdev_list);
    IOTIVITYTEST_LOG(INFO, "[PMHelper] OCDiscoverMultipleOwnerEnabledDevices API returns: %s\n",
            PMCsdkUtilityHelper::getOCStackResult(res));

    if (expectedResult != res)
    {
        m_failureMessage = setFailureMessage(res, expectedResult);
        return false;
    }

    IOTIVITYTEST_LOG(INFO, "[PMHelper] Discovered Multiple Owner Enabled Devices List :");

    if (motdev_list != NULL)
    {
        g_motDevCount = PMCsdkUtilityHelper::printDevList(*motdev_list);
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] discoverMultipleOwnerEnabledDevices OUT");
    return true;
}

bool PMCsdkMotHelper::discoverMultipleOwnedDevices(int nTime, OCProvisionDev_t** motOnwedDev_list,
        OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] discoverMultipleOwnedDevices IN");

    OCStackResult res = OCDiscoverMultipleOwnedDevices(nTime, motOnwedDev_list);
    IOTIVITYTEST_LOG(INFO, "[PMHelper] OCDiscoverMultipleOwnerEnabledDevices API returns: %s\n",
            PMCsdkUtilityHelper::getOCStackResult(res));

    if (expectedResult != res)
    {
        m_failureMessage = setFailureMessage(res, expectedResult);
        return false;
    }

    IOTIVITYTEST_LOG(INFO, "[PMHelper] Discovered Multiple Owner Enabled Devices List :");

    if (motOnwedDev_list != NULL)
    {
        g_motOwnedDevCount = PMCsdkUtilityHelper::printDevList(*motOnwedDev_list);
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] discoverMultipleOwnedDevices OUT");
    return true;
}

bool PMCsdkMotHelper::addPreconfigPIN(const OCProvisionDev_t *targetDeviceInfo,
        const char* preconfPIN, size_t preconfPINLen, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] addPreconfigPIN IN");

    OCStackResult res = OCAddPreconfigPIN(targetDeviceInfo, preconfPIN, preconfPINLen);
    IOTIVITYTEST_LOG(INFO, "[PMHelper] OCAddPreconfigPIN API returns: %s\n",
            PMCsdkUtilityHelper::getOCStackResult(res));

    if (expectedResult != res)
    {
        m_failureMessage = setFailureMessage(res, expectedResult);
        return false;
    }
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] addPreconfigPIN OUT");
    return true;
}

bool PMCsdkMotHelper::doMultipleOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices,
        OCProvisionResultCB resultCallback, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] doMultipleOwnershipTransfer IN");

    IOTIVITYTEST_LOG(INFO, "[PMHelper] Printing MOT Device List to be owned IN");
    g_motDevCount = PMCsdkUtilityHelper::printDevList(targetDevices);

    g_motCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult res = OCDoMultipleOwnershipTransfer(ctx, targetDevices, resultCallback);
    IOTIVITYTEST_LOG(INFO, "[PMHelper] OCDoMultipleOwnershipTransfer API returns: %s",
            PMCsdkUtilityHelper::getOCStackResult(res));

    if (expectedResult != res)
    {
        m_failureMessage = setFailureMessage(res, expectedResult);
        return false;
    }

    if (OC_STACK_OK == res)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            m_failureMessage = setFailureMessage("CALLBACK Not invoked");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] doMultipleOwnershipTransfer OUT");
    return true;
}

std::string PMCsdkMotHelper::getFailureMessage()
{
    return m_failureMessage;
}

void PMCsdkMotHelper::multipleOwnershipTransferCB(void* ctx, int nOfRes, OCProvisionResult_t* arr, bool hasError)
{
    if (!hasError)
    {
        IOTIVITYTEST_LOG(INFO, "Multiple Ownership Transfer SUCCEEDED - ctx: %s", (char* ) ctx);
    }
    else
    {
        IOTIVITYTEST_LOG(ERROR, "Ownership Transfer FAILED - ctx: %s", (char* ) ctx);
        PMCsdkUtilityHelper::printResultList((const OCProvisionResult_t*) arr, nOfRes);
    }

    g_motCbInvoked = CALLBACK_INVOKED;
}

int PMCsdkMotHelper::waitCallbackRet(void)
{
    IOTIVITYTEST_LOG(DEBUG, "waitCallbackRet IN");

    for (int i = 0; CALLBACK_TIMEOUT > i; ++i)
    {
        IOTIVITYTEST_LOG(INFO, "waitCallbackRet Loop = %d", i);

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

    IOTIVITYTEST_LOG(DEBUG, "waitCallbackRet OUT");
    return CALLBACK_NOT_INVOKED;
}

/**
 * Function to set failure message
 */
std::string PMCsdkMotHelper::setFailureMessage(OCStackResult actualResult,
        OCStackResult expectedResult)
{
    std::string errorMessage("\033[1;31m[Error] Expected : ");
    errorMessage.append(PMCsdkUtilityHelper::getOCStackResult(expectedResult));
    errorMessage.append("\033[0m  \033[1;31mActual : ");
    errorMessage.append(PMCsdkUtilityHelper::getOCStackResult(actualResult));
    errorMessage.append("\033[0m");
    return errorMessage;
}

/**
 * Function to set failure message
 */
std::string PMCsdkMotHelper::setFailureMessage(std::string errorMessage)
{
    std::string retErrorMessage("\033[1;31m[Error] Expected : ");
    retErrorMessage.append(errorMessage);
    retErrorMessage.append("\033[0m");

    return retErrorMessage;
}
#endif/*#if defined(__MOT__)*/
