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

#include "PMCppHelper.h"

int g_cbInvoked = CALLBACK_NOT_INVOKED;

void printDevices(DeviceList_t &list)
{
    for (unsigned int i = 0; i < list.size(); i++)
    {
        std::cout << "Device " << i + 1 << " ID: ";
        std::cout << list[i]->getDeviceID() << " From IP: ";
        std::cout << list[i]->getDevAddr() << " Device Status(On/ Off) ";
        std::cout << list[i]->getDeviceStatus() << " Owned Status: ";
        std::cout << list[i]->getOwnedStatus() << std::endl;
    }
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

void printUuid(OicUuid_t uuid)
{
    for (int i = 0; i < UUID_LENGTH; i++)
    {
        std::cout << std::hex << uuid.id[i] << " ";
    }
    std::cout << std::endl;
}

static FILE* clientOpen(const char *UNUSED_PARAM, const char *mode)
{
    (void) UNUSED_PARAM;
    return fopen(CBOR_DB_PATH, mode);
}

void InputPinCB(char* pin, size_t len)
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

OicSecAcl_t* createAcl(const int dev_num, int nPermission, DeviceList_t &m_OwnedDevList)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] createAcl IN");

    OicSecAcl_t* acl = (OicSecAcl_t*) OICCalloc(1, sizeof(OicSecAcl_t));
    OicSecAce_t* ace = (OicSecAce_t*) OICCalloc(1, sizeof(OicSecAce_t));

    LL_APPEND(acl->aces, ace);
    int num = 0;

    if(dev_num == 1) {
        memcpy(&ace->subjectuuid, ACL_ROWNER_UUID_02, UUID_LENGTH);
    } else {
        memcpy(&ace->subjectuuid, ACL_ROWNER_UUID_01, UUID_LENGTH);
    }

    num = 1;
    char rsrc_in[ACL_RESRC_MAX_LEN+1] = {0};  // '1' for null termination
    for(int i = 0; num > i; ++i)
    {
        OicSecRsrc_t* rsrc = (OicSecRsrc_t*)OICCalloc(1, sizeof(OicSecRsrc_t));

        // Resource URI
        size_t len = strlen(LIGHT_RESOURCE_URI_01)+1;  // '1' for null termination
        rsrc->href = (char*) OICCalloc(len, sizeof(char));
        OICStrcpy(rsrc->href, len, LIGHT_RESOURCE_URI_01);

        // Resource Type
        rsrc->typeLen = 1;
        rsrc->types = (char**)OICCalloc(rsrc->typeLen, sizeof(char*));
        for(int i = 0; i < rsrc->typeLen; i++)
        {
            rsrc->types[i] = OICStrdup(LIGHT_RESOURCE_URI_01);
        }

        rsrc->interfaceLen = 1;
        rsrc->interfaces = (char**)OICCalloc(rsrc->typeLen, sizeof(char*));
        for(int i = 0; i < rsrc->interfaceLen; i++)
        {
            rsrc->interfaces[i] = OICStrdup(LIGHT_RESOURCE_URI_01);
        }

        LL_APPEND(ace->resources, rsrc);
    }

    ace->permission = nPermission;

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] createAcl OUT");
    return acl;
}

PMCppHelper::PMCppHelper()
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] Constructor Called");
}

bool PMCppHelper::provisionInit()
{
    OCPersistentStorage ps
    { clientOpen, fread, fwrite, fclose, unlink };

    PlatformConfig cfg
    { OC::ServiceType::InProc, OC::ModeType::Both, "0.0.0.0", 0, OC::QualityOfService::LowQos, &ps };

    OCPlatform::Configure(cfg);

    if (OCSecure::provisionInit(EMPTY_STRING) != OC_STACK_OK)
    {
        std::cout << "PM Init failed" << std::endl;
        return false;
    }

    std::cout << "setup succeeded" << std::endl;
    return true;
}

