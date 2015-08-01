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

/**
 * @file
 *
 * This file contains the "RCSResourceAttributes" class & its helper classes
 */
#ifndef RES_ENCAPSULATION_RESOURCEATTRIBUTES_H
#define RES_ENCAPSULATION_RESOURCEATTRIBUTES_H

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

#include <RCSException.h>

namespace OIC
{
    namespace Service
    {

        /**
        * RCSResourceAttributes represents the attributes for a resource.
        *
        * It provides similar usage to c++ standard containers. (iterator,
        * operators and accessors)<br/>
        * An attribute value can be one of various types. <br/>
        *
        * @note If client developer wants to get the RCSResourceAttributes for the resource of
        *            interest following are the steps:
        *            - first call the discover API of DiscoveryManager class.
        *            - After getting the RemoteResourceObject, call getRemoteAttributes() API
        *               of RemoteResourceObject class
        *
        * @see Value
        * @see Type
        * @see iterator
        * @see const_iterator
        * @see RCSDiscoveryManager
        * @see RCSRemoteResourceObject
        * @see RCSResourceObject
        */
        class RCSResourceAttributes
        {
        private:
            template< typename T > struct IsSupportedTypeHelper;

            typedef boost::variant<
                std::nullptr_t,
                int,
                double,
                bool,
                std::string,
                RCSResourceAttributes
            > ValueVariant;

            template< typename T, typename V = void,
                    typename = typename std::enable_if<
                        IsSupportedTypeHelper< T >::type::value, V >::type >
            struct enable_if_supported
            {
                typedef V type;
            };

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

            template <typename T> struct IndexOfType;

        public:

            /**
             * Trait class that identifies whether T is supported by the Value.
             */
            template< typename T >
            struct is_supported_type: public std::conditional<
                IsSupportedTypeHelper< T >::type::value, std::true_type, std::false_type>::type { };

            /**
             * Identifier for types of Value.
             *
             * @see Type
             */
            enum class TypeId
            {
                NULL_T, /**< nullptr_t */
                INT, /**< int */
                DOUBLE, /**< double */
                BOOL, /**< bool */
                STRING, /**< std::string */
                ATTRIBUTES, /**< RCSResourceAttributes */
                VECTOR /**< std::vector */
            };

            /**
             * A Helper class to identify types of Value.
             *
             * @see RCSResourceAttributes
             * @see Value
             * @see TypeId
             */
            class Type
            {
            public:
                Type(const Type&) = default;
                Type(Type&&) = default;

                Type& operator=(const Type&) = default;
                Type& operator=(Type&&) = default;

                /**
                 * Returns type identifier.
                 *
                 * @return Identifier of type.
                 */
                TypeId getId() const;

                /**
                 * Factory method to create Type instance from T.
                 *
                 * @return An instance that has TypeId for T.
                 *
                 * @note T must be supported by Value. Otherwise, it won't be compiled.
                 *
                 * @see is_supported_type
                 */
                template < typename T >
                static Type typeOf(const T& value)
                {
                    return Type(value);
                }

                //! @cond
                friend bool operator==(const Type&, const Type&);
                //! @endcond

            private:
                template < typename T >
                explicit Type(const T&) :
                    m_which{ IndexOfType< T >::value }
                {
                }

            private:
                int m_which;
            };

            /**
             * Value holds a value among various types at a time.
             *
             * Type helps identify type information of Value.
             *
             * @see RCSResourceAttributes
             * @see Type
             * @see is_supported_type
             */
            class Value
            {
            public:
                class ComparisonHelper;

                Value();
                Value(const Value&);
                Value(Value&&);

                /**
                 * Constructs a Value if T is a supported type.<br/>
                 *       Otherwise it won't be compiled.
                 */
                template< typename T, typename = typename enable_if_supported< T >::type >
                Value(T&& value) :
                        m_data{ new ValueVariant{ std::forward< T >(value) } }
                {
                }

                Value(const char* value);

                Value& operator=(const Value&);
                Value& operator=(Value&&);

                template< typename T, typename = typename enable_if_supported< T >::type >
                Value& operator=(T&& rhs)
                {
                    *m_data = std::forward< T >(rhs);
                    return *this;
                }

