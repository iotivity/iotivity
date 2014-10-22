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

/// @file OCRepresentation.h

/// @brief  This file contains the declaration of classes and its members
///         related to OCRepresentation

#ifndef __OCREPRESENTATION_H
#define __OCREPRESENTATION_H


#include <string>
#include <sstream>
#include <vector>
#include <map>

#include <AttributeValue.h>
#include <StringConstants.h>

// note: is there any way to move this later so that the implementers don't need to
// reference them?
#include <cereal/cereal.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/vector.hpp>
#include <OicJsonSerializer.hpp>// Customized Cereal serializer, required for a few functions
#include <cereal/types/utility.hpp>

#include <OCException.h>
namespace OC
{

    enum class InterfaceType
    {
        None,
        LinkParent,
        BatchParent,
        DefaultParent,
        LinkChild,
        BatchChild,
        DefaultChild
    };

    // The consumer requires resource info to be printed in 2 different ways, both with the "oc":[]
    // and without.  This enum is used to differentiate between the two situations.  When the
    // serialize is called with Include OC, we encode OC, otherwise we skip it and return just the
    // contents of the array.
    enum class OCInfoFormat
    {
        IncludeOC,
        ExcludeOC
    };

    class MessageContainer
    {
        public:
            void setJSONRepresentation(const std::string& payload);

            void setJSONRepresentation(const unsigned char* payload);

            std::string getJSONRepresentation(OCInfoFormat f) const;

            const std::vector<OCRepresentation>& representations() const;

            void addRepresentation(const OCRepresentation& rep);

            const OCRepresentation& operator[](int index) const
            {
                return m_reps[index];
            }

            const OCRepresentation& back() const
            {
                return m_reps.back();
            }
        private:
            std::vector<OCRepresentation> m_reps;
    };
    class OCRepresentation
    {
        public:
            OCRepresentation();
            std::string getJSONRepresentation() const;

            void addChild(const OCRepresentation&);

            void clearChildren();

            const std::vector<OCRepresentation>& getChildren() const;

            void setChildren(const std::vector<OCRepresentation>& children);

            void setUri(const std::string& uri);

            std::string getUri() const;

            const std::vector<std::string>& getResourceTypes() const;

            void setResourceTypes(const std::vector<std::string>& resourceTypes);

            const std::vector<std::string>& getResourceInterfaces() const;

            void setResourceInterfaces(const std::vector<std::string>& resourceInterfaces);

            bool empty() const;

            int numberOfAttributes() const;

            bool erase(const std::string& str);

            template <typename T>
            void setValue(const std::string& str, const T& val)
            {
                m_values[str] = val;
            }

            template <typename T>
            bool getValue(const std::string& str, T& val) const
            {
                auto x = m_values.find(str);

                if(x!= m_values.end())
                {
                    val = boost::get<T>(x->second);
                    return true;
                }
                else
                {
                    val = T();
                    return false;
                }
            }

            template <typename T>
            T getValue(const std::string& str) const
            {
                T val = T();
                auto x = m_values.find(str);
                if(x != m_values.end())
                {
                    val = boost::get<T>(x->second);
                }
                return val;
            }

            bool hasAttribute(const std::string& str) const;

            void setNULL(const std::string& str);

            bool isNULL(const std::string& str) const;
        private:
            friend class OCResourceResponse;
            friend class cereal::access;

            // the root node has a slightly different JSON version
            // based on the interface type configured in ResourceResponse.
            // This allows ResourceResponse to set it, so that the save function
            // doesn't serialize things that it isn't supposed to serialize.
            void setInterfaceType(InterfaceType ift)
            {
                m_interfaceType = ift;
            }

            // class used to wrap the 'prop' feature of the save/load
            class Prop
            {
                public:
                    Prop(std::vector<std::string>& resourceTypes,
                            std::vector<std::string>& interfaces)
                    : m_types(resourceTypes), m_interfaces(interfaces)
                    {}

                 /*   Prop(const std::vector<std::string>& resourceTypes,
                            const std::vector<std::string>& interfaces)
                    :m_types(resourceTypes),
                    m_interfaces(interfaces)
                    {}*/
                private:
                    friend class cereal::access;
                    template <class Archive>
                    void save(Archive& ar) const
                    {
                        if(m_types.size() > 0)
                        {
                            ar(cereal::make_nvp(Key::RESOURCETYPESKEY, m_types));
                        }

                        if(m_interfaces.size()>0)
                        {
                            ar(cereal::make_nvp(Key::INTERFACESKEY, m_interfaces));
                        }
                    }

