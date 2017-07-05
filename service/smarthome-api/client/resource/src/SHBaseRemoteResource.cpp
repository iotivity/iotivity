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
#include <SHBaseRemoteResource.h>
#include <SHBaseRemoteResource_Impl.h>
#include <PropertyBundle.h>
#include <ResourceQuery.h>
#include <CommonException.h>
#include <SHBaseRemoteResourceBuilder.h>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            SHBaseRemoteResource::SHBaseRemoteResource()
            {
                m_remoteResourceImpl = new SHBaseRemoteResource_Impl;
            }

            SHBaseRemoteResource::~SHBaseRemoteResource()
            {
                delete m_remoteResourceImpl;
            }

            std::string SHBaseRemoteResource::getUri() const
            {
                return m_remoteResourceImpl->getUri();
            }

            std::string SHBaseRemoteResource::getHost() const
            {
                return m_remoteResourceImpl->getHost();
            }

            std::list<std::string> SHBaseRemoteResource::getHosts() const
            {
                return m_remoteResourceImpl->getHosts();
            }

            std::string SHBaseRemoteResource::setHost(const std::string &host)
            {
                return m_remoteResourceImpl->setHost(host);
            }

            std::list<std::string> SHBaseRemoteResource::getResourceTypes() const
            {
                return m_remoteResourceImpl->getResourceTypes();
            }

            std::list<std::string> SHBaseRemoteResource::getInterfaces() const
            {
                return m_remoteResourceImpl->getInterfaces();
            }

            bool SHBaseRemoteResource::hasResourceType(const std::string resourceType) const
            {
                return m_remoteResourceImpl->hasResourceType(resourceType);
            }

            void SHBaseRemoteResource::setDelegate(SHBaseRemoteResourceDelegate *delegate)
            {
                m_remoteResourceImpl->setDelegate(delegate);
            }

            SHRequestHandle SHBaseRemoteResource::getPropertyBundle()
            {
                return m_remoteResourceImpl->getPropertyBundle();
            }

            SHRequestHandle SHBaseRemoteResource::getPropertyBundle(const ResourceQuery &query)
            {
                return m_remoteResourceImpl->getPropertyBundle(query);
            }

            SHRequestHandle SHBaseRemoteResource::setPropertyBundle(const PropertyBundle &bundle)
            {
                return m_remoteResourceImpl->setPropertyBundle(bundle);
            }

            SHRequestHandle SHBaseRemoteResource::setPropertyBundle(const PropertyBundle &bundle,
                    const ResourceQuery &query)
            {
                return m_remoteResourceImpl->setPropertyBundle(bundle, query);
            }

            bool SHBaseRemoteResource::startObserve()
            {
                return m_remoteResourceImpl->startObserve();
            }

            bool SHBaseRemoteResource::startObserve(const ResourceQuery &query)
            {
                return m_remoteResourceImpl->startObserve(query);
            }

            bool SHBaseRemoteResource::stopObserve()
            {
                return m_remoteResourceImpl->stopObserve();
            }

            SHBaseRemoteResource* SHBaseRemoteResource::constructResourceObject(
                                                const std::string& host,
                                                const std::string& uri,
                                                OCConnectivityType connectivityType,
                                                bool isObservable,
                                                const std::list<std::string>& resourceTypes,
                                                const std::list<std::string>& interfaces,
                                                const std::list<std::string>& endpoints)
            {
                if (host.empty() || uri.empty() || resourceTypes.empty() || interfaces.empty())
                {
                    throw CommonException("Invalid arguments");
                    return NULL;
                }

                if (uri.length() == 1 && uri[0] == '/')
                {
                    throw CommonException("Invalid uri");
                    return NULL;
                }

                if (uri[0] != '/')
                {
                    throw CommonException("Invalid uri");
                    return NULL;
                }

                return SHBaseRemoteResourceBuilder::constructSHBaseRemoteResourceObject(
                                                            host, uri, connectivityType,
                                                            isObservable, resourceTypes,
                                                            interfaces, endpoints);
            }
        }
    }
}
