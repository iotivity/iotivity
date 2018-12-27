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
#include "PMCppCallbackHelper.h"
#include "PMCppUtilityHelper.h"

OCPersistentStorage PMCppHelper::s_ps =
{ 0, 0, 0, 0, 0 };

FILE* PMCppHelper::clientOpen(const char *UNUSED_PARAM, const char *mode)
{
    if (0 == strcmp(UNUSED_PARAM, OC_SECURITY_DB_DAT_FILE_NAME))
    {
        return fopen(CBOR_DB_PATH, mode);
    }
    else
    {
        return fopen(UNUSED_PARAM, mode);
    }
}

bool PMCppHelper::provisionInit(const std::string& dbPath)
{
    PMCppHelper::s_ps.open = PMCppHelper::clientOpen;
    PMCppHelper::s_ps.read = fread;
    PMCppHelper::s_ps.write = fwrite;
    PMCppHelper::s_ps.close = fclose;
    PMCppHelper::s_ps.unlink = unlink;

    PlatformConfig cfg
    { OC::ServiceType::InProc, OC::ModeType::Both, "0.0.0.0", 0, OC::QualityOfService::LowQos,
            &PMCppHelper::s_ps };

    OCPlatform::Configure(cfg);

    if (OCSecure::provisionInit(dbPath) != OC_STACK_OK)
    {
        std::cout << "PM Init failed" << std::endl;
        return false;
    }

    std::cout << "setup succeeded" << std::endl;
    return true;
}

FILE* PMCppHelper::clientOpen2(const char *UNUSED_PARAM, const char *mode)
{
    if (0 == strcmp(UNUSED_PARAM, OC_SECURITY_DB_DAT_FILE_NAME))
    {
        return fopen(JUSTWORKS_SERVER_CBOR_O1, mode);
    }
    else
    {
        return fopen(UNUSED_PARAM, mode);
    }
}

bool PMCppHelper::provisionInit2(const std::string& dbPath)
{
    __FUNC_IN__

    PMCppHelper::s_ps.open = PMCppHelper::clientOpen2;
    PMCppHelper::s_ps.read = fread;
    PMCppHelper::s_ps.write = fwrite;
    PMCppHelper::s_ps.close = fclose;
    PMCppHelper::s_ps.unlink = unlink;

    PlatformConfig cfg
    { OC::ServiceType::InProc, OC::ModeType::Both, "0.0.0.0", 0, OC::QualityOfService::LowQos,
            &PMCppHelper::s_ps };

    OCPlatform::Configure(cfg);

    if (OCSecure::provisionInit(dbPath) != OC_STACK_OK)
    {
        std::cout << "PM Init failed" << std::endl;
        return false;
    }

    std::cout << "setup succeeded" << std::endl;
    __FUNC_OUT__
    return true;
}

/**
 * Helper Method for OCPDMCleanupForTimeout
 */
bool PMCppHelper::pdmCleanupForTimeout(OCStackResult expectedResult)
{
    __FUNC_IN__

    OCStackResult res = OCSecure::pdmCleanupForTimeout();

    IOTIVITYTEST_LOG(INFO, "[PMHelper] pdmCleanupForTimeout API returns: %s",
            CommonUtil::getOCStackResult(res));

    if (expectedResult != res)
    {
        m_failureMessage = PMCppUtilityHelper::setFailureMessage(expectedResult, res);
        return false;
    }

    __FUNC_OUT__
    return true;
}

bool PMCppHelper::configSelfOwnership(OCStackResult expectedResult)
{
    __FUNC_IN__

    OCStackResult result = OCSecure::configSelfOwnership();
    IOTIVITYTEST_LOG(INFO, "[PMHelper] configSelfOwnership returns %s",
            CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = PMCppUtilityHelper::setFailureMessage(expectedResult, result);
        return false;
    }

    __FUNC_OUT__
    return true;
}

