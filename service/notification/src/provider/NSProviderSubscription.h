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

#ifndef _NS_PROVIDER_SUBSCRIPTION_H_
#define _NS_PROVIDER_SUBSCRIPTION_H_

#include "experimental/logger.h"
#include "ocstack.h"
#include "ocpayload.h"
#include "NSCommon.h"
#include "NSConstants.h"
#include "NSProviderScheduler.h"
#include "NSProviderResource.h"
#include "NSProviderMemoryCache.h"
#include "NSProviderSystem.h"
#include "oic_string.h"
#include "oic_malloc.h"
#include <stdbool.h>

NSCacheList * consumerSubList;

/**
 * Initialize subscription list
 *
 * @return NS_OK if initialized, otherwise ERROR CODES.
 */
NSResult NSInitSubscriptionList(void);

/**
 * Set subscription access policy
 *
 * @param policy   access policy
 *
 * @return NS_OK if success, otherwise ERROR CODES.
 */
NSResult NSSetSubscriptionAccessPolicy(bool policy);

/**
 * Send access policy response
 *
 * @param entityHandlerRequest    request on entity handler
 *
 * @return NS_OK if success, otherwise ERROR CODES.
 */
NSResult NSSendAccessPolicyResponse(OCEntityHandlerRequest *entityHandlerRequest);

/**
 * Handle subscription
 *
 * @param entityHandlerRequest     request on entity hadler
 * @param resourceType             resource type
 */
void NSHandleSubscription(OCEntityHandlerRequest *entityHandlerRequest,
        NSResourceType resourceType);

/**
 * Handle Unsubscription
 *
 * @param entityHandlerRequest     request on entity hadler
 */
void NSHandleUnsubscription(OCEntityHandlerRequest *entityHandlerRequest);

/**
 * Acceptance from user
 *
 * @param entityHandlerRequest     request on entity hadler
 */
void NSAskAcceptanceToUser(OCEntityHandlerRequest *entityHandlerRequest);

/**
 * Send consumer subscription response
 *
 * @param entityHandlerRequest     request on entity hadler
 *
 * @return NS_OK if success, otherwise ERROR CODES.
 */
NSResult NSSendConsumerSubResponse(OCEntityHandlerRequest *entityHandlerRequest);

/**
 * Send response
 *
 * @param id        id to find in list
 * @param accepted  true or false value
 *
 * @return NS_OK if success, otherwise ERROR CODES.
 */
NSResult NSSendResponse(const char * id, bool accepted);

#endif /* _NS_PROVIDER_SUBSCRIPTION_H_ */
