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

#if defined(__MOT__)
#include "PMCppAppHelper.h"

int g_motDevCount = 0;
int g_motOwnedDevCount = 0;
int g_motCbInvoked = CALLBACK_NOT_INVOKED;

bool changeMOTMode(DeviceList_t& deviceList, const OicSecMomType_t momType, ResultCallBack resultCallback,
        OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] OCChangeMOTMode IN");

    g_motCbInvoked = false;

    OCStackResult res = deviceList[0]->changeMOTMode(momType, resultCallback);
    IOTIVITYTEST_LOG(INFO, "[PMHelper]  OCChangeMOTMode API returns: %s\n", getOCStackResultCPP(res).c_str());

    if (expectedResult != res)
    {
        IOTIVITYTEST_LOG(ERROR, "Expected Result Mismatch");
        return false;
    }

    if (OC_STACK_OK == res)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRetMot())
        {
            IOTIVITYTEST_LOG(ERROR, "Callback not Invoked");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] OCChangeMOTMode OUT");
    return true;
}

bool discoverMultipleOwnerEnabledDevices(unsigned short timeout, DeviceList_t &list,
        OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] discoverMultipleOwnerEnabledDevices IN");

    OCStackResult res = OCSecure::discoverMultipleOwnerEnabledDevices(timeout, list);
    IOTIVITYTEST_LOG(INFO, "[PMHelper] OCDiscoverMultipleOwnerEnabledDevices API returns: %s\n",
            getOCStackResultCPP(res).c_str());

    if (expectedResult != res)
    {
        IOTIVITYTEST_LOG(ERROR, "Expected Result Mismatch");
        return false;
    }

    IOTIVITYTEST_LOG(INFO, "[PMHelper] Discovered Multiple Owner Enabled Devices List :");

    if (list.size())
    {
        IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] Found secure devices, count = %d\n",
                list.size());
        printDevices(list);
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] discoverMultipleOwnerEnabledDevices OUT");
    return true;
}

bool addPreconfigPIN(DeviceList_t &list, const char * preconfPin,
        size_t preconfPinLength, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] addPreconfigPIN IN");

    g_motCbInvoked = false;

    OCStackResult res = list[0]->addPreconfigPIN(preconfPin, preconfPinLength);
    IOTIVITYTEST_LOG(INFO, "[PMHelper] addPreconfigPIN API returns: %s\n",
            getOCStackResultCPP(res).c_str());

    if (expectedResult != res)
    {
        IOTIVITYTEST_LOG(ERROR, "Expected Result Mismatch");
        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] addPreconfigPIN OUT");
    return true;
}

bool provisionPreconfPin(DeviceList_t &list,const char * preconfPin, size_t preconfPinLength, ResultCallBack resultCallback,
        OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] provisionPreconfPin IN");

    g_motCbInvoked = false;

    OCStackResult res = list[0]->provisionPreconfPin(preconfPin, preconfPinLength,resultCallback);
    IOTIVITYTEST_LOG(INFO, "[PMHelper] provisionPreconfPin API returns: %s\n",
            getOCStackResultCPP(res).c_str());

    if (expectedResult != res)
    {
        IOTIVITYTEST_LOG(ERROR, "Expected Result Mismatch");
        return false;
    }

    if (OC_STACK_OK == res)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRetMot())
        {
            IOTIVITYTEST_LOG(ERROR, "Callback not Invoked");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] provisionPreconfPin OUT");
    return true;
}

bool selectMOTMethod(DeviceList_t &list, const OicSecOxm_t oxmSelVal,
        ResultCallBack resultCallback, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] selectMOTMethod IN");

    g_motCbInvoked = false;

    OCStackResult res = list[0]->selectMOTMethod(oxmSelVal, resultCallback);
    IOTIVITYTEST_LOG(INFO, "[PMHelper] selectMOTMethod API returns: %s\n", getOCStackResultCPP(res).c_str());

    if (expectedResult != res)
    {
        IOTIVITYTEST_LOG(ERROR, "Expected Result Mismatch");
        return false;
    }

    if (OC_STACK_OK == res)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRetMot())
        {
            IOTIVITYTEST_LOG(ERROR, "Callback not Invoked");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] selectMOTMethod OUT");
    return true;
}

bool doMultipleOwnershipTransfer(DeviceList_t &list,ResultCallBack resultCallback,OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] doMultipleOwnershipTransfer IN");

    g_motCbInvoked = false;
    OCStackResult res = list[0]->doMultipleOwnershipTransfer(resultCallback);
    IOTIVITYTEST_LOG(INFO, "[PMHelper] doMultipleOwnershipTransfer API returns: %s\n",
            getOCStackResultCPP(res).c_str());

    if (expectedResult != res)
    {
        IOTIVITYTEST_LOG(ERROR, "Expected Result Mismatch");
        return false;
    }

    if (OC_STACK_OK == res)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRetMot())
        {
            IOTIVITYTEST_LOG(ERROR, "Callback not Invoked");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] doMultipleOwnershipTransfer OUT");
    return true;
}

