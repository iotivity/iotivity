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

/// @file OCRepresentation.cpp

/// @brief  This file contains the implementation of classes and its members
///         related to OCRepresentation

#ifdef __ANDROID__
#include "OCAndroid.h"
#endif

#include <OCRepresentation.h>

#include <cereal/cereal.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/utility.hpp>
#include <OicJsonSerializer.hpp>
#include <algorithm>

// code needed to serialize a string=>Attribute value map
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

namespace OC
{
    typedef cereal::JSONOutputArchive OutputArchiveType;
    typedef cereal::JSONInputArchive InputArchiveType;

    void MessageContainer::setJSONRepresentation(const std::string& payload)
    {
        std::stringstream os(payload);
        {
            InputArchiveType archive(os);
            archive(cereal::make_nvp(OC::Key::OCKEY, m_reps));
        }
    }

    void MessageContainer::setJSONRepresentation(const unsigned char* payload)
    {
        setJSONRepresentation(std::string(reinterpret_cast<const char*>(payload)));
    }

    std::string MessageContainer::getJSONRepresentation(OCInfoFormat f) const
    {
        std::stringstream os;

        // note: the block is required because cereal closes the JSON string
        // upon destruction, so the archive needs to be destroyed before accessing
        // the data
        {
            if(f == OCInfoFormat::IncludeOC)
            {
                OutputArchiveType archive(os);
                archive(cereal::make_nvp(OC::Key::OCKEY, m_reps));
            }
            else if(f== OCInfoFormat::ExcludeOC)
            {
                bool firstPrinted = false;
                for(std::vector<OCRepresentation>::size_type i = 0; i< m_reps.size();++i)
                {
                    if(!m_reps[i].empty())
                    {
                        if(firstPrinted)
                        {
                            os<<',';
                        }
                        firstPrinted=true;
                        os << m_reps[i].getJSONRepresentation();
                    }
                }
            }
        }
        return os.str();
    }

    const std::vector<OCRepresentation>& MessageContainer::representations() const
    {
        return m_reps;
    }

    void MessageContainer::addRepresentation(const OCRepresentation& rep)
    {
        m_reps.push_back(rep);
    }
}

namespace OC
{
    OCRepresentation::OCRepresentation()
    :m_interfaceType(InterfaceType::None)
    { }
    std::string OCRepresentation::getJSONRepresentation() const
    {
        if(empty())
        {
            return "{}";
        }

        std::stringstream os;

        // note: the block is required because cereal closes the JSON string
        // upon destruction, so the archive needs to be destroyed before accessing
        // the data
        {
            OutputArchiveType archive (os);
            save(archive);
        }

        return os.str();
    }

    void OCRepresentation::addChild(const OCRepresentation& rep)
    {
        m_children.push_back(rep);
    }

    void OCRepresentation::clearChildren()
    {
        m_children.clear();
    }

    const std::vector<OCRepresentation>& OCRepresentation::getChildren() const
    {
        return m_children;
    }

    void OCRepresentation::setUri(const std::string& uri)
    {
        m_uri = uri;
    }

    std::string OCRepresentation::getUri() const
    {
        return m_uri;
    }

    const std::vector<std::string>& OCRepresentation::getResourceTypes() const
    {
        return m_resourceTypes;
    }

    void OCRepresentation::setResourceTypes(const std::vector<std::string>& resourceTypes)
    {
        m_resourceTypes = resourceTypes;
    }

    const std::vector<std::string>& OCRepresentation::getResourceInterfaces() const
    {
        return m_interfaces;
    }

    void OCRepresentation::setResourceInterfaces(const std::vector<std::string>& resourceInterfaces)
    {
        m_interfaces = resourceInterfaces;
    }

    bool OCRepresentation::hasAttribute(const std::string& str) const
    {
        return m_values.find(str) != m_values.end();
    }

    bool OCRepresentation::empty() const
    {
        // This logic is meant to determine whether based on the JSON serialization rules
        // if this object will result in empty JSON.  URI is only serialized if there is valid
        // data, ResourceType and Interfaces are only serialized if we are a nothing, a
        // child of a default or link item.
        // Our values array is only printed in the if we are the child of a Batch resource,
        // the parent in a 'default' situation, or not in a child/parent relationship.
        if(!m_uri.empty())
        {
            return false;
        }
        else if ((m_interfaceType == InterfaceType::None
                        || m_interfaceType==InterfaceType::DefaultChild
                        || m_interfaceType==InterfaceType::LinkChild)
                    && (m_resourceTypes.size()>0 || m_interfaces.size()>0))
        {
            return false;
        }
        else if((m_interfaceType == InterfaceType::None
                        || m_interfaceType == InterfaceType::BatchChild
                        || m_interfaceType == InterfaceType::DefaultParent)
                    && m_values.size()>0)
        {
            return false;
        }

        if(m_children.size() > 0)
        {
            return false;
        }

        return true;
    }

