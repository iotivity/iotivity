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

#ifndef JSON_SCHEMA_H_
#define JSON_SCHEMA_H_

#include <string>
#include <vector>
#include <map>
#include "Properties.h"
#include "Items.h"
#include "Definitions.h"
#include "cJSON.h"
#include "Helpers.h"
#include "AllowedValues.h"

#include "IncludeResolver.h"

namespace RAML
{
    class JsonSchema
    {
        public:
            JsonSchema() : m_cjson(NULL), m_includeResolver(NULL)  {}
            JsonSchema(const IncludeResolverPtr &includeResolver) : m_cjson(NULL),
                m_includeResolver(includeResolver) {}
            JsonSchema(cJSON *cjson , const IncludeResolverPtr &includeResolver) : m_cjson(cjson),
                m_includeResolver(includeResolver)  { readJson(); }

            void setcJson(cJSON *cjson) {m_cjson = cjson; readJson(); }
            int size() const
            {
                return m_properties.size();
            }
            inline bool getProperty(const std::string &name, Properties *value)
            {
                if (m_properties.end() != m_properties.find(name))
                {
                    value = m_properties[name];
                    return true;
                }
                return false;
            }
            inline std::map<std::string, Properties *> getProperties()
            {
                return m_properties;
            }
            inline std::map<std::string, Definitions *> getDefinitions()
            {
                return m_definition;
            }
            void addProperty(const std::string &name, Properties *property)
            {
                if (m_properties.end() == m_properties.find(name))
                {
                    m_properties[name] = property;
                }
            }
            void setRequiredValue(const std::string &reqValue)
            {
                if (m_required.end() == std::find(m_required.begin(), m_required.end(), reqValue))
                {
                    m_required.push_back(reqValue);
                }
            }
            std::vector<std::string> getRequiredValues()
            {
                return m_required;
            }
            void addDefinition(const std::string &defName, Definitions *definition)
            {
                if (m_definition.end() == m_definition.find(defName))
                {
                    m_definition[defName] = definition;
                }
            }
            Definitions *getDefinition(const std::string &defName)
            {
                if (m_definition.end() != m_definition.find(defName))
                {
                    return m_definition[defName];
                }
                return nullptr;
            }
            std::string getType()
            {
                return  m_type;
            }
            std::string getId()
            {
                return  m_id;
            }
            std::string getSchema()
            {
                return  m_schema;
            }
            std::string getDescription()
            {
                return  m_description;
            }
            std::string getTitle()
            {
                return  m_title;
            }

            bool getAdditionalProperties()
            {
                return  m_additionalProperties;
            }
            void setItem(Items *item)
            {
                m_items.push_back(item);
            }
            std::vector<Items *> getItems()
            {
                return m_items;
            }

            void readJson();
            Definitions *readDef(cJSON *childDefinitions, const std::string &defName);
            Properties *readProp(cJSON *childProperties, const std::string &attName );
            void readValues( cJSON *childProperties,  Properties *property , const std::string &attType);
            void readString( cJSON *childProperties, Properties *property);
            void readArray( cJSON *childProperties,  Properties *property);
            void readNumber( cJSON *childProperties,  Properties *property);
            Definitions *readRef(std::string m_ref);


            void readJsonRef(cJSON *jsonReference);
            void readDefRef(cJSON *defReference, Definitions *definition);
            void readAllOf(cJSON *allofValues);
            void readDefAllOf(cJSON *allofValues, Definitions *definition);
            Items *readItems(cJSON *item);
            void readItemRef(cJSON *itemReference, Items *item);
            void readItemAllOf(cJSON *allofValues,  Items *item);

        private:
            std::map<std::string, Properties *> m_properties;
            std::map<std::string, Definitions *> m_definition;
            std::string m_id;
            std::string m_schema;
            std::string m_title;
            std::string m_description;
            bool m_additionalProperties;
            std::string m_type;
            cJSON *m_cjson;
            std::vector<std::string>  m_required;
            std::vector<Items *> m_items;
            IncludeResolverPtr m_includeResolver;
    };
    typedef std::shared_ptr<JsonSchema> JsonSchemaPtr;

}

#endif
