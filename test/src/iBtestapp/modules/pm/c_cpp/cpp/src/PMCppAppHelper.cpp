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
#include "../include/PMCppAppHelper.h"

int g_cbInvoked = CALLBACK_NOT_INVOKED;
static OicSecPconf_t g_pconf;
static const OicSecPrm_t  SUPPORTED_PRMS[1] =
{
    PRM_PRE_CONFIGURED,
};

void printDevices(DeviceList_t &list)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] Total Devices %d", list.size());

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
    printf("[PMHelper] printUuid IN\n");

    for (int i = 0; i < UUID_LENGTH;)
    {
        printf("%02X", (*uid).id[i++]);
        if (i == 4 || i == 6 || i == 8 || i == 10) // canonical format for UUID has '8-4-4-4-12'
        {
            printf(DASH);
        }
    }
    printf("\n");

    printf("[PMHelper] printUuid OUT\n");
}

void printUuid(OicUuid_t uuid)
{
    for (int i = 0; i < UUID_LENGTH; i++)
    {
        std::cout << std::hex << uuid.id[i] << " ";
    }
    std::cout << std::endl;
}

FILE* clientCppAppOpen(const char *UNUSED_PARAM, const char *mode)
{
    (void) UNUSED_PARAM;
    return fopen(CLIENT_CBOR, mode);
}

void InputPinCB(char* pin, size_t len)
{

    if (!pin || OXM_RANDOM_PIN_SIZE >= len)
    {
        IOTIVITYTEST_LOG(ERROR, "inputPinCB invalid parameters");
        return;
    }

    printf("   > INPUT PIN: ");
    for (int ret = 0; 1 != ret;)
    {
        ret = scanf("%8s", pin);
        for (; 0x20 <= getchar();)
            ; // for removing overflow garbages
              // '0x20<=code' is character region
    }

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

bool provisionInit()
{
    OCPersistentStorage ps
    { clientCppAppOpen, fread, fwrite, fclose, unlink };

    PlatformConfig cfg
    { OC::ServiceType::InProc, OC::ModeType::Both, "0.0.0.0", 0, OC::QualityOfService::LowQos, &ps };

    OCPlatform::Configure(cfg);

    if (OCSecure::provisionInit(EMPTY_STRING) != OC_STACK_OK)
    {
        printf("Provision Initialization failed");
        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "Provision Initialization Successful\n");
    return true;
}

bool discoverUnownedDevices(int time, DeviceList_t& deviceList, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] discoverUnownedDevices IN\n");

    OCStackResult res = OC_STACK_OK;
    res = OCSecure::discoverUnownedDevices(time, deviceList);
    IOTIVITYTEST_LOG(DEBUG, "[API Return Code] discoverUnownedDevices returns : %s\n",
            getOCStackResultCPP(res).c_str());

    if (res != expectedResult)
    {
        IOTIVITYTEST_LOG(ERROR, "Expected Result Mismatch");
        return false;
    }

    if (deviceList.size())
    {
        IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] Found secure devices, count = %d\n", deviceList.size());
        printDevices(deviceList);

    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] discoverUnownedDevices OUT\n");
    return true;
}

bool discoverOwnedDevices(int time, DeviceList_t& deviceList, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] discoverOwnedDevices IN");

    OCStackResult res = OC_STACK_OK;
    res = OCSecure::discoverOwnedDevices(time, deviceList);
    IOTIVITYTEST_LOG(DEBUG, "[API Return Code] discoverOwnedDevices returns : %s\n",
            getOCStackResultCPP(res).c_str());

    if (res != expectedResult)
    {
        IOTIVITYTEST_LOG(ERROR, "Expected Result Mismatch");
        return false;
    }

    if (deviceList.size())
    {
        IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] Found secure devices, count = %d\n", deviceList.size());
        printDevices(deviceList);
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] discoverOwnedDevices OUT\n");
    return true;
}

bool getDevInfoFromNetwork(unsigned short time, DeviceList_t& ownedDevList,
        DeviceList_t &unownedDevList, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] getDevInfoFromNetwork IN");

    OCStackResult res = OC_STACK_OK;
    res = OCSecure::getDevInfoFromNetwork(time, ownedDevList, unownedDevList);
    IOTIVITYTEST_LOG(DEBUG, "[API Return Code] getDevInfoFromNetwork returns : %s",
            getOCStackResultCPP(res).c_str());

    if (res != expectedResult)
    {
        IOTIVITYTEST_LOG(ERROR, "Expected Result Mismatch");
        return false;
    }

    if (unownedDevList.size())
    {
        IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] Found secure devices, count = %d\n", unownedDevList.size());
        printDevices(unownedDevList);
    }

    if (ownedDevList.size())
    {
        IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] Found secure devices, count = %d\n", ownedDevList.size());
        printDevices(ownedDevList);
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] getDevInfoFromNetwork OUT");
    return true;
}

