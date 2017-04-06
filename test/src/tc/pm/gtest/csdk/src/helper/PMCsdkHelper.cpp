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
#include "PMCsdkHelper.h"
#include "PMCsdkUtilityHelper.h"

int g_OwnDevCount = 0;
int g_UnownDevCount = 0;
int g_motPMDevCount = 0;
bool g_CBInvoked = CALLBACK_NOT_INVOKED;
bool g_inputPinCbInvoked = CALLBACK_NOT_INVOKED;

FILE* fopenProvManager(const char* path, const char* mode)
{
    //OC_UNUSED(path);
    if (0 == strncmp(path, OC_SECURITY_DB_DAT_FILE_NAME, strlen(OC_SECURITY_DB_DAT_FILE_NAME)))
    {
        return fopen(SVR_DB_FILE_NAME, mode);
    }
    else
    {
        return fopen(path, mode);
    }
}

int waitCallbackRet(void)
{
    __FUNC_IN__

    for (int i = 0; CALLBACK_TIMEOUT > i; ++i)
    {
        IOTIVITYTEST_LOG(DEBUG, "waitCallbackRet Loop = %d", i);

        if (true == g_CBInvoked)
        {
            return CALLBACK_INVOKED;
        }

        CommonUtil::waitInSecond(DELAY_SHORT);

        if (OC_STACK_OK != OCProcess())
        {
            IOTIVITYTEST_LOG(ERROR, "OCStack process error");
            return CALLBACK_NOT_INVOKED;
        }
    }

    __FUNC_OUT__
    return CALLBACK_NOT_INVOKED;
}

void PMCsdkHelper::ownershipTransferCB(void* ctx, size_t nOfRes, OCProvisionResult_t* arr,
bool hasError)
{
    if (!hasError)
    {
        IOTIVITYTEST_LOG(INFO, "Ownership Transfer SUCCEEDED - ctx: %s", (char* ) ctx);
        g_CBInvoked = true;
    }
    else
    {
        IOTIVITYTEST_LOG(ERROR, "Ownership Transfer FAILED - ctx: %s", (char* ) ctx);
        PMCsdkUtilityHelper::printResultList((const OCProvisionResult_t*) arr, nOfRes);
    }
}

void PMCsdkHelper::provisionPairwiseCB(void* ctx, size_t nOfRes, OCProvisionResult_t* arr,
bool hasError)
{
    if (!hasError)
    {
        IOTIVITYTEST_LOG(INFO, "Provision Pairwise SUCCEEDED - ctx: %s", (char* ) ctx);
        g_CBInvoked = true;
    }
    else
    {
        IOTIVITYTEST_LOG(ERROR, "Provision Pairwise Failed - ctx: %s", (char* ) ctx);
        PMCsdkUtilityHelper::printResultList((const OCProvisionResult_t*) arr, nOfRes);
    }
}

void PMCsdkHelper::getAclCB(void* ctx, size_t nOfRes, OCProvisionResult_t* arr, bool hasError)
{
    if (!hasError)
    {
        IOTIVITYTEST_LOG(INFO, "Get ACL Req Failed- ctx: %s", (char* ) ctx);
        g_CBInvoked = true;
    }
    else
    {
        IOTIVITYTEST_LOG(ERROR, "Get ACL Req Success- ctx: %s", (char* ) ctx);
        PMCsdkUtilityHelper::printResultList((const OCProvisionResult_t*) arr, nOfRes);
    }
}

void PMCsdkHelper::getCredCB(void* ctx, size_t nOfRes, OCProvisionResult_t* arr, bool hasError)
{
    if (!hasError)
    {
        IOTIVITYTEST_LOG(INFO, "Get Credential Req SUCCEEDED - ctx: %s", (char* ) ctx);
        g_CBInvoked = true;
    }
    else
    {
        IOTIVITYTEST_LOG(ERROR, "Get Credential Req - ctx: %s", (char* ) ctx);
        PMCsdkUtilityHelper::printResultList((const OCProvisionResult_t*) arr, nOfRes);
    }
}

void PMCsdkHelper::provisionCredCB(void* ctx, size_t nOfRes, OCProvisionResult_t* arr,
bool hasError)
{
    if (!hasError)
    {
        IOTIVITYTEST_LOG(INFO, "Provision Credential SUCCEEDED - ctx: %s", (char* ) ctx);
        g_CBInvoked = true;
    }
    else
    {
        IOTIVITYTEST_LOG(ERROR, "Provision Credential FAILED - ctx: %s", (char* ) ctx);
        PMCsdkUtilityHelper::printResultList((const OCProvisionResult_t*) arr, nOfRes);
    }
}

void PMCsdkHelper::provisionAclCB(void* ctx, size_t nOfRes, OCProvisionResult_t* arr, bool hasError)
{
    if (!hasError)
    {
        IOTIVITYTEST_LOG(DEBUG, "Provision ACL SUCCEEDED - ctx: %s", (char* ) ctx);
        g_CBInvoked = true;
    }
    else
    {
        IOTIVITYTEST_LOG(DEBUG, "Provision ACL FAILED - ctx: %s", (char* ) ctx);
        PMCsdkUtilityHelper::printResultList((const OCProvisionResult_t*) arr, nOfRes);
    }
}

