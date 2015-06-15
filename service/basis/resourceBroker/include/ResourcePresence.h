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

#include "BrokerTypes.h"

class ResourcePresence
{
public:
    ResourcePresence(PrimitiveResourcePtr pResource, BrokerCB _cb);
    ~ResourcePresence();

    void addBrokerRequesterCB(BrokerCB _cb);
    void removeBrokerRequesterCB(BrokerCB _cb);
    void removeAllBrokerRequesterCB();

    void requestResourceState();

    PrimitiveResourcePtr getPrimitiveResource();
    BROKER_STATE getResourceState();
    void setResourcestate(BROKER_STATE _state);
    void executeAllBrokerCB();

private:
    PrimitiveResourcePtr primitiveResource;

    std::unique_ptr<std::list<BrokerRequesterInfoPtr>> requesterList;

    BROKER_STATE state;
    GetCallback pGetCB;
    TimeoutCallback pTimeoutCB;
    void GetCB(const HeaderOptions &hos, const ResponseStatement& rep, int seq);
    bool isWithinTime;
    void TimeOutCB(int msg);

};

#endif /* RESOURCEPRESENCE_H_ */

