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

#ifndef _EXPIRY_TIMER_IMPL_H_
#define _EXPIRY_TIMER_IMPL_H_

#include <functional>
#include <map>
#include <mutex>
#include <thread>
#include <chrono>
#include <condition_variable>
#include <random>

class ExpiryTimerImpl
{
public:
    typedef unsigned int Id;
    typedef std::function<void(Id)> CB;

    typedef long long DelayInMilliSec;
    typedef std::chrono::milliseconds MilliSeconds;
    typedef std::chrono::duration<int64_t, std::milli> MilliDelayTime;
    typedef std::chrono::duration<int64_t, std::milli> ExpiredTime;

private:
    struct TimerCBInfo
    {
        Id m_id;
        CB m_cB;
    };

private:
   ExpiryTimerImpl();
   ExpiryTimerImpl(const ExpiryTimerImpl&) = delete;
   ExpiryTimerImpl& operator=(const ExpiryTimerImpl&) = delete;
   ~ExpiryTimerImpl();

public:
    static ExpiryTimerImpl* getInstance();
    void destroyInstance();

    Id post(DelayInMilliSec, CB);
    bool cancel(Id);

private:
   Id generateId();

   void insertTimerCBInfo(ExpiredTime, CB ,Id);
   ExpiredTime countExpireTime(MilliSeconds);

   void runChecker();

   void runExecutor(ExpiredTime);

private:
   static ExpiryTimerImpl* s_instance;
   static std::once_flag* s_flag;

   std::multimap<ExpiredTime, TimerCBInfo> m_timerCBList;

   std::thread m_checkerThread;
   std::mutex m_mutex;
   std::condition_variable m_cond;

   std::random_device m_device;
   std::default_random_engine m_engine;
   std::uniform_int_distribution<Id> m_dist;

public:
   class ExecutorThread
   {
   public:
       ExecutorThread(TimerCBInfo);
       ~ExecutorThread();

   public:
       void executorFunc(TimerCBInfo);

   private:
       std::thread m_executorThread;
   };
};

#endif //_EXPIRY_TIMER_IMPL_H_
