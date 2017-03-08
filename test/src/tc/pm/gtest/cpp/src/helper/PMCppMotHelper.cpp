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

#if defined(__MOT__)
#include "PMCppMotHelper.h"
#include "PMCppUtilityHelper.h"

OCPersistentStorage PMCppMotHelper::s_pstMot =
{   0,0,0,0,0};
static int g_cbInvoked = CALLBACK_NOT_INVOKED;

FILE* PMCppMotHelper::fopenMotClient(const char *UNUSED_PARAM, const char *mode)
{
    (void) UNUSED_PARAM;
    return fopen(MOT_CBOR_FILE, mode);
}

PMCppMotHelper::PMCppMotHelper()
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppMotHelper] Constructor Called");
}

bool PMCppMotHelper::initMotClient()
{
    __FUNC_IN__

    PMCppMotHelper::s_pstMot.open = PMCppMotHelper::fopenMotClient;
    PMCppMotHelper::s_pstMot.read = fread;
    PMCppMotHelper::s_pstMot.write = fwrite;
    PMCppMotHelper::s_pstMot.close = fclose;
    PMCppMotHelper::s_pstMot.unlink = unlink;

    PlatformConfig cfg
    {   OC::ServiceType::InProc, OC::ModeType::Both, "0.0.0.0", 0, OC::QualityOfService::LowQos,
        &s_pstMot};

    OCPlatform::Configure(cfg);

    if (OCSecure::provisionInit(MOT_PRVN_DB_FILE_NAME) != OC_STACK_OK)
    {
        std::cout << "PM Init failed" << std::endl;
        return false;
    }

    __FUNC_OUT__
    return true;
}

bool PMCppMotHelper::discoverMultipleOwnerEnabledDevices(unsigned short timeout, DeviceList_t &list,
        OCStackResult expectedResult)
{
    __FUNC_IN__

    OCStackResult res = OCSecure::discoverMultipleOwnerEnabledDevices(timeout, list);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] discoverMultipleOwnerEnabledDevices returns : %s",
            CommonUtil::getOCStackResult(res));

    if (res != expectedResult)
    {
        m_failureMessage = "Failed discoverMultipleOwnedDevices.";
        return false;
    }

    if (list.size())
    {
        IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] Found secure devices, count = %d",
                list.size());
        PMCppUtilityHelper::printDevices(list);
    }

    __FUNC_OUT__
    return true;
}
bool PMCppMotHelper::discoverMultipleOwnedDevices(unsigned short timeout, DeviceList_t &list,
        OCStackResult expectedResult)
{
    __FUNC_IN__

    OCStackResult res = OCSecure::discoverMultipleOwnedDevices(timeout, list);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] discoverMultipleOwnedDevices returns : %s",
            CommonUtil::getOCStackResult(res));

    if (res != expectedResult)
    {
        m_failureMessage = "Failed discoverMultipleOwnedDevices.";
        return false;
    }

    if (list.size())
    {
        IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] Found secure devices, count = %d",
                list.size());
        PMCppUtilityHelper::printDevices(list);

    }

    __FUNC_OUT__
    return true;
}
bool PMCppMotHelper::discoverMultipleOwnerEnabledDevice(unsigned short timeout,
        const OicUuid_t* deviceID, std::shared_ptr< OCSecureResource > &foundDevice,
        OCStackResult expectedResult)
{
    __FUNC_IN__

    OCStackResult res = OCSecure::discoverMultipleOwnerEnabledDevice(timeout, deviceID,
            foundDevice);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] discoverMultipleOwnerEnabledDevice returns : %s",
            CommonUtil::getOCStackResult(res));

    if (res != expectedResult)
    {
        m_failureMessage = "Failed discoverMultipleOwnerEnabledDevice.";
        return false;
    }

    if (foundDevice!=NULL)
    {
        std::cout << "[PMCppMotHelper] Found secure devices: " << foundDevice->getDeviceID() << endl;
    }

    __FUNC_OUT__
    return true;
}
bool PMCppMotHelper::addPreconfigPIN(DeviceList_t &data, const char* preconfPIN,
        size_t preconfPINLength, OCStackResult expectedResult)
{
    __FUNC_IN__

    OCStackResult res = data[0]->addPreconfigPIN(preconfPIN, preconfPINLength);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] addPreconfigPIN returns : %s",
            CommonUtil::getOCStackResult(res));

    if (res != expectedResult)
    {
        m_failureMessage = "Failed addPreconfigPIN.";
        return false;
    }

    __FUNC_OUT__
    return true;
}

