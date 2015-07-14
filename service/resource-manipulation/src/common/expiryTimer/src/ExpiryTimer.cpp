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
#include "ExpiryTimer_Impl.h"

ExpiryTimer_Impl* timerPtr;

ExpiryTimer::ExpiryTimer()
{
    timerPtr = ExpiryTimer_Impl::getInstance();
}

ExpiryTimer::~ExpiryTimer()
{
}

void ExpiryTimer::destroyTimer()
{
    for(auto it : mTimerIDList)
    {
        timerPtr->cancelTimer(it);
    }
    timerPtr->destroyInstance();
    mTimerIDList.clear();

    this->~ExpiryTimer();
}

ExpiryTimer::TimerID ExpiryTimer::postTimer(DelayMilliSec sec, TimerCB cb)
{
    TimerID retID = 0;

    retID = timerPtr->postTimer(sec, cb);
    mTimerIDList.push_back(retID);

    return retID;
}

bool ExpiryTimer::cancelTimer(TimerID timerID)
{
    bool ret = false;

    ret = timerPtr->cancelTimer(timerID);
    mTimerIDList.remove(timerID);

    return ret;
}
