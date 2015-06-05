//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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
#ifndef __INTERNALUTIL_H
#define __INTERNALUTIL_H

#include "ResourceAttributes.h"

#include <OCRepresentation.h>

class ResourceAttributesConverter
{
private:
    ResourceAttributesConverter() = delete;

    template< typename T >
    using SupportedType = typename std::enable_if<
    ResourceAttributes::is_supported_type< T >::type::value, T >::type;

    template< typename T >
    using UnsupportedType = typename std::enable_if<
    !ResourceAttributes::is_supported_type< T >::type::value, T >::type;

    class ResourceAttributesBuilder
    {
    public:
        ResourceAttributesBuilder() = default;

        void insertItemTo(const OC::OCRepresentation::AttributeItem& item)
        {
            switch (item.type())
            {
                case OC::AttributeType::Null:
                    return putValue(item.attrname(), nullptr);

                case OC::AttributeType::Integer:
                    return putValue(item.attrname(), item.getValue< int >());

                case OC::AttributeType::Double:
                    return putValue(item.attrname(), item.getValue< double >());

                case OC::AttributeType::Boolean:
                    return putValue(item.attrname(), item.getValue< bool >());

                case OC::AttributeType::String:
                    return putValue(item.attrname(), item.getValue< std::string >());

                case OC::AttributeType::OCRepresentation:
                    return putValue(item.attrname(),
                            ResourceAttributesConverter::fromOCRepresentation(
                                    item.getValue< OC::OCRepresentation >()));

                case OC::AttributeType::Vector:
                    // ResourceAttributes doesn't support vector yet!
                    return;
            }
        }

        ResourceAttributes&& extract()
        {
            return std::move(m_target);
        }

    private:
        template< typename T >
        void putValue(const std::string key, T&& value)
        {
            putValue< T >(key, std::forward< T >(value));
        }

        template< typename T >
        void putValue(const std::string key, SupportedType< T > && value)
        {
            m_target[key] = std::forward< T >(value);
        }

        template< typename T >
        void putValue(const std::string key, UnsupportedType< T > && value)
        {
        }

    private:
        ResourceAttributes m_target;
    };

    class AttrVisitor
    {
    public:
        AttrVisitor() = default;

        template< typename T >
        void operator()(const std::string& key, const T& value)
        {
            m_target[key] = value;
        }

        void operator()(const std::string& key, const std::nullptr_t&)
        {
            m_target.setNULL(key);
        }

        void operator()(const std::string& key, const ResourceAttributes& value)
        {
            m_target[key] = ResourceAttributesConverter::toOCRepresentation(value);
        }

        OC::OCRepresentation&& extract()
        {
            return std::move(m_target);
        }

    private:
        OC::OCRepresentation m_target;
    };

public:
    static ResourceAttributes fromOCRepresentation(const OC::OCRepresentation& ocRepresentation)
    {
        ResourceAttributesBuilder builder;

        for (const auto& item : ocRepresentation)
        {
            builder.insertItemTo(item);
        }

        return builder.extract();
    }

    static OC::OCRepresentation toOCRepresentation(const ResourceAttributes& resourceAttributes)
    {
        AttrVisitor visitor;

        resourceAttributes.visit(visitor);

        return visitor.extract();
    }
};

#endif // __INTERNALUTIL_H
