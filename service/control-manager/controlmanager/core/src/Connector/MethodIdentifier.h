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
#include "CMConfig.h"

namespace OC
{
    namespace Cm
    {
        namespace Connector
        {
            /**
             * @class       MethodIdentifier
             * @brief       This class holds the information related to a method to be executed on a resource
             * @since       1.0
             */
            class CMAPI MethodIdentifier
            {

                public:

                    /**
                     * Default constructor of this class
                     */
                    MethodIdentifier();

                    /**
                     * Default destructor of this class
                     */
                    virtual ~MethodIdentifier();

                    /**
                     * This API returns the method
                     *
                     * @return method of the request.
                     */
                    const std::string &getMethod() const;

                    /**
                     * This API sets method
                     *
                     * @param[in] method method
                     */
                    void setMethod(const std::string &method);


                    /**
                     * This API adds a parameter into the request being send.
                     *
                     * @param[in] name name of the parameter
                     * @param[in] value value of the parameter
                     */
                    void addParam(const std::string &name, const std::string &value);

                    /**
                     * This API returns value of a parameter from the request.
                     *
                     * @param[in] name      Represents name of the parameter
                     * @param[out] value    Represents the value of the parameter
                     *
                     * @return  Returns @c true in case of success @n
                     *                  @c false in case of error
                     */
                    bool getParam(const std::string &name, std::string &value) ;


                    /**
                     * This API sets a query string. This class can parse query string
                     *
                     * @param[in] queryParams name of the parameter
                     */
                    void setQueryStringParams(const std::string &queryParams) ;

                    /**
                     * This API returns query string from the request
                     *
                     * @return query string
                     */
                    std::string getQueryString() ;

                    /**
                    * This API returns query device Address.
                    *
                    * @return Device Address
                    */
                    std::string getDeviceAddress() ;

                    /**
                     * This API sets a deviceAddress.
                     *
                     * @param[in] deviceAddress Device address.
                     */
                    void setDeviceAddress(const std::string &deviceAddress) ;

                    std::map<std::string, std::string>
                    m_queryParams; /**< Represents map to hold query string parameters */

                private:

                    std::string m_method;           /**< Represents the method of request */

                    std::string m_device_address;   /**< Represents the device address */

                    std::string m_query_string;     /**< Represents the query string parameter */

            };
        }
    }
}

#endif /* _METHODIDENTIFIER_H */
