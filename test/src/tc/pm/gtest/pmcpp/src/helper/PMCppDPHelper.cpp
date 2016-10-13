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

#if defined(__DIRECTPAIRING__)
#include "PMCppDPHelper.h"

int g_dpCBInvoked = CALLBACK_NOT_INVOKED;
OCStackResult g_expectedCbResult = OC_STACK_OK;
static PairedDevices s_discoveredDpDevList, s_pairedDpDevList;

static void printDevices(PairedDevices& list)
{
    for (size_t i = 0; i < list.size(); i++)
    {
        std::cout << "[" << i + 1 << "]" << " ID: " << list[i]->getDeviceID() << std::endl;
    }
}

/*
 * CallBack Functions
 */
void PMCppDPHelper::findCallback(PairedDevices discoveredDevList)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] pairedDevListCB Invoked");
    if (0 == discoveredDevList.size())
    {
        IOTIVITYTEST_LOG(ERROR, "No Direct-pairing Support device Found");
    }
    else
    {
        IOTIVITYTEST_LOG(DEBUG, "Discovered Direct-Pairing Support Device");
        s_discoveredDpDevList = discoveredDevList;
        printDevices(discoveredDevList);
    }

    g_dpCBInvoked = CALLBACK_INVOKED;
}

void PMCppDPHelper::resultCallback(std::shared_ptr< OCDirectPairing > ptr, OCStackResult res)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] resultCallback Invoked");

    if (g_expectedCbResult != res)
    {
        IOTIVITYTEST_LOG(ERROR, "[PMCsdkDpHelper] pairingReqCB returns = %s",
                getDpOCStackResult(res).c_str());
        g_dpCBInvoked = CALLBACK_RES_WRONG;
        return;
    }

    if (OC_STACK_OK == res)
    {
        IOTIVITYTEST_LOG(DEBUG, "Direct-Pairing SUCCESS :");
        std::cout << "Host Info:" << ptr->getHost() << std::endl;
    }
    else
    {
        IOTIVITYTEST_LOG(ERROR, "ResultCallback Direct-Pairing FAILED");
    }

    g_dpCBInvoked = CALLBACK_INVOKED;
}

void PMCppDPHelper::pairedDevListCB(PairedDevices pairedDevList)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] pairedDevListCB Invoked");
    if (0 == pairedDevList.size())
    {
        IOTIVITYTEST_LOG(ERROR, "No Paired device Found");
    }
    else
    {
        s_pairedDpDevList = pairedDevList;
        printDevices(pairedDevList);
    }

    g_dpCBInvoked = CALLBACK_INVOKED;
}

FILE* clientOpen(const char* /*fileName*/, const char *mode)
{
    return fopen(DIRECT_PAIRING_CLIENT_DAT, mode);
}

PMCppDPHelper::PMCppDPHelper()
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] Constructor Called");
}

bool PMCppDPHelper::findDirectPairingDevices(unsigned short waittime,
        GetDirectPairedCallback callback, OCStackResult expectedApiResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] findDirectPairingDevices In");

    g_dpCBInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult res = OC::OCPlatform::findDirectPairingDevices(DP_DISCOVERY_TIMEOUT, callback);
    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] findDirectPairingDevices returns %s",
            getDpOCStackResult(res).c_str());

    if (res != expectedApiResult)
    {
        m_failureMessage = setFailureMessage(expectedApiResult, res);
        return false;
    }

    if (OC_STACK_OK == res)
    {
        int nCallbackResult = waitCallbackRet();
        if (CALLBACK_NOT_INVOKED == nCallbackResult || CALLBACK_RES_WRONG == nCallbackResult)
        {
            IOTIVITYTEST_LOG(ERROR, "[PMCsdkDpHelper] Callback Not Invoked properly");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] findDirectPairingDevices Out");
    return true;
}

