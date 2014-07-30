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

/// @file OCResourceRequest.h 

/// @brief  This file contains the declaration of classes and its members related to 
///         ResourceRequest.

#ifndef __OCRESOURCEREQUEST_H
#define __OCRESOURCEREQUEST_H

#include "OCApi.h"

namespace OC
{
    /**
    *   @brief  OCResourceRequest provides APIs to extract details from a request URI
    */
    class OCResourceRequest
    {
    public:
        typedef std::shared_ptr<OCResourceRequest> Ptr;

        /**
        *  Virtual destructor 
        */
        virtual ~OCResourceRequest(void);

        /**
        *  Retrieves the type of request string for the entity handler function to operate
        *  @return std::string request type. This could be 'GET'/'PUT'/'POST'/'DELETE'
        */
        std::string getRequestType() const {return m_requestType;}
        
        /**
        *  Retrieves the payload from the request.
        *  NOTE: Query parameters are retrived in a separate API (see 'getRequestPayload')
        *  @return std:string request payload
        */
        std::string getRequestPayload() const {return m_requestPayload;}

        /**
        *  Retrieves the query parameters from the request
        *  @return std::string query parameters in the request
        */
        const QueryParamsMap& getQueryParameters() const {return m_queryParameters;}

        /**
        *  Retrieves the request handler flag type. This can be either INIT flag or REQUEST flag 
        *  OBSERVE flag. 
        *  NOTE: 
        *  INIT indicates that the vendor's entity handler should go and perform initialization operations
        *  REQUEST indicates that it is a request of certain type (GET/PUT/POST/DELETE) and entity handler needs to perform 
        *  corresponding operations
        *  OBSERVE indicates that the request is of type Observe and entity handler needs to perform corresponding operations
        *  @return std::string type of request flag
        */
        RequestHandlerFlag getRequestHandlerFlag() const {return m_requestHandlerFlag;}

        /**
        *  Provides the entire resource attribute representation
        *  @return std::map AttributeMap reference containing the name value pairs representing the resource's attributes
        */
        const AttributeMap& getResourceRepresentation() const {return m_attributeMap;}

    private:
        std::string m_requestType;
        std::string m_requestPayload;
        QueryParamsMap m_queryParameters;
        RequestHandlerFlag m_requestHandlerFlag;
        AttributeMap m_attributeMap;

    private:
        void setRequestType(const std::string& requestType);
        void setRequestPayload(const std::string& requestPayload);
        void setQueryParams(QueryParamsMap& queryParams);
        void setRequestHandlerFlag(RequestHandlerFlag requestHandlerFlag);
        void setResourceRepresentation(AttributeMap& attributeMap);
    };

} // namespace OC

#endif //__OCRESOURCEREQUEST_H
