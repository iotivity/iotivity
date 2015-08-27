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
 *		http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#ifndef ABSTRACT_PARAM_H
#define ABSTRACT_PARAM_H


#include <map>
#include <list>
#include <string>
#include "Utils.h"


namespace RAML
{
    class AbstractParam
    {
        public:
            virtual std::string getDefaultValue() const;
            virtual void setDefaultValue(const std::string &defaultValue);

            virtual std::string getDescription() const;
            virtual void setDescription(const std::string &description);

            virtual std::string getDisplayName() const;
            virtual void setDisplayName(const std::string &displayName);

            virtual std::list<std::string> getEnumeration() const;
            virtual void setEnumeration(const std::string &enumeration);

            virtual std::string getExample() const;
            virtual void setExample(const std::string &example);

            virtual int getMaxLength() const;
            virtual void setMaxLength(int maxLength);

            virtual int getMaximum() const;
            virtual void setMaximum(int maximum);

            virtual int getMinLength() const;
            virtual void setMinLength(int minLength);

            virtual int getMinimum() const;
            virtual void setMinimum(int minimum);

            virtual std::string getPattern() const;
            virtual void setPattern(const std::string &pattern) ;

            virtual std::string getType() const;
            virtual void setType(const std::string &type);

            virtual bool isRepeat() const;
            virtual void setRepeat(bool repeat);

            virtual bool isRequired() const;
            virtual void setRequired(bool required);


            AbstractParam() : m_maximum(0), m_minimum(0), m_minLength(0), m_maxLength(0), m_repeat(false),
                m_required(false) {}
            AbstractParam(const YAML::Node &yamlNode) : m_maximum(0), m_minimum(0), m_minLength(0),
                m_maxLength(0), m_repeat(false), m_required(false)
            {
                readParameters(yamlNode);
            }
        private:
            virtual void readParameters(const YAML::Node &yamlNode);

        private:
            std::string m_defaultValue;
            std::string m_description;
            std::string m_displayName;
            std::list<std::string> m_enumeration;
            std::string m_example;
            int m_maxLength;
            int m_maximum;
            int m_minLength;
            int m_minimum;
            std::string m_pattern;
            bool m_repeat;
            bool m_required;
            std::string m_type;


    };

}
#endif
