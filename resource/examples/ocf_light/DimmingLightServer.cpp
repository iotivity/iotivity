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

#include <iostream>

#include "DimmingLightServer.h"
#include "OCPlatform.h"

using namespace OC;

DimmingLightServer::DimmingLightServer(void):
m_binaryswitchInstance(),
m_dimmingInstance()
{
    std::cout << "Running: DimmingLightServer constructor" << std::endl;
}

DimmingLightServer::~DimmingLightServer(void)
{
    std::cout << "Running: DimmingLightServer destructor" << std::endl;
}

OCStackResult DimmingLightServer::registerResources(uint8_t resourceProperty)
{
    OCStackResult result = OC_STACK_ERROR;
    result = m_binaryswitchInstance.registerResource(resourceProperty);
    if (OC_STACK_OK != result)
    {
        return result;
    }
    result = m_dimmingInstance.registerResource(resourceProperty);
    return result;
}
