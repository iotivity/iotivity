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

#ifndef __RESOURCEATTRIBUTES_H
#define __RESOURCEATTRIBUTES_H

// To avoid conflict using different boost::variant configuration with OC.
// It causes compile errors.
#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#define BOOST_MPL_LIMIT_LIST_SIZE 30
#define BOOST_MPL_LIMIT_VECTOR_SIZE 30

#include <functional>
#include <unordered_map>

#include <boost/variant.hpp>
#include <boost/mpl/contains.hpp>
#include <boost/mpl/find.hpp>
#include <boost/mpl/distance.hpp>
#include <boost/mpl/begin_end.hpp>
#include <boost/scoped_ptr.hpp>

#include <PrimitiveException.h>

namespace OIC
{
    namespace Service
    {
        class BadGetException: public PrimitiveException
        {
        public:
            BadGetException(const std::string& what) : PrimitiveException{ what } {}
            BadGetException(std::string&& what) : PrimitiveException{ std::move(what) } {}
        };

        class InvalidKeyException: public PrimitiveException
        {
        public:
            InvalidKeyException(const std::string& what) : PrimitiveException{ what } {}
            InvalidKeyException(std::string&& what) : PrimitiveException{ std::move(what) } {}
        };

        class ResourceAttributes
        {
        private:
            template< typename T > struct is_supported_type_helper;

            using ValueVariant = boost::variant<
                std::nullptr_t,
                int,
                double,
                bool,
                std::string,
                ResourceAttributes
            >;

            template< typename T >
            using mpl_begin = typename boost::mpl::begin<T>::type;

            template< typename T >
            using enable_if_supported = typename std::enable_if<
                    is_supported_type_helper< T >::type::value >::type;

            template< typename T >
            using disable_if_unsupported = typename std::enable_if<
                    !is_supported_type_helper< T >::type::value >::type;

            template< typename VISITOR >
            class KeyValueVisitorHelper: public boost::static_visitor< >
            {
            public:
                KeyValueVisitorHelper(VISITOR& visitor) :
                        m_visitor( visitor )
                {
                }

                template< typename T >
                void operator()(const std::string& key, const T& value) const
                {
                    m_visitor(key, value);
                }

            private:
                VISITOR& m_visitor;
            };

        public:
            template< typename T >
            using is_supported_type = typename is_supported_type_helper< T >::type;

            class Value
            {
            public:
                Value();
                Value(const Value&);
                Value(Value&&);

                template< typename T, typename = enable_if_supported< T > >
                Value(T&& value) :
                        m_data{ new ValueVariant{ std::forward< T >(value) } }
                {
                }

                Value& operator=(const Value&);
                Value& operator=(Value&&) = default;

                template< typename T, typename = enable_if_supported< T > >
                Value& operator=(T&& rhs)
                {
                    *m_data = std::forward< T >(rhs);
                    return *this;
                }

                Value& operator=(const char*);
                Value& operator=(std::nullptr_t);

                template< typename T >
                typename std::add_lvalue_reference< const T >::type get() const
                {
                    return checkedGet< T >();
                }

                template< typename T >
                typename std::add_lvalue_reference< T >::type get()
                {
                    return checkedGet< T >();
                }

                bool isTypeEqualWith(const Value& rhs) const
                {
                    return m_data->which() == rhs.m_data->which();
                }

                template< typename T, enable_if_supported< T >* = nullptr >
                bool isTypeOf() const
                {
                    using iter = typename boost::mpl::find< ValueVariant::types, T >::type;

                    return m_data->which()
                            == boost::mpl::distance< mpl_begin< ValueVariant::types >, iter >::value;
                }

                template< typename T, disable_if_unsupported< T >* = nullptr >
                bool isTypeOf() const
                {
                    return false;
                }

                std::string toString() const;

                friend bool operator==(const Value&, const Value&);

                template< typename T >
                friend bool operator==(const Value&, const T&);

                bool operator==(const char*) const;

            private:
                template< typename T, typename = enable_if_supported< T > >
                typename std::add_lvalue_reference< T >::type checkedGet() const
                {
                    try
                    {
                        return boost::get< T >(*m_data);
                    }
                    catch (const boost::bad_get&)
                    {
                        throw BadGetException{ "Wrong type" };
                    }
                }

                template< typename T, typename U >
                bool equals(const U& rhs) const
                {
                    try
                    {
                        return get< T >() == rhs;
                    }
                    catch (const BadGetException&)
                    {
                        return false;
                    }
                }

            public:
                boost::scoped_ptr< ValueVariant > m_data;
            };

            class KeyValuePair;
            class iterator;
            class const_iterator;

        public:
            ResourceAttributes() = default;
            ResourceAttributes(const ResourceAttributes&) = default;
            ResourceAttributes(ResourceAttributes&&) = default;

