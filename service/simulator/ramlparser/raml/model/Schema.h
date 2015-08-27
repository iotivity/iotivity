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

#ifndef SCHEMAS_H
#define SCHEMAS_H

#include <string>
#include "cJSON.h"
#include "IncludeResolver.h"

#include "ResourceProperties.h"

namespace RAML
{
    class Schema
    {
        public:
            virtual cJSON *getJson() const;
            virtual void setJson(cJSON *cjson);
            virtual std::string getSchema() const;
            virtual void setSchema(const std::string &schema);
            virtual ResourceProperties *getProperties() const;

            Schema(cJSON *cjson): m_cjson(cjson) {}
            Schema(const std::string &schema): m_schema(schema) , m_cjson(cJSON_Parse(schema.c_str())),
                m_resProperties(new ResourceProperties(m_cjson) ) {}
            Schema() {}

        private:
            cJSON *m_cjson;
            std::string m_schema;
            ResourceProperties *m_resProperties;
    };

}
#endif
