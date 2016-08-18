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
#include "../include/PMCppDPAppHelper.h"

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
void findCallback(PairedDevices discoveredDevList)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] pairedDevListCB Invoked");
    if (0 == discoveredDevList.size())
    {
        std::cout << "No Direct-pairing Support device Found" << std::endl;
    }
    else
    {
        std::cout << "Discovered Direct-Pairing Support Device" << std::endl;
        s_discoveredDpDevList = discoveredDevList;
        printDevices(discoveredDevList);
    }

    g_dpCBInvoked = CALLBACK_INVOKED;
}

void resultCallback(std::shared_ptr< OCDirectPairing > ptr, OCStackResult res)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] resultCallback Invoked");

    if (g_expectedCbResult != res)
    {
        IOTIVITYTEST_LOG(ERROR, "[PMCsdkDpHelper] pairingReqCB returns = %s", getOCStackResultCPP(res).c_str());
        g_dpCBInvoked = CALLBACK_RES_WRONG;
        return;
    }

    if (OC_STACK_OK == res)
    {
        std::cout << " Direct-Pairing SUCCESS" << std::endl;
        std::cout << "Host Info:" << ptr->getHost() << std::endl;
    }
    else
    {
        std::cout << " resultCallback Direct-Pairing FAILED" << std::endl;
    }

    g_dpCBInvoked = CALLBACK_INVOKED;
}

void pairedDevListCB(PairedDevices pairedDevList)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] pairedDevListCB Invoked");
    if (0 == pairedDevList.size())
    {
        std::cout << "No Paired Device Found" << std::endl;
    }
    else
    {
        s_pairedDpDevList = pairedDevList;
        printDevices(pairedDevList);
    }

    g_dpCBInvoked = CALLBACK_INVOKED;
}

FILE* clientDpCppOpen(const char* /*fileName*/, const char *mode)
{
    return fopen(DIRECT_PAIRING_CLIENT_DAT, mode);
}


bool findDirectPairingDevices(unsigned short waittime,
        GetDirectPairedCallback callback, OCStackResult expectedApiResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] findDirectPairingDevices In");

    g_dpCBInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult res = OC::OCPlatform::findDirectPairingDevices(DP_DISCOVERY_TIMEOUT, callback);
    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] findDirectPairingDevices returns %s",
            getOCStackResultCPP(res).c_str());

    if (res != expectedApiResult)
    {
        IOTIVITYTEST_LOG(ERROR, "[PMCppDPHelper] Expected Error Code Mismatched");
        return false;
    }

    if (OC_STACK_OK == res)
    {
        int nCallbackResult = waitDpCallbackRet();
        if (CALLBACK_NOT_INVOKED == nCallbackResult || CALLBACK_RES_WRONG == nCallbackResult)
        {
            IOTIVITYTEST_LOG(ERROR, "[PMCsdkDpHelper] Callback Not Invoked properly");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] findDirectPairingDevices Out");
    return true;
}

bool doDirectPairing(int nPeerDevInstance, OCPrm_t pmSel,
        const std::string& pinNumber, DirectPairingCallback resultCallback,
        OCStackResult expectedApiResult, OCStackResult expectedCbResult = OC_STACK_OK)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] doDirectPairing In");

    g_dpCBInvoked = CALLBACK_NOT_INVOKED;
    g_expectedCbResult = expectedCbResult;
    OCStackResult res = OC::OCPlatform::doDirectPairing(s_discoveredDpDevList[nPeerDevInstance],
            pmSel, pinNumber, resultCallback);
    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] doDirectPairing returns %s",
            getOCStackResultCPP(res).c_str());

    if (res != expectedApiResult)
    {
        IOTIVITYTEST_LOG(ERROR, "[PMCppDPHelper] Expected Error Code Mismatched");
        return false;
    }

    if (OC_STACK_OK == res)
    {
        int nCallbackResult = waitDpCallbackRet();
        if (CALLBACK_NOT_INVOKED == nCallbackResult || CALLBACK_RES_WRONG == nCallbackResult)
        {
            IOTIVITYTEST_LOG(ERROR, "[PMCsdkDpHelper] Callback Not Invoked properly");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] doDirectPairing Out");
    return true;
}

bool getDirectPairedDevices(GetDirectPairedCallback callback,
        OCStackResult expectedApiResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] getDirectPairedDevices In");

    g_dpCBInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult res = OC::OCPlatform::getDirectPairedDevices(callback);
    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] getDirectPairedDevices returns %s",
            getOCStackResultCPP(res).c_str());

    if (res != expectedApiResult)
    {
        IOTIVITYTEST_LOG(ERROR, "[PMCppDPHelper] Expected Error Code Mismatched");
        return false;
    }

    if (OC_STACK_OK == res)
    {
        int nCallbackResult = waitDpCallbackRet();
        if (CALLBACK_NOT_INVOKED == nCallbackResult || CALLBACK_RES_WRONG == nCallbackResult)
        {
            IOTIVITYTEST_LOG(ERROR, "[PMCsdkDpHelper] Callback Not Invoked properly");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] getDirectPairedDevices Out");
    return true;
}

bool getHost(int nDevInstance)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] getHost In");
    std::string hostAddress = s_discoveredDpDevList[nDevInstance]->getHost();
    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] getHost returns Host Adress returns %s",
            hostAddress.c_str());

    if (hostAddress.length())
    {
        return true;
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] getHost Out");
    return false;
}

bool getDeviceID(int nDevInstance)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] getDeviceID In");
    std::string devID = s_discoveredDpDevList[nDevInstance]->getDeviceID();
    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] getDeviceID returns Host Adress returns %s",
            devID.c_str());

    if (devID.length())
    {
        return true;
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] getDeviceID out");
    return false;
}

bool getPairingMethods(int nDevInstance)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] getPairingMethods In");

    auto prms = s_discoveredDpDevList[nDevInstance]->getPairingMethods();

    if (prms.size())
    {
        for (int i = 0; i < prms.size(); i++)
        {
            std::cout << "[PMCppDPHelper] PAIRING METHOD TYPE : " << getPairingMethodsType(prms[i])
                    << std::endl;
        }
        return true;
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] getPairingMethods Out");
    return false;
}

std::string getPairingMethodsType(int connectionType)
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

bool getConnType(int nDevInstance)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] getConnType In");

    OCConnectivityType actualConnectivityType = s_discoveredDpDevList[nDevInstance]->getConnType();

    std::cout << "[PMCppDPHelper] ConnectivityType  : "
            << getConnectivityTypeName(actualConnectivityType) << std::endl;

    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] getConnType Out");
    return true;
}

std::string getConnectivityTypeName(OCConnectivityType connType)
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

bool getDev(int nDevInstance)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] getDev In");

    OCDPDev_t* ptrDevInstance = s_discoveredDpDevList[nDevInstance]->getDev();

    IOTIVITYTEST_LOG(DEBUG, "[PMCppDPHelper] getDev Out");
    return true;
}

int waitDpCallbackRet()
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
