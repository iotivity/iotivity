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

#include <memory>
#include <cstdlib>
#include <functional>
#include <map>
#include <utility>
#include <ctime>

#include "DataCache.h"

#include "ResponseStatement.h"
#include "RCSResourceAttributes.h"
#include "ExpiryTimer.h"

namespace OIC
{
    namespace Service
    {
        DataCache::DataCache()
        {
            subscriberList = std::unique_ptr<SubscriberInfo>(new SubscriberInfo());

            sResource = nullptr;

            state = CACHE_STATE::READY_YET;

            networkTimeOutHandle = 0;
            pollingHandle = 0;

            pObserveCB = (ObserveCB)(std::bind(&DataCache::onObserve, this,
                                               std::placeholders::_1, std::placeholders::_2,
                                               std::placeholders::_3, std::placeholders::_4));
            pGetCB = (GetCB)(std::bind(&DataCache::onGet, this,
                                       std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
            pTimerCB = (TimerCB)(std::bind(&DataCache::onTimeOut, this, std::placeholders::_1));
            pPollingCB = (TimerCB)(std::bind(&DataCache::onPollingOut, this, std::placeholders::_1));

        }

        DataCache::~DataCache()
        {
            state = CACHE_STATE::DESTROYED;

            sResource->cancelObserve();
            if (subscriberList != nullptr)
            {
                subscriberList->clear();
                subscriberList.release();
            }
        }

        void DataCache::initializeDataCache(PrimitiveResourcePtr pResource)
        {
            sResource = pResource;

            sResource->requestGet(pGetCB);
            if (sResource->isObservable())
            {
                sResource->requestObserve(pObserveCB);
            }
            networkTimeOutHandle = networkTimer.postTimer(CACHE_DEFAULT_EXPIRED_MILLITIME, pTimerCB);
        }

        CacheID DataCache::addSubscriber(CacheCB func, REPORT_FREQUENCY rf, long repeatTime)
        {
            Report_Info newItem;
            newItem.rf = rf;
            newItem.repeatTime = repeatTime;
            newItem.timerID = 0;

            newItem.reportID = generateCacheID();

            if (subscriberList != nullptr)
            {
                subscriberList->insert(
                    std::make_pair(newItem.reportID, std::make_pair(newItem, func)));
            }

            return newItem.reportID;
        }

        CacheID DataCache::deleteSubscriber(CacheID id)
        {
            CacheID ret = 0;

            SubscriberInfoPair pair = findSubscriber(id);
            if (pair.first != 0)
            {
                ret = pair.first;
                subscriberList->erase(pair.first);
            }

            return ret;
        }

        SubscriberInfoPair DataCache::findSubscriber(CacheID id)
        {
            SubscriberInfoPair ret;

            for (auto &i : *subscriberList)
            {
                if (i.first == id)
                {
                    ret = std::make_pair(i.first, std::make_pair((Report_Info)i.second.first,
                                         (CacheCB)i.second.second));
                    break;
                }
            }

            return ret;
        }

        const PrimitiveResourcePtr DataCache::getPrimitiveResource() const
        {
            return (sResource != nullptr) ? sResource : nullptr;
        }

        const RCSResourceAttributes DataCache::getCachedData() const
        {
            if (state != CACHE_STATE::READY || attributes.empty())
            {
                return RCSResourceAttributes();
            }
            const RCSResourceAttributes retAtt = attributes;
            return retAtt;
        }

        void DataCache::onObserve(
            const HeaderOptions &_hos, const ResponseStatement &_rep, int _result, int _seq)
        {

            if (_result != OC_STACK_OK || _rep.getAttributes().empty())
            {
                return;
            }

            if (state != CACHE_STATE::READY)
            {
                state = CACHE_STATE::READY;
            }

            networkTimer.cancelTimer(networkTimeOutHandle);
            networkTimeOutHandle = networkTimer.postTimer(CACHE_DEFAULT_EXPIRED_MILLITIME, pTimerCB);

            notifyObservers(_rep.getAttributes());
        }

        void DataCache::onGet(const HeaderOptions &_hos,
                              const ResponseStatement &_rep, int _result)
        {
            if (_result != OC_STACK_OK || _rep.getAttributes().empty())
            {
                return;
            }

            if (state != CACHE_STATE::READY)
            {
                state = CACHE_STATE::READY;
            }

            if (!sResource->isObservable())
            {
                networkTimer.cancelTimer(networkTimeOutHandle);
                networkTimeOutHandle = networkTimer.postTimer(
                                           CACHE_DEFAULT_EXPIRED_MILLITIME, pTimerCB);

                pollingHandle = pollingTimer.postTimer(CACHE_DEFAULT_REPORT_MILLITIME, pPollingCB);
            }

            notifyObservers(_rep.getAttributes());
        }

        void DataCache::notifyObservers(RCSResourceAttributes Att)
        {
            if (attributes == Att)
            {
                return;
            }

            attributes = Att;

            RCSResourceAttributes retAtt = Att;
            for (auto &i : * subscriberList)
            {
                if (i.second.first.rf == REPORT_FREQUENCY::UPTODATE)
                {
                    i.second.second(this->sResource, retAtt);
                }
            }
        }

        CACHE_STATE DataCache::getCacheState() const
        {
            return state;
        }

        void DataCache::onTimeOut(unsigned int timerID)
        {
            state = CACHE_STATE::LOST_SIGNAL;
            return;
        }
        void DataCache::onPollingOut(const unsigned int timerID)
        {
            if (sResource != nullptr)
            {
                sResource->requestGet(pGetCB);
            }
            return;
        }

        CacheID DataCache::generateCacheID()
        {
            CacheID retID = 0;
            srand(time(NULL));

            while (1)
            {
                if (findSubscriber(retID).first == 0 && retID != 0)
                {
                    break;
                }
                retID = rand();
            }

            return retID;
        }

        void DataCache::requestGet()
        {
            state = CACHE_STATE::UPDATING;
            if (sResource != nullptr)
            {
                sResource->requestGet(pGetCB);
            }
        }

        bool DataCache::isEmptySubscriber() const
        {
            return (subscriberList != nullptr) ? subscriberList->empty() : true;
        }
    } // namespace Service
} // namespace OIC
