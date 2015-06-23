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
#include <list>

#include "logger.h"
#include "PrimitiveResource.h"

#define BROKER_TAG PCF("BROKER")

#define BROKER_TRANSPORT OCConnectivityType::CT_ADAPTER_IP

using namespace OIC::Service;

/*
 * @BROKER_STATE
 * brief : resourcePresence state
 * ALIVE       - It means that 'getCB' function receives 'OK' message
 * REQUESTED   - It means that broker receives the request for presence checking
 * LOST_SIGNAL - In case that 'getCB' function receives the message except 'OK'
 * DESTROYED   - In case that the presence checking is dismissed for the resource ,
 *               or there is no matched value in the Broker Callback list
 * NONE        - To be determined.
 */
enum class BROKER_STATE
{
    ALIVE = 0,
    REQUESTED,
    LOST_SIGNAL,
    DESTROYED,
    NONE
};

/*
 * @DEVICE_STATE
 * brief : devicePresence state
 * ALIVE       - It means that 'subscribeCB' function receives 'OK' message
 * REQUESTED   - It means that broker receives the request for presence checking
 * LOST_SIGNAL - In case that 'subscribeCB' function receives the message except 'OK'
 */
enum class DEVICE_STATE
{
    ALIVE = 0,
    REQUESTED,
    LOST_SIGNAL
};

enum class BROKER_MODE
{
    DEVICE_PRESENCE_MODE = 0,
    NON_PRESENCE_MODE
};

typedef unsigned int BrokerID;

typedef std::function<void(BROKER_STATE)> BrokerCB;
struct BrokerRequesterInfo
{
    BrokerID brockerId;
    BrokerCB brockerCB;
};
typedef std::shared_ptr<BrokerRequesterInfo> BrokerRequesterInfoPtr;

class ResourcePresence;
class DevicePresence;

typedef std::function<void(std::shared_ptr<OC::OCResource>)> FindCB;

typedef std::shared_ptr<PrimitiveResource> PrimitiveResourcePtr;

typedef std::shared_ptr<ResourcePresence> ResourcePresencePtr;
typedef std::shared_ptr<DevicePresence> DevicePresencePtr;
typedef std::list< ResourcePresencePtr > PresenceList;

struct BrokerCBResourcePair
{
    BrokerCBResourcePair(ResourcePresencePtr _pResource, BrokerCB _cb)
    : pResource(_pResource), brokerCB(_cb){}
    ResourcePresencePtr pResource;
    BrokerCB brokerCB;
};
typedef std::map<BrokerID, BrokerCBResourcePair> BrokerIDMap;

typedef std::function<void(OCStackResult, const unsigned int,
        const std::string&)> SubscribeCB;

typedef std::function<void(const HeaderOptions&, const ResponseStatement&, int)> RequestGetCB;
typedef std::function<void(int)> TimeoutCB;

#endif // BROKERTYPES_H_