void PMCppHelper::OnInputPinCB(OicUuid_t deviceId, char* pinBuf, size_t bufSize)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] OnInputPinCB IN");

    CommonUtil::waitInSecond(DELAY_LONG);
    char buff[PIN_MAX_SIZE];

    if (!pinBuf || OXM_RANDOM_PIN_MIN_SIZE >= bufSize)
    {
        IOTIVITYTEST_LOG(ERROR, "[PMHelper] Invalid PIN");
        return;
    }

    FILE *fp;
    fp = fopen(RANDOM_PIN_TEXT_FILE, "r");
    char *str;
    str = fgets(buff, PIN_MAX_SIZE, (FILE*) fp);
    OC_UNUSED(str);
    fclose(fp);

    IOTIVITYTEST_LOG(DEBUG, "[PIN CODE] %s\n", buff);

    strncpy(pinBuf, (const char *) buff, bufSize);

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] OnInputPinCB Out");
}

void PMCppHelper::OnDisplayPinCB(char* pinData, size_t pinSize)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] OnDisplayPinCB IN");

    IOTIVITYTEST_LOG(DEBUG, "[PIN CODE] %s\n", pinData);

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] OnDisplayPinCB Out");
}

void PMCppHelper::inputPinCB(char* pin, size_t len)
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
    char *str;
    str = fgets(buff, PIN_MAX_SIZE, (FILE *) fp);
    OC_UNUSED(str);
    fclose(fp);

    IOTIVITYTEST_LOG(DEBUG, "[PIN CODE] %s\n", buff);

    strncpy(pin, (const char *) buff, len);

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] inputPinCB Out");
}

OCStackResult PMCppHelper::displayMutualVerifNumCB(uint8_t mutualVerifNum[MUTUAL_VERIF_NUM_LEN])
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] displayNumCB IN");
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] ############ mutualVerifNum ############");

    for (int i = 0; i < MUTUAL_VERIF_NUM_LEN; i++)
    {
        IOTIVITYTEST_LOG(DEBUG, "[Test Server] %02X ", mutualVerifNum[i]);
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] ############ mutualVerifNum ############");
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] displayNumCB OUT");
    return OC_STACK_OK;
}

OCStackResult PMCppHelper::confirmMutualVerifNumCB(void)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] confirmMutualVerifNumCB IN");
    for (;;)
    {
        int userConfirm;
        CommonUtil::waitInSecond(DELAY_SHORT);
        userConfirm = 1;
        OC_UNUSED(userConfirm);
        break;
    }
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] confirmMutualVerifNumCB OUT");
    return OC_STACK_OK;
}

void PMCppHelper::createAcl(OicSecAcl_t *acl, const int dev_num, int nPermission,
        DeviceList_t &m_OwnedDevList)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] PMCppHelper::createAcl IN");

    OicSecAce_t* ace = (OicSecAce_t*) OICCalloc(1, sizeof(OicSecAce_t));

    LL_APPEND(acl->aces, ace);
    int num = 0;

    if (dev_num == 1)
    {
        memcpy(&ace->subjectuuid, DEFAULT_SUBJECT_UUID, UUID_LENGTH);
    }
    else
    {
        memcpy(&ace->subjectuuid, DEFAULT_SUBJECT_UUID, UUID_LENGTH);
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

    ace->permission = nPermission;
}

OicSecAcl_t* PMCppHelper::createAclForLEDAccess(const OicUuid_t* subject)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] createAclForLEDAccess In");

    OicSecAcl_t* acl = (OicSecAcl_t*) OICCalloc(1, sizeof(OicSecAcl_t));
    OicSecAce_t* ace = (OicSecAce_t*) OICCalloc(1, sizeof(OicSecAce_t));

    LL_APPEND(acl->aces, ace);
    memcpy(ace->subjectuuid.id, subject->id, sizeof(subject->id));

    // fill the href
    char* rsrc_in = "/a/led"; // '1' for null termination
    OicSecRsrc_t* rsrc = (OicSecRsrc_t*) OICCalloc(1, sizeof(OicSecRsrc_t));

    size_t len = strlen(rsrc_in) + 1; // '1' for null termination
    rsrc->href = (char*) OICCalloc(len, sizeof(char));

    OICStrcpy(rsrc->href, len, rsrc_in);

    //fill the resource type (rt)
    rsrc->typeLen = 1;
    rsrc->types = (char**) OICCalloc(1, sizeof(char*));
    rsrc->types[0] = OICStrdup("oic.r.core");
    rsrc->interfaceLen = 1;
    rsrc->interfaces = (char**) OICCalloc(1, sizeof(char*));
    rsrc->interfaces[0] = OICStrdup("oic.if.baseline");

    LL_APPEND(ace->resources, rsrc);

    ace->permission = PERMISSION_FULL_CONTROL;

    ace->eownerID = (OicUuid_t*) OICCalloc(1, sizeof(OicUuid_t));

    memcpy(ace->eownerID->id, subject->id, sizeof(subject->id));

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] createAclForLEDAccess OUT");

    return acl;
}

