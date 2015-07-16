//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include "hosting.h"

// Standard API
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

#include "octypes.h"
#include "logger.h"
#include "ResourceHosting.h"

#define OC_TRANSPORT CT_ADAPTER_IP
#define HOSTING_TAG  PCF("Hosting")

namespace
{
    OIC::Service::ResourceHosting * rhInstance = OIC::Service::ResourceHosting::getInstance();
}

OCStackResult OICStartCoordinate()
{
    OCStackResult retResult = OC_STACK_OK;
    try
    {
        rhInstance->startHosting();
    }catch(...)
    {
        retResult = OC_STACK_ERROR;
    }

    return retResult;
}

OCStackResult OICStopCoordinate()
{
    OCStackResult retResult = OC_STACK_OK;
    rhInstance->stopHosting();

    return retResult;
}