void PMCsdkHelper::unlinkDevicesCB(void* ctx, size_t nOfRes, OCProvisionResult_t* arr,
bool hasError)
{
    if (!hasError)
    {
        IOTIVITYTEST_LOG(DEBUG, "Unlink Devices SUCCEEDED - ctx: %s", (char* ) ctx);
        g_CBInvoked = true;
    }
    else
    {
        IOTIVITYTEST_LOG(DEBUG, "Unlink Devices FAILED - ctx: %s", (char* ) ctx);
        PMCsdkUtilityHelper::printResultList((const OCProvisionResult_t*) arr, nOfRes);
    }
}

void PMCsdkHelper::removeDeviceCB(void* ctx, size_t nOfRes, OCProvisionResult_t* arr, bool hasError)
{
    if (!hasError)
    {
        IOTIVITYTEST_LOG(DEBUG, "Remove Devices SUCCEEDED - ctx: %s", (char* ) ctx);
        g_CBInvoked = true;
    }
    else
    {
        IOTIVITYTEST_LOG(DEBUG, "Remove Devices FAILED - ctx: %s", (char* ) ctx);
        PMCsdkUtilityHelper::printResultList((const OCProvisionResult_t*) arr, nOfRes);
    }
}

void PMCsdkHelper::syncDeviceCB(void* ctx, size_t nOfRes, OCProvisionResult_t* arr, bool hasError)
{
    if (!hasError)
    {
        IOTIVITYTEST_LOG(DEBUG, "Sync Device SUCCEEDED - ctx: %s", (char* ) ctx);
        g_CBInvoked = true;
    }
    else
    {
        IOTIVITYTEST_LOG(DEBUG, "Sync Device FAILED - ctx: %s", (char* ) ctx);
        PMCsdkUtilityHelper::printResultList((const OCProvisionResult_t*) arr, nOfRes);
    }
}

static void inputPinCB(char* pin, size_t len)
{
    __FUNC_IN__

    CommonUtil::waitInSecond(DELAY_LONG);
    char buff[PIN_MAX_SIZE];

    if (!pin || OXM_RANDOM_PIN_MIN_SIZE >= len)
    {
        IOTIVITYTEST_LOG(ERROR, "[PMHelper] Invalid PIN");
        return;
    }

    FILE *fp;
    fp = fopen(RANDOM_PIN_TEXT_FILE, "r");

    if (NULL == fp)
    {
        IOTIVITYTEST_LOG(ERROR, "[PMHelper] ERROR Opening File");
    }

    if (NULL == fgets(buff, PIN_MAX_SIZE, (FILE*) fp))
    {
        IOTIVITYTEST_LOG(ERROR, "[PMHelper] Unable to Get input from File");
    }

    fclose(fp);

    IOTIVITYTEST_LOG(DEBUG, "[PIN CODE] %s\n\n", buff);

    strcpy(pin, (const char*) buff);

    __FUNC_OUT__
}

OCStackResult PMCsdkHelper::displayMutualVerifNumCB(void * ctx, uint8_t mutualVerifNum[MUTUAL_VERIF_NUM_LEN])
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] displayMutualVerifNumCB IN");
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] ############ mutualVerifNum ############");

    for(int i = 0; i< MUTUAL_VERIF_NUM_LEN ; i++)
    {
        IOTIVITYTEST_LOG(DEBUG, "[PMHelper] %02X ", mutualVerifNum[i] );
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] ############ mutualVerifNum ############");
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] displayMutualVerifNumCB OUT");
    return OC_STACK_OK;
}

OCStackResult PMCsdkHelper::confirmMutualVerifNumCB(void * ctx)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] confirmMutualVerifNumCB IN");
    for (;;)
    {
        int userConfirm;
        CommonUtil::waitInSecond(DELAY_SHORT);
        userConfirm = 1;

        if (1 == userConfirm)
        {
            break;
        }
        else if (0 == userConfirm)
        {
            return OC_STACK_USER_DENIED_REQ;
        }
        printf("   Entered Wrong Number. Please Enter Again\n");
    }
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] confirmMutualVerifNumCB OUT");
    return OC_STACK_OK;
}

// CAPI for Provisioning Manager

PMCsdkHelper::PMCsdkHelper()
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] Constructor");
}

