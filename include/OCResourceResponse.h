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

/// @file OCResourceResponse.h 

/// @brief  This file contains the declaration of classes and its members related to 
///         ResourceResponse.

#ifndef __OCRESOURCERESPONSE_H
#define __OCRESOURCERESPONSE_H

#include "OCApi.h"

namespace OC
{
    /**
    *   @brief  OCResourceResponse provides APIs to set the response details
    */
    class OCResourceResponse
    {
    public:
        typedef std::shared_ptr<OCResourceResponse> Ptr;

        /**
        *  OCResourceResponse Construtor
        *  @param responseHeaders response header information in a map
        *  @param eCode HTTP error code for this response.
        *  @param attributeMap reference to AttributeMap which contains the full attribute representation
        *  of the resource.
        */
        OCResourceResponse(HeadersMap& responseHeaders, int eCode, AttributeMap& attributeMap) : 
                m_responseHeaders(responseHeaders), m_HTTPErrorCode(eCode), m_attributeMap(attributeMap) {}

        /**
        *  Virtual destructor 
        */
        virtual ~OCResourceResponse(void);

        /**
        *  This API sets the response headers for the response
        *  @param responseHeaders std::string reference 
        */
        void setResponseHeaders(HeadersMap& responseHeaders) { m_responseHeaders = responseHeaders; }
        
        /**
        *  This API sets the HTTP error code for this response
        *  @param eCode HTTP error code to set
        */
        void setHTTPErrorCode(const int eCode) { m_HTTPErrorCode = eCode; }

        /**
        *  API to set the entire resource attribute representation
        *  @param attributeMap reference containing the name value pairs representing the resource's attributes
        */
        void setResourceRepresentation(AttributeMap& attributeMap) { m_attributeMap = attributeMap; }

    private:
        HeadersMap m_responseHeaders;
        int m_HTTPErrorCode; // TODO remove 'HTTP'. It can be any protocol and ISV need not know it 
        AttributeMap m_attributeMap;

    private:

        /** 
        * Get response headers
        */
        HeadersMap& getResponseHeaders() const; 
 
        /** 
        * Get HTTP status error code 
        */
        int getHTTPErrorCode() const; 

        /**
        * Get the resource attribute representation
        */
        AttributeMap& getResourceRepresentation() const; 
    };

} // namespace OC

#endif //__OCRESOURCERESPONSE_H
