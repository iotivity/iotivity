/******************************************************************
 *
 * Copyright 2019 Samsung Electronics All Rights Reserved.
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

#ifndef __MBEDTLS_MESSAGES__
#define __MBEDTLS_MESSAGES__

#if defined (__WITH_TLS__) || defined(__WITH_DTLS__)

#include "mbedtls/error.h"

#define _BUF_SIZE_ 2048

#if !defined(lTAG)
#define lTAG "MBEDTLS"
#endif

/**
 * Macro to log an mbedtls error
 * For mbedtls functions that return 0 as non-error
 * @note Invoker must provide message buffer, and must include "logger.h"
 */
#if !defined(LOG_MBED_ERROR)
#define LOG_MBED_ERROR(tag, ret, buf, bufSize, logLevel)            \
    do                                                              \
    {                                                               \
        if (0!=(ret))                                               \
        {                                                           \
            mbedtls_strerror((ret), (buf), (bufSize));              \
            OIC_LOG_V((logLevel), (tag), "%s", (buf));              \
        }                                                           \
    }                                                               \
    while(0)
#endif//LOG_MBED_ERROR
#if !defined(LOG_MBEDTLS_VERIFY_ERROR)
#define LOG_MBEDTLS_VERIFY_ERROR(flags)                             \
    do                                                              \
    {                                                               \
        if (0!=(flags))                                             \
        {                                                           \
            char *_buf = (char*)OICCalloc(_BUF_SIZE_, sizeof(char));\
            if (_buf)                                               \
            {                                                       \
                mbedtls_x509_crt_verify_info(_buf, _BUF_SIZE_, "", flags); \
                OIC_LOG_V(ERROR, lTAG" VERIFY", "%s", _buf);        \
                OICFree(_buf);                                      \
            }                                                       \
        }                                                           \
    }                                                               \
    while(0)
#endif//LOG_MBEDTLS_VERIFY_ERROR
#if !defined(LOG_MBEDTLS_ERROR)
#define LOG_MBEDTLS_ERROR(ret)                                      \
    do                                                              \
    {                                                               \
        if (0!=(ret))                                               \
        {                                                           \
            char *_buf = (char*)OICCalloc(_BUF_SIZE_, sizeof(char));\
            if (_buf)                                               \
            {                                                       \
                mbedtls_strerror((ret), _buf, _BUF_SIZE_);          \
                OIC_LOG_V(ERROR, lTAG, "%s", _buf);                 \
                OICFree(_buf);                                      \
            }                                                       \
        }                                                           \
    }                                                               \
    while(0)
#endif//LOG_MBEDTLS_ERROR
#if !defined(LOG_MBEDTLS_ERROR_LOCAL)
#define LOG_MBEDTLS_ERROR_LOCAL(TAG, ret)                           \
    do                                                              \
    {                                                               \
        if (0!=(ret))                                               \
        {                                                           \
            char *_buf = (char*)OICCalloc(_BUF_SIZE_, sizeof(char));\
            if (_buf)                                               \
            {                                                       \
                mbedtls_strerror((ret), _buf, _BUF_SIZE_);          \
                OIC_LOG_V(ERROR, TAG, "%s", _buf);                  \
                OICFree(_buf);                                      \
            }                                                       \
        }                                                           \
    }                                                               \
    while(0)
#endif//LOG_MBEDTLS_ERROR

#endif //(__WITH_TLS__) || defined(__WITH_DTLS__)

#endif //__MBEDTLS_MESSAGES__
