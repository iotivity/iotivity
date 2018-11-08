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
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

/// @file ResourceServer.h
/// @brief  This file contains the declaration of the resource server
///         class and its members.
#ifndef _RESOURCE_SERVER_H_
#define _RESOURCE_SERVER_H_

#include <chrono>
#include <iostream>
#include <sstream>
#include <pthread.h>
#include <ResourceHelper.h>
#include <stdexcept>
#include <memory>
#include <string>
#include <memory>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include "OCPlatform.h"
#include "OCApi.h"
#include "ocstack.h"

using namespace OC;

    /**
     * Types of handlers for Sample Resource
     *
     */
    enum class HandlerType
    {
        DEFAULT,
        SAMPLE
    };

    /**
     * Types of Sample Resource handler response time
     *
     * NOTE: This will matter only when using SAMPLE resource handler
     */
    enum class ResponseTimeType
    {
        SLOW,
        NORMAL
    };

/**
 *   @brief  To use a sample server, ResourceServer class must
 *           be instantiated. then it should be constructed and run
 */
class ResourceServer
{
protected:
    OCRepresentation m_representation;
    std::string m_resourceURI;
    std::string m_resourceTypeName;
    std::vector<std::string> m_resourceTypeNames;
    std::string m_resourceInterface;
    std::vector<std::string> m_resourceInterfaces;
    std::vector<ResourceServer*> m_childResourceList;
    OCResourceHandle m_resourceHandle;
    PlatformConfig m_platformConfig;
    ResponseTimeType m_responseTimeType;
    bool m_isRegisteredForPresence;
    bool m_isServerRunning;
    uint8_t m_resourceProperty;
    static bool s_isServerConstructed;
    static OCPlatformInfo s_platformInfo;
    static OCDeviceInfo s_deviceInfo;
    static ResourceHelper* p_resourceHelper;

public:

    /**
     * Constructor for ResourceServer. Initializes server variables
     *
     */
    ResourceServer(void);

    /**
     * virtual destructor for Resource Server.
     */
    virtual ~ResourceServer(void);

    /**
     * API for constructing the server with a PlatformConfig object
     * @param cfg - PlatformConfig object containing platform settings
     *
     * @return OCStackResult - returns OC_STACK_OK if successful to construct server,
     *                          else OC_STACK_ERROR
     *
     * NOTE: This API is for already available PlatformConfig
     * NOTE: PlatformConfig is defined in OCApi.h.
     */
    static OCStackResult constructServer(PlatformConfig &cfg);

    /**
     * API for constructing the server with a client IP & port
     *
     * @param ip - IP address of the server
     * @param port - port of the server
     *
     * @return OCStackResult - returns OC_STACK_OK if successful to construct server,
     *                          else OC_STACK_ERROR
     *
     * NOTE: This API is for construction the server in Confirmable QoC mode
     */
    static OCStackResult constructServer(std::string ip, int port);

    /**
     * API for getting device info
     *
     * @return OCStackResult - returns s_deviceInfo
     *
     */
    static OCDeviceInfo getDeviceInfo();

    /**
     * API to set the basic properties of the resource
     *
     * @param resourceURI - uri of the resource
     * @param resourceTypeName - resource type name of the resource
     * @param resourceInterface - interface of the resource
     *
     * @return OCStackResult - returns OC_STACK_OK if successful to set properties,
     *                          else OC_STACK_ERROR
     *
     * NOTE: This function must be called before starting the resource
     */
    OCStackResult setResourceProperties(std::string resourceUri, std::string resourceTypeName,
            std::string resourceInterface);

    /**
     * API to get the resource handle of the resource
     *
     * @return OCResourceHandle - returns the resource handle to the resource
     *
     * NOTE: available interfaces are defined in OCApi.h
     */
    OCResourceHandle getResourceHandle(void);

    /**
     * API to handle and response to the incoming request
     *
     * @param request - pointer to the OCResourceRequest type incoming request
     * @param response - pointer to the OCResourceResponse type outgoing response
     *
     * NOTE: available interfaces are defined in OCApi.h
     */

    virtual OCEntityHandlerResult entityHandler(std::shared_ptr< OCResourceRequest > request);

