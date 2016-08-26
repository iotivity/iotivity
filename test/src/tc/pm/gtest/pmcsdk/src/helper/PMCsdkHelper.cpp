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
#include "PMCsdkHelper.h"

int g_OwnDevCount = 0;
int g_UnownDevCount = 0;
bool g_CBInvoked = CALLBACK_NOT_INVOKED;

FILE* fopenProvManager(const char* path, const char* mode)
{
    return fopen(SVR_DB_FILE_NAME, mode);
}

OCProvisionDev_t* getDevInst(OCProvisionDev_t* dev_lst, const int dev_num)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] getDevInst IN");

    if (!dev_lst || 0 >= dev_num)
    {
        IOTIVITYTEST_LOG(ERROR, "[PMHelper] Device List is Empty");
        return NULL;
    }

    OCProvisionDev_t* lst = (OCProvisionDev_t*) dev_lst;

    for (int i = 0; lst;)
    {
        if (dev_num == ++i)
        {
            IOTIVITYTEST_LOG(DEBUG, "[PMHelper] getDevInst OUT");
            return lst;
        }
        lst = lst->next;
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] getDevInst OUT");
    return NULL; // in here |lst| is always |NULL|
}

int printDevList(OCProvisionDev_t* dev_lst)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] printDevList IN");

    if (!dev_lst)
    {
        IOTIVITYTEST_LOG(INFO, "[PMHelper] Device List is Empty...");
        return 0;
    }

    OCProvisionDev_t* lst = (OCProvisionDev_t*) dev_lst;
    int lst_cnt = 0;

    for (; lst;)
    {
        printf("     [%d] ", ++lst_cnt);
        printUuid((const OicUuid_t*) &lst->doxm->deviceID);
        printf("\n");
        lst = lst->next;
    }
    printf("\n");

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] printDevList OUT");
    return lst_cnt;
}

static size_t printUuidList(const OCUuidList_t* uid_lst)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] printUuidList IN");

    if (!uid_lst)
    {
        IOTIVITYTEST_LOG(INFO, "[PMHelper] Device List is Empty...");
        return 0;
    }

    OCUuidList_t* lst = (OCUuidList_t*) uid_lst;
    size_t lst_cnt = 0;

    for (; lst;)
    {
        printf("     [%zu] ", ++lst_cnt);
        printUuid((const OicUuid_t*) &lst->dev);
        printf("\n");
        lst = lst->next;
    }
    printf("\n");

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] printUuidList OUT");
    return lst_cnt;
}

static int printResultList(const OCProvisionResult_t* rslt_lst, const int rslt_cnt)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] printResultList IN");

    if (!rslt_lst || 0 >= rslt_cnt)
    {
        IOTIVITYTEST_LOG(INFO, "[PMHelper] Device List is Empty...");
        return 0;
    }

    int lst_cnt = 0;
    for (; rslt_cnt > lst_cnt; ++lst_cnt)
    {
        printf("     [%d] ", lst_cnt + 1);
        printUuid((const OicUuid_t*) &rslt_lst[lst_cnt].deviceId);
        printf(" - result: %s\n", getOCStackResult(rslt_lst[lst_cnt].res));
    }
    printf("\n");

    IOTIVITYTEST_LOG(INFO, "[PMHelper] printResultList IN");
    return lst_cnt;
}

static void printUuid(const OicUuid_t* uid)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] printUuid IN");

    for (int i = 0; i < UUID_LENGTH;)
    {
        printf("%02X", (*uid).id[i++]);
        if (i == 4 || i == 6 || i == 8 || i == 10) // canonical format for UUID has '8-4-4-4-12'
        {
            printf(DASH);
        }
    }
    printf("\n");

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] printUuid OUT");
}

static int waitCallbackRet(void)
{
    IOTIVITYTEST_LOG(DEBUG, "waitCallbackRet IN");

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

    IOTIVITYTEST_LOG(DEBUG, "waitCallbackRet OUT");
    return CALLBACK_NOT_INVOKED;
}

