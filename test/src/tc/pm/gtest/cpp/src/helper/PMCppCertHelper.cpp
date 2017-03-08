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

#include "PMCppCertHelper.h"
#include "PMCppUtilityHelper.h"

int PMCppCertHelper::s_cbInvoked = CALLBACK_NOT_INVOKED;
OCPersistentStorage PMCppCertHelper::s_ps =
{ 0, 0, 0, 0, 0 };

FILE* PMCppCertHelper::clientOpen(const char *UNUSED_PARAM, const char *mode)
{
    return fopen(CBOR_DB_PATH, mode);
}

bool PMCppCertHelper::provisionInit(const std::string& dbPath)
{
    PMCppCertHelper::s_ps.open = PMCppCertHelper::clientOpen;
    PMCppCertHelper::s_ps.read = fread;
    PMCppCertHelper::s_ps.write = fwrite;
    PMCppCertHelper::s_ps.close = fclose;
    PMCppCertHelper::s_ps.unlink = unlink;

    PlatformConfig cfg
    { OC::ServiceType::InProc, OC::ModeType::Both, "0.0.0.0", 0, OC::QualityOfService::LowQos,
            &PMCppCertHelper::s_ps };

    OCPlatform::Configure(cfg);

    if (OCSecure::provisionInit(dbPath) != OC_STACK_OK)
    {
        std::cout << "PM Init failed" << std::endl;
        return false;
    }

    std::cout << "setup succeeded" << std::endl;
    return true;
}

PMCppCertHelper::PMCppCertHelper()
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppCertHelper] Constructor Called");
}

void PMCppCertHelper::certChainCB(uint16_t credId, uint8_t *trustCertChain, size_t chainSize)
{

    __FUNC_IN__

    IOTIVITYTEST_LOG(INFO, "CRED ID : %d", credId);

    OIC_LOG_BUFFER(DEBUG, "Cert Cb", trustCertChain, chainSize);

    PMCppCertHelper::s_cbInvoked = CALLBACK_INVOKED;

    __FUNC_OUT__
}

/**
 * Callback function for provisioning.
 *
 * @param[in] result Result list
 * @param[in] hasError indicates if the result has error
 */
void PMCppCertHelper::provisionCB(PMResultList_t *result, int hasError)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppCertHelper] provisionCB IN");

    if (hasError)
    {
        IOTIVITYTEST_LOG(ERROR, "[PMCppCertHelper] Provisioning ERROR %d!!!", hasError);
    }
    else
    {
        IOTIVITYTEST_LOG(DEBUG, "[PMCppCertHelper] Received provisioning results: ");

        for (unsigned int i = 0; i < result->size(); i++)
        {
            std::cout << "Result is = " << result->at(i).res << " for device ";
            PMCppUtilityHelper::printUuid(result->at(i).deviceId);
        }
        PMCppCertHelper::s_cbInvoked = CALLBACK_INVOKED;
        delete result;
    }
    IOTIVITYTEST_LOG(DEBUG, "[PMCppCertHelper] provisionCB OUT");
}
/**
 * Function for getting failure message
 */
std::string PMCppCertHelper::getFailureMessage()
{
    return m_failureMessage;
}

int PMCppCertHelper::waitCallbackRet()
{
    IOTIVITYTEST_LOG(DEBUG, "waitCallbackRet IN");

    for (int i = 0; CALLBACK_TIMEOUT > i; ++i)
    {
        IOTIVITYTEST_LOG(DEBUG, "waitCallbackRet Loop = %d", i);

        if (CALLBACK_INVOKED == PMCppCertHelper::s_cbInvoked)
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

bool PMCppCertHelper::saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize,
        OicEncodingType_t encodingType, uint16_t *credId, OCStackResult expectedResult)
{
    __FUNC_IN__

    OCStackResult result = OCSecure::saveTrustCertChain(trustCertChain, chainSize, encodingType,
            credId);
    IOTIVITYTEST_LOG(INFO, "[PM CERT] OCSaveTrustCertChain returns %s",
            CommonUtil::getOCStackResult(result));

    IOTIVITYTEST_LOG(INFO, "CredId of Saved Trust Cert. Chain into Cred of SVR : %d", *credId);

    if (expectedResult != result)
    {
        m_failureMessage = PMCppUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    __FUNC_OUT__
    return true;
}

bool PMCppCertHelper::readTrustCertChain(uint16_t credId, uint8_t **trustCertChain,
        size_t *chainSize, OCStackResult expectedResult)
{
    __FUNC_IN__

    OCStackResult result = OCSecure::readTrustCertChain(credId, trustCertChain, chainSize);
    IOTIVITYTEST_LOG(INFO, "[PM Cert] readTrustCertChain returns %s",
            CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = PMCppUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    __FUNC_OUT__
    return true;
}

bool PMCppCertHelper::registerTrustCertChangeNotifier(CertChainCallBack Callback,
        OCStackResult expectedResult)
{
    __FUNC_IN__

    PMCppCertHelper::s_cbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = OCSecure::registerTrustCertChangeNotifier(Callback);
    IOTIVITYTEST_LOG(INFO, "[PM CERT] registerTrustCertChangeNotifier returns %s",
            CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = PMCppUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    __FUNC_OUT__
    return true;
}

bool PMCppCertHelper::removeTrustCertChangeNotifier()
{
    __FUNC_IN__

    OCStackResult result = OCSecure::removeTrustCertChangeNotifier();
    IOTIVITYTEST_LOG(INFO, "[PM CERT] registerTrustCertChangeNotifier returns %s",
            CommonUtil::getOCStackResult(result));

    __FUNC_OUT__
    return true;
}

bool PMCppCertHelper::provisionTrustCertChain(DeviceList_t& deviceList, OicSecCredType_t type,
        uint16_t credId, ResultCallBack resultCallback, OCStackResult expectedResult)
{
    __FUNC_IN__

    PMCppCertHelper::s_cbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = deviceList[0]->provisionTrustCertChain(type, credId, resultCallback);
    IOTIVITYTEST_LOG(INFO, "[API Return Code] provisionTrustCertChain returns : %s",
            CommonUtil::getOCStackResult(result));

    if (result != expectedResult)
    {
        m_failureMessage = PMCppUtilityHelper::setFailureMessage(expectedResult, result);
        return false;
    }

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            m_failureMessage = "provisionTrustCertChain Callback Not Invoked";
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}
