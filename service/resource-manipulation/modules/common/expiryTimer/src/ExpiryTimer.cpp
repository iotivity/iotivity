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

#include "ExpiryTimer.h"

#include <ctime>

ExpiryTimer::ExpiryTimer()
{
    timerPtr = ExpiryTimer_Impl::getInstance();
}

ExpiryTimer::~ExpiryTimer()
{
    // clear ExpiryTimer id
    for(auto it : mTimerIDList)
    {
        timerPtr->cancelTimer(it);
    }
    mTimerIDList.clear();
}

TimerID ExpiryTimer::requestTimer(long long sec, TimerCB cb)
{
    TimerID retID;
    long long expiredTime;

    expiredTime = countExpiredTime(sec);
    retID = timerPtr->requestTimer(expiredTime, cb);

    mTimerIDList.push_back(retID);

    return retID;
}

void ExpiryTimer::cancelTimer(TimerID timerID)
{
    timerPtr->cancelTimer(timerID);
    mTimerIDList.remove(timerID);
}

long long ExpiryTimer::countExpiredTime(long long sec)
{
    time_t curSEC;
    time(&curSEC);
    return curSEC + sec;
}
