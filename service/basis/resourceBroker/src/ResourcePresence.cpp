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

#include "ResourcePresence.h"

ResourcePresence::ResourcePresence(PrimitiveResourcePtr pResource, BrokerCB _cb)
{
    primitiveResource = pResource;

    pGetCB = std::bind(&ResourcePresence::GetCB, this,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

    requesterList
    = std::unique_ptr<std::list<BrokerRequesterInfoPtr>>(new std::list<BrokerRequesterInfoPtr>);

    addBrokerRequesterCB(_cb);
}

ResourcePresence::~ResourcePresence()
{
    requesterList->clear();
}

void ResourcePresence::addBrokerRequesterCB(BrokerCB _cb)
{
    BrokerRequesterInfoPtr newRequester = BrokerRequesterInfoPtr(new BrokerRequesterInfo());
    newRequester->brockerCB = _cb;
    requesterList->push_back(newRequester);
}

void ResourcePresence::requestResourceState()
{
    primitiveResource->requestGet(pGetCB);
}

void ResourcePresence::GetCB(const HeaderOptions &hos, const ResponseStatement& rep, int seq)
{

}
