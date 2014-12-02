//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
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
#include <IServerWrapper.h>
#include <ocstack.h>
#include <OCRepresentation.h>

using namespace std;


void (processResourceResponse)(OCEntityHandlerFlag,
                                 OCEntityHandlerRequest*,
                                 std::shared_ptr<OC::OCResourceResponse> pResponse);


namespace OC
{
    /**
    *   @brief  OCResourceResponse provides APIs to set the response details
    */
    class OCResourceResponse
    {
    public:
        typedef std::shared_ptr<OCResourceResponse> Ptr;

        OCResourceResponse()
        {}

        virtual ~OCResourceResponse(void) {}

        /**
        *  This API sets the error code for this response
        *  @param eCode error code to set
        */
        void setErrorCode(const int eCode) { m_errorCode = eCode; }

        /**
        *  gets new resource uri
        *  @return std::string new resource uri
        */
        std::string getNewResourceUri(void)
        {
            return m_newResourceUri;
        }

        /**
        *  sets new resource uri
        *  @param newResourceUri specifies the resource uri of the resource created
        */
        void setNewResourceUri(const std::string newResourceUri)
        {
            m_newResourceUri = newResourceUri;
        }

        /**
        * This API allows to set headerOptions in the response
        * @param headerOptions HeaderOptions vector consisting of OCHeaderOption objects
        */
        void setHeaderOptions(const HeaderOptions& headerOptions)
        {
            m_headerOptions = headerOptions;
        }

        /**
        *  API to set the entire resource attribute representation
        *  @param attributeMap reference containing the name value pairs representing
        *         the resource's attributes
        *  @param interface specifies the interface
        */
        void setResourceRepresentation(OCRepresentation& rep, std::string interface) {
            m_interface = interface;
            m_representation = rep;
        }

        /**
        *  API to set the entire resource attribute representation
        *  @param attributeMap rvalue reference containing the name value pairs representing
        *         the resource's attributes
        *  @param interface specifies the interface
        */
        void setResourceRepresentation(OCRepresentation&& rep, std::string interface) {
            setResourceRepresentation(rep, interface);
        }

        /**
        *  API to set the entire resource attribute representation
        *  @param attributeMap reference containing the name value pairs representing the resource's
        *  attributes
        */
        void setResourceRepresentation(OCRepresentation& rep) {
            // Call the default
            m_interface = DEFAULT_INTERFACE;
            m_representation = rep;
        }

        /**
        *  API to set the entire resource attribute representation
        *  @param attributeMap rvalue reference containing the name value pairs representing the
        *  resource's attributes
        */
        void setResourceRepresentation(OCRepresentation&& rep) {
            // Call the above function
            setResourceRepresentation(rep);
        }
    private:
        std::string m_newResourceUri;
        int m_errorCode;
        HeaderOptions m_headerOptions;
        std::string m_interface;
        OCRepresentation m_representation;
    private:
        friend void (::processResourceResponse)(OCEntityHandlerFlag,
                            OCEntityHandlerRequest*,
                            std::shared_ptr<OC::OCResourceResponse> pResponse);
        std::string getPayload() const
        {
            MessageContainer inf;
            OCRepresentation first(m_representation);

            if(m_interface==LINK_INTERFACE)
            {
                first.setInterfaceType(InterfaceType::LinkParent);
            }
            else if(m_interface==BATCH_INTERFACE)
            {
                first.setInterfaceType(InterfaceType::BatchParent);
            }
            else
            {
                first.setInterfaceType(InterfaceType::DefaultParent);
            }

            inf.addRepresentation(first);

            for(const OCRepresentation& rep : m_representation.getChildren())
            {
                OCRepresentation cur(rep);

                if(m_interface==LINK_INTERFACE)
                {
                    cur.setInterfaceType(InterfaceType::LinkChild);
                }
                else if(m_interface==BATCH_INTERFACE)
                {
                    cur.setInterfaceType(InterfaceType::BatchChild);
                }
                else
                {
                    cur.setInterfaceType(InterfaceType::DefaultChild);
                }

                inf.addRepresentation(cur);

            }

            return inf.getJSONRepresentation(OCInfoFormat::ExcludeOC);
        }
    public:

        /**
        * Get error code
        */
        int getErrorCode() const;

        /**
         * Get the Response Representation
         */
        const OCRepresentation& getResourceRepresentation() const
        {
            return m_representation;
        }
        /**
        * This API allows to retrieve headerOptions from a response
        */
        const HeaderOptions& getHeaderOptions() const
        {
            return m_headerOptions;
        }
    };

} // namespace OC

#endif //__OCRESOURCERESPONSE_H
