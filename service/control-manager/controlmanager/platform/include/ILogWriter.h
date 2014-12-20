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



#ifndef ILOGWRITER_H_
#define ILOGWRITER_H_

namespace OC
{
    namespace Cm
    {
        namespace Log
        {

            typedef enum log_level
            {
                CM_LOG_LEVEL_SILENT = -3,       // To disable logging

                CM_LOG_LEVEL_ERROR = -2,

                CM_LOG_LEVEL_WARN   = -1,

                CM_LOG_LEVEL_INFO       = 0,    // Default log Level

                CM_LOG_LEVEL_DEBUG  = 1,

            } CMLogLevel;


            class ILogWriter
            {
                public:

                    virtual ~ILogWriter() {};

                    virtual void printLog(CMLogLevel level, const char *message) = 0 ;

                    virtual void printLog(CMLogLevel level, const char *format, ...) = 0 ;

                    //  virtual void printLog(CMLogLevel level, const char *format, va_list arguments) = 0 ;
            };


        } /* namespace Log */
    } /* namespace Cm */
} /* namespace OC */

#endif /* ILOGWRITER_H_ */
