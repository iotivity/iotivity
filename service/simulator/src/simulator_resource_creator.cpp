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

    if (configPath.length() > 0)
    {
        std::size_t found = configPath.find_last_of("/\\");
        if (found > configPath.length())
        {
            return resource;
        }
        std::string filePath = configPath.substr(0, found) + "/";
        std::string fileName = configPath.substr(found + 1);

        RamlPtr raml = (new RamlParser(filePath, fileName))->build();
        for (auto  resours : raml->getResources())
        {
            resource->setName(resours.first);
            resource->setURI(resours.second.getResourceUri());
            for (auto  action :  resours.second.getActions())
            {
                for (auto  response :  action.second.getResponses())
                {
                    for (auto bdy :  response.second.getResponseBody())
                    {
                        auto resourceProperties = bdy.second.getSchema()->getProperties();

                        resource->setResourceType(resourceProperties->getResoureType());
                        resource->setInterfaceType(resourceProperties->getInterface());

                        for ( auto property : resourceProperties->getAttributes() )
                        {
                            int type = property.second.getValueType();
                            if (type)
                            {
                                std::string attributeValue = property.second.getValueString();
                                resource->addAttribute(property.second.getName(), std::string(attributeValue));
                            }
                            else
                            {
                                int attributeValue = property.second.getValueInt();
                                resource->addAttribute(property.second.getName(), int(attributeValue));
                            }

                            resource->setUpdateInterval(property.second.getName(), property.second.getUpdateFrequencyTime());

                            int min = 0, max = 0;
                            property.second.getRange(min, max);
                            resource->setRange(property.second.getName(), min, max);


                            if (property.second.getAllowedValuesSize() > 0)
                                resource->setAllowedValues(property.second.getName(), property.second.getAllowedValues());
                        }
                        SIM_LOG(ILogger::INFO, "Created sample resource");
                        return resource;
                    }
                }

            }
        }
    }
    SIM_LOG(ILogger::INFO, "Created sample resource");
    return resource;
}

