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



#ifndef CMCOMMON_H_
#define CMCOMMON_H_

#include "Log.h"

#define HTTP_OK                     200
#define HTTP_CREATED                201
#define HTTP_NO_CONTENT             204

#define HTTP_MOVED_PERMANENTLY      301
#define HTTP_REDIRECT               302

#define HTTP_BAD_REQUEST            400
#define HTTP_UNAUTHORIZED           401
#define HTTP_FORBIDDEN              403
#define HTTP_NOT_FOUND              404
#define HTTP_METHOD_NOT_ALLOWED     405
#define HTTP_NOT_ACCEPTABLE         406
#define HTTP_EXPECTION_FAILED       417

#define HTTP_INTERNAL_SERVER_ERROR  500
#define HTTP_NOT_IMPLEMENTED        501


#define PACKET_SIZE 1024


#define OIC_LOCATION_HEADER             2048
#define OIC_VERSION_HEADER              2050
#define OIC_AUTHORIZATION_HEADER        2052
#define OIC_WWW_AUTHORIZATION_HEADER    2054



#define HTTP_HEADER_VERSION     "X-API-Version"

#define HTTP_QUERY_FILTER       "filter"
#define HTTP_QUERY_RECURSIVE    "recursive"
#define HTTP_AUTHORIZATION_HEADER       "Authorization"
#define HTTP_WWW_AUTHORIZATION_HEADER   "WWW-Authenticate"
#define HTTP_AUTH_HEADER_VALUE  "Bearer "
#define HTTP_WWW_AUTH_HEADER_VALUE      "Bearer error=\"invalid_token\""
#define CM_VERSION_STRING       "v1.0.0"
#define DEV_TOKEN_PATH      "/devicetoken/?UUID="
#define UUID_DELIMETER      "UUID="
#define DEVICE_TOKEN_WAIT_TIME_IN_SEC 100
#define EASY_SETUP_WAIT_TIME_IN_SEC 300

#define DEFAULT_MAX_AGE_INTERVAL    600     //  Default SSDP Max-AGE interval is 10 minutes (it shall be 1 to 10 minutes)
#define EASY_SETUP_MAX_AGE_INTERVAL 2       //  SSDP Max-AGE interval in Easy Setup process is 2 seconds

// TryReturn Macros
#define TryReturn(condition, returnValue, ...) \
    if (!(condition)) { \
        OC::Cm::Log::Log::log( __FUNCTION__, __LINE__, MODULE_ID,  OC::Cm::Log::CM_LOG_LEVEL_INFO, __VA_ARGS__ );\
        return returnValue; \
    } else {;}

#define TryReturnVoid(condition, ...) \
    if (!(condition)) { \
        OC::Cm::Log::Log::log( __FUNCTION__, __LINE__, MODULE_ID,  OC::Cm::Log::CM_LOG_LEVEL_INFO, __VA_ARGS__ );\
        return; \
    } else {;}

#define TryReturnEx(evaluation, returnValue, expressions, ...) \
    if (!(evaluation)) { \
        OC::Cm::Log::Log::log( __FUNCTION__, __LINE__, MODULE_ID,  OC::Cm::Log::CM_LOG_LEVEL_INFO, __VA_ARGS__ );\
        expressions; \
        return returnValue; \
    } else {;}

#define TryReturnVoidEx(condition, expressions, ...) \
    if (!(condition)) { \
        OC::Cm::Log::Log::log( __FUNCTION__, __LINE__, MODULE_ID,  OC::Cm::Log::CM_LOG_LEVEL_INFO, __VA_ARGS__ );\
        expressions; \
        return; \
    } else {;}

#define TryLog(evaluation, ...) \
    if (!(evaluation)) { \
        OC::Cm::Log::Log::log( __FUNCTION__, __LINE__, MODULE_ID,  OC::Cm::Log::CM_LOG_LEVEL_INFO, __VA_ARGS__ );\
    } else {;}

#define TryReturnNoLog(evaluation, returnValue, expressions) \
    if (!(evaluation)) { \
        expressions; \
        return returnValue; \
    } else {;}

#define TryReturnVoidNoLog(evaluation, expressions) \
    if (!(evaluation)) { \
        expressions; \
        return ; \
    } else {;}


#endif /* CMCOMMON_H_ */
