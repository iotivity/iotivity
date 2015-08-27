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

#include "Action.h"

namespace RAML
{
    ActionType Action::getType() const
    {
        return m_type;
    }
    void Action::setType(const ActionType &type)
    {
        m_type = type;
    }
    std::string Action::getDescription() const
    {
        return m_description;
    }
    void Action::setDescription(const std::string &description)
    {
        m_description = description;
    }
    std::map<std::string, Header> Action::getHeaders() const
    {
        return m_headers;
    }
    void Action::setHeader(const std::string &headerName, const Header &header)
    {
        m_headers[headerName] = header;
    }
    std::map<std::string, QueryParameter> Action::getQueryParameters()const
    {
        return m_queryParameters;
    }
    void Action::setQueryParameter(const std::string &paramName, const QueryParameter &queryParameter)
    {
        m_queryParameters[paramName] = queryParameter;
    }
    RequestResponseBody &Action::getRequestBody(std::string bodyType)
    {
        return m_requestBody[bodyType];
    }

    std::map<std::string, RequestResponseBody> Action::getRequestBody() const
    {
        return m_requestBody;
    }
    void Action::setRequestBody(const std::string &typeName)
    {
        m_requestBody[typeName] = *(new RequestResponseBody(typeName));
    }

    void Action::setRequestBody(const std::string &typeName , const RequestResponseBody &body)
    {
        m_requestBody[typeName] = body;
    }
    Response &Action::getResponse(std::string responseCode)
    {
        return m_responses[responseCode];
    }

    std::map<std::string, Response> Action::getResponses() const
    {
        return m_responses;
    }
    void Action::setResponse(const std::string &responseCode, const Response &response)
    {
        m_responses[responseCode] = response;
    }

    std::list<std::string> Action::getProtocols() const
    {
        return m_protocols;
    }
    void Action::setProtocol(const std::string &protocol)
    {
        m_protocols.push_back(protocol);
    }
    std::map<std::string, UriParameter > Action::getBaseUriParameters() const
    {
        return m_baseUriParameters;
    }
    void Action::setBaseUriParameter(const std::string &paramName ,
                                     const UriParameter  &baseUriParameter)
    {
        m_baseUriParameters[paramName] = baseUriParameter;
    }

    std::list<std::string> Action::getTraits() const
    {
        return m_trait;
    }
    void Action::setTrait(const std::string &trait)
    {
        m_trait.push_back(trait);
    }
    void Action::readAction(const ActionType actionType, const YAML::Node &yamlNode,
                            IncludeResolver *includeResolver)
    {
        m_includeResolver = includeResolver;
        m_type = actionType;
        for ( YAML::const_iterator it = yamlNode.begin(); it != yamlNode.end(); ++it )
        {
            std::string key = READ_NODE_AS_STRING(it->first);

            if (key == Keys::Description)
                setDescription(READ_NODE_AS_STRING(it->second));
            else if (key == Keys::Responses)
            {
                YAML::Node responseNode = it->second;
                for ( YAML::const_iterator tt = responseNode.begin(); tt != responseNode.end(); ++tt )
                {
                    std::string responseCode = READ_NODE_AS_STRING(tt->first);
                    setResponse(responseCode, *(new Response(tt->second, m_includeResolver)));
                }
            }
            else if (key == Keys::Headers)
            {
                YAML::Node paramNode = it->second;
                for ( YAML::const_iterator tt = paramNode.begin(); tt != paramNode.end(); ++tt )
                {
                    Header *header = new Header(tt->second);
                    setHeader(READ_NODE_AS_STRING(tt->first), *header);
                }
            }
            else if (key == Keys::QueryParameters)
            {
                YAML::Node paramNode = it->second;
                for ( YAML::const_iterator tt = paramNode.begin(); tt != paramNode.end(); ++tt )
                {
                    QueryParameter *queryParameter = new QueryParameter(tt->second);
                    setQueryParameter(READ_NODE_AS_STRING(tt->first), *queryParameter);
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
            else if (key == Keys::BaseUriParameters)
            {
                YAML::Node paramNode = it->second;
                for ( YAML::const_iterator tt = paramNode.begin(); tt != paramNode.end(); ++tt )
                {
                    UriParameter *uriParameter = new UriParameter(tt->second);
                    setBaseUriParameter(READ_NODE_AS_STRING(tt->first), *uriParameter);
                }
            }
            else if (key == Keys::Body)
            {
                YAML::Node responseBody = it->second;

                for ( YAML::const_iterator tt = responseBody.begin(); tt != responseBody.end(); ++tt )
                {
                    std::string type = READ_NODE_AS_STRING(tt->first);
                    setRequestBody(type, *(new RequestResponseBody(type, tt->second, m_includeResolver)));
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
        }
    }

};

