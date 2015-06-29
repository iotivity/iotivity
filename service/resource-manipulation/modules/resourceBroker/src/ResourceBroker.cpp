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

#include <time.h>

#include "BrokerTypes.h"
#include "ResourceBroker.h"

ResourceBroker * ResourceBroker::s_instance = NULL;
std::mutex ResourceBroker::s_mutexForCreation;
std::unique_ptr<PresenceList>  ResourceBroker::s_presenceList(nullptr);
std::unique_ptr<BrokerIDMap> ResourceBroker::s_brokerIDMap(nullptr);

ResourceBroker::ResourceBroker()
{
    if(s_presenceList == nullptr)
    {
        s_presenceList = std::unique_ptr<PresenceList>(new PresenceList);
    }
    if(s_brokerIDMap == nullptr)
    {
        s_brokerIDMap = std::unique_ptr<BrokerIDMap>(new BrokerIDMap);
    }
}

ResourceBroker::~ResourceBroker()
{
    if(s_presenceList != nullptr)
    {
        s_presenceList->erase(s_presenceList->begin(), s_presenceList->end());
        s_presenceList->clear();
    }
    if(s_brokerIDMap != nullptr)
    {
        s_brokerIDMap->erase(s_brokerIDMap->begin(), s_brokerIDMap->end());
        s_brokerIDMap->clear();
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

BrokerID ResourceBroker::hostResource(PrimitiveResourcePtr pResource, BrokerCB cb)
{
    BrokerID retID = 0;

    if (pResource->getUri().empty() || pResource->getHost().empty())
    {
        return retID;
    }
    if (cb == NULL)
    {
        return retID;
    }

    retID = generateBrokerID();

    ResourcePresencePtr presenceItem = findResourcePresence(pResource);
    if(presenceItem == nullptr)
    {
        OC_LOG_V(DEBUG, BROKER_TAG, "Not found any Handled Resource.");
        OC_LOG_V(DEBUG, BROKER_TAG, "Create New Resource Presence Handler.");

        presenceItem = ResourcePresencePtr(new ResourcePresence(pResource));
        s_presenceList->push_back(presenceItem);
    }
    presenceItem->addBrokerRequester(retID, cb);

    BrokerCBResourcePair pair(presenceItem, cb);
    s_brokerIDMap->insert(std::pair<BrokerID, BrokerCBResourcePair>
        (retID, BrokerCBResourcePair(presenceItem, cb)));

    return retID;
}

BrokerID ResourceBroker::cancelHostResource(BrokerID brokerId)
{
    BrokerIDMap::iterator it = s_brokerIDMap->find(brokerId);
    if(brokerId == 0)
    {
        // input parameter is wrong.
        // hostResource never return value 0;
        return brokerId;
    }
    else if(it == s_brokerIDMap->end())
    {
        // not found requested brokerId in BrokerMap;
        return brokerId;
    }
    else
    {
        ResourcePresencePtr presenceItem = it->second.pResource;
        presenceItem->removeBrokerRequester(brokerId);
        s_brokerIDMap->erase(brokerId);

        if(presenceItem->isEmptyRequester())
        {
            auto iter = std::find(s_presenceList->begin(), s_presenceList->end(), presenceItem);
            s_presenceList->erase(iter);
            presenceItem.reset();
        }
    }

    return brokerId;
}

BROKER_STATE ResourceBroker::getResourceState(BrokerID brokerId)
{
    BROKER_STATE retState = BROKER_STATE::NONE;

    BrokerIDMap::iterator it = s_brokerIDMap->find(brokerId);
    if(it != s_brokerIDMap->end())
    {
        ResourcePresencePtr foundResource = it->second.pResource;
        retState = foundResource->getResourceState();
    }

    return retState;
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

BrokerID ResourceBroker::generateBrokerID()
{
    BrokerID retID = 0;
    srand(time(NULL));

    while(1)
    {
        if(retID == 0 || s_brokerIDMap->find(retID) != s_brokerIDMap->end())
        {
            retID = (unsigned int)rand();
        }
        else
        {
            break;
        }
    }
    return retID;
}
