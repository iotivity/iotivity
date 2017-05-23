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
#include <iostream>

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

            std::list<std::string> SHBaseRemoteResource::getResourceTypes() const
            {
                return m_remoteResourceImpl->getResourceTypes();
            }

            std::string SHBaseRemoteResource::getUri() const
            {
                return m_remoteResourceImpl->getUri();
            }

            std::list<std::string> SHBaseRemoteResource::getHosts() const
            {
                return m_remoteResourceImpl->getHosts();
            }

            void SHBaseRemoteResource::setDelegate(const SHBaseRemoteResourceDelegate *delegate)
            {
                m_remoteResourceImpl->setDelegate(delegate);
            }

            void SHBaseRemoteResource::getPropertyBundle()
            {
                m_remoteResourceImpl->getPropertyBundle();
            }

            void SHBaseRemoteResource::getPropertyBundle(const ResourceQuery &query)
            {
                m_remoteResourceImpl->getPropertyBundle(query);
            }

            void SHBaseRemoteResource::setPropertyBundle(const PropertyBundle &bundle)
            {
                m_remoteResourceImpl->setPropertyBundle(bundle);
            }

            void SHBaseRemoteResource::setPropertyBundle(const PropertyBundle &bundle,
                                                         const ResourceQuery &query)
            {
                m_remoteResourceImpl->setPropertyBundle(bundle, query);
            }
        }
    }
}