bool PMCppDPHelper::doDirectPairing(int nPeerDevInstance, OCPrm_t pmSel,
        const std::string& pinNumber, DirectPairingCallback resultCallback,
        OCStackResult expectedApiResult, OCStackResult expectedCbResult = OC_STACK_OK)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] doDirectPairing In");

    g_dpCBInvoked = CALLBACK_NOT_INVOKED;
    g_expectedCbResult = expectedCbResult;
    OCStackResult res = OC::OCPlatform::doDirectPairing(s_discoveredDpDevList[nPeerDevInstance],
            pmSel, pinNumber, resultCallback);
    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] doDirectPairing returns %s",
            getDpOCStackResult(res).c_str());

    if (res != expectedApiResult)
    {
        m_failureMessage = setFailureMessage(expectedApiResult, res);
        return false;
    }

    if (OC_STACK_OK == res)
    {
        int nCallbackResult = waitCallbackRet();
        if (CALLBACK_NOT_INVOKED == nCallbackResult || CALLBACK_RES_WRONG == nCallbackResult)
        {
            IOTIVITYTEST_LOG(ERROR, "[PMCsdkDpHelper] Callback Not Invoked properly");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] doDirectPairing Out");
    return true;
}

bool PMCppDPHelper::getDirectPairedDevices(GetDirectPairedCallback callback,
        OCStackResult expectedApiResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] getDirectPairedDevices In");

    g_dpCBInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult res = OC::OCPlatform::getDirectPairedDevices(callback);
    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] getDirectPairedDevices returns %s",
            getDpOCStackResult(res).c_str());

    if (res != expectedApiResult)
    {
        m_failureMessage = setFailureMessage(expectedApiResult, res);
        return false;
    }

    if (OC_STACK_OK == res)
    {
        int nCallbackResult = waitCallbackRet();
        if (CALLBACK_NOT_INVOKED == nCallbackResult || CALLBACK_RES_WRONG == nCallbackResult)
        {
            IOTIVITYTEST_LOG(ERROR, "[PMCsdkDpHelper] Callback Not Invoked properly");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] getDirectPairedDevices Out");
    return true;
}

bool PMCppDPHelper::getHost(int nDevInstance)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] getHost In");

    try
    {
        std::string hostAddress = s_discoveredDpDevList[nDevInstance]->getHost();
        IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] getHost returns Host Address returns %s",
                hostAddress.c_str());
    }
    catch (exception& e)
    {
        IOTIVITYTEST_LOG(DEBUG, "Exception occurred for  getHost: %s",
                std::string(e.what()).c_str());
        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] getHost Out");
    return true;
}

bool PMCppDPHelper::getDeviceID(int nDevInstance)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] getDeviceID In");

    try
    {
        std::string devID = s_discoveredDpDevList[nDevInstance]->getDeviceID();
        IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] getDeviceID returns Host Adress returns %s",
                devID.c_str());
    }
    catch (exception& e)
    {
        IOTIVITYTEST_LOG(DEBUG, "Exception occurred for getDeviceID: %s",
                std::string(e.what()).c_str());
        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] getDeviceID out");
    return true;
}

bool PMCppDPHelper::getPairingMethods(int nDevInstance)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] getPairingMethods In");

    try
    {
        auto prms = s_discoveredDpDevList[nDevInstance]->getPairingMethods();

        if (prms.size())
        {
            for (int i = 0; i < prms.size(); i++)
            {
                std::cout << "[PMCppDPHelper] PAIRING METHOD TYPE : "
                << getPairingMethodsType(prms[i]) << std::endl;
            }
        }
    }
    catch (exception& e)
    {
        IOTIVITYTEST_LOG(DEBUG, "Exception occurred for getPairingMethods: %s",
                std::string(e.what()).c_str());
        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] getPairingMethods Out");
    return true;
}

