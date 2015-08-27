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

#include "RamlResource.h"


namespace RAML
{
    RamlResource *RamlResource::getParentResource()
    {
        return m_parentResource;
    }
    void RamlResource::setParentResource(RamlResource *parentResource)
    {
        m_parentResource = parentResource;
    }

    std::string RamlResource::getDisplayName() const
    {
        return m_displayName;
    }
    void RamlResource::setDisplayName(const std::string &displayName)
    {
        m_displayName = displayName;
    }
    std::string RamlResource::getDescription() const
    {
        return m_description;
    }
    void RamlResource::setDescription(const std::string &description)
    {
        m_description = description;
    }
    std::string RamlResource::getParentUri() const
    {
        return m_parentUri;
    }
    void RamlResource::setParentUri(const std::string &parentUri)
    {
        m_parentUri = parentUri;
    }
    std::string RamlResource::getRelativeUri() const
    {
        return m_relativeUri;
    }
    void RamlResource::setRelativeUri(const std::string &relativeUri)
    {
        m_relativeUri = relativeUri;
    }
    std::map<std::string, UriParameter> RamlResource::getUriParameters() const
    {
        return m_uriParameters;
    }
    void RamlResource::setUriParameter(const std::string &paramName, const UriParameter &uriParameter)
    {
        m_uriParameters[paramName] = uriParameter;
    }
    std::map<std::string, UriParameter > RamlResource::getBaseUriParameters() const
    {
        return m_baseUriParameters;
    }
    void RamlResource::setBaseUriParameter(const std::string &paramName,
                                           const UriParameter &baseUriParameter)
    {
        m_baseUriParameters[paramName] = baseUriParameter;
    }
    Action &RamlResource::getAction(ActionType actionType)
    {
        return m_actions[actionType];
    }

    std::map<ActionType , Action> RamlResource::getActions() const
    {
        return m_actions;
    }
    void RamlResource::setAction(const ActionType &actiontype , const Action &action )
    {
        m_actions[actiontype] = action;
    }
    std::map<std::string, RamlResource> RamlResource::getResources() const
    {
        return m_resources;
    }
    void RamlResource::setResource(const std::string &resourceName, const RamlResource &resources)
    {
        m_resources[resourceName] = resources;
    }
    std::list<std::string> RamlResource::getTraits() const
    {
        return m_traits;
    }
    void RamlResource::setTrait(const std::string &trait)
    {
        m_traits.push_back(trait);
    }
    std::string RamlResource::getResourceType() const
    {
        return m_resourceType;
    }
    void RamlResource::setResourceType(const std::string &type)
    {
        m_resourceType = type;
    }
    std::string RamlResource::getResourceUri() const
    {
        return (m_parentUri + m_relativeUri);
    }
    void RamlResource::readResource(const std::string resourceKey, const YAML::Node &yamlNode,
                                    IncludeResolver *includeResolver, const std::string &parentUri)
    {
        m_includeResolver = includeResolver;
        m_relativeUri = resourceKey;
        m_parentUri = parentUri;
        for ( YAML::const_iterator it = yamlNode.begin(); it != yamlNode.end(); ++it )
        {
            std::string key = READ_NODE_AS_STRING(it->first);

            if (key == Keys::DisplayName)
                setDisplayName(READ_NODE_AS_STRING(it->second));
            else if (key == Keys::Description)
                setDescription(READ_NODE_AS_STRING(it->second));
            else if (std::find(Keys::ActionType.begin(), Keys::ActionType.end(), key) != Keys::ActionType.end())
            {
                ActionType actionType = GET_ACTION_TYPE(key);

                setAction(actionType, *(new Action(actionType, it->second, m_includeResolver)));
            }
            else if (key == Keys::UriParameters)
            {
                YAML::Node paramNode = it->second;
                for ( YAML::const_iterator tt = paramNode.begin(); tt != paramNode.end(); ++tt )
                {
                    UriParameter *uriParameter = new UriParameter(tt->second);
                    setUriParameter(READ_NODE_AS_STRING(tt->first), *uriParameter);
                }
            }
            else if (key == Keys::BaseUriParameters)
            {
                YAML::Node paramNode = it->second;
                for ( YAML::const_iterator tt = paramNode.begin(); tt != paramNode.end(); ++tt )
                {
                    UriParameter *uriParameter = new UriParameter(tt->second);
                    setBaseUriParameter(READ_NODE_AS_STRING(tt->first), *uriParameter);
                }
            }
            else if (key == Keys::IsTrait)
            {
                YAML::Node traitNode = it->second;
                for ( YAML::const_iterator tt = traitNode.begin(); tt != traitNode.end(); ++tt )
                {
                    setTrait(READ_NODE_AS_STRING(*tt));
                }
            }
            else if (key == Keys::Type)
            {
                setResourceType(READ_NODE_AS_STRING(it->second));
            }
        }
    }

}
