/*******************************************************************
 *
 * Copyright (c) 2018 LG Electronics, Inc.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

#ifndef __LOGGING_H__
#define __LOGGING_H__

#include <PmLogLib.h>

extern PmLogContext gLogContext;

/* Logging for ocserverbasciops context ********
 * The parameters needed are
 * msgid - unique message id
 * kvcount - count for key-value pairs
 * ... - key-value pairs and free text. key-value pairs are formed using PMLOGKS or PMLOGKFV
 * e.g.)
 * WCALOG_CRITICAL(msgid, 2, PMLOGKS("key1", "value1"), PMLOGKFV("key2", "%d", value2), "free text message");
 **********************************************/
#define OCSAMPLE_LOG_CRITICAL(msgid, kvcount, ...) \
        PmLogCritical(gLogContext, msgid, kvcount, ##__VA_ARGS__)

#define OCSAMPLE_LOG_ERROR(msgid, kvcount, ...) \
        PmLogError(gLogContext, msgid, kvcount,##__VA_ARGS__)

#define OCSAMPLE_LOG_WARNING(msgid, kvcount, ...) \
        PmLogWarning(gLogContext, msgid, kvcount, ##__VA_ARGS__)

#define OCSAMPLE_LOG_INFO(msgid, kvcount, ...) \
        PmLogInfo(gLogContext, msgid, kvcount, ##__VA_ARGS__)

#define OCSAMPLE_LOG_DEBUG(...) \
        PmLogDebug(gLogContext, ##__VA_ARGS__)

#define OCSAMPLE_LOG_ESCAPED_ERRMSG(msgid, errmsg) \
    do { \
    gchar *escaped_errtext = g_strescape(errmsg, NULL); \
    WCALOG_ERROR(msgid, 1, PMLOGKS("Error", escaped_errtext), ""); \
    g_free(escaped_errtext); \
    } while(0)

#define OCSAMPLE_LOG_ADDR_INFOMSG(msgid, name, addr) \
    do { \
    gchar straddr[16]; \
    snprintf(straddr, 16, "%p", addr); \
    WCALOG_INFO(msgid, 1, PMLOGKS(name, straddr), ""); \
    } while(0)

/** list of MSGID's */
#define MSGID_WIFI_SRVC_REGISTER_FAIL                   "WIFI_SRVC_REGISTER_FAIL"

#endif // __LOGGING_H__