static void PMCsdkHelper::ownershipTransferCB(void* ctx, int nOfRes, OCProvisionResult_t* arr,
        bool hasError)
{
    if (!hasError)
    {
        IOTIVITYTEST_LOG(DEBUG, "Ownership Transfer SUCCEEDED - ctx: %s", (char* ) ctx);
    }
    else
    {
        IOTIVITYTEST_LOG(DEBUG, "Ownership Transfer FAILED - ctx: %s", (char* ) ctx);
        printResultList((const OCProvisionResult_t*) arr, nOfRes);
    }

    g_CBInvoked = true;
}

void PMCsdkHelper::provisionPairwiseCB(void* ctx, int nOfRes, OCProvisionResult_t* arr,
        bool hasError)
{
    if (!hasError)
    {
        IOTIVITYTEST_LOG(DEBUG, "Provision Pairwise SUCCEEDED - ctx: %s\n", (char* ) ctx);
    }
    else
    {
        IOTIVITYTEST_LOG(DEBUG, "Provision Pairwise Failed - ctx: %s", (char* ) ctx);
        printResultList((const OCProvisionResult_t*) arr, nOfRes);
    }

    g_CBInvoked = true;
}

void PMCsdkHelper::provisionDPCB(void* ctx, int nOfRes, OCProvisionResult_t* arr, bool hasError)
{
    if (!hasError)
    {
        IOTIVITYTEST_LOG(DEBUG, "Provision Direct Pairing SUCCEEDED - ctx: %s\n", (char* ) ctx);
    }
    else
    {
        IOTIVITYTEST_LOG(DEBUG, "Provision Direct Pairing Failed - ctx: %s", (char* ) ctx);
        printResultList((const OCProvisionResult_t*) arr, nOfRes);
    }

    g_CBInvoked = true;
}

void PMCsdkHelper::getAclCB(void* ctx, int nOfRes, OCProvisionResult_t* arr, bool hasError)
{
    if (!hasError)
    {
        IOTIVITYTEST_LOG(ERROR, "Get ACL Req Failed- ctx: %s\n", (char* ) ctx);
    }
    else
    {
        IOTIVITYTEST_LOG(ERROR, "Get ACL Req Success- ctx: %s", (char* ) ctx);
        printResultList((const OCProvisionResult_t*) arr, nOfRes);
    }

    g_CBInvoked = true;
}

void PMCsdkHelper::getCredCB(void* ctx, int nOfRes, OCProvisionResult_t* arr, bool hasError)
{
    if (!hasError)
    {
        IOTIVITYTEST_LOG(ERROR, "Get Credential Req SUCCEEDED - ctx: %s\n", (char* ) ctx);
    }
    else
    {
        IOTIVITYTEST_LOG(ERROR, "Get Credential Req - ctx: %s", (char* ) ctx);
        printResultList((const OCProvisionResult_t*) arr, nOfRes);
    }

    g_CBInvoked = true;
}
void PMCsdkHelper::provisionCredCB(void* ctx, int nOfRes, OCProvisionResult_t* arr, bool hasError)
{
    if (!hasError)
    {
        IOTIVITYTEST_LOG(DEBUG, "Provision Credential SUCCEEDED - ctx: %s", (char* ) ctx);
    }
    else
    {
        IOTIVITYTEST_LOG(DEBUG, "Provision Credential FAILED - ctx: %s", (char* ) ctx);
        printResultList((const OCProvisionResult_t*) arr, nOfRes);
    }

    g_CBInvoked = true;
}

void PMCsdkHelper::provisionAclCB(void* ctx, int nOfRes, OCProvisionResult_t* arr, bool hasError)
{
    if (!hasError)
    {
        IOTIVITYTEST_LOG(DEBUG, "Provision ACL SUCCEEDED - ctx: %s", (char* ) ctx);
    }
    else
    {
        IOTIVITYTEST_LOG(DEBUG, "Provision ACL FAILED - ctx: %s", (char* ) ctx);
        printResultList((const OCProvisionResult_t*) arr, nOfRes);
    }

    g_CBInvoked = true;
}

