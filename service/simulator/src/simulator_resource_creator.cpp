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

SimulatorResourceServerPtr SimulatorResourceCreator::createResource(const std::string &configPath)
{
    std::shared_ptr<SimulatorResourceServer> resource(new SimulatorResourceServer);

    RamlParser *ramlParser = new RamlParser(configPath);
    RamlPtr raml = ramlParser->getRamlPtr();
        for (auto  resours : raml->getResources())
        {
            resource->setName(resours.first);
        resource->setURI(resours.second->getResourceUri());
        // TODO: Currently setting only baseline interface.
        resource->setInterfaceType(OC::DEFAULT_INTERFACE);
        // TODO: Need to modify based on the spec for observable property
        resource->setObservable(true);
        for (auto  action :  resours.second->getActions())
            {
            for (auto  response :  action.second->getResponses())
                {
                for (auto bdy :  response.second->getResponseBody())
                    {
                    auto resourceProperties = bdy.second->getSchema()->getProperties();

                    for ( auto property : resourceProperties->getProperties() )
                    {
                            int type = property.second->getValueType();
                            if (type)
                            {
                                std::string attributeValue = property.second->getValueString();
                                resource->addAttribute(property.second->getName(), std::string(attributeValue));
                            }
                            else
                            {
                                int attributeValue = property.second->getValueInt();
                                resource->addAttribute(property.second->getName(), int(attributeValue));
                            }

                            resource->setUpdateInterval(property.second->getName(), property.second->getUpdateFrequencyTime());

                            int min = 0, max = 0, multipleof = 0;
                            property.second->getRange(min, max, multipleof);
                            resource->setRange(property.second->getName(), min, max);


                            if (property.second->getAllowedValuesSize() > 0)
                                resource->setAllowedValues(property.second->getName(), property.second->getAllowedValues());
                        }
                        SIM_LOG(ILogger::INFO, "Created sample resource");
                        return resource;
                    }
                }

            }
        }

    return resource;
}

