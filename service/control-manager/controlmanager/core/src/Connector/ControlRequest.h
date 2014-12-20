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



#if !defined(_ControlRequest_H)
#define _ControlRequest_H

#include "ResourceIdentifier.h"
#include "MethodIdentifier.h"
#include "Connector/HeadersList.h"

#include "CMConfig.h"
#include "map"
#include "list"

#include "OCPlatform.h"

namespace OC
{
    namespace Cm
    {
        namespace Connector
        {
            /**
             * @class       ControlRequest
             * @brief       This class holds the details of Control Manager request
             * @since       1.0
             */
            class CMAPI ControlRequest
            {

                public:
                    /**
                     * Default constructor
                     */
                    ControlRequest();

                    /**
                     * retrieves Control Manager Version of this request
                     *
                     * @param[out] version string which represents Control Manager protocol version
                     *
                     * @return  returns @c true if request is sent successfully @n
                     *                  @c false in case of any error
                     */
                    bool getProtocolVersion(std::string &version);

                    /**
                     * configures Control Manager Version of this request
                     *
                     * @param[in] version pointer to the string which represents Control Manager protocol version
                     *
                     * @return  returns @c true if request is sent successfully @n
                     *                  @c false in case input parameter is either NULL or empty string
                     */
                    bool setProtocolVersion(const char *version);

                    std::string m_payloadType;  /**< Represents content type of the payload */

                    std::string m_payload;      /**< Represents payload of the request */

                    int m_payloadSize;          /**< Represents size of the payload */

                    ResourceIdentifier m_resourceIdentifier;    /**< Represents resource Identifier */

                    MethodIdentifier m_methodIdentifier;    /**< Represents method identifier */

                    HeadersList     m_headers ;             /**< Represents request Headers */

                    std::map<std::string, std::string>  m_template_parameters
                    ;     /**< Represents Template parameters of request resource path */

                    std::list<std::string> m_accept_header_values ;     /**< Represents list of accept header values */

                    std::string m_headers_str ; /**< Represents the Control Manager Request Header */

                    int m_headers_str_length ;  /**< Represents length of the Control Manager Request Header */

                    std::string m_payload_pretty_print; /**< Represents the string version of complete request */

                    OC::OCRepresentation m_representation ;     /** <Represents the Resource of OC layer > */

            };
        }
    }
}

#endif  //_ControlRequest_H
