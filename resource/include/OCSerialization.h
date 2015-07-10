//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
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

#include <StringConstants.h>
#include "ocpayload.h"
#include "ocrandom.h"

namespace OC
{
    class ListenOCContainer
    {
        private:
            static std::vector<std::string> StringLLToVector(OCStringLL* ll)
            {
                std::vector<std::string> strs;
                while(ll)
                {
                    strs.push_back(ll->value);
                    ll = ll->next;
                }
                return strs;
            }

        public:
            ListenOCContainer(std::weak_ptr<IClientWrapper> cw,
                    const OCDevAddr& devAddr, OCDiscoveryPayload* payload)
                    : m_clientWrapper(cw), m_devAddr(devAddr)
            {
                OCResourcePayload* res = payload->resources;

                while(res)
                {
                    char uuidString[UUID_STRING_SIZE];
                    if(OCConvertUuidToString(res->sid, uuidString) != RAND_UUID_OK)
                    {
                        uuidString[0]= '\0';
                    }

                    m_resources.push_back(std::shared_ptr<OC::OCResource>(
                                new OC::OCResource(m_clientWrapper, m_devAddr,
                                    std::string(res->uri),
                                    std::string(uuidString),
                                    (res->bitmap & OC_OBSERVABLE) == OC_OBSERVABLE,
                                    StringLLToVector(res->types),
                                    StringLLToVector(res->interfaces)
                                    )));
                    res = res->next;
                }

            }

            const std::vector<std::shared_ptr<OCResource>>& Resources() const
            {
                return m_resources;
            }
        private:
            std::vector<std::shared_ptr<OC::OCResource>> m_resources;
            std::weak_ptr<IClientWrapper> m_clientWrapper;
            const OCDevAddr& m_devAddr;
    };
    /*
    class ListenOCContainer
    {
        private:
        enum class OCSecureType
        {
            NotSecure,
            Secure
        };

        class DiscoveredResources
        {
            class Resource
            {
                friend class cereal::access;

                class ResourcePolicy
                {
                    friend class cereal::access;
                    friend class Resource;

                    template<class Archive>
                    void serialize(Archive& ar)
                    {
                        try
                        {
                            m_observable = false;
                            ar(cereal::make_nvp(OC::Key::BMKEY, m_bm));

                            if(m_bm & OC_OBSERVABLE)
                            {
                                m_observable = true;
                            }
                        }
                        catch(cereal::Exception&)
                        {
                            ar.setNextName(nullptr);
                        }

                        try
                        {
                            m_secure = false;
                            int secureTemp;
                            ar(cereal::make_nvp(OC::Key::SECUREKEY, secureTemp));
                            m_secure = secureTemp != 0;

                            m_port = -1;
                            ar(cereal::make_nvp(OC::Key::PORTKEY, m_port));
                        }
                        catch(cereal::Exception&)
                        {
                            ar.setNextName(nullptr);
                        }
                     }

                     bool m_observable;
                     uint8_t m_bm;
                     bool m_secure;
                     int m_port;
                };

                template <class Archive>
                void serialize(Archive& ar)
                {
                    try
                    {
                        ar(cereal::make_nvp(OC::Key::URIKEY, m_uri));
                        m_loaded = true;
                    }
                    catch(cereal::Exception&)
                    {
                        ar.setNextName(nullptr);
                    }

                    try
                    {
                        ar(cereal::make_nvp(OC::Key::RESOURCETYPESKEY,m_resourceTypes));
                        m_loaded = true;
                    }
                    catch(cereal::Exception&)
                    {
                        ar.setNextName(nullptr);
                    }

                    try
                    {
                        ar(cereal::make_nvp(OC::Key::INTERFACESKEY, m_interfaces));
                        m_loaded = true;
                    }
                    catch(cereal::Exception&)
                    {
                        ar.setNextName(nullptr);
                    }

                    try
                    {
                        ar(cereal::make_nvp(OC::Key::POLICYKEY, m_policy));
                        m_loaded = true;
                    }
                    catch(cereal::Exception&)
                    {
                        ar.setNextName(nullptr);
                    }

                    // Although not expected, a server id as part of a resource's own
                    // representation is legal. It may be used if needed.
                    try
                    {
                        ar(cereal::make_nvp(OC::Key::DEVICEIDKEY, m_serverId));
                        m_loaded = true;
                    }
                    catch(cereal::Exception&)
                    {
                        ar.setNextName(nullptr);
                    }
                }
            public:
                Resource(): m_loaded(false)
                {}

                bool observable() const
                {
                    return m_policy.m_observable;
                }

                OCSecureType secureType() const
                {
                    return m_policy.m_secure ? OCSecureType::Secure : OCSecureType::NotSecure;
                }

                int port() const
                {
                    return m_policy.m_port;
                }

                std::vector<std::string> resourceTypes() const
                {
                    return m_resourceTypes;
                }

                std::vector<std::string> interfaces() const
                {
                    return m_interfaces;
                }

                bool loaded() const{
                    return m_loaded;
                }

                std::string m_uri;
                std::string m_serverId;
                std::vector<std::string> m_resourceTypes;
                std::vector<std::string> m_interfaces;
                ResourcePolicy m_policy;
                bool m_loaded;
            };

            public:
            DiscoveredResources()
            {}

            private:
            friend class cereal::access;
            friend class ListenOCContainer;

            template <class Archive>
            void serialize(Archive& ar)
            {
                try
                {
                    ar(cereal::make_nvp(OC::Key::DEVICEIDKEY, m_serverIdOfThisDevice));
                }
                catch(cereal::Exception&) { ar.setNextName(nullptr); }

                try
                {
                    ar(cereal::make_nvp(OC::Key::LINKS, resources));
                }
                catch(cereal::Exception&) { ar.setNextName(nullptr); }
            }

            std::string m_serverIdOfThisDevice;
            std::vector<Resource> resources;
        };

        private:
            friend class cereal::access;
            template <class Archive>
            void serialize(Archive& ar)
            {
                std::vector<DiscoveredResources> resources;
                ar(resources);
            }
        public:
            ListenOCContainer(std::weak_ptr<IClientWrapper> cw,
                    const OCDevAddr& devAddr, std::stringstream& json)
                    : m_clientWrapper(cw), m_devAddr(devAddr)

            {
                LoadFromJson(json);
            }

            const std::vector<std::shared_ptr<OCResource>>& Resources() const
            {
                return m_resources;
            }

        private:
            void LoadFromJson(std::stringstream& json)
            {
                cereal::JSONInputArchive archive(json);

                std::vector<DiscoveredResources> resources;
                archive(cereal::make_nvp(OC::Key::OCKEY, resources));

                m_resources.clear();

                for(const auto& resourcesAtDevice : resources)
                {
                    std::string serverIDForThisResourceRep = resourcesAtDevice.m_serverIdOfThisDevice;

                    for (const auto& resource : resourcesAtDevice.resources)
                    {
                        try
                        {
                            if(resource.loaded())
                            {
                                m_resources.push_back(std::shared_ptr<OCResource>
                                (
                                    new OCResource
                                    (
                                        m_clientWrapper,
                                        m_devAddr,
                                        resource.m_uri,
                                        serverIDForThisResourceRep,
                                        resource.observable(),
                                        resource.resourceTypes(),
                                        resource.interfaces()
                                    )
                                ));
                            }
                        }
                        catch(ResourceInitException& e)
                        {
                            oclog() << "listenCallback(): failed to create resource: " << e.what()
                                    << std::flush;
                        }
                    }
                }
            }
    };
*/
}