bool PMCppHelper::discoverUnownedDevices(int time, DeviceList_t& deviceList,
        OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] discoverUnownedDevices IN");

    OCStackResult res = OC_STACK_OK;
    res = OCSecure::discoverUnownedDevices(time, deviceList);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] discoverUnownedDevices returns : %s",
            getOCStackResultCPP(res).c_str());

    if (res != expectedResult)
    {
        m_failureMessage = setFailureMessage(expectedResult, res);
        return false;
    }

    if (deviceList.size())
    {
        IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] Found secure devices, count = %d",
                deviceList.size());
        printDevices(deviceList);

    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] discoverUnownedDevices OUT");
    return true;
}

bool PMCppHelper::discoverOwnedDevices(int time, DeviceList_t& deviceList,
        OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] discoverOwnedDevices IN");

    OCStackResult res = OC_STACK_OK;
    res = OCSecure::discoverOwnedDevices(time, deviceList);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] discoverOwnedDevices returns : %s",
            getOCStackResultCPP(res).c_str());

    if (res != expectedResult)
    {
        m_failureMessage = setFailureMessage(expectedResult, res);
        return false;
    }

    if (deviceList.size())
    {
        IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] Found secure devices, count = %d",
                deviceList.size());
        printDevices(deviceList);
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] discoverOwnedDevices OUT");
    return true;
}

bool PMCppHelper::getDevInfoFromNetwork(unsigned short time, DeviceList_t& ownedDevList,
        DeviceList_t &unownedDevList, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] getDevInfoFromNetwork IN");

    OCStackResult res = OC_STACK_OK;
    res = OCSecure::getDevInfoFromNetwork(time, ownedDevList, unownedDevList);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] getDevInfoFromNetwork returns : %s",
            getOCStackResultCPP(res).c_str());

    if (res != expectedResult)
    {
        m_failureMessage = setFailureMessage(expectedResult, res);
        return false;
    }

    if (unownedDevList.size())
    {
        IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] Found secure devices, count = %d",
                unownedDevList.size());
        printDevices(unownedDevList);
    }

    if (ownedDevList.size())
    {
        IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] Found secure devices, count = %d",
                ownedDevList.size());
        printDevices(ownedDevList);
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] getDevInfoFromNetwork OUT");
    return true;
}

bool PMCppHelper::setOwnerTransferCallbackData(int num, OTMCallbackData_t &data,
        InputPinCallback inputPin, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] setOwnerTransferCallbackData IN");

    OCStackResult res = OC_STACK_OK;

    res = OCSecure::setOwnerTransferCallbackData((OicSecOxm_t) num, &data, inputPin);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] setOwnerTransferCallbackData returns : %s",
            getOCStackResultCPP(res).c_str());

    if (res != expectedResult)
    {
        m_failureMessage = setFailureMessage(expectedResult, res);
        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] setOwnerTransferCallbackData OUT");
    return true;
}

bool PMCppHelper::doOwnershipTransfer(DeviceList_t &data, ResultCallBack resultCallback,
        OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] ownershipTransfer IN");
    OCStackResult res = OC_STACK_OK;
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] Device to be owned in : %d ", data.size());

    for (int i = 0; i < data.size(); i++)
    {
        g_cbInvoked = CALLBACK_NOT_INVOKED;

        res = data[i]->doOwnershipTransfer(resultCallback);
        IOTIVITYTEST_LOG(INFO, "[API Return Code] doOwnershipTransfer returns : %s",
                getOCStackResultCPP(res).c_str());

        sleep(10);

        if (res != expectedResult)
        {
            m_failureMessage = "Failed doOwnershipTransfer.";
            return false;
        }

        if (OC_STACK_OK == res)
        {
            if (CALLBACK_NOT_INVOKED == waitCallbackRet())
            {
                m_failureMessage = "doOwnershipTransfer Callback Not Invoked";
                return false;
            }
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] ownershipTransfer OUT");
    return true;
}