bool PMCppHelper::setRandomPinPolicy(size_t pinSize, OicSecPinType_t pinType,
        OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] setRandomPinPolicy IN");

    OCStackResult res = OC_STACK_OK;
    res = OCSecure::setRandomPinPolicy(pinSize, pinType);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] setRandomPinPolicy returns : %s",
            CommonUtil::getOCStackResult(res));

    if (res != expectedResult)
    {
        m_failureMessage = PMCppUtilityHelper::setFailureMessage(expectedResult, res);
        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] setRandomPinPolicy OUT");
    return true;
}

bool PMCppHelper::setDisplayPinCB(GeneratePinCallback displayPin, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] setDisplayPinCB IN");

    OCStackResult res = OC_STACK_OK;
    res = OCSecure::setDisplayPinCB(displayPin);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] setDisplayPinCB returns : %s",
            CommonUtil::getOCStackResult(res));

    if (res != expectedResult)
    {
        m_failureMessage = PMCppUtilityHelper::setFailureMessage(expectedResult, res);
        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] setDisplayPinCB OUT");
    return true;
}

bool PMCppHelper::unsetDisplayPinCB(OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] unsetDisplayPinCB IN");

    OCStackResult res = OC_STACK_OK;
    res = OCSecure::unsetDisplayPinCB();
    IOTIVITYTEST_LOG(INFO, "[API Return Code] unsetDisplayPinCB returns : %s",
            CommonUtil::getOCStackResult(res));

    if (res != expectedResult)
    {
        m_failureMessage = PMCppUtilityHelper::setFailureMessage(expectedResult, res);
        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] unsetDisplayPinCB OUT");
    return true;
}

bool PMCppHelper::registerDisplayPinCallback(DisplayPinCB displayPinCB,
        DisplayPinCallbackHandle* displayPinCallbackHandle, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] registerDisplayPinCallback IN");

    OCStackResult res = OC_STACK_OK;
    res = OCSecure::registerDisplayPinCallback(displayPinCB, displayPinCallbackHandle);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] registerDisplayPinCallback returns : %s",
            CommonUtil::getOCStackResult(res));

    if (res != expectedResult)
    {
        m_failureMessage = PMCppUtilityHelper::setFailureMessage(expectedResult, res);
        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] registerDisplayPinCallback OUT");
    return true;
}

bool PMCppHelper::deregisterDisplayPinCallback(DisplayPinCallbackHandle displayPinCallbackHandle,
        OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] deregisterDisplayPinCallback IN");

    OCStackResult res = OC_STACK_OK;
    res = OCSecure::deregisterDisplayPinCallback(displayPinCallbackHandle);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] deregisterDisplayPinCallback returns : %s",
            CommonUtil::getOCStackResult(res));

    if (res != expectedResult)
    {
        m_failureMessage = PMCppUtilityHelper::setFailureMessage(expectedResult, res);
        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] deregisterDisplayPinCallback OUT");
    return true;
}

bool PMCppHelper::registerInputPinCallback(InputPinCB inputPinCB,
        InputPinCallbackHandle* inputPinCallbackHandle, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] registerInputPinCallback IN");

    OCStackResult res = OC_STACK_OK;
    res = OCSecure::registerInputPinCallback(inputPinCB, inputPinCallbackHandle);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] registerInputPinCallback returns : %s",
            CommonUtil::getOCStackResult(res));

    if (res != expectedResult)
    {
        m_failureMessage = PMCppUtilityHelper::setFailureMessage(expectedResult, res);
        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] registerInputPinCallback OUT");
    return true;
}

