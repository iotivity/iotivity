//******************************************************************
//
// Copyright 2015 Intel Mobile Communications GmbH All Rights Reserved.
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

// Defining _POSIX_C_SOURCE macro with 199309L (or greater) as value
// causes header files to expose definitions
// corresponding to the POSIX.1b, Real-time extensions
// (IEEE Std 1003.1b-1993) specification
//
// For this specific file, see use of clock_gettime,
// Refer to http://pubs.opengroup.org/stage7tc1/functions/clock_gettime.html
// and to http://man7.org/linux/man-pages/man2/clock_gettime.2.html
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include "oic_time.h"
#include <stdio.h>

#ifndef WITH_ARDUINO
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#endif

#define TAG "OIC_TIME"

uint64_t OICGetCurrentTime(OICTimePrecision precision)
{
    uint64_t currentTime = 0;
    int err = 0;

#ifdef WITH_ARDUINO
    currentTime = (TIME_IN_MS == precision) ? (uint64_t)millis() : (uint64_t)micros();
#else
    #if _POSIX_TIMERS > 0
        int32_t clockId = CLOCK_REALTIME;
        static int32_t cachedRet = 0;

        #if defined(CLOCK_MONOTONIC_COARSE)
            clockId = CLOCK_MONOTONIC_COARSE;
        #elif _POSIX_MONOTONIC_CLOCK >= 0
            //Option _POSIX_MONOTONIC_CLOCK == 0 indicates that the option is
            //available at compile time but may not be supported at run time.
            //Checking if option _POSIX_MONOTONIC_CLOCK  is supported at run time.
            #if _POSIX_MONOTONIC_CLOCK == 0
                cachedRet = (0 == cachedRet) ? sysconf(_SC_MONOTONIC_CLOCK) : cachedRet;
                if(cachedRet > 0)
                {
                    clockId = CLOCK_MONOTONIC;
                }
            #else
                clockId = CLOCK_MONOTONIC;
            #endif
        #else
            clockId = CLOCK_REALTIME;
        #endif

        struct timespec current = {.tv_sec=0, .tv_nsec=0};
        if((err = clock_gettime(clockId, &current)) != -1)
        {
            currentTime = (TIME_IN_MS == precision) ?
                          (((uint64_t)current.tv_sec * MS_PER_SEC) + (current.tv_nsec / NS_PER_MS)):
                          (((uint64_t)current.tv_sec * US_PER_SEC) + (current.tv_nsec / NS_PER_US));
        }
    #else
        struct timeval current = {.tv_sec=0, .tv_usec=0};
        if((err = gettimeofday(&current, NULL)) != -1)
        {
            currentTime = (TIME_IN_MS == precision) ?
                          (((uint64_t)current.tv_sec * MS_PER_SEC) + (current.tv_usec / US_PER_MS)):
                          (((uint64_t)current.tv_sec * US_PER_SEC) + (current.tv_usec));
        }
    #endif
#endif
    return (!err) ? currentTime : 0;
}