void PMCsdkHelper::unlinkDevicesCB(void* ctx, int nOfRes, OCProvisionResult_t* arr, bool hasError)
{
    if (!hasError)
    {
        IOTIVITYTEST_LOG(DEBUG, "Unlink Devices SUCCEEDED - ctx: %s", (char* ) ctx);
    }
    else
    {
        IOTIVITYTEST_LOG(DEBUG, "Unlink Devices FAILED - ctx: %s", (char* ) ctx);
        printResultList((const OCProvisionResult_t*) arr, nOfRes);
    }

    g_CBInvoked = true;
}

void PMCsdkHelper::removeDeviceCB(void* ctx, int nOfRes, OCProvisionResult_t* arr, bool hasError)
{
    if (!hasError)
    {
        IOTIVITYTEST_LOG(DEBUG, "Remove Devices SUCCEEDED - ctx: %s", (char* ) ctx);
    }
    else
    {
        IOTIVITYTEST_LOG(DEBUG, "Remove Devices FAILED - ctx: %s", (char* ) ctx);
        printResultList((const OCProvisionResult_t*) arr, nOfRes);
    }

    g_CBInvoked = true;
}

static void inputPinCB(char* pin, size_t len)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] inputPinCB IN");

    CommonUtil::waitInSecond(DELAY_LONG);
    char buff[PIN_MAX_SIZE];

    if (!pin || OXM_RANDOM_PIN_SIZE >= len)
    {
        IOTIVITYTEST_LOG(ERROR, "[PMHelper] Invalid PIN");
        return;
    }

    FILE *fp;
    fp = fopen(RANDOM_PIN_TEXT_FILE, "r");
    fgets(buff, PIN_MAX_SIZE, (FILE*) fp);
    fclose(fp);

    IOTIVITYTEST_LOG(DEBUG, "[PIN CODE] %s\n\n", buff);
    strcpy(pin, (const char*) buff);

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] inputPinCB Out");
}

// CAPI for Provisioning Manager

PMCsdkHelper::PMCsdkHelper()
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] Constructor");
}

bool PMCsdkHelper::initProvisionClient(int clientOTMType = OTM_JUSTWORK, char* chDBPath =
        (char*) PRVN_DB_FILE_NAME)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] initProvisionClient IN");

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

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] initProvisionClient OUT");
    return true;
}

/**
 * Helper Method for OCGetDevInfoFromNetwork
 */
bool PMCsdkHelper::discoverAllDevices(int nTime, OCProvisionDev_t** own_list,
        OCProvisionDev_t** unown_list, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] discoverAllDevices In");

    OCStackResult res = OCGetDevInfoFromNetwork(nTime, own_list, unown_list);
    IOTIVITYTEST_LOG(INFO, "[PMHelper] OCGetDevInfoFromNetwork API returns: %s\n",
            getOCStackResult(res));

    if (expectedResult != res)
    {
        m_failureMessage = setFailureMessage(res, expectedResult);
        return false;
    }

    if (OC_STACK_OK == res)
    {
        IOTIVITYTEST_LOG(INFO, "[PMHelper] Discovered Owned Devices List :");
        g_OwnDevCount = printDevList(*own_list);

        IOTIVITYTEST_LOG(INFO, "[PMHelper] Discovered Unowned Devices List :");
        g_UnownDevCount = printDevList(*unown_list);

    }

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] discoverAllDevices OUT");
    return true;
}

/**
 * Helper Method for OCDiscoverUnownedDevices
 */
bool PMCsdkHelper::discoverUnownedDevices(int nTime, OCProvisionDev_t** unown_list,
        OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] discoverUnownedDevices IN");

    OCStackResult res = OCDiscoverUnownedDevices(nTime, unown_list);
    IOTIVITYTEST_LOG(INFO, "[PMHelper] OCDiscoverUnownedDevices API returns: %s\n",
            getOCStackResult(res));

    if (expectedResult != res)
    {
        m_failureMessage = setFailureMessage(res, expectedResult);
        return false;
    }

    printf("\n[PMHelper] Discovered Unwned Devices List :\n");

    if (unown_list != NULL)
    {
        g_UnownDevCount = printDevList(*unown_list);
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] discoverUnownedDevices OUT");
    return true;
}

