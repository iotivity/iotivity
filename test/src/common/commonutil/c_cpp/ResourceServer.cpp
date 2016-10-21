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
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

#include "ResourceServer.h"
#include "ocstack.h"
#include "ocpayload.h"

namespace PH = std::placeholders;

bool ResourceServer::m_isServerConstructed = false;
OCPlatformInfo ResourceServer::m_platformInfo;
OCDeviceInfo ResourceServer::m_deviceInfo;
ResourceHelper* ResourceServer::p_resourceHelper = ResourceHelper::getInstance();

ResourceServer::ResourceServer(void) :
        m_resourceHandle(NULL), m_platformConfig(
        { OC::ServiceType::InProc, OC::ModeType::Both, "", (uint16_t) 0,
                OC::QualityOfService::MidQos })
{
    m_resourceURI = "/device";
    m_resourceTypeName = "core.mock";
    m_resourceInterface = DEFAULT_INTERFACE;
    m_isServerRunning = false;
    m_isServerConstructed = false;
    m_isSlowResource = false;
    m_resourceProperty = OC_ACTIVE;
    m_resourceTypeNames.clear();
    m_resourceInterfaces.clear();
}

ResourceServer::~ResourceServer(void)
{
    if (m_isServerRunning)
    {
        stopResource();
    }
}

OCStackResult ResourceServer::constructServer(PlatformConfig &cfg)
{
    OCStackResult result = OC_STACK_OK;
    try
    {
        OCPlatform::Configure(cfg);
        m_isServerConstructed = true;

        setPlatformInfo(PLATFORM_ID, MANUFACTURER_NAME, MANUFACTURER_URL, MODEL_NUMBER,
        DATE_OF_MANUFACTURE, PLATFORM_VERSION, OPERATING_SYSTEM_VERSION,
        HARDWARE_VERSION, FIRMWARE_VERSION, SUPPORT_URL, SYSTEM_TIME);

        OCResourcePayloadAddStringLL(&m_deviceInfo.types, "oic.wk.d");
        OCSetDeviceInfo(m_deviceInfo);

        setDeviceInfo(DEVICE_NAME);

        result = OCPlatform::registerPlatformInfo(m_platformInfo);

        result = OCPlatform::registerDeviceInfo(m_deviceInfo);

        cout << "Server Created..." << endl;
    }
    catch (OCException &e)
    {
        result = OC_STACK_ERROR;
        cerr << "Error occurred while creating server, error code: " << e.what() << endl;
    }

    return result;
}

OCStackResult ResourceServer::constructServer(std::string ip, int port)
{
    OCStackResult result = OC_STACK_OK;

    PlatformConfig cfg
    { OC::ServiceType::InProc, OC::ModeType::Both, ip, (uint16_t) port, OC::QualityOfService::LowQos };

    result = constructServer(cfg);

    return result;
}

OCEntityHandlerResult ResourceServer::entityHandler(std::shared_ptr< OCResourceRequest > request)
{

    OCEntityHandlerResult result = OC_EH_OK;

    cout << "\tIn Server entity handler:\n";

    if (request)
    {
        if (m_isSlowResource)
        {
            std::thread t(bind(&ResourceServer::handleSlowResponse, this, PH::_1), request);
            t.detach();
            result = OC_EH_SLOW;
        }
        else
        {
            handleResponse(request);
            result = OC_EH_OK;
        }
    }
    else
    {
        cerr << "Request invalid" << endl;
    }

    return result;
}

OCRepresentation ResourceServer::getRepresentation(void)
{
    return getResourceRepresentation(m_representation);
}

OCResourceHandle ResourceServer::getResourceHandle(void)
{
    return m_resourceHandle;
}

OCStackResult ResourceServer::setResourceProperties(std::string resourceUri,
        std::string resourceTypeName, std::string resourceInterface)
{
    m_resourceURI = resourceUri;
    m_resourceTypeName = resourceTypeName;
    m_resourceInterface = resourceInterface;

    OCStackResult result = OC_STACK_OK;

    if ((m_resourceURI.compare("") == 0) || (m_resourceTypeName.compare("") == 0)
            || (m_resourceInterface.compare("") == 0))
    {
        result = OC_STACK_ERROR;
    }

    return result;
}

