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
#ifdef __SECURED__
#include "SecurityCommonUtil.h"

static int g_OwnDevCount = 0;
static int g_UnownDevCount = 0;
static int g_motPMDevCount = 0;
static bool g_CBInvoked = CALLBACK_NOT_INVOKED;
static bool g_inputPinCbInvoked = CALLBACK_NOT_INVOKED;
static char g_allowedIp[3][30];
static std::size_t IpLength[3];

OTMCallbackData_t otmCbRegister(int otmType);

FILE* SecurityCommonUtil::fopenProvManager(const char* path, const char* mode)
{
    if (0 == strncmp(path, OC_SECURITY_DB_DAT_FILE_NAME, strlen(OC_SECURITY_DB_DAT_FILE_NAME)))
    {
        return fopen(SVR_DB_FILE_NAME, mode);
    }
    else
    {
        return fopen(path, mode);
    }
}

int SecurityCommonUtil::waitCallbackRet(void)
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

void SecurityCommonUtil::ownershipTransferCB(void* ctx, size_t nOfRes, OCProvisionResult_t* arr,
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
        SecurityCommonUtil::printResultList((const OCProvisionResult_t*) arr, nOfRes);
    }
}

void SecurityCommonUtil::provisionCB(void* ctx, size_t nOfRes, OCProvisionResult_t* arr,
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
        SecurityCommonUtil::printResultList((const OCProvisionResult_t*) arr, nOfRes);
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

SecurityCommonUtil::SecurityCommonUtil()
{
    IOTIVITYTEST_LOG(DEBUG, "[SecurityCommonUtil] Constructor");
}

bool SecurityCommonUtil::initProvisionClient(int clientOTMType, char* chDBPath)
{
    __FUNC_IN__
    CommonUtil::waitInSecond(DELAY_MEDIUM);
    static OCPersistentStorage pstStr;
    pstStr.open = SecurityCommonUtil::fopenProvManager;
    pstStr.read = fread;
    pstStr.write = fwrite;
    pstStr.close = fclose;
    pstStr.unlink = remove;

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

    if (access(PM_CLIENT_DB, F_OK) != -1)
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

        SetInputPinCB((InputPinCallback)inputPinCB);
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

    __FUNC_OUT__
    return true;
}

bool SecurityCommonUtil::setOxmAllowStatus(const OicSecOxm_t oxm, const bool allowStatus,
        OCStackResult expectedResult)
{
    __FUNC_IN__

    OCStackResult res = OCSetOxmAllowStatus(oxm, allowStatus);

    IOTIVITYTEST_LOG(INFO, "[PMHelper] OCSetOxmAllowStatus API returns: %s",
            CommonUtil::getOCStackResult(res));

    if (expectedResult != res)
    {
        IOTIVITYTEST_LOG(ERROR, "[Security Common] API Returns = %s instead of %s",
                CommonUtil::getOCStackResult(res), CommonUtil::getOCStackResult(expectedResult));
        return false;
    }

    __FUNC_OUT__
    return true;
}

/**
 * Helper Method for OCDiscoverUnownedDevices
 */
bool SecurityCommonUtil::discoverUnownedDevices(int nTime, OCProvisionDev_t** unown_list,
        OCStackResult expectedResult)
{
    __FUNC_IN__

    OCStackResult res = OCDiscoverUnownedDevices(nTime, unown_list);
    IOTIVITYTEST_LOG(INFO, "[PMHelper] OCDiscoverUnownedDevices API returns: %s",
            CommonUtil::getOCStackResult(res));

    if (expectedResult != res)
    {
        IOTIVITYTEST_LOG(ERROR, "[Security Common] API Returns = %s instead of %s",
                CommonUtil::getOCStackResult(res), CommonUtil::getOCStackResult(expectedResult));
        return false;
    }

    IOTIVITYTEST_LOG(INFO, "[PMHelper] Discovered Unwned Devices List :");
    if (unown_list != NULL)
    {
        g_UnownDevCount = SecurityCommonUtil::printDevList(*unown_list);
    }

    __FUNC_OUT__
    return true;
}

/**
 * Helper Method for OCGetDevInfoFromNetwork
 */
bool SecurityCommonUtil::discoverAllDevices(int nTime, OCProvisionDev_t** own_list,
        OCProvisionDev_t** unown_list, OCStackResult expectedResult)
{
    __FUNC_IN__

    OCStackResult res = OCGetDevInfoFromNetwork(nTime, own_list, unown_list);
    IOTIVITYTEST_LOG(INFO, "[PMHelper] OCGetDevInfoFromNetwork API returns: %s",
            CommonUtil::getOCStackResult(res));

    if (expectedResult != res)
    {
        IOTIVITYTEST_LOG(ERROR, "[Security Common] API Returns = %s instead of %s",
                CommonUtil::getOCStackResult(res), CommonUtil::getOCStackResult(expectedResult));
        return false;
    }

    if (OC_STACK_OK == res)
    {
        IOTIVITYTEST_LOG(INFO, "[PMHelper] Discovered Owned Devices List :");
        g_OwnDevCount = SecurityCommonUtil::printDevList(*own_list);

        IOTIVITYTEST_LOG(INFO, "[PMHelper] Discovered Unowned Devices List :");
        g_UnownDevCount = SecurityCommonUtil::printDevList(*unown_list);

    }

    __FUNC_OUT__
    return true;
}

/**
 * Helper Method for OCDiscoverOwnedDevices
 */
bool SecurityCommonUtil::discoverOwnedDevices(int nTime, OCProvisionDev_t** own_list,
        OCStackResult expectedResult)
{
    __FUNC_IN__

    OCStackResult res = OCDiscoverOwnedDevices(nTime, own_list);
    IOTIVITYTEST_LOG(INFO, "[PMHelper] OCDiscoverOwnedDevices API returns: %s",
            CommonUtil::getOCStackResult(res));

    if (expectedResult != res)
    {
        IOTIVITYTEST_LOG(ERROR, "[Security Common] API Returns = %s instead of %s",
                CommonUtil::getOCStackResult(res), CommonUtil::getOCStackResult(expectedResult));
        return false;
    }

    if (own_list != NULL)
    {
        IOTIVITYTEST_LOG(INFO, "[PMHelper] [PMHelper] Discovered Owned Devices List :");
        g_OwnDevCount = SecurityCommonUtil::printDevList(*own_list);
    }

    __FUNC_OUT__
    return true;
}

/**
 * Helper Method for OCDoOwnershipTransfer
 */
bool SecurityCommonUtil::doOwnerShipTransfer(void* ctx, OCProvisionDev_t** unown_list,
        OCProvisionResultCB resultCallback, OCStackResult expectedResult, bool checkCallback)
{
    __FUNC_IN__

    IOTIVITYTEST_LOG(INFO, "[PMHelper] Printing Unowned Device List to be owned IN");
    g_UnownDevCount = SecurityCommonUtil::printDevList(*unown_list);

    g_CBInvoked = false;

    OCStackResult res = OCDoOwnershipTransfer(ctx, *unown_list, resultCallback);
    IOTIVITYTEST_LOG(INFO, "[PMHelper] OCDoOwnershipTransfer API returns: %s",
            CommonUtil::getOCStackResult(res));

    if (expectedResult != res)
    {
        IOTIVITYTEST_LOG(ERROR, "[Security Common] API Returns = %s instead of %s",
                CommonUtil::getOCStackResult(res), CommonUtil::getOCStackResult(expectedResult));
        return false;
    }

    if (OC_STACK_OK == res && true == checkCallback)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

/**
 * Helper Method for OCProvisionPairwiseDevices
 */
bool SecurityCommonUtil::provisionPairwiseDevices(void* ctx, OicSecCredType_t type, size_t keySize,
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
        IOTIVITYTEST_LOG(ERROR, "[Security Common] API Returns = %s instead of %s",
                CommonUtil::getOCStackResult(res), CommonUtil::getOCStackResult(expectedResult));
        return false;
    }

    if (OC_STACK_OK == res)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

/**
 * Helper Method for OCDeleteDiscoveredDevices
 */
bool SecurityCommonUtil::deleteDiscoveredDevices(OCProvisionDev_t *pList)
{
    __FUNC_IN__

    try
    {
        OCDeleteDiscoveredDevices(pList);
    }
    catch (const std::exception& e)
    {
        IOTIVITYTEST_LOG(ERROR, "[PMHelper] Exception : %s", e.what());
        return false;
    }

    __FUNC_OUT__

    return true;
}

/**
 * Helper Method for OCDeleteUuidList
 */
bool SecurityCommonUtil::deleteUuidList(OCUuidList_t* pList)
{
    __FUNC_IN__

    try
    {
        OCDeleteUuidList(pList);
    }
    catch (const std::exception& e)
    {
        IOTIVITYTEST_LOG(ERROR, "[PMHelper] Exception : %s", e.what());
        return false;
    }

    __FUNC_OUT__

    return true;
}

/**
 * Helper Method for OCDeleteACLList
 */
bool SecurityCommonUtil::deleteACLList(OicSecAcl_t* pList)
{
    __FUNC_IN__

    try
    {
        OCDeleteACLList(pList);
    }
    catch (const std::exception& e)
    {
        IOTIVITYTEST_LOG(ERROR, "[PMHelper] Exception : %s", e.what());
        return false;
    }

    __FUNC_OUT__

    return true;
}

OTMCallbackData_t SecurityCommonUtil::otmCbRegister(int otmType)
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

OicSecAcl_t* SecurityCommonUtil::createAcl(const int dev_num, int permission, OCProvisionDev_t** m_own_list,
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

    OCProvisionDev_t* dev = SecurityCommonUtil::getDevInst((OCProvisionDev_t*) *m_own_list, num);

    SecurityCommonUtil::printDevList(dev);

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

/**
 * Function for Convert String to Device Uuid
 */
bool SecurityCommonUtil::convertStrToUuid(std::string uuid, OicUuid_t* deviceID,
        OCStackResult expectedResult)
{
    __FUNC_IN__

    OCStackResult rst = ConvertStrToUuid(uuid.c_str(), deviceID);
    IOTIVITYTEST_LOG(DEBUG, "[SecurityCommonUtil] convertStrToUuid returns : %s",
            CommonUtil::getOCStackResult(rst));

    if (OC_STACK_OK != rst)
    {
        return false;
    }

    __FUNC_OUT__

    return true;
}

OCProvisionDev_t* SecurityCommonUtil::getDevInst(OCProvisionDev_t* dev_lst, const int dev_num)
{
    __FUNC_IN__

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
            lst->next = NULL;
            IOTIVITYTEST_LOG(DEBUG, "[PMHelper] getDevInst OUT");
            return lst;
        }
        lst = lst->next;
    }

    __FUNC_OUT__
    return NULL; // in here |lst| is always |NULL|
}

int SecurityCommonUtil::printDevList(OCProvisionDev_t* dev_lst)
{
    __FUNC_IN__

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
        printf("  [%s]",lst->endpoint.addr);
        printf("\n");
        lst = lst->next;
    }
    printf("\n");

    __FUNC_OUT__
    return lst_cnt;
}