bool PMCppHelper::deregisterInputPinCallback(InputPinCallbackHandle inputPinCallbackHandle,
        OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] deregisterInputPinCallback IN");

    OCStackResult res = OC_STACK_OK;
    res = OCSecure::deregisterInputPinCallback(inputPinCallbackHandle);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] deregisterInputPinCallback returns : %s",
            CommonUtil::getOCStackResult(res));

    if (res != expectedResult)
    {
        m_failureMessage = PMCppUtilityHelper::setFailureMessage(expectedResult, res);
        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] deregisterInputPinCallback OUT");
    return true;
}

bool PMCppHelper::setInputPinCallback(InputPinCallback inputPin, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] setInputPinCallback IN");

    OCStackResult res = OC_STACK_OK;
    res = OCSecure::setInputPinCallback(inputPin);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] setInputPinCallback returns : %s",
            CommonUtil::getOCStackResult(res));

    if (res != expectedResult)
    {
        m_failureMessage = PMCppUtilityHelper::setFailureMessage(expectedResult, res);
        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] setInputPinCallback OUT");
    return true;
}

bool PMCppHelper::unsetInputPinCallback(OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] unsetInputPinCallback IN");

    OCStackResult res = OC_STACK_OK;
    res = OCSecure::unsetInputPinCallback();
    IOTIVITYTEST_LOG(INFO, "[API Return Code] unsetInputPinCallback returns : %s",
            CommonUtil::getOCStackResult(res));

    if (res != expectedResult)
    {
        m_failureMessage = PMCppUtilityHelper::setFailureMessage(expectedResult, res);
        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] unsetInputPinCallback OUT");
    return true;
}

bool PMCppHelper::registerDisplayNumCallback(DisplayNumCB displayNumCB,
        OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] registerDisplayNumCallback IN");

    OCStackResult res = OC_STACK_OK;
    OC::DisplayNumContext *displayNumContext = new OC::DisplayNumContext(displayNumCB);
    res = OCSecure::registerDisplayNumCallback(displayNumContext);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] registerDisplayNumCallback returns : %s",
            CommonUtil::getOCStackResult(res));

    if (res != expectedResult)
    {
        m_failureMessage = PMCppUtilityHelper::setFailureMessage(expectedResult, res);
        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] registerDisplayNumCallback OUT");
    return true;
}

bool PMCppHelper::deregisterDisplayNumCallback(OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] deregisterDisplayNumCallback IN");

    OCStackResult res = OC_STACK_OK;
    res = OCSecure::deregisterDisplayNumCallback();
    IOTIVITYTEST_LOG(INFO, "[API Return Code] deregisterDisplayNumCallback returns : %s",
            CommonUtil::getOCStackResult(res));

    if (res != expectedResult)
    {
        m_failureMessage = PMCppUtilityHelper::setFailureMessage(expectedResult, res);
        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] deregisterDisplayNumCallback OUT");
    return true;
}

bool PMCppHelper::registerUserConfirmCallback(UserConfirmNumCB userConfirmCB,
        OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] registerUserConfirmCallback IN");

    OCStackResult res = OC_STACK_OK;
    OC::UserConfirmNumContext* userConfirmNumContext = new OC::UserConfirmNumContext(userConfirmCB);
    res = OCSecure::registerUserConfirmCallback(userConfirmNumContext);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] registerUserConfirmCallback returns : %s",
            CommonUtil::getOCStackResult(res));

    if (res != expectedResult)
    {
        m_failureMessage = PMCppUtilityHelper::setFailureMessage(expectedResult, res);
        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] registerUserConfirmCallback OUT");
    return true;
}

bool PMCppHelper::deregisterUserConfirmCallback(OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] deregisterUserConfirmCallback IN");

    OCStackResult res = OC_STACK_OK;
    res = OCSecure::deregisterUserConfirmCallback();
    IOTIVITYTEST_LOG(INFO, "[API Return Code] deregisterUserConfirmCallback returns : %s",
            CommonUtil::getOCStackResult(res));

    if (res != expectedResult)
    {
        m_failureMessage = PMCppUtilityHelper::setFailureMessage(expectedResult, res);
        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] deregisterUserConfirmCallback OUT");
    return true;
}

