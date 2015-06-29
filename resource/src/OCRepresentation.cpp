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

/**
 * @file
 *
 * This file contains the implementation of classes and its members related
 * to OCRepresentation.
 */


#include <OCRepresentation.h>

#include <boost/lexical_cast.hpp>
#include <algorithm>
#include "ocpayload.h"

namespace OC
{

    void MessageContainer::setPayload(const OCRepPayload* payload)
    {
        const OCRepPayload* pl = payload;
        while(pl)
        {
            OCRepresentation cur;
            cur.setUri(pl->uri);

            OCStringLL* ll = pl->types;
            while(ll)
            {
                cur.addResourceType(ll->value);
                ll = ll->next;
            }

            ll = pl->interfaces;
            while(ll)
            {
                cur.addResourceInterface(ll->value);
                ll = ll->next;
            }

            OCRepPayloadValue* val = pl->values;

            while(val)
            {
                switch(val->type)
                {
                    case OCREP_PROP_INT:
                        cur.setValue<int>(val->name, val->i);
                        break;
                    case OCREP_PROP_BOOL:
                        cur.setValue<bool>(val->name, val->b);
                        break;
                    case OCREP_PROP_STRING:
                        cur.setValue<std::string>(val->name, val->str);
                        break;
                    default:
                        throw std::logic_error(std::string("Not Implemented!") +
                                std::to_string((int)val->type));
                        break;
                }
                val = val->next;
            }

            pl = pl->next;
            this->addRepresentation(cur);
        }
    }

