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
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#ifndef _NS_CONSTANTS_H_
#define _NS_CONSTANTS_H_

#define __PRINTLOG 0
#define __NS_FILE__ ( strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__ )

#include "experimental/logger.h"

#ifdef TB_LOG
#ifdef __TIZEN__
#include <dlog.h>
#ifdef LOG_TAG
#undef LOG_TAG
#endif // LOG_TAG
#define LOG_TAG "NotificationService"

/**
 * convert log level
 * @param level   according to log level print the information
 * @return log level information
 */
inline log_priority NSConvertLogLevel(int level)
{
    // Always log private data on Tizen.
    level &= ~OC_LOG_PRIVATE_DATA;
    /** debug log */
    if (level == DEBUG)
    {
        return DLOG_DEBUG;
    }
    /** information log */
    if ((level == INFO) || (level == DEBUG_LITE) || (level == INFO_LITE))
    {
        return DLOG_INFO;
    }
    /** warning log */
    if (level == WARNING)
    {
        return DLOG_WARN;
    }
    /** error log */
    if ((level == ERROR) || (level == FATAL))
    {
        return DLOG_ERROR;
    }

    return DLOG_INFO;
}

#define NS_LOG_V(level, format, ...) (dlog_print(NSConvertLogLevel(level), LOG_TAG, (format), __VA_ARGS__))
#define NS_LOG(level, msg) (dlog_print(NSConvertLogLevel(level), LOG_TAG, (msg)))
#else // __TIZEN__
#define NS_LOG_V(level, format, ...) OIC_LOG_V((level), __NS_FILE__, (format), __VA_ARGS__)
#define NS_LOG(level, msg) OIC_LOG((level), __NS_FILE__, (msg))
#endif // __TIZEN__
#else // TB_LOG
#if (__PRINTLOG == 1)
#define NSConvertLogLevel(level) ( \
        ((level) == 0) ? "DEBUG" : \
        ((level) == 1) ? "INFO" : \
        ((level) == 2) ? "WARNING" : \
        ((level) == 3) ? "ERROR" : \
        ((level) == 4) ? "FATAL" : \
        ((level) == 5) ? "DEBUG_LITE" : \
        ((level) == 6) ? "INFO_LITE" : \
        ((level) == 7) ? "INFO_PRIVATE" : "INFO_PRIVATE")
#define NS_LOG_V(level, format, ...) \
    { \
        printf("%s: %s ", NSConvertLogLevel(level), __NS_FILE__); \
        printf((format), __VA_ARGS__); \
        printf("\n"); \
    }
#define NS_LOG(level, msg) \
    { \
        printf("%s: %s ", NSConvertLogLevel(level), __NS_FILE__); \
        printf((msg)); \
        printf("\n"); \
    }
#else // (__PRINTLOG == 1)
#define NSConvertLogLevel(level)
#define NS_LOG(level, msg)
#define NS_LOG_V(level, format, ...) NS_LOG((level), ((format), __VA_ARGS__))
#endif // (__PRINTLOG == 1)
#endif // TB_LOG

#define NS_TAG                     "IOT_NOTI"   /**< notification tag information */

/* SCHEDULE */
#define THREAD_COUNT               5

/* NOTIOBJ */
#define NOTIOBJ_TITLE_KEY          "x.org.iotivity.ns.title" /**< notification object title key */
#define NOTIOBJ_ID_KEY             "x.org.iotivity.ns.id"    /**< notification object Id key */
#define NOTOOBJ_CONTENT_KEY        "x.org.iotivity.ns.contenttext" /**< notification object content key */

#define DISCOVERY_TAG              "NS_PROVIDER_DISCOVERY"    /**< discovery tag */
#define SUBSCRIPTION_TAG           "NS_PROVIDER_SUBSCRIPTION" /**< subscription tag */
#define INTERFACE_TAG              "NS_PROVIDER_INTERFACE"    /**< Interface tag */
#define NOTIFICATION_TAG           "NS_PROVIDER_NOTIFICATION" /**< notification tag */
#define SCHEDULER_TAG              "NS_PROVIDER_SCHEDULER"    /**< scheduler tag */
#define LISTENER_TAG               "NS_PROVIDER_LISTENER"     /**< listener tag */
#define RESOURCE_TAG               "NS_PROVIDER_RESOURCE"     /**< resource tag */
#define TOPIC_TAG                  "NS_PROVIDER_TOPIC"        /**< topic tag */

