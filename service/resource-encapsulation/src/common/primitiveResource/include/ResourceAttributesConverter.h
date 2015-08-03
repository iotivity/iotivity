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

#ifndef COMMON_INTERNAL_RESOURCEATTRIBUTESCONVERTER_H
#define COMMON_INTERNAL_RESOURCEATTRIBUTESCONVERTER_H

#include <RCSResourceAttributes.h>

#include <OCRepresentation.h>

namespace OIC
{
    namespace Service
    {

        class ResourceAttributesConverter
        {
        private:
            ResourceAttributesConverter() = delete;

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
                            // RCSResourceAttributes doesn't support vector yet!
                            return;
                    }
                }

                RCSResourceAttributes&& extract()
                {
                    return std::move(m_target);
                }

            private:
                template< typename T >
                typename std::enable_if<RCSResourceAttributes::is_supported_type< T >::value >::type
                putValue(const std::string& key, T && value)
                {
                    m_target[key] = std::forward< T >(value);
                }

                template< typename T >
                typename std::enable_if<!RCSResourceAttributes::is_supported_type< T >::value >::type
                putValue(const std::string& key, T && value)
                {
                }

            private:
                RCSResourceAttributes m_target;
            };

            class OCRepresentationBuilder
            {
            public:
                OCRepresentationBuilder() = default;

                template< typename T >
                void operator()(const std::string& key, const T& value)
                {
                    m_target[key] = value;
                }

                void operator()(const std::string& key, const std::nullptr_t&)
                {
                    m_target.setNULL(key);
                }

                void operator()(const std::string& key, const RCSResourceAttributes& value)
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
            static RCSResourceAttributes fromOCRepresentation(
                    const OC::OCRepresentation& ocRepresentation)
            {
                ResourceAttributesBuilder builder;

                for (const auto& item : ocRepresentation)
                {
                    builder.insertItemTo(item);
                }

                return builder.extract();
            }

            static OC::OCRepresentation toOCRepresentation(
                    const RCSResourceAttributes& resourceAttributes)
            {
                OCRepresentationBuilder builder;

                resourceAttributes.visit(builder);

                return builder.extract();
            }
        };

    }
}

#endif // COMMON_INTERNAL_RESOURCEATTRIBUTESCONVERTER_H
