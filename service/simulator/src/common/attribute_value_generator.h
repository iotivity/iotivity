/******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
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

#ifndef SIMULATOR_ATTRIBUTE_VALUE_GENERATOR_H_
#define SIMULATOR_ATTRIBUTE_VALUE_GENERATOR_H_

#include "simulator_resource_model_schema.h"

/**
 * @class AttributeValueGen
 */
class AttributeValueGen
{
    public:
        /**
         * check the next value
         * @return boolean value
         */
        virtual bool hasNext() = 0;
        /**
         * This method is to get the next index value
         * @return attribute value variant object
         */
        virtual AttributeValueVariant next() = 0;
        /**
         * This method is to get the index value
         * @return attribute value variant object
         */
        virtual AttributeValueVariant value() = 0;
        /** This method is to reset the value */
        virtual void reset() = 0;
};

/**
 * @class RangeValueGen
 */
template <typename TYPE>
class RangeValueGen : public AttributeValueGen
{
    private:
        TYPE m_min;
        TYPE m_max;
        TYPE m_cur;

    public:
        /**
         * This method is to generate the range values
         * @param[in] min    minimum range value
         * @param[in] max    maximum range value
         */
        RangeValueGen(TYPE min, TYPE max) : m_min(min), m_max(max), m_cur(min) {}

        bool hasNext()
        {
            return (m_cur <= m_max);
        }

        /**
         * This method is get the next index value
         * @return index value
         */
        AttributeValueVariant next()
        {
            TYPE value = m_cur;
            m_cur++;
            return value;
        }

        /**
         * This method is get the previous index value
         * @return index value
         */
        AttributeValueVariant value()
        {
            return m_cur - 1;
        }

        /** This method is to reset the value with minimum range value */
        void reset()
        {
            m_cur = m_min;
        }
};

/**
 * @class ValuesSetGen
 */
template <typename TYPE>
class ValuesSetGen : public AttributeValueGen
{
    private:
        std::vector<TYPE> m_values;
        size_t m_index;

    public:
        /**
         * This method is to set the value for the given place
         * @param[in] values   attribute value
         */
        ValuesSetGen(const std::vector<TYPE> &values) : m_values(values), m_index(0) {}

        bool hasNext()
        {
            return (m_index < m_values.size());
        }

        /**
         * This method is get the next index value
         * @return index value
         */
        AttributeValueVariant next()
        {
            return m_values[m_index++];
        }

        /**
         * This method is get the previous index value
         * @return index value
         */
        AttributeValueVariant value()
        {
            return m_values[m_index - 1];
        }

        /** This method is to reset the value */
        void reset()
        {
            m_index = 0;
        }
};

/**
 * @class AttributeValueGenFactory
 */
class AttributeValueGenFactory
{
    public:
        /**
         * generate the attribute value
         * @param[in] property  attribute property
         * @return attribute value object
         */
        static std::unique_ptr<AttributeValueGen> create(
            const std::shared_ptr<AttributeProperty> &property);
};

#endif
