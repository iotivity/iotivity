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
#include "ExpiryTimerImpl.h"

ExpiryTimer::ExpiryTimer()
{
    timerPtr = ExpiryTimerImpl::getInstance();
}

ExpiryTimer::~ExpiryTimer()
{
    cancelAll();
}

void ExpiryTimer::cancelAll()
{
    for(auto id : m_timerIDList)
    {
        timerPtr->cancel(id);
    }
    m_timerIDList.clear();
}

ExpiryTimer::Id ExpiryTimer::postTimer(DelayInMilliSec milliSec, CB cb)
{
    Id retID = timerPtr->post(milliSec, std::move(cb));
    m_timerIDList.push_back(retID);

    return retID;
}

bool ExpiryTimer::cancelTimer(Id id)
{
    bool ret = timerPtr->cancel(id);
    m_timerIDList.remove(id);

    return ret;
}

void ExpiryTimer::destroyTimer()
{
}
