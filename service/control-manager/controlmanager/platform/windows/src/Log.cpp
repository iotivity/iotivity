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



#include "Log.h"
#include "iostream"
#include "stdarg.h"
#include "stdio.h"
#include "time.h"

#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>

#if defined(__ANDROID__)
#include "android/log.h"
#elif __TIZEN__
#include "FBaseLog.h"
#elif defined(unix) || defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
#include <sys/time.h>
#include <unistd.h>

#if defined(__MACH__) && defined(__APPLE__)
#include <mach/mach.h>
#include <mach/mach_time.h>
#endif
#elif defined(_WIN32) || defined(_MSC_VER)
#include "windows.h"
#include "sstream"
#elif defined(__ANDROID__)
#include "android/log.h"
#endif

#define __DISABLE_SSDP_LOGGING__ 1
#define __DISABLE_JNI_LOGGING__ 1

static boost::mutex     s_logMutex;

namespace OC
{
    namespace Cm
    {
        namespace Log
        {


            ILogWriter             *Log::s_logWriter ;

            int32_t                 Log::s_log_enabled_modules ;

//CMLogLevel            Log::s_current_log_level = CM_LOG_LEVEL_ERROR;
            CMLogLevel          Log::s_current_log_level = CM_LOG_LEVEL_DEBUG;

            bool                    Log::s_enableSSDPLogging = false;


#ifdef __ANDROID__
            android_LogPriority getLogLevelPriority(CMLogLevel level)
            {
                switch (level)
                {
                    case CM_LOG_LEVEL_ERROR:
                        {
                            return ANDROID_LOG_ERROR;
                        }
                    case CM_LOG_LEVEL_WARN:
                        {
                            return ANDROID_LOG_WARN;
                        }
                    case CM_LOG_LEVEL_INFO:
                        {
                            return ANDROID_LOG_INFO;
                        }
                    case CM_LOG_LEVEL_DEBUG:
                        {
                            return ANDROID_LOG_DEBUG;
                        }
                    default:
                        {
                            return ANDROID_LOG_INFO;
                        }
                }
            }
#endif

            void
            Log::enableSSDPLogging(bool value)
            {
                std::cout << "\n\n\n\nEntered Log::enableSSDPLogging() with value:>>" << ( (
                              value) ? ("TRUE") : ("FALSE") ) << "<<\n\n\n\n" << std::endl ;
                if (true == value)
                {
                    s_enableSSDPLogging = true;
                }
                else
                {
                    s_enableSSDPLogging = false;
                }
            }

            void
            Log::enableLogging(int32_t modules)
            {

            }

            void
            Log::disableLogging(int32_t modules)
            {

            }

            void
            Log::setLogLevel(CMLogLevel level)
            {
                s_current_log_level = level;
            }

            void
            Log::setLogWriter(ILogWriter *writer)
            {

            }

            void
            Log::log(const char *function_name, uint32_t line_number, int32_t module, CMLogLevel level,
                     const char *message, ...)
            {
//#ifdef _MSC_VER
//  std::ostringstream logMsg(std::ostringstream::out);
//
//  char line[1024] = {0x00,} ;
//
//  va_list args ;
//  va_start(args, message);
//  vsnprintf_s(line, 1023,_TRUNCATE, message, args);
//  va_end(args);
//
//  logMsg.str("");
//
//  logMsg<< getLogLevelString(level) << "/" << getModuleString(module) << "::" << function_name << ":" << line_number << "=>" << "-" << line << std::endl ;
//
//  OutputDebugStringA((logMsg.str()).c_str());
//
//#else
//  char line[1024] = {0x00,} ;
//
//  va_list args ;
//  va_start(args, message);
//  vsnprintf(line, 1023, message, args);
//  va_end(args);
//
//  std::string threadId = boost::lexical_cast<std::string>(boost::this_thread::get_id());
//
//  std::cout << "\n" << threadId << "::" << getLogLevelString(level) << "/" << getModuleString(module) << "::" << function_name << ":" << line_number << "=>" << "-" << line << std::endl ;
//
//#endif

//#ifdef __DISABLE_SSDP_LOGGING__
//  if (false == s_enableSSDPLogging &&
//          module == CM_CORE_DEVICE_FINDER_SSDP &&
//          OC::Cm::Log::CM_LOG_LEVEL_ERROR != level) {
//      return;
//  }
//#endif

#ifdef __DISABLE_JNI_LOGGING__
                if (module == CM_JNI)
                {
                    return;
                }
#endif

                if (level > s_current_log_level)
                {
                    return;
                }

                boost::unique_lock<boost::mutex> scoped_lock(s_logMutex);

                va_list args ;
                va_start(args, message);

                int32_t buffSize = -1;

#ifdef _MSC_VER
                buffSize = _vscprintf(message, args);
#else
                /*
                 * Modifying the code to avoid any future stack corruption
                 * due to reading 'args' one more time while copying actual data
                 */
                //buffSize = vsnprintf(NULL, 0, message, args);

                va_list argsBkUP;
                va_copy(argsBkUP, args);
                buffSize = vsnprintf(NULL, 0, message, argsBkUP);
                va_end(argsBkUP);
#endif

                if (buffSize <= -1)
                {
                    return ;
                }

                /*
                #ifdef _MSC_VER
                    char line[2048]={0,};
                #else
                    char line[buffSize+1];
                    memset(line, 0, buffSize+1);
                #endif
                */
                char *line = new char[buffSize + 1];
                memset(line, 0, buffSize + 1);

#ifdef _MSC_VER
                vsnprintf_s(line, buffSize + 1, _TRUNCATE, message, args);
#else
                vsnprintf(line, buffSize + 1, message, args);
#endif

                va_end(args);

                std::ostringstream logMsg(std::ostringstream::out);
                logMsg.str("");
                logMsg << getLogLevelString(level) << "/" << getModuleString(module) << "::" << function_name << ":"
                       << line_number << " => " << line << std::endl;
                delete[] line;

#ifdef __ANDROID__
                __android_log_write(getLogLevelPriority(level), "CM", logMsg.str().c_str());

#elif __TIZEN__
                AppLogTagInternal("CM", function_name, line_number, logMsg.str().c_str());

#elif _MSC_VER
                OutputDebugStringA(logMsg.str().c_str());
#else

                /*
                 * Adding Time Stamp (till milliseconds) to the log - START
                 *
                 * Format of the time stamp: MM-DD HH:MM:SS.MSEC, (e.g., 03-18 20:31:24.654)
                 *
                 */

                std::string threadId = boost::lexical_cast<std::string>(boost::this_thread::get_id());
                std::string timeStamp = getCurrentTimeStampValue();

                std::cout << timeStamp << " => [TID:" << threadId << "]" << logMsg.str() << std::endl;
#endif
            }

