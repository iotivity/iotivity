/******************************************************************
*
* Copyright 2016 Samsung Electronics All Rights Reserved.
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

#ifndef INCLUDE_IOTIVITYTEST_LOGGER_H_
#define INCLUDE_IOTIVITYTEST_LOGGER_H_

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "CommonUtil.h"

//if ISLOG is 0, log will be not printed
#ifndef ISLOG
#define ISLOG 1
#endif

#ifndef LOG_LEVEL
#define LOG_LEVEL 0
#endif

// Max buffer size used in variable argument log function
#define MAX_LOG_V_BUFFER_SIZE (2048)

// Log levels
typedef enum {
    DEBUG = 0,
    INFO,
    WARNING,
    ERROR,
    FATAL
} LogLevel;
static const char * LEVEL[] __attribute__ ((unused)) = {"DEBUG", "INFO", "WARNING", "ERROR", "FATAL"};
static const char * envLogWriteFile = "NOLOGFILE";

#define __DEFAULT_LOG__

#if defined(__ANDROID__)

#ifndef __CA__

#undef __DEFAULT_LOG__

#define IOTIVITYTEST_LOG(strLog) do{ \
		__android_log_print(LEVEL[level], "", "%s:%d %s", __PRETTY_FUNCTION__, __LINE__, strLog); \
	}while(0)
#endif
#endif


#ifdef __DEFAULT_LOG__

static FILE* g_logfile_out = NULL;
#define IOTIVITYTEST_LOG(level,format,...) do{ \
		if(level!=DEBUG||ISLOG){ \
			if(level >= LOG_LEVEL && LOG_LEVEL>-1) { \
				char buffer[MAX_LOG_V_BUFFER_SIZE]; \
				sprintf(buffer,"[%s][%s][%s:%d]:",CommonUtil::GetTimeStampString(), LEVEL[level], __FILE__,__LINE__); \
				vPrintf(buffer,format,##__VA_ARGS__); \
			}} \
}while(0)

static std::string splitFilename (const std::string path, std::string &appName)
{
    unsigned found = path.find_last_of("/\\");
    appName = path.substr(found+1);

    return path.substr(0,found);
}

static FILE* _initLogFile()
{
	if (g_logfile_out == NULL) {
			const char* timestamp = CommonUtil::GetTimeStampString();
			std::string dirName = "log";
			std::string filename = "";

#ifdef __LINUX__
			std::string appName;
			std::string fullpath = splitFilename(std::string(getenv("_")), appName);
			CommonUtil::mkDir(fullpath + "/" + dirName);

			filename = (fullpath + "/" + dirName + "/" + appName + "_" + timestamp + ".log");
#endif

#ifdef __TIZEN__
			CommonUtil::mkDir(dirName);
			extern char *__progname;
			filename = (dirName + "/" + std::string(__progname) + "_" + timestamp + ".log");
#endif
			FILE* fp = fopen(filename.c_str(), "w");
			if (fp == NULL) {
				fprintf(stderr, "cannot open %s file...\n", filename.c_str());
			} else {
				g_logfile_out = fp;
				setbuf(g_logfile_out,NULL);
				fp = NULL;
			}
		}
	return g_logfile_out;
}
static void vPrintf(const char* timebuffer,const char*const format, ...)
{
	char buffer[MAX_LOG_V_BUFFER_SIZE];
	va_list args;
	va_start(args,format);
	vsprintf(buffer,format,args);
	printf("%s %s\n",timebuffer,buffer);
	if(getenv(envLogWriteFile)==NULL&&_initLogFile()!=NULL)
		fprintf(g_logfile_out,"%s %s\n",timebuffer,buffer);
	va_end(args);
}
#endif
#endif
