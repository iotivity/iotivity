//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
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


/*
 * SimpleLogger.cpp
 *
 *  Created on: Mar 27, 2014
 *      Author: dkhudson
 */
#include "SimpleLogger.h"

#ifdef __ANDROID__
  #include <android/log.h>
#endif

namespace Intel {
namespace CCFL {

void LOG(const char * format, ...) {
#ifdef __ANDROID__
  va_list args;
  va_start (args, format);
  __android_log_vprint(ANDROID_LOG_INFO, "CCF-Lite-ANDROID", format, args);
  va_end (args);
#elif defined __linux__
  printf("INFO: CCF-Lite-LINUX: ");
  va_list args;
  va_start (args, format);
  vprintf(format, args);
  va_end (args);
  printf("\n");
#endif
}

void logDebug(const char * tag, const char * format, ...) {
#ifdef __ANDROID__
    va_list args;
    va_start (args, format);
    __android_log_vprint(ANDROID_LOG_DEBUG, tag, format, args);
    va_end (args);
#elif defined __linux__
    printf("DEBUG: %s: ", tag);
    va_list args;
    va_start (args, format);
    vprintf(format, args);
    va_end (args);
    printf("\n");
#endif
}
void logInfo(const char * tag, const char * format, ...) {
#ifdef __ANDROID__
    va_list args;
    va_start (args, format);
    __android_log_vprint(ANDROID_LOG_INFO, tag, format, args);
    va_end (args);
#elif defined __linux__
    printf("INFO: %s: ", tag);
    va_list args;
    va_start (args, format);
    vprintf(format, args);
    va_end (args);
    printf("\n");
#endif
}

void logWarn(const char * tag, const char * format, ...) {
#ifdef __ANDROID__
    va_list args;
    va_start (args, format);
    __android_log_vprint(ANDROID_LOG_WARN, tag, format, args);
    va_end (args);
#elif defined __linux__
    printf("WARN: %s: ", tag);
    va_list args;
    va_start (args, format);
    vprintf(format, args);
    va_end (args);
    printf("\n");
#endif
}

void logError(const char * tag, const char * format, ...) {
#ifdef __ANDROID__
    va_list args;
    va_start (args, format);
    __android_log_vprint(ANDROID_LOG_ERROR, tag, format, args);
    va_end (args);
#elif defined __linux__
    printf("ERROR: %s: ", tag);
    va_list args;
    va_start (args, format);
    vprintf(format, args);
    va_end (args);
    printf("\n");
#endif
}

void logFatal(const char * tag, const char * format, ...) {
#ifdef __ANDROID__
    va_list args;
    va_start (args, format);
    __android_log_vprint(ANDROID_LOG_FATAL, tag, format, args);
    va_end (args);
#elif defined __linux__
    printf("FATAL: %s: ", tag);
    va_list args;
    va_start (args, format);
    vprintf(format, args);
    va_end (args);
    printf("\n");
#endif
}

}
}
