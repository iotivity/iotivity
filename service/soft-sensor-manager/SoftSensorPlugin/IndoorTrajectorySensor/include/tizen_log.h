/******************************************************************
*
* Copyright 2014 Samsung Electronics All Rights Reserved.
*
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/
#ifndef __OICAPP_LOG_H__
#define __OICAPP_LOG_H__

#define TIZEN_DEBUG_ENABLE
#define LOG_TAG "OIC_TEST"
#include <dlog.h>

#define LOG_COLOR_RED       "\033[0;31m"
#define LOG_COLOR_BROWN     "\033[0;33m"
#define LOG_COLOR_BLUE      "\033[0;34m"
#define LOG_COLOR_END       "\033[0;m"


#if 0
#define _DBG(fmt, arg...) SLOGD(fmt, ##arg)
#define _INFO(fmt, arg...) SLOGI(fmt, ##arg)
#define _WARN(fmt, arg...) SLOGW(fmt, ##arg)
#define _ERR(fmt, arg...) SLOGE(fmt, ##arg)
#else
#define _DBG(fmt, arg...) \
    printf("[OIC_TEST]%s(%d):" fmt "\n", __FUNCTION__, __LINE__, ##arg)
#define _INFO(fmt, arg...) \
    printf("[OIC_TEST]%s(%d):" fmt "\n", __FUNCTION__, __LINE__, ##arg)
#define _WARN(fmt, arg...) \
    printf("[OIC_TEST]%s(%d):" fmt "\n", __FUNCTION__, __LINE__, ##arg)
#define _ERR(fmt, arg...) \
    printf("[OIC_TEST]%s(%d):" fmt "\n", __FUNCTION__, __LINE__, ##arg)
#endif

#define DBG(fmt, arg...) _DBG(fmt, ##arg)
#define WARN(fmt, arg...) _WARN(LOG_COLOR_BROWN fmt LOG_COLOR_END, ##arg)
#define ERR(fmt, arg...) _ERR(LOG_COLOR_RED fmt LOG_COLOR_END, ##arg)
#define INFO(fmt, arg...) _INFO(LOG_COLOR_BLUE fmt LOG_COLOR_END, ##arg)

#define ret_if(expr) \
    do { \
        if (expr) { \
            ERR("(%s)", #expr); \
            return; \
        }\
    } while(0)
#define retv_if(expr, val) \
    do {\
        if (expr) { \
            ERR("(%s)", #expr); \
            return (val); \
        } \
    } while(0)
#define retm_if(expr, fmt, arg...) \
    do {\
        if (expr) { \
            ERR(fmt, ##arg); \
            return; \
        }\
    } while(0)
#define retvm_if(expr, val, fmt, arg...) \
    do {\
        if (expr) { \
            ERR(fmt, ##arg); \
            return (val); \
        } \
    } while(0)
#define warn_if(expr) \
    do { \
        if (expr) { \
            WARN("(%s)", #expr); \
        } \
    } while (0)



#define BT_ERROR_CHECK( _FUNC_ )        {                               \
        bt_error_e __err__ = (bt_error_e)(_FUNC_);      \
    if ( __err__ != BT_ERROR_NONE )                             \
    {                                                                                                   \
        what_return_value( __err__ );                               \
        goto BT_ERROR;                                              \
    }                                                                               \
}


#endif //__OICAPP_LOG_H__