            ResourceAttributes& operator=(const ResourceAttributes&) = default;
            ResourceAttributes& operator=(ResourceAttributes&&) = default;

            iterator begin();
            iterator end();

            const_iterator begin() const;
            const_iterator end() const;

            const_iterator cbegin() const;
            const_iterator cend() const;

            Value& operator[](const std::string&);

            Value& at(const std::string&);
            const Value& at(const std::string&) const;

            bool erase(const std::string&);

            bool contains(const std::string&) const;
            bool empty() const;
            size_t size() const;

            friend bool operator==(const ResourceAttributes&, const ResourceAttributes&);

        private:
            template< typename VISITOR >
            void visit(VISITOR& visitor) const
            {
                KeyValueVisitorHelper< VISITOR > helper{ visitor };

                for (const auto& i : m_keyValues)
                {
                    boost::variant< const std::string& > key{ i.first };
                    boost::apply_visitor(helper, key, *i.second.m_data);
                }
            }

        private:
            std::unordered_map< std::string, Value > m_keyValues;

            friend class ResourceAttributesConverter;
        };

        template< typename T > struct ResourceAttributes::is_supported_type_helper
        {
            using type = boost::mpl::contains<ValueVariant::types, typename std::decay< T >::type>;
        };

        template< typename T >
        bool operator==(const ResourceAttributes::Value& lhs, const T& rhs)
        {
            using TypeChecker = typename std::enable_if<
                    ResourceAttributes::is_supported_type< T >::value >::type;

            return lhs.equals< T >(rhs);
        }

        template< typename T >
        bool operator==(const T& lhs, const ResourceAttributes::Value& rhs)
        {
            return rhs == lhs;
        }

        bool operator==(const char* lhs, const ResourceAttributes::Value& rhs);

        class ResourceAttributes::KeyValuePair
        {
        private:
            class KeyVisitor: public boost::static_visitor< const std::string& >
            {
            public:
                result_type operator()(iterator*) const;
                result_type operator()(const_iterator*) const;
            };

            class ValueVisitor: public boost::static_visitor< Value& >
            {
            public:
                result_type operator()(iterator*);
                result_type operator()(const_iterator*);
            };

            class ConstValueVisitor: public boost::static_visitor< const Value& >
            {
            public:
                result_type operator()(iterator*) const;
                result_type operator()(const_iterator*) const;
            };

        public:
            const std::string& key() const;
            const ResourceAttributes::Value& value() const;
            ResourceAttributes::Value& value();

        private:
            KeyValuePair(const KeyValuePair&) = default;
            KeyValuePair(boost::variant< iterator*, const_iterator* >&&);

            KeyValuePair& operator=(const KeyValuePair&) = default;

        private:
            boost::variant< iterator*, const_iterator* > m_iterRef;

            KeyVisitor m_keyVisitor;
            ValueVisitor m_valueVisitor;
            ConstValueVisitor m_constValueVisitor;

            friend class iterator;
            friend class const_iterator;
        };

        class ResourceAttributes::iterator: public std::iterator< std::forward_iterator_tag,
                ResourceAttributes::KeyValuePair >
        {
        private:
            using base_iterator = std::unordered_map< std::string, Value >::iterator;

        public:
            iterator();
            iterator(const iterator&) = default;

            iterator& operator=(const iterator&) = default;

            reference operator*();
            pointer operator->();

            iterator& operator++();
            iterator operator++(int);

            bool operator==(const iterator&) const;
            bool operator!=(const iterator&) const;

        private:
            explicit iterator(base_iterator&&);

        private:
            base_iterator m_cur;
            ResourceAttributes::KeyValuePair m_keyValuePair;

            friend class ResourceAttributes;
        };

        class ResourceAttributes::const_iterator: public std::iterator< std::forward_iterator_tag,
                const ResourceAttributes::KeyValuePair >
        {
        private:
            using base_iterator = std::unordered_map< std::string, Value >::const_iterator;

        public:
            const_iterator();
            const_iterator(const const_iterator&) = default;
            const_iterator(const ResourceAttributes::iterator&);

            const_iterator& operator=(const const_iterator&) = default;
            const_iterator& operator=(const ResourceAttributes::iterator&);

            reference operator*() const;
            pointer operator->() const;

            const_iterator& operator++();
            const_iterator operator++(int);

            bool operator==(const const_iterator&) const;
            bool operator!=(const const_iterator&) const;

        private:
            explicit const_iterator(base_iterator&&);

        private:
            base_iterator m_cur;
            ResourceAttributes::KeyValuePair m_keyValuePair;

            friend class ResourceAttributes;
        };

    }
}

#endif // __RESOURCEATTRIBUTES_H
