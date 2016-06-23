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

namespace PH = std::placeholders;

bool  ResourceServer :: m_isServerConstructed = false;

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
}

ResourceServer::~ResourceServer(void)
{
    if (m_isServerRunning)
    {
        stopServer();
    }
}

OCStackResult ResourceServer::constructServer(PlatformConfig& cfg)
{
    OCStackResult result = OC_STACK_OK;
    try
    {
//        m_platformConfig = cfg;
        OCPlatform::Configure(cfg);
        m_isServerConstructed = true;

        IOTIVITYTEST_LOG(INFO, "Server Created...");
    }
    catch (OCException& e)
    {
        result = OC_STACK_ERROR;
        IOTIVITYTEST_LOG(ERROR, "Error occurred while creating server, error code: %s", e.what());
    }

    return result;
}

OCStackResult ResourceServer::constructServer(std::string ip, int port)
{
    OCStackResult result = OC_STACK_OK;

    PlatformConfig cfg
    { OC::ServiceType::InProc, OC::ModeType::Both, ip, (uint16_t) port,
            OC::QualityOfService::LowQos };

//    m_platformConfig = cfg;
    result = constructServer(cfg);

    return result;
}

OCEntityHandlerResult ResourceServer::entityHandler(std::shared_ptr< OCResourceRequest > request)
{

    auto pResponse = std::make_shared< OC::OCResourceResponse >();
    OCEntityHandlerResult result = OC_EH_OK;

    IOTIVITYTEST_LOG(DEBUG, "\tIn Server entity handler:\n");

    if (request)
    {
        pResponse->setRequestHandle(request->getRequestHandle());
        pResponse->setResourceHandle(request->getResourceHandle());

        // Get the request type and request flag
        std::string requestType = request->getRequestType();
        RequestHandlerFlag requestFlag = (RequestHandlerFlag) request->getRequestHandlerFlag();

//        if (requestFlag == RequestHandlerFlag::InitFlag)
//        {
//            IOTIVITYTEST_LOG(INFO, "\t\trequestFlag : Init\n");
//            // entity handler to perform resource initialization operations
//
//            // Check for query params (if any)
//            QueryParamsMap queryParamsMap = request->getQueryParameters();
//
//            handleInitRequest(queryParamsMap, pResponse); // Process query params and do required operations ..
//        }
//        else
        if (requestFlag == RequestHandlerFlag::RequestFlag)
        {
            IOTIVITYTEST_LOG(INFO, "\t\trequestFlag : Request\n");

            // If the request type is GET
            if (requestType == "GET")
            {
                IOTIVITYTEST_LOG(INFO, "\t\t\trequestType : GET\n");

                // Check for query params (if any)
                QueryParamsMap queryParamsMap = request->getQueryParameters();

                handleGetRequest(queryParamsMap, pResponse); // Process query params and do required operations ..

            }
            else if (requestType == "PUT")
            {
                IOTIVITYTEST_LOG(INFO, "\t\t\trequestType : PUT\n");

                OCRepresentation incomingRepresentation = request->getResourceRepresentation();

                // Check for query params (if any)
                QueryParamsMap queryParamsMap = request->getQueryParameters();

                handlePutRequest(queryParamsMap, incomingRepresentation, pResponse); // Process query params and do required operations ..
            }
            else if (requestType == "POST")
            {
                // POST request operations
                IOTIVITYTEST_LOG(INFO, "\t\t\trequestType : POST\n");

                OCRepresentation incomingRepresentation = request->getResourceRepresentation();

                // Check for query params (if any)
                QueryParamsMap queryParamsMap = request->getQueryParameters();

                handlePostRequest(queryParamsMap, incomingRepresentation, pResponse); // Process query params and do required operations ..
            }
            else if (requestType == "DELETE")
            {
                // DELETE request operations
                IOTIVITYTEST_LOG(INFO, "\t\t\trequestType : Delete\n");

                OCRepresentation incomingRepresentation = request->getResourceRepresentation();
                // Check for query params (if any)
                QueryParamsMap queryParamsMap = request->getQueryParameters();

                handleDeleteRequest(queryParamsMap, incomingRepresentation, pResponse); // Process query params and do required operations ..
            }
        }
        else if (requestFlag & RequestHandlerFlag::ObserverFlag)
        {
            // OBSERVE flag operations
            IOTIVITYTEST_LOG(INFO, "\t\t\trequestType : Observe\n");

            // Check for query params (if any)
            QueryParamsMap queryParamsMap = request->getQueryParameters();

            handleObserveRequest(queryParamsMap, request, pResponse); // Process query params and do required operations ..
        }
    }
    else
    {
        IOTIVITYTEST_LOG(WARNING, "Request invalid");
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

    // This will internally create and register the resource.
    result = OCPlatform::registerResource(m_resourceHandle, m_resourceURI, m_resourceTypeName,
            m_resourceInterface, std::bind(&ResourceServer::entityHandler, this, PH::_1),
            resourceProperty);

    if (result != OC_STACK_OK)
    {
        IOTIVITYTEST_LOG(ERROR, "Device Resource failed to start, result code =  %s",
                std::to_string(result).c_str());
        throw std::runtime_error(
                std::string("Device Resource failed to start") + std::to_string(result));
    }
    else
    {
        m_isServerRunning = true;
        IOTIVITYTEST_LOG(INFO, "Server Started");

        bool isRegisteredForPresence = false;
        int presenceInterval = 0;
        onResourceServerStarted(isRegisteredForPresence, presenceInterval); // pure virtual method to implement task for child after resource server starts

        if (isRegisteredForPresence)
        {
            OCPlatform::startPresence(presenceInterval);
        }

        m_representation.setUri(m_resourceURI);

        std::vector< std::string > interfaces;
        interfaces.push_back(m_resourceInterface);
        m_representation.setResourceInterfaces(interfaces);

        std::vector< std::string > types;
        types.push_back(m_resourceTypeName);
        m_representation.setResourceTypes(types);
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
        IOTIVITYTEST_LOG(ERROR, "Resource server is not started; Please start it before stopping ");

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
        IOTIVITYTEST_LOG(ERROR, "Unable to stop server");
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