            std::string
            Log::getCurrentTimeStampValue()
            {
                std::string currentTimeStamp = "";
                std::ostringstream formattedTime(std::ostringstream::out);
                std::ostringstream elapsedUSecStrStream;
                std::string elapsedUSecStr = "";

                double elapsedUSec = -1;

#if defined(unix) || defined(__unix__) || defined(__unix) || (defined(__MACH__) && defined(__APPLE__))
                struct timeval currUnixTime;
                gettimeofday( &currUnixTime, NULL );

                elapsedUSec = (double)currUnixTime.tv_usec;
                elapsedUSecStrStream << elapsedUSec;
                elapsedUSecStr = elapsedUSecStrStream.str();

                if (elapsedUSecStr.length() > 2)
                    elapsedUSecStr = elapsedUSecStr.substr(0, 3);

                struct tm *currLocalTime = NULL;

                currLocalTime = localtime((time_t *)&currUnixTime.tv_sec);

                if (!currLocalTime)
                {
                    return currentTimeStamp;
                }

                formattedTime.str("");
                formattedTime.str();

                formattedTime << std::setfill('0') \
                              /*<< std::setw(4) << (1900+currLocalTime->tm_year) \*/
                              << std::setw(2) << (currLocalTime->tm_mon + 1) << "-" \
                              << std::setw(2) << (currLocalTime->tm_mday) << " " \
                              << std::setw(2) << (currLocalTime->tm_hour) << ":" \
                              << std::setw(2) << (currLocalTime->tm_min) << ":" \
                              << std::setw(2) << (currLocalTime->tm_sec) << "." \
                              << std::setw(3) << elapsedUSecStr;

                currentTimeStamp = formattedTime.str();

#elif defined(_WIN32)
                FILETIME currWinUTCFileTime;

                /*
                 * Handle Different Windows Versions:
                 *          If Windows is 'Windows 8' || 'Windows Server 2012' || or later, then
                 *              call GetSystemTimePreciseAsFileTime();
                 *          ELSE
                 *              call GetSystemTimeAsFileTime();
                 */
#if defined(NTDDI_WIN8) && (NTDDI_VERSION >= NTDDI_WIN8)
                GetSystemTimePreciseAsFileTime(&currWinUTCFileTime);
#else
                GetSystemTimeAsFileTime(&currWinUTCFileTime);
#endif

                // Convert retrieved UTC FILE Time to Local FILE Time
                FILETIME currWinLocalFileTime;
                FileTimeToLocalFileTime (&currWinUTCFileTime, &currWinLocalFileTime);

                // Get the time
                ULONGLONG currTime = ((ULONGLONG)currWinUTCFileTime.dwHighDateTime << 32) |
                                     (ULONGLONG)currWinUTCFileTime.dwLowDateTime;

                elapsedUSec = fmod((double)currTime, 10000000.0) / 10;
                elapsedUSecStrStream << elapsedUSec;
                elapsedUSecStr = elapsedUSecStrStream.str();

                if (elapsedUSecStr.length() > 2)
                    elapsedUSecStr = elapsedUSecStr.substr(0, 3);

                SYSTEMTIME currentSysTime;
                // Now, convert File Time to System Time for printing
                FileTimeToSystemTime(&currWinLocalFileTime, &currentSysTime);

                //sprintf(currTimeStamp, "%d:%d:%d, %d.%d.%d \r",stime.wHour,stime.wMinute,stime.wSecond, stime.wDay,stime.wMonth,stime.wYear);

                formattedTime.str("");
                formattedTime.str();

                formattedTime << std::setfill('0') \
                              /*<< std::setw(4) << (currentSysTime.wYear) \*/
                              << std::setw(2) << (currentSysTime.wMonth) << "-" \
                              << std::setw(2) << (currentSysTime.wDay) << " " \
                              << std::setw(2) << (currentSysTime.wHour) << ":" \
                              << std::setw(2) << (currentSysTime.wMinute) << ":" \
                              << std::setw(2) << (currentSysTime.wSecond) << "." \
                              << std::setw(3) << elapsedUSecStr;

                currentTimeStamp = formattedTime.str();

                //std::cout<<"Windows: >>"<<currentTimeStamp.c_str()<<"<<";
                /*  #elif defined(__MACH__) && defined(__APPLE__)
                    static double machTimeConvert = 0.0;
                    if ( machTimeConvert == 0.0 )
                    {
                        mach_timebase_info_data_t machTimeBase;
                        (void)mach_timebase_info( &machTimeBase );
                        machTimeConvert = (double)machTimeBase.numer / (double)machTimeBase.denom / 1000000000.0;
                    }
                    return (double)mach_absolute_time() * machTimeConvert;*/

#else
                std::cout << "UNKNOWN OS: >>" << currentTimeStamp.c_str() << "<<";
                currentTimeStamp = "";
#endif

                return currentTimeStamp;
            }
            /*
             * Adding Time Stamp (till seconds) to the log - END
             */
            const char *
            Log::getLogLevelString(CMLogLevel level)
            {
                switch (level)
                {
                    case CM_LOG_LEVEL_ERROR:
                        {
                            return "[ERROR]";
                        }
                    case CM_LOG_LEVEL_WARN:
                        {
                            return "[WARN]";
                        }
                    case CM_LOG_LEVEL_INFO:
                        {
                            return "[INFO]";
                        }
                    case CM_LOG_LEVEL_DEBUG:
                        {
                            return "[DEBUG]";
                        }
                    default:
                        {
                            return "[DEFAULT]";
                        }
                }
            }