bool PMCppHelper::provisionACL(DeviceList_t& deviceList, const OicSecAcl_t* acl,
        ResultCallBack resultCallback, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] provisionACL IN");
    OCStackResult res = OC_STACK_OK;
    g_cbInvoked = CALLBACK_NOT_INVOKED;

    res = deviceList[0]->provisionACL(acl, resultCallback);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] provisionACL returns : %s",
            getOCStackResultCPP(res).c_str());

    if (res != expectedResult)
    {
        m_failureMessage = setFailureMessage(expectedResult, res);
        return false;
    }

    if (OC_STACK_OK == res)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            m_failureMessage = "provision ACL Callback Not Invoked";
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] provisionACL OUT");
    return true;
}

bool PMCppHelper::provisionCredentials(DeviceList_t& deviceList, const Credential &cred,
        const OCSecureResource &device2, ResultCallBack resultCallback,
        OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] provisionCredentials IN");
    OCStackResult res = OC_STACK_OK;
    g_cbInvoked = CALLBACK_NOT_INVOKED;

    res = deviceList[0]->provisionCredentials(cred, device2, resultCallback);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] provisionCredentials returns : %s",
            getOCStackResultCPP(res).c_str());

    if (res != expectedResult)
    {
        m_failureMessage = setFailureMessage(expectedResult, res);
        return false;
    }

    if (OC_STACK_OK == res)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            m_failureMessage = "provisionCredentials Callback Not Invoked";
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] provisionCredentials OUT");
    return true;
}

bool PMCppHelper::provisionPairwiseDevices(DeviceList_t& deviceList, const Credential &cred,
        const OicSecAcl_t* acl1, const OCSecureResource &device2, const OicSecAcl_t* acl2,
        ResultCallBack resultCallback, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] provisionPairwiseDevices IN");
    OCStackResult res = OC_STACK_OK;
    g_cbInvoked = CALLBACK_NOT_INVOKED;

    res = deviceList[0]->provisionPairwiseDevices(cred, acl1, device2, acl2, resultCallback);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] provisionPairwiseDevices returns : %s",
            getOCStackResultCPP(res).c_str());

    if (res != expectedResult)
    {
        m_failureMessage = setFailureMessage(expectedResult, res);
        return false;
    }

    if (OC_STACK_OK == res)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            m_failureMessage = "provisionCredentials Callback Not Invoked";
            return false;

        }
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] provisionPairwiseDevices OUT");
    return true;
}

bool PMCppHelper::provisionDirectPairing(DeviceList_t& deviceList, const OicSecPconf_t pconf,
        ResultCallBack resultCallback, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] provisionDirectPairing IN");

    g_cbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult res = deviceList[0]->provisionDirectPairing(&pconf, resultCallback);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] provisionDirectPairing returns : %s",
            getOCStackResultCPP(res).c_str());

    if (res != expectedResult)
    {
        m_failureMessage = setFailureMessage(expectedResult, res);
        return false;
    }

    if (OC_STACK_OK == res)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            m_failureMessage = "provisionDirectPairing Callback Not Invoked";
            return false;

        }
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] provisionDirectPairing OUT");
    return true;
}

bool PMCppHelper::getLinkedDevices(DeviceList_t& deviceList, UuidList_t &uuidList,
        OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] getLinkedDevices IN");
    OCStackResult res = OC_STACK_OK;
    g_cbInvoked = CALLBACK_NOT_INVOKED;

    res = deviceList[0]->getLinkedDevices(uuidList);

    IOTIVITYTEST_LOG(INFO, "[API Return Code] getLinkedDevices returns : %s",
            getOCStackResultCPP(res).c_str());

    if (res != expectedResult)
    {
        m_failureMessage = setFailureMessage(expectedResult, res);
        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] getLinkedDevices OUT");
    return true;
}

