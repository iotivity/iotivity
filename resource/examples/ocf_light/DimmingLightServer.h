/*
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 *
 * Copyright 2018 Intel Corporation All Rights Reserved.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
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
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

#ifndef EXAMPLE_OCF_LIGHT_DIMMINGLIGHTSERVER_H_
#define EXAMPLE_OCF_LIGHT_DIMMINGLIGHTSERVER_H_

#include "BinarySwitchResource.h"
#include "DimmingResource.h"

/**
 * Enables registering the resources for the dimming light server
 */
class DimmingLightServer
{
    friend class DimmingLightControl;
public:
    /**
     *  constructor
     */
    DimmingLightServer(void);

    /**
     *  destructor
     */
    ~DimmingLightServer(void);

    /**
     * Register the resources with the server
     *
     * setting resourceProperty as OC_DISCOVERABLE will allow Discovery of this resource
     * setting resourceProperty as OC_OBSERVABLE will allow observation
     * setting resourceProperty as OC_DISCOVERABLE | OC_OBSERVABLE will allow both discovery and observation
     * setting resourceProperty as OC_SECURE the resource supports access via secure endpoints
     * setting resourceProperty as OC_NONSECURE the resource supports access via non-secure endpoints
     * setting resourceProperty as OC_SECURE | OC_NONSECURE will allow access via secure and non-secure endpoints
     *
     * @param resourceProperty indicates the property of the resources. Defined in octypes.h.
     */
    OCStackResult registerResources(uint8_t resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE | OC_SECURE);

private:
    // Resources that are part of this server
    BinarySwitchResource  m_binaryswitchInstance;
    DimmingResource  m_dimmingInstance;
};

#endif /* EXAMPLE_OCF_LIGHT_DIMMINGLIGHTSERVER_H_ */