size_t SecurityCommonUtil::printUuidList(const OCUuidList_t* uid_lst)
{
    __FUNC_IN__

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

    __FUNC_OUT__
    return lst_cnt;
}

int SecurityCommonUtil::printResultList(const OCProvisionResult_t* rslt_lst, const size_t rslt_cnt)
{
    __FUNC_IN__

    if (!rslt_lst || 0 >= rslt_cnt)
    {
        IOTIVITYTEST_LOG(INFO, "[PMHelper] Device List is Empty...");
        return 0;
    }

    size_t lst_cnt = 0;
    for (; rslt_cnt > lst_cnt; ++lst_cnt)
    {
        printf("     [%d] ", lst_cnt + 1);
        printUuid((const OicUuid_t*) &rslt_lst[lst_cnt].deviceId);
        printf(" - result: %s\n", CommonUtil::getOCStackResult(rslt_lst[lst_cnt].res));
    }
    printf("\n");

    __FUNC_OUT__
    return lst_cnt;
}

void SecurityCommonUtil::printUuid(const OicUuid_t* uid)
{
    __FUNC_IN__

    for (int i = 0; i < UUID_LENGTH;)
    {
        printf("%02X", (*uid).id[i++]);
        if (i == 4 || i == 6 || i == 8 || i == 10) // canonical format for UUID has '8-4-4-4-12'
        {
            printf(DASH);
        }
    }
    printf("\n");

    __FUNC_OUT__
}

