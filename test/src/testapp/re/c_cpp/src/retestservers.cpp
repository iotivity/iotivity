/******************************************************************
*
* Copyright 2016 Samsung Electronics All Rights Reserved.
*
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

#include <vector>
#include <iostream>
#include <stdlib.h>

#include "REHelper.h"

int main()
{
    PlatformConfig config
    {
        OC::ServiceType::InProc, ModeType::Both, "0.0.0.0", 0, OC::QualityOfService::LowQos
    };

    OCPlatform::Configure(config);

    RCSResourceObject::Builder* pBuilderLight;
    shared_ptr< RCSResourceObject > resourceObjectLight;

    RCSResourceObject::Builder* pBuilderFan;
    shared_ptr< RCSResourceObject > resourceObjectFan;

    pBuilderLight = new RCSResourceObject::Builder(LIGHT_URI, RESOURCE_TYPE_LIGHT,
            DEFAULT_LIGHT_INTERFACE);
    pBuilderLight->setDiscoverable(true);
    pBuilderLight->setObservable(true);
    resourceObjectLight = pBuilderLight->build();
    resourceObjectLight->setAttribute(DEFAULT_POWER_KEY, DEFAULT_POWER_VALUE);
    resourceObjectLight->setAttribute(DEFAULT_INTENSITY_KEY, DEFAULT_INTENSITY_VALUE);
    resourceObjectLight->setAttribute(DEFAULT_TEMPETATURE_KEY, DEFAULT_TEMPETATURE_VALUE);

    pBuilderFan = new RCSResourceObject::Builder(FAN_URI, RESOURCE_TYPE_FAN, DEFAULT_INTERFACE);
    pBuilderFan->setDiscoverable(true);
    pBuilderFan->setObservable(true);
    resourceObjectFan = pBuilderFan->build();
    resourceObjectFan->setAttribute(DEFAULT_POWER_KEY, DEFAULT_POWER_VALUE);
    resourceObjectFan->setAttribute(DEFAULT_INTENSITY_KEY, DEFAULT_INTENSITY_VALUE);

    if (resourceObjectFan == NULL || resourceObjectLight == NULL)
    {
        IOTIVITYTEST_LOG(ERROR, "Unable to start servers...");
        return 1;
    }

    IOTIVITYTEST_LOG(INFO, "Server is running :-)....");

    while(true)
    {
    }


    return 0;
}
