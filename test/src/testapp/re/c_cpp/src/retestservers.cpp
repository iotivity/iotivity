/***************************************************************************************************
 * Copyright (c) 2010 Samsung Electronics Co., Ltd. All right reserved.
 *
 * This software is the confidential and proprietary information of Samsung Electronics, Inc.
 * ("Confidential Information"). You shall not disclose such Confidential Information and shall
 * use it only in accordance with the terms of the license agreement you entered into with
 * Samsung Electronics.
 ***************************************************************************************************/

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
    resourceObjectLight->setAttribute(DEFAULT_TEMPETATURE_KEY, DEFAULT_TEMPETATURE_VALUE);

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
