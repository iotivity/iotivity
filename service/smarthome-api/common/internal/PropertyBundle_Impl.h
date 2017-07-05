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
#ifndef SMARTHOME_API_COMMON_PROPERTYBUNDLE_IMPL_H_
#define SMARTHOME_API_COMMON_PROPERTYBUNDLE_IMPL_H_

#include <map>
#include <list>
#include <typeinfo>
#include <PayloadConverter.h>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            /*
             * Types of PropertyValues.
             */
            enum PropertyType
            {
                Unknown,
                Integer,
                Double,
                Boolean,
                String,
                Integer_Array,
                Double_Array,
                Boolean_Array,
                String_Array
            };

            /*
             * A PropertyValue interface.
             */
            class PropertyValue
            {
            public:
                PropertyValue(PropertyType t) : m_type(t)
                {
                }
                virtual ~PropertyValue(){};
                PropertyType getType()
                {
                    return m_type;
                }
            private:
                PropertyType m_type;
            };

            /*
             * Derived class for all types of PropertyValues.
             */
            template <typename T>
            class PropertyValue_ : public PropertyValue
            {
            public:
                PropertyValue_(T val, PropertyType t) : PropertyValue(t), m_value(val)
                {
                }
                virtual ~PropertyValue_(){};

                T getValue() const
                {
                    return m_value;
                }

            public:
                T m_value;
            };

            /*
             * Container class to store multiple properties.
             */
            class PropertyBundle_Impl
            {
            protected:
                template <typename T>
                bool getValue(const std::string& key, T& val) const
                {
                    std::map<std::string, PropertyValue*>::const_iterator it = m_values.find(key);
                    if (it != m_values.end())
                    {
                        PropertyValue_<T>* propValue = (PropertyValue_<T>*) it->second;
                        val = propValue->m_value;
                        return true;
                    }
                    val = T();
                    return false;
                }

                template <typename T>
                T getValue(const std::string& key) const
                {
                    T val = T();
                    std::map<std::string, PropertyValue*>::const_iterator it = m_values.find(key);
                    if (it != m_values.end())
                    {
                        PropertyValue_<T>* propValue = (PropertyValue_<T>*) it->second;
                        val = propValue->m_value;
                    }
                    return val;
                }

                template <typename T>
                bool setValue(const std::string& key, const T val)
                {
                    // Check if input property type exists in supported type lists.
                    PropertyType type = checkPropertyType(val);
                    if (type == Unknown)
                    {
                        // TODO: Throw invalid property type exception.
                        return false;
                    }
                    m_values[key] = (PropertyValue*) new PropertyValue_<T>(val, type);
                    return true;
                }

                bool contains(const std::string& key) const
                {
                    std::map<std::string, PropertyValue*>::const_iterator it = m_values.find(key);
                    if (it != m_values.end())
                    {
                        return true;
                    }
                    return false;
                }

            private:
                friend class PayloadConverter;

                template <typename T>
                PropertyType checkPropertyType(T& val);
                PropertyType getType(const std::string& key);
                OCRepPayload* getPayload() const;
                void setPayload(const OCRepPayload* pl);
                void getPayloadArray(OCRepPayload* payload,
                                     std::string key, PropertyType type) const;
                void setPayloadArray(const OCRepPayloadValue* pl);
                size_t calcArrayDepth(const size_t dimensions[MAX_REP_ARRAY_DEPTH]);
                template<typename T>
                void payload_array_helper(const OCRepPayloadValue* pl, size_t depth);
                template<typename T>
                T payload_array_helper_copy(size_t index, const OCRepPayloadValue* pl);

            private:
                std::map<std::string, PropertyValue* > m_values;
            };
        }
    }
}
#endif /* SMARTHOME_API_COMMON_PROPERTYBUNDLE_IMPL_H_ */
