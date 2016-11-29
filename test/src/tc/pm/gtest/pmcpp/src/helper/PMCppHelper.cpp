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
#include "PMCppUtilityHelper.h"

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

void PMCppHelper::InputPinCB(char* pin, size_t len)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] inputPinCB IN");

    CommonUtil::waitInSecond(DELAY_LONG);
    char buff[PIN_MAX_SIZE];

    if (!pin || OXM_RANDOM_PIN_MIN_SIZE >= len)
    {
        IOTIVITYTEST_LOG(ERROR, "[PMHelper] Invalid PIN");
        return;
    }

    FILE *fp;
    fp = fopen(RANDOM_PIN_TEXT_FILE, "r");
    fgets(buff, PIN_MAX_SIZE, (FILE*) fp);
    fclose(fp);

    IOTIVITYTEST_LOG(DEBUG, "[PIN CODE] %s\n", buff);

    strcpy(pin, (const char*) buff);

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] inputPinCB Out");
}

OicSecAcl_t* PMCppHelper::createAcl(const int dev_num, int nPermission,
        DeviceList_t &m_OwnedDevList)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] PMCppHelper::createAcl IN");

    OicSecAcl_t* acl = (OicSecAcl_t*) OICCalloc(1, sizeof(OicSecAcl_t));
    OicSecAce_t* ace = (OicSecAce_t*) OICCalloc(1, sizeof(OicSecAce_t));

    LL_APPEND(acl->aces, ace);
    int num = 0;

    if (dev_num == 1)
    {
        memcpy(&ace->subjectuuid, ACL_ROWNER_UUID_02, UUID_LENGTH);
    }
    else
    {
        memcpy(&ace->subjectuuid, ACL_ROWNER_UUID_01, UUID_LENGTH);
    }

    num = 1;

    for (int i = 0; num > i; ++i)
    {
        OicSecRsrc_t* rsrc = (OicSecRsrc_t*) OICCalloc(1, sizeof(OicSecRsrc_t));

        // Resource URI
        size_t len = strlen(LIGHT_RESOURCE_URI_01) + 1; // '1' for null termination
        rsrc->href = (char*) OICCalloc(len, sizeof(char));
        OICStrcpy(rsrc->href, len, LIGHT_RESOURCE_URI_01);

        // Resource Type
        rsrc->typeLen = 1;
        rsrc->types = (char**) OICCalloc(rsrc->typeLen, sizeof(char*));
        for (size_t i = 0; i < rsrc->typeLen; i++)
        {
            rsrc->types[i] = OICStrdup(LIGHT_RESOURCE_URI_01);
        }

        rsrc->interfaceLen = 1;
        rsrc->interfaces = (char**) OICCalloc(rsrc->typeLen, sizeof(char*));
        for (size_t i = 0; i < rsrc->interfaceLen; i++)
        {
            rsrc->interfaces[i] = OICStrdup(LIGHT_RESOURCE_URI_01);
        }

        LL_APPEND(ace->resources, rsrc);
    }

    ace->permission = nPermission;

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] PMCppHelper::createAcl OUT");
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
            PMCppUtilityHelper::getOCStackResult(res));

    if (res != expectedResult)
    {
        m_failureMessage = PMCppUtilityHelper::setFailureMessage(expectedResult, res);
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
            PMCppUtilityHelper::getOCStackResult(res));

    if (res != expectedResult)
    {
        m_failureMessage = PMCppUtilityHelper::setFailureMessage(expectedResult, res);
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
            PMCppUtilityHelper::getOCStackResult(res));

    if (res != expectedResult)
    {
        m_failureMessage = PMCppUtilityHelper::setFailureMessage(expectedResult, res);
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
            PMCppUtilityHelper::getOCStackResult(res));

    if (res != expectedResult)
    {
        m_failureMessage = PMCppUtilityHelper::setFailureMessage(expectedResult, res);
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

    for (size_t i = 0; i < data.size(); i++)
    {
        g_cbInvoked = CALLBACK_NOT_INVOKED;
        res = data[i]->doOwnershipTransfer(resultCallback);
        IOTIVITYTEST_LOG(INFO, "[API Return Code] doOwnershipTransfer returns : %s",
                PMCppUtilityHelper::getOCStackResult(res));

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
            PMCppUtilityHelper::getOCStackResult(res));

    if (res != expectedResult)
    {
        m_failureMessage = PMCppUtilityHelper::setFailureMessage(expectedResult, res);
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
            PMCppUtilityHelper::getOCStackResult(res));

    if (res != expectedResult)
    {
        m_failureMessage = PMCppUtilityHelper::setFailureMessage(expectedResult, res);
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
            PMCppUtilityHelper::getOCStackResult(res));

    if (res != expectedResult)
    {
        m_failureMessage = PMCppUtilityHelper::setFailureMessage(expectedResult, res);
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
            PMCppUtilityHelper::getOCStackResult(res));

    if (res != expectedResult)
    {
        m_failureMessage = PMCppUtilityHelper::setFailureMessage(expectedResult, res);
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
            PMCppUtilityHelper::getOCStackResult(res));

    if (res != expectedResult)
    {
        m_failureMessage = PMCppUtilityHelper::setFailureMessage(expectedResult, res);
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
    IOTIVITYTEST_LOG(DEBUG, "[API Return Code] removeDevice returns : %s",
            PMCppUtilityHelper::getOCStackResult(res));

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

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] removeDevice OUT");
    return true;
}

bool PMCppHelper::provisionTrustCertChain(DeviceList_t& deviceList, OicSecCredType_t type,
        uint16_t credId, ResultCallBack resultCallback, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] provisionTrustCertChain IN");
    OCStackResult res = OC_STACK_OK;
    g_cbInvoked = CALLBACK_NOT_INVOKED;

    res = deviceList[0]->provisionTrustCertChain(type, credId, resultCallback);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] provisionTrustCertChain returns : %s",
            PMCppUtilityHelper::getOCStackResult(res));

    if (res != expectedResult)
    {
        m_failureMessage = PMCppUtilityHelper::setFailureMessage(expectedResult, res);
        return false;
    }

    if (OC_STACK_OK == res)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            m_failureMessage = "provisionTrustCertChain Callback Not Invoked";
            return false;

        }
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] provisionTrustCertChain OUT");
    return true;
}

bool PMCppHelper::removeDeviceWithUuid(unsigned short waitTimeForOwnedDeviceDiscovery,
        std::string uuid, ResultCallBack resultCallback, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] removeDeviceWithUuid IN");
    OCStackResult res = OC_STACK_OK;
    g_cbInvoked = CALLBACK_NOT_INVOKED;

    res = OCSecure::removeDeviceWithUuid(waitTimeForOwnedDeviceDiscovery, uuid,
            PMCppHelper::provisionCB);
    IOTIVITYTEST_LOG(DEBUG, "[API Return Code] removeDeviceWithUuid returns : %s",
            PMCppUtilityHelper::getOCStackResult(res));

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
        g_cbInvoked = CALLBACK_INVOKED;
        delete result;
    }
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
        g_cbInvoked = CALLBACK_INVOKED;
        delete result;
    }
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] provisionCB OUT");
}
/**
 * Function for getting failure message
 */
std::string PMCppHelper::getFailureMessage()
{
    return m_failureMessage;
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