#define NS_ROOT_TYPE               "x.org.iotivity.notification" /**< notification root type */
#define NS_COLLECTION_MESSAGE_TYPE "x.org.iotivity.notification.message" /**< collection message type */
#define NS_COLLECTION_SYNC_TYPE    "x.org.iotivity.notification.sync"    /**< collection sync type */
#define NS_COLLECTION_TOPIC_TYPE   "x.org.iotivity.notification.topic"   /**< collection topic type */

#define NS_INTERFACE_READ          "oic.if.r"        /**< read */
#define NS_INTERFACE_READWRITE     "oic.if.rw"       /**< read write */
#define NS_INTERFACE_BASELINE      "oic.if.baseline" /**< baseline */

#define NS_ROOT_URI                "/notification"   /**< root uri */
#define NS_COLLECTION_MESSAGE_URI  "/notification/message" /**< collection message uri */
#define NS_COLLECTION_SYNC_URI     "/notification/sync"    /**< collection sync uri */
#define NS_COLLECTION_TOPIC_URI    "/notification/topic"   /**< collection topic uri */

#define NS_QUERY_SEPARATOR         "&;" /**< query separator */
#define NS_KEY_VALUE_DELIMITER     "="  /**< key value delimiter */

#define NS_QUERY_CONSUMER_ID       "x.org.iotivity.ns.consumerid" /**< query consumer Id */
#define NS_QUERY_PROVIDER_ID       "x.org.iotivity.ns.providerid" /**< query provider Id */
#define NS_QUERY_INTERFACE         "if" /**< query interface */

#define NS_QUERY_ID_SIZE           10   /**< query Id size */

#define NS_POLICY_PROVIDER         1    /**< policy provider */
#define NS_POLICY_CONSUMER         0    /**< policy consumer */

#define NS_RD_PUBLISH_QUERY        "/oic/rd?rt=oic.wk.rdpub"   /**< RD publish query */

#ifdef WITH_MQ
#define NS_ATTRIBUTE_MQ_MESSAGE   "x.org.iotivity.ns.message"   /**< attribute MQ message */
#define NS_ATTIRBUTE_MQ_TOPICLIST "x.org.iotivity.ns.topiclist" /**< attribute MQ topic list */
#endif

/**
 * Verify not null value
 */
