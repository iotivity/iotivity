//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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

#ifndef __OC_ACTIONSET__
#define __OC_ACTIONSET__

#include <string>
#include <vector>
#include <cstdio>
#include <iostream>

#include <ctime>

#include <timer.h>

using namespace std;

namespace OIC
{
enum ACTIONSET_TYPE
{
    NONE = 0, SCHEDULED, RECURSIVE
};

typedef tm OCTime;

class Time
{
public:
    long int mDelay;
    OCTime mTime;
    ACTIONSET_TYPE type;

    Time();
    ~Time();

    void setTime(OCTime t);
    void setTime(unsigned int yy, unsigned int mm, unsigned int dd,
            unsigned int h, unsigned int m, unsigned int s,
            int dayoftheweek);
    void setDayOfWeekForRecursive(int day);
    unsigned int getYear();
    unsigned int getMonth();
    unsigned int getDay();
    unsigned int getHour();
    unsigned int getMin();
    unsigned int getSec();
    long int getSecondsFromAbsoluteTime();
    long int getSecAbsTime();
    long int getSecondsForWeeklySchedule();
    void setDelay(long int seconds);
    std::string toString() const;
};

class Capability
{
public:
    std::string capability;
    std::string status;
};

class Action
{
public:
    Action();
    ~Action();

    std::string target;
    std::vector<Capability*> listOfCapability;
};

class ActionSet: public Time
{
public:
    /**
     * Constructor for ActionSet
     */
    ActionSet();
    /**
     * Virtual destructor for ActionSet
     */
    ~ActionSet();

    std::string actionsetName;
    std::vector<Action*> listOfAction;
};
}
#endif