                Value& operator=(const char*);
                Value& operator=(std::nullptr_t);

                /**
                 * Returns the underlying value as T.
                 *
                 * @return const reference to the underlying value.
                 *
                 * @throws BadGetException If type of the underlying value is not T.
                 */
                template< typename T >
                typename std::add_lvalue_reference< const T >::type get() const
                {
                    return checkedGet< T >();
                }

                /**
                 * Returns the underlying value as T.
                 *
                 * @return reference to the underlying value.
                 *
                 * @throws BadGetException If type of the underlying value is not T.
                 */
                template< typename T >
                typename std::add_lvalue_reference< T >::type get()
                {
                    return checkedGet< T >();
                }

                /**
                 * Returns Type information.
                 *
                 * @see Type
                 */
                Type getType() const;

                /**
                 * Returns a string representation.
                 *
                 */
                std::string toString() const;

                /**
                 * Exchanges the content of the object by the content of the parameter.
                 */
                void swap(Value&);

                //! @cond
                friend class RCSResourceAttributes;
                //! @endcond

            private:
                template< typename T, typename = typename enable_if_supported< T >::type >
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

            private:
                boost::scoped_ptr< ValueVariant > m_data;
            };

            class KeyValuePair;
            class iterator;
            class const_iterator;

        public:
            RCSResourceAttributes() = default;
            RCSResourceAttributes(const RCSResourceAttributes&) = default;
            RCSResourceAttributes(RCSResourceAttributes&&) = default;

            RCSResourceAttributes& operator=(const RCSResourceAttributes&) = default;
            RCSResourceAttributes& operator=(RCSResourceAttributes&&) = default;

            /**
             * Returns an {@link iterator} referring to the first element.
             */
            iterator begin();

            /**
             * Returns an {@link iterator} referring to the <i>past-the-end element</i>.
             */
            iterator end();

            /**
             * @copydoc cbegin()
             */
            const_iterator begin() const;

            /**
             * @copydoc cend()
             */
            const_iterator end() const;

            /**
             * Returns a const_iterator referring to the first element.
             */
            const_iterator cbegin() const;

            /**
             * Returns a const_iterator referring to the <i>past-the-end element</i>.
             */
            const_iterator cend() const;

            /**
             * Accesses a value.
             *
             * If @a key matches the key of a value,
             * returns a reference to its mapped value. <br/>
             * If @a key doesn't match the key of any value,
             * inserts a new value with that key and returns a reference to it.
             * The element is a Value that has null.
             *
             * @param key Key of the element whose mapped value is accessed.
             *
             * @return A reference to the mapped value with @a key.
             *
             * @see at
             */
            Value& operator[](const std::string& key);

            /**
             * Accesses a value.
             *
             * If @a key matches the key of a value,
             * returns a reference to its mapped value. <br/>
             * If @a key doesn't match the key of any value,
             * inserts a new value with that key and returns a reference to it.
             * The value has null.
             *
             * @param key Key of the element whose mapped value is accessed.
             *        This is moved instead of copied when a new value is inserted.
             *
             * @return A reference to the mapped value with @a key.
             *
             * @see at
             */
            Value& operator[](std::string&& key);

            /**
             * Accesses a value.
             *
             * If @a key matches the key of a value,
             * returns a reference to its mapped value. <br/>
             * If @a key doesn't match the key of any value, throws InvalidKeyException.
             *
             * @param key Key of the element whose mapped value is accessed.
             *
             * @throws InvalidKeyException If @a key doesn't match the key of any value.
             *
             * @return A reference to the mapped value with @a key.
             *
             * @see operator[]
             */
            Value& at(const std::string& key);

            /**
              * Accesses a value.
              *
              * If @a key matches the key of a value,
              * returns a reference to its mapped value. <br/>
              * If @a key doesn't match the key of any value, throws InvalidKeyException.
              *
              * @param key Key of the element whose mapped value is accessed.
              *
              * @throws InvalidKeyException If @a key doesn't match the key of any value.
              *
              * @return A const reference to the mapped value with @a key.
              *
              * @see operator[]
              */
            const Value& at(const std::string& key) const;