#define NS_VERIFY_NOT_NULL_V(obj) \
    { \
        if ((obj) == NULL) \
        { \
            NS_LOG_V(ERROR, "%s : %s is null", __func__, #obj); \
            return; \
        } \
    }

/**
 * Verify not null value
 */
#define NS_VERIFY_NOT_NULL(obj, retVal) \
    { \
        if ((obj) == NULL) \
        { \
            NS_LOG_V(ERROR, "%s : %s is null", __func__, #obj); \
            return (retVal); \
        } \
    }

/**
 * Verify not null value with post cleanup
 */
#define NS_VERIFY_NOT_NULL_WITH_POST_CLEANING_V(obj, func) \
    { \
        if ((obj) == NULL) \
        { \
            NS_LOG_V(ERROR, "%s : %s is null", __func__, #obj); \
            NS_LOG(ERROR, "execute deletion"); \
            (func); \
            return; \
        } \
    }

/**
 * Verify not null value of object
 * If value is null return with cleanup
 */
#define NS_VERIFY_NOT_NULL_WITH_POST_CLEANING(obj, retVal, func) \
    { \
        if ((obj) == NULL) \
        { \
            NS_LOG_V(ERROR, "%s : %s is null", __func__, #obj); \
            NS_LOG(ERROR, "execute deletion"); \
            (func); \
            return (retVal); \
        } \
    }

/**
 * Verify with success value.
 */
#define NS_VERIFY_STACK_SUCCESS_V(obj) \
    { \
        bool _ret = (obj); \
        if ( _ret != true) \
        { \
            NS_LOG_V(ERROR, "%s : %s is not OC_STACK_OK : %d", __func__, #obj, _ret); \
            return; \
        } \
    }

/**
 * Verify with success value.
 */
#define NS_VERIFY_STACK_SUCCESS(obj, retVal) \
    { \
        bool _ret = (obj); \
        if ( _ret != true) \
        { \
            NS_LOG_V(ERROR, "%s : %s is not OC_STACK_OK : %d", __func__, #obj, _ret); \
            return (retVal); \
        } \
    }

/**
 * Verify with success value.
 * If value is not match then cleanup and return.
 */
#define NS_VERIFY_STACK_SUCCESS_WITH_POST_CLEANING(obj, retVal, func) \
    { \
        bool _ret = (obj); \
        if ( _ret != true) \
        { \
            NS_LOG_V(ERROR, "%s : %s is not OC_STACK_OK : %d", __func__, #obj, _ret); \
            (func); \
            return (retVal); \
        } \
    }

/**
 * OIC free object
 */
#define NSOICFree(obj) \
    { \
        if ((obj)) \
        { \
            OICFree((obj)); \
            (obj) = NULL; \
            NS_LOG_V(DEBUG, "%s : %s Removed", __func__, #obj); \
        } \
    }

#define VERSION        IOTIVITY_VERSION

#define NS_ATTRIBUTE_VERSION "x.org.iotivity.ns.version"            /**< attribute version */
#define NS_ATTRIBUTE_POLICY "x.org.iotivity.ns.subcontrollability"  /**< attribute policy */
#define NS_ATTRIBUTE_MESSAGE "x.org.iotivity.ns.messageuri"         /**< attribute messge */
#define NS_ATTRIBUTE_SYNC "x.org.iotivity.ns.syncuri"               /**< attribute sync */
#define NS_ATTRIBUTE_TOPIC "x.org.iotivity.ns.topicuri"             /**< attribute topic */
#define NS_ATTRIBUTE_MESSAGE_ID "x.org.iotivity.ns.messageid"       /**< attribute message Id */
#define NS_ATTRIBUTE_PROVIDER_ID "x.org.iotivity.ns.providerid"     /**< attribute provider Id */
#define NS_ATTRIBUTE_CONSUMER_ID "x.org.iotivity.ns.consumerid"     /**< attribute consumer Id */
#define NS_ATTRIBUTE_TOPIC_LIST "x.org.iotivity.ns.topiclist"       /**< attribute topic list */
#define NS_ATTRIBUTE_TOPIC_NAME "x.org.iotivity.ns.topicname"       /**< attribute topic name */
#define NS_ATTRIBUTE_TOPIC_SELECTION "x.org.iotivity.ns.topicstate" /**< attribute topic selection */
#define NS_ATTRIBUTE_TITLE "x.org.iotivity.ns.title"                /**< attribute title */
#define NS_ATTRIBUTE_TEXT "x.org.iotivity.ns.contenttext"           /**< attribute text */
#define NS_ATTRIBUTE_SOURCE "x.org.iotivity.ns.source"              /**< attribute source */
#define NS_ATTRIBUTE_STATE "x.org.iotivity.ns.state"                /**< attribute state */
#define NS_ATTRIBUTE_DEVICE "x.org.iotivity.ns.device"              /**< attribute device */
#define NS_ATTRIBUTE_TYPE "x.org.iotivity.ns.type"                  /**< attribute type */
#define NS_ATTRIBUTE_DATETIME "x.org.iotivity.ns.datetime"          /**< attribute date time */
#define NS_ATTRIBUTE_TTL "x.org.iotivity.ns.ttl"                    /**< attribute ttl */
#define NS_ATTRIBUTE_ICON_IMAGE "x.org.iotivity.ns.iconimage"       /**< attribute icon image */

typedef enum eConnectionState
{
    DISCONNECTED = 0,           /**< disconnected state */
    CONNECTED = 1,              /**< connected state */

} NSConnectionState;

typedef enum eSchedulerType
{
    CALLBACK_RESPONSE_SCHEDULER = 0,    /**< callback response scheduler */
    DISCOVERY_SCHEDULER = 1,            /**< discovery scheduler */
    SUBSCRIPTION_SCHEDULER = 2,         /**< subscription scheduler */
    NOTIFICATION_SCHEDULER = 3,         /**< notification scheduler */
    TOPIC_SCHEDULER = 4,                /**< topic scheduler */

} NSSchedulerType;

typedef enum eTaskType
{
#ifdef WITH_MQ
    TASK_MQ_REQ_SUBSCRIBE = 20001,              /**< subscribe request */
#endif

    TASK_REGISTER_RESOURCE = 1000,             /**< register resource */
    TASK_PUBLISH_RESOURCE = 1001,              /**< publish resource */

    TASK_START_PRESENCE = 2000,                /**< start presence */
    TASK_STOP_PRESENCE = 2001,                 /**< stop presence */

    TASK_RECV_SUBSCRIPTION = 3000,             /**< receive subscription */
    TASK_RECV_UNSUBSCRIPTION = 3001,           /**< receive unsubscription */
    TASK_SEND_POLICY = 3002,                   /**< send policy */
    TASK_SEND_ALLOW = 3003,                    /**< allow send */
    TASK_SEND_DENY = 3004,                     /**< deny send */
    TASK_SYNC_SUBSCRIPTION = 3005,             /**< sync subscription */

    TASK_SEND_NOTIFICATION = 4000,            /**< send notification */
    TASK_SEND_PENDING_NOTI = 4001,            /**< send pending notification */

    TASK_RECV_SYNCINFO = 5000,                /**< receive sync information */
    TASK_RECV_READ = 5001,                    /**< receive read */
    TASK_RECV_DISMISS = 5003,                 /**< receive dismiss */
    TASK_SEND_SYNCINFO = 5099,                /**< send sync information */
    TASK_MAKE_SYNCINFO = 5100,                /**< make sync information */
    TASK_SEND_READ = 5101,                    /**< send read */
    TASK_SEND_DISMISS = 5102,

    TASK_CONSUMER_REQ_DISCOVER = 8001,         /**< consumer request discover */
    TASK_CONSUMER_REQ_SUBSCRIBE = 8002,        /**< consumer request subscribe */
    TASK_CONSUMER_REQ_SUBSCRIBE_CANCEL = 8003, /**< cancle consumer request subscribe */
    TASK_CONSUMER_SENT_REQ_OBSERVE = 8004,     /**< consumer send request observe */
    TASK_CONSUMER_RECV_PROVIDER_CHANGED = 8005,/**< consumer receive change provider */
    TASK_CONSUMER_RECV_MESSAGE = 8101,         /**< consumer receive message */

    TASK_CONSUMER_PROVIDER_DISCOVERED = 8201,  /**< consumer provider discovered */
    TASK_CONSUMER_PROVIDER_DELETED = 8202,     /**< consumer provider deleted */
    TASK_CONSUMER_RECV_CONFIRM = 8206,         /**< consumer receive confirm */

    TASK_CONSUMER_REQ_TOPIC_URI = 8299,       /**< consumer request topic uri */
    TASK_CONSUMER_REQ_TOPIC_LIST = 8300,      /**< consumer request topic list */
    TASK_CONSUMER_RECV_TOPIC_LIST = 8031,     /**< consumer receive topic list */
    TASK_CONSUMER_SELECT_TOPIC_LIST = 8303,   /**< consumer select topic list */

    TASK_EVENT_CONNECTED = 9000,             /**< event connected */
    TASK_EVENT_CONNECTED_TCP = 9001,         /**< TCP event connected */
    TASK_EVENT_DISCONNECTED = 9002,          /**< event disconnected */

    TASK_CB_SUBSCRIPTION = 10000,            /**< subscription callback */
    TASK_CB_SYNC = 10001,                    /**< sync callback */

    TASK_SEND_TOPICS = 11000,               /**< send topics */
    TASK_REGISTER_TOPIC = 11001,            /**< register topic */
    TASK_UNREGISTER_TOPIC = 11002,          /**< unregister topic */
    TASK_SUBSCRIBE_TOPIC = 11003,           /**< subscribe topic */
    TASK_UNSUBSCRIBE_TOPIC = 11004,         /**< unsubscribe topic */
    TASK_POST_TOPIC = 11005,                /**< post topic */
    TASK_GET_TOPICS = 11006,                /**< get topic */
    TAST_GET_CONSUMER_TOPICS = 11007        /**< get consumer topic */

} NSTaskType;

typedef enum eCache
{
    NS_CONSUMER_BLACKLIST = 0,           /**< consumer black list */
    NS_CONSUMER_WHITELIST = 1,           /**< consumer white list */

} NSCache;

typedef enum eCacheType
{
    NS_PROVIDER_CACHE_SUBSCRIBER = 1000,           /**< provider cache subscriber */
    NS_PROVIDER_CACHE_MESSAGE = 1001,              /**< provider cache messge */
    NS_PROVIDER_CACHE_CONSUMER_TOPIC_NAME = 1002,  /**< provider cache consumer topic name */
    NS_PROVIDER_CACHE_CONSUMER_TOPIC_CID = 1003,   /**< provider cache consumer topic CID */
    NS_PROVIDER_CACHE_REGISTER_TOPIC = 1004,       /**< provider cache register topic */
    NS_PROVIDER_CACHE_SUBSCRIBER_OBSERVE_ID = 1005,/**< provider cache subscriber observe Id */

    NS_CONSUMER_CACHE_PROVIDER = 2000,             /**< consumer cache provider */
    NS_CONSUMER_CACHE_MESSAGE = 2001,              /**< consumer cache message */

} NSCacheType;

typedef enum eResourceType
{
    NS_RESOURCE_MESSAGE = 1000,   /**< resource message */
    NS_RESOURCE_SYNC = 1001,      /**< resource sync */
    NS_RESOURCE_TOPIC = 1002,     /**< resource topic */

} NSResourceType;

typedef enum eInterfaceType
{
    NS_INTERFACE_TYPE_READ = 1,      /**< read */
    NS_INTERFACE_TYPE_READWRITE = 2, /**< read write */

} NSInterfaceType;

#endif /* _NS_CONSTANTS_H_ */