/**
 * Helper Method for OCDiscoverOwnedDevices
 */
bool PMCsdkHelper::discoverOwnedDevices(int nTime, OCProvisionDev_t** own_list,
        OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] discoverOwnedDevices IN");

    OCStackResult res = OCDiscoverOwnedDevices(nTime, own_list);
    IOTIVITYTEST_LOG(INFO, "[PMHelper] OCDiscoverOwnedDevices API returns: %s\n",
            getOCStackResult(res));

    if (expectedResult != res)
    {
        m_failureMessage = setFailureMessage(res, expectedResult);
        return false;
    }

    if (own_list != NULL)
    {
        IOTIVITYTEST_LOG(INFO, "[PMHelper] [PMHelper] Discovered Owned Devices List :");
        g_OwnDevCount = printDevList(*own_list);
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] discoverOwnedDevices OUT");
    return true;
}

/**
 * Helper Method for OCDoOwnershipTransfer
 */
bool PMCsdkHelper::doOwnerShipTransfer(void* ctx, OCProvisionDev_t** unown_list,
        OCProvisionResultCB resultCallback, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] doOwnerShipTransfer IN");

    IOTIVITYTEST_LOG(INFO, "[PMHelper] Printing Unowned Device List to be owned IN");
    g_UnownDevCount = printDevList(*unown_list);

    g_CBInvoked = false;

    OCStackResult res = OCDoOwnershipTransfer(ctx, *unown_list, resultCallback);
    IOTIVITYTEST_LOG(INFO, "[PMHelper] OCDoOwnershipTransfer API returns: %s",
            getOCStackResult(res));

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

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] doOwnerShipTransfer OUT");
    return true;
}

/**
 * Helper Method for OCProvisionACL
 */
bool PMCsdkHelper::provisionACL(void* ctx, const OCProvisionDev_t* selectedDeviceInfo,
        OicSecAcl_t *acl, OCProvisionResultCB resultCallback, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] OCProvisionACL IN");

    g_CBInvoked = false;

    OCStackResult res = OCProvisionACL(ctx, selectedDeviceInfo, acl, resultCallback);
    IOTIVITYTEST_LOG(INFO, "[PMHelper]  OCProvisionACL API returns: %s\n", getOCStackResult(res));

    if (expectedResult != res)
    {
        m_failureMessage = setFailureMessage(res, expectedResult);
        return false;
    }

    if (res == OC_STACK_OK)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            m_failureMessage = setFailureMessage("CALLBACK Not invoked");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] OCProvisionACL Out");
    return true;
}

/**
 * Helper Method for OCProvisionDirectPairing
 */
bool PMCsdkHelper::proivisioningDirectPairing(void* ctx, const OCProvisionDev_t *selectedDeviceInfo,
        OicSecPconf_t *pconf, OCProvisionResultCB resultCallback, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] proivisioningDirectPairing IN");

    g_CBInvoked = false;

    OCStackResult res = OCProvisionDirectPairing(ctx, selectedDeviceInfo, pconf, resultCallback);
    IOTIVITYTEST_LOG(INFO, "[PMHelper]  OCProvisionDirectPairing API returns: %s---- %d\n",
            getOCStackResult(res), res);

    if (expectedResult != res)
    {
        m_failureMessage = setFailureMessage(res, expectedResult);
        return false;
    }

    if (res == OC_STACK_OK)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            m_failureMessage = setFailureMessage("CALLBACK Not invoked");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] proivisioningDirectPairing OUT");
    return true;
}

/**
 * Helper Method for OCDeletePdAclList
 */
void PMCsdkHelper::deletePdAclList(OicSecPdAcl_t* pPdAcl)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper]  OCDeletePdAclList API In");
    OCDeletePdAclList(pPdAcl);
}

/**
 * Helper Method for OCProvisionCredentials
 */