PMCppHelper::PMCppHelper()
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] Constructor Called");
}


bool filterDevicesByIp(std::string ip) {
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

std::string exec(const char* cmd) {
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

DeviceList_t filterDevices(DeviceList_t &deviceList)
{

    __FUNC_IN__

    if (deviceList.size())
    {
        std::vector< std::string > allowedDevIdVector(g_allowedDevId, end (g_allowedDevId));
        for (unsigned i = 0; i < deviceList.size(); i++)
        {
            bool isDeviceAllowed = false;

            /**
             *  Check if the Devices are from appropiate network
             *  If false do ownership transfer else skip the device
             */
            if (deviceList[i]->getDevAddr().find(ALLOWED_NETWORK) == string::npos)
            {
                IOTIVITYTEST_LOG(ERROR,
                        "[PMCppHelper] EndPoint Info Mismatch, Ignoring Device Id : %s  %s for Onboarding",
                        deviceList[i]->getDeviceID().c_str(), deviceList[i]->getDevAddr().c_str());
                deviceList.erase(deviceList.begin() + i);
                i--;
                continue;
            }

            for (unsigned j = 0; j < allowedDevIdVector.size(); j++)
            {
                if (allowedDevIdVector[j].compare(deviceList[i]->getDeviceID()) == 0&&filterDevicesByIp(deviceList[i]->getDevAddr()))
                {
                    isDeviceAllowed = true;
                    break;
                }
            }

            if (!isDeviceAllowed)
            {
                IOTIVITYTEST_LOG(ERROR,
                                        "[PMCppHelper] Current Device is Not Allowed\nId : %s\nIP : %s ",
                                        deviceList[i]->getDeviceID().c_str(), deviceList[i]->getDevAddr().c_str());
                deviceList.erase(deviceList.begin() + i);
                i--;
            }

        }
    }

    __FUNC_OUT__
    return deviceList;
}

bool PMCppHelper::discoverUnownedDevices(int time, DeviceList_t& deviceList,
        OCStackResult expectedResult)
{
    __FUNC_IN__

    OCStackResult res = OC_STACK_OK;
    res = OCSecure::discoverUnownedDevices(time, deviceList);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] discoverUnownedDevices returns : %s",
            CommonUtil::getOCStackResult(res));

    if (res != expectedResult)
    {
        m_failureMessage = PMCppUtilityHelper::setFailureMessage(expectedResult, res);
        return false;
    }

    if (deviceList.size())
    {
        IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] Found Unowned Deevices, count = %d",
                deviceList.size());
        PMCppUtilityHelper::printDevices(deviceList);

        deviceList = filterDevices(deviceList);
        IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] Found Filtered Unowned devices, count = %d",
                deviceList.size());

        PMCppUtilityHelper::printDevices(deviceList);
    }

    __FUNC_OUT__
    return true;
}

bool PMCppHelper::discoverSingleDevice(unsigned short timeout, const OicUuid_t* deviceID,
        std::shared_ptr< OCSecureResource > &foundDevice, OCStackResult expectedResult)
{
    __FUNC_IN__

    OCStackResult res = OC_STACK_OK;
    res = OCSecure::discoverSingleDevice(timeout, deviceID, foundDevice);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] discoverSingleDevice returns : %s",
            CommonUtil::getOCStackResult(res));

    if (res != expectedResult)
    {
        m_failureMessage = PMCppUtilityHelper::setFailureMessage(expectedResult, res);
        return false;
    }

    if (foundDevice != NULL)
    {
        std::cout << "[PMCppHelper] Found secure devices: " << foundDevice->getDeviceID() << endl;
    }

    __FUNC_OUT__
    return true;
}

