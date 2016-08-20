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

#include "RIHelper.h"

RIHelper* RIHelper::s_riHelperInstance = NULL;
std::mutex RIHelper::s_mutex;

RIHelper::RIHelper()
{
    m_ResourceHandle = NULL;
}

RIHelper::~RIHelper()
{
    s_riHelperInstance = NULL;
}

RIHelper* RIHelper::getInstance(void)
{
    if (s_riHelperInstance == NULL)
    {
        s_mutex.lock();

        s_riHelperInstance = new RIHelper();

        s_mutex.unlock();
    }

    return s_riHelperInstance;
}

void RIHelper::configClientServerPlatform(void)
{
    PlatformConfig config
    { OC::ServiceType::InProc, OC::ModeType::Both, CT_DEFAULT, CT_DEFAULT,
            OC::QualityOfService::HighQos };

    OCPlatform::Configure(config);
}

void RIHelper::setTemperatureRep()
{
    m_temperatureRep.setValue(KEY_TEMPERATURE, m_temp);
    m_temperatureRep.setValue(KEY_HOUR, m_hour);
}

OCEntityHandlerResult RIHelper::entityHandler(shared_ptr< OCResourceRequest > request)
{
    IOTIVITYTEST_LOG(INFO, "In Server CPP entity handler:");
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    if (request)
    {
        IOTIVITYTEST_LOG(INFO, "\tValid Request");
        std::string requestType = request->getRequestType();
        RequestHandlerFlag requestFlag = (RequestHandlerFlag) request->getRequestHandlerFlag();

        auto pResponse = std::make_shared< OC::OCResourceResponse >();
        pResponse->setRequestHandle(request->getRequestHandle());
        pResponse->setResourceHandle(request->getResourceHandle());

        if (requestFlag == RequestHandlerFlag::RequestFlag)
        {
            IOTIVITYTEST_LOG(INFO, "\t\trequestFlag : Request");

            if (requestType == "GET")
            {
                IOTIVITYTEST_LOG(INFO, "\t\t\trequestType : GET");
                pResponse->setErrorCode(200);
                pResponse->setResponseResult(OC_EH_OK);
                setTemperatureRep();
                pResponse->setResourceRepresentation(m_temperatureRep);
                if (OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
                else
                {
                    isServerOk = false;
                    m_failureMsg = m_failureMsg + "Server: Get Request Response failed. ";
                    ehResult = OC_EH_ERROR;
                }
            }
            if (requestType == "PUT")
            {
                IOTIVITYTEST_LOG(INFO, "\t\t\trequestType : PUT");
                pResponse->setErrorCode(200);
                OCRepresentation rep = request->getResourceRepresentation();

                if (!rep.getValue(KEY_TEMPERATURE, m_temp))
                {
                    isServerOk = false;
                    m_failureMsg = m_failureMsg
                            + "Server: Representation in Put does not have temperature attribute. ";
                }
                if (!rep.getValue(KEY_HOUR, m_hour))
                {
                    isServerOk = false;
                    m_failureMsg = m_failureMsg
                            + "Server: Representation in Put does not have hour attribute. ";
                }

                if (isServerOk)
                {
                    setTemperatureRep();
                    pResponse->setResponseResult(OC_EH_OK);
                    pResponse->setResourceRepresentation(m_temperatureRep);
                }
                else
                {
                    pResponse->setResponseResult(OC_EH_ERROR);
                }

                if (OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
                else
                {
                    isServerOk = false;
                    m_failureMsg = m_failureMsg + "Server: Put Request Response failed. ";
                    ehResult = OC_EH_ERROR;
                }
            }
            if (requestType == "POST")
            {
                IOTIVITYTEST_LOG(INFO, "\t\t\trequestType : POST");
                pResponse->setErrorCode(200);
                OCRepresentation rep = request->getResourceRepresentation();
                if (!rep.getValue(KEY_TEMPERATURE, m_temp))
                {
                    isServerOk = false;
                    m_failureMsg =
                            m_failureMsg
                                    + "Server: Representation in Post does not have temperature attribute. ";
                }
                if (!rep.getValue(KEY_HOUR, m_hour))
                {
                    isServerOk = false;
                    m_failureMsg = m_failureMsg
                            + "Server: Representation in Post does not have hour attribute. ";
                }

                if (isServerOk)
                {
                    setTemperatureRep();
                    pResponse->setResponseResult(OC_EH_OK);
                    pResponse->setResourceRepresentation(m_temperatureRep);
                }
                else
                {
                    pResponse->setResponseResult(OC_EH_ERROR);
                }

                if (OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                }
                else
                {
                    isServerOk = false;
                    m_failureMsg = m_failureMsg + "Server: Post Request Response failed";
                    ehResult = OC_EH_ERROR;
                }
            }
            if (requestType == "DELETE")
            {
                IOTIVITYTEST_LOG(INFO, "\t\t\trequestType : DELETE");
                pResponse->setErrorCode(200);
                pResponse->setResponseResult(OC_EH_RESOURCE_DELETED);
                if (OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                {
                    ehResult = OC_EH_OK;
                    IOTIVITYTEST_LOG(INFO, "\t\t\t response done");
                }
                else
                {
                    isServerOk = false;
                    m_failureMsg = m_failureMsg + "Server: Get Request Response failed. ";
                    ehResult = OC_EH_ERROR;
                    IOTIVITYTEST_LOG(INFO, "\t\t\t response not done");
                }
                OCPlatform::unregisterResource(m_ResourceHandle);
            }
        }
    }

    return ehResult;
}

bool RIHelper::ocResourcePayloadAddStringLL(OCStringLL **stringLL, string value)
{
    char *dup = NULL;
    duplicateString(&dup, value);
    if (!*stringLL)
    {
        *stringLL = (OCStringLL *) OICCalloc(1, sizeof(OCStringLL));
        (*stringLL)->value = dup;
        return true;
    }
    else
    {
        OCStringLL *temp = *stringLL;
        while (temp->next)
        {
            temp = temp->next;
        }
        temp->next = (OCStringLL *) OICCalloc(1, sizeof(OCStringLL));
        temp->next->value = dup;
        return true;
    }
    return false;
}

void RIHelper::duplicateString(char ** targetString, string sourceString)
{
    *targetString = new char[sourceString.length() + 1];
    strncpy(*targetString, sourceString.c_str(), (sourceString.length() + 1));
}

OCPlatformInfo RIHelper::getPlatformInfo()
{
    configClientServerPlatform();

    OCPlatformInfo platformInfo;

    duplicateString(&platformInfo.platformID, PLATFORM_ID);
    duplicateString(&platformInfo.manufacturerName, MANUFACTURER_NAME);
    duplicateString(&platformInfo.manufacturerUrl, MANUFACTURER_URL);
    duplicateString(&platformInfo.modelNumber, MODEL_NO);
    duplicateString(&platformInfo.dateOfManufacture, MANUFACTURE_DATE);
    duplicateString(&platformInfo.platformVersion, PLATFORM_VERSION);
    duplicateString(&platformInfo.operatingSystemVersion, OS_VERSION);
    duplicateString(&platformInfo.hardwareVersion, HW_VERSION);
    duplicateString(&platformInfo.firmwareVersion, FIRMWARE_VERSION);
    duplicateString(&platformInfo.supportUrl, SUPPORT_URL);
    duplicateString(&platformInfo.systemTime, SYSTEM_TIME);

    return platformInfo;
}

OCDeviceInfo RIHelper::getDeviceInfo()
{
    configClientServerPlatform();
    OCDeviceInfo deviceInfo;
    duplicateString(&deviceInfo.deviceName, DEVICE_NAME);

    OCSetDeviceInfo(deviceInfo);

    return deviceInfo;
}

OCResourceHandle RIHelper::registerResource(string uri, string type, string iface)
{
    configClientServerPlatform();

    OCStackResult result = OCPlatform::registerResource(m_ResourceHandle, uri, type, iface,
            bind(&RIHelper::entityHandler, this, PH::_1), m_ResourceProperty);

    if (result == OC_STACK_OK)
        return m_ResourceHandle;
    else
        return NULL;
}

OCResource::Ptr RIHelper::constructResourceObject(string host, string uri)
{
    vector< string > types =
    { DEFAULT_RESOURCE_TYPE };
    vector< string > ifaces =
    { DEFAULT_INTERFACE };

    return OCPlatform::constructResourceObject(host, uri, CT_DEFAULT, false, types, ifaces);
}

string RIHelper::getFailure()
{
    return m_failureMsg;
}
