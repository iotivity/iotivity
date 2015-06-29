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

#ifndef RESOURCEPRESENCE_H_
#define RESOURCEPRESENCE_H_

#include <functional>
#include <list>
#include <string>
#include <atomic>

#include "BrokerTypes.h"
#include "../../common/primitiveTimer/include/PrimitiveTimer.h"

class ResourcePresence
{
public:
    ResourcePresence(PrimitiveResourcePtr pResource);
    ~ResourcePresence();

    void addBrokerRequester(BrokerID _id, BrokerCB _cb);
    void removeBrokerRequester(BrokerID _id);
    void removeAllBrokerRequester();
    void requestResourceState();
    void setResourcestate(BROKER_STATE _state);
    void executeAllBrokerCB(BROKER_STATE changedState);
    void changePresenceMode(BROKER_MODE newMode);

    bool isEmptyRequester() const;

    PrimitiveResourcePtr getPrimitiveResource();
    BROKER_STATE getResourceState();

private:
    void getCB(const HeaderOptions &hos, const ResponseStatement& rep, int eCode);
    void verifiedGetResponse(int eCode);
    void registerDevicePresence();
    void * timeOutCB(unsigned int msg);
    void * pollingCB(unsigned int msg);

    std::unique_ptr<std::list<BrokerRequesterInfoPtr>> requesterList;
    PrimitiveResourcePtr primitiveResource;
    PrimitiveTimer primitiveTimer;

    BROKER_STATE state;
    BROKER_MODE mode;

    bool isWithinTime;
    std::atomic_bool isTimeoutCB;
    std::atomic_long receivedTime;
    unsigned int timeoutHandle;

    RequestGetCB pGetCB;
    TimeoutCB pTimeoutCB;
    TimeoutCB pPollingCB;

};

#endif /* RESOURCEPRESENCE_H_ */