    int OCRepresentation::numberOfAttributes() const
    {
        return m_values.size();
    }

    bool OCRepresentation::erase(const std::string& str)
    {
        return m_values.erase(str);
    }

    void OCRepresentation::setNULL(const std::string& str)
    {
        m_values[str] = OC::NullType();
    }

    bool OCRepresentation::isNULL(const std::string& str) const
    {
        auto x = m_values.find(str);

        if(m_values.end() != x)
        {
            return x->second.which() == AttributeValueNullIndex;
        }
        else
        {
            throw OCException(OC::Exception::INVALID_ATTRIBUTE+ str);
        }
    }
}

namespace OC
{
    template <class Archive, class Val>
    void OCRepresentation::optional_load(Archive& ar, Val&& v)
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
    void OCRepresentation::save(Archive& ar) const
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
    void OCRepresentation::load(Archive& ar)
    {
        optional_load(ar, cereal::make_nvp(Key::URIKEY, m_uri));
        {
            Prop temp(m_resourceTypes, m_interfaces);
            optional_load(ar, cereal::make_nvp(Key::PROPERTYKEY, temp));
        }
        optional_load(ar, cereal::make_nvp(Key::REPKEY, m_values));
    }

    template<class Archive>
    void OCRepresentation::Prop::save(Archive& ar) const
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
    void OCRepresentation::Prop::load(Archive& ar)
    {
        optional_load(ar, cereal::make_nvp(Key::RESOURCETYPESKEY, m_types));
        optional_load(ar, cereal::make_nvp(Key::INTERFACESKEY, m_interfaces));
    }
}

// note: the below is used to load an AttributeValue map out of JSON
namespace OC
{
    namespace detail
    {
        enum class typeTag:uint8_t
        {
            NOTHING = 0,
            _string,
            _int,
            _double,
            _bool,
            _representation
        };

        typedef rapidjson::Document::GenericValue GenericValue;

        AttributeValue parseAttributeValue(const GenericValue& v);
        AttributeValue parseAttributeValue(const GenericValue& v,
                const unsigned int curLevel, unsigned int& maxDepth, typeTag& t);
        AttributeValue parseAttributeValueObject(const GenericValue& v, typeTag& t);
        AttributeValue parseAttributeValueArray(const GenericValue& v,
                const unsigned int curLevel, unsigned int& maxDepth, typeTag& t);
        AttributeValue parseAttributeValuePrimitive(const GenericValue& v, typeTag& t);

        AttributeValue parseAttributeValue(const GenericValue& v)
        {
            // base entrance, start everything at '0'
            unsigned int max_depth {0};
            typeTag t {typeTag::NOTHING};

            return parseAttributeValue(v, 0, max_depth, t);
        }

        AttributeValue parseAttributeValue(const GenericValue& v,
                const unsigned int curLevel, unsigned int& maxDepth, typeTag& t)
        {
            if(v.IsObject())
            {
                return parseAttributeValueObject(v, t);
            }
            else if(v.IsArray())
            {
                return parseAttributeValueArray(v, curLevel + 1, maxDepth, t);
            }
            else
            {
                return parseAttributeValuePrimitive(v,t);
            }
        }

        AttributeValue parseAttributeValueObject(const GenericValue& v, typeTag& t)
        {
            typedef rapidjson::Value::ConstMemberIterator CMI;
            t = typeTag::_representation;
            OC::OCRepresentation rep;

            for(CMI itr = v.MemberBegin(); itr!= v.MemberEnd(); ++itr)
            {
                std::string keyName = itr->name.GetString();

                if(keyName == OC::Key::URIKEY)
                {
                    rep.setUri(boost::get<std::string>(parseAttributeValue(itr->value)));
                }
                else if (keyName == OC::Key::PROPERTYKEY)
                {
                    for(CMI itr2 = itr->value.MemberBegin();
                            itr->value.MemberEnd()!=itr2;
                            ++itr2)
                    {
                        if(keyName == OC::Key::RESOURCETYPESKEY)
                        {
                            rep.setResourceTypes(
                                    boost::get<std::vector<std::string>>(
                                        parseAttributeValue(itr->value)));
                        }
                        else if(keyName == OC::Key::PROPERTYKEY)
                        {
                            rep.setResourceInterfaces(
                                    boost::get<std::vector<std::string>>(
                                        parseAttributeValue(itr->value)));
                        }
                    }
                }
                else if (keyName == OC::Key::REPKEY)
                {
                    for(CMI itr2 = itr->value.MemberBegin();
                            itr->value.MemberEnd()!=itr2;
                            ++itr2)
                    {
                        rep.setValue(itr2->name.GetString(),
                                parseAttributeValue(itr2->value));
                    }
                }
            }

            return rep;
        }

