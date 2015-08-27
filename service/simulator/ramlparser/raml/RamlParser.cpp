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

#include "RamlParser.h"
#include <map>

namespace RAML
{
    RamlPtr RamlParser::build()
    {
        YAML::Node yamlRootNode = YAML::LoadFile(m_fileLocation + m_ramlName);
        m_ramlPtr->readRamlFromYaml(yamlRootNode);
        setDataFromRoot();
        return (m_ramlPtr);

    }
    RamlPtr RamlParser::getRamlPtr()
    {
        return m_ramlPtr;
    }
    void RamlParser::setDataFromRoot()
    {
        setTypes(getRamlPtr()->getResources());
        setTraits(getRamlPtr()->getResources());
        setBodyDefaultMediaType(getRamlPtr()->getResources());
        setBodySchema(getRamlPtr()->getResources());
    }
    void RamlParser::setBodyDefaultMediaType(std::map<std::string, RamlResource> resource)
    {
        if (getRamlPtr()->getMediaType().empty())
        {
            return;
        }
        for (auto it : resource)
        {
            std::string type = getRamlPtr()->getMediaType();

            for (auto  action :  it.second.getActions())
            {
                if (action.second.getRequestBody().empty())
                {
                    std::string resName = it.first;
                    getRamlPtr()->getResource(resName).getAction(action.first).setRequestBody(type);
                }
                for (auto  response : action.second.getResponses())
                {
                    if (response.second.getResponseBody().empty())
                    {
                        std::string resName = it.first;
                        std::string responseCode = response.first;
                        getRamlPtr()->getResource(resName).getAction(action.first).getResponse(
                            responseCode).setResponseBody(
                                type);
                    }
                }
            }
        }
    }
    void RamlParser::setBodySchema(std::map<std::string, RamlResource> resource)
    {
        if (getRamlPtr()->getSchemas().empty())
        {
            return;
        }
        for (auto  it : resource)
        {
            for (auto  action :  it.second.getActions())
            {
                for (auto  body :  action.second.getRequestBody())
                {
                    Schema *schema = body.second.getSchema();

                    if (schema != NULL)
                    {
                        std::string schemaValue = schema->getSchema();
                        auto pos = std::find_if(getRamlPtr()->getSchemas().begin(), getRamlPtr()->getSchemas().end(),
                                                [schemaValue](std::pair<std::string, Schema> const & pair)
                        {
                            return (pair.first == schemaValue);
                        });
                        if (pos != getRamlPtr()->getSchemas().end())
                        {
                            schema->setSchema((pos->second.getSchema()));
                        }
                    }
                }
                for (auto  response : action.second.getResponses())
                {
                    for (auto  body :  response.second.getResponseBody())
                    {
                        Schema *schema = body.second.getSchema();
                        if (schema != NULL)
                        {
                            std::string schemaValue = schema->getSchema();
                            auto schemas = getRamlPtr()->getSchemas();

                            auto iter = schemas.begin();
                            for (; iter != schemas.end(); iter++)
                            {
                                if (((*iter).first) == schemaValue)
                                    break;
                            }
                            if (iter != schemas.end())
                            {
                                schema->setSchema((*iter).second.getSchema());
                            }
                        }
                    }
                }
            }
        }
    }
    void RamlParser::setTypes(std::map<std::string, RamlResource> resource)
    {
        if (getRamlPtr()->getResourceTypes().empty())
        {
            return;
        }
        for (auto  it : resource)
        {
            auto resourceTypes = getRamlPtr()->getResourceTypes();
            std::string typeValue = it.second.getResourceType();

            auto iter = resourceTypes.begin();
            for (; iter != resourceTypes.end(); iter++)
            {
                if (((*iter).first) == typeValue)
                    break;
            }
            if (iter != resourceTypes.end())
            {
                std::string resName = it.first;
                RamlResource &res = getRamlPtr()->getResource(resName);
                RamlResource resType =  (*iter).second;

                if (resType.getActions().empty())
                    return;

                for (auto resActions : resType.getActions())
                {
                    if (res.getActions().count(resActions.first) == 0)
                        res.setAction(resActions.first, resActions.second);
                }
            }
        }
    }
    void RamlParser::setTraits(std::map<std::string, RamlResource> resource)
    {
        if (getRamlPtr()->getTraits().empty())
        {
            return;
        }
        for (auto  it : resource)
        {
            auto trait = getRamlPtr()->getTraits();
            for (auto act : it.second.getActions())
            {
                for (std::string  traitValue :  act.second.getTraits())
                {
                    auto iter = trait.begin();
                    for (; iter != trait.end(); iter++)
                    {
                        if (((*iter).first) == traitValue)
                            break;
                    }
                    if (iter != trait.end())
                    {
                        std::string resName = it.first;
                        RamlResource &res = getRamlPtr()->getResource(resName);
                        Action resTrait =  (*iter).second;

                        Action &action = res.getAction(act.first);
                        for (auto head : resTrait.getHeaders())
                        {
                            if (action.getHeaders().count(head.first) == 0)
                                action.setHeader(head.first, head.second);
                        }
                        for (auto query : resTrait.getQueryParameters())
                        {
                            if (action.getQueryParameters().count(query.first) == 0)
                                action.setQueryParameter(query.first, query.second);
                        }
                        for (auto resp : resTrait.getResponses())
                        {
                            if (action.getResponses().count(resp.first) == 0)
                                action.setResponse(resp.first, resp.second);
                        }
                    }
                }
            }
            for (std::string  traitValue :  it.second.getTraits())
            {
                auto iter = trait.begin();
                for (; iter != trait.end(); iter++)
                {
                    if (((*iter).first) == traitValue)
                        break;
                }
                if (iter != trait.end())
                {
                    std::string resName = it.first;
                    RamlResource &res = getRamlPtr()->getResource(resName);
                    Action resTrait =  (*iter).second;

                    for (auto act : res.getActions())
                    {
                        Action &action = res.getAction(act.first);
                        for (auto head : resTrait.getHeaders())
                        {
                            if (action.getHeaders().count(head.first) == 0)
                                action.setHeader(head.first, head.second);
                        }
                        for (auto query : resTrait.getQueryParameters())
                        {
                            if (action.getQueryParameters().count(query.first) == 0)
                                action.setQueryParameter(query.first, query.second);
                        }
                        for (auto resp : resTrait.getResponses())
                        {
                            if (action.getResponses().count(resp.first) == 0)
                                action.setResponse(resp.first, resp.second);
                        }
                    }
                }
            }
        }
    }

}
