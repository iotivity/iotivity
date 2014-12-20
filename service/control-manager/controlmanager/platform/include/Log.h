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



#ifndef LOG_H_
#define LOG_H_


#include "CMDataTypes.h"
#include "ILogWriter.h"
#include "CMPALConfig.h"

#define __ENABLE_LOG__

#ifdef _MSC_VER
// Visual Studio Compiler
#define CM_LOGGING_INFO 1
#define CM_LOGGING_ERROR 1
#define CM_LOGGING_DEBUG 1
#define CM_LOGGING_WARNING 1
#endif

typedef enum
{
    CM_API                      = 0x01,

    CM_API_SERVER               = 0x02,

    CM_API_CLIENT               = 0x04,

    CM_CORE_DEVICE_FINDER       = 0x08,

    CM_CORE_DEVICE_FINDER_OC   = 0x10,

    CM_CORE_CLIENT              = 0x20,

    CM_CORE_SERVER              = 0x40,

    CM_CORE_HTTP_CLIENT     = 0x80,

    CM_CORE_HTTP_SERVER     = 0x100,

    CM_CORE_ENGINE              = 0x200,

    CM_JSON                    = 0x400,

    CM_CORE_EVENT_MANAGER       = 0x800,

    CM_PAL                      = 0x1000,

    CM_PAL_NET                  = 0x2000,

    CM_PAL_THREAD               = 0x4000,

    CM_CORE_HTTP                = 0x8000,

    CM_CORE_NOTIFICATION        = 0x10000,

    CM_API_FCGI_SERVER          = 0x20000,

    CM_API_FCGI_CLIENT          = 0x40000,

    CM_CORE_HTTPS_CLIENT        = 0x80000,

    CM_SCS_CLIENT               = 0x100000,

    CM_REMOTE_DEVICE_FINDER = 0x200000,

    CM_CORE_AGENT               = 0x400000,

    CM_JNI                      = 0x800000,

    CM_FILE_DATABASE            = 0x1000000,

    CM_REMOTE_CLIENT            = 0x2000000,

    CM_REMOTE_SERVER            = 0x4000000,

    CM_SUB_MGR                  = 0x8000000,

    CM_UTILS                    = 0x10000000,

    CM_ALL_MODULES              = 0xFFFFFFFF,

} CMModuleID;


namespace OC
{
    namespace Cm
    {
        namespace Log
        {

            class CMPAL_API Log
            {

                    static ILogWriter      *s_logWriter ;

                    static int32_t          s_log_enabled_modules ;

                    static CMLogLevel       s_current_log_level ;

                    static bool             s_enableSSDPLogging;

                public:

                    static void enableLogging(int32_t modules) ;

                    static void disableLogging(int32_t modules) ;

                    static void setLogLevel(CMLogLevel level);

                    static void setLogWriter(ILogWriter *writer);

                    static void enableSSDPLogging(bool value);      // SSDP Log enabler

#if defined(__GNUG__) || defined (__GNUC__)
                    static void log(const char *function_name, uint32_t line_number, int32_t module, CMLogLevel level,
                                    const char *message, ...) __attribute__ ((format (printf, 5, 6)));
#else
                    static void log(const char *function_name, uint32_t line_number, int32_t module, CMLogLevel level,
                                    const char *message, ...);
#endif

                private:

                    static const char *getModuleString(int32_t module);

                    static const char *getLogLevelString(CMLogLevel level);

                    static std::string getCurrentTimeStampValue();
            };

#ifdef CM_LOGGING_SILENT
#undef __ENABLE_LOG__
#endif

#ifdef __ENABLE_LOG__
            /*
             * -DCM_LOGGING_ERROR=1 -DCM_LOGGING_WARNING=1 -DCM_LOGGING_INFO=1 -DCM_LOGGING_DEBUG=1
             */
#ifdef CM_LOGGING_INFO
#define CM_LOG_INFO_A(module, message, ... )     OC::Cm::Log::Log::log( __FUNCTION__, __LINE__, module,  OC::Cm::Log::CM_LOG_LEVEL_INFO, message, __VA_ARGS__ )
            //#define CM_LOG_INFO(module, message)  OC::Cm::Log::Log::log( __FUNCTION__, __LINE__, module,  OC::Cm::Log::CM_LOG_LEVEL_INFO, message )
#else
#define CM_LOG_INFO_A(module, message, ... )
#endif

#ifdef CM_LOGGING_ERROR
#define CM_LOG_ERROR_A(module, message, ... )    OC::Cm::Log::Log::log( __FUNCTION__, __LINE__, module,  OC::Cm::Log::CM_LOG_LEVEL_ERROR, message, __VA_ARGS__ )
            //#define CM_LOG_ERROR(module, message) OC::Cm::Log::Log::log( __FUNCTION__, __LINE__, module,  OC::Cm::Log::CM_LOG_LEVEL_ERROR, message )
#else
#define CM_LOG_ERROR_A(module, message, ... )
#endif

#ifdef CM_LOGGING_DEBUG
#define CM_LOG_DEBUG_A(module, message, ... )    OC::Cm::Log::Log::log( __FUNCTION__, __LINE__, module,  OC::Cm::Log::CM_LOG_LEVEL_DEBUG, message, __VA_ARGS__ )
            //#define CM_LOG_DEBUG(module, message) OC::Cm::Log::Log::log( __FUNCTION__, __LINE__, module,  OC::Cm::Log::CM_LOG_LEVEL_DEBUG, message )
#else
#define CM_LOG_DEBUG_A(module, message, ... )
#endif

#ifdef CM_LOGGING_WARNING
#define CM_LOG_WARN_A(module, message, ... )    OC::Cm::Log::Log::log( __FUNCTION__, __LINE__, module,  OC::Cm::Log::CM_LOG_LEVEL_WARN, message, __VA_ARGS__ )
            //#define CM_LOG_WARN(module, message)  OC::Cm::Log::Log::log( __FUNCTION__, __LINE__, module,  OC::Cm::Log::CM_LOG_LEVEL_WARN, message )
#else
#define CM_LOG_WARN_A(module, message, ... )
#endif

#else
#define CM_LOG_INFO_A(module, message, ... )
#define CM_LOG_INFO(module, message)

#define CM_LOG_ERROR_A(module, message, ... )
#define CM_LOG_ERROR(module, message)

#define CM_LOG_DEBUG_A(module, message, ... )
#define CM_LOG_DEBUG(module, message)

#define CM_LOG_WARN_A(module, message, ... )
#define CM_LOG_WARN(module, message)
#endif


        } /* namespace Log */
    } /* namespace Cm */
} /* namespace OC */
#endif /* LOG_H_ */