bool PMCppHelper::discoverSingleDeviceInUnicast(unsigned short timeout, const OicUuid_t* deviceID,
        const std::string& hostAddress, OCConnectivityType connType,
        std::shared_ptr< OCSecureResource > &foundDevice, OCStackResult expectedResult)
{
    __FUNC_IN__

    OCStackResult res = OC_STACK_OK;
    res = OCSecure::discoverSingleDeviceInUnicast(timeout, deviceID, hostAddress, connType,
            foundDevice);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] discoverSingleDeviceInUnicast returns : %s",
            CommonUtil::getOCStackResult(res));

    if (res != expectedResult)
    {
        m_failureMessage = PMCppUtilityHelper::setFailureMessage(expectedResult, res);
        return false;
    }

    if (foundDevice != NULL)
    {
        std::cout << "[PMCppHelper] Found secure devices: " << foundDevice->getDeviceID() << endl;
    }

    __FUNC_OUT__
    return true;
}

bool PMCppHelper::discoverOwnedDevices(int time, DeviceList_t& deviceList,
        OCStackResult expectedResult)
{
    __FUNC_IN__

    OCStackResult res = OC_STACK_OK;
    res = OCSecure::discoverOwnedDevices(time, deviceList);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] discoverOwnedDevices returns : %s",
            CommonUtil::getOCStackResult(res));

    if (res != expectedResult)
    {
        m_failureMessage = PMCppUtilityHelper::setFailureMessage(expectedResult, res);
        return false;
    }

    if (deviceList.size())
    {
        IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] Found Owned DEevices, count = %d",
                deviceList.size());
        PMCppUtilityHelper::printDevices(deviceList);

        deviceList = filterDevices(deviceList);
        IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] Found Filtered Owned devices, count = %d",
                deviceList.size());
        PMCppUtilityHelper::printDevices(deviceList);
    }

    __FUNC_OUT__
    return true;
}

bool PMCppHelper::getDevInfoFromNetwork(unsigned short time, DeviceList_t& ownedDevList,
        DeviceList_t &unownedDevList, OCStackResult expectedResult)
{
    __FUNC_IN__

    OCStackResult res = OC_STACK_OK;
    res = OCSecure::getDevInfoFromNetwork(time, ownedDevList, unownedDevList);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] getDevInfoFromNetwork returns : %s",
            CommonUtil::getOCStackResult(res));

    if (res != expectedResult)
    {
        m_failureMessage = PMCppUtilityHelper::setFailureMessage(expectedResult, res);
        return false;
    }

    if (unownedDevList.size())
    {
        IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] Found secure devices, count = %d",
                unownedDevList.size());
        PMCppUtilityHelper::printDevices(unownedDevList);
    }

    if (ownedDevList.size())
    {
        IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] Found secure devices, count = %d",
                ownedDevList.size());
        PMCppUtilityHelper::printDevices(ownedDevList);
    }

    __FUNC_OUT__
    return true;
}

bool PMCppHelper::setOwnerTransferCallbackData(int num, OTMCallbackData_t &data,
        InputPinCallback inputPin, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] setOwnerTransferCallbackData IN");
#ifdef __1_2_1__
    OCStackResult res = OC_STACK_OK;

    res = OCSecure::setOwnerTransferCallbackData((OicSecOxm_t) num, &data, inputPin);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] setOwnerTransferCallbackData returns : %s",
            CommonUtil::getOCStackResult(res));

    if (res != expectedResult)
    {
        m_failureMessage = PMCppUtilityHelper::setFailureMessage(expectedResult, res);
        return false;
    }
#endif

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] setOwnerTransferCallbackData OUT");
    return true;
}

