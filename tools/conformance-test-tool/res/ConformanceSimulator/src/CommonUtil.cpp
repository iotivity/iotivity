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

#include "CommonUtil.h"

long CommonUtil::s_setUpDynamicMemoryUsage = 0;
long CommonUtil::s_tearDownDynamicMemoryUsage = 0;
long CommonUtil::s_memoryDiffSum = 0;

long CommonUtil::getCurrentAllocatedMemory()
{
    return mallinfo().uordblks;
}

void CommonUtil::launchApp(std::string app)
{
#ifdef __LINUX__
    std::string cmd = "";
//    cmd += "nohup gnome-terminal -x sh -c ";
    cmd += app;
    cmd += " &";
    system(cmd.c_str());
#endif
}

void CommonUtil::killApp(std::string app)
{
    std::string appName = app.substr(app.find_last_of("/\\") + 1);
#ifdef __LINUX__
    std::string prefix = "kill -9 $(pgrep ";
    std::string postfix = ")";
    std::string command = prefix + appName + postfix;
    system(command.c_str());

    command = "killall " + appName;
    system(command.c_str());

    command = "pkill -9 " + appName;
    system(command.c_str());

#endif
}

void CommonUtil::waitInSecond(unsigned int seconds)
{
#ifdef __LINUX__
    sleep(seconds);
#endif

#ifdef __WINDOWS__
    sleep(seconds * 1000);
#endif
#ifdef __TIZEN__
    sleep(seconds);
#endif
}

void CommonUtil::getCurrentTime(struct tm &currentTime)
{
    time_t t = time(0); // get time now
    struct tm *now = localtime(&t);

    currentTime.tm_isdst = 0;
    currentTime.tm_year = now->tm_year + 1900;
    currentTime.tm_mon = now->tm_mon + 1;
    currentTime.tm_mday = now->tm_mday;
    currentTime.tm_hour = now->tm_hour;
    currentTime.tm_min = now->tm_min;
    currentTime.tm_sec = now->tm_sec;

    std::cout << "Date = " << currentTime.tm_year << "-" << currentTime.tm_mon << "-"
            << currentTime.tm_mday << std::endl;
    std::cout << "Time = " << currentTime.tm_hour << ":" << currentTime.tm_min << ":"
            << currentTime.tm_sec << std::endl;
}

void CommonUtil::duplicateString(char ** targetString, string sourceString)
{
    *targetString = new char[sourceString.length() + 1];
    strncpy(*targetString, sourceString.c_str(), (sourceString.length() + 1));
}

