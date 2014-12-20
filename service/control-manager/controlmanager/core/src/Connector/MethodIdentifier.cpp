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



#include "CMConnector.h"

namespace OC
{
    namespace Cm
    {
        namespace Connector
        {
            // TODO: Apply URL Encoding for Query parameters....

            MethodIdentifier::MethodIdentifier()
            {

            }

            MethodIdentifier::~MethodIdentifier()
            {

            }

            void
            MethodIdentifier::addParam(const std::string &name, const std::string &value)
            {
                m_queryParams.insert(std::pair<std::string, std::string>(name, value));
            }

            bool
            MethodIdentifier::getParam(const std::string &name, std::string &value)
            {
                if (m_queryParams.find(name) == m_queryParams.end())
                {
                    return false ;
                }

                value = this->m_queryParams[name];
                return true ;
            }

            const std::string &
            MethodIdentifier::getMethod() const
            {
                return m_method;
            }

            void
            MethodIdentifier::setMethod(const std::string &method)
            {
                m_method = method;
            }

            void
            MethodIdentifier::setQueryStringParams(const std::string &queryParams)
            {
                m_query_string = queryParams ;

                int firstIndex = 0 ;
                int lastIndex = 0 ;

                while (lastIndex != (int)std::string::npos)
                {
                    lastIndex = queryParams.find('&', firstIndex);

                    std::string token = queryParams.substr(firstIndex, lastIndex);

                    int index = token.find("=") ;

                    if ( index != (int)std::string::npos )
                    {
                        std::string paramName = token.substr(0, index);
                        std::string paramValue = token.substr(index + 1) ;

                        m_queryParams.insert(std::pair<std::string, std::string>(paramName, paramValue));
                    }

                    firstIndex = lastIndex + 1 ;
                }
            }

            std::string
            MethodIdentifier::getQueryString()
            {
                std::string retValue = "";

                for (std::map<std::string, std::string>::iterator iter = m_queryParams.begin();
                     iter != m_queryParams.end(); iter++)
                {
                    if (retValue.size() != 0)
                    {
                        retValue.append("&") ;
                    }

                    retValue.append(iter->first) ;
                    retValue.append("=") ;
                    retValue.append(iter->second) ;
                }

                return retValue ;
            }

            std::string MethodIdentifier::getDeviceAddress()
            {
                return m_device_address;
            }

            void MethodIdentifier::setDeviceAddress(const std::string &deviceAddress)
            {
                m_device_address = deviceAddress;
            }
        }
    }
}
