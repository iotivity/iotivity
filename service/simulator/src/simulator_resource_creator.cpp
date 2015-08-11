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

SimulatorResourceServerPtr SimulatorResourceCreator::createLightResoure()
{
    std::shared_ptr<SimulatorResourceServer> lightResource(new SimulatorResourceServer);

    // set power attribute with its properties
    {
        lightResource->addAttribute("power", std::string("on"));
        std::vector<std::string> values {"on", "off"};
        lightResource->setAllowedValues("power", values);
        lightResource->setUpdateInterval("power", 2000);
    }

    // set intensity attributes with its properties
    {
        lightResource->addAttribute("intensity", int(1));
        lightResource->setRange("intensity", 1, 10);
        lightResource->setUpdateInterval("intensity", 3000);
    }

    // set other properties
    lightResource->setName("Light");
    lightResource->setURI("/oic/light");
    lightResource->setResourceType("oic.light");
    lightResource->setInterfaceType(OC::DEFAULT_INTERFACE);

    SIM_LOG(ILogger::INFO, "Created sample light resource");
    return lightResource;
}

