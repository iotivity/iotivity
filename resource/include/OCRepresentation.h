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
 * This file contains the declaration of classes and its members related
 * to OCRepresentation.
 */

#ifndef OC_REPRESENTATION_H_
#define OC_REPRESENTATION_H_

#include <string>
#include <sstream>
#include <vector>
#include <map>

#include <AttributeValue.h>
#include <StringConstants.h>

#include "platform_features.h"

#ifdef __ANDROID__
#include "OCAndroid.h"
#endif

#include <OCException.h>
#include <OCRepresentationInternal.h>

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

    class MessageContainer
    {
        public:
            void setPayload(const OCPayload* rep);

            void setPayload(const OCRepPayload* rep);

            OCRepPayload* getPayload() const;

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
        // Verifies if a type is supported by AttributeValue in order to enable
        // the correct template for a given type.
        template <typename T>
        struct IsSupportedType {
            static const bool value =
                std::is_same<typename std::remove_cv<typename std::remove_reference<T>::type>::type, int64_t>::value ||
                std::is_same<typename std::remove_cv<typename std::remove_reference<T>::type>::type, double>::value ||
                std::is_same<typename std::remove_cv<typename std::remove_reference<T>::type>::type, bool>::value ||
                std::is_same<typename std::remove_cv<typename std::remove_reference<T>::type>::type, std::string>::value ||
                std::is_same<typename std::remove_cv<typename std::remove_reference<T>::type>::type, OCRepresentation>::value ||
                std::is_same<typename std::remove_cv<typename std::remove_reference<T>::type>::type, OCByteString>::value ||
                std::is_same<typename std::remove_cv<typename std::remove_reference<T>::type>::type, std::vector<int64_t>>::value ||
                std::is_same<typename std::remove_cv<typename std::remove_reference<T>::type>::type, std::vector<std::vector<int64_t>>>::value ||
                std::is_same<typename std::remove_cv<typename std::remove_reference<T>::type>::type, std::vector<std::vector<std::vector<int64_t>>>>::value ||
                std::is_same<typename std::remove_cv<typename std::remove_reference<T>::type>::type, std::vector<double>>::value ||
                std::is_same<typename std::remove_cv<typename std::remove_reference<T>::type>::type, std::vector<std::vector<double>>>::value ||
                std::is_same<typename std::remove_cv<typename std::remove_reference<T>::type>::type, std::vector<std::vector<std::vector<double>>>>::value ||
                std::is_same<typename std::remove_cv<typename std::remove_reference<T>::type>::type, std::vector<bool>>::value ||
                std::is_same<typename std::remove_cv<typename std::remove_reference<T>::type>::type, std::vector<std::vector<bool>>>::value ||
                std::is_same<typename std::remove_cv<typename std::remove_reference<T>::type>::type, std::vector<std::vector<std::vector<bool>>>>::value ||
                std::is_same<typename std::remove_cv<typename std::remove_reference<T>::type>::type, std::vector<std::string>>::value ||
                std::is_same<typename std::remove_cv<typename std::remove_reference<T>::type>::type, std::vector<std::vector<std::string>>>::value ||
                std::is_same<typename std::remove_cv<typename std::remove_reference<T>::type>::type, std::vector<std::vector<std::vector<std::string>>>>::value ||
                std::is_same<typename std::remove_cv<typename std::remove_reference<T>::type>::type, std::vector<OCRepresentation>>::value ||
                std::is_same<typename std::remove_cv<typename std::remove_reference<T>::type>::type, std::vector<std::vector<OCRepresentation>>>::value ||
                std::is_same<typename std::remove_cv<typename std::remove_reference<T>::type>::type, std::vector<std::vector<std::vector<OCRepresentation>>>>::value ||
                std::is_same<typename std::remove_cv<typename std::remove_reference<T>::type>::type, std::vector<OCByteString>>::value ||
                std::is_same<typename std::remove_cv<typename std::remove_reference<T>::type>::type, std::vector<std::vector<OCByteString>>>::value ||
                std::is_same<typename std::remove_cv<typename std::remove_reference<T>::type>::type, std::vector<std::vector<std::vector<OCByteString>>>>::value ||
                std::is_same<typename std::remove_cv<typename std::remove_reference<T>::type>::type, std::vector<uint8_t>>::value ||
                std::is_same<typename std::remove_cv<typename std::remove_reference<T>::type>::type, AttributeValue>::value ||
                std::is_same<typename std::remove_cv<typename std::remove_reference<T>::type>::type, NullType>::value;
        };

        public:
            friend bool operator==(const OC::OCRepresentation&, const OC::OCRepresentation&);
            // Note: Implementation of all constructors and destructors
            // are all placed in the same location due to a crash that
            // was observed in Android, where merely constructing/destructing
            // an OCRepresentation object was enough to cause an invalid 'free'.
            // It is believed that this is a result of incompatible compiler
            // options between the gradle JNI and armeabi scons build, however
            // this fix will work in the meantime.
            OCRepresentation(): m_interfaceType(InterfaceType::None){}

#if defined(_MSC_VER) && (_MSC_VER < 1900)
            OCRepresentation(OCRepresentation&& o)
            {
                std::memmove(this, &o, sizeof(o));
            }
#else
            OCRepresentation(OCRepresentation&&) = default;
#endif

            OCRepresentation(const OCRepresentation&) = default;

            OCRepresentation& operator=(const OCRepresentation&) = default;

#if defined(_MSC_VER) && (_MSC_VER < 1900)
            OCRepresentation& operator=(OCRepresentation&& o)
            {
                std::memmove(this, &o, sizeof(o));
                return *this;
            }
#else
            OCRepresentation& operator=(OCRepresentation&&) = default;
#endif

            virtual ~OCRepresentation(){}

            void setDevAddr(const OCDevAddr&);

            const std::string getHost() const;

            OCRepPayload* getPayload() const;

            void addChild(const OCRepresentation&);

            void clearChildren();

            const std::vector<OCRepresentation>& getChildren() const;

            void setChildren(const std::vector<OCRepresentation>& children);

            void setUri(const char* uri);

            void setUri(const std::string& uri);

            std::string getUri() const;

            const std::vector<std::string>& getResourceTypes() const;

            const std::vector<std::string>& getDataModelVersions() const;

            void setResourceTypes(const std::vector<std::string>& resourceTypes);

            void addResourceType(const std::string& str);

            const std::vector<std::string>& getResourceInterfaces() const;

            void setResourceInterfaces(const std::vector<std::string>& resourceInterfaces);

            void addResourceInterface(const std::string& str);

            void addDataModelVersion(const std::string& str);

            bool emptyData() const;

            size_t numberOfAttributes() const;

            bool erase(const std::string& str);

            // Implementation is different for all int-related types, because AttributeValue
            // uses only one (int64_t) type for integers.
            template<typename T, typename std::enable_if<IsSupportedType<T>::value, int>::type = 0>
            void setValue(const std::string& str, const T& rhs)
            {
                m_values[str] = rhs;
            }

            template<typename T, typename std::enable_if<!IsSupportedType<T>::value, int>::type = 0>
            void setValue(const std::string& str, const T& rhs)
            {
                assignAttributeValueContent(rhs, m_values[str]);
            }

            // using R-value(or universal ref depending) to move string and vector<uint8_t>
            // Implementation is different for all int-related types, because AttributeValue
            // uses only one (int64_t) type for integers.
            template<typename T, typename std::enable_if<IsSupportedType<T>::value, int>::type = 0>
            void setValue(const std::string& str, T&& rhs)
            {
                m_values[str] = std::forward<T>(rhs);
            }

            template<typename T, typename std::enable_if<!IsSupportedType<T>::value, int>::type = 0>
            void setValue(const std::string& str, T&& rhs)
            {
                assignAttributeValueContent(rhs, m_values[str]);
            }

            const std::map<std::string, AttributeValue>& getValues() const {
                return m_values;
            }

            /**
             *  Retrieve the attribute value associated with the supplied name.
             *  Implementation is different for all int-related types, because AttributeValue
             *  uses only one (int64_t) type for integers.
             *
             *  @param str Name of the attribute
             *  @param val Value of the attribute
             *  @return The getValue method returns true if the attribute was
             *        found in the representation. Otherwise it returns false.
             *
             *  @throws OCException if retrieving 'int' and the value is outside
             *                      of 'int' range.
             */
            template<typename T, typename std::enable_if<IsSupportedType<T>::value, int>::type = 0>
            bool getValue(const std::string& str, T& val) const
            {
                auto x = m_values.find(str);

                if (x != m_values.end())
                {
                    try
                    {
                        val = boost::get<T>(x->second);
                        return true;
                    }
                    catch (boost::bad_get&)
                    {
                    }
                }
                val = T();
                return false;
            }

            template<typename T, typename std::enable_if<!IsSupportedType<T>::value, int>::type = 0>
            bool getValue(const std::string& str, T& val) const
            {
                auto item = m_values.find(str);

                if (item != m_values.end())
                {
                    try
                    {
                        retrieveAttributeValueContent(item->second, val);
                        return true;
                    }
                    catch (boost::bad_get&)
                    {
                    }
                }
                val = T();
                return false;
            }

            /**
             *  Return the attribute value associated with the supplied name.
             *  Implementation is different for all int-related types, because AttributeValue
             *  uses only one (int64_t) type for integers.
             *
             *  @param str Name of the attribute
             *  @return When the representation contains the attribute, the
             *       associated value is returned. Otherwise, getValue
             *       returns the default contructed value for the type.
             *
             *  @throws OCException if retrieving 'int' and the value is outside
             *                      of 'int' range.
             */
            template<typename T, typename std::enable_if<IsSupportedType<T>::value, int>::type = 0>
            T getValue(const std::string& str) const
            {
                auto x = m_values.find(str);
                if (x != m_values.end())
                {
                    try
                    {
                        return boost::get<T>(x->second);
                    }
                    catch (boost::bad_get&)
                    {
                    }
                }
                return T();
            }

            template<typename T, typename std::enable_if<!IsSupportedType<T>::value, int>::type = 0>
            T getValue(const std::string& str) const
            {
                T val = T();
                auto x = m_values.find(str);
                if (x != m_values.end())
                {
                    try
                    {
                        retrieveAttributeValueContent(x->second, val);
                    }
                    catch (boost::bad_get&)
                    {
                    }
                }
                return val;
            }

           /**
            *  Retrieve the attributevalue structure associated with the supplied name
            *
            *  @param str Name of the attribute
            *  @param attrValue Attribute Value structure
            *  @return The getAttributeValue method returns true if the attribute was
            *        found in the representation.  Otherwise it returns false.
            */
            bool getAttributeValue(const std::string& str, AttributeValue& attrValue) const
            {
                auto x = m_values.find(str);

                if (x != m_values.end())
                {
                    attrValue = x->second;
                    return true;
                }
                else
                {
                    return false;
                }
            }

            std::string getValueToString(const std::string& key) const;
            bool hasAttribute(const std::string& str) const;

            void setNULL(const std::string& str);

            bool isNULL(const std::string& str) const;

        private:
            std::string m_host;

            // STL Container stuff
        public:
            class iterator;
            class const_iterator;
            // Shim class to allow iterating and indexing of the OCRepresentation
            // object.
            class AttributeItem
            {
                friend class OCRepresentation;
                friend class iterator;
                friend class const_iterator;
                public:
                    const std::string& attrname() const;
                    AttributeType type() const;
                    AttributeType base_type() const;
                    size_t depth() const;

                    template<typename T, typename std::enable_if<IsSupportedType<T>::value, int>::type = 0>
                    T getValue() const
                    {
                        try
                        {
                            return boost::get<T>(m_values.at(m_attrName));
                        }
                        catch (boost::bad_get&)
                        {
                            T val = T();
                            return val;
                        }
                    }

                    template<typename T, typename std::enable_if<!IsSupportedType<T>::value, int>::type = 0>
                    T getValue() const
                    {
                        T val = T();
                        try
                        {
                            retrieveAttributeValueContent(m_values.at(m_attrName), val);
                        }
                        catch (boost::bad_get&)
                        {
                        }
                        return val;
                    }

                    std::string getValueToString() const;

                    template<typename T, typename std::enable_if<IsSupportedType<T>::value, int>::type = 0>
                    AttributeItem& operator=(T&& rhs)
                    {
                        m_values[m_attrName] = std::forward<T>(rhs);
                        return *this;
                    }

                    template<typename T, typename std::enable_if<!IsSupportedType<T>::value, int>::type = 0>
                    AttributeItem& operator=(T&& rhs)
                    {
                        assignAttributeValueContent(rhs, m_values[m_attrName]);
                        return *this;
                    }

                    AttributeItem& operator=(std::nullptr_t /*rhs*/)
                    {
                        NullType t;
                        m_values[m_attrName] = t;
                        return *this;
                    }

                    // Enable-if required to prevent conversions to alternate types.  This prevents
                    // ambigious conversions in the case where conversions can include a number of
                    // types, such as the string constructor.
#if (defined(_MSC_VER) ) || (defined(__GNUC__) && (__GNUC__ <= 5))
                    template<typename T, typename std::enable_if<
                     std::is_same<T, int>::value ||
                     std::is_same<T, int64_t>::value ||
                     std::is_same<T, double>::value ||
                     std::is_same<T, bool>::value ||
                     std::is_same<T, std::string>::value ||
                     std::is_same<T, OCRepresentation>::value ||
                     std::is_same<T, OCByteString>::value ||
                     std::is_same<T, std::vector<int>>::value ||
                     std::is_same<T, std::vector<std::vector<int>>>::value ||
                     std::is_same<T, std::vector<std::vector<std::vector<int>>>>::value ||
                     std::is_same<T, std::vector<int64_t>>::value ||
                     std::is_same<T, std::vector<std::vector<int64_t>>>::value ||
                     std::is_same<T, std::vector<std::vector<std::vector<int64_t>>>>::value ||
                     std::is_same<T, std::vector<double>>::value ||
                     std::is_same<T, std::vector<std::vector<double>>>::value ||
                     std::is_same<T, std::vector<std::vector<std::vector<double>>>>::value ||
                     std::is_same<T, std::vector<bool>>::value ||
                     std::is_same<T, std::vector<std::vector<bool>>>::value ||
                     std::is_same<T, std::vector<std::vector<std::vector<bool>>>>::value ||
                     std::is_same<T, std::vector<std::string>>::value ||
                     std::is_same<T, std::vector<std::vector<std::string>>>::value ||
                     std::is_same<T, std::vector<std::vector<std::vector<std::string>>>>::value ||
                     std::is_same<T, std::vector<OCRepresentation>>::value ||
                     std::is_same<T, std::vector<std::vector<OCRepresentation>>>::value ||
                     std::is_same<T, std::vector<std::vector<std::vector<OCRepresentation>>>>::value ||
                     std::is_same<T, std::vector<OCByteString>>::value ||
                     std::is_same<T, std::vector<std::vector<OCByteString>>>::value ||
                     std::is_same<T, std::vector<std::vector<std::vector<OCByteString>>>>::value
                     , int>::type = 0// enable_if
                    >
#else
                    template<typename T, typename std::enable_if<
                        is_component<T,
                            remove_first<AttributeValue>::type
                            >::value
                        , int>::type = 0
                    >
#endif
                    operator T() const
                    {
                        return this->getValue<T>();
                    }

                    template<typename T, typename std::enable_if<
                        std::is_same<T, std::nullptr_t>::value
                        , int>::type = 0
                    >
                    operator T() const
                    {
                        this->getValue<NullType>();
                        return nullptr;
                    }

                private:
                    AttributeItem(const std::string& name,
                            std::map<std::string, AttributeValue>& vals);
                    AttributeItem(const AttributeItem&) = default;
                    std::string m_attrName;
                    std::map<std::string, AttributeValue>& m_values;
            };

            // Iterator to allow iteration via STL containers/methods
            class iterator
            {
                friend class OCRepresentation;
                public:
                    typedef iterator self_type;
                    typedef AttributeItem value_type;
                    typedef value_type& reference;
                    typedef value_type* pointer;
                    typedef std::forward_iterator_tag iterator_category;
                    typedef int difference_type;

                    iterator(const iterator&) = default;
                    ~iterator() = default;

                    bool operator ==(const iterator&) const;
                    bool operator !=(const iterator&) const;

                    iterator& operator++();
                    iterator operator++(int);

                    reference operator*();
                    pointer operator->();
                private:
                    iterator(std::map<std::string, AttributeValue>::iterator&& itr,
                            std::map<std::string, AttributeValue>& vals)
                        : m_iterator(std::move(itr)),
                        m_item(m_iterator != vals.end() ? m_iterator->first:"", vals){}
                    std::map<std::string, AttributeValue>::iterator m_iterator;
                    AttributeItem m_item;
            };

            class const_iterator
            {
                friend class OCRepresentation;
                public:
                    typedef iterator self_type;
                    typedef const AttributeItem value_type;
                    typedef value_type& const_reference;
                    typedef value_type* const_pointer;
                    typedef std::forward_iterator_tag iterator_category;
                    typedef int difference_type;

                    const_iterator(const iterator& rhs)
                        :m_iterator(rhs.m_iterator), m_item(rhs.m_item){}
                    const_iterator(const const_iterator&) = default;
                    ~const_iterator() = default;

                    bool operator ==(const const_iterator&) const;
                    bool operator !=(const const_iterator&) const;

                    const_iterator& operator++();
                    const_iterator operator++(int);

                    const_reference operator*() const;
                    const_pointer operator->() const;
                private:
                    const_iterator(std::map<std::string, AttributeValue>::const_iterator&& itr,
                            std::map<std::string, AttributeValue>& vals)
                        : m_iterator(std::move(itr)),
                        m_item(m_iterator != vals.end() ? m_iterator->first: "", vals){}
                    std::map<std::string, AttributeValue>::const_iterator m_iterator;
                    AttributeItem m_item;
            };

            iterator begin();
            const_iterator begin() const;
            const_iterator cbegin() const;
            iterator end();
            const_iterator end() const;
            const_iterator cend() const;
            size_t size() const;
            bool empty() const;

            AttributeItem operator[](const std::string& key);
            const AttributeItem operator[](const std::string& key) const;
        private:
            friend class OCResourceResponse;
            friend class MessageContainer;

            static void retrieveAttributeValueContent(const AttributeValue& attributeValue, int& val);
            static void retrieveAttributeValueContent(const AttributeValue& attributeValue, std::vector<int>& val);
            static void retrieveAttributeValueContent(const AttributeValue& attributeValue, std::vector<std::vector<int>>& val);
            static void retrieveAttributeValueContent(const AttributeValue& attributeValue, std::vector<std::vector<std::vector<int>>>& val);

            static void assignAttributeValueContent(const int& val, AttributeValue& attributeValue);
            static void assignAttributeValueContent(const std::vector<int>&  val, AttributeValue& attributeValue);
            static void assignAttributeValueContent(const std::vector<std::vector<int>>&  val, AttributeValue& attributeValue);
            static void assignAttributeValueContent(const std::vector<std::vector<std::vector<int>>>&  val, AttributeValue& attributeValue);

            template<typename T>
            void payload_array_helper(const OCRepPayloadValue* pl, size_t depth);
            template<typename T>
            T payload_array_helper_copy(size_t index, const OCRepPayloadValue* pl);
            void setPayload(const OCRepPayload* payload);
            void setPayloadArray(const OCRepPayloadValue* pl);
            void getPayloadArray(OCRepPayload* payload,
                    const OCRepresentation::AttributeItem& item) const;
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
                    std::vector<std::string>& m_types;
                    std::vector<std::string>& m_interfaces;
            };
        private:
            std::string m_uri;
            std::vector<OCRepresentation> m_children;
            mutable std::map<std::string, AttributeValue> m_values;
            std::vector<std::string> m_resourceTypes;
            std::vector<std::string> m_interfaces;
            std::vector<std::string> m_dataModelVersions;

            InterfaceType m_interfaceType;
    };

    std::ostream& operator <<(std::ostream& os, const OCRepresentation::AttributeItem& ai);
} // namespace OC


#endif // OC_REPRESENTATION_H_
