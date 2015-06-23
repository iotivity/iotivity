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

#ifndef DEVICEPRESENCE_H_
#define DEVICEPRESENCE_H_

#include <list>
#include <string>

#include "BrokerTypes.h"
#include "ResourcePresence.h"
#include "PresenceSubscriber.h"

class DevicePresence
{
public:
    DevicePresence(PrimitiveResourcePtr pResource);
    ~DevicePresence();

    const std::string getAddress() const;
    void addPresenceResource(ResourcePresence * rPresence);
    void removePresenceResource(ResourcePresence * rPresence);

    bool isEmptyResourcePresence() const;

private:
    std::string address;
    DEVICE_STATE state;
    bool isWithinTime;

    std::list<ResourcePresence * > resourcePresenceList;

    SubscribeCB pSubscribeRequestCB;
    TimeoutCB pTimeoutCB;
    PresenceSubscriber presenceSubscriber;

    void requestAllResourcePresence();

    void subscribeCB(OCStackResult ret,const unsigned int seq, const std::string& Hostaddress);
    void timeOutCB(int msg);

};

#endif /* DEVICEPRESENCE_H_ */
