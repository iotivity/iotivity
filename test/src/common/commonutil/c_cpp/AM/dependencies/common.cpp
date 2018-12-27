/******************************************************************
*
* Copyright 2018 Open Connectivity Foundation All Rights Reserved.
*
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

#include "common.h"

#include "ocstack.h"
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#define TAG "sample-common"

const char *getResult(OCStackResult result) {
    switch (result) {
    case OC_STACK_OK:
        return "OC_STACK_OK";
    case OC_STACK_RESOURCE_CREATED:
        return "OC_STACK_RESOURCE_CREATED";
    case OC_STACK_RESOURCE_DELETED:
        return "OC_STACK_RESOURCE_DELETED";
    case OC_STACK_INVALID_URI:
        return "OC_STACK_INVALID_URI";
    case OC_STACK_INVALID_QUERY:
        return "OC_STACK_INVALID_QUERY";
    case OC_STACK_INVALID_IP:
        return "OC_STACK_INVALID_IP";
    case OC_STACK_INVALID_PORT:
        return "OC_STACK_INVALID_PORT";
    case OC_STACK_INVALID_CALLBACK:
        return "OC_STACK_INVALID_CALLBACK";
    case OC_STACK_INVALID_METHOD:
        return "OC_STACK_INVALID_METHOD";
    case OC_STACK_NO_MEMORY:
        return "OC_STACK_NO_MEMORY";
    case OC_STACK_COMM_ERROR:
        return "OC_STACK_COMM_ERROR";
    case OC_STACK_INVALID_PARAM:
        return "OC_STACK_INVALID_PARAM";
    case OC_STACK_NOTIMPL:
        return "OC_STACK_NOTIMPL";
    case OC_STACK_NO_RESOURCE:
        return "OC_STACK_NO_RESOURCE";
    case OC_STACK_RESOURCE_ERROR:
        return "OC_STACK_RESOURCE_ERROR";
    case OC_STACK_SLOW_RESOURCE:
        return "OC_STACK_SLOW_RESOURCE";
    case OC_STACK_NO_OBSERVERS:
        return "OC_STACK_NO_OBSERVERS";
    case OC_STACK_UNAUTHORIZED_REQ:
        return "OC_STACK_UNAUTHORIZED_REQ";
    #ifdef WITH_PRESENCE
    case OC_STACK_PRESENCE_STOPPED:
        return "OC_STACK_PRESENCE_STOPPED";
    #endif
    case OC_STACK_ERROR:
        return "OC_STACK_ERROR";
    default:
        return "UNKNOWN";
    }
}

void StripNewLineChar(char* str) {
    int i = 0;
    if (str)
    {
        while( str[i])
        {
            if (str[i] == '\n')
                str[i] = '\0';
            i++;
        }
    }
}

time_t _user_set_time = 0;

void getCurrentTime(char * buf) {
    time_t now;
    time(&now);
    now -= _user_set_time;
    struct tm* tm_info;
    tm_info = localtime(&now);
    strftime(buf, 25, "%Y-%m-%dT%H:%M:%S%z", tm_info);
}

void setUserTime(char * buf) {
    time_t now;
    time(&now);

    struct tm _user_time = {0};
    int year, month, day, hour, min, sec, timezone = 0;
    sscanf(buf, "%04d-%02d-%02dT%02d:%02d:%02d+%04d",
        &year, &month, &day, &hour, &min, &sec, &timezone);
    _user_time.tm_year = year - 1900;
    _user_time.tm_mon = month -1;
    _user_time.tm_mday = day;
    _user_time.tm_hour = hour;
    _user_time.tm_min = min;
    _user_time.tm_sec = sec;

    time_t user_time = mktime(&_user_time);

    _user_set_time = now - user_time;
}