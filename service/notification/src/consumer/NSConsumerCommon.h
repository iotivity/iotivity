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

#ifndef _NS_CONSUMER_COMMON_H_
#define _NS_CONSUMER_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdio.h>
#include <stdbool.h>

#include "NSConsumerInterface.h"
#include "NSStructs.h"
#include "ocstack.h"

#define NS_QOS OC_HIGH_QOS                             /**< high QOS */
#define NS_RESOURCE_TYPE "x.org.iotivity.notification" /**< resource type */
#define NS_RESOURCE_URI "/notification"                /**< resource uri */
#define NS_INTERFACE_BASELINE "oic.if.baseline"        /**< interface baseline */
#define NS_RESOURCE_QUERY "/oic/res"                   /**< resource query */

#define NS_DISCOVER_QUERY "/oic/res?rt=x.org.iotivity.notification" /**< resource query */
#define NS_DEVICE_ID_LENGTH 37                                      /**< device ID length */

typedef enum
{
    NS_DISCOVER_DEFAULT, /**<  will work for adapter_ip. */
    NS_DISCOVER_UDP,
    NS_DISCOVER_TCP,
    NS_DISCOVER_CLOUD
} NSConsumerDiscoverType;

typedef enum
{
    NS_SELECTION_CONSUMER = 0, /**< select consumer */
    NS_SELECTION_PROVIDER = 1  /**< select provider */
} NSSelector;

typedef struct NSProviderConnectionInfo
{
    OCDevAddr * addr;          /**< device address */

    OCDoHandle messageHandle;  /**< message handle */
    OCDoHandle syncHandle;     /**< sync handle */

    bool isCloudConnection;    /**< check cloud connection */
    bool isSubscribing;        /**< check subscribing or not */

    struct NSProviderConnectionInfo * next; /**< pointer to nect node */

} NSProviderConnectionInfo;

typedef struct
{
    char providerId[NS_DEVICE_ID_LENGTH]; /**< provider Id */

    NSTopicLL * topicLL;                  /**< pointer to topic link list */

    char * messageUri;                    /**< message URI */
    char * syncUri;                       /**< sync URI */
    char * topicUri;                      /**< topic URI */

    NSSelector accessPolicy;             /**< access policy */
    NSProviderState state;               /**< provider state information */

    NSProviderConnectionInfo * connection; /**< pointer to next node */

} NSProvider_internal;

typedef struct
{
    uint64_t messageId;                      /**< message Id */
    char providerId[NS_DEVICE_ID_LENGTH];    /**< provider Id */
    NSSyncType state;                        /**< sync type state */

    NSProviderConnectionInfo * connection;   /**< pointer to next node */

} NSSyncInfo_internal;

/**
 * API to check consumer started.
 *
 * @return boolean value
 */
bool NSIsStartedConsumer(void);

/**
 * Set start value of consumer.
 *
 * @param   setValue        value to set.
 */
void NSSetIsStartedConsumer(bool setValue);

/**
 * Set provider changed callback.
 *
 * @param cb        provider state callback.
 */
void NSSetProviderChangedCb(NSProviderStateCallback cb);

/**
 * Provider changed.
 *
 * @param provider      notification provider.
 * @param response      provider state.
 */
void NSProviderChanged(NSProvider * provider, NSProviderState response);

/**
 * Set message posted callback.
 *
 * @param cb      message received callback.
 */
void NSSetMessagePostedCb(NSMessageReceivedCallback  cb);

/**
 * Message post.
 *
 * @param obj     message object.
 */
void NSMessagePost(NSMessage * obj);

/**
 * set notification sync callback.
 *
 * @param cb      sync information received callback.
 */
void NSSetNotificationSyncCb(NSSyncInfoReceivedCallback cb);

/**
 * Notification sync.
 *
 * @param sync sync information.
 */
void NSNotificationSync(NSSyncInfo * sync);

/**
 * Get consumer ID.
 *
 * @return   return consumer ID.
 */
char ** NSGetConsumerId(void);

/**
 * Set consumer ID.
 *
 * @param   cId        consumer ID.
 */
void NSSetConsumerId(char * cId);

/**
 * create request uri with consumer ID.
 *
 * @param   uri        request uri.
 *
 * @return    return request query.
 */
char * NSMakeRequestUriWithConsumerId(const char * uri);