bool PMCppMotHelper::doMultipleOwnershipTransfer(DeviceList_t &data, ResultCallBack resultCallback,
        OCStackResult expectedResult)
{
    __FUNC_IN__

    g_cbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult res = data[0]->doMultipleOwnershipTransfer(resultCallback);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] doMultipleOwnershipTransfer returns : %s",
            CommonUtil::getOCStackResult(res));

    if (res != expectedResult)
    {
        m_failureMessage = "Failed provisionPreconfPin.";
        return false;
    }

    if (OC_STACK_OK == res)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            m_failureMessage = "doMultipleOwnershipTransfer Callback Not Invoked";
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

bool PMCppMotHelper::provisionPreconfPin(DeviceList_t &data, const char *preconfPin,
        size_t preconfPinLength, ResultCallBack resultCallback, OCStackResult expectedResult)
{
    __FUNC_IN__

    g_cbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult res = data[0]->provisionPreconfPin(preconfPin, preconfPinLength, resultCallback);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] provisionPreconfPin returns : %s",
            CommonUtil::getOCStackResult(res));

    if (res != expectedResult)
    {
        m_failureMessage = "Failed provisionPreconfPin.";
        return false;
    }

    if (OC_STACK_OK == res)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            m_failureMessage = "provisionPreconfPin Callback Not Invoked";
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}
bool PMCppMotHelper::changeMOTMode(DeviceList_t &data, const OicSecMomType_t momType,
        ResultCallBack resultCallback, OCStackResult expectedResult)
{
    __FUNC_IN__

    g_cbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult res = data[0]->changeMOTMode(momType, resultCallback);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] changeMOTMode returns : %s",
            CommonUtil::getOCStackResult(res));

    if (res != expectedResult)
    {
        m_failureMessage = "Failed changeMOTMode.";
        return false;
    }

    if (OC_STACK_OK == res)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            m_failureMessage = "changeMOTMode Callback Not Invoked";
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

bool PMCppMotHelper::selectMOTMethod(DeviceList_t &data, const OicSecOxm_t oxmSelVal,
        ResultCallBack resultCallback, OCStackResult expectedResult)
{
    __FUNC_IN__

    g_cbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult res = data[0]->selectMOTMethod(oxmSelVal, resultCallback);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] selectMOTMethod returns : %s",
            CommonUtil::getOCStackResult(res));

    if (res != expectedResult)
    {
        m_failureMessage = "Failed selectMOTMethod.";
        return false;
    }

    if (OC_STACK_OK == res)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            m_failureMessage = "selectMOTMethod Callback Not Invoked";
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

bool PMCppMotHelper::isSubownerOfDevice(DeviceList_t &data, bool* subowner,
        OCStackResult expectedResult, bool expectedVal)
{
    __FUNC_IN__

    g_cbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult res = data[0]->isSubownerOfDevice(subowner);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] isSubownerOfDevice returns : %s",
            CommonUtil::getOCStackResult(res));

    if(res == OC_STACK_OK)
    {
        IOTIVITYTEST_LOG(INFO, "If the Caller is Subowner : %s", (*subowner)? "true" : "false");
    }

    if (res != expectedResult || expectedVal != *subowner)
    {
        m_failureMessage = "Failed selectMOTMethod.";
        return false;
    }

    __FUNC_OUT__
    return true;

}
bool PMCppMotHelper::getMOTMethod(DeviceList_t &data, OicSecOxm_t* oxm,
        OCStackResult expectedResult)
{
    __FUNC_IN__

    g_cbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult res = data[0]->getMOTMethod(oxm);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] getMOTMethod returns : %s",
            CommonUtil::getOCStackResult(res));

    if (res != expectedResult)
    {
        m_failureMessage = "Failed selectMOTMethod.";
        return false;
    }
    else if(res == OC_STACK_OK)
    {
        IOTIVITYTEST_LOG(INFO, "[API Return Code] Returned OXM Method : %s", PMCppUtilityHelper::getOxmType(*oxm));
    }

    __FUNC_OUT__
    return true;
}
bool PMCppMotHelper::isMOTSupported(DeviceList_t &data, bool expectedVal)
{
    __FUNC_IN__

    g_cbInvoked = CALLBACK_NOT_INVOKED;

    bool retVal = data[0]->isMOTSupported();
    IOTIVITYTEST_LOG(INFO, "[API Return Code] isMOTSupported returns : %d", retVal);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] MOT Supoorted : %d", retVal);

    if (retVal != expectedVal)
    {
        m_failureMessage = "Failed selectMOTMethod.";
        return false;
    }

    __FUNC_OUT__
    return true;
}