bool PMCppHelper::unlinkDevices(DeviceList_t& deviceList, const OCSecureResource &device2,
        ResultCallBack resultCallback, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] unlinkDevices IN");
    OCStackResult res = OC_STACK_OK;
    g_cbInvoked = CALLBACK_NOT_INVOKED;

    res = deviceList[0]->unlinkDevices(device2, resultCallback);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] unlinkDevices returns : %s",
            getOCStackResultCPP(res).c_str());

    if (res != expectedResult)
    {
        m_failureMessage = setFailureMessage(expectedResult, res);
        return false;
    }

    if (OC_STACK_OK == res)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            m_failureMessage = "unlinkDevices Callback Not Invoked";
            return false;

        }
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] unlinkDevices OUT");
    return true;
}

bool PMCppHelper::removeDevice(DeviceList_t& deviceList,
        unsigned short waitTimeForOwnedDeviceDiscovery, ResultCallBack resultCallback,
        OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] removeDevice IN");
    OCStackResult res = OC_STACK_OK;
    g_cbInvoked = CALLBACK_NOT_INVOKED;

    res = deviceList[0]->removeDevice(waitTimeForOwnedDeviceDiscovery, resultCallback);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] removeDevice returns : %s",
            getOCStackResultCPP(res).c_str());

    if (res != expectedResult)
    {
        m_failureMessage = setFailureMessage(expectedResult, res);
        return false;
    }

    if (OC_STACK_OK == res)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            m_failureMessage = "removeDevice Callback Not Invoked";
            return false;

        }
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] removeDevice OUT");
    return true;
}

bool PMCppHelper::removeDeviceWithUuid(unsigned short waitTimeForOwnedDeviceDiscovery, std::string uuid,
        ResultCallBack resultCallback, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] removeDeviceWithUuid IN");
    OCStackResult res = OC_STACK_OK;
    g_cbInvoked = CALLBACK_NOT_INVOKED;

    res = OCSecure::removeDeviceWithUuid(waitTimeForOwnedDeviceDiscovery, uuid, PMCppHelper::provisionCB);
    IOTIVITYTEST_LOG(DEBUG, "[API Return Code] removeDeviceWithUuid returns : %s\n", getOCStackResultCPP(res).c_str());

    if (res != expectedResult)
    {
        IOTIVITYTEST_LOG(ERROR, "Expected Result Mismatch");
        return false;
    }

    if (OC_STACK_OK == res)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "Callback Not Invoked");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] removeDeviceWithUuid OUT");
    return true;
}

OicSecPdAcl_t* createPdAcl(int nPermission)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] createPdAcl IN");

    OicSecPdAcl_t* pdAcl = (OicSecPdAcl_t*) OICCalloc(1, sizeof(OicSecPdAcl_t));

    if (!pdAcl)
    {
        IOTIVITYTEST_LOG(ERROR, "[PMCppHelper] createPdAcl: OICCalloc error return");
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
        IOTIVITYTEST_LOG(ERROR, "[PMCppHelper] createPdAcl: OICCalloc error return");
    }

    for (int i = 0; num > i; ++i)
    {
        size_t len = strlen(rsrc_in[i]) + 1; // '1' for null termination
        char* rsrc = (char*) OICCalloc(len, sizeof(char));
        if (!rsrc)
        {
            IOTIVITYTEST_LOG(ERROR, "[PMCppHelper] createPdAcl: OICCalloc error return");
            IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] createPdAcl OUT");
            return NULL;
        }
        OICStrcpy(rsrc, len, rsrc_in[i]);
        pdAcl->resources[i] = rsrc; // after here, |rsrc| points nothing
    }

    // permission
    pdAcl->permission = nPermission;

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] createPdAcl OUT");
    return pdAcl;
}

/**
 * Callback function for doOwnership Transfer
 *
 * @param[in] result Result list
 * @param[in] hasError indicates if the result has error
 */
void PMCppHelper::ownershipTransferCB(PMResultList_t *result, int hasError)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] ownershipTransferCB IN");

    if (hasError)
    {
        IOTIVITYTEST_LOG(ERROR, "[PMCppHelper] OwnershipTransfer ERROR!!!");
    }
    else
    {
        IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] \nTransferred Ownership successfully for device : ");
        printUuid(result->at(0).deviceId);
        delete result;
    }
    g_cbInvoked = CALLBACK_INVOKED;

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] ownershipTransferCB OUT");
}

