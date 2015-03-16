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
            IPv4Secure,
            IPv4
        };

        class ListenResourceContainer
        {
            class ListenResourcePropertiesContainer
            {
                friend class cereal::access;
                friend class ListenResourceContainer;

                template<class Archive>
                void serialize(Archive& ar)
                {
                    try
                    {
                        m_observable=false;
                        int obsTemp;
                        ar(cereal::make_nvp(OC::Key::OBSERVABLEKEY, obsTemp));
                        m_observable = obsTemp != 0;
                    }
                    catch(cereal::Exception&)
                    {
                        // we swallow this exception, since it means the key
                        // doesn't exist, allowing these to be optional
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

                bool m_observable;
                std::vector<std::string> m_resourceTypes;
                std::vector<std::string> m_interfaces;
                bool m_secure;
                int m_port;
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
                return m_props.m_observable;
            }

            OCSecureType secureType() const
            {
                return m_props.m_secure?OCSecureType::IPv4Secure :OCSecureType::IPv4;
            }

            int port() const
            {
                return m_props.m_port;
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
            ListenOCContainer(std::weak_ptr<IClientWrapper> cw, const OCDevAddr& address,
                    OCConnectivityType connectivityType, std::stringstream& json):
                m_clientWrapper(cw), m_address(address), m_connectivityType(connectivityType)
            {
                LoadFromJson(json);
            }

            const std::vector<std::shared_ptr<OCResource>>& Resources() const
            {
                return m_resources;
            }

        private:
            std::string ConvertOCAddrToString(OCSecureType sec, int secureport)
            {
                uint16_t port;
                std::ostringstream os;

                if(sec== OCSecureType::IPv4)
                {
                    os<<"coap://";
                }
                else if(sec == OCSecureType::IPv4Secure)
                {
                    os<<"coaps://";
                }
                else
                {
                    oclog() << "ConvertOCAddrToString():  invalid SecureType"<<std::flush;
                    throw ResourceInitException(false, false, false, false, false, true);
                }

                uint8_t a;
                uint8_t b;
                uint8_t c;
                uint8_t d;
                if(OCDevAddrToIPv4Addr(&m_address, &a, &b, &c, &d) != 0)
                {
                    oclog() << "ConvertOCAddrToString(): Invalid Ip"
                            << std::flush;
                    throw ResourceInitException(false, false, false, false, false, true);
                }

                os<<static_cast<int>(a)<<"."<<static_cast<int>(b)
                        <<"."<<static_cast<int>(c)<<"."<<static_cast<int>(d);

                if(sec == OCSecureType::IPv4Secure && secureport>0 && secureport<=65535)
                {
                    port = static_cast<uint16_t>(secureport);
                }
                else if(sec == OCSecureType::IPv4 && 0==OCDevAddrToPort(&m_address, &port))
                {
                    // nothing to do, this is a successful case
                }
                else
                {
                    oclog() << "ConvertOCAddrToString() : Invalid Port"
                            <<std::flush;
                    throw ResourceInitException(false, false, false, false, true, false);
                }

                os <<":"<< static_cast<int>(port);

                return os.str();
            }

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
                                new OCResource(m_clientWrapper,
                                    ConvertOCAddrToString(res.secureType(),res.port()),
                                    res.m_uri, res.m_serverId, m_connectivityType, res.observable(),
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
            OCDevAddr m_address;
            OCConnectivityType m_connectivityType;
    };
}

