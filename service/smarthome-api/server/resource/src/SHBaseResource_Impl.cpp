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
#include <CommonApi.h>
#include <SHBaseResource.h>
#include <SHBaseResource_Impl.h>
#include <EntityHandlerWrapper.h>
#include <EntityHandlerContext.h>
#include <ResourceQuery.h>
#include "ocstack.h"
#include "logger.h"

#define TAG "SH_SERVER_BASE_RESOURCE"

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            SHBaseResource_Impl::SHBaseResource_Impl(const std::string& uri, 
                                                     const std::string& type)
             : m_resourceHandle(NULL),
               m_resourceUri(uri),
               m_context(NULL),
               m_delegate(NULL)
            {
                m_resourceType.push_back(type);
                m_resourceInterface.push_back(INTERFACE::BASELINE);
                registerResource();
            }

            SHBaseResource_Impl::SHBaseResource_Impl(const std::string& uri,
                                                     const std::string& type,
                                                     const std::string& interface)
             : m_resourceHandle(NULL),
               m_resourceUri(uri),
               m_context(NULL),
               m_delegate(NULL)
            {
                m_resourceType.push_back(type);
                m_resourceInterface.push_back(interface);
                registerResource();
            }

            SHBaseResource_Impl::SHBaseResource_Impl(const std::string& uri,
                                                     const std::list<std::string>& types)
             : m_resourceHandle(NULL),
               m_resourceType(types),
               m_resourceUri(uri),
               m_context(NULL),
               m_delegate(NULL)
            {
                m_resourceInterface.push_back(INTERFACE::BASELINE);
                registerResource();
            }

            SHBaseResource_Impl::SHBaseResource_Impl(const std::string& uri,
                                                     const std::list<std::string>& types,
                                                     const std::list<std::string>& interfaces)
             : m_resourceHandle(NULL),
               m_resourceUri(uri),
               m_resourceType(types),
               m_resourceInterface(interfaces),
               m_context(NULL),
               m_delegate(NULL)
            {
                registerResource();
            }

            SHBaseResource_Impl::~SHBaseResource_Impl()
            {
                deregisterResource();
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
                this->bindTypesToResource(types);
                std::list<std::string>::const_iterator iter = types.begin();
                while (iter != types.end())
                {
                    m_resourceType.push_back(*iter);
                    ++iter;
                }
            }

            std::list<std::string> SHBaseResource_Impl::getInterfaces() const
            {
                return m_resourceInterface;
            }

            void SHBaseResource_Impl::setInterfaces(const std::list<std::string>& interfaces)
            {
                this->bindInterfacesToResource(interfaces);
                std::list<std::string>::const_iterator iter = interfaces.begin();
                while (iter != interfaces.end())
                {
                    m_resourceInterface.push_back(*iter);
                    ++iter;
                }
            }

            void SHBaseResource_Impl::setPropertyBundle(const PropertyBundle& bundle)
            {
                OIC_LOG(DEBUG, TAG, "Entered setPropertyBundle");
                m_propertyBundle = bundle;
            }

            const PropertyBundle& SHBaseResource_Impl::getPropertyBundle() const
            {
                OIC_LOG(DEBUG, TAG, "Entered getPropertyBundle");
                return m_propertyBundle;
            }

            bool SHBaseResource_Impl::sendResponse(RequestId requestId,
                                                   const PropertyBundle& bundle)
            {
                OIC_LOG(DEBUG, TAG, "Entered sendResponse");
                return true;
            }

            bool SHBaseResource_Impl::sendErrorResponse(RequestId requestId,
                                                        const PropertyBundle& bundle)
            {
                OIC_LOG(DEBUG, TAG, "Entered sendErrorResponse");
                return true;
            }

            void SHBaseResource_Impl::setDelegate(SHBaseResourceDelegate *delegate)
            {
                OIC_LOG(DEBUG, TAG, "Entered setDelegate");
                m_delegate = delegate;
            }

            void SHBaseResource_Impl::registerResource()
            {
                if (!m_context)
                {
                    m_context = new EntityHandlerContext(this);
                }

                // TODO: we should provide a public api to set resource property.
                m_resourceProperty = OC_DISCOVERABLE;

                // Bind the first element of resource type/interface.
                // Only one resource type/interface can be set at a time.
                std::list<std::string>::iterator typeIter = m_resourceType.begin();
                std::list<std::string>::iterator ifIter = m_resourceInterface.begin();
                OCStackResult result = OCCreateResource(&m_resourceHandle,
                                                        typeIter->c_str(),
                                                        ifIter->c_str(),
                                                        m_resourceUri.c_str(),
                                                        EntityHandlerWrapper::ResourceEntityHandler,
                                                        m_context, m_resourceProperty);
                if (result != OC_STACK_OK)
                {
                    throw CommonException("Exception on OCCreateResource");
                }

                // Skip the first element. It was already bound when OCCreateResource is called.
                typeIter = m_resourceType.begin();
                ++typeIter;
                for (; typeIter != m_resourceType.end(); ++typeIter)
                {
                    result = OCBindResourceInterfaceToResource(m_resourceHandle,
                                                               typeIter->c_str());
                    if (result != OC_STACK_OK)
                    {
                        throw CommonException("Exception on OCBindResourceInterfaceToResource");
                    }
                }

                // Skip the first element. It was already bound when OCCreateResource is called.
                ifIter = m_resourceInterface.begin();
                ++ifIter;
                for (; ifIter != m_resourceInterface.end(); ++ifIter)
                {
                    result = OCBindResourceInterfaceToResource(m_resourceHandle,
                                                               ifIter->c_str());
                    if (result != OC_STACK_OK)
                    {
                        throw CommonException("Exception on OCBindResourceInterfaceToResource");
                    }
                }
            }

            void SHBaseResource_Impl::deregisterResource()
            {
                if (m_context)
                {
                    delete m_context;
                }

                OCStackResult result = OCDeleteResource(m_resourceHandle);
                if (result != OC_STACK_OK)
                {
                    throw CommonException("Exception on OCDeleteResource");
                }
            }

            void SHBaseResource_Impl::bindTypesToResource(std::list<std::string> types)
            {
                std::list<std::string>::const_iterator iter;
                for (iter = types.begin(); iter != types.end(); ++iter)
                {
                    OCStackResult result = OCBindResourceTypeToResource(m_resourceHandle,
                                                                        iter->c_str());
                    if (result != OC_STACK_OK)
                    {
                        throw CommonException("Exception on OCBindResourceTypeToResource");
                    }
                }
            }

            void SHBaseResource_Impl::bindInterfacesToResource(std::list<std::string> interfaces)
            {
                std::list<std::string>::const_iterator iter;
                for (iter = interfaces.begin(); iter != interfaces.end(); ++iter)
                {
                    OCStackResult result = OCBindResourceInterfaceToResource(m_resourceHandle,
                                                                             iter->c_str());
                    if (result != OC_STACK_OK)
                    {
                        throw CommonException("Exception on OCBindResourceInterfaceToResource");
                    }
                }
            }

            ResultCode SHBaseResource_Impl::handleGetRequest(RequestId requestId,
                                                             const ResourceQuery& query) const
            {
                OIC_LOG(DEBUG, TAG, "Entered handleGetRequest");

                if (!m_delegate)
                {
                    return FAIL;
                }

                // Call user onGet delegate callback.
                return m_delegate->onGet(requestId, query);
            }

            ResultCode SHBaseResource_Impl::handleSetRequest(RequestId requestId,
                                                             const PropertyBundle& bundle,
                                                             const ResourceQuery& query) const
            {
                OIC_LOG(DEBUG, TAG, "Entered handleSetRequest");

                if (!m_delegate)
                {
                    return FAIL;
                }

                // Call user onSet delegate callback.
                return m_delegate->onSet(requestId, bundle, query);
            }
        }
    }
}