/**
 * Callback function for provisioning.
 *
 * @param[in] result Result list
 * @param[in] hasError indicates if the result has error
 */
void PMCppHelper::provisionCB(PMResultList_t *result, int hasError)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] provisionCB IN");

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
            printUuid(result->at(i).deviceId);
        }

        delete result;
    }

    g_cbInvoked = CALLBACK_INVOKED;
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] provisionCB OUT");
}

std::string getOCStackResultCPP(OCStackResult ocstackresult)
{
    std::string resultString = EMPTY_STRING;

    switch (ocstackresult)
    {
        case OC_STACK_OK:
            resultString = "OC_STACK_OK";
            break;
        case OC_STACK_RESOURCE_CREATED:
            resultString = "OC_STACK_RESOURCE_CREATED";
            break;
        case OC_STACK_RESOURCE_DELETED:
            resultString = "OC_STACK_RESOURCE_DELETED";
            break;
        case OC_STACK_INVALID_URI:
            resultString = "OC_STACK_INVALID_URI";
            break;
        case OC_STACK_INVALID_QUERY:
            resultString = "OC_STACK_INVALID_QUERY";
            break;
        case OC_STACK_INVALID_IP:
            resultString = "OC_STACK_INVALID_IP";
            break;
        case OC_STACK_INVALID_PORT:
            resultString = "OC_STACK_INVALID_PORT";
            break;
        case OC_STACK_INVALID_CALLBACK:
            resultString = "OC_STACK_INVALID_CALLBACK";
            break;
        case OC_STACK_INVALID_METHOD:
            resultString = "OC_STACK_INVALID_METHOD";
            break;
        case OC_STACK_INVALID_PARAM:
            resultString = "OC_STACK_INVALID_PARAM";
            break;
        case OC_STACK_INVALID_OBSERVE_PARAM:
            resultString = "OC_STACK_INVALID_OBSERVE_PARAM";
            break;
        case OC_STACK_NO_MEMORY:
            resultString = "OC_STACK_NO_MEMORY";
            break;
        case OC_STACK_COMM_ERROR:
            resultString = "OC_STACK_COMM_ERROR";
            break;
        case OC_STACK_TIMEOUT:
            resultString = "OC_STACK_TIMEOUT";
            break;
        case OC_STACK_ADAPTER_NOT_ENABLED:
            resultString = "OC_STACK_ADAPTER_NOT_ENABLED";
            break;
        case OC_STACK_NOTIMPL:
            resultString = "OC_STACK_NOTIMPL";
            break;
        case OC_STACK_NO_RESOURCE:
            resultString = "OC_STACK_NO_RESOURCE";
            break;
        case OC_STACK_ERROR:
            return "OC_STACK_ERROR";
            break;
        default:
            resultString = "UNKNOWN_STATE";
            break;
    }

    return resultString;
}

/**
 * Function for getting failure message
 */
std::string PMCppHelper::getFailureMessage()
{
    return m_failureMessage;
}

/**
 * Function to set failure message
 */
std::string PMCppHelper::setFailureMessage(OCStackResult expectedResult, OCStackResult actualResult)
{
    std::string errorMessage = "\033[1;31m[Error] Expected : \033[0m"
            + getOCStackResultCPP(expectedResult) + " \033[1;31mActual : \033[0m"
            + getOCStackResultCPP(actualResult);
    return errorMessage;
}

int PMCppHelper::waitCallbackRet()
{
    IOTIVITYTEST_LOG(DEBUG, "waitCallbackRet IN");

    for (int i = 0; CALLBACK_TIMEOUT > i; ++i)
    {
        IOTIVITYTEST_LOG(DEBUG, "waitCallbackRet Loop = %d", i);

        if (CALLBACK_INVOKED == g_cbInvoked)
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