bool discoverMultipleOwnedDevices(unsigned short timeout, DeviceList_t &list, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] discoverMultipleOwnedDevices IN");

    OCStackResult res = OCSecure::discoverMultipleOwnedDevices(timeout, list);
    IOTIVITYTEST_LOG(INFO, "[PMHelper] discoverMultipleOwnedDevices API returns: %s\n",
            getOCStackResultCPP(res).c_str());

    if (expectedResult != res)
    {
        IOTIVITYTEST_LOG(ERROR, "Expected Result Mismatch");
        return false;
    }

    IOTIVITYTEST_LOG(INFO, "[PMHelper] Discovered Multiple Owned Devices List :");

    if (list.size())
    {
        IOTIVITYTEST_LOG(DEBUG, "[PMCppHelper] Found secure devices, count = %d\n",
                list.size());
        printDevices(list);
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] discoverMultipleOwnedDevices OUT");
    return true;
}

bool getLedResource(DeviceList_t &list,GetCallback attributeHandler,OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] getLedResource IN");

    g_motCbInvoked = false;

    std::vector<std::string> ledTypes =
    {   "core.led"};
    std::vector<std::string> ifaces =
    {   "oic.if.baseline"};
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] getLedResource IN1");
    OCProvisionDev_t *selDev = list[0]->getDevPtr();
    std::stringstream host;
    host << "coaps:://";
    host << list[0]->getDevAddr();
    host << selDev->securePort;
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] getLedResource IN2");

    OCResource::Ptr led = OC::OCPlatform::constructResourceObject(
            host.str(), "/a/led", selDev->connType, false, ledTypes, ifaces);
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] getLedResource IN3");
    if(!led)
    {
        IOTIVITYTEST_LOG(ERROR, "[PMHelper] Error: Led Object construction returned null");
        return false;
    }
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] getLedResource IN4");
    OCStackResult res = led->get(QueryParamsMap(), attributeHandler);
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] getLedResource IN5");
    if (expectedResult != res)
    {
        IOTIVITYTEST_LOG(ERROR, "Expected Result Mismatch");
        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] getLedResource OUT");
    return true;
}

void changeMOTModeCB(PMResultList_t *result, int hasError)
{
    if (hasError)
    {
        std::cout << "Error!!! in callback"<<std::endl;
    }
    else
    {
        std::cout<< "callback successfull"<<std::endl;
        delete result;
    }

    g_motCbInvoked = true;
}

void selectMOTMethodCB(PMResultList_t *result, int hasError)
{
    if (hasError)
    {
        std::cout << "Error!!! in callback"<<std::endl;
    }
    else
    {
        std::cout<< "callback successfull"<<std::endl;
        delete result;
    }

    g_motCbInvoked = true;
}

void preConfigPinCB(PMResultList_t *result, int hasError)
{
    if (hasError)
    {
        std::cout << "Error!!! in callback"<<std::endl;
    }
    else
    {
        std::cout<< "callback successfull"<<std::endl;
        delete result;
    }

    g_motCbInvoked = true;
}

void multipleOwnershipTransferCB(PMResultList_t *result, int hasError)
{
    if (hasError)
    {
        std::cout << "Error!!! in MultipleOwnershipTransfer"<<std::endl;
    }
    else
    {
        std::cout<< "\nTransferred MultipleOwnership successfuly for device : ";
        printUuid(result->at(0).deviceId);
        delete result;
    }

    g_motCbInvoked = true;
}

void getCallback(const HeaderOptions& headerOptions, const OCRepresentation& rep, const int eCode)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] getCallback IN");
    (void)(headerOptions);
    try
    {
        if (OC_STACK_OK == eCode)
        {
            std::cout << "Callback Context for GET query recvd successfully" << std::endl;
            std::cout << "Resource URI: " << rep.getUri() << std::endl;

            bool state = false;
            int power = 0;
            rep.getValue("state", state);
            rep.getValue("power", power);

            std::cout << "\tstate: " << state << std::endl;
            std::cout << "\tpower: " << power << std::endl;
        }
        else
        {
            std::cout << "getCallback Response error: " << eCode << std::endl;
        }
    }
    catch(std::exception& e)
    {
        std::cout << "Exception: " << e.what() << " in onGet" << std::endl;
    }
    g_motCbInvoked = true;

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] getCallback OUT");
}

int waitCallbackRetMot(void)
{
    IOTIVITYTEST_LOG(DEBUG, "waitCallbackRetMot IN");

    for (int i = 0; CALLBACK_TIMEOUT > i; ++i)
    {
        IOTIVITYTEST_LOG(INFO, "waitCallbackRetMot Loop = %d", i);

        if (CALLBACK_INVOKED == g_motCbInvoked)
        {
            return CALLBACK_INVOKED;
        }

        sleep(DELAY_SHORT);

        if (OC_STACK_OK != OCProcess())
        {
            IOTIVITYTEST_LOG(ERROR, "OCStack Process Failed");
            return CALLBACK_NOT_INVOKED;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "waitCallbackRetMot OUT");
    return CALLBACK_NOT_INVOKED;
}

#endif