    /**
     * API to get the representation model of the resource
     *
     * @return OCRepresentation, the representation class of the resource server
     *
     * NOTE: OCRepresentation is defined in OCApi.h
     */
    OCRepresentation getRepresentation(void);

    /**
     * API to get the representation model of the resource
     *
     * @return vector<string>, list of resource interfaces
     */
    std::vector<std::string> getResourceInterfaces(void);

    /**
     * API to get the representation model of the resource
     *
     * @return vector<string>, list of resource types
     */
    std::vector<std::string> getResourceTypes(void);

    /**
     * API to get the representation model of the resource
     *
     * @param OCRepresentation - the representation to set
     *
     * NOTE: OCRepresentation is defined in OCApi.h
     */
    void setResourceRepresentation(OCRepresentation oCRepresentation);

    /**
     * API to stop the server
     *
     * @return OCStackResult - returns OC_STACK_OK if successful to stop server,
     *                          else OC_STACK_ERROR
     */
    OCStackResult stopResource(void);

    /**
     * API for starting the server.
     *
     * @param handlerType - type of handler to use by created resource
     *
     * @return OCStackResult - returns OC_STACK_OK if successful to start server,
     *                          else OC_STACK_ERROR
     *
     * NOTE: The server will stop when the main program exits
     */
    OCStackResult startResource(uint8_t resourceProperty = OC_ACTIVE, HandlerType handlerType = HandlerType::SAMPLE);

    /**
     * API for getting the uri of the resource
     *
     * @return string - returns the uri of the resource
     *
     */
    std::string getUri(void);

    /**
     * API for setting the resource response as secured
     *
     */
    void setAsSecuredResource(void);

    /**
     * API for setting the resource response as discoverable
     *
     */
    void setAsDiscoverableResource(void);

    /**
     * API for setting the resource response as observable
     *
     */
    void setAsObservableResource(void);

    /**
     * API for setting the resource response
     *
     * @param responseTimeType - resource response time type to set
     *
     */
    void setResponseTimeType(ResponseTimeType responseTimeType);

    /**
     * API for setting the resource response as slow
     *
     */
    void setAsSlowResource(void);

    /**
     * API for setting the resource response as normal
     *
     */
    void setAsNormalResource(void);

    /**
     * API for handling normal response
     *
     * @param[in] request - OCResourceRequest variable, the request from client
     *
     */
    void handleResponse(std::shared_ptr< OCResourceRequest > request);

    /**
     * API for handling slow response
     *
     * @param[in] request - OCResourceRequest variable, the request from client
     *
     */
    void handleSlowResponse(std::shared_ptr< OCResourceRequest > request);

    /**
     * API to know whether the resource is observable or not
     *
     * @return bool - if resource is observable. true is returned, else  false
     *
     */
    bool isObservableResource(void);

    /**
     * API to know whether the resource is secured or not
     *
     * @return bool - if resource is observable. true is returned, else  false
     *
     */
    bool isSecuredResource(void);

    /**
     * API to know whether the resource is discoverable or not
     *
     * @return bool - if resource is discoverable. true is returned, else  false
     *
     */
    bool isDiscoverableResource(void);

   /**
     * API to add a new resource type to the existing resource
     *
     * @param[in] - resourceType - resource type to add
     *
     */
    void addResourceType(string resourceType);

    /**
      * API to add a new resource interface to the existing resource
      *
      * @param[in] - resourceinterface - resource interface to add
      *
      */
    void addResourceInterface(string resourceInterface);

    /**
      * API to set Platform info of the current device platform
      *
      * @param[in] - platformID - Unique ID of the platform in form of UUID
      * @param[in] - manufacturerName - Human radable name of the device manufacturer
      * @param[in] - manufacturerUrl - URI to the device manufacturer webpage
      * @param[in] - platformID - Unique ID of the platform in form of UUID
      * @param[in] - platformID - Unique ID of the platform in form of UUID
      * @param[in] - platformID - Unique ID of the platform in form of UUID
      *
      */
    static OCStackResult setPlatformInfo(string platformID, string manufacturerName,
            string manufacturerUrl, string modelNumber, string dateOfManufacture,
            string platformVersion, string operatingSystemVersion, string hardwareVersion,
            string firmwareVersion, string supportUrl, string systemTime);

