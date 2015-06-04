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

#include <memory>
#include <string>
#include <functional>
#include <unordered_map>
#include <vector>

class ResourceAttributes
{
    public:
        /*
         * @brief Value manipulation helper class.
         *          You may need to wrap OCRepresent::AttributeItem class.
         *          Do not expose the class as the part of API.
         */
        class Item
        {
            public:
                /*
                 * @brief Get the value as the type of T
                 *
                 * @throw BadCastException if the type is not correct.
                 */
                template<typename T>
                T get() const;

                /**
                 * @brief Get the value as the type of T
                 *
                 * @return True if the type is correct.
                 */
                template<typename T>
                bool get(T &var) const;

                // ...
                // we need other helper methods for this class.
                // please refer OCRepresent::AttributeItem class.
        };

        /**
         * @brief There will be container in this class, and we need iterator for that.
         * Implement one or just define alias.
         */
        typedef std::unordered_map<std::string, Item>::const_iterator const_iterator;

        /**
         * @brief std::map style get method.
         */
        Item &operator[](const std::string &key);

        /**
         * @brief std::map style get method.
         *
         * @throw InvalidKeyException If no value for the key.
         */
        Item &at(const std::string &key);

        /**
         * @brief std::map style get method.
         *
         * @throw InvalidKeyException If no value for the key.
         */
        const Item &at(const std::string &key) const;

        /**
         * @return True if there is a value for the key.
         */
        bool contains(const std::string &key) const;

        const_iterator cbegin() const;
        const_iterator cend() const;

        size_t getSize() const;
        bool isEmpty() const;

        template<typename T>
        void insert(const std::string &key, const T &value);

        void remove(const std::string &key);

        void clear();
};

