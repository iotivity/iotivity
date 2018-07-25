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

#ifndef _NS_PROVIDER_RESOURCE_H_
#define _NS_PROVIDER_RESOURCE_H_

#include <ocstack.h>
#include "experimental/logger.h"
#include "NSCommon.h"
#include "NSProviderListener.h"
#include "NSProviderSystem.h"
#include "oic_malloc.h"
#include "oic_string.h"

#define NS_CREATE_RESOURCE(func, obj) \
    { \
        NSResult _ret = (func); \
        if ( _ret != NS_OK) \
        { \
            NS_LOG_V(ERROR, "%s : %s", __func__, #obj); \
            return (NS_ERROR); \
        } \
    }

#define NS_DELETE_RESOURCE(func, obj) \
    { \
        OCStackResult _ret = (func); \
        if ( _ret != OC_STACK_OK) \
        { \
            NS_LOG_V(ERROR, "%s : %s", __func__, #obj); \
            return (NS_ERROR); \
        } \
    }

/**
 * Create resource.
 *
 * @param uri     resource URI.
 *
 * @return OK if success, otherwise ERROR.
 */
NSResult NSCreateResource(char *uri);

/**
 * Publish resource to cloud.
 *
 * @param host    server address.
 *
 * @return OK if success.
 */
NSResult NSPublishResourceToCloud(char *host);

/**
 * Register resource.
 *
 * @return OK if success, otherwise ERROR.
 */
NSResult NSRegisterResource(void);

/**
 * Remove resource.
 *
 * @return OK if success, otherwise ERROR.
 */
NSResult NSUnRegisterResource(void);

/**
 * Put notification resource.
 *
 * @param[in] policy    notification policy
 * @param[in] handle    resource handler
 *
 * @return OK if success, otherwise ERROR.
 */
NSResult NSPutNotificationResource(bool policy, OCResourceHandle * handle);

/**
 * Put message resource.
 *
 * @param[in] msg      notification message
 * @param[in] handle   resource handler
 *
 * @return OK if success, otherwise ERROR.
 */
NSResult NSPutMessageResource(NSMessage *msg, OCResourceHandle * handle);

/**
 * Put sync resource.
 *
 * @param[in] sync     notification sync inforamtion
 * @param[in] handle   resource handler
 *
 * @return success code.
 */
NSResult NSPutSyncResource(NSSyncInfo *sync, OCResourceHandle * handle);

/**
 * Put topic resource.
 *
 * @param[in] topicList   notification topic list
 * @param[in] handle      resource handler
 *
 * @return success code.
 */
NSResult NSPutTopicResource(NSTopicList *topicList, OCResourceHandle * handle);

#endif /* _NS_PROVIDER_RESOURCE_H_ */