bool PMCsdkHelper::provisionCredentials(void *ctx, OicSecCredType_t type, size_t keySize,
        const OCProvisionDev_t *pDev1, const OCProvisionDev_t *pDev2,
        OCProvisionResultCB resultCallback, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] provisionCredentials IN");

    g_CBInvoked = false;

    OCStackResult res = OCProvisionCredentials(ctx, type, keySize, pDev1, pDev2, resultCallback);
    IOTIVITYTEST_LOG(INFO, "[PMHelper]  OCProvisionCredentials API returns: %s\n",
            getOCStackResult(res));

    if (expectedResult != res)
    {
        m_failureMessage = setFailureMessage(res, expectedResult);
        return false;
    }

    if (res == OC_STACK_OK)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            m_failureMessage = setFailureMessage("CALLBACK Not invoked");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] provisionCredentials OUT");
    return true;
}

/**
 * Helper Method for OCProvisionPairwiseDevices
 */
bool PMCsdkHelper::provisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize,
        const OCProvisionDev_t *pDev1, OicSecAcl_t *pDev1Acl, const OCProvisionDev_t *pDev2,
        OicSecAcl_t *pDev2Acl, OCProvisionResultCB resultCallback, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] provisionPairwiseDevices IN");

    g_CBInvoked = false;

    OCStackResult res = OCProvisionPairwiseDevices(ctx, type, keySize, pDev1, pDev1Acl, pDev2,
            pDev2Acl, resultCallback);
    IOTIVITYTEST_LOG(INFO, "[PMHelper]  OCProvisionPairwiseDevices API returns: %s\n",
            getOCStackResult(res));

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

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] provisionPairwiseDevices OUT");
    return true;
}

/**
 * Helper Method for OCGetCredResource
 */
bool PMCsdkHelper::getCredResource(void* ctx, const OCProvisionDev_t *selectedDeviceInfo,
        OCProvisionResultCB resultCallback, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] getCredResource IN");

    g_CBInvoked = false;

    OCStackResult res = OCGetCredResource(ctx, selectedDeviceInfo, resultCallback);
    IOTIVITYTEST_LOG(INFO, "[PMHelper]  OCGetACLResource API returns: %s\n", getOCStackResult(res));

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

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] getCredResource OUT");
    return true;
}

/**
 * Helper Method for OCGetACLResource
 */
bool PMCsdkHelper::getACLResource(void* ctx, const OCProvisionDev_t *selectedDeviceInfo,
        OCProvisionResultCB resultCallback, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] getACLResource IN");

    g_CBInvoked = false;

    OCStackResult res = OCGetACLResource(ctx, selectedDeviceInfo, resultCallback);
    IOTIVITYTEST_LOG(INFO, "[PMHelper]  OCGetACLResource API returns: %s\n", getOCStackResult(res));

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

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] getACLResource OUT");
    return true;
}

/**
 * Helper Method for OCGetLinkedStatus
 */
bool PMCsdkHelper::getLinkedStatus(const OicUuid_t* uuidOfDevice, OCUuidList_t** uuidList,
        size_t* numOfDevices, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] getLinkedStatus IN");

    OCStackResult res;

    res = OCGetLinkedStatus(uuidOfDevice, uuidList, numOfDevices);

    IOTIVITYTEST_LOG(INFO, "[PMHelper]  OCGetLinkedStatus API returns: %s\n",
            getOCStackResult(res));

    if (expectedResult != res)
    {
        m_failureMessage = setFailureMessage(res, expectedResult);
        return false;
    }

    if (OC_STACK_OK == res)
    {
        IOTIVITYTEST_LOG(INFO, "[PMHelper]  Total Linked Devices =  %d\n", *numOfDevices);
        printUuidList(*uuidList);
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] getLinkedStatus OUT");
    return true;
}

/**
 * Helper Method for OCUnlinkDevices
 */
bool PMCsdkHelper::unlinkDevices(void* ctx, const OCProvisionDev_t* pTargetDev1,
        const OCProvisionDev_t* pTargetDev2, OCProvisionResultCB resultCallback,
        OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] unlinkDevices IN");

    g_CBInvoked = false;

    OCStackResult res = OCUnlinkDevices(ctx, pTargetDev1, pTargetDev2, resultCallback);
    IOTIVITYTEST_LOG(INFO, "[PMHelper]  OCUnlinkDevices API returns: %s\n", getOCStackResult(res));

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

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] unlinkDevices OUT");
    return true;
}