    /**
      * API to set Device info of the current device
      *
      * @param[in] - deviceName - Human readable name of the device
      * @param[in] - deviceTypes - List of device types of the device(Optional)
      * @param[in] - specVersion - OCF version for device, with default ocf.1.0.0
      *
      */
    static OCStackResult setDeviceInfo(string deviceName, vector<string> deviceTypes = vector<string>(), string specVersion = CORE_SPEC_VERSION);

    /**
      * API to set Device info of the current device
      *
      * @param[in] - deviceName - Human readable name of the device
      * @param[in] - deviceTypes - Device type of the device(Optional)
      * @param[in] - specVersion - OCF version for device, with default ocf.1.0.0
      *
      */
    static OCStackResult setDeviceInfo(string deviceName, string deviceTypes, string specVersion = CORE_SPEC_VERSION);

    /***
     * API to perform additional task for child
     *
     * @param[out] isRegisteredForPresence - Boolean variable to register for presence,
     *             default value is false, to register for presence, set it to true
     * @param[out] presenceInterval - interval between presences in millisecond
     *             default value is 0 millisecond
     *
     * NOTE: This is a pure virtual function, thus it must be implemented by the child
     */
    virtual void onResourceServerStarted(bool &isRegisteredForPresence, int &presenceInterval) = 0;

    /***
     * API to handle OBSERVE request
     *
     * @param[in] queryParamsMap - A map containing all query parameters
     * @param[out] response - response to the incoming request
     *
     * NOTE: This is a pure virtual function, thus it must be implemented by the child
     */
    virtual void handleObserveRequest(QueryParamsMap &queryParamsMap,
            std::shared_ptr< OCResourceRequest > request,
            std::shared_ptr< OCResourceResponse > response) = 0;

    /***
     * API to handle DELETE request
     *
     * @param[in] queryParamsMap - A map containing all query parameters
     * @param[out] response - response to the incoming request
     *
     * NOTE: This is a pure virtual function, thus it must be implemented by the child
     */
    virtual void handleDeleteRequest(QueryParamsMap &queryParamsMap,
            OCRepresentation incomingRepresentation, std::shared_ptr< OCResourceRequest > request,
            std::shared_ptr< OCResourceResponse > response) = 0;

    /***
     * API to handle POST request
     *
     * @param[in] queryParamsMap - A map containing all query parameters
     * @param[out] response - response to the incoming request
     *
     * NOTE: This is a pure virtual function, thus it must be implemented by the child
     */
    virtual void handlePostRequest(QueryParamsMap &queryParamsMap,
            OCRepresentation incomingRepresentation, std::shared_ptr< OCResourceRequest > request,
            std::shared_ptr< OCResourceResponse > response) = 0;

    /***
     * API to handle GET request
     *
     * @param[in] queryParamsMap - A map containing all query parameters
     * @param[out] response - response to the incoming request
     *
     * NOTE: This is a pure virtual function, thus it must be implemented by the child
     */
    virtual void handleGetRequest(QueryParamsMap &queryParamsMap,
            std::shared_ptr< OCResourceRequest > request,
            std::shared_ptr< OCResourceResponse > response) = 0;

    /***
     * API to handle PUT request
     *
     * @param[in] queryParamsMap - A map containing all query parameters
     * @param[out] response - response to the incoming request
     *
     * NOTE: This is a pure virtual function, thus it must be implemented by the child
     */
    virtual void handlePutRequest(QueryParamsMap &queryParamsMap,
            OCRepresentation incomingRepresentation, std::shared_ptr< OCResourceRequest > request,
            std::shared_ptr< OCResourceResponse > response) = 0;

    /***
     * API to handle INIT request
     *
     * @param[in] queryParamsMap - A map containing all query parameters
     * @param[out] response - response to the incoming request
     *
     * NOTE: This is a pure virtual function, thus it must be implemented by the child
     */
    virtual void handleInitRequest(QueryParamsMap &queryParamsMap,
            std::shared_ptr< OCResourceRequest > request,
            std::shared_ptr< OCResourceResponse > response) = 0;

    /***
     * API to get the attribute list of the resource server
     *
     * @param[in] temp - epresentation of the resource server
     *
     * @return OCRepresentation - representation of the resource server
     */
    virtual OCRepresentation getResourceRepresentation(OCRepresentation &temp) = 0;
};

#endif // _RESOURCE_SERVER_H_ end
