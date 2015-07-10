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

#ifndef _EXPIRY_TIMER_Impl_H_
#define _EXPIRY_TIMER_Impl_H_

// CHECKER_WAIT_TIME : checker thread waits new request for 10 seconds
#define CHECKER_WAIT_TIME 10

#include <iostream>
#include <functional>
#include <map>
#include <mutex>
#include <thread>
#include <chrono>
#include <condition_variable>

class ExpiryTimer_Impl
{
public:
    typedef unsigned int Id;
    typedef std::function<void*(Id)> TimerCb;

    typedef long long DelayMilliSec;
    typedef std::chrono::milliseconds milliSeconds;
    typedef std::chrono::duration<int64_t, std::milli> milliDelayTime;
    typedef std::chrono::duration<int64_t, std::milli> ExpiredTime;

private:
    struct TimerCBInfo
    {
        Id m_id;
        TimerCb m_cb;
    };

private:
   ExpiryTimer_Impl();
   ExpiryTimer_Impl(const ExpiryTimer_Impl&);
   ExpiryTimer_Impl& operator=(const ExpiryTimer_Impl&);
   ~ExpiryTimer_Impl();

public:
    static ExpiryTimer_Impl* getInstance();
    void destroyInstance();

    Id postTimer(DelayMilliSec, TimerCb);
    bool cancelTimer(Id);

private:
   Id generateID();

   void insertTimerCBInfo(ExpiredTime, TimerCb ,Id);
   ExpiredTime countExpireTime(milliSeconds);

   void createChecker();
   void doChecker();

   void doExecutor(ExpiredTime);

private:
   static ExpiryTimer_Impl* s_instance;
   static std::once_flag* mflag;

   std::multimap<ExpiredTime, TimerCBInfo> mTimerCBList;

   std::thread check;
   std::mutex m_mutex;
   std::mutex cond_mutex;
   std::condition_variable m_cond;

public:
   class ExecutorThread
   {
   public:
       ExecutorThread(TimerCBInfo);
       ~ExecutorThread();

   public:
       void executorFunc(TimerCBInfo);

   private:
       std::thread execute;
   };
};

#endif //_EXPIRY_TIMER_Impl_H_
