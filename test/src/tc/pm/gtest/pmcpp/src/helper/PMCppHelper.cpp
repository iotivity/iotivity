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

OicSecAcl_t* createAcl(const int dev_num)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] createAcl IN");

    OicSecAcl_t *acl = (OicSecAcl_t *) OICCalloc(1, sizeof(OicSecAcl_t));
    char* subject = NULL;
    char* owner = NULL;

    if (dev_num == 1)
    {
        subject = (char*) ACL_SUBJECT_UUID_02;
        owner = (char*) ACL_ROWNER_UUID_02;
    }
    else
    {
        subject = (char*) ACL_SUBJECT_UUID_01;
        owner = (char*) ACL_ROWNER_UUID_01;
    }

    // Subject
    strcpy((char *) acl->subject.id, (const char*) subject);

    // resource
    acl->resourcesLen = ACL_RESOURCE_LEN;
    acl->resources = (char **) OICCalloc(acl->resourcesLen, sizeof(char *));
    acl->resources[0] = (char*) LIGHT_RESOURCE_URI_01;
    acl->resources[1] = (char*) LIGHT_RESOURCE_URI_02;

    // Permission
    acl->permission = FULL_PERMISSION;

    // Owner
    memcpy(&acl->rownerID, &owner, sizeof(OicUuid_t));

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] createAcl OUT");

    return acl;
}

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
    else if (deviceList.size())
    {
        IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] Found secure devices, count = %d",
                deviceList.size());
        printDevices(deviceList);

    }
    else
    {
        if (res == OC_STACK_OK)
        {
            IOTIVITYTEST_LOG(ERROR, "[PMCppHelper] No Device found");
            return false;
        }
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
    else if (deviceList.size())
    {
        IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] Found secure devices, count = %d",
                deviceList.size());
        printDevices(deviceList);
    }
    else
    {
        if (res == OC_STACK_OK)
        {
            IOTIVITYTEST_LOG(ERROR, "[PMCppHelper] No Device found");
            return false;
        }
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
    else if (ownedDevList.size())
    {
        IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] Found secure devices, count = %d",
                ownedDevList.size());
        printDevices(ownedDevList);
    }
    else if (unownedDevList.size())
    {
        IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] Found secure devices, count = %d",
                unownedDevList.size());
        printDevices(unownedDevList);
    }
    else
    {
        if (res == OC_STACK_OK)
        {
            IOTIVITYTEST_LOG(ERROR, "[PMCppHelper] No Device found");
            return false;
        }

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
        IOTIVITYTEST_LOG(ERROR, "[PMCppHelper] Provisioning ERROR!!!");
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
    }

    IOTIVITYTEST_LOG(DEBUG, "waitCallbackRet OUT");
    return CALLBACK_NOT_INVOKED;
}
