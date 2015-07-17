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

#ifndef _EXPIRY_TIMER_H_
#define _EXPIRY_TIMER_H_

#include <functional>
#include <list>

class ExpiryTimerImpl;

class ExpiryTimer
{
public:
    typedef unsigned int Id;
    typedef std::function<void(Id)> CB;
    typedef long long DelayInMilliSec;

public:
    ExpiryTimer();
    ~ExpiryTimer();

public:
    Id postTimer(DelayInMilliSec, CB); // will change name to post()
    bool cancelTimer(Id); // will change name to cancel()
    void destroyTimer(); // This function will be removed

private:
    void cancelAll();

private:
    std::list<Id> m_timerIDList;
    ExpiryTimerImpl* timerPtr;
};

#endif //_EXPIRY_TIMER_H_