/**
 * Helper Method for OCRemoveDevice
 */
bool PMCsdkHelper::removeDevice(void* ctx, unsigned short waitTimeForOwnedDeviceDiscovery,
        const OCProvisionDev_t* pTargetDev, OCProvisionResultCB resultCallback,
        OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] OCRemoveDevice IN");

    g_CBInvoked = false;

    OCStackResult res = OCRemoveDevice(ctx, waitTimeForOwnedDeviceDiscovery, pTargetDev,
            resultCallback);
    IOTIVITYTEST_LOG(INFO, "[PMHelper]  OCRemoveDevice API returns: %s\n", getOCStackResult(res));

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

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] OCRemoveDevice OUT");
    return true;
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
            break;
    }

    return otmcb;
}

OicSecAcl_t* createAcl(const int dev_num, int permission, OCProvisionDev_t** m_own_list)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] createAcl IN");

    printDevList(*m_own_list);

    OicSecAcl_t* acl = (OicSecAcl_t*) OICCalloc(1, sizeof(OicSecAcl_t));

    OicSecAce_t* ace = (OicSecAce_t*) OICCalloc(1, sizeof(OicSecAce_t));

    LL_APPEND(acl->aces, ace);

    int num = (dev_num == DEVICE_INDEX_TWO) ? DEVICE_INDEX_ONE : DEVICE_INDEX_TWO;

    OCProvisionDev_t* dev = getDevInst((const OCProvisionDev_t*) *m_own_list, num);
    if (!dev || !dev->doxm)
    {
        IOTIVITYTEST_LOG(ERROR, "createAcl: device instance empty");
        return NULL;
    }

    memcpy(&ace->subjectuuid, &dev->doxm->deviceID, UUID_LENGTH);

    num = 1;
    char rsrc_in[129]; // '1' for null termination
    for (int i = 0; num > i; ++i)
    {
        OicSecRsrc_t* rsrc = (OicSecRsrc_t*) OICCalloc(1, sizeof(OicSecRsrc_t));

        //Resource URI
        OICStrcpy(rsrc->href, ACL_RESOURCE_LENGTH, ACL_RESOURCE_URI);

        rsrc->typeLen = 1;
        rsrc->types = (char**) OICCalloc(rsrc->typeLen, sizeof(char*));

        for (int i = 0; i < rsrc->typeLen; i++)
        {
            rsrc->types[i] = OICStrdup(ACL_RES_TYPE_NAME);
        }

        rsrc->interfaceLen = 1;
        rsrc->interfaces = (char**) OICCalloc(rsrc->interfaceLen, sizeof(char*));

        for (int i = 0; i < rsrc->interfaceLen; i++)
        {
            rsrc->interfaces[i] = OICStrdup(ACL_RES_IF_TYPE_NAME);
        }

        LL_APPEND(ace->resources, rsrc);
    }

    ace->permission = permission;

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] createAcl OUT");

    return acl;
}

OicSecPdAcl_t* createPdAcl(int nPermission)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] createPdAcl IN");

    OicSecPdAcl_t* pdAcl = (OicSecPdAcl_t*) OICCalloc(1, sizeof(OicSecPdAcl_t));

    if (!pdAcl)
    {
        IOTIVITYTEST_LOG(ERROR, "[PMHelper] createPdAcl: OICCalloc error return");
        return NULL; // not need to 'goto' |ERROR| before allocating |acl|
    }

    // number of resources
    char rsrc_in[][ACL_RESRC_MAX_LEN + 1] =
    { "*", "/rsrc/*" };
    pdAcl->resourcesLen = 1;

    // resource
    int num = pdAcl->resourcesLen;
    pdAcl->resources = (char**) OICCalloc(num, sizeof(char*));

    if (!pdAcl->resources)
    {
        IOTIVITYTEST_LOG(ERROR, "[PMHelper] createPdAcl: OICCalloc error return");
    }

    for (int i = 0; num > i; ++i)
    {
        size_t len = strlen(rsrc_in[i]) + 1; // '1' for null termination
        char* rsrc = (char*) OICCalloc(len, sizeof(char));
        if (!rsrc)
        {
            IOTIVITYTEST_LOG(ERROR, "[PMHelper] createPdAcl: OICCalloc error return");
        }
        OICStrcpy(rsrc, len, rsrc_in[i]);
        pdAcl->resources[i] = rsrc; // after here, |rsrc| points nothing
    }

    // permission
    pdAcl->permission = nPermission;

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] createPdAcl OUT");
    return pdAcl;
}

