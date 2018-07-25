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

#ifndef SIMULATOR_RESOURCE_MODEL_SCHEMA_H_
#define SIMULATOR_RESOURCE_MODEL_SCHEMA_H_

#include <unordered_map>
#include <memory>

#include "simulator_resource_model.h"

class IntegerProperty;
class DoubleProperty;
class BooleanProperty;
class StringProperty;
class ArrayProperty;
class ModelProperty;
class AttributeProperty
{
    public:
        enum class Type
        {
            INTEGER,   /**< Integer type */
            DOUBLE,    /**< Double type */
            BOOLEAN,   /**< Boolean type */
            STRING,    /**< String type */
            MODEL,     /**< Model type */
            ARRAY      /**< Array type */
        };

        explicit AttributeProperty(Type type);
        virtual ~AttributeProperty() {}

        /**
         * get the attribute type
         * @return attribute type
         */
        Type getType() const;
        /**
         * Check the integer value
         * @return boolean value
         */
        virtual bool isInteger() const
        {
            return false;
        }
        /**
         * Check the double value
         * @return boolean value
         */
        virtual bool isDouble() const
        {
            return false;
        }
        /**
         * Check the boolean value
         * @return boolean value
         */
        virtual bool isBoolean() const
        {
            return false;
        }
        /**
         * Check the string value
         * @return boolean value
         */
        virtual bool isString() const
        {
            return false;
        }
        /**
         * Check array
         * @return boolean value
         */
        virtual bool isArray() const
        {
            return false;
        }
        /**
         * Check model
         * @return boolean value
         */
        virtual bool isModel() const
        {
            return false;
        }

        /**
         * Integer property
         * @return integer property object
         */
        virtual std::shared_ptr<IntegerProperty> asInteger()
        {
            return nullptr;
        }

        /**
         * double property
         * @return double property object
         */
        virtual std::shared_ptr<DoubleProperty> asDouble()
        {
            return nullptr;
        }

        /**
         * boolean property
         * @return boolean property object
         */
        virtual std::shared_ptr<BooleanProperty> asBoolean()
        {
            return nullptr;
        }

        /**
         * string property
         * @return string property object
         */
        virtual std::shared_ptr<StringProperty> asString()
        {
            return nullptr;
        }

        /**
         * array property
         * @return array property object
         */
        virtual std::shared_ptr<ArrayProperty> asArray()
        {
            return nullptr;
        }

        /**
         * model property
         * @return model property object
         */
        virtual std::shared_ptr<ModelProperty> asModel()
        {
            return nullptr;
        }

        /**
         * validate the attribute value
         * @param[in] value    value to validate
         * @return boolean value
         */
        virtual bool validate(const AttributeValueVariant &value) = 0;
        /**
         * This method is to get the attribute default value
         * @return class object
         */
        virtual AttributeValueVariant buildValue() = 0;

    private:
        Type m_type;
};

/**
 * IntegerProperty class
 */
class IntegerProperty : public AttributeProperty,
    public std::enable_shared_from_this<IntegerProperty>
{
    public:
        static std::shared_ptr<IntegerProperty> build(int defaultValue = 0);

        /**
         * check value is integer or not
         * @return boolean value
         */
        bool isInteger() const;
        std::shared_ptr<IntegerProperty> asInteger();
        /**
         * set the given default vlaue
         * @param value  value to set
         */
        void setDefaultValue(int value);

        /**
         * set the range
         * @param min   minimum range value
         * @param max   maximum range value
         */
        void setRange(int min, int max);

        /**
         * set the property values
         * @param values   value to set
         */
        void setValues(const std::vector<int> &values);

        /**
         * check range and return the true or false value
         * @return boolean value
         */
        bool hasRange() const;
        /**
         * check value and return the true or false value
         * @return boolean value
         */
        bool hasValues() const;
        /**
         * get the default property value
         * @return default value
         */
        int getDefaultValue() const;
        /**
         * get the minimum and maximum range value
         * @param[out] min   minimum range
         * @param[out] max   maximum range
         * @return boolean value
         */
        bool getRange(int &min, int &max) const;
        /**
         * get the property values
         * @param[out] values  property value
         * @return boolean value
         */
        bool getValues(std::vector<int> &values) const;
        /**
         * validate the attribute value
         * @param[in] value   attribute value
         * @return boolean value
         */
        bool validate(const AttributeValueVariant &value);
        /**
         * validate the value
         * @param[in] value   attribute value
         * @return boolean value
         */
        bool validate(const int &value);
        AttributeValueVariant buildValue();

    private:
        explicit IntegerProperty(int defaultValue);

        int m_defaultValue;
        int m_min;
        int m_max;
        std::vector<int> m_values;
        bool m_hasRange;
};