bool PMCsdkHelper::initProvisionClient(int clientOTMType, char* chDBPath)
{
    __FUNC_IN__

    static OCPersistentStorage pstStr;
    pstStr.open = fopenProvManager;
    pstStr.read = fread;
    pstStr.write = fwrite;
    pstStr.close = fclose;
    pstStr.unlink = unlink;

    if (OC_STACK_OK != OCRegisterPersistentStorageHandler(&pstStr))
    {
        IOTIVITYTEST_LOG(ERROR, "[PMHelper] OCRegisterPersistentStorageHandler error");
        return false;
    }

    // initialize OC stack and provisioning manager
    if (OC_STACK_OK != OCInit(NULL, 0, OC_CLIENT_SERVER))
    {
        IOTIVITYTEST_LOG(ERROR, "[PMHelper] OCStack init error");
        return false;
    }

    if (access(PRVN_DB_FILE_NAME, F_OK) != -1)
    {
        IOTIVITYTEST_LOG(DEBUG, "[PMHelper] Provisioning DB file already exists.");
    }
    else
    {
        IOTIVITYTEST_LOG(DEBUG, "[PMHelper] No provisioning DB file, creating new ");
    }

    IOTIVITYTEST_LOG(INFO, "************OCInitPM In************");

    if (OC_STACK_OK != OCInitPM(chDBPath))
    {
        IOTIVITYTEST_LOG(ERROR, "[PMHelper] OCInitPM Error");
        return false;
    }

    // register callback function(s) to each OxM
    OTMCallbackData_t otmcb;

    if (clientOTMType == OTM_JUSTWORK || clientOTMType == OTM_ALL)
    {
        IOTIVITYTEST_LOG(INFO, "****OCSetOwnerTransferCallbackData JustWork In****");

        otmcb = otmCbRegister(OTM_JUSTWORK);

        if (OC_STACK_OK != OCSetOwnerTransferCallbackData(OIC_JUST_WORKS, &otmcb))
        {
            IOTIVITYTEST_LOG(ERROR,
                    "[PMHelper] OCSetOwnerTransferCallbackData error: OIC_JUST_WORKS");
            return false;
        }
    }

    if (clientOTMType == OTM_RANDOMPIN || clientOTMType == OTM_ALL)
    {
        IOTIVITYTEST_LOG(INFO, "****OCSetOwnerTransferCallbackData RandomPin In****");

        otmcb = otmCbRegister(OTM_RANDOMPIN);

        if (OC_STACK_OK != OCSetOwnerTransferCallbackData(OIC_RANDOM_DEVICE_PIN, &otmcb))
        {
            IOTIVITYTEST_LOG(ERROR,
                    "[PMHelper] OCSetOwnerTransferCallbackData error: OIC_RANDOM_DEVICE_PIN");
            return false;
        }

        SetInputPinCB(inputPinCB);
    }

    if (!setOxmAllowStatus(OIC_JUST_WORKS, true, OC_STACK_OK))
    {
        IOTIVITYTEST_LOG(ERROR, "[PMHelper] OCSetOxmAllowStatus error: OIC_JUST_WORKS");
        return false;
    }

    if (!setOxmAllowStatus(OIC_RANDOM_DEVICE_PIN, true, OC_STACK_OK))
    {
        IOTIVITYTEST_LOG(ERROR, "[PMHelper] OCSetOxmAllowStatus error: OIC_RANDOM_DEVICE_PIN");
        return false;
    }

    SetDisplayNumCB(NULL, displayMutualVerifNumCB);
    SetUserConfirmCB(NULL, confirmMutualVerifNumCB);

    //SetPreconfigPin("12341234", 8);

    __FUNC_OUT__
    return true;
}

/**
 * Helper Method for OCSetOxmAllowStatus
 */
bool PMCsdkHelper::setOxmAllowStatus(const OicSecOxm_t oxm, const bool allowStatus,
        OCStackResult expectedResult)
{
    __FUNC_IN__

    OCStackResult res = OCSetOxmAllowStatus(oxm, allowStatus);

    IOTIVITYTEST_LOG(INFO, "[PMHelper] OCSetOxmAllowStatus API returns: %s",
            CommonUtil::getOCStackResult(res));

    if (expectedResult != res)
    {
        m_failureMessage = PMCsdkUtilityHelper::setFailureMessage(res, expectedResult);
        return false;
    }

    __FUNC_OUT__
    return true;
}

/**
 * Helper Method for OCGetDevInfoFromNetwork
 */
bool PMCsdkHelper::discoverAllDevices(int nTime, OCProvisionDev_t** own_list,
        OCProvisionDev_t** unown_list, OCStackResult expectedResult)
{
    __FUNC_IN__

    OCStackResult res = OCGetDevInfoFromNetwork(nTime, own_list, unown_list);
    IOTIVITYTEST_LOG(INFO, "[PMHelper] OCGetDevInfoFromNetwork API returns: %s",
            CommonUtil::getOCStackResult(res));

    if (expectedResult != res)
    {
        m_failureMessage = PMCsdkUtilityHelper::setFailureMessage(res, expectedResult);
        return false;
    }

    if (OC_STACK_OK == res)
    {
        IOTIVITYTEST_LOG(INFO, "[PMHelper] Discovered Owned Devices List :");
        g_OwnDevCount = PMCsdkUtilityHelper::printDevList(*own_list);

        IOTIVITYTEST_LOG(INFO, "[PMHelper] Discovered Unowned Devices List :");
        g_UnownDevCount = PMCsdkUtilityHelper::printDevList(*unown_list);

    }

    __FUNC_OUT__
    return true;
}

/**
 * Helper Method for OCDiscoverSingleDevice
 */
bool PMCsdkHelper::discoverSingleDevice(unsigned short nTime, const OicUuid_t* deviceID,
        OCProvisionDev_t** ppFoundDevice, OCStackResult expectedResult)
{
    __FUNC_IN__

    OCStackResult res = OCDiscoverSingleDevice(nTime, deviceID, ppFoundDevice);

    IOTIVITYTEST_LOG(INFO, "[PMHelper] OCDiscoverSingleDevice API returns: %s",
            CommonUtil::getOCStackResult(res));

    if (expectedResult != res)
    {
        m_failureMessage = PMCsdkUtilityHelper::setFailureMessage(res, expectedResult);
        return false;
    }

    IOTIVITYTEST_LOG(INFO, "[PMHelper] Discovered Single Device :");
    if (ppFoundDevice != NULL)
    {
        g_UnownDevCount = PMCsdkUtilityHelper::printDevList(*ppFoundDevice);
    }

    __FUNC_OUT__
    return true;
}

/**
 * Helper Method for OCDiscoverSingleDeviceInUnicast
 */