char* SecurityCommonUtil::getOxmType(OicSecOxm_t oxmType)
{

    char* resultString = NULL;

    switch (oxmType)
    {
        case OIC_JUST_WORKS:
            resultString = (char*) "OIC_JUST_WORKS";
            break;
        case OIC_RANDOM_DEVICE_PIN:
            resultString = (char*) "OIC_RANDOM_DEVICE_PIN";
            break;
        case OIC_MANUFACTURER_CERTIFICATE:
            resultString = (char*) "OIC_MANUFACTURER_CERTIFICATE";
            break;
        case OIC_DECENTRALIZED_PUBLIC_KEY:
            resultString = (char*) "OIC_DECENTRALIZED_PUBLIC_KEY";
            break;
        case OIC_OXM_COUNT:
            resultString = (char*) "OIC_OXM_COUNT";
            break;
        case OIC_PRECONFIG_PIN:
            resultString = (char*) "OIC_PRECONFIG_PIN";
            break;
        case OIC_MV_JUST_WORKS:
            resultString = (char*) "OC_STACK_RESOURCE_CREATED";
            break;
        case OIC_CON_MFG_CERT:
            resultString = (char*) "OIC_CON_MFG_CERT";
            break;
        default:
            resultString = (char*) "UNKNOWN_OXM_TYPE";
    }

    return resultString;
}