/*
 * DoubleProperty class
 */
class DoubleProperty : public AttributeProperty,
    public std::enable_shared_from_this<DoubleProperty>
{
    public:
        static std::shared_ptr<DoubleProperty> build(double defaultValue = 0.0);

        /**
         * check value is double or not
         * @return boolean value
         */
        bool isDouble() const;
        std::shared_ptr<DoubleProperty> asDouble();

        /**
         * set the given default vlaue
         * @param value  value to set
         */
        void setDefaultValue(double value);

        /**
         * set the range
         * @param min   minimum range value
         * @param max   maximum range value
         */
        void setRange(double min, double max);

        /**
         * set the property values
         * @param values   value to set
         */
        void setValues(const std::vector<double> &values);
        /**
         * check range and return the true or false value
         * @return boolean value
         */
        bool hasRange() const;
        /**
         * check value and return the true or false value
         * @return boolean value
         */
        bool hasValues() const;
        /**
         * get the default property value
         * @return default vlaue
         */
        double getDefaultValue() const;
        /**
         * get the minimum and maximum range value
         * @param[out] min   minimum range
         * @param[out] max   maximum range
         * @return boolean value
         */
        bool getRange(double &min, double &max) const;
        /**
         * get the property values
         * @param[out] values  property value
         * @return boolean value
         */
        bool getValues(std::vector<double> &values) const;
        /**
         * validate the attribute value
         * @param[in] value   attribute value
         * @return boolean value
         */
        bool validate(const AttributeValueVariant &value);
        /**
         * validate the value
         * @param[in] value   attribute value
         * @return boolean value
         */
        bool validate(const double &value);
        AttributeValueVariant buildValue();

    private:
        explicit DoubleProperty(double defaultValue);

        double m_defaultValue;
        double m_min;
        double m_max;
        std::vector<double> m_values;
        bool m_hasRange;
};

/**
 * BooleanProperty class
 */
class BooleanProperty : public AttributeProperty,
    public std::enable_shared_from_this<BooleanProperty>
{
    public:
        static std::shared_ptr<BooleanProperty> build(bool defaultValue = true);

        /**
         * check value is boolean or not
         * @return boolean value
         */
        bool isBoolean() const;
        std::shared_ptr<BooleanProperty> asBoolean();
        /**
         * set default value
         * @param value    attribute default value
         */
        void setDefaultValue(bool value);
        /**
         * get default value
         * @return boolean value
         */
        bool getDefaultValue() const;
        /**
         * validate attribute value
         * @param[in] value   attribute value
         * @return boolean value
         */
        bool validate(const AttributeValueVariant &value);
        AttributeValueVariant buildValue();

    private:
        explicit BooleanProperty(bool defaultValue);

        bool m_defaultValue;
};

/**
 * StringProperty class
 */
class StringProperty : public AttributeProperty,
    public std::enable_shared_from_this<StringProperty>
{
    public:
        static std::shared_ptr<StringProperty> build(const std::string &defaultValue = "");

        /**
         * check the string value
         * @return boolean value
         */
        bool isString() const;
        std::shared_ptr<StringProperty> asString();

        /**
         * set the given default vlaue
         * @param value  value to set
         */
        void setDefaultValue(const std::string &value);

        /**
         * set the range
         * @param min   minimum range value
         * @param max   maximum range value
         */
        void setRange(size_t min, size_t max);

        /**
         * set the property values
         * @param values   value to set
         */
        void setValues(const std::vector<std::string> &values);
        /**
         * check range and return the true or false value
         * @return boolean value
         */
        bool hasRange() const;
        /**
         * check value and return the true or false value
         * @return boolean value
         */
        bool hasValues() const;
        std::string getDefaultValue() const;
        /**
         * get the minimum and maximum range value
         * @param min    minimum range value
         * @param max    maximum range value
         * @return boolean value
         */
        bool getRange(size_t &min, size_t &max) const;
        /**
         * get the property values
         * @param[in] values   attribute value
         * @return boolean value
         */
        bool getValues(std::vector<std::string> &values) const;
        /**
         * validate attribute value
         * @param[in] values   attribute value
         * @return boolean value
         */
        bool validate(const AttributeValueVariant &value);
        /**
         * validate value
         * @param[in] values   attribute value
         * @return boolean value
         */
        bool validate(const std::string &value);
        AttributeValueVariant buildValue();

    private:
        StringProperty(const std::string &defaultValue);

        std::string m_defaultValue;
        size_t m_min;
        size_t m_max;
        std::vector<std::string> m_values;
        bool m_hasRange;
};

