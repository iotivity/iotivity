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

#include <cereal/cereal.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/json.hpp>

#include <StringConstants.h>

namespace OC
{
    class ListenOCContainer
    {
        private:
        enum class OCSecureType
        {
            NotSecure,
            Secure
        };

        class ListenResourceContainer
        {
            class ListenResourcePolicyContainer
            {
                friend class cereal::access;
                friend class ListenResourceContainer;
                friend class ListenResourcePropertiesContainer;

                template<class Archive>
                void serialize(Archive& ar)
                {
                    try
                    {
                        m_observable = false;
                        ar(cereal::make_nvp(OC::Key::BMKEY, m_bm));
                        // In case of observable
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

            class ListenResourcePropertiesContainer
            {
                friend class cereal::access;
                friend class ListenResourceContainer;

                template<class Archive>
                void serialize(Archive& ar)
                {
                    try
                    {
                        ar(cereal::make_nvp(OC::Key::POLICYKEY, m_policy));

                    }
                    catch(cereal::Exception&)
                    {
                        // we swallow this exception, since it means the key
                        // doesn't exist, allowing these to be optional
                        oclog() << "Invalid POLICYKEY"<<std::flush;
                        ar.setNextName(nullptr);
                    }

                    try
                    {
                        ar(cereal::make_nvp(OC::Key::RESOURCETYPESKEY,m_resourceTypes));
                    }
                    catch(cereal::Exception&)
                    {
                        ar.setNextName(nullptr);
                    }
                    try
                    {
                        ar(cereal::make_nvp(OC::Key::INTERFACESKEY, m_interfaces));
                    }
                    catch(cereal::Exception&)
                    {
                        ar.setNextName(nullptr);
                    }
                }

                std::vector<std::string> m_resourceTypes;
                std::vector<std::string> m_interfaces;
                ListenResourcePolicyContainer m_policy;
            };

            public:
            ListenResourceContainer() : m_loaded(false)
            {}

            private:
            friend class cereal::access;
            friend class ListenOCContainer;

            template <class Archive>
            void serialize(Archive& ar)
            {
                try
                {
                    ar(cereal::make_nvp(OC::Key::URIKEY, m_uri));
                    m_loaded=true;
                }
                catch(cereal::Exception&)
                {
                    ar.setNextName(nullptr);
                }
                try
                {
                    ar(cereal::make_nvp(OC::Key::SERVERIDKEY, m_serverId));
                    m_loaded=true;
                }
                catch(cereal::Exception&)
                {
                    ar.setNextName(nullptr);
                }
                try
                {
                    ar(cereal::make_nvp(OC::Key::PROPERTYKEY, m_props));
                    m_loaded=true;
                }
                catch(cereal::Exception&)
                {
                    ar.setNextName(nullptr);
                }

            }

            std::string m_uri;
            std::string m_serverId;
            bool m_loaded;
            ListenResourcePropertiesContainer m_props;

            bool loaded() const
            {
                return m_loaded;
            }

            bool observable() const
            {
                return m_props.m_policy.m_observable;
            }

            OCSecureType secureType() const
            {
                return m_props.m_policy.m_secure ? OCSecureType::Secure : OCSecureType::NotSecure;
            }

            int port() const
            {
                return m_props.m_policy.m_port;
            }

            std::vector<std::string> resourceTypes() const
            {
                return m_props.m_resourceTypes;
            }

            std::vector<std::string> interfaces() const
            {
                return m_props.m_interfaces;
            }
        };

        private:
            friend class cereal::access;
            template <class Archive>
            void serialize(Archive& ar)
            {
                std::vector<ListenResourceContainer> resources;
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

                std::vector<ListenResourceContainer> resources;
                archive(cereal::make_nvp(OC::Key::OCKEY, resources));

                m_resources.clear();

                for(const auto& res : resources)
                {
                    try
                    {
                        if(res.loaded())
                        {
                            m_resources.push_back(std::shared_ptr<OCResource>(
                                new OCResource(m_clientWrapper, m_devAddr,
                                    res.m_uri, res.m_serverId, res.observable(),
                                    res.resourceTypes(), res.interfaces())));
                        }

                    }
                    catch(ResourceInitException& e)
                    {
                        oclog() << "listenCallback(): failed to create resource: " << e.what()
                                << std::flush;
                    }
                }
            }
            std::vector<std::shared_ptr<OC::OCResource>> m_resources;
            std::weak_ptr<IClientWrapper> m_clientWrapper;
            const OCDevAddr& m_devAddr;
    };
}
