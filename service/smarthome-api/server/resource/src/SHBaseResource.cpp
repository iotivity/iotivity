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
#include <SHBaseResource.h>
#include <SHBaseResource_Impl.h>
#include <ResourceQuery.h>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            SHBaseResource::SHBaseResource(const std::string& uri, const std::string& type)
            {
                m_resourceImpl = new SHBaseResource_Impl(uri, type);
            }

            SHBaseResource::SHBaseResource(const std::string& uri, const std::string& type,
                    const std::string& interface)
            {
                m_resourceImpl = new SHBaseResource_Impl(uri, type, interface);
            }

            SHBaseResource::SHBaseResource(const std::string& uri,
                    const std::list<std::string>& types)
            {
                m_resourceImpl = new SHBaseResource_Impl(uri, types);
            }

            SHBaseResource::SHBaseResource(const std::string& uri,
                    const std::list<std::string>& types, const std::list<std::string>& interfaces)
            {
            }

            SHBaseResource::SHBaseResource(const std::string& uri, const std::string& type,
                    uint8_t properties)
            {
                m_resourceImpl = new SHBaseResource_Impl(uri, type, properties);
            }

            SHBaseResource::SHBaseResource(const std::string& uri, const std::string& type,
                    std::string& interface, uint8_t properties)
            {
                m_resourceImpl = new SHBaseResource_Impl(uri, type, interface, properties);
            }

            SHBaseResource::SHBaseResource(const std::string& uri,
                    const std::list<std::string>& types, uint8_t properties)
            {
                m_resourceImpl = new SHBaseResource_Impl(uri, types, properties);
            }

            SHBaseResource::SHBaseResource(const std::string& uri,
                    const std::list<std::string>& types, const std::list<std::string>& interfaces,
                    uint8_t properties)
            {
                m_resourceImpl = new SHBaseResource_Impl(uri, types, interfaces, properties);
            }

            SHBaseResource::~SHBaseResource()
            {
                delete m_resourceImpl;
            }

            std::string SHBaseResource::getUri() const
            {
                return m_resourceImpl->getUri();
            }

            void SHBaseResource::setUri(const std::string& uri)
            {
                m_resourceImpl->setUri(uri);
            }

            std::list<std::string> SHBaseResource::getTypes() const
            {
                return m_resourceImpl->getTypes();
            }

            void SHBaseResource::setTypes(const std::list<std::string>& types)
            {
                m_resourceImpl->setTypes(types);
            }

            std::list<std::string> SHBaseResource::getInterfaces() const
            {
                return m_resourceImpl->getInterfaces();
            }

            void SHBaseResource::setInterfaces(const std::list<std::string>& interfaces)
            {
                m_resourceImpl->setInterfaces(interfaces);
            }

            void SHBaseResource::setPropertyBundle(const PropertyBundle& bundle)
            {
                m_resourceImpl->setPropertyBundle(bundle);
            }

            const PropertyBundle& SHBaseResource::getPropertyBundle() const
            {
                return m_resourceImpl->getPropertyBundle();
            }

            bool SHBaseResource::sendResponse(RequestId requestId, const PropertyBundle& bundle)
            {
                return m_resourceImpl->sendResponse(requestId, bundle);
            }

            bool SHBaseResource::sendErrorResponse(RequestId requestId,
                    const PropertyBundle& bundle)
            {
                return m_resourceImpl->sendErrorResponse(requestId, bundle);
            }

            void SHBaseResource::setDelegate(SHBaseResourceDelegate *delegate)
            {
                m_resourceImpl->setDelegate(delegate);
            }
        }
    }
}