bool setOwnerTransferCallbackData(int num, OTMCallbackData_t &data, InputPinCallback inputPin,
        OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] setOwnerTransferCallbackData IN");

    OCStackResult res = OC_STACK_OK;

    res = OCSecure::setOwnerTransferCallbackData((OicSecOxm_t) num, &data, inputPin);
    IOTIVITYTEST_LOG(DEBUG, "[API Return Code] setOwnerTransferCallbackData returns : %s\n",
            getOCStackResultCPP(res).c_str());

    if (res != expectedResult)
    {
        IOTIVITYTEST_LOG(ERROR, "Expected Result Mismatch");
        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] setOwnerTransferCallbackData OUT");
    return true;
}

bool doOwnershipTransfer(DeviceList_t &data, ResultCallBack resultCallback,
        OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] ownershipTransfer IN\n");
    OCStackResult res = OC_STACK_OK;
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] Device to be owned in : %d \n", data.size());

    for (int i = 0; i < data.size(); i++)
    {
        g_cbInvoked = CALLBACK_NOT_INVOKED;

        res = data[i]->doOwnershipTransfer(resultCallback);
        IOTIVITYTEST_LOG(DEBUG, "[API Return Code] doOwnershipTransfer returns : %s\n",
                getOCStackResultCPP(res).c_str());

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
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] ownershipTransfer OUT\n");
    return true;
}

bool provisionACL(DeviceList_t& deviceList, const OicSecAcl_t* acl, ResultCallBack resultCallback,
        OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] provisionACL IN\n");
    OCStackResult res = OC_STACK_OK;
    g_cbInvoked = CALLBACK_NOT_INVOKED;

    res = deviceList[0]->provisionACL(acl, resultCallback);
    IOTIVITYTEST_LOG(DEBUG, "[API Return Code] provisionACL returns : %s\n", getOCStackResultCPP(res).c_str());

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

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] provisionACL OUT\n");
    return true;
}

bool provisionCredentials(DeviceList_t& deviceList, const Credential &cred,
        const OCSecureResource &device2, ResultCallBack resultCallback,
        OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] provisionCredentials IN\n");
    OCStackResult res = OC_STACK_OK;
    g_cbInvoked = CALLBACK_NOT_INVOKED;

    res = deviceList[0]->provisionCredentials(cred, device2, resultCallback);
    IOTIVITYTEST_LOG(DEBUG, "[API Return Code] provisionCredentials returns : %s\n",
            getOCStackResultCPP(res).c_str());

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

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] provisionCredentials OUT\n");
    return true;
}

bool provisionPairwiseDevices(DeviceList_t& deviceList, const Credential &cred,
        const OicSecAcl_t* acl1, const OCSecureResource &device2, const OicSecAcl_t* acl2,
        ResultCallBack resultCallback, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] provisionPairwiseDevices IN\n");
    OCStackResult res = OC_STACK_OK;
    g_cbInvoked = CALLBACK_NOT_INVOKED;

    res = deviceList[0]->provisionPairwiseDevices(cred, acl1, device2, acl2, resultCallback);
    IOTIVITYTEST_LOG(DEBUG, "[API Return Code] provisionPairwiseDevices returns : %s\n",
            getOCStackResultCPP(res).c_str());

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

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] provisionPairwiseDevices OUT\n");
    return true;
}

bool getLinkedDevices(DeviceList_t& deviceList, UuidList_t &uuidList, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] getLinkedDevices IN\n");
    OCStackResult res = OC_STACK_OK;
    g_cbInvoked = CALLBACK_NOT_INVOKED;

    res = deviceList[0]->getLinkedDevices(uuidList);

    IOTIVITYTEST_LOG(DEBUG, "[API Return Code] getLinkedDevices returns : %s\n", getOCStackResultCPP(res).c_str());

    if (res != expectedResult)
    {
        //m_failureMessage = setFailureMessage(expectedResult, res);
        return false;
    }

    if (!uuidList.size())
    {
        IOTIVITYTEST_LOG(DEBUG, "No Devices are Linked\n");
    }

    for (unsigned int i = 0; i < uuidList.size(); i++)
    {
        printf("%d. Linked Device ID : ", i);
        printUuid(uuidList[i]);
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] getLinkedDevices OUT\n");
    return true;
}

bool unlinkDevices(DeviceList_t& deviceList, const OCSecureResource &device2,
        ResultCallBack resultCallback, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] unlinkDevices IN\n");
    OCStackResult res = OC_STACK_OK;
    g_cbInvoked = CALLBACK_NOT_INVOKED;

    res = deviceList[0]->unlinkDevices(device2, resultCallback);
    IOTIVITYTEST_LOG(DEBUG, "[API Return Code] unlinkDevices returns : %s\n", getOCStackResultCPP(res).c_str());

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

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] unlinkDevices OUT\n");
    return true;
}

