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
#include <iostream>
#include <list>

class ExpiryTimer
{
public:
    typedef unsigned int TimerID;
    typedef std::function<void*(TimerID)> TimerCB;
    typedef long long DelayMilliSec;

public:
    ExpiryTimer();

private:
    ~ExpiryTimer();

public:
    TimerID postTimer(DelayMilliSec sec, TimerCB);
    bool cancelTimer(TimerID timerID);
    void destroyTimer();

private:
    std::list<TimerID> mTimerIDList;
};

#endif //_TIMER_H_
