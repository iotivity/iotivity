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

#include "ExpiryTimerImpl.h"

#include <unistd.h>
#include <cstdlib>
#include <utility>

ExpiryTimerImpl* ExpiryTimerImpl::s_instance = nullptr;
std::once_flag* ExpiryTimerImpl::s_flag = new std::once_flag;

ExpiryTimerImpl::ExpiryTimerImpl()
{
    m_engine = std::default_random_engine(m_device());
    m_checkerThread = std::thread(&ExpiryTimerImpl::runChecker, this);
}

ExpiryTimerImpl::~ExpiryTimerImpl()
{
    m_checkerThread.join();
}

ExpiryTimerImpl* ExpiryTimerImpl::getInstance()
{
    std::call_once(*s_flag, [](){ s_instance = new ExpiryTimerImpl(); });
    return s_instance;
}

ExpiryTimerImpl::Id ExpiryTimerImpl::post(DelayInMilliSec millisec, CB cb)
{
    Id retID = generateId();

    MilliSeconds delay(millisec);
    insertTimerCBInfo(countExpireTime(delay), cb, retID);

    return retID;
}

bool ExpiryTimerImpl::cancel(Id id)
{
    bool ret = false;
    std::lock_guard<std::mutex> lockf(m_mutex);
    for(auto it: m_timerCBList)
    {
        if(it.second.m_id == id)
        {
            if(m_timerCBList.erase(it.first)!=0)
                ret = true;
            else
                ret = false;
        }
    }
    return ret;
}

void ExpiryTimerImpl::insertTimerCBInfo(ExpiredTime msec, CB cb, Id id)
{
    TimerCBInfo newInfo{id, cb};
    std::lock_guard<std::mutex> lockf(m_mutex);
    m_timerCBList.insert({msec, newInfo});
    m_cond.notify_all();
}

ExpiryTimerImpl::ExpiredTime ExpiryTimerImpl::countExpireTime(MilliSeconds msec)
{
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<MilliSeconds>(now.time_since_epoch()) + msec;
}

ExpiryTimerImpl::Id ExpiryTimerImpl::generateId()
{
    Id retID = m_dist(m_device);

    for(auto it = m_timerCBList.begin(); it != m_timerCBList.end(); )
     {
       if(it->second.m_id == retID || retID == 0)
        {
            retID = m_dist(m_device);
            it = m_timerCBList.begin();
        }
       else
       {
           ++it;
       }
     }

    return retID;
}

void ExpiryTimerImpl::runChecker()
{
    while(true)
    {
        std::unique_lock<std::mutex> ul(m_mutex);

        if(m_timerCBList.empty())
        {
            m_cond.wait(ul);
        }
        else
        {
            ExpiredTime expireTime;
            expireTime = m_timerCBList.begin()->first;

            auto now = std::chrono::system_clock::now();
            MilliSeconds waitTime = expireTime - std::chrono::duration_cast<MilliSeconds>(now.time_since_epoch());
            m_cond.wait_for(ul, waitTime);

            auto callTime = std::chrono::system_clock::now();
            runExecutor(std::chrono::duration_cast<MilliSeconds>(callTime.time_since_epoch()));
        }
    }
}

void ExpiryTimerImpl::runExecutor(ExpiredTime expireTime)
{
    for(auto it = m_timerCBList.begin(); it != m_timerCBList.end(); ++it)
    {
        if(it->first <= expireTime)
        {
            ExecutorThread executor(it->second);
            m_timerCBList.erase(it);
        }
        else
        {
            break;
        }
    }
}

// ExecutorThread Class
ExpiryTimerImpl::ExecutorThread::ExecutorThread(TimerCBInfo cbInfo)
{
    m_executorThread = std::thread(&ExpiryTimerImpl::ExecutorThread::executorFunc, this, cbInfo);
}

ExpiryTimerImpl::ExecutorThread::~ExecutorThread()
{
    m_executorThread.detach();
}

void ExpiryTimerImpl::ExecutorThread::executorFunc(TimerCBInfo cbInfo)
{
    cbInfo.m_cB(cbInfo.m_id);
}
