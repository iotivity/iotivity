/* ****************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/
#include <iostream>
#include <SHBaseResource_Impl.h>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            SHBaseResource_Impl::SHBaseResource_Impl(const std::string& uri)
             : m_resourceUri(uri), m_delegate(NULL)
            {
            }

            SHBaseResource_Impl::SHBaseResource_Impl(const std::string& uri,
                                                     const std::list<std::string>& types,
                                                     const std::list<std::string>& interfaces)
             : m_resourceUri(uri),
               m_resourceType(types),
               m_resourceInterface(interfaces),
               m_delegate(NULL)
            {
            }

            SHBaseResource_Impl::~SHBaseResource_Impl()
            {
                // TODO Auto-generated destructor stub
            }

            std::string SHBaseResource_Impl::getUri() const
            {
                return m_resourceUri;
            }

            void SHBaseResource_Impl::setUri(const std::string& uri)
            {
                m_resourceUri = uri;
            }

            std::list<std::string> SHBaseResource_Impl::getTypes() const
            {
                return m_resourceType;
            }

            void SHBaseResource_Impl::setTypes(const std::list<std::string>& types)
            {
                m_resourceType = types;
            }

            std::list<std::string> SHBaseResource_Impl::getInterfaces() const
            {
                return m_resourceInterface;
            }

            void SHBaseResource_Impl::setInterfaces(const std::list<std::string>& interfaces)
            {
                m_resourceInterface = interfaces;
            }

            void SHBaseResource_Impl::setPropertyBundle(const PropertyBundle& bundle)
            {
                std::cout << "[Resource] setPropertyBundle" << std::endl;
                m_propertyBundle = bundle;
            }

            const PropertyBundle& SHBaseResource_Impl::getPropertyBundle() const
            {
                std::cout << "[Resource] getPropertyBundle" << std::endl;
                return m_propertyBundle;
            }

            bool SHBaseResource_Impl::sendResponse(int requestId, const PropertyBundle& bundle)
            {
                std::cout << "[Resource] sendResponse" << std::endl;
                return true;
            }

            bool SHBaseResource_Impl::sendErrorResponse(int requestId, const PropertyBundle& bundle)
            {
                std::cout << "[Resource] sendErrorResponse" << std::endl;
                return true;
            }

            void SHBaseResource_Impl::setDelegate(SHBaseResourceDelegate *delegate)
            {
                std::cout << "[Resource] setDelegate" << std::endl;
                m_delegate = delegate;
            }
        }
    }
}