    OCRepPayload* MessageContainer::getPayload() const
    {
        OCRepPayload* root = nullptr;
        for(const auto& r : representations())
        {
            if(!root)
            {
                root = r.getPayload();
            }
            else
            {
                OCRepPayloadAppend(root, r.getPayload());
            }
        }

        return root;
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
    OCRepPayload* OCRepresentation::getPayload() const
    {
        OCRepPayload* root = OCRepPayloadCreate();
        if(!root)
        {
            throw std::bad_alloc();
        }

        OCRepPayloadSetUri(root, getUri().c_str());

        for(const std::string& type : getResourceTypes())
        {
            OCRepPayloadAddResourceType(root, type.c_str());
        }

        for(const std::string& iface : getResourceInterfaces())
        {
            OCRepPayloadAddInterface(root, iface.c_str());
        }

        for(auto& val : *this)
        {
            switch(val.type())
            {
                case AttributeType::Integer:
                    OCRepPayloadSetPropInt(root, val.attrname().c_str(), static_cast<int>(val));
                    break;
                case AttributeType::Boolean:
                    OCRepPayloadSetPropBool(root, val.attrname().c_str(), val);
                    break;
                case AttributeType::String:
                    OCRepPayloadSetPropString(root, val.attrname().c_str(),
                            static_cast<std::string>(val).c_str());
                    break;
                default:
                    throw std::logic_error(std::string("Getpayload: Not Implemented") +
                            std::to_string((int)val.type()));
                    break;
            }
        }

        OCRepPayload* cur = root;
        for(auto& child : this->getChildren())
        {
            cur->next = child.getPayload();
            cur = cur->next;
        }

        return root;
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

    void OCRepresentation::setChildren(const std::vector<OCRepresentation>& children)
    {
        m_children = children;
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

    void OCRepresentation::addResourceType(const std::string& str)
    {
        m_resourceTypes.push_back(str);
    }

    const std::vector<std::string>& OCRepresentation::getResourceInterfaces() const
    {
        return m_interfaces;
    }

    void OCRepresentation::addResourceInterface(const std::string& str)
    {
        m_interfaces.push_back(str);
    }

    void OCRepresentation::setResourceInterfaces(const std::vector<std::string>& resourceInterfaces)
    {
        m_interfaces = resourceInterfaces;
    }

    bool OCRepresentation::hasAttribute(const std::string& str) const
    {
        return m_values.find(str) != m_values.end();
    }

    bool OCRepresentation::emptyData() const
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
    std::ostream& operator <<(std::ostream& os, const AttributeType at)
    {
        switch(at)
        {
            case AttributeType::Null:
                os << "Null";
                break;
            case AttributeType::Integer:
                os << "Integer";
                break;
            case AttributeType::Double:
                os << "Double";
                break;
            case AttributeType::Boolean:
                os << "Boolean";
                break;
            case AttributeType::String:
                os << "String";
                break;
            case AttributeType::OCRepresentation:
                os << "OCRepresentation";
                break;
            case AttributeType::Vector:
                os << "Vector";
                break;
        }
        return os;
    }
}

// STL Container For OCRepresentation
namespace OC
{
    OCRepresentation::AttributeItem::AttributeItem(const std::string& name,
            std::map<std::string, AttributeValue>& vals):
            m_attrName(name), m_values(vals){}

    OCRepresentation::AttributeItem OCRepresentation::operator[](const std::string& key)
    {
        OCRepresentation::AttributeItem attr{key, m_values};
        return std::move(attr);
    }

    const OCRepresentation::AttributeItem OCRepresentation::operator[](const std::string& key) const
    {
        OCRepresentation::AttributeItem attr{key, m_values};
        return std::move(attr);
    }

    const std::string& OCRepresentation::AttributeItem::attrname() const
    {
        return m_attrName;
    }

    template<typename T, typename = void>
    struct type_info
    {
        // contains the actual type
        typedef T type;
        // contains the inner most vector-type
        typedef T base_type;
        // contains the AttributeType for this item
        constexpr static AttributeType enum_type =
            AttributeTypeConvert<T>::type;
        // contains the AttributeType for this base-type
        constexpr static AttributeType enum_base_type =
            AttributeTypeConvert<T>::type;
        // depth of the vector
        constexpr static size_t depth = 0;
    };

    template<typename T>
    struct type_info<T, typename std::enable_if<is_vector<T>::value>::type>
    {
        typedef T type;
        typedef typename type_info<typename T::value_type>::base_type base_type;
        constexpr static AttributeType enum_type = AttributeType::Vector;
        constexpr static AttributeType enum_base_type =
            type_info<typename T::value_type>::enum_base_type;
        constexpr static size_t depth = 1 +
            type_info<typename T::value_type>::depth;
    };

    struct type_introspection_visitor : boost::static_visitor<>
    {
        AttributeType type;
        AttributeType base_type;
        size_t depth;

        type_introspection_visitor() : boost::static_visitor<>(),
            type(AttributeType::Null), base_type(AttributeType::Null), depth(0){}

        template <typename T>
        void operator()(T const& item)
        {
            type = type_info<T>::enum_type;
            base_type = type_info<T>::enum_base_type;
            depth = type_info<T>::depth;
        }
    };

    AttributeType OCRepresentation::AttributeItem::type() const
    {
        type_introspection_visitor vis;
        boost::apply_visitor(vis, m_values[m_attrName]);
        return vis.type;
    }

    AttributeType OCRepresentation::AttributeItem::base_type() const
    {
        type_introspection_visitor vis;
        boost::apply_visitor(vis, m_values[m_attrName]);
        return vis.base_type;
    }

    size_t OCRepresentation::AttributeItem::depth() const
    {
        type_introspection_visitor vis;
        boost::apply_visitor(vis, m_values[m_attrName]);
        return vis.depth;
    }

    OCRepresentation::iterator OCRepresentation::begin()
    {
        return OCRepresentation::iterator(m_values.begin(), m_values);
    }

    OCRepresentation::const_iterator OCRepresentation::begin() const
    {
         return OCRepresentation::const_iterator(m_values.begin(), m_values);
    }

    OCRepresentation::const_iterator OCRepresentation::cbegin() const
    {
        return OCRepresentation::const_iterator(m_values.cbegin(), m_values);
    }

    OCRepresentation::iterator OCRepresentation::end()
    {
        return OCRepresentation::iterator(m_values.end(), m_values);
    }

    OCRepresentation::const_iterator OCRepresentation::end() const
    {
        return OCRepresentation::const_iterator(m_values.end(), m_values);
    }

    OCRepresentation::const_iterator OCRepresentation::cend() const
    {
        return OCRepresentation::const_iterator(m_values.cend(), m_values);
    }

    size_t OCRepresentation::size() const
    {
        return m_values.size();
    }

    bool OCRepresentation::empty() const
    {
        return m_values.empty();
    }

    bool OCRepresentation::iterator::operator==(const OCRepresentation::iterator& rhs) const
    {
        return m_iterator == rhs.m_iterator;
    }

    bool OCRepresentation::iterator::operator!=(const OCRepresentation::iterator& rhs) const
    {
        return m_iterator != rhs.m_iterator;
    }

    bool OCRepresentation::const_iterator::operator==(
            const OCRepresentation::const_iterator& rhs) const
    {
        return m_iterator == rhs.m_iterator;
    }

    bool OCRepresentation::const_iterator::operator!=(
            const OCRepresentation::const_iterator& rhs) const
    {
        return m_iterator != rhs.m_iterator;
    }

    OCRepresentation::iterator::reference OCRepresentation::iterator::operator*()
    {
        return m_item;
    }

    OCRepresentation::const_iterator::const_reference
        OCRepresentation::const_iterator::operator*() const
    {
        return m_item;
    }

    OCRepresentation::iterator::pointer OCRepresentation::iterator::operator->()
    {
        return &m_item;
    }

    OCRepresentation::const_iterator::const_pointer
        OCRepresentation::const_iterator::operator->() const
    {
        return &m_item;
    }

    OCRepresentation::iterator& OCRepresentation::iterator::operator++()
    {
        m_iterator++;
        if(m_iterator != m_item.m_values.end())
        {
            m_item.m_attrName = m_iterator->first;
        }
        else
        {
            m_item.m_attrName = "";
        }
        return *this;
    }

    OCRepresentation::const_iterator& OCRepresentation::const_iterator::operator++()
    {
        m_iterator++;
        if(m_iterator != m_item.m_values.end())
        {
            m_item.m_attrName = m_iterator->first;
        }
        else
        {
            m_item.m_attrName = "";
        }
        return *this;
    }

    OCRepresentation::iterator OCRepresentation::iterator::operator++(int)
    {
        OCRepresentation::iterator itr(*this);
        ++(*this);
        return itr;
    }

    OCRepresentation::const_iterator OCRepresentation::const_iterator::operator++(int)
    {
        OCRepresentation::const_iterator itr(*this);
        ++(*this);
        return itr;
    }

    struct to_string_visitor : boost::static_visitor<>
    {
        std::string str;
        template <typename T>
        void operator()(T const& item)
        {
            str = boost::lexical_cast<std::string>(item);
        }

        template <typename T>
        void operator()(std::vector<T> const& item)
        {
            to_string_visitor vis;
            std::ostringstream stream;
            stream << "[";

            for(const auto& i : item)
            {
                vis(i);
                stream << vis.str  << " ";
            }
            stream << "]";
            str = stream.str();
        }
    };

    template<>
    void to_string_visitor::operator()(bool const& item)
    {
        str = item ? "true" : "false";
    }

    template<>
    void to_string_visitor::operator()(std::string const& item)
    {
        str = item;
    }

    template<>
    void to_string_visitor::operator()(NullType const& item)
    {
        str = "(null)";
    }

    template<>
    void to_string_visitor::operator()(OCRepresentation const& item)
    {
        str = "OC::OCRepresentation";
    }

    std::string OCRepresentation::getValueToString(const std::string& key) const
    {
        auto x = m_values.find(key);
        if(x != m_values.end())
        {
            to_string_visitor vis;
            boost::apply_visitor(vis, x->second);
            return std::move(vis.str);
        }

        return "";
    }

    std::string OCRepresentation::AttributeItem::getValueToString() const
    {
        to_string_visitor vis;
        boost::apply_visitor(vis, m_values[m_attrName]);
        return std::move(vis.str);
    }

    std::ostream& operator<<(std::ostream& os, const OCRepresentation::AttributeItem& ai)
    {
        os << ai.getValueToString();
        return os;
    }
}

