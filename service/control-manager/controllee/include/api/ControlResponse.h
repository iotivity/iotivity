//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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

#if !defined(_ControlResponse_H)
#define _ControlResponse_H

#include "HeadersList.h"
#include "OCPlatform.h"



namespace OC
{
    namespace Cm
    {
        namespace Connector
        {
            /**
             * @class       ControlResponse
             * @brief       This class holds the details of Control Manager response
             * @since       1.0
             */
            class  ControlResponse
            {
                public:

                    std::string m_payloadType;  /**< Represents content type of the payload */
                    std::string m_payload;      /**< Represents payload of the response */
                    int m_payloadSize;          /**< Represents size of the payload */
                    int m_response_code;        /**< Represents HTTP Response code */
                    HeadersList     m_headers ;                 /**< Represents response headers */
                    std::string m_headers_str ;                 /**< Represents the Control Manager Response Header */
                    int m_headers_str_length ;                  /**< Represents length of the Control Manager Response Header */
                    // OIC TODO: Remove later
                    // OC::AttributeMap m_attributeMap ;            /** <Represents the OC layer payload > */
                    OC::OCRepresentation m_representation ;     /** <Represents the Resource of OC layer > */

                    /**
                     * ControlResponse - Default constructor
                     */
                    ControlResponse();

                    /**
                     * getProtocolVersion - retrieves Control Manager Version of this response
                     *
                     * @param version string which represents Control Manager protocol version as output
                     *
                     * @return  -bool- true if response is sent successfully
                     *                 false in case of any error
                     */
                    bool getProtocolVersion(std::string &version);

                    /**
                     * setProtocolVersion configures Control Manager Version of this response
                     *
                     * @param version pointer to the string which represents Control Manager protocol version
                     *
                     * @return  -bool- true if response is sent successfully
                     *                  false in case input parameter is either NULL or empty string
                     */
                    bool setProtocolVersion(const char *version);
            };
        }
    }
}

#endif  //_ControlResponse_H
