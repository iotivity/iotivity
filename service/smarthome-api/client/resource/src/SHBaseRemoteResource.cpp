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
            }

            SHBaseRemoteResource::~SHBaseRemoteResource()
            {
            }

            std::list<std::string> SHBaseRemoteResource::getResourceTypes() const
            {
                return SHBaseRemoteResource_Impl::getResourceTypes();
            }

            std::string SHBaseRemoteResource::getUri() const
            {
                return SHBaseRemoteResource_Impl::getUri();
            }

            std::list<std::string> SHBaseRemoteResource::getHosts() const
            {
                return SHBaseRemoteResource_Impl::getHosts();
            }

            void SHBaseRemoteResource::setDelegate(const SHBaseRemoteResourceDelegate *delegate)
            {
                SHBaseRemoteResource_Impl::setDelegate(delegate);
            }

            void SHBaseRemoteResource::getPropertyBundle()
            {
                SHBaseRemoteResource_Impl::getPropertyBundle();
            }

            void SHBaseRemoteResource::getPropertyBundle(const ResourceQuery &query)
            {
                SHBaseRemoteResource_Impl::getPropertyBundle(query);
            }

            void SHBaseRemoteResource::setPropertyBundle(const PropertyBundle &bundle)
            {
                SHBaseRemoteResource_Impl::setPropertyBundle(bundle);
            }

            void SHBaseRemoteResource::setPropertyBundle(const PropertyBundle &bundle,
                                                         const ResourceQuery &query)
            {
                SHBaseRemoteResource_Impl::setPropertyBundle(bundle, query);
            }
        }
    }
}
