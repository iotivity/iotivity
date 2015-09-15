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

#include "simulator_resource_creator.h"
#include "simulator_logger.h"
#include <RamlParser.h>
#include <boost/lexical_cast.hpp>

using namespace RAML;

unsigned int SimulatorResourceCreator::s_id;
SimulatorResourceServerImplSP SimulatorResourceCreator::createResource(
    const std::string &configPath)
{
    RamlParser *ramlParser = new RamlParser(configPath);
    RamlPtr raml = ramlParser->getRamlPtr();
    for (auto  resourceElement : raml->getResources())
    {
        SimulatorResourceServerImplSP resource(new SimulatorResourceServerImpl());
        resource->setName(resourceElement.first);
        resource->setURI(resourceElement.second->getResourceUri());

        // TODO: Currently setting only baseline interface.
        resource->setInterfaceType(OC::DEFAULT_INTERFACE);
        // TODO: Need to modify based on the spec for observable property
        resource->setObservable(true);

        for (auto  action :  resourceElement.second->getActions())
        {
            for (auto  response :  action.second->getResponses())
            {
                for (auto bdy :  response.second->getResponseBody())
                {
                    auto resourceProperties = bdy.second->getSchema()->getProperties();

                    for ( auto property : resourceProperties->getProperties() )
                    {
                        std::string propName = property.second->getName();

                        if (propName == "rt")
                        {
                            resource->setResourceType(property.second->getValueString());
                            continue;
                        }

                        // Include more property names if required based on spec support.
                        if (propName == "if" || propName == "p" || propName == "n" || propName == "id")
                            continue;

                        SimulatorResourceModel::Attribute *attr = new SimulatorResourceModel::Attribute(propName);

                        int type = property.second->getValueType();
                        switch (type)
                        {
                            case 0: // Integer
                                {
                                    int attributeValue = property.second->getValueInt();
                                    attr->setValue(attributeValue);
                                }
                                break;

                            case 3: // String
                                {
                                    std::string attributeValue = property.second->getValueString();
                                    attr->setValue(attributeValue);
                                }
                                break;
                        }

                        attr->setUpdateFrequencyTime(property.second->getUpdateFrequencyTime());

                        int min = 0, max = 0, multipleof = 0;
                        property.second->getRange(min, max, multipleof);
                        attr->setRange(min, max);

                        if (property.second->getAllowedValuesSize() > 0)
                            attr->setAllowedValues(property.second->getAllowedValues());

                        resource->addAttribute(*attr);
                    }

                    resource->setURI(constructURI(resource->getURI()));
                    return resource;
                }
            }
        }
    }

    return nullptr;
}

/**
 * This method appends a unique key to the given URI to make the URI unique in simulator.
 * Example: If input is "/a/light", then the output will be "/a/light/simulator/0" for the first resource
 * and "/a/light/simulator/1" for the second resource and so on.
 */
std::string SimulatorResourceCreator::constructURI(const std::string &uri)
{
    std::ostringstream os;
    os << uri;
    if (!uri.empty() && '/' != uri[uri.length() - 1])
        os << '/';
    os << "simulator/" << s_id++;
    return os.str();
}