bool removeDevice(DeviceList_t& deviceList, unsigned short waitTimeForOwnedDeviceDiscovery,
        ResultCallBack resultCallback, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] removeDevice IN");
    OCStackResult res = OC_STACK_OK;
    g_cbInvoked = CALLBACK_NOT_INVOKED;

    res = deviceList[0]->removeDevice(waitTimeForOwnedDeviceDiscovery, resultCallback);
    IOTIVITYTEST_LOG(DEBUG, "[API Return Code] removeDevice returns : %s\n", getOCStackResultCPP(res).c_str());

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

bool removeDeviceWithUuid(unsigned short waitTimeForOwnedDeviceDiscovery, std::string uuid,
        ResultCallBack resultCallback, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] removeDeviceWithUuid IN");
    OCStackResult res = OC_STACK_OK;
    g_cbInvoked = CALLBACK_NOT_INVOKED;

    res = OCSecure::removeDeviceWithUuid(waitTimeForOwnedDeviceDiscovery, uuid, resultCallback);
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

/**
 * Callback function for doOwnership Transfer
 *
 * @param[in] result Result list
 * @param[in] hasError indicates if the result has error
 */
void ownershipTransferCB(PMResultList_t *result, int hasError)
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
void provisionCB(PMResultList_t *result, int hasError)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] provisionCB IN\n");

    if (hasError > 4)
    {
        IOTIVITYTEST_LOG(ERROR, "[PMCppHelper] Provisioning ERROR %d!!!\n", hasError);
    }
    else
    {
        IOTIVITYTEST_LOG(DEBUG,"[PMCppHelper] Received provisioning results: \n");

        for (unsigned int i = 0; i < result->size(); i++)
        {
            IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] Result is = %d for device : ", result->at(i).res);
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

int waitCallbackRet()
{
    printf("waitCallbackRet IN\n");

    for (int i = 0; CALLBACK_TIMEOUT > i; ++i)
    {
        printf("waitCallbackRet Loop = %d\n", i);

        if (CALLBACK_INVOKED == g_cbInvoked)
        {
            return CALLBACK_INVOKED;
        }

        sleep(DELAY_SHORT);

        if (OC_STACK_OK != OCProcess())
        {
            printf("OCStack process error\n");
            return CALLBACK_NOT_INVOKED;
        }
    }

    printf("waitCallbackRet OUT\n");
    return CALLBACK_NOT_INVOKED;
}

static void provisionDirectPairingCB(PMResultList_t *result, int hasError)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] provisionDirectPairingCB IN");
    if (hasError)
    {
        std::cout << "Error in provisioning operation!" << std::endl;
    }
    else
    {
        std::cout << "\nReceived provisioning results: Direct Pairing is successful ";
        for (unsigned int i = 0; i < result->size(); i++)
        {
            std::cout << "Result is = " << result->at(i).res << " for device ";
            printUuid(result->at(i).deviceId);
        }
    }

    g_cbInvoked = CALLBACK_INVOKED;
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] provisionDirectPairingCB OUT");
}

static OicSecPdAcl_t* inputPdACL(int nPermission)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] inputPdACL IN");
    OicSecPdAcl_t *acl = (OicSecPdAcl_t *)OICCalloc(1,sizeof(OicSecPdAcl_t));

    acl->resourcesLen =1;
    acl->resources = (char **)OICCalloc(acl->resourcesLen, sizeof(char *));

    for (size_t i = 0; i < acl->resourcesLen; i++)
    {
        char* resourceName = LIGHT_RESOURCE_URI_01;
        acl->resources[i] = OICStrdup(resourceName);
    }

    acl->permission = nPermission;
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] inputPdACL OUT");
    return acl;
}

bool provisionDirectPairing(DeviceList_t& deviceList, const OicSecPconf_t pconf,
        ResultCallBack resultCallback, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] provisionDirectPairing IN");

    g_cbInvoked = CALLBACK_NOT_INVOKED;

    std::string pin = DEFAULT_DP_PROVSIONING_PIN;
    g_pconf.edp = true;

    g_pconf.prmLen = sizeof(SUPPORTED_PRMS) / sizeof(OicSecPrm_t);

    g_pconf.prm = (OicSecPrm_t *) OICCalloc(g_pconf.prmLen, sizeof(OicSecPrm_t));

    if (g_pconf.prm)
    {
        for (size_t i = 0; i < g_pconf.prmLen; i++)
        {
            g_pconf.prm[i] = SUPPORTED_PRMS[i];
        }
    }

    memcpy(g_pconf.pin.val, pin.c_str(), DP_PIN_LENGTH);
    g_pconf.pdacls = inputPdACL(FULL_PERMISSION);

    OCStackResult res = deviceList[0]->provisionDirectPairing(&g_pconf, provisionDirectPairingCB);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] provisionDirectPairing returns : %s",
            getOCStackResultCPP(res).c_str());

    if (res != expectedResult)
    {
        return false;
    }

    if (OC_STACK_OK == res)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] provisionDirectPairing OUT");
    return true;
}