/**
 * create task.
 *
 * @param NSTaskType      task type.
 * @param void *          void data.
 *
 * @return created task.
 */
NSTask * NSMakeTask(NSTaskType, void *);

/**
 * push event API
 *
 * @param task  notification task
 *
 * @return OK if success.
 */
NSResult NSConsumerPushEvent(NSTask *);

/**
 * Get message.
 *
 * @param payload         representation payload.
 *
 * @return message.
 */
NSMessage * NSGetMessage(OCRepPayload * payload);

/**
 * Copy message.
 *
 * @param msg    notification message.
 *
 * @return new message.
 */
NSMessage * NSCopyMessage(NSMessage *);

/**
 * Remove message.
 *
 * @param msg    notification message.
 */
void NSRemoveMessage(NSMessage *);

/**
 * Get provider after clean.
 *
 * @param pId        provider Id
 * @param mUri       message uri
 * @param sUri       sync uri
 * @param tUri       topic uri
 * @param connection provider connection information
 */
void NSGetProviderPostClean(
        char * pId, char * mUri, char * sUri, char * tUri, NSProviderConnectionInfo * connection);

/**
 * Get provider.
 *
 * @param clientResponse    client response.
 *
 * @return new provider.
 */
NSProvider_internal * NSGetProvider(OCClientResponse * clientResponse);

/**
 * Create provider connections.
 *
 * @param inAddr   device address.
 *
 * @return provider connection information.
 */
NSProviderConnectionInfo * NSCreateProviderConnections(OCDevAddr *);

/**
 * Copy provider connections.
 *
 * @param NSProviderConnectionInfo     connection information.
 *
 * @return provider connection information.
 */
NSProviderConnectionInfo * NSCopyProviderConnections(NSProviderConnectionInfo *);

/**
 * Remove connections.
 *
 * @param NSProviderConnectionInfo    connection to remove.
 */
void NSRemoveConnections(NSProviderConnectionInfo *);

/**
 * Copy provider internal.
 */
NSProvider_internal * NSCopyProvider_internal(NSProvider_internal *);

/**
 * Copy provider.
 */
NSProvider * NSCopyProvider(NSProvider_internal *);

/**
 * Remove provider internal.
 */
void NSRemoveProvider_internal(void *);

/**
 * Remove provider.
 *
 * @param NSProvider        provider to remove.
 */
void NSRemoveProvider(NSProvider *);

/**
 * Copy topic node.
 *
 * @param NSTopicLL    new topic node.
 *
 * @return new topic node list.
 */
NSTopicLL * NSCopyTopicNode(NSTopicLL *);

/**
 * Remove topic node.
 *
 * @param NSTopicLL    topic node to remove.
 */
void NSRemoveTopicNode(NSTopicLL *);

/**
 * Inssert topic node.
 *
 * @param NSTopicLL   head node of the list.
 * @param NSTopicLL   new node to add.
 *
 * @return NS_OK to success, otherwise NS_ERROR.
 */
NSResult NSInsertTopicNode(NSTopicLL *, NSTopicLL *);

/**
 * Copy topic list.
 *
 * @param NSTopicLL     head node of the list.
 *
 * @return new head node of list.
 */
NSTopicLL * NSCopyTopicLL(NSTopicLL *);

/**
 * Remove topic list.
 *
 * @param NSTopicLL    head node to remove.
 */
void NSRemoveTopicLL(NSTopicLL *);

/**
 * Invoke request.
 *
 * @param handle        OCDoHandle of resource
 * @param method        OCMethod type
 * @param addr          device address
 * @param queryUri      query uri
 * @param payload       oc payload
 * @param callbackFunc  callback function
 * @param callbackData  callback data
 * @param cd            oc client context deleter
 * @param type          oc connectivity type
 *
 * @return oc stack result
 */
OCStackResult NSInvokeRequest(OCDoHandle * handle,
        OCMethod method, const OCDevAddr * addr,
        const char * queryUrl, OCPayload * payload,
        void * callbackFunc, void * callbackData,
        OCClientContextDeleter cd, OCConnectivityType type);

/**
 * OC stack result.
 *
 * @param ret    return value.
 *
 * @return true if ret value found, otherwise false.
 */
bool NSOCResultToSuccess(OCStackResult ret);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _NS_CONSUMER_COMMON_H_