        AttributeValue parseAttributeValuePrimitive(const GenericValue& v, typeTag& t)
        {
            if(v.IsString())
            {
                t = typeTag::_string;
                return std::string(v.GetString());
            }
            else if (v.IsNumber())
            {
                if(v.IsDouble())
                {
                    t = typeTag::_double;
                    return double(v.GetDouble());
                }
                else if (v.IsInt())
                {
                    t = typeTag::_int;
                    return int(v.GetInt());
                }
                else
                {
                    throw OC::OCException(OC::Exception::INVALID_JSON_NUMERIC
                            + std::to_string(v.GetType()));
                }
            }
            else if(v.IsBool_())
            {
                t=typeTag::_bool;
                return bool(v.GetBool_());
            }
            else if(v.IsNull_())
            {
                return OC::NullType();
            }
            else
            {
                throw OC::OCException(OC::Exception::INVALID_JSON_TYPE
                        + std::to_string(v.GetType()));
            }
        }

        std::vector<AttributeValue> gatherArrayContents(const GenericValue& v,
                const unsigned int curLevel, unsigned int& maxDepth, typeTag& t)
        {
            std::vector<AttributeValue> out;

            std::transform(v.Begin(), v.End(), back_inserter(out),
                    [curLevel, &maxDepth, &t](const GenericValue& x)
                    {
                        return parseAttributeValue(x, curLevel, maxDepth, t);
                    });
            return out;
        }

        template<class OutT>
        struct valueToConcrete
        {
            OutT operator()(const AttributeValue& v)
            {
                return boost::get<OutT>(v);
            }

        };

        template <class OutSeqT>
        OutSeqT valuesToConcreteVectors(const std::vector<AttributeValue>& vs)
        {
            OutSeqT ret;

            std::transform(begin(vs),end(vs), back_inserter(ret),
                valueToConcrete<typename OutSeqT::value_type>());
            return ret;
        }

        template<class valueType>
        AttributeValue remapArrayDepth(const unsigned int curLevel,
                const std::vector<OC::AttributeValue>& vs)
        {
            switch(curLevel)
            {
                default:
                    throw OC::OCException(OC::Exception::INVALID_JSON_ARRAY_DEPTH);
                    break;
                case 1:
                    return valuesToConcreteVectors<std::vector<valueType>>(vs);
                    break;
                case 2:
                    return valuesToConcreteVectors<std::vector<std::vector<valueType>>>(vs);
                    break;
                case 3:
                    return valuesToConcreteVectors
                        <std::vector<std::vector<std::vector<valueType>>>>(vs);
                    break;
            }
        }

        AttributeValue convertArrayToConcretes(const typeTag t,
                const unsigned int curLevel, const std::vector<OC::AttributeValue>& vs)
        {
            // This function converts a std::vector of AttributeValue to a std::vector
            // of concrete types.  Since we don't use a recursive Variant, we need
            // to get back to a 'base' primitive type
            switch(t)
            {
                default:
                case typeTag::NOTHING:
                    throw OC::OCException(OC::Exception::INVALID_JSON_TYPE_TAG);
                    break;
                case typeTag::_string:
                    return remapArrayDepth<std::string>(curLevel, vs);
                    break;
                case typeTag::_int:
                    return remapArrayDepth<int>(curLevel, vs);
                    break;
                case typeTag::_double:
                    return remapArrayDepth<double>(curLevel, vs);
                    break;
                case typeTag::_bool:
                    return remapArrayDepth<bool>(curLevel, vs);
                    break;
                case typeTag::_representation:
                    return remapArrayDepth<OCRepresentation>(curLevel, vs);
                    break;
            }
        }

        AttributeValue parseAttributeValueArray(const GenericValue& v,
                const unsigned int curLevel, unsigned int& maxDepth, typeTag& t)
        {
            const unsigned int max_level = 3;

            if(curLevel > max_level)
            {
                throw OC::OCException(OC::Exception::INVALID_JSON_ARRAY_DEPTH);
            }

            if(curLevel > maxDepth)
            {
                maxDepth = curLevel;
            }

            auto arrayItems = gatherArrayContents(v, curLevel, maxDepth, t);
            const int remapLevel = maxDepth - (curLevel -1);
            return convertArrayToConcretes(t, remapLevel, arrayItems);
        }
    }
}

namespace cereal
{
   void JSONInputArchive::loadAttributeValues(std::map<std::string, OC::AttributeValue>& map)
   {
       for(auto&b = itsIteratorStack.back();
           b.Member && b.itsMemberItEnd != b.itsMemberItBegin+b.itsIndex;
           ++b)
       {
           std::string key = b.itsMemberItBegin[b.itsIndex].name.GetString();
           const GenericValue& v = itsIteratorStack.back().value();
           map[key] = OC::detail::parseAttributeValue(v);
       }
   }
}