OCStackResult ResourceServer::startResource(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_OK;

    m_resourceProperty = m_resourceProperty | resourceProperty;

    if (isObservableResource())
    {
        cout << "This resource is Observable" << endl;
    }
    if (isDiscoverableResource())
    {
        cout << "This resource is Discoverable" << endl;
    }
    if (isSecuredResource())
    {
        cout << "This resource is Secured" << endl;
    }

    if (m_resourceTypeName.find(" ") != string::npos)
    {
        stringstream typeStream(m_resourceTypeName);
        string currentType = "";
        while (typeStream >> currentType)
        {
            m_resourceTypeNames.push_back(currentType);
        }
        m_resourceTypeName = m_resourceTypeNames.at(0);
    }
    else
    {
        m_resourceTypeNames.push_back(m_resourceTypeName);
    }

    if (m_resourceInterface.find(" ") != string::npos)
    {
        stringstream interfaceStream(m_resourceInterface);
        string currentInterface = "";
        while (interfaceStream >> currentInterface)
        {
            m_resourceInterfaces.push_back(currentInterface);
        }
        m_resourceInterface = m_resourceInterfaces.at(0);
    }
    else
    {
        m_resourceInterfaces.push_back(m_resourceInterface);
    }

    // This will internally create and register the resource.
    result = OCPlatform::registerResource(m_resourceHandle, m_resourceURI, m_resourceTypeName,
            m_resourceInterface, std::bind(&ResourceServer::entityHandler, this, PH::_1),
            m_resourceProperty);
    if (m_resourceTypeNames.size() > 1)
    {
        for (unsigned int i = 1; i < m_resourceTypeNames.size(); i++)
        {
            OCPlatform::bindTypeToResource(m_resourceHandle, m_resourceTypeNames.at(i));
        }
    }

    if (m_resourceInterfaces.size() > 1)
    {
        for (unsigned int i = 1; i < m_resourceInterfaces.size(); i++)
        {
            OCPlatform::bindInterfaceToResource(m_resourceHandle, m_resourceInterfaces.at(i));
        }
    }

    if (result != OC_STACK_OK)
    {
        cerr << "Device Resource failed to start, result code =  " << result << endl;
        throw std::runtime_error(
                std::string("Device Resource failed to start") + std::to_string(result));
    }
    else
    {
        m_isServerRunning = true;
        cout << "Server Started" << endl;

        m_isRegisteredForPresence = false;
        int presenceInterval = 0;
        onResourceServerStarted(m_isRegisteredForPresence, presenceInterval); // pure virtual method to implement task for child after resource server starts

        if (m_isRegisteredForPresence)
        {
            OCPlatform::startPresence(presenceInterval);
        }

        m_representation.setUri(m_resourceURI);

    }

    return result;
}

OCStackResult ResourceServer::stopResource(void)
{
    OCStackResult result = OC_STACK_OK;

    //check whether server is running
    if (m_isServerRunning == false)
    {
        result = OC_STACK_ERROR;
        cerr << "Resource server is not started; Please start it before stopping " << endl;

        return result;
    }

    //stop presence
    if (m_isRegisteredForPresence)
    {
        OCPlatform::stopPresence();
    }

    //stop server
    result = OCPlatform::unregisterResource(m_resourceHandle);

    if (result == OC_STACK_OK)
    {
        m_isServerRunning = false;
    }
    else
    {
        cerr << "Unable to stop server" << endl;
    }

    return result;
}

void ResourceServer::setResourceRepresentation(OCRepresentation ocRepresentation)
{
    m_representation = ocRepresentation;
}

string ResourceServer::getUri(void)
{
    return m_resourceURI;
}

OCStackResult ResourceServer::setPlatformInfo(string platformID, string manufacturerName,
        string manufacturerUrl, string modelNumber, string dateOfManufacture,
        string platformVersion, string operatingSystemVersion, string hardwareVersion,
        string firmwareVersion, string supportUrl, string systemTime)
{
    p_resourceHelper->duplicateString(&m_platformInfo.platformID, platformID);
    p_resourceHelper->duplicateString(&m_platformInfo.manufacturerName, manufacturerName);
    p_resourceHelper->duplicateString(&m_platformInfo.manufacturerUrl, manufacturerUrl);
    p_resourceHelper->duplicateString(&m_platformInfo.modelNumber, modelNumber);
    p_resourceHelper->duplicateString(&m_platformInfo.dateOfManufacture, dateOfManufacture);
    p_resourceHelper->duplicateString(&m_platformInfo.platformVersion, platformVersion);
    p_resourceHelper->duplicateString(&m_platformInfo.operatingSystemVersion,
            operatingSystemVersion);
    p_resourceHelper->duplicateString(&m_platformInfo.hardwareVersion, hardwareVersion);
    p_resourceHelper->duplicateString(&m_platformInfo.firmwareVersion, firmwareVersion);
    p_resourceHelper->duplicateString(&m_platformInfo.supportUrl, supportUrl);
    p_resourceHelper->duplicateString(&m_platformInfo.systemTime, systemTime);
    return OC_STACK_OK;
}

