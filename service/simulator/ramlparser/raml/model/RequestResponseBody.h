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

#ifndef REQUEST_RESPONSE_BODY_H
#define REQUEST_RESPONSE_BODY_H

#include <map>
#include <list>
#include "FormParameter.h"
#include "Utils.h"
#include "IncludeResolver.h"
#include "Schema.h"

namespace RAML
{
    class RequestResponseBody
    {
        public:
            virtual std::string getType() const;
            virtual void setType(const std::string &type);
            virtual Schema *getSchema() const;
            virtual void setSchema(Schema *schema);
            virtual std::string getExample() const;
            virtual void setExample(const std::string &example);
            virtual std::map<std::string, FormParameter > getFormParameters() const;
            virtual void setFormParameter(const std::string &paramName, const FormParameter  &formParameter);
            RequestResponseBody(): m_schema(NULL) {}
            RequestResponseBody(const std::string type) : m_type(type), m_schema(NULL) {}
            RequestResponseBody(const std::string type, const YAML::Node &yamlNode,
                                IncludeResolver *includeResolver) { readRequestResponseBody(type, yamlNode, includeResolver); }
        private:
            virtual void readRequestResponseBody(const std::string &type, const YAML::Node &yamlNode,
                                                 IncludeResolver *includeResolver) ;
        private:
            std::string m_type;
            Schema *m_schema;
            std::string m_example;
            std::map<std::string, FormParameter > m_formParameters;
            IncludeResolver *m_includeResolver;


    };
}
#endif
