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
#ifndef SMARTHOME_API_COMMON_PROPERTYBUNDLE_H_
#define SMARTHOME_API_COMMON_PROPERTYBUNDLE_H_

#include <iostream>
#include <map>
#include <list>
#include <stdint.h>
#include <CommonException.h>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            namespace utils
            {
                /*
                 * Template Structure.
                 * Provides the member value equal to true. Otherwise value is false.
                 */
                struct true_type
                {
                    enum _value_ { value = true};
                };

                struct false_type
                {
                    enum _value_ { value = false};
                };

                template<typename T, typename U>
                struct is_same : false_type {};

                template<typename T>
                struct is_same<T, T> : true_type {};
            }

            class PropByteString
            {
            public:
                uint8_t* bytes;
                size_t   len;

                PropByteString()
                {
                }

                PropByteString(uint8_t* bytes, size_t size)
                : bytes(bytes), len(size)
                {
                }
            };

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
                ByteString,
                PropertyBundle_Object,
                Integer_Array,
                Double_Array,
                Boolean_Array,
                String_Array,
                PropertyValue_Array,
                ByteString_Array
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
            class PropertyBundle
            {
            friend class PayloadConverter;
            public:
                template <typename T>
                void getValue(const std::string& key, T& val) const
                {
                    std::map<std::string, PropertyValue*>::const_iterator it = m_values.find(key);
                    if (it == m_values.end())
                    {
                        throw CommonException("Invalid property key exception");
                    }
                    PropertyValue_<T>* propValue = (PropertyValue_<T>*) it->second;
                    val = propValue->m_value;
                }

                template <typename T>
                T getValue(const std::string& key) const
                {
                    std::map<std::string, PropertyValue*>::const_iterator it = m_values.find(key);
                    if (it == m_values.end())
                    {
                        throw CommonException("Invalid property key exception");
                    }
                    PropertyValue_<T>* propValue = (PropertyValue_<T>*) it->second;
                    T val = propValue->m_value;
                    return val;
                }

                template <typename T>
                void setValue(const std::string& key, const T& val)
                {
                    // Check if input property type exists in supported type lists.
                    PropertyType type = checkPropertyType(val);
                    if (type == Unknown)
                    {
                        throw CommonException("Invalid property type exception");
                    }
                    m_values[key] = (PropertyValue*) new PropertyValue_<T>(val, type);
                }

                bool contains(const std::string& key) const;
                size_t size() const;
                PropertyType getType(const std::string& key);

            private:
                template <typename T>
                PropertyType checkPropertyType(const T& val)
                {
                    (void) val;
                    PropertyType type = Unknown;
                    if (utils::is_same<T, int>::value)
                    {
                        type = Integer;
                    }
                    else if (utils::is_same<T, double>::value)
                    {
                        type = Double;
                    }
                    else if (utils::is_same<T, bool>::value)
                    {
                        type = Boolean;
                    }
                    else if (utils::is_same<T, std::string>::value)
                    {
                        type = String;
                    }
                    else if (utils::is_same<T, PropByteString>::value)
                    {
                        type = ByteString;
                    }
                    else if (utils::is_same<T, PropertyBundle>::value)
                    {
                        type = PropertyBundle_Object;
                    }
                    else if (utils::is_same<T, std::list<int> >::value)
                    {
                        type = Integer_Array;
                    }
                    else if (utils::is_same<T, std::list<double> >::value)
                    {
                        type = Double_Array;
                    }
                    else if (utils::is_same<T, std::list<bool> >::value)
                    {
                        type = Boolean_Array;
                    }
                    else if (utils::is_same<T, std::list<std::string> >::value)
                    {
                        type = String_Array;
                    }
                    else if (utils::is_same<T, std::list<PropertyBundle> >::value)
                    {
                        type = PropertyValue_Array;
                    }
                    else if (utils::is_same<T, std::list<PropByteString> >::value)
                    {
                        type = ByteString_Array;
                    }
                    return type;
                }

                std::map<std::string, PropertyValue* > m_values;
            };
        }
    }
}

#endif /* SMARTHOME_API_COMMON_PROPERTYBUNDLE_H_ */