bool PMCppMotHelper::isMOTEnabled(DeviceList_t &data, bool expectedVal)
{
    __FUNC_IN__

    g_cbInvoked = CALLBACK_NOT_INVOKED;

    bool retVal = data[0]->isMOTEnabled();
    IOTIVITYTEST_LOG(INFO, "[API Return Code] isMOTEnabled returns : %d", retVal);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] MOT Enabled : %d", retVal);

    if (retVal != expectedVal)
    {
        m_failureMessage = "Failed selectMOTMethod.";
        return false;
    }

    __FUNC_OUT__
    return true;
}

void PMCppMotHelper::multipleOwnershipTransferCB(PMResultList_t *result, int hasError)
{
    __FUNC_IN__

    if (hasError)
    {
        IOTIVITYTEST_LOG(ERROR, "[PMCppMotHelper] Provisioning ERROR %d %d!!!", hasError,result->at(0).res);
    }
    else
    {
        IOTIVITYTEST_LOG(DEBUG, "[PMCppMotHelper] Received provisioning results: ");

        for (unsigned int i = 0; i < result->size(); i++)
        {
            std::cout << "Result is = " << result->at(i).res << " for device ";
            PMCppUtilityHelper::printUuid(result->at(i).deviceId);
        }
        g_cbInvoked = CALLBACK_INVOKED;
        delete result;
    }
    __FUNC_OUT__
}

void PMCppMotHelper::provisionPreconfPinCB(PMResultList_t *result, int hasError)
{
    __FUNC_IN__

    if (hasError)
    {
        IOTIVITYTEST_LOG(ERROR, "[PMCppMotHelper] Provisioning ERROR %d!!!", hasError);
    }
    else
    {
        IOTIVITYTEST_LOG(DEBUG, "[PMCppMotHelper] Received provisioning results: ");

        for (unsigned int i = 0; i < result->size(); i++)
        {
            std::cout << "Result is = " << result->at(i).res << " for device ";
            PMCppUtilityHelper::printUuid(result->at(i).deviceId);
        }
        g_cbInvoked = CALLBACK_INVOKED;
        delete result;
    }
    __FUNC_OUT__
}

void PMCppMotHelper::changeMOTModeCB(PMResultList_t *result, int hasError)
{
    __FUNC_IN__

    if (hasError)
    {
        IOTIVITYTEST_LOG(ERROR, "[PMCppMotHelper] Provisioning ERROR %d!!!", hasError);
    }
    else
    {
        IOTIVITYTEST_LOG(DEBUG, "[PMCppMotHelper] Received provisioning results: ");

        for (unsigned int i = 0; i < result->size(); i++)
        {
            std::cout << "Result is = " << result->at(i).res << " for device ";
            PMCppUtilityHelper::printUuid(result->at(i).deviceId);
        }
        g_cbInvoked = CALLBACK_INVOKED;
        delete result;
    }
    __FUNC_OUT__
}

void PMCppMotHelper::selectMOTMethodCB(PMResultList_t *result, int hasError)
{
    __FUNC_IN__

    if (hasError)
    {
        IOTIVITYTEST_LOG(ERROR, "[PMCppMotHelper] Provisioning ERROR %d!!!", hasError);
    }
    else
    {
        IOTIVITYTEST_LOG(DEBUG, "[PMCppMotHelper] Received provisioning results: ");

        for (unsigned int i = 0; i < result->size(); i++)
        {
            std::cout << "Result is = " << result->at(i).res << " for device ";
            PMCppUtilityHelper::printUuid(result->at(i).deviceId);
        }
        g_cbInvoked = CALLBACK_INVOKED;
        delete result;
    }
    __FUNC_OUT__
}

/**
 * Function for getting failure message
 */
std::string PMCppMotHelper::getFailureMessage()
{
    return m_failureMessage;
}

int PMCppMotHelper::waitCallbackRet()
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
#endif /*_MOT_*/