void SecurityCommonUtil::pushDevices(OCProvisionDev_t** head_ref, OCProvisionDev_t *new_node)
{
    /* 3. Make next of new node as head */
    new_node->next = (*head_ref);

    /* 4. move the head to point to the new node */
    (*head_ref) = new_node;
}

OCProvisionDev_t* SecurityCommonUtil::filterDevices(OCProvisionDev_t *deviceList) {

    // Filtering Desired Devices
    OCProvisionDev_t *head = NULL;
    OCProvisionDev_t *current = deviceList;

    std::vector< std::string > allowedDevIdVector(g_allowedDevId, end(g_allowedDevId));
    std::vector< OCProvisionDev_t * > allowedDevVector;

    while (current)
    {
        std::ostringstream deviceId("");
        char devID[UUID_STRING_SIZE];
        OCConvertUuidToString(current->doxm->deviceID.id, devID);
        deviceId << devID;

        for (int i = 0; i < allowedDevIdVector.size(); i++)
        {
            if (allowedDevIdVector[i].compare(deviceId.str()) == 0&&SecurityCommonUtil::filterDevicesByIp(current->endpoint.addr))
            {
                allowedDevVector.push_back(current);
                break;
            }
        }
        current = current->next;
    }

    for (int i = 0; i < allowedDevVector.size(); i++)
    {
        pushDevices(&head, allowedDevVector[i]);
    }

    return head;
}

bool SecurityCommonUtil::filterDevicesByIp(std::string ip) {
    // Filtering Desired Devices by Ip
    std::string ipv4Eth0, ipv4Wlan0;
    std::string ipv6Eth0, ipv6Wlan0;

    printf("Device IP: %s \n\n",ip.c_str());

    ipv4Eth0=CommonUtil::getLocalIPAll((InternetProtocolVersion)0,"eth0")+"%eth0";
    ipv4Wlan0=CommonUtil::getLocalIPAll((InternetProtocolVersion)0,"wlan0")+"%wlan0";

    ipv6Eth0=CommonUtil::getLocalIPAll((InternetProtocolVersion)1,"eth0")+"%eth0";
    ipv6Wlan0=CommonUtil::getLocalIPAll((InternetProtocolVersion)1,"wlan0")+"%wlan0";

#if defined(__TIZEN__)

    ipv4Wlan0=SecurityCommonUtil::exec("ifconfig wlan0 | grep 'inet addr:' | cut -d: -f2 | awk '{ print $1}'");
    ipv4Wlan0 = ipv4Wlan0.substr(0, ipv4Wlan0.size() - 1);
    ipv4Wlan0=ipv4Wlan0+"%wlan0";

    ipv6Wlan0=SecurityCommonUtil::exec("ifconfig wlan0 |  awk '/inet6/{print $3}'");
    ipv6Wlan0 = ipv6Wlan0.substr(0, ipv6Wlan0.size() - 4);
    ipv6Wlan0=ipv6Wlan0+"%wlan0";
#endif

    if(ipv6Eth0.compare(ip)==0)
    {
        return true;
    }

    if(ipv6Wlan0.compare(ip)==0)
    {
        return true;
    }

    if(ipv4Eth0.compare(ip)==0)
    {
        return true;
    }

    if(ipv4Wlan0.compare(ip)==0)
    {
        return true;
    }

    return false;
}

std::string SecurityCommonUtil::exec(const char* cmd) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (!feof(pipe)) {
            if (fgets(buffer, 128, pipe) != NULL)
                result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}

#endif // #ifdef __SECURED__
