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

#include "BrokerTypes.h"
#include "ResourceBroker.h"

#define OIC_COAP "coap://"
#define DEFAULT_CONTEXT_VALUE 0x99

ResourceBroker * ResourceBroker::s_instance = NULL;
std::mutex ResourceBroker::s_mutexForCreation;
std::unique_ptr<std::list< ResourcePresencePtr >>  ResourceBroker::s_presenceList(nullptr);

ResourceBroker::ResourceBroker()
{
    // TODO Auto-generated constructor stub
    if(s_presenceList == nullptr)
    {
        s_presenceList = std::unique_ptr<std::list< ResourcePresencePtr >>(new std::list<ResourcePresencePtr>);
    }
}

ResourceBroker::~ResourceBroker()
{
    // TODO Auto-generated destructor stub
    if(s_presenceList != nullptr)
    {
        s_presenceList->clear();
    }
}

ResourceBroker * ResourceBroker::getInstance()
{
    if (!s_instance)
    {
        s_mutexForCreation.lock();
        if (!s_instance)
        {
            s_instance = new ResourceBroker();
        }
        s_mutexForCreation.unlock();
    }
    return s_instance;
}

OCStackResult ResourceBroker::hostResource(PrimitiveResourcePtr pResource, BrokerCB cb)
{
    OCStackResult ret = OC_STACK_INVALID_PARAM;
    if (pResource->getUri().empty() || pResource->getHost().empty())
    {
        return ret;
    }
    if (cb == NULL)
    {
        ret = OC_STACK_INVALID_CALLBACK;
        return ret;
    }

    ResourcePresencePtr presenceItem = findResourcePresence(pResource);
    if(presenceItem == nullptr)
    {
        OC_LOG_V(DEBUG, BROKER_TAG, "Not found any Handled Resource.");
        OC_LOG_V(DEBUG, BROKER_TAG, "Create New Resource Presence Handler.");

        presenceItem = ResourcePresencePtr(new ResourcePresence(pResource));
        s_presenceList->push_back(presenceItem);
    }
    else
    {
        presenceItem->addBrokerRequesterCB(cb);
    }

    return ret;
}

OCStackResult ResourceBroker::cancelHostResource(PrimitiveResourcePtr pResource)
{
    return OC_STACK_OK;
}

BROKER_STATE ResourceBroker::getResourceState(PrimitiveResourcePtr pResource)
{
    BROKER_STATE retState = BROKER_STATE::NONE;

    ResourcePresencePtr foundResource = findResourcePresence(pResource);

    if(foundResource != nullptr)
    {
        retState = foundResource->getResourceState();
    }
    return retState;
}

ResourcePresencePtr ResourceBroker::findResourcePresence(PrimitiveResourcePtr pResource)
{
    ResourcePresencePtr retResource(nullptr);

    if(s_presenceList->empty() != true)
    {
        for(auto & it : * s_presenceList)
        {
            PrimitiveResourcePtr temp = it->getPrimitiveResource();
            if(temp == pResource)
            {
                retResource = it;
                break;
            }
        }
    }

    return retResource;
}