bool PMCsdkHelper::discoverSingleDeviceInUnicast(unsigned short timeout, const OicUuid_t* deviceID,
        const char* hostAddress, OCConnectivityType connType, OCProvisionDev_t **ppFoundDevice,
        OCStackResult expectedResult)
{
    __FUNC_IN__

    OCStackResult res = OCDiscoverSingleDeviceInUnicast(timeout, deviceID, hostAddress, connType,
            ppFoundDevice);

    IOTIVITYTEST_LOG(INFO, "[PMHelper] OCDiscoverSingleDeviceInUnicast API returns: %s",
            CommonUtil::getOCStackResult(res));

    if (expectedResult != res)
    {
        m_failureMessage = PMCsdkUtilityHelper::setFailureMessage(res, expectedResult);
        return false;
    }

    IOTIVITYTEST_LOG(INFO, "[PMHelper] Discovered Single Device :");
    if (ppFoundDevice != NULL)
    {
        g_UnownDevCount = PMCsdkUtilityHelper::printDevList(*ppFoundDevice);
    }

    __FUNC_OUT__
    return true;
}

/**
 * Helper Method for OCDiscoverUnownedDevices
 */
bool PMCsdkHelper::discoverUnownedDevices(int nTime, OCProvisionDev_t** unown_list,
        OCStackResult expectedResult)
{
    __FUNC_IN__

    OCStackResult res = OCDiscoverUnownedDevices(nTime, unown_list);
    IOTIVITYTEST_LOG(INFO, "[PMHelper] OCDiscoverUnownedDevices API returns: %s",
            CommonUtil::getOCStackResult(res));

    if (expectedResult != res)
    {
        m_failureMessage = PMCsdkUtilityHelper::setFailureMessage(res, expectedResult);
        return false;
    }

    IOTIVITYTEST_LOG(INFO, "[PMHelper] Discovered Unwned Devices List :");
    if (unown_list != NULL)
    {
        g_UnownDevCount = PMCsdkUtilityHelper::printDevList(*unown_list);
    }

    __FUNC_OUT__
    return true;
}

/**
 * Helper Method for OCDiscoverOwnedDevices
 */
bool PMCsdkHelper::discoverOwnedDevices(int nTime, OCProvisionDev_t** own_list,
        OCStackResult expectedResult)
{
    __FUNC_IN__

    OCStackResult res = OCDiscoverOwnedDevices(nTime, own_list);
    IOTIVITYTEST_LOG(INFO, "[PMHelper] OCDiscoverOwnedDevices API returns: %s",
            CommonUtil::getOCStackResult(res));

    if (expectedResult != res)
    {
        m_failureMessage = PMCsdkUtilityHelper::setFailureMessage(res, expectedResult);
        return false;
    }

    if (own_list != NULL)
    {
        IOTIVITYTEST_LOG(INFO, "[PMHelper] [PMHelper] Discovered Owned Devices List :");
        g_OwnDevCount = PMCsdkUtilityHelper::printDevList(*own_list);
    }

    __FUNC_OUT__
    return true;
}

/**
 * Helper Method for OCDoOwnershipTransfer
 */
