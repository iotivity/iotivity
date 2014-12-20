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

#ifndef _METHODIDENTIFIER_H
#define _METHODIDENTIFIER_H

#include "string"
#include "map"

namespace OC
{
    namespace Cm
    {
        namespace Connector
        {
            /**
             * @class       MethodIdentifier
             * @brief       This class holds the information related to a method to be executed on a resource
             */
            class MethodIdentifier
            {

                public:

                    /**
                     * MethodIdentifier - Default constructor of this class
                     */
                    MethodIdentifier();

                    /**
                     * ~MethodIdentifier -Default destructor of this class
                     */
                    virtual ~MethodIdentifier();

                    /**
                     * getMethod - returns the method
                     *
                     * @return type: const std::string& - method of the request.
                     */
                    const std::string &getMethod() const;

                    /**
                     *setMethod sets method
                     *
                     * @param The method to use
                     * @return -none
                     */
                    void setMethod(const std::string &method);

                    /**
                     * addParam adds a parameter into the request being send.
                     *
                     * @param name name of the parameter
                     * @param value value of the parameter
                     * @return -none
                     */
                    void addParam(const std::string &name, const std::string &value);

                    /**
                     * This API returns value of a parameter from the request.
                     *
                     * @param name      Represents name of the parameter
                     * @param value     Represents the value of the parameter(output)
                     *
                     * @return  type:bool true in case of success, false in case of error
                     */
                    bool getParam(const std::string &name, std::string &value);

                    /**
                     * setQueryStringParams sets a query string. This class can parse query string
                     *
                     * @param queryParams name of the parameter
                     * @return -none
                     */
                    void setQueryStringParams(const std::string &queryParams);

                    /**
                     * getQueryString returns query string from the request
                     *
                     *@param -none
                     *@return type:string query string
                     */
                    std::string getQueryString();

                    /**
                     * getDeviceAddress returns query device Address.
                     * @param -none
                     *
                     * @return type:string Device Address
                     */
                    std::string getDeviceAddress();

                    /**
                     * setDeviceAddress sets a deviceAddress.
                     *
                     * @param deviceAddress Device address.
                     * @return -none
                     */
                    void setDeviceAddress(const std::string &deviceAddress);

                    std::map<std::string, std::string> m_queryParams; /**< Represents map to hold query string parameters */

                private:

                    std::string m_method; /**< Represents the method of request */
                    std::string m_device_address; /**< Represents the device address */
                    std::string m_query_string; /**< Represents the query string parameter */

            };
        }
    }
}

#endif /* _METHODIDENTIFIER_H */