            /**
             * Removes all elements.
             */
            void clear();

            /**
             * Removes a single element.
             *
             * @param key Key of the element to be removed.
             *
             * @return true if an element is erased, false otherwise.
             */
            bool erase(const std::string& key);

            /**
             * Checks the container has an element with a Key equivalent to key.
             *
             * @param key Key to check.
             *
             * @return true if an element exists, false otherwise.
             */
            bool contains(const std::string& key) const;

            /**
             * Returns whether it is empty.
             *
             * @see size
             */
            bool empty() const;

            /**
             * Returns the number of elements.
             *
             * @see empty
             */
            size_t size() const;

        private:
            template< typename VISITOR >
            void visit(VISITOR& visitor) const
            {
                KeyValueVisitorHelper< VISITOR > helper{ visitor };

                for (const auto& i : m_values)
                {
                    boost::variant< const std::string& > key{ i.first };
                    boost::apply_visitor(helper, key, *i.second.m_data);
                }
            }

        private:
            std::unordered_map< std::string, Value > m_values;

            //! @cond
            friend class ResourceAttributesConverter;

            friend bool operator==(const RCSResourceAttributes&, const RCSResourceAttributes&);
            //! @endcond
        };

        /**
         * A helper class to avoid obscure comparisons of values which are supported
         * by RCSResourceAttributes::Value caused by implicitly converting a value
         * to a RCSResourceAttributes::Value.
         *
         * @see Value
         * @see RCSResourceAttributes
         * @see is_supported_type
         */
        class RCSResourceAttributes::Value::ComparisonHelper
        {
        public:
            ComparisonHelper(const Value&);

            template< typename T >
            typename std::enable_if< is_supported_type< T >::value, bool >::type equals(
                    const T& v) const
            {
                return m_valueRef.equals< T >(v);
            }

            bool equals(const std::string& v) const
            {
                return m_valueRef.equals< std::string >(v);
            }

            bool operator==(const ComparisonHelper&) const;

        private:
            const Value& m_valueRef;
        };

        template< typename T >
        struct RCSResourceAttributes::IsSupportedTypeHelper
        {
            typedef boost::mpl::contains<ValueVariant::types, typename std::decay< T >::type> type;
        };

        template <typename T>
        struct RCSResourceAttributes::IndexOfType
        {
            typedef typename boost::mpl::find< ValueVariant::types, T >::type iter;
            typedef typename boost::mpl::begin< ValueVariant::types >::type mpl_begin;

            static constexpr int value = boost::mpl::distance< mpl_begin, iter >::value;
        };

        /**
         * @relates RCSResourceAttributes::Type
         *
         * Checks if the objects are equal, that is, whether types are exactly same.
         *
         * @return true if the objects are equal, false otherwise.
         */
        bool operator==(const RCSResourceAttributes::Type&, const RCSResourceAttributes::Type&);

        /**
         * @relates RCSResourceAttributes::Type
         *
         * Checks if the objects are not equal, that is, whether types are not exactly same.
         *
         * @return true if the objects are not equal, false otherwise.
         */
        bool operator!=(const RCSResourceAttributes::Type&, const RCSResourceAttributes::Type&);

        /**
         * @relates RCSResourceAttributes::Value
         *
         * Checks if the contents are equal, that is,
         * whether types are matched and underlying values are equal.
         *
         * @return true if the contents are equal, false otherwise.
         */
        bool operator==(const RCSResourceAttributes::Value::ComparisonHelper&,
                const RCSResourceAttributes::Value::ComparisonHelper&);

        /**
         * @relates RCSResourceAttributes::Value
         *
         * Checks if the contents are not equal, that is,
         * whether types are not matched or underlying values are not equal.
         *
         * @return true if the contents are not equal, false otherwise.
         */
        bool operator!=(const RCSResourceAttributes::Value::ComparisonHelper&,
                const RCSResourceAttributes::Value::ComparisonHelper&);

        //! @cond
        template< typename T >
        typename std::enable_if< RCSResourceAttributes::is_supported_type< T >::value ||
            std::is_constructible< std::string, T >::value, bool >::type
        operator==(const RCSResourceAttributes::Value::ComparisonHelper& lhs, const T& rhs)
        {
            return lhs.equals(rhs);
        }

