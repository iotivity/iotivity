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

#ifndef BROKERTYPES_H_
#define BROKERTYPES_H_

#include <iostream>
#include <functional>

#include "OCPlatform.h"
#include "octypes.h"

#include "PrimitiveResource.h"

#define BROKER_TAG PCF("BROKER")
#define BASE_LAYER_C 0
#define BROKER_TRANSPORT OCConnectivityType::OC_IPV4

typedef std::function<void(std::shared_ptr<OC::OCResource>)> FindCB;

typedef std::function<OCStackResult(BROKER_STATE)> BrokerCB;

typedef std::function<DISCOVER_TRANSACTION(std::shared_ptr<PrimitiveResource> resource)> DiscoverCB;

enum class DISCOVER_TRANSACTION
{
    KEEP = 0,
    DELETE
};

enum class BROKER_STATE
{
    ALIVE = 0,
    REQUESTED,
    LOST_SIGNAL,
    DESTROYED
};

typedef OC::OCPlatform::OCPresenceHandle BasePresenceHandle;
typedef std::function<void(OCStackResult, const unsigned int,
        const std::string&)> SubscribeCallback;

typedef std::function<void(const HeaderOptions&, const ResponseStatement&, int)> GetCallback;

#endif // BROKERTYPES_H_