bool PMCppHelper::doOwnershipTransfer(DeviceList_t &data, ResultCallBack resultCallback,
        OCStackResult expectedResult)
{
    __FUNC_IN__
    OCStackResult res = OC_STACK_OK;
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] Device to be owned in : %d ", data.size());

    for (size_t i = 0; i < data.size(); i++)
    {
        PMCppCallbackHelper::s_isCBInvoked = CALLBACK_NOT_INVOKED;
        res = data[i]->doOwnershipTransfer(resultCallback);

        IOTIVITYTEST_LOG(INFO, "[API Return Code] doOwnershipTransfer returns : %s",
                CommonUtil::getOCStackResult(res));

        if (res != expectedResult)
        {
            m_failureMessage = PMCppUtilityHelper::setFailureMessage(expectedResult, res);
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

    __FUNC_OUT__
    return true;
}

bool PMCppHelper::provisionACL(DeviceList_t& deviceList, const OicSecAcl_t* acl,
        ResultCallBack resultCallback, OCStackResult expectedResult)
{
    __FUNC_IN__
    OCStackResult res = OC_STACK_OK;
    PMCppCallbackHelper::s_isCBInvoked = CALLBACK_NOT_INVOKED;

    res = deviceList[0]->provisionACL(acl, resultCallback);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] provisionACL returns : %s",
            CommonUtil::getOCStackResult(res));

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

    __FUNC_OUT__
    return true;
}

bool PMCppHelper::provisionCredentials(DeviceList_t& deviceList, const Credential &cred,
        const OCSecureResource &device2, ResultCallBack resultCallback,
        OCStackResult expectedResult)
{
    __FUNC_IN__
    OCStackResult res = OC_STACK_OK;
    PMCppCallbackHelper::s_isCBInvoked = CALLBACK_NOT_INVOKED;

    res = deviceList[0]->provisionCredentials(cred, device2, resultCallback);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] provisionCredentials returns : %s",
            CommonUtil::getOCStackResult(res));

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

    __FUNC_OUT__
    return true;
}

bool PMCppHelper::provisionPairwiseDevices(DeviceList_t& deviceList, const Credential &cred,
        const OicSecAcl_t* acl1, const OCSecureResource &device2, const OicSecAcl_t* acl2,
        ResultCallBack resultCallback, OCStackResult expectedResult)
{
    __FUNC_IN__
    OCStackResult res = OC_STACK_OK;
    PMCppCallbackHelper::s_isCBInvoked = CALLBACK_NOT_INVOKED;

    res = deviceList[0]->provisionPairwiseDevices(cred, acl1, device2, acl2, resultCallback);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] provisionPairwiseDevices returns : %s",
            CommonUtil::getOCStackResult(res));

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

    __FUNC_OUT__
    return true;
}

bool PMCppHelper::getLinkedDevices(DeviceList_t& deviceList, UuidList_t &uuidList,
        OCStackResult expectedResult)
{
    __FUNC_IN__
    OCStackResult res = OC_STACK_OK;
    PMCppCallbackHelper::s_isCBInvoked = CALLBACK_NOT_INVOKED;

    res = deviceList[0]->getLinkedDevices(uuidList);

    IOTIVITYTEST_LOG(INFO, "[API Return Code] getLinkedDevices returns : %s",
            CommonUtil::getOCStackResult(res));

    if (res != expectedResult)
    {
        m_failureMessage = PMCppUtilityHelper::setFailureMessage(expectedResult, res);
        return false;
    }

    __FUNC_OUT__
    return true;
}

bool PMCppHelper::getOTMethod(DeviceList_t& deviceList, OicSecOxm_t* oxm,
        OCStackResult expectedResult)
{
    __FUNC_IN__

    OCStackResult res = deviceList[0]->getOTMethod(oxm);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] getOTMethod returns : %s",
            CommonUtil::getOCStackResult(res));

    if (res != expectedResult)
    {
        m_failureMessage = PMCppUtilityHelper::setFailureMessage(expectedResult, res);
        return false;
    }
    else if (res == OC_STACK_OK)
    {
        IOTIVITYTEST_LOG(INFO, "[API Return Code] Returned OXM Method : %s",
                PMCppUtilityHelper::getOxmType(*oxm));
    }

    __FUNC_OUT__
    return true;
}

bool PMCppHelper::getSelectedOwnershipTransferMethod(DeviceList_t& deviceList,
        OicSecOxm_t expectedResult)
{
    __FUNC_IN__

    OicSecOxm_t oxm = deviceList[0]->getSelectedOwnershipTransferMethod();
    IOTIVITYTEST_LOG(INFO, "[API Return Code] getSelectedOwnershipTransferMethod returns : %s",
            PMCppUtilityHelper::getOxmType(oxm));

    if (oxm != expectedResult)
    {
        m_failureMessage = PMCppUtilityHelper::setFailureMessage(expectedResult, oxm);
        return false;
    }

    __FUNC_OUT__
    return true;
}

