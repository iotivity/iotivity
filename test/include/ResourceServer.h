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

/// @brief  This file contains the declaration of the resource server
///         class and its members.
#ifndef _RESOURCE_SERVER_H_
#define _RESOURCE_SERVER_H_

#include <chrono>
#include <iostream>
#include <pthread.h>
#include <stdexcept>
#include <memory>
#include <string>
#include <memory>
#include <unistd.h>

#include "OCPlatform.h"
#include "OCApi.h"
#include "ocstack.h"
#include "IotivityTest_Logger.h"

using namespace OC;

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
    std::string m_resourceInterface;
    OCResourceHandle m_resourceHandle;
    PlatformConfig m_platformConfig;
    bool m_isServerRunning;
    static bool m_isServerConstructed;

public:

    /**
     * Constructor for ResourceServer. Initializes server variables
     */
    ResourceServer(void);

    /**
     * virtual destructor for Resource Server.
     */
    virtual ~ResourceServer(void);

    /**
     * API for constructing the server with a PlatformConfig object.
     *
     * @param cfg - PlatformConfig object containing platform settings
     *
     * @return OCStackResult - returns OC_STACK_OK if successful to construct server,
     *                          else OC_STACK_ERROR
     *
     * NOTE: This API is for already available PlatformConfig
     * NOTE: PlatformConfig is defined in OCApi.h.
     */
    static OCStackResult constructServer(PlatformConfig& cfg);

    /**
     * API for constructing the server with a client IP & port.
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
//    virtual OCEntityHandlerResult entityHandler(std::shared_ptr< OCResourceRequest > request,
//            std::shared_ptr< OCResourceResponse > response);
    virtual OCEntityHandlerResult entityHandler(std::shared_ptr< OCResourceRequest > request);

    /**
     * API to get the representation model of the resource
     *
     * @param request - pointer to the OCResourceRequest type incoming request
     * @param response - pointer to the OCResourceResponse type outgoing response
     *
     * @return OCRepresentation, the representation class of the resource server
     *
     * NOTE: OCRepresentation is defined in OCApi.h
     */
    OCRepresentation getRepresentation(void);

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
    OCStackResult stopServer(void);

    /**
     * API for starting the server.
     *
     * @return OCStackResult - returns OC_STACK_OK if successful to start server,
     *                          else OC_STACK_ERROR
     *
     * NOTE: The server will stop when the main program exits
     */
    OCStackResult startServer(uint8_t resourceProperty = (uint8_t) OC_ACTIVE | OC_DISCOVERABLE | OC_OBSERVABLE);

    /**
     * API for getting the uri of the resource
     *
     * @return string - returns the uri of the resource
     *
     */
    string getUri(void);

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
    virtual void onResourceServerStarted(bool& isRegisteredForPresence, int& presenceInterval) = 0;

    /***
     * API to handle OBSERVE request
     *
     * @param[in] queryParamsMap - A map containing all query parameters
     * @param[out] response - response to the incoming request
     *
     * NOTE: This is a pure virtual function, thus it must be implemented by the child
     */
    virtual void handleObserveRequest(QueryParamsMap& queryParamsMap, std::shared_ptr< OCResourceRequest > request,
            std::shared_ptr< OCResourceResponse > response) = 0;

    /***
     * API to handle DELETE request
     *
     * @param[in] queryParamsMap - A map containing all query parameters
     * @param[out] response - response to the incoming request
     *
     * NOTE: This is a pure virtual function, thus it must be implemented by the child
     */
    virtual void handleDeleteRequest(QueryParamsMap& queryParamsMap,
            OCRepresentation incomingRepresentation,
            std::shared_ptr< OCResourceResponse > response) = 0;

    /***
     * API to handle POST request
     *
     * @param[in] queryParamsMap - A map containing all query parameters
     * @param[out] response - response to the incoming request
     *
     * NOTE: This is a pure virtual function, thus it must be implemented by the child
     */
    virtual void handlePostRequest(QueryParamsMap& queryParamsMap,
            OCRepresentation incomingRepresentation,
            std::shared_ptr< OCResourceResponse > response) = 0;

    /***
     * API to handle GET request
     *
     * @param[in] queryParamsMap - A map containing all query parameters
     * @param[out] response - response to the incoming request
     *
     * NOTE: This is a pure virtual function, thus it must be implemented by the child
     */
    virtual void handleGetRequest(QueryParamsMap& queryParamsMap,
            std::shared_ptr< OCResourceResponse > response) = 0;

    /***
     * API to handle PUT request
     *
     * @param[in] queryParamsMap - A map containing all query parameters
     * @param[out] response - response to the incoming request
     *
     * NOTE: This is a pure virtual function, thus it must be implemented by the child
     */
    virtual void handlePutRequest(QueryParamsMap& queryParamsMap,
            OCRepresentation incomingRepresentation,
            std::shared_ptr< OCResourceResponse > response) = 0;

    /***
     * API to handle INIT request
     *
     * @param[in] queryParamsMap - A map containing all query parameters
     * @param[out] response - response to the incoming request
     *
     * NOTE: This is a pure virtual function, thus it must be implemented by the child
     */
    virtual void handleInitRequest(QueryParamsMap& queryParamsMap,
            std::shared_ptr< OCResourceResponse > response) = 0;

    /***
     * API to get the attribute list of the resource server
     *
     * @param[in] temp - epresentation of the resource server
     *
     * @return OCRepresentation - representation of the resource server
     */
    virtual OCRepresentation getResourceRepresentation(OCRepresentation& temp)=0;
};

#endif // _RESOURCE_SERVER_H_ end