                    template<class Archive>
                    void load(Archive& ar)
                    {
                        optional_load(ar, cereal::make_nvp(Key::RESOURCETYPESKEY, m_types));
                        optional_load(ar, cereal::make_nvp(Key::INTERFACESKEY, m_interfaces));
                    }

                    std::vector<std::string>& m_types;
                    std::vector<std::string>& m_interfaces;
            };
            template<class Archive, class Val>
            static void optional_load(Archive& ar, Val&& v)
            {
                try
                {
                    ar(v);
                }
                catch(cereal::Exception& e)
                {
                    ar.setNextName(nullptr);
                    // Loading a key that doesn't exist results in an exception
                    // Since "Not Found" is a valid condition for us, we swallow
                    // this exception and the archive will not load anything
                }
            }
            template<class Archive>
            void save(Archive& ar) const
            {
                // printed for all interface types
                if(!m_uri.empty())
                {
                    ar(cereal::make_nvp(Key::URIKEY, m_uri));
                }

                if((m_interfaceType == InterfaceType::None
                            || m_interfaceType==InterfaceType::DefaultChild
                            || m_interfaceType==InterfaceType::LinkChild)
                        && (m_resourceTypes.size()>0 || m_interfaces.size()>0))
                {
                    // The Prop object requires that it refer to non-const vectors
                    // so that it can alter them in the 'load' case.  In the save case
                    // (initiated here) it will not modify the object.  So, to keep the
                    // compiler happy, removing the 'const' context here is necessary.
                    const std::vector<std::string>& rt(m_resourceTypes);
                    const std::vector<std::string>& intf(m_interfaces);
                    Prop temp(const_cast<std::vector<std::string>&>(rt),
                            const_cast<std::vector<std::string>&>(intf));
                    ar(cereal::make_nvp(Key::PROPERTYKEY, temp));
                }

                // printed only for BatchChildren and DefaultParent
                if((m_interfaceType == InterfaceType::None
                            || m_interfaceType == InterfaceType::BatchChild
                            || m_interfaceType == InterfaceType::DefaultParent)
                        && m_values.size()>0)
                {
                    ar(cereal::make_nvp(Key::REPKEY, m_values));
                }
            }

            template<class Archive>
            void load(Archive& ar)
            {
                optional_load(ar, cereal::make_nvp(Key::URIKEY, m_uri));
                {
                    Prop temp(m_resourceTypes, m_interfaces);
                    optional_load(ar, cereal::make_nvp(Key::PROPERTYKEY, temp));
                }
                optional_load(ar, cereal::make_nvp(Key::REPKEY, m_values));
            }

        private:
            std::string m_uri;
            std::vector<OCRepresentation> m_children;
            std::map<std::string, AttributeValue> m_values;
            std::vector<std::string> m_resourceTypes;
            std::vector<std::string> m_interfaces;

            InterfaceType m_interfaceType;
    };
} // namespace OC

// code needed to serialize a string::Attribute value map
namespace OC
{
    namespace detail
    {
        template<class Archive>
        class WriteAttributeValue : public boost::static_visitor<>
        {
            public:
                WriteAttributeValue(const std::string& name, Archive& ar)
                    :m_name(name), m_archive(ar)
                {}

                template<class T>
                void operator()(const T& value) const
                {
                    m_archive(cereal::make_nvp(m_name, value));
                }
            private:
                std::string m_name;
                Archive& m_archive;
        };
    }
}

namespace cereal
{
    // take no action when serializing the null type, because the 'save' below
    // doesn't use the visitor for this type.
    template <class Archive>
    void serialize(Archive&, OC::NullType t)
    {}

    template<class Archive>
    void save(Archive& ar, const std::map<std::string, OC::AttributeValue>& vals)
    {
        for(const auto& kv : vals)
        {
            const auto& k = kv.first;
            const auto& v = kv.second;

            if(v.which() != OC::AttributeValueNullIndex)
            {
                OC::detail::WriteAttributeValue<Archive> writer(k,ar);
                boost::apply_visitor(writer, v);
            }
            else
            {
                ar.setNextName(k.c_str());
                ar.writeName();
                ar.saveValue();
            }
        }
    }

    template<class Archive>
    void load(Archive& ar, std::map<std::string, OC::AttributeValue>& vals)
    {
        ar.loadAttributeValues(vals);
    }
}

#endif //__OCREPRESENTATION_H