bool PMCppHelper::unlinkDevices(DeviceList_t& deviceList, const OCSecureResource &device2,
        ResultCallBack resultCallback, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] unlinkDevices IN");
    OCStackResult res = OC_STACK_OK;
    PMCppCallbackHelper::s_isCBInvoked = CALLBACK_NOT_INVOKED;

    res = deviceList[0]->unlinkDevices(device2, resultCallback);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] unlinkDevices returns : %s",
            CommonUtil::getOCStackResult(res));

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
    PMCppCallbackHelper::s_isCBInvoked = CALLBACK_NOT_INVOKED;

    res = deviceList[0]->removeDevice(waitTimeForOwnedDeviceDiscovery, resultCallback);
    IOTIVITYTEST_LOG(DEBUG, "[API Return Code] removeDevice returns : %s",
            CommonUtil::getOCStackResult(res));

    if (res != expectedResult)
    {
        m_failureMessage = PMCppUtilityHelper::setFailureMessage(expectedResult, res);
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

bool PMCppHelper::removeDeviceWithUuid(unsigned short waitTimeForOwnedDeviceDiscovery,
        std::string uuid, ResultCallBack resultCallback, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] removeDeviceWithUuid IN");
    OCStackResult res = OC_STACK_OK;
    PMCppCallbackHelper::s_isCBInvoked = CALLBACK_NOT_INVOKED;

    res = OCSecure::removeDeviceWithUuid(waitTimeForOwnedDeviceDiscovery, uuid, resultCallback);
    IOTIVITYTEST_LOG(DEBUG, "[API Return Code] removeDeviceWithUuid returns : %s",
            CommonUtil::getOCStackResult(res));

    if (res != expectedResult)
    {
        m_failureMessage = PMCppUtilityHelper::setFailureMessage(expectedResult, res);
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

bool PMCppHelper::saveACL(const OicSecAcl_t* acl, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] saveACL IN");
    OCStackResult res = OC_STACK_OK;
    PMCppCallbackHelper::s_isCBInvoked = CALLBACK_NOT_INVOKED;

    res = OCSecure::saveACL(acl);
    IOTIVITYTEST_LOG(DEBUG, "[API Return Code] saveACL returns : %s",
            CommonUtil::getOCStackResult(res));

    if (res != expectedResult)
    {
        IOTIVITYTEST_LOG(ERROR, "Expected Result Mismatch");
        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] saveACL OUT");
    return true;
}

bool PMCppHelper::provisionTrustCertChain(DeviceList_t& deviceList, OicSecCredType_t type,
        uint16_t credId, ResultCallBack resultCallback, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] provisionTrustCertChain IN");
    OCStackResult res = OC_STACK_OK;
    PMCppCallbackHelper::s_isCBInvoked = CALLBACK_NOT_INVOKED;

    res = deviceList[0]->provisionTrustCertChain(type, credId, resultCallback);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] provisionTrustCertChain returns : %s",
            CommonUtil::getOCStackResult(res));

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

bool PMCppHelper::deleteACLList(OicSecAcl_t* pAcl)
{
    __FUNC_IN__

    OCDeleteACLList(pAcl);

    __FUNC_OUT__
    return true;
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

        if (CALLBACK_INVOKED == PMCppCallbackHelper::s_isCBInvoked)
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

/**
 * Function for Convert String to Device Uuid
 */
bool PMCppHelper::convertStrToUuid(std::string uuid, OicUuid_t* deviceID,
        OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] convertStrToUuid IN");

    OCStackResult rst;

    rst = ConvertStrToUuid(uuid.c_str(), deviceID);
    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] convertStrToUuid returns : %s",
            CommonUtil::getOCStackResult(rst));

    if (OC_STACK_OK != rst)
    {
        m_failureMessage = PMCppUtilityHelper::setFailureMessage(expectedResult, rst);
        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] convertStrToUuid OUT");

    return true;
}
