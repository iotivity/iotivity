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
#include <ResourceQuery.h>
#include <iostream>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            SHBaseResource::SHBaseResource(const std::string& uri)
            : SHBaseResource_Impl::SHBaseResource_Impl(uri)
            {
            }

            SHBaseResource::SHBaseResource(const std::string& uri,
                                           const std::list<std::string>& types,
                                           const std::list<std::string>& interfaces)
            : SHBaseResource_Impl::SHBaseResource_Impl(uri, types, interfaces)
            {
            }

            SHBaseResource::~SHBaseResource()
            {
            }

            std::string SHBaseResource::getUri() const
            {
                return SHBaseResource_Impl::getUri();
            }

            void SHBaseResource::setUri(const std::string& uri)
            {
                SHBaseResource_Impl::setUri(uri);
            }

            std::list<std::string> SHBaseResource::getTypes() const
            {
                return SHBaseResource_Impl::getTypes();
            }

            void SHBaseResource::setTypes(const std::list<std::string>& types)
            {
                SHBaseResource_Impl::setTypes(types);
            }

            std::list<std::string> SHBaseResource::getInterfaces() const
            {
                return SHBaseResource_Impl::getInterfaces();
            }

            void SHBaseResource::setInterfaces(const std::list<std::string>& interfaces)
            {
                SHBaseResource_Impl::setInterfaces(interfaces);
            }

            void SHBaseResource::setPropertyBundle(const PropertyBundle& bundle)
            {
                SHBaseResource_Impl::setPropertyBundle(bundle);
            }

            const PropertyBundle& SHBaseResource::getPropertyBundle() const
            {
                return SHBaseResource_Impl::getPropertyBundle();
            }

            bool SHBaseResource::sendResponse(int requestId, const PropertyBundle& bundle)
            {
                return SHBaseResource_Impl::sendResponse(requestId, bundle);
            }

            bool SHBaseResource::sendErrorResponse(int requestId, const PropertyBundle& bundle)
            {
                return SHBaseResource_Impl::sendErrorResponse(requestId, bundle);
            }

            void SHBaseResource::setDelegate(SHBaseResourceDelegate *delegate)
            {
                SHBaseResource_Impl::setDelegate(delegate);
            }
        }
    }
}
