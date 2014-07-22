//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
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
