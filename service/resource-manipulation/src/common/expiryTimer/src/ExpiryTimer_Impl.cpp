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

#include "ExpiryTimer_Impl.h"

#include <unistd.h>
#include <algorithm>
#include <cstdlib>
#include <random>
#include <utility>

ExpiryTimer_Impl* ExpiryTimer_Impl::s_instance = nullptr;
std::once_flag* ExpiryTimer_Impl::mflag = new std::once_flag;

ExpiryTimer_Impl::ExpiryTimer_Impl()
{
    createChecker();
}

ExpiryTimer_Impl::~ExpiryTimer_Impl()
{
    check.detach();
    mflag = new std::once_flag;
    s_instance = nullptr;
}

void ExpiryTimer_Impl::destroyInstance()
{
    if(mTimerCBList.empty())
    {
        try
        {
            s_instance->~ExpiryTimer_Impl();
        }
        catch(std::exception &e)
        {
            std::cout << e.what();
        }
    }
}

ExpiryTimer_Impl* ExpiryTimer_Impl::getInstance()
{
    std::call_once((*mflag), [](){ s_instance = new ExpiryTimer_Impl(); });
    return s_instance;
}

ExpiryTimer_Impl::Id ExpiryTimer_Impl::postTimer(DelayMilliSec msec, TimerCb cb)
{
    Id retID;
    retID = generateID();

    milliSeconds delay(msec);
    insertTimerCBInfo(countExpireTime(delay), cb, retID);

    return retID;
}

bool ExpiryTimer_Impl::cancelTimer(Id timerID)
{
    bool ret = false;
    std::lock_guard<std::mutex> lockf(m_mutex);
    for(auto it: mTimerCBList)
    {
        if(it.second.m_id == timerID)
        {
            if(mTimerCBList.erase(it.first)!=0)
                ret = true;
            else
                ret = false;
        }
    }
    return ret;
}

void ExpiryTimer_Impl::insertTimerCBInfo(ExpiredTime msec, TimerCb cb, Id timerID)
{
    TimerCBInfo newInfo = {timerID, cb};
    std::lock_guard<std::mutex> lockf(m_mutex);
    mTimerCBList.insert(std::multimap<ExpiredTime, TimerCBInfo>::value_type(msec, newInfo));
    m_cond.notify_all();
}

ExpiryTimer_Impl::ExpiredTime ExpiryTimer_Impl::countExpireTime(milliDelayTime msec)
{
    auto now = std::chrono::system_clock::now();
    milliSeconds ret = std::chrono::duration_cast<milliSeconds>(now.time_since_epoch()) + msec;

    return ret;
}

ExpiryTimer_Impl::Id ExpiryTimer_Impl::generateID()
{
    std::srand((unsigned)std::time(NULL));
    Id retID = std::rand();

    for(std::multimap<ExpiredTime, TimerCBInfo>::iterator it=mTimerCBList.begin(); it!=mTimerCBList.end(); )
     {
       if((*it).second.m_id == retID || retID == 0)
        {
            retID = std::rand();
            it = mTimerCBList.begin();
        }
       else
       {
           ++it;
       }
     }
    return retID;
}

void ExpiryTimer_Impl::createChecker()
{
    check = std::thread(&ExpiryTimer_Impl::doChecker, this);
}

void ExpiryTimer_Impl::doChecker()
{
    while(true)
    {
        std::unique_lock<std::mutex> ul(cond_mutex);

        if(mTimerCBList.empty())
        {
            m_cond.wait_for(ul, std::chrono::seconds(CHECKER_WAIT_TIME));
        }
        else
        {
            ExpiredTime expireTime;
            expireTime = mTimerCBList.begin()->first;

            auto now = std::chrono::system_clock::now();
            milliSeconds waitTime = expireTime - std::chrono::duration_cast<milliSeconds>(now.time_since_epoch());
            m_cond.wait_for(ul, waitTime);

            auto callTime = std::chrono::system_clock::now();
            doExecutor(std::chrono::duration_cast<milliSeconds>(callTime.time_since_epoch()));
        }
    }
}

void ExpiryTimer_Impl::doExecutor(ExpiredTime expireTime)
{
    std::lock_guard<std::mutex> lockf(m_mutex);
    for(auto it: mTimerCBList)
    {
        if(it.first <= expireTime)
        {
            new ExecutorThread(it.second);
            mTimerCBList.erase(mTimerCBList.begin());
        }
        else
            break;
    }
}

// ExecutorThread Class
ExpiryTimer_Impl::ExecutorThread::ExecutorThread(TimerCBInfo cbInfo)
{
    execute = std::thread(&ExpiryTimer_Impl::ExecutorThread::executorFunc, this, cbInfo);
}

ExpiryTimer_Impl::ExecutorThread::~ExecutorThread()
{
    execute.detach();
}

void ExpiryTimer_Impl::ExecutorThread::executorFunc(TimerCBInfo cbInfo)
{
    cbInfo.m_cb(cbInfo.m_id);
}