/**
 * ArrayProperty class
 */
class ArrayProperty : public AttributeProperty,
    public std::enable_shared_from_this<ArrayProperty>
{
    public:
        static std::shared_ptr<ArrayProperty> build();

        /**
         * check array
         * @return boolean value
         */
        bool isArray() const;
        std::shared_ptr<ArrayProperty> asArray();

        /**
         * set the range
         * @param minItems   minimum range value
         * @param maxItems   maximum range value
         */
        void setRange(size_t minItems, size_t maxItems);
        /**
         * set variable state
         * @param[in] state  value to set
         */
        void setVariable(bool state);
        /**
         * set the state value
         * @param[in] state  value to set
         */
        void setUnique(bool state);
        /**
         * set element property
         * @param[in] property    attribute property
         * @return boolean value
         */
        bool setElementProperty(const std::shared_ptr<AttributeProperty> &property);
        /**
         * check range and return the true or false value
         * @return boolean value
         */
        bool hasRange() const;
        /**
         * check the variable value
         * @return boolean value
         */
        bool isVariable() const;
        /**
         * check the unique value
         * @return boolean value
         */
        bool isUnique() const;
        /**
         * get the minimum items value
         * @return minimum item value
         */
        size_t getMinItems() const;
        /**
         * get the maximum items value
         * @return miximum item value
         */
        size_t getMaxItems() const;
        /**
         * get the element property
         * @return element property
         */
        std::shared_ptr<AttributeProperty> getElementProperty();
        /**
         * validate attribute value
         * @param[in] value   attribute value
         * @return boolean value
         */
        bool validate(const AttributeValueVariant &value);
        AttributeValueVariant buildValue();

    private:
        ArrayProperty();
        int findDepth(std::shared_ptr<AttributeProperty> &elementProperty);

        size_t m_min;
        size_t m_max;
        bool m_isVariableSize;
        bool m_isUnique;
        std::shared_ptr<AttributeProperty> m_elementProperty;
        bool m_hasRange;
};

/**
 * ModelProperty class
 */
class ModelProperty : public AttributeProperty,
    public std::enable_shared_from_this<ModelProperty>
{
    public:
        static std::shared_ptr<ModelProperty> build();

        /**
         * check model
         * @return boolean value
         */
        bool isModel() const;
        std::shared_ptr<ModelProperty> asModel();
        bool add(const std::string &name, const std::shared_ptr<AttributeProperty> &property,
                 bool required = false);
        std::shared_ptr<AttributeProperty> get(const std::string &name);
        std::unordered_map<std::string, std::shared_ptr<AttributeProperty>> getChildProperties();
        /**
         * check name value and return true or false value
         * @param[in] name    string value
         * @return boolean value
         */
        bool isRequired(const std::string &name);
        /**
         * remove the given name
         * @param[in] name   string value
         */
        void remove(const std::string &name);
        /**
         * set the given value
         * @param[in] name    set the value
         */
        void setRequired(const std::string &name);
        /**
         * unset the given value
         * @param[in] name   unset the value
         */
        void unsetRequired(const std::string &name);
        SimulatorResourceModel buildResourceModel();
        /**
         * validate attribute value
         * @param[in] value   attribute value
         * @return boolean value
         */
        bool validate(const AttributeValueVariant &value);
        /**
         * validate the model value
         * @param[in] model  model to validate
         * @return boolean value
         */
        bool validate(const SimulatorResourceModel &model);
        AttributeValueVariant buildValue();

    private:
        ModelProperty();

        std::unordered_map<std::string, bool> m_requiredAttributes;
        std::unordered_map<std::string, std::shared_ptr<AttributeProperty>> m_childProperties;
};

typedef ModelProperty SimulatorResourceModelSchema;

#endif