            const char *
            Log::getModuleString(int32_t module)
            {
                switch (module)
                {
                    case CM_API:
                        {
                            return "CM API";
                        }
                    case CM_API_SERVER:
                        {
                            return "CM SERVER API";
                        }
                    case CM_API_CLIENT:
                        {
                            return "CM ClIENT API";
                        }
                    case CM_CORE_DEVICE_FINDER:
                        {
                            return "DEVICE FINDER";
                        }
                    case CM_CORE_DEVICE_FINDER_OC:
                        {
                            return "OC";
                        }
                    case CM_CORE_CLIENT:
                        {
                            return "CM CORE CLIENT";
                        }
                    case CM_CORE_SERVER:
                        {
                            return "CM CORE SERVER";
                        }
                    case CM_CORE_HTTP_CLIENT:
                        {
                            return "CM CORE HTTP CLIENT";
                        }
                    case CM_CORE_HTTP_SERVER:
                        {
                            return "CM CORE HTTP SERVER";
                        }
                    case CM_CORE_ENGINE:
                        {
                            return "CM CORE ENGINE";
                        }
                    case CM_JSON:
                        {
                            return "CM JSON";
                        }
                    case CM_CORE_EVENT_MANAGER:
                        {
                            return "CM EVENT MANAGER";
                        }
                    case CM_PAL:
                        {
                            return "CM PAL";
                        }
                    case CM_PAL_NET:
                        {
                            return "CM PAL NET";
                        }
                    case CM_PAL_THREAD:
                        {
                            return "CM PAL THREAD";
                        }
                    case CM_CORE_HTTPS_CLIENT:
                        {
                            return "CM CORE HTTPS CLIENT";
                        }
                    case CM_SCS_CLIENT:
                        {
                            return "CM SCS CLIENT";
                        }
                    case CM_REMOTE_DEVICE_FINDER:
                        {
                            return "CM REMOTE DEVICE FINDER";
                        }
                    case CM_REMOTE_CLIENT:
                        {
                            return "CM REMOTE CLIENT";
                        }
                    case CM_REMOTE_SERVER:
                        {
                            return "CM REMOTE SERVER";
                        }
                    case CM_CORE_AGENT:
                        {
                            return "CM_CORE_AGENT";
                        }
                    case CM_FILE_DATABASE:
                        {
                            return "CM_FILE_DATABASE";
                        }
                    case CM_SUB_MGR:
                        {
                            return "CM_SUB_MGR";
                        }
                    case CM_UTILS:
                        {
                            return "CM_UTILS";
                        }
                    default:
                        {
                            return "";
                        }
                }
            }


        } /* namespace Log */
    } /* namespace Cm */
} /* namespace OC */
