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

#ifndef LOGGER_H_
#define LOGGER_H_

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

namespace Intel {
namespace CCFL {

void logDebug(const char * tag, const char * format, ...);
void logInfo(const char * tag, const char * format, ...);
void logWarn(const char * tag, const char * format, ...);
void logError(const char * tag, const char * format, ...);
void logFatal(const char * tag, const char * format, ...);


//#ifndef NDEBUG
//#if 0
//	#ifdef __linux__
//		#include <stdio.h>
//		#define LOG(...) printf(__VA_ARGS__); printf("\n");
//	#elif __ANDROID__
//		#include <android/log.h>
//		#define LOG(...) _android_log_print(ANDROID_LOG_INFO, "LOG_TAG", __VA_ARGS__);
//	#elif _WIN64
//		#define LOG(...)
//	#elif _WIN32
//		#define LOG(...)
//	#elif __APPLE__
//		#define LOG(...)
//	#else
//	#endif
//#else
//	#define LOG(...)
//#endif

}
}

#endif /* LOGGER_H_ */
