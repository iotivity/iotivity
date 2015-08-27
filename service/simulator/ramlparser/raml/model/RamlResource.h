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

#ifndef RESOURCE_H
#define RESOURCE_H

#include <map>
#include <list>
#include <string>
#include "UriParameter.h"
#include "ActionType.h"
#include "Action.h"
#include "Utils.h"
#include "IncludeResolver.h"

namespace RAML
{
    class RamlResource
    {
        public:
            virtual RamlResource *getParentResource();
            virtual void setParentResource(RamlResource *parentResource);

            virtual std::string getDisplayName() const;
            virtual void setDisplayName(const std::string &displayName);

            virtual std::string getDescription() const;
            virtual void setDescription(const std::string &description);

            virtual std::string getParentUri() const;
            virtual void setParentUri(const std::string &parentUri);

            virtual std::string getRelativeUri() const;
            virtual void setRelativeUri(const std::string &relativeUri);

            virtual std::map<std::string, UriParameter> getUriParameters() const;
            virtual void setUriParameter(const std::string &paramName, const UriParameter &uriParameter);

            virtual std::map<std::string, UriParameter > getBaseUriParameters() const;
            virtual void setBaseUriParameter(const std::string &paramName,
                                             const UriParameter &baseUriParameter);

            virtual Action &getAction(ActionType actionType);
            virtual std::map<ActionType , Action> getActions() const;
            virtual void setAction(const ActionType &actiontype , const Action &action );

            virtual std::map<std::string, RamlResource> getResources() const;
            virtual void setResource(const std::string &resourceName, const RamlResource &resources);

            virtual std::list<std::string> getTraits() const;
            virtual void setTrait(const std::string &trait);

            virtual std::string getResourceType() const;
            virtual void setResourceType(const std::string &type);

            virtual std::string getResourceUri() const;
            RamlResource() {}
            RamlResource(const std::string resourceKey, const YAML::Node &yamlNode ,
                         IncludeResolver *includeResolver, const std::string &parentUri) { readResource(resourceKey, yamlNode, includeResolver, parentUri); }
        private:
            void readResource(const std::string resourceKey, const YAML::Node &yamlNode,
                              IncludeResolver *includeResolver, const std::string &parentUri);
        private:
            std::string m_displayName;
            std::string m_description;
            std::string m_relativeUri;
            std::map<std::string, UriParameter> m_uriParameters;
            std::map<std::string, UriParameter > m_baseUriParameters;
            std::map<ActionType , Action> m_actions;
            std::list<std::string> m_traits;
            std::string m_resourceType;
            RamlResource *m_parentResource;
            std::string m_parentUri;
            std::map<std::string, RamlResource> m_resources;
            IncludeResolver *m_includeResolver;
    };
}
#endif