        template< typename T >
        typename std::enable_if< RCSResourceAttributes::is_supported_type< T >::value ||
                    std::is_constructible< std::string, T >::value, bool >::type
        operator==(const T& lhs, const RCSResourceAttributes::Value::ComparisonHelper& rhs)
        {
            return rhs == lhs;
        }

        template< typename T >
        typename std::enable_if< RCSResourceAttributes::is_supported_type< T >::value ||
                    std::is_constructible< std::string, T >::value, bool >::type
        operator!=(const RCSResourceAttributes::Value::ComparisonHelper& lhs, const T& rhs)
        {
            return !(lhs == rhs);
        }

        template< typename T >
        typename std::enable_if< RCSResourceAttributes::is_supported_type< T >::value ||
                    std::is_constructible< std::string, T >::value, bool >::type
        operator!=(const T& lhs, const RCSResourceAttributes::Value::ComparisonHelper& rhs)
        {
            return !(rhs == lhs);
        }
        //! @endcond

        /**
          * @relates RCSResourceAttributes
          *
          * Checks if the attributes are equal, that is, whether contents are equal.
          *
          * @return true if the attributes are equal, false otherwise.
          */
        bool operator==(const RCSResourceAttributes& lhs, const RCSResourceAttributes& rhs);

        /**
          * @relates RCSResourceAttributes
          *
          * Checks if the attributes are not equal, that is, whether contents are not equal.
          *
          * @return true if the attributes are not equal, false otherwise.
          */
        bool operator!=(const RCSResourceAttributes&, const RCSResourceAttributes&);

        /**
         * KeyValuePair is a class to access attribute's key and value of an element pointed by
         * iterators of RCSResourceAttributes.
         *
         *
         * @see RCSResourceAttributes
         * @see iterator
         * @see const_iterator
         */
        class RCSResourceAttributes::KeyValuePair
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
            const RCSResourceAttributes::Value& value() const;
            RCSResourceAttributes::Value& value();

        private:
            KeyValuePair(const KeyValuePair&) = default;
            KeyValuePair(boost::variant< iterator*, const_iterator* >&&);

            KeyValuePair& operator=(const KeyValuePair&) = default;

        private:
            boost::variant< iterator*, const_iterator* > m_iterRef;

            KeyVisitor m_keyVisitor;
            ValueVisitor m_valueVisitor;
            ConstValueVisitor m_constValueVisitor;

            //! @cond
            friend class iterator;
            friend class const_iterator;
            //! @endcond
        };

        /**
         * A forward iterator to KeyValuePair.
         *
         * @see RCSResourceAttributes
         * @see KeyValuePair
         * @see const_iterator
         */
        class RCSResourceAttributes::iterator:
                public std::iterator< std::forward_iterator_tag, RCSResourceAttributes::KeyValuePair >
        {
        private:
            typedef std::unordered_map< std::string, Value >::iterator base_iterator;

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
            RCSResourceAttributes::KeyValuePair m_keyValuePair;

            //! @cond
            friend class RCSResourceAttributes;
            //! @endcond
        };


        /**
         * A forward iterator to const KeyValuePair.
         *
         * @see RCSResourceAttributes
         * @see KeyValuePair
         * @see iterator
         */
        class RCSResourceAttributes::const_iterator:
                public std::iterator < std::forward_iterator_tag,
                                       const RCSResourceAttributes::KeyValuePair >
        {
        private:
            typedef std::unordered_map< std::string, Value >::const_iterator base_iterator;

        public:
            const_iterator();
            const_iterator(const const_iterator&) = default;
            const_iterator(const RCSResourceAttributes::iterator&);

            const_iterator& operator=(const const_iterator&) = default;
            const_iterator& operator=(const RCSResourceAttributes::iterator&);

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
            RCSResourceAttributes::KeyValuePair m_keyValuePair;

            //! @cond
            friend class RCSResourceAttributes;
            //! @endcond
        };

    }
}

#endif // RES_ENCAPSULATION_RESOURCEATTRIBUTES_H
