//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
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

/// @file OCPlatform.h 

/// @brief  This file contains the declaration of classes and its members related to 
///         OCPlatform.

#ifndef __OCPLATFORM_H
#define __OCPLATFORM_H

#include <map>

#include "OCApi.h"
#include "OCResource.h"
#include "OCPlatformHandler.h"
#include "WrapperFactory.h"
#include "OCResourceRequest.h"
#include "OCResourceResponse.h"

namespace OC
{
    /**
    *   @brief  Both server and client must initialize the core platform by instantiating OCPlatform.
    *           On successful initialization, an instance of the OCPlatform is returned.  
    *           APIs in OCPlatform provide mechanism to register a resource and host the resource
    *           on the server, find resources on the network etc.
    */
    class OCPlatform
    {
    public:
        /**
        * Constructor for OCPlatform. Constructs a new OCPlatform from a given PlatformConfig with 
        * appropriate fields
        * @param config PlatformConfig struct which has details such as modeType (server/client/both),
        *               in-proc/out-of-proc etc. 
        */
        OCPlatform(const PlatformConfig& config);

        /**
        * Virtual destructor
        */
        virtual ~OCPlatform(void);

        /**
        * API for Service and Resource Discovery. 
        * NOTE: This API applies to client side only.
        * 
        * @param host - Host IP Address of a service to direct resource discovery query. If null or
        *        empty, performs multicast resource discovery query  
        * @param resourceURI - name of the resource. If null or empty, performs search for all resource names
        * @param handler - Handles callbacks, success states and failure states.
        * 
        *        Four modes of discovery defined as follows: 
        *        (NULL/Empty, NULL/Empty) - Performs ALL service discovery AND ALL resource discovery. 
        *        (NULL/Empty, Not Empty) - Performs query for a filtered/scoped/particular resource(s) 
        *                                  from ALL services. 
        *        (Not Empty, NULL/Empty) - Performs ALL resource discovery on a particular service. 
        *        (Not Empty, Not Empty) - Performs query for a filtered/scoped/particular resource(s) 
        *                                  from a particular service.
        * NOTE: First parameter 'host' currently represents an IP address. This will change in future
        * and will refer to endpoint interface so that we can refer to other transports such as BTH etc.
        * Example:
        * 
        */
        void findResource(const std::string& host, const std::string& resourceURI, 
            std::function<void(OCResource::Ptr)> resourceHandler);

        /**
        * This API registers a resource with the server
        * NOTE: This API applies to server side only.
        * 
        * @param resourceURI - The URI of the resource. Example: "a/light". See NOTE below
        * @param resourceTypeName - The resource type. Example: "light"
        * @param resourceInterface - The resource interface (whether it is collection etc). 
        * @param entityHandler - entity handler callback. 
        * @param resourceFlag - flag to indicate discoverable/observable etc.
        * 
        * NOTE: "a/light" is a relative URI. 
        * Above relative URI will be prepended (by core) with a host IP + namespace "oc"
        * Therefore, fully qualified URI format would be //HostIP-Address/namespace/relativeURI"
        * Example, a relative URI: 'a/light' will result in a fully qualified URI: //134.134.161.33/oc/a/light"
        * First parameter can take a relative URI and core will take care of preparing the fully qualified URI 
        * OR
        * first paramter can take fully qualified URI and core will take that as is for further operations
        */
        void registerResource(std::string& resourceURI, const std::string& resourceTypeName, const std::string& resourceInterface, 
            std::function<void(const OCResourceRequest::Ptr, const OCResourceResponse::Ptr)> entityHandler, ResourceFlag resourceFlag); 

    private:
        std::unique_ptr<WrapperFactory> m_WrapperInstance;
        IServerWrapper::Ptr m_server;
        IClientWrapper::Ptr m_client;

        /**
        *  Private function to initalize the platfrom 
        */
        void init(const PlatformConfig& config);

        /**
        *  Private function cleanup the platform
        */
        void cleanup();
    };
}

#endif //__OCPLATFORM_H
