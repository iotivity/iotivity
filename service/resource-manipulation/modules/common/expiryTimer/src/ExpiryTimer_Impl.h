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

#include <mutex>
#include <pthread.h>
#include <functional>
#include <list>
#include <map>
#include <iostream>
#include <new>

#define EXPIRY_THREAD_LIST 50
#define OVERFLOW_THREAD_NUM -1
// Current checker thread design have to get checking interval.
// SLEEP_TIME value will be removed later.
#define SLEEP_TIME 50000

using namespace std;

typedef unsigned int TimerID;
typedef function<void*(TimerID)> TimerCB;

class ExpiryTimer_Impl
{
private:
   struct timerCBInfo
    {
       TimerID m_id;
       TimerCB m_pCB;
    };

    ExpiryTimer_Impl();
    ExpiryTimer_Impl(const ExpiryTimer_Impl & other);
    ~ExpiryTimer_Impl();

public:

     static ExpiryTimer_Impl * getInstance();

    TimerID requestTimer(long long sec, TimerCB);
    void cancelTimer(TimerID timerID);

private:

   static void killTimer();
   static void *threadExecutor(void * msg);
   static void *threadChecker(void * msg);

   void registerCBTimer(long long countSEC, TimerCB _cb, TimerID id);
   void initThCheck();
   void initThExecutor(timerCBInfo cbInfo);
   void checkTimeOut();

   TimerID generateTimerID();
   long long getSeconds(long long sec);

public:

    list<TimerID> timerIDList;

private:

    static ExpiryTimer_Impl * s_instance;
    static mutex s_mutexForCreation;
    static bool isDestroyed;

    multimap<long long, timerCBInfo> mTimerCBList;
    bool checkThreadRun;
    list<int> mExecutorIndexList;
    int threadNum;

    pthread_t checker_th;
    pthread_mutex_t checker_mutex;
};
#endif //_EXPIRY_TIMER_Impl_H_
