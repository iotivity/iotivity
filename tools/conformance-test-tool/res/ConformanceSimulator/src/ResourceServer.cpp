//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include "ResourceServer.h"

namespace PH = std::placeholders;

bool ResourceServer::m_isServerConstructed = false;
OCPlatformInfo ResourceServer::m_platformInfo;
OCDeviceInfo ResourceServer::m_deviceInfo;

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
    m_observeStatus = false;
    m_discoverStatus = false;
}

ResourceServer::~ResourceServer(void)
{
    if (m_isServerRunning)
    {
        stopServer();
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

OCStackResult ResourceServer::startServer(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_OK;

    if (resourceProperty & OC_OBSERVABLE)
    {
        cout << "This resource is Observable" << endl;
        m_observeStatus = true;
    }
    if (resourceProperty & OC_DISCOVERABLE)
    {
        cout << "This resource is Discoverable" << endl;
        m_discoverStatus = true;
    }

    vector< string > resourceTypeNames;
    vector< string > resourceInterfaces;
    if (m_resourceTypeName.find(" ") != string::npos)
    {
        stringstream typeStream(m_resourceTypeName);
        string currentType = "";
        while (typeStream >> currentType)
        {
            resourceTypeNames.push_back(currentType);
        }
        m_resourceTypeName = resourceTypeNames.at(0);
        resourceTypeNames.erase(resourceTypeNames.begin());
    }

    if (m_resourceInterface.find(" ") != string::npos)
    {
        stringstream interfaceStream(m_resourceInterface);
        string currentInterface = "";
        while (interfaceStream >> currentInterface)
        {
            resourceInterfaces.push_back(currentInterface);
        }
        m_resourceInterface = resourceInterfaces.at(0);
        resourceInterfaces.erase(resourceInterfaces.begin());
    }

    // This will internally create and register the resource.
    result = OCPlatform::registerResource(m_resourceHandle, m_resourceURI, m_resourceTypeName,
            m_resourceInterface, std::bind(&ResourceServer::entityHandler, this, PH::_1),
            resourceProperty);
    if (resourceTypeNames.size() > 0)
    {
        for (unsigned int i = 0; i < resourceTypeNames.size(); i++)
        {
            OCPlatform::bindTypeToResource(m_resourceHandle, resourceTypeNames.at(i));
        }
    }

    if (resourceInterfaces.size() > 0)
    {
        for (unsigned int i = 0; i < resourceInterfaces.size(); i++)
        {
            OCPlatform::bindInterfaceToResource(m_resourceHandle, resourceInterfaces.at(i));
        }
    }

    OCPlatform::bindInterfaceToResource(m_resourceHandle, OC_RSRVD_INTERFACE_DEFAULT);

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

        bool isRegisteredForPresence = false;
        int presenceInterval = 0;
        onResourceServerStarted(isRegisteredForPresence, presenceInterval); // pure virtual method to implement task for child after resource server starts

        if (isRegisteredForPresence)
        {
            OCPlatform::startPresence(presenceInterval);
        }

        m_representation.setUri(m_resourceURI);

        std::vector< std::string > interfaces = resourceInterfaces;
        interfaces.push_back(m_resourceInterface);
        m_representation.setResourceInterfaces(interfaces);

        std::vector< std::string > types = resourceTypeNames;
        types.push_back(m_resourceTypeName);
        m_representation.setResourceTypes(types);

        m_representation.addResourceInterface(OC_RSRVD_INTERFACE_DEFAULT);
        m_representation.setUri(m_resourceURI);
    }

    return result;
}

OCStackResult ResourceServer::stopServer(void)
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
    result = OCPlatform::stopPresence();

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

void ResourceServer::setResourceRepresentation(OCRepresentation oCRepresentation)
{
    m_representation = oCRepresentation;
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
    CommonUtil::duplicateString(&m_platformInfo.platformID, platformID);
    CommonUtil::duplicateString(&m_platformInfo.manufacturerName, manufacturerName);
    CommonUtil::duplicateString(&m_platformInfo.manufacturerUrl, manufacturerUrl);
    CommonUtil::duplicateString(&m_platformInfo.modelNumber, modelNumber);
    CommonUtil::duplicateString(&m_platformInfo.dateOfManufacture, dateOfManufacture);
    CommonUtil::duplicateString(&m_platformInfo.platformVersion, platformVersion);
    CommonUtil::duplicateString(&m_platformInfo.operatingSystemVersion, operatingSystemVersion);
    CommonUtil::duplicateString(&m_platformInfo.hardwareVersion, hardwareVersion);
    CommonUtil::duplicateString(&m_platformInfo.firmwareVersion, firmwareVersion);
    CommonUtil::duplicateString(&m_platformInfo.supportUrl, supportUrl);
    CommonUtil::duplicateString(&m_platformInfo.systemTime, systemTime);
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

OCStackResult ResourceServer::setDeviceInfo(string deviceName)
{
    CommonUtil::duplicateString(&m_deviceInfo.deviceName, deviceName);
    return OC_STACK_OK;
}

void ResourceServer::handleResponse(std::shared_ptr< OCResourceRequest > request)
{
    auto pResponse = std::make_shared< OC::OCResourceResponse >();
    pResponse->setRequestHandle(request->getRequestHandle());
    pResponse->setResourceHandle(request->getResourceHandle());

    // Get the request type and request flag
    std::string requestType = request->getRequestType();
    RequestHandlerFlag requestFlag = (RequestHandlerFlag) request->getRequestHandlerFlag();

    if (requestFlag == RequestHandlerFlag::RequestFlag)
    {
        cout << "\t\trequestFlag : Request\n";

        // If the request type is GET
        if (requestType == "GET")
        {
            cout << "\t\t\trequestType : GET\n";

            // Check for query params (if any)
            QueryParamsMap queryParamsMap = request->getQueryParameters();

            handleGetRequest(queryParamsMap, request, pResponse); // Process query params and do required operations ..

        }
        else if (requestType == "PUT")
        {
            cout << "\t\t\trequestType : PUT\n";

            OCRepresentation incomingRepresentation = request->getResourceRepresentation();

            // Check for query params (if any)
            QueryParamsMap queryParamsMap = request->getQueryParameters();

            handlePutRequest(queryParamsMap, incomingRepresentation, request, pResponse); // Process query params and do required operations ..
        }
        else if (requestType == "POST")
        {
            // POST request operations
            cout << "\t\t\trequestType : POST\n";

            OCRepresentation incomingRepresentation = request->getResourceRepresentation();

            // Check for query params (if any)
            QueryParamsMap queryParamsMap = request->getQueryParameters();

            handlePostRequest(queryParamsMap, incomingRepresentation, request, pResponse); // Process query params and do required operations ..
        }
        else if (requestType == "DELETE")
        {
            // DELETE request operations
            cout << "\t\t\trequestType : Delete\n";

            OCRepresentation incomingRepresentation = request->getResourceRepresentation();
            // Check for query params (if any)
            QueryParamsMap queryParamsMap = request->getQueryParameters();

            handleDeleteRequest(queryParamsMap, incomingRepresentation, request, pResponse); // Process query params and do required operations ..
        }
    }
    else if (requestFlag & RequestHandlerFlag::ObserverFlag)
    {
        // OBSERVE flag operations
        cout << "\t\t\trequestType : Observe\n";

        // Check for query params (if any)
        QueryParamsMap queryParamsMap = request->getQueryParameters();

        handleObserveRequest(queryParamsMap, request, pResponse); // Process query params and do required operations ..
    }
}

void ResourceServer::handleSlowResponse(std::shared_ptr< OCResourceRequest > request)
{
    cout << "Acting as Slow Resource...." << endl;
    CommonUtil::waitInSecond(CALLBACK_WAIT_MAX);
    cout << "Slow working period is over" << endl;

    handleResponse(request);
}

bool ResourceServer::isObservableResource(void)
{
    return m_observeStatus;
}

bool ResourceServer::isDiscoverableResource(void)
{
    return m_discoverStatus;
}
