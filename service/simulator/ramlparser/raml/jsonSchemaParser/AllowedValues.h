/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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
#ifndef ALLOWED_VALUES_H_
#define ALLOWED_VALUES_H_

#include <string>
#include <vector>
#include <map>
#include <boost/variant.hpp>
#include <boost/lexical_cast.hpp>
#include "Helpers.h"

namespace RAML
{
    class AllowedValues
    {
        public:
            template <typename T>
            void addValue(const T &value)
            {
                ValueVariant temp = value;
                m_values.push_back(temp);
            }

            template <typename T>
            void addValues(const std::vector<T> &values)
            {
                for (auto value : values)
                {
                    ValueVariant vValue = value;
                    m_values.push_back(vValue);
                }
            }

            inline ValueVariant &at(int index)
            {
                return m_values.at(index);
            }
            inline int size() const
            {
                return m_values.size();
            }

            inline std::vector<ValueVariant> getValues()
            {
                return m_values;
            }
            inline std::vector<int> getValuesInt()
            {
                std::vector<int> values;
                for (auto value : m_values)
                {
                    values.push_back(boost::lexical_cast<int> (value));
                }
                return values;
            }
            inline std::vector<std::string> getValuesString()
            {
                std::vector<std::string> values;
                for (auto value : m_values)
                {
                    values.push_back(boost::lexical_cast<std::string> (value));
                }
                return values;
            }

        private:
            std::vector<ValueVariant> m_values;
    };

}
#endif
