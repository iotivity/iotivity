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
            SHBaseRemoteResource_Impl::SHBaseRemoteResource_Impl() : m_delegate(NULL)
            {
            }

            SHBaseRemoteResource_Impl::~SHBaseRemoteResource_Impl()
            {
            }

            std::list<std::string> SHBaseRemoteResource_Impl::getResourceTypes() const
            {
            }

            std::string SHBaseRemoteResource_Impl::getUri() const
            {
            }

            std::list<std::string> SHBaseRemoteResource_Impl::getHosts() const
            {
            }

            void SHBaseRemoteResource_Impl::setDelegate(
                    const SHBaseRemoteResourceDelegate *delegate)
            {
                this->m_delegate = delegate;
            }

            void SHBaseRemoteResource_Impl::getPropertyBundle()
            {
                std::cout <<"getProperty bundle." << std::endl;
            }

            void SHBaseRemoteResource_Impl::getPropertyBundle(const ResourceQuery &query)
            {
                std::cout <<"getProperty bundle with query." << std::endl;
            }

            void SHBaseRemoteResource_Impl::setPropertyBundle(const PropertyBundle &bundle)
            {
                std::cout <<"setProperty bundle" << std::endl;
            }

            void SHBaseRemoteResource_Impl::setPropertyBundle(const PropertyBundle &bundle,
                                                              const ResourceQuery &query)
            {
                std::cout <<"setProperty bundle with query" << std::endl;
            }
        }
    }
}
