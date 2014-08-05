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
#include <IServerWrapper.h>
#include <ocstack.h>

using namespace std;

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
        *  Default destructor 
        */
        OCResourceResponse() {}

        /**
        *  Virtual destructor 
        */
        virtual ~OCResourceResponse(void) {}

        /**
        *  This API sets the error code for this response
        *  @param eCode error code to set
        */
        void setErrorCode(const int eCode) { m_errorCode = eCode; }

        /**
        *  API to set the entire resource attribute representation
        *  @param attributeMap reference containing the name value pairs representing the resource's attributes
        */
        void setResourceRepresentation(AttributeMap& attributes) { 

            // TODO To be refactored
            ostringstream payload;

            payload << "{\"oc\":{\"payload\":{";

            for(AttributeMap::const_iterator itr = attributes.begin(); itr!= attributes.end(); ++ itr)
            {
                if(itr != attributes.begin())
                {
                    payload << ',';
                }
                // cout << itr->first << ":" <, itr->second.front() << endl;
                payload << "\""<<itr->first<<"\":" << itr->second.front();
            }
            payload << "}}}";

            m_payload = payload.str();
        }

    private:
        std::string m_payload;
        int m_errorCode;

    // TODO only stack should have visibility and apps should not
    public:

        /** 
        * Get error code 
        */
        int getErrorCode() const; 

        /**
        * Get the resource attribute representation
        */
        AttributeMap& getResourceRepresentation() const; 

        // TODO This should go away & just use getResourceRepresentation 
        std::string getPayload()
        {
            return m_payload;
        }
    };

} // namespace OC

#endif //__OCRESOURCERESPONSE_H
