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

#ifndef RAML_H
#define RAML_H

#include <map>
#include <list>
#include <string>
#include "AbstractParam.h"
#include "UriParameter.h"
#include "QueryParameter.h"
#include "FormParameter.h"
#include "Header.h"

#include "RequestResponseBody.h"
#include "RamlResource.h"
#include "ActionType.h"
#include "Action.h"
#include "Response.h"
#include "Schema.h"
#include "IncludeResolver.h"

#include "DocumentationItem.h"
#include "yaml-cpp/yaml.h"
#include "Utils.h"
#include "cJSON.h"


namespace RAML
{
    class Raml
    {
        public:
            virtual std::string getTitle() const;
            virtual void setTitle(const std::string &title);

            virtual std::string getVersion() const;
            virtual void setVersion(const std::string &version);

            virtual std::string getBaseUri() const;
            virtual void setBaseUri(const std::string &baseUri);

            virtual std::list<std::string> getProtocols() const;
            virtual void setProtocol(const std::string &protocol);


            virtual std::map<std::string, UriParameter> getBaseUriParameters() const;
            virtual void setBaseUriParameter(const std::string &paramName, const UriParameter &uriParameter);

            virtual std::string getMediaType() const;
            virtual void setMediaType(const std::string &mediaType);

            virtual std::list<std::pair<std::string, Schema> > getSchemas() const;
            virtual void setSchema(const std::pair<std::string, Schema> &schema);

            virtual std::list<std::pair<std::string, RamlResource> > getResourceTypes() const;
            virtual void setResourceType(const std::pair<std::string, RamlResource> &resourceType);

            virtual std::list<std::pair<std::string, Action> > getTraits() const;
            virtual void setTrait(const std::pair<std::string, Action> &trait);

            virtual RamlResource &getResource(std::string resourceName);
            virtual std::map<std::string, RamlResource> getResources() const;
            virtual void setResource(const std::string &resourceKey, const RamlResource &resource);

            virtual void setDocumentationItem(const DocumentationItem &documentationItem);
            virtual std::list<DocumentationItem> getDocumentation() const;

            void readRamlFromYaml(const YAML::Node &yamlNode);
            Raml() : m_includeResolver(new IncludeResolver()) {}
            Raml(std::string &resourceLocation) : m_includeResolver(new IncludeResolver(resourceLocation)) {}
        private:
            std::string m_title;
            std::string m_version;
            std::string m_baseUri;
            std::list<std::string> m_protocols;
            std::map<std::string, UriParameter> m_baseUriParameters;
            std::string m_mediaType;
            std::list <std::pair<std::string, Schema> > m_schemas;
            std::list <std::pair<std::string, RamlResource> > m_resourceTypes;
            std::list <std::pair<std::string, Action> > m_traits;
            std::map<std::string, RamlResource> m_resources;
            std::list<DocumentationItem> m_documentation;
            IncludeResolver *m_includeResolver;
    };

    typedef std::shared_ptr<Raml> RamlPtr;

}
#endif
