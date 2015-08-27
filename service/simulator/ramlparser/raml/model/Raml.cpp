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

#include "Raml.h"

namespace RAML
{

    std::string Raml::getTitle() const
    {
        return m_title;
    }
    void Raml::setTitle(const std::string &title)
    {
        m_title = title;
    }

    std::string Raml::getVersion() const
    {
        return m_version;
    }
    void Raml::setVersion(const std::string &version)
    {
        m_version = version;
    }

    std::string Raml::getBaseUri() const
    {
        return m_baseUri;
    }
    void Raml::setBaseUri(const std::string &baseUri)
    {
        m_baseUri = baseUri;
    }

    std::list<std::string> Raml::getProtocols() const
    {
        return m_protocols;
    }
    void Raml::setProtocol(const std::string &protocol)
    {
        m_protocols.push_back(protocol);
    }
    std::map<std::string, UriParameter> Raml::getBaseUriParameters() const
    {
        return m_baseUriParameters;
    }
    void Raml::setBaseUriParameter(const std::string &paramName, const UriParameter &uriParameter)
    {
        m_baseUriParameters[paramName] = uriParameter;
    }

    std::string Raml::getMediaType() const
    {
        return m_mediaType;
    }
    void Raml::setMediaType(const std::string &mediaType)
    {
        m_mediaType = mediaType;
    }

    std::list<std::pair<std::string, Schema> > Raml::getSchemas() const
    {
        return m_schemas;
    }

    void Raml::setSchema(const std::pair<std::string, Schema> &schema)
    {
        m_schemas.push_back(schema);
    }

    std::list<std::pair<std::string, RamlResource> > Raml::getResourceTypes() const
    {
        return m_resourceTypes;
    }
    void Raml::setResourceType(const std::pair<std::string, RamlResource> &resourceType)
    {
        m_resourceTypes.push_back(resourceType);
    }

    std::list<std::pair<std::string, Action> > Raml::getTraits() const
    {
        return m_traits;
    }
    void Raml::setTrait(const std::pair<std::string, Action> &trait)
    {
        m_traits.push_back(trait);
    }
    RamlResource &Raml::getResource(std::string resourceName)
    {
        return m_resources[resourceName];
    }

    std::map<std::string, RamlResource> Raml::getResources() const
    {
        return m_resources;
    }

    void Raml::setResource(const std::string &resourceKey, const RamlResource &resource)
    {
        m_resources[resourceKey] = resource;
    }

    void Raml::setDocumentationItem(const DocumentationItem &documentationItem)
    {
        m_documentation.push_back(documentationItem);
    }

    std::list<DocumentationItem> Raml::getDocumentation() const
    {
        return m_documentation;
    }
    void Raml::readRamlFromYaml(const YAML::Node &yamlNode )
    {
        if (yamlNode.Type() == YAML::NodeType::Map)
        {
            for ( YAML::const_iterator it = yamlNode.begin(); it != yamlNode.end(); ++it )
            {
                std::string key = READ_NODE_AS_STRING(it->first);
                if (key == Keys::Title)
                {
                    setTitle(READ_NODE_AS_STRING(it->second));
                }
                else if (key == Keys::Version)
                {
                    setVersion(READ_NODE_AS_STRING(it->second));
                }
                else if (key == Keys::BaseUri)
                {
                    setBaseUri(READ_NODE_AS_STRING(it->second));
                }
                else if ((key == Keys::BaseUriParameters) || (key == Keys::UriParameters))
                {
                    YAML::Node paramNode = it->second;
                    for ( YAML::const_iterator tt = paramNode.begin(); tt != paramNode.end(); ++tt )
                    {
                        UriParameter *uriParameter = new UriParameter(tt->second);
                        setBaseUriParameter(READ_NODE_AS_STRING(tt->first), *uriParameter);
                    }
                }
                else if (key == Keys::Protocols)
                {
                    YAML::Node protocolNode = it->second;
                    for ( YAML::const_iterator tt = protocolNode.begin(); tt != protocolNode.end(); ++tt )
                    {
                        setProtocol(READ_NODE_AS_STRING(*tt));
                    }
                }
                else if (key == Keys::MediaType)
                {
                    setMediaType(READ_NODE_AS_STRING(it->second));
                }
                else if (key == Keys::Documentation)
                {
                    YAML::Node docNode = it->second;
                    for ( YAML::const_iterator iit = docNode.begin(); iit != docNode.end(); ++iit )
                    {
                        std::string title ;
                        std::string content ;

                        for ( YAML::const_iterator tt = (*iit).begin(); tt != (*iit).end(); ++tt )
                        {
                            std::string key = READ_NODE_AS_STRING(tt->first);

                            if (key == Keys::Title)
                                title = READ_NODE_AS_STRING(tt->second);
                            else if (key == Keys::Content)
                                content = READ_NODE_AS_STRING(tt->second);

                        }
                        setDocumentationItem(*(new DocumentationItem(title, content)));
                    }
                }
                else if (key == Keys::Schemas)
                {
                    YAML::Node schemaNode = it->second;
                    for ( YAML::const_iterator iit = schemaNode.begin(); iit != schemaNode.end(); ++iit )
                    {
                        for ( YAML::const_iterator tt = (*iit).begin(); tt != (*iit).end(); ++tt )
                        {
                            std::string key = READ_NODE_AS_STRING(tt->first);
                            std::pair<std::string, Schema> schema;

                            IncludeResolver::FileType fileType = m_includeResolver->getFileType(tt->second);
                            if ((fileType == IncludeResolver::FileType::JSON) || (fileType == IncludeResolver::FileType::FILE))
                            {
                                Schema *schemaPtr = new Schema(m_includeResolver->readFromFile(tt->second));
                                schema = std::make_pair(key, *schemaPtr);
                            }
                            else
                            {
                                std::string value = READ_NODE_AS_STRING(tt->second);
                                schema = std::make_pair(key, *(new Schema(value)));
                            }
                            setSchema(schema);
                        }
                    }
                }

                else if (key.compare(0, Keys::Resource.length(), Keys::Resource)  == 0)
                {
                    RamlResource *resource = new RamlResource(key, it->second, m_includeResolver, getBaseUri());
                    setResource(key, *resource);
                }
                else if (key == Keys::Traits)
                {
                    YAML::Node traitNode = it->second;
                    for ( YAML::const_iterator tt = traitNode.begin(); tt != traitNode.end(); ++tt )
                    {
                        for (auto elem : *tt)
                        {
                            std::string trait = READ_NODE_AS_STRING(elem.first);
                            Action *action = new Action(ActionType::NONE, elem.second , m_includeResolver);

                            std::pair<std::string, Action> resourceTrait;
                            resourceTrait = std::make_pair(trait, *action);

                            setTrait(resourceTrait);
                        }
                    }
                }
                else if (key == Keys::ResourceTypes)
                {
                    YAML::Node typeNode = it->second;
                    for ( YAML::const_iterator tt = typeNode.begin(); tt != typeNode.end(); ++tt )
                    {
                        for (auto elem : *tt)
                        {
                            std::string type = READ_NODE_AS_STRING(elem.first);
                            RamlResource *resource = new RamlResource(type, elem.second, m_includeResolver, getBaseUri());

                            std::pair<std::string, RamlResource> resourceType;
                            resourceType = std::make_pair(type, *resource);

                            setResourceType(resourceType);

                        }

                    }
                }
            }
        }

    }

}