char *getOCStackResult(OCStackResult ocstackresult)
{
    char* resultString = NULL;

    switch (ocstackresult)
    {
        case OC_STACK_OK:
            resultString = (char*) "OC_STACK_OK";
            break;
        case OC_STACK_RESOURCE_CREATED:
            resultString = (char*) "OC_STACK_RESOURCE_CREATED";
            break;
        case OC_STACK_RESOURCE_DELETED:
            resultString = (char*) "OC_STACK_RESOURCE_DELETED";
            break;
        case OC_STACK_INVALID_URI:
            resultString = (char*) "OC_STACK_INVALID_URI";
            break;
        case OC_STACK_INVALID_QUERY:
            resultString = (char*) "OC_STACK_INVALID_QUERY";
            break;
        case OC_STACK_INVALID_IP:
            resultString = (char*) "OC_STACK_INVALID_IP";
            break;
        case OC_STACK_INVALID_PORT:
            resultString = (char*) "OC_STACK_INVALID_PORT";
            break;
        case OC_STACK_INVALID_CALLBACK:
            resultString = (char*) "OC_STACK_INVALID_CALLBACK";
            break;
        case OC_STACK_INVALID_METHOD:
            resultString = (char*) "OC_STACK_INVALID_METHOD";
            break;
        case OC_STACK_INVALID_PARAM:
            resultString = (char*) "OC_STACK_INVALID_PARAM";
            break;
        case OC_STACK_INVALID_OBSERVE_PARAM:
            resultString = (char*) "OC_STACK_INVALID_OBSERVE_PARAM";
            break;
        case OC_STACK_NO_MEMORY:
            resultString = (char*) "OC_STACK_NO_MEMORY";
            break;
        case OC_STACK_COMM_ERROR:
            resultString = (char*) "OC_STACK_COMM_ERROR";
            break;
        case OC_STACK_TIMEOUT:
            resultString = (char*) "OC_STACK_TIMEOUT";
            break;
        case OC_STACK_ADAPTER_NOT_ENABLED:
            resultString = (char*) "OC_STACK_ADAPTER_NOT_ENABLED";
            break;
        case OC_STACK_NOTIMPL:
            resultString = (char*) "OC_STACK_NOTIMPL";
            break;
        case OC_STACK_NO_RESOURCE:
            resultString = (char*) "OC_STACK_NO_RESOURCE";
            break;
        case OC_STACK_UNAUTHORIZED_REQ:
            resultString = (char*) "OC_STACK_UNAUTHORIZED_REQ";
            break;
        case OC_STACK_ERROR:
            resultString = (char*) "OC_STACK_ERROR";
            break;
        default:
            resultString = (char*) "UNKNOWN_STATE";
            break;
    }

    return resultString;
}

std::string PMCsdkHelper::getFailureMessage()
{
    return m_failureMessage;
}

/**
 * Function to set failure message
 */
std::string PMCsdkHelper::setFailureMessage(OCStackResult actualResult,
        OCStackResult expectedResult)
{
    std::string errorMessage("\033[1;31m[Error] Expected : ");
    errorMessage.append(getOCStackResult(expectedResult));
    errorMessage.append("\033[0m  \033[1;31mActual : ");
    errorMessage.append(getOCStackResult(actualResult));
    errorMessage.append("\033[0m");
    return errorMessage;
}

/**
 * Function to set failure message
 */
std::string PMCsdkHelper::setFailureMessage(std::string errorMessage)
{
    std::string retErrorMessage("\033[1;31m[Error] Expected : ");
    retErrorMessage.append(errorMessage);
    retErrorMessage.append("\033[0m");

    return retErrorMessage;
}