void ResourceServer::setAsSlowResource()
{
    m_isSlowResource = true;
}

void ResourceServer::setAsNormalResource()
{
    m_isSlowResource = false;
}

OCStackResult ResourceServer::setDeviceInfo(string deviceName, string deviceType)
{
    p_resourceHelper->duplicateString(&m_deviceInfo.deviceName, deviceName);
    if (deviceType.compare("") != 0)
    {
        OCResourcePayloadAddStringLL(&m_deviceInfo.types, deviceType.c_str());
        p_resourceHelper->duplicateString(&m_deviceInfo.specVersion, CORE_SPEC_VERSION);
        OCResourcePayloadAddStringLL(&m_deviceInfo.dataModelVersions, RESOURCE_TYPE_SPEC_VERSION);
        OCResourcePayloadAddStringLL(&m_deviceInfo.dataModelVersions, SMART_HOME_SPEC_VERSION);
    }

    OCSetDeviceInfo(m_deviceInfo);
    return OC_STACK_OK;
}

void ResourceServer::handleResponse(std::shared_ptr< OCResourceRequest > request)
{
    auto pResponse = make_shared< OC::OCResourceResponse >();
    pResponse->setRequestHandle(request->getRequestHandle());
    pResponse->setResourceHandle(request->getResourceHandle());

    // Get the request type and request flag
    string requestType = request->getRequestType();
    RequestHandlerFlag requestFlag = (RequestHandlerFlag) request->getRequestHandlerFlag();

    if (requestFlag == RequestHandlerFlag::RequestFlag)
    {
        cout << "\t\trequestFlag : Request\n";
        cout << "\t\t\trequestType : " << requestType << endl;

        // If the request type is GET
        if (requestType == "GET")
        {
            // Check for query params (if any)
            QueryParamsMap queryParamsMap = request->getQueryParameters();

            handleGetRequest(queryParamsMap, request, pResponse); // Process query params and do required operations ..

        }
        else if (requestType == "PUT")
        {
            OCRepresentation incomingRepresentation = request->getResourceRepresentation();

            // Check for query params (if any)
            QueryParamsMap queryParamsMap = request->getQueryParameters();

            handlePutRequest(queryParamsMap, incomingRepresentation, request, pResponse); // Process query params and do required operations ..
        }
        else if (requestType == "POST")
        {
            // POST request operations
            OCRepresentation incomingRepresentation = request->getResourceRepresentation();

            // Check for query params (if any)
            QueryParamsMap queryParamsMap = request->getQueryParameters();

            handlePostRequest(queryParamsMap, incomingRepresentation, request, pResponse); // Process query params and do required operations ..
        }
        else if (requestType == "DELETE")
        {
            // DELETE request operations
            OCRepresentation incomingRepresentation = request->getResourceRepresentation();
            // Check for query params (if any)
            QueryParamsMap queryParamsMap = request->getQueryParameters();

            handleDeleteRequest(queryParamsMap, incomingRepresentation, request, pResponse); // Process query params and do required operations ..
        }
    }
    else if (requestFlag & RequestHandlerFlag::ObserverFlag)
    {
        // Check for query params (if any)
        QueryParamsMap queryParamsMap = request->getQueryParameters();

        handleObserveRequest(queryParamsMap, request, pResponse); // Process query params and do required operations ..
    }
}

void ResourceServer::handleSlowResponse(std::shared_ptr< OCResourceRequest > request)
{
    cout << "Acting as Slow Resource...." << endl;
    p_resourceHelper->waitInSecond(CALLBACK_WAIT_MAX);
    cout << "Slow working period is over" << endl;

    handleResponse(request);
}

bool ResourceServer::isObservableResource(void)
{
    return m_resourceProperty & OC_OBSERVABLE;
}

bool ResourceServer::isDiscoverableResource(void)
{
    return m_resourceProperty & OC_DISCOVERABLE;
}

std::vector< std::string > ResourceServer::getResourceInterfaces(void)
{
    return m_resourceInterfaces;
}

std::vector< std::string > ResourceServer::getResourceTypes(void)
{
    return m_resourceTypeNames;
}

void ResourceServer::setAsSecuredResource(void)
{
    m_resourceProperty = m_resourceProperty | OC_SECURE;
}

void ResourceServer::setAsDiscoverableResource(void)
{
    m_resourceProperty = m_resourceProperty | OC_DISCOVERABLE;
}

void ResourceServer::setAsObservableResource(void)
{
    m_resourceProperty = m_resourceProperty | OC_OBSERVABLE;
}

bool ResourceServer::isSecuredResource(void)
{
    return m_resourceProperty & OC_SECURE;
}