std::string PMCppDPHelper::getPairingMethodsType(int connectionType)
{
    std::string pairingMethodTypeName = EMPTY_STRING;
    switch (connectionType)
    {
        case DP_NOT_ALLOWED:
        pairingMethodTypeName = "DP_NOT_ALLOWED";
        break;
        case DP_PRE_CONFIGURED:
        pairingMethodTypeName = "DP_PRE_CONFIGURED";
        break;
        case DP_RANDOM_PIN:
        pairingMethodTypeName = "DP_RANDOM_PIN";
        break;
        default:
        pairingMethodTypeName = "DP_UNKNOWN_TYPE";
        break;
    }

    return pairingMethodTypeName;
}

bool PMCppDPHelper::getConnType(int nDevInstance)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] getConnType In");

    try
    {
        OCConnectivityType actualConnectivityType =
        s_discoveredDpDevList[nDevInstance]->getConnType();

        std::cout << "[PMCppDPHelper] ConnectivityType  : "
        << getConnectivityTypeName(actualConnectivityType) << std::endl;
    }
    catch (exception& e)
    {
        IOTIVITYTEST_LOG(DEBUG, "Exception occurred for getConnType: %s",
                std::string(e.what()).c_str());
        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] getConnType Out");
    return true;
}

std::string PMCppDPHelper::getConnectivityTypeName(OCConnectivityType connType)
{
    switch (connType & CT_MASK_ADAPTER)
    {
        case CT_ADAPTER_IP:
        return "IP";

        case CT_IP_USE_V4:
        return "IPv4";

        case CT_IP_USE_V6:
        return "IPv6";

        case CT_ADAPTER_GATT_BTLE:
        return "GATT";

        case CT_ADAPTER_RFCOMM_BTEDR:
        return "RFCOMM";

        default:
        return "Incorrect connectivity";
    }
}

bool PMCppDPHelper::getDev(int nDevInstance)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] getDev In");

    try
    {
        OCDPDev_t* ptrDevInstance = s_discoveredDpDevList[nDevInstance]->getDev();
    }
    catch (exception& e)
    {
        IOTIVITYTEST_LOG(DEBUG, "Exception occurred for getDev: %s", std::string(e.what()).c_str());
        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] getDev Out");
    return true;
}

int PMCppDPHelper::waitCallbackRet()
{
    IOTIVITYTEST_LOG(DEBUG, "waitDpCallbackRet IN");

    for (int i = 0; CALLBACK_TIMEOUT > i; ++i)
    {
        IOTIVITYTEST_LOG(DEBUG, "waitDpCallbackRet Loop = %d", i);

        if (CALLBACK_INVOKED == g_dpCBInvoked)
        {
            return CALLBACK_INVOKED;
        }

        if (CALLBACK_RES_WRONG == g_dpCBInvoked)
        {
            return CALLBACK_RES_WRONG;
        }

        CommonUtil::waitInSecond(DELAY_SHORT);
    }

    IOTIVITYTEST_LOG(DEBUG, "waitDpCallbackRet OUT");
    return CALLBACK_NOT_INVOKED;
}

std::string PMCppDPHelper::getDpOCStackResult(OCStackResult ocstackresult)
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
        case OC_STACK_CONTINUE:
        resultString = "OC_STACK_CONTINUE";
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
        case OC_STACK_AUTHENTICATION_FAILURE:
        return "OC_STACK_AUTHENTICATION_FAILURE";
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
std::string PMCppDPHelper::getFailureMessage()
{
    return m_failureMessage;
}

/**
 * Function to set failure message
 */
std::string PMCppDPHelper::setFailureMessage(OCStackResult expectedApiResult,
        OCStackResult actualResult)
{
    std::string errorMessage = "\033[1;31m[Error] Expected : \033[0m"
    + getDpOCStackResult(expectedApiResult) + " \033[1;31mActual : \033[0m"
    + getDpOCStackResult(actualResult);
    return errorMessage;
}

std::string PMCppDPHelper::setFailureMessage(std::string errorMessage)
{
    std::string retErrorMessage("\033[1;31m[Error] Expected : ");
    retErrorMessage.append(errorMessage);
    retErrorMessage.append("\033[0m");

    return retErrorMessage;
}

#endif /*#if defined(__DIRECTPAIRING__)*/
