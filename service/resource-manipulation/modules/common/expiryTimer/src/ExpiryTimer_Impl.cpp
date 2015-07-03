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
#include <utility>


ExpiryTimer_Impl* ExpiryTimer_Impl::s_instance = nullptr;
std::mutex ExpiryTimer_Impl::s_mutexForCreation;
bool ExpiryTimer_Impl::isDestroyed = false;

ExpiryTimer_Impl::ExpiryTimer_Impl()
{
    threadNum = 0;
    checkThreadRun = false;
}

ExpiryTimer_Impl::~ExpiryTimer_Impl()
{
    isDestroyed = true;
    int status;

    pthread_join(checker_th, (void **)&status);
    pthread_detach(checker_th);
}

void ExpiryTimer_Impl::killTimer()
{
    s_instance->~ExpiryTimer_Impl();
}

ExpiryTimer_Impl* ExpiryTimer_Impl::getInstance()
{
    if(isDestroyed)
    {
        new(s_instance) ExpiryTimer_Impl;
        atexit(killTimer);
        isDestroyed = false;
    }
    else if(s_instance == nullptr)
    {
        static ExpiryTimer_Impl tmp_instance;
        s_instance = &tmp_instance;
    }
    return s_instance;
}

TimerID ExpiryTimer_Impl::requestTimer(long long sec, TimerCB cb)
{
    if(threadNum < EXPIRY_THREAD_LIST)
    {
        unsigned int timerID = generateTimerID();
        ExpiryTimer_Impl::getInstance()->registerCBTimer(sec, cb, timerID);
        return timerID;
    }
    else
        return OVERFLOW_THREAD_NUM;
}

void ExpiryTimer_Impl::cancelTimer(TimerID timerID)
{
    for( auto it : mTimerCBList)
    {
        if(it.second.m_id == timerID)
        {
            mTimerCBList.erase(it.first);
            timerIDList.remove(it.second.m_id);
        }
    }
}

void ExpiryTimer_Impl::registerCBTimer(long long countSEC, TimerCB _cb, TimerID id)
{
    timerCBInfo newInfo = {id, _cb};
    mTimerCBList.insert(multimap<long long, ExpiryTimer_Impl::timerCBInfo>::value_type(countSEC, newInfo));

    if (checkThreadRun == false)
    {
        initThCheck();
    }
}

void ExpiryTimer_Impl::checkTimeOut()
{
    while (1)
    {
        if(mTimerCBList.empty())
        {
            checkThreadRun = false;
            break;
        }
       else
        {
           long long curSEC = getSeconds(0);
           long long expireTime;
           expireTime = mTimerCBList.begin()->first;

           if(curSEC >= expireTime)
           {
               initThExecutor(mTimerCBList.begin()->second);
               mTimerCBList.erase(mTimerCBList.begin());
           }
        }
       usleep(SLEEP_TIME);
    }
}

void* ExpiryTimer_Impl::threadChecker(void * msg)
{
    if(s_instance != nullptr)
        s_instance->checkTimeOut();
    return NULL;
}

void ExpiryTimer_Impl::initThCheck()
{
    int retThreadCreation;

    retThreadCreation = pthread_create(&checker_th, NULL, s_instance->threadChecker, NULL);
    if (retThreadCreation != 0)
    {
        return;
    }
    else
    {
        checkThreadRun = true;
    }
}

void *ExpiryTimer_Impl::threadExecutor(void * msg)
{
    TimerCB cb;
    timerCBInfo *curCBInfo;
    curCBInfo= (timerCBInfo *) msg;

    cb = curCBInfo->m_pCB;
    cb(curCBInfo->m_id);

    return NULL;
}

void ExpiryTimer_Impl::initThExecutor(timerCBInfo cbInfo)
{

    int retThreadCreation;
    int status;
    pthread_t executor_th;

    retThreadCreation = pthread_create(&executor_th, NULL, ExpiryTimer_Impl::threadExecutor, (void *)&cbInfo);
    threadNum++;

    if (retThreadCreation != 0)
    {
        return;
    }
    else
    {
        pthread_join(executor_th, (void **)&status);
        pthread_detach(executor_th);
        threadNum--;
    }
}

TimerID ExpiryTimer_Impl::generateTimerID()
{
    srand(time(NULL));
    unsigned int retID = rand();

    for(auto it : timerIDList)
     {
       if(it == retID || retID == 0)
        {
            retID = rand();
            it = s_instance->timerIDList.front();
        }
     }
    timerIDList.push_back(retID);

    return retID;
}

long long ExpiryTimer_Impl::getSeconds(long long sec)
{
    time_t curSEC;
    time(&curSEC);
    long long retSEC = curSEC + sec;
    return retSEC;
}