bool PMCsdkHelper::doOwnerShipTransfer(void* ctx, OCProvisionDev_t** unown_list,
        OCProvisionResultCB resultCallback, OCStackResult expectedResult, bool checkCallback)
{
    __FUNC_IN__

    IOTIVITYTEST_LOG(INFO, "[PMHelper] Printing Unowned Device List to be owned IN");
    g_UnownDevCount = PMCsdkUtilityHelper::printDevList(*unown_list);

    g_CBInvoked = false;

    OCStackResult res = OCDoOwnershipTransfer(ctx, *unown_list, resultCallback);
    IOTIVITYTEST_LOG(INFO, "[PMHelper] OCDoOwnershipTransfer API returns: %s",
            CommonUtil::getOCStackResult(res));

    if (expectedResult != res)
    {
        m_failureMessage = PMCsdkUtilityHelper::setFailureMessage(res, expectedResult);
        return false;
    }

    if (OC_STACK_OK == res && true == checkCallback)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            m_failureMessage = PMCsdkUtilityHelper::setFailureMessage("CALLBACK Not invoked");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

/**
 * Helper Method for OCProvisionACL
 */
bool PMCsdkHelper::provisionACL(void* ctx, const OCProvisionDev_t* selectedDeviceInfo,
        OicSecAcl_t *acl, OCProvisionResultCB resultCallback, OCStackResult expectedResult)
{
    __FUNC_IN__

    g_CBInvoked = false;

    OCStackResult res = OCProvisionACL(ctx, selectedDeviceInfo, acl, resultCallback);
    IOTIVITYTEST_LOG(INFO, "[PMHelper]  OCProvisionACL API returns: %s",
            CommonUtil::getOCStackResult(res));

    if (expectedResult != res)
    {
        m_failureMessage = PMCsdkUtilityHelper::setFailureMessage(res, expectedResult);
        return false;
    }

    if (res == OC_STACK_OK)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            m_failureMessage = PMCsdkUtilityHelper::setFailureMessage("CALLBACK Not invoked");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

/**
 * Helper Method for OCSaveACL
 */
bool PMCsdkHelper::saveACL(const OicSecAcl_t* acl, OCStackResult expectedResult)
{
    __FUNC_IN__

    OCStackResult res = OCSaveACL(acl);

    IOTIVITYTEST_LOG(INFO, "[PMHelper] OCSaveACL API returns: %s",
            CommonUtil::getOCStackResult(res));

    if (expectedResult != res)
    {
        m_failureMessage = PMCsdkUtilityHelper::setFailureMessage(res, expectedResult);
        return false;
    }

    __FUNC_OUT__
    return true;
}

/**
 * Helper Method for OCProvisionCredentials
 */
bool PMCsdkHelper::provisionCredentials(void *ctx, OicSecCredType_t type, size_t keySize,
        const OCProvisionDev_t *pDev1, const OCProvisionDev_t *pDev2,
        OCProvisionResultCB resultCallback, OCStackResult expectedResult)
{
    __FUNC_IN__

    g_CBInvoked = false;

    OCStackResult res = OCProvisionCredentials(ctx, type, keySize, pDev1, pDev2, resultCallback);
    IOTIVITYTEST_LOG(INFO, "[PMHelper]  OCProvisionCredentials API returns: %s",
            CommonUtil::getOCStackResult(res));

    if (expectedResult != res)
    {
        m_failureMessage = PMCsdkUtilityHelper::setFailureMessage(res, expectedResult);
        return false;
    }

    if (res == OC_STACK_OK)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            m_failureMessage = PMCsdkUtilityHelper::setFailureMessage("CALLBACK Not invoked");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

/**
 * Helper Method for OCProvisionPairwiseDevices
 */
bool PMCsdkHelper::provisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize,
        const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2,
        OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback, OCStackResult expectedResult)
{
    __FUNC_IN__

    g_CBInvoked = false;

    OCStackResult res = OCProvisionPairwiseDevices(ctx, type, keySize, pDev1, pDev1Acl, pDev2,
            pDev2Acl, resultCallback);
    IOTIVITYTEST_LOG(INFO, "[PMHelper]  OCProvisionPairwiseDevices API returns: %s",
            CommonUtil::getOCStackResult(res));

    if (expectedResult != res)
    {
        m_failureMessage = PMCsdkUtilityHelper::setFailureMessage(res, expectedResult);
        return false;
    }

    if (OC_STACK_OK == res)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            m_failureMessage = PMCsdkUtilityHelper::setFailureMessage("CALLBACK Not invoked");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

/**
 * Helper Method for OCGetCredResource
 */
bool PMCsdkHelper::getCredResource(void* ctx, const OCProvisionDev_t *selectedDeviceInfo,
        OCProvisionResultCB resultCallback, OCStackResult expectedResult)
{
    __FUNC_IN__

    g_CBInvoked = false;

    OCStackResult res = OCGetCredResource(ctx, selectedDeviceInfo, resultCallback);
    IOTIVITYTEST_LOG(INFO, "[PMHelper]  OCGetACLResource API returns: %s",
            CommonUtil::getOCStackResult(res));

    if (expectedResult != res)
    {
        m_failureMessage = PMCsdkUtilityHelper::setFailureMessage(res, expectedResult);
        return false;
    }

    if (OC_STACK_OK == res)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            m_failureMessage = PMCsdkUtilityHelper::setFailureMessage("CALLBACK Not invoked");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

/**
 * Helper Method for OCGetACLResource
 */
bool PMCsdkHelper::getACLResource(void* ctx, const OCProvisionDev_t *selectedDeviceInfo,
        OCProvisionResultCB resultCallback, OCStackResult expectedResult)
{
    __FUNC_IN__

    g_CBInvoked = false;

    OCStackResult res = OCGetACLResource(ctx, selectedDeviceInfo, resultCallback);
    IOTIVITYTEST_LOG(INFO, "[PMHelper]  OCGetACLResource API returns: %s",
            CommonUtil::getOCStackResult(res));

    if (expectedResult != res)
    {
        m_failureMessage = PMCsdkUtilityHelper::setFailureMessage(res, expectedResult);
        return false;
    }

    if (OC_STACK_OK == res)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            m_failureMessage = PMCsdkUtilityHelper::setFailureMessage("CALLBACK Not invoked");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

/**
 * Helper Method for OCGetLinkedStatus
 */
bool PMCsdkHelper::getLinkedStatus(const OicUuid_t* uuidOfDevice, OCUuidList_t** uuidList,
        size_t* numOfDevices, OCStackResult expectedResult)
{
    __FUNC_IN__

    OCStackResult res;

    res = OCGetLinkedStatus(uuidOfDevice, uuidList, numOfDevices);

    IOTIVITYTEST_LOG(INFO, "[PMHelper]  OCGetLinkedStatus API returns: %s",
            CommonUtil::getOCStackResult(res));

    if (expectedResult != res)
    {
        m_failureMessage = PMCsdkUtilityHelper::setFailureMessage(res, expectedResult);
        return false;
    }

    if (OC_STACK_OK == res)
    {
        IOTIVITYTEST_LOG(INFO, "[PMHelper]  Total Linked Devices =  %d", *numOfDevices);
        PMCsdkUtilityHelper::printUuidList(*uuidList);
    }

    __FUNC_OUT__
    return true;
}

/**
 * Helper Method for OCUnlinkDevices
 */
bool PMCsdkHelper::unlinkDevices(void* ctx, const OCProvisionDev_t* pTargetDev1,
        const OCProvisionDev_t* pTargetDev2, OCProvisionResultCB resultCallback,
        OCStackResult expectedResult)
{
    __FUNC_IN__

    g_CBInvoked = false;

    OCStackResult res = OCUnlinkDevices(ctx, pTargetDev1, pTargetDev2, resultCallback);
    IOTIVITYTEST_LOG(INFO, "[PMHelper]  OCUnlinkDevices API returns: %s",
            CommonUtil::getOCStackResult(res));

    if (expectedResult != res)
    {
        m_failureMessage = PMCsdkUtilityHelper::setFailureMessage(res, expectedResult);
        return false;
    }

    if (OC_STACK_OK == res)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            m_failureMessage = PMCsdkUtilityHelper::setFailureMessage("CALLBACK Not invoked");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

/**
 * Helper Method for OCRemoveDevice
 */
bool PMCsdkHelper::removeDevice(void* ctx, unsigned short waitTimeForOwnedDeviceDiscovery,
        const OCProvisionDev_t* pTargetDev, OCProvisionResultCB resultCallback,
        OCStackResult expectedResult)
{
    __FUNC_IN__

    g_CBInvoked = false;

    OCStackResult res = OCRemoveDevice(ctx, waitTimeForOwnedDeviceDiscovery, pTargetDev,
            resultCallback);
    IOTIVITYTEST_LOG(INFO, "[PMHelper]  OCRemoveDevice API returns: %s",
            CommonUtil::getOCStackResult(res));

    if (expectedResult != res)
    {
        m_failureMessage = PMCsdkUtilityHelper::setFailureMessage(res, expectedResult);
        return false;
    }

    if (OC_STACK_OK == res)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            m_failureMessage = PMCsdkUtilityHelper::setFailureMessage("CALLBACK Not invoked");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

/**
 * Helper Method for OCRemoveDeviceWithUuid
 */
bool PMCsdkHelper::removeDeviceWithUuid(void* ctx, unsigned short waitTimeForOwnedDeviceDiscovery,
        const OicUuid_t* pTargetUuid, OCProvisionResultCB resultCallback,
        OCStackResult expectedResult)
{
    __FUNC_IN__

    g_CBInvoked = false;

    OCStackResult res = OCRemoveDeviceWithUuid(ctx, waitTimeForOwnedDeviceDiscovery, pTargetUuid,
            resultCallback);
    IOTIVITYTEST_LOG(INFO, "[PMHelper]  OCRemoveDeviceWithUuid API returns: %s",
            CommonUtil::getOCStackResult(res));

    if (expectedResult != res)
    {
        m_failureMessage = PMCsdkUtilityHelper::setFailureMessage(res, expectedResult);
        return false;
    }

    if (OC_STACK_OK == res)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            m_failureMessage = PMCsdkUtilityHelper::setFailureMessage("CALLBACK Not invoked");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

/**
 * Helper Method for OCResetDevice
 */
bool PMCsdkHelper::resetDevice(void* ctx, unsigned short waitTimeForOwnedDeviceDiscovery,
        const OCProvisionDev_t* pTargetDev, OCProvisionResultCB resultCallback,
        OCStackResult expectedResult)
{
    __FUNC_IN__

    g_CBInvoked = false;

    OCStackResult res = OCResetDevice(ctx, waitTimeForOwnedDeviceDiscovery, pTargetDev,
            resultCallback);
    IOTIVITYTEST_LOG(INFO, "[PMHelper]  OCResetDevice API returns: %s",
            CommonUtil::getOCStackResult(res));

    if (expectedResult != res)
    {
        m_failureMessage = PMCsdkUtilityHelper::setFailureMessage(res, expectedResult);
        return false;
    }

    if (OC_STACK_OK == res)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            m_failureMessage = PMCsdkUtilityHelper::setFailureMessage("CALLBACK Not invoked");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

/**
 * Helper Method for OCResetSVRDB
 */
bool PMCsdkHelper::resetSVRDB(OCStackResult expectedResult)
{
    __FUNC_IN__

    OCStackResult res = OCResetSVRDB();
    IOTIVITYTEST_LOG(INFO, "[PMHelper]  OCResetSVRDB API returns: %s",
            CommonUtil::getOCStackResult(res));

    if (expectedResult != res)
    {
        m_failureMessage = PMCsdkUtilityHelper::setFailureMessage(res, expectedResult);
        return false;
    }

    __FUNC_OUT__
    return true;
}
/**
 * Helper Method for OCDeleteDiscoveredDevices
 */
bool PMCsdkHelper::deleteDiscoveredDevices(OCProvisionDev_t *pList)
{
    __FUNC_IN__

    try
    {
        OCDeleteDiscoveredDevices(pList);
    }
    catch (const std::exception& e)
    {
        m_failureMessage = PMCsdkUtilityHelper::setFailureMessage(
                "Caught exception: " + std::string(e.what()));
        return false;
    }

    __FUNC_OUT__

    return true;
}

/**
 * Helper Method for OCDeleteUuidList
 */
bool PMCsdkHelper::deleteUuidList(OCUuidList_t* pList)
{
    __FUNC_IN__

    try
    {
        OCDeleteUuidList(pList);
    }
    catch (const std::exception& e)
    {
        m_failureMessage = PMCsdkUtilityHelper::setFailureMessage(
                "Caught exception: " + std::string(e.what()));
        return false;
    }

    __FUNC_OUT__

    return true;
}

/**
 * Helper Method for OCDeleteACLList
 */
bool PMCsdkHelper::deleteACLList(OicSecAcl_t* pList)
{
    __FUNC_IN__

    try
    {
        OCDeleteACLList(pList);
    }
    catch (const std::exception& e)
    {
        m_failureMessage = PMCsdkUtilityHelper::setFailureMessage(
                "Caught exception: " + std::string(e.what()));
        return false;
    }

    __FUNC_OUT__

    return true;
}

bool PMCsdkHelper::provisionTrustCertChain(void *ctx, OicSecCredType_t type, uint16_t credId,
        const OCProvisionDev_t *selectedDeviceInfo, OCProvisionResultCB resultCallback,
        OCStackResult expectedResult)
{
    __FUNC_IN__
    g_CBInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = OCProvisionTrustCertChain(ctx, type, credId, selectedDeviceInfo,
            resultCallback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] provisionTrustCertChain returns %s",
            CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = PMCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

bool PMCsdkHelper::saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize,
        OicEncodingType_t encodingType, uint16_t *credId, OCStackResult expectedResult)
{
    __FUNC_IN__

    OCStackResult result = OCSaveTrustCertChain(trustCertChain, chainSize, encodingType, credId);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCSaveTrustCertChain returns %s",
            CommonUtil::getOCStackResult(result));

    IOTIVITYTEST_LOG(INFO, "CredId of Saved Trust Cert. Chain into Cred of SVR : %d", *credId);

    if (expectedResult != result)
    {
        m_failureMessage = PMCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    __FUNC_OUT__
    return true;
}

bool PMCsdkHelper::selectOwnershipTransferMethod(const OicSecOxm_t *supportedMethods,
        size_t numberOfMethods, OicSecOxm_t *selectedMethod, OwnerType_t ownerType,
        OCStackResult expectedResult, OicSecOxm_t expectedOxm)
{
    __FUNC_IN__

    OCStackResult result = OCSelectOwnershipTransferMethod(supportedMethods, numberOfMethods,
            selectedMethod, ownerType);
    IOTIVITYTEST_LOG(INFO, "[PMHelper] OCSelectOwnershipTransferMethod returns %s",
            CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = PMCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }
    if (expectedOxm != *selectedMethod)
    {
        IOTIVITYTEST_LOG(INFO, "[API Return Code] Selected Method returns : %s",
                PMCsdkUtilityHelper::getOxmType(*selectedMethod));
        m_failureMessage = PMCsdkUtilityHelper::setFailureMessage(*selectedMethod, expectedOxm);
        return false;
    }
    else
    {
        IOTIVITYTEST_LOG(INFO, "[API Return Code] Selected Method returns : %s",
                PMCsdkUtilityHelper::getOxmType(*selectedMethod));
    }

    __FUNC_OUT__
    return true;
}

bool readFile(const char *name, OCByteString *out)
{
    FILE *file = NULL;
    int length = 0;
    uint8_t *buffer = NULL;
    bool result = false;
    size_t count, realCount;
    //Open file
    file = fopen(name, "rb");
    if (!file)
    {
        IOTIVITYTEST_LOG(ERROR, "Unable to open file %s", name);
        return result;
    }

    //Get file length
    if (fseek(file, 0, SEEK_END))
    {
        IOTIVITYTEST_LOG(ERROR, "Failed to SEEK_END");
        goto exit;
    }

    length = ftell(file);
    if (length < 0)
    {
        IOTIVITYTEST_LOG(ERROR, "Failed to ftell");
        goto exit;
    }

    if (fseek(file, 0, SEEK_SET))
    {
        IOTIVITYTEST_LOG(ERROR, "Failed to SEEK_SET");
        goto exit;
    }

    //Allocate memory
    buffer = (uint8_t *) malloc(length);
    if (!buffer)
    {
        IOTIVITYTEST_LOG(ERROR, "Failed to allocate buffer");
        goto exit;
    }

    //Read file contents into buffer
    count = 1;
    realCount = fread(buffer, length, count, file);
    if (realCount != count)
    {
        IOTIVITYTEST_LOG(ERROR, "Read %d bytes %zu times instead of %zu", length, realCount, count);
        goto exit;
    }

    out->bytes = buffer;
    out->len = length;

    result = true;
    exit: fclose(file);
    return result;
}

ByteArray_t PMCsdkHelper::getTrustCertChainArray()
{
    IOTIVITYTEST_LOG(INFO, "Save Trust Cert. Chain into Cred of SVR");

    ByteArray trustCertChainArray =
    { 0, 0 };

    FILE *fp = fopen(ROOT_CERT_FILE, "rb+");

    if (fp)
    {
        size_t fsize;
        if (fseeko(fp, 0, SEEK_END) == 0 && (fsize = ftello(fp)))
        {
            trustCertChainArray.data = (uint8_t*) OICCalloc(1, fsize + 1);
            trustCertChainArray.len = fsize + 1;
            if (NULL == trustCertChainArray.data)
            {
                IOTIVITYTEST_LOG(ERROR, "OICCalloc");
                fclose(fp);
            }
            rewind(fp);
            fsize = fread(trustCertChainArray.data, 1, fsize, fp);
            fclose(fp);
        }
    }

    //OIC_LOG_BUFFER(DEBUG, "CLOUD ACL", trustCertChainArray.data, trustCertChainArray.len);

    return trustCertChainArray;
}

OTMCallbackData_t otmCbRegister(int otmType)
{
    OTMCallbackData_t otmcb;

    switch (otmType)
    {
        case OTM_RANDOMPIN:
            otmcb.loadSecretCB = InputPinCodeCallback;
            otmcb.createSecureSessionCB = CreateSecureSessionRandomPinCallback;
            otmcb.createSelectOxmPayloadCB = CreatePinBasedSelectOxmPayload;
            otmcb.createOwnerTransferPayloadCB = CreatePinBasedOwnerTransferPayload;
            break;

        default: // JUSTWORK
            otmcb.loadSecretCB = LoadSecretJustWorksCallback;
            otmcb.createSecureSessionCB = CreateSecureSessionJustWorksCallback;
            otmcb.createSelectOxmPayloadCB = CreateJustWorksSelectOxmPayload;
            otmcb.createOwnerTransferPayloadCB = CreateJustWorksOwnerTransferPayload;
    }

    return otmcb;
}

OicSecAcl_t* createAcl(const int dev_num, int permission, OCProvisionDev_t** m_own_list,
bool multiple_dev)
{
    __FUNC_IN__

    OicSecAcl_t* acl = (OicSecAcl_t*) OICCalloc(1, sizeof(OicSecAcl_t));
    OicSecAce_t* ace = (OicSecAce_t*) OICCalloc(1, sizeof(OicSecAce_t));
    LL_APPEND(acl->aces, ace);

    int num;
    if (multiple_dev == true)
    {
        num = (dev_num == DEVICE_INDEX_TWO) ? DEVICE_INDEX_ONE : DEVICE_INDEX_TWO;
    }
    else
    {
        num = dev_num;
    }

    OCProvisionDev_t* dev = PMCsdkUtilityHelper::getDevInst((OCProvisionDev_t*) *m_own_list, num);

    PMCsdkUtilityHelper::printDevList(dev);

    memcpy(&ace->subjectuuid, &dev->doxm->deviceID, UUID_LENGTH);

    if (!dev || !dev->doxm)
    {
        IOTIVITYTEST_LOG(DEBUG, "[PMHelper] createAcl: device instance empty");
        return NULL;
    }

    num = 1;
    for (int i = 0; num > i; ++i)
    {
        OicSecRsrc_t* rsrc = (OicSecRsrc_t*) OICCalloc(1, sizeof(OicSecRsrc_t));

        // Resource URI
        size_t len = strlen(DEAFULT_RESOURCE_URI) + 1; // '1' for null termination
        rsrc->href = (char*) OICCalloc(len, sizeof(char));
        OICStrcpy(rsrc->href, len, DEAFULT_RESOURCE_URI);

        // Resource Type
        rsrc->typeLen = 1;
        rsrc->types = (char**) OICCalloc(rsrc->typeLen, sizeof(char*));
        for (size_t i = 0; i < rsrc->typeLen; i++)
        {
            rsrc->types[i] = OICStrdup(DEFAULT_RESOURCE_TYPE);
        }

        rsrc->interfaceLen = 1;
        rsrc->interfaces = (char**) OICCalloc(rsrc->typeLen, sizeof(char*));
        for (size_t i = 0; i < rsrc->interfaceLen; i++)
        {
            rsrc->interfaces[i] = OICStrdup(DEAFULT_INTERFACE_TYPE);
        }

        LL_APPEND(ace->resources, rsrc);
    }

    ace->permission = permission;

    __FUNC_OUT__
    return acl;
}

OicSecAcl_t* createAclForLEDAccess(const OicUuid_t* subject)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] createAclForLEDAccess In");

    OicSecAcl_t* acl = (OicSecAcl_t*) OICCalloc(1, sizeof(OicSecAcl_t));
    OicSecAce_t* ace = (OicSecAce_t*) OICCalloc(1, sizeof(OicSecAce_t));

    LL_APPEND(acl->aces, ace);
    memcpy(ace->subjectuuid.id, subject->id, sizeof(subject->id));

    // fill the href
    char* rsrc_in = "/a/led";  // '1' for null termination
    OicSecRsrc_t* rsrc = (OicSecRsrc_t*)OICCalloc(1, sizeof(OicSecRsrc_t));

    size_t len = strlen(rsrc_in)+1;  // '1' for null termination
    rsrc->href = (char*) OICCalloc(len, sizeof(char));

    OICStrcpy(rsrc->href, len, rsrc_in);

    //fill the resource type (rt)
    rsrc->typeLen = 1;
    rsrc->types = (char**)OICCalloc(1, sizeof(char*));
    rsrc->types[0] = OICStrdup("oic.r.core");
    rsrc->interfaceLen = 1;
    rsrc->interfaces = (char**)OICCalloc(1, sizeof(char*));
    rsrc->interfaces[0] = OICStrdup("oic.if.baseline");

    LL_APPEND(ace->resources, rsrc);

    ace->permission = PERMISSION_FULL_CONTROL;

    ace->eownerID = (OicUuid_t*)OICCalloc(1, sizeof(OicUuid_t));

    memcpy(ace->eownerID->id, subject->id, sizeof(subject->id));

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] createAclForLEDAccess OUT");

    return acl;
}

std::string PMCsdkHelper::getFailureMessage()
{
    return m_failureMessage;
}

/**
 * Function for Convert String to Device Uuid
 */
bool PMCsdkHelper::convertStrToUuid(std::string uuid, OicUuid_t* deviceID,
        OCStackResult expectedResult)
{
    __FUNC_IN__

    OCStackResult rst = ConvertStrToUuid(uuid.c_str(), deviceID);
    IOTIVITYTEST_LOG(DEBUG, "[PMCsdkHelper] convertStrToUuid returns : %s",
            CommonUtil::getOCStackResult(rst));

    if (OC_STACK_OK != rst)
    {
        m_failureMessage = PMCsdkUtilityHelper::setFailureMessage(expectedResult, rst);
        return false;
    }

    __FUNC_OUT__

    return true;
}

