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
    pTimeoutCB = std::bind(&ResourcePresence::TimeOutCB, this,
            std::placeholders::_1);
    requesterList
    = std::unique_ptr<std::list<BrokerRequesterInfoPtr>>(new std::list<BrokerRequesterInfoPtr>);
    //TODO generate Timer(if(!isTimer))
    //register pTimeroutCB
    addBrokerRequesterCB(_cb);
    state = BROKER_STATE::REQUESTED;
    this->isWithinTime = true;
}

ResourcePresence::~ResourcePresence()
{
    requesterList->clear();

    state = BROKER_STATE::DESTROYED;
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
void ResourcePresence::executeAllBrokerCB()
{
    OC_LOG_V(DEBUG, BROKER_TAG, "executeAllBrokerCB()");
    for(BrokerRequesterInfoPtr & item : * requesterList)
    {
        item->brockerCB(this->state);
    }
}
void ResourcePresence::setResourcestate(BROKER_STATE _state)
{
    this->state = _state;

}
void ResourcePresence::TimeOutCB(int msg)
{
    this->isWithinTime = false;
    OC_LOG_V(DEBUG, BROKER_TAG, "Timeout execution. will be discard after receiving cb message");
    this->state = BROKER_STATE::LOST_SIGNAL;

}
void ResourcePresence::GetCB(const HeaderOptions &hos, const ResponseStatement& rep, int seq)
{
    try
    {
        //TODO : cancel timer if(isTimer)
        if(isWithinTime)
        {

            OC_LOG_V(DEBUG, BROKER_TAG, "broker state :: %d",(int)state);
            state = BROKER_STATE::ALIVE;
            OC_LOG_V(DEBUG, BROKER_TAG, "broker state changed :: %d",(int)state);
            OC_LOG_V(DEBUG, BROKER_TAG, "GET request was successful");

            if(!requesterList->empty())
            {
                executeAllBrokerCB();
            }
            else
            {
                OC_LOG_V(DEBUG, BROKER_TAG, "None exist resource for request");
                state = BROKER_STATE::DESTROYED;
            }
        }
        else
        {
            OC_LOG_V(DEBUG, BROKER_TAG, "its message is not available because of Timeout msg");
            OC_LOG_V(DEBUG, BROKER_TAG, "broker state :: %d",(int)state);
            state = BROKER_STATE::ALIVE;
            OC_LOG_V(DEBUG, BROKER_TAG, "broker state changed :: %d",(int)state);
            OC_LOG_V(DEBUG, BROKER_TAG, "GET request was successful");
            //notify cb message to user.
            if(!requesterList->empty())
            {
                executeAllBrokerCB();
            }
            else
            {
                OC_LOG_V(DEBUG, BROKER_TAG, "None exist resource for request");
                state = BROKER_STATE::DESTROYED;
            }
            isWithinTime = true;
        }
    }
    catch(std::exception& e)
    {

    }
}
/*
 * TODO : getCB logic is not fixed until now. below logic will be used forwarding task.
 *
 * //            if(eCode == OC_STACK_OK)
//            {
                OC_LOG_V(DEBUG, BROKER_TAG, "broker state :: %d",(int)state);
                state = BROKER_STATE::ALIVE;
                OC_LOG_V(DEBUG, BROKER_TAG, "broker state changed :: %d",(int)state);
                OC_LOG_V(DEBUG, BROKER_TAG, "GET request was successful");
//                std::cout << "[ResourcePresence]" << "Resource URI: " << rep.getUri() << std::endl;
//            }
//            else
//            {
//                std::cout << "[ResourcePresence]" << "onGET Response error: " << eCode << std::endl;
                if(!requesterList->empty())
                {
                    executeAllBrokerCB();
                }
                else
                {
                    OC_LOG_V(DEBUG, BROKER_TAG, "None exist resource for request");
                    state = BROKER_STATE::DESTROYED;
                }
//            }
 */

PrimitiveResourcePtr ResourcePresence::getPrimitiveResource()
{
    return primitiveResource;
}

BROKER_STATE ResourcePresence::getResourceState()
{
    return state;
}
