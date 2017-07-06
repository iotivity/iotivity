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
#ifndef SMARTHOME_API_CLIENT_SHBASEREMOTERESOURCE_IMPL_H_
#define SMARTHOME_API_CLIENT_SHBASEREMOTERESOURCE_IMPL_H_

#include <list>
#include <string>

#include <CommonApi.h>
#include "octypes.h"

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            class PropertyBundle;
            class ResourceQuery;
            class SHBaseRemoteResourceDelegate;

             class SHBaseRemoteResource_Impl
            {
            friend class SHBaseRemoteResource;
            friend class SHBaseRemoteResourceBuilder;
            public:
                ~SHBaseRemoteResource_Impl();

                std::string getUri() const;

                std::string getHost() const;

                std::list<std::string> getHosts() const;

                std::string setHost(const std::string &host);

                std::list<std::string> getResourceTypes() const;

                std::list<std::string> getInterfaces() const;

                bool hasResourceType(const std::string resourceType) const;

                bool startObserve();

                bool startObserve(const ResourceQuery &query);

                bool stopObserve();

            protected:
                void setDelegate(SHBaseRemoteResourceDelegate *delegate);

                SHRequestHandle getPropertyBundle();

                SHRequestHandle getPropertyBundle(const ResourceQuery &query);

                SHRequestHandle setPropertyBundle(const PropertyBundle &bundle);

                SHRequestHandle setPropertyBundle(const PropertyBundle &bundle,
                        const ResourceQuery &query);

                bool setSHBaseRemoteResource_Impl(const std::string& uri,
                        const OCDevAddr& devAddr,
                        uint8_t property,
                        const std::list<std::string>& resourceTypes,
                        const std::list<std::string>& interfaces);

                bool setSHBaseRemoteResource_Impl(const std::string& uri,
                        const OCDevAddr& devAddr,
                        uint8_t property,
                        const std::list<std::string>& resourceTypes,
                        const std::list<std::string>& interfaces,
                        const std::list<std::string>& endpoints);

                bool setSHBaseRemoteResource_Impl(const std::string& host,
                        OCConnectivityType connectivityType,
                        const std::string& uri,
                        uint8_t property,
                        const std::list<std::string>& resourceTypes,
                        const std::list<std::string>& interfaces,
                        const std::list<std::string>& endpoints);

            private:
                SHBaseRemoteResource_Impl();
                SHBaseRemoteResourceDelegate *m_delegate;

                std::string m_resourceUri;
                OCDevAddr m_devAddr;
                //bool m_useHostString;
                //bool m_isCollection;
                uint8_t m_property;
                std::list<std::string> m_resourceTypes;
                std::list<std::string> m_interfaces;
                //std::list<std::string> m_children;
                std::list<std::string> m_endpoints;
                SHRequestHandle m_observeHandle;
                //HeaderOptions m_headerOptions;
            };
        }
    }
}

#endif /* SMARTHOME_API_CLIENT_SHBASEREMOTERESOURCE_IMPL_H_ */
