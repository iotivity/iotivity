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
            virtual SchemaPtr const &getSchema() const;
            virtual void setSchema(const SchemaPtr &schema);
            virtual std::string getExample() const;
            virtual void setExample(const std::string &example);
            virtual std::map<std::string, FormParameterPtr> const &getFormParameters() const;
            virtual void setFormParameter(const std::string &paramName, const FormParameterPtr &formParameter);
            RequestResponseBody(): m_schema(NULL), m_includeResolver(NULL) {}
            RequestResponseBody(const std::string type) : m_type(type), m_schema(NULL),
                m_includeResolver(NULL) {}
            RequestResponseBody(const std::string type, const YAML::Node &yamlNode,
                                const IncludeResolverPtr &includeResolver): m_schema(NULL), m_includeResolver(includeResolver) { readRequestResponseBody(type, yamlNode); }

        private:
            virtual void readRequestResponseBody(const std::string &type, const YAML::Node &yamlNode) ;
        private:
            std::string m_type;
            SchemaPtr m_schema;
            std::string m_example;
            std::map<std::string, FormParameterPtr> m_formParameters;
            IncludeResolverPtr m_includeResolver;
    };

    typedef std::shared_ptr<RequestResponseBody> RequestResponseBodyPtr;
}
#endif
