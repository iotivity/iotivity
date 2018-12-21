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

#ifndef _NS_PROVIDER_LISTENER__H_
#define _NS_PROVIDER_LISTENER__H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <octypes.h>
#include "ocstack.h"
#include "experimental/logger.h"
#include "ocpayload.h"
#include "NSStructs.h"
#include "NSConstants.h"
#include "NSProviderSystem.h"
#include "NSProviderScheduler.h"
#include "cautilinterface.h"
#include "oic_string.h"
#include "oic_malloc.h"
#include "NSUtil.h"
#include "NSProviderMemoryCache.h"

/**
 * Notification entity handler callback.
 * @param[in] flag                   entity handler falg
 * @param[in] entityHandlerRequest   entity handler request to handle the incomming requests
 * @param[in] callback               callback function
 * @return entity handler result
 */
OCEntityHandlerResult NSEntityHandlerNotificationCb(OCEntityHandlerFlag flag,
        OCEntityHandlerRequest *entityHandlerRequest, void* callback);

/**
 * Message entity handler callback.
 * @param[in] flag                  entity handler falg
 * @param[in] entityHandlerRequest  entity handler request to handle the incomming requests
 * @param[in] callback              callback function
 * @return entity handler result
 */
OCEntityHandlerResult NSEntityHandlerMessageCb(OCEntityHandlerFlag flag,
        OCEntityHandlerRequest *entityHandlerRequest, void* callback);

/**
 * Sync entity handler callback.
 * @param[in] flag                  entity handler falg
 * @param[in] entityHandlerRequest  entity handler request to handle the incomming requests
 * @param[in] callback              callback function
 * @return entity handler result
 */
OCEntityHandlerResult NSEntityHandlerSyncCb(OCEntityHandlerFlag flag,
        OCEntityHandlerRequest *entityHandlerRequest, void* callback);

/**
 * Topic entity handler callback.
 * @param[in] flag                  entity handler falg
 * @param[in] entityHandlerRequest  entity handler request to handle the incomming requests
 * @param[in] callback              callback function
 * @return entity handler result
 */
OCEntityHandlerResult NSEntityHandlerTopicCb(OCEntityHandlerFlag flag,
        OCEntityHandlerRequest *entityHandlerRequest, void* callback);

/**
 * provider connection state listener.
 * @param[in] info           provider connection information
 * @param[in] isConnected    connection state
 */
void NSProviderConnectionStateListener(const CAEndpoint_t * info, bool isConnected);

/**
 * provider adapter state listener.
 * @param[in] adapter     transport adapter
 * @param[in] enabled     connection state
 */
void NSProviderAdapterStateListener(CATransportAdapter_t adapter, bool enabled);

#ifdef WITH_MQ
/**
 * get provider MQ response callback.
 * @param[in] handle            dohandle
 * @param[in] clientResponse    oc client response
 * @return oc stack application result
 */
OCStackApplicationResult NSProviderGetMQResponseCB(void * ctx, OCDoHandle handle,
        OCClientResponse * clientResponse);

/**
 * publish provider MQ response callback.
 * @param[in] handle            dohandle
 * @param[in] clientResponse    oc client response
 * @return oc stack application result
 */
OCStackApplicationResult NSProviderPublishMQResponseCB(void *ctx, OCDoHandle handle,
        OCClientResponse *clientResponse);
#endif

#endif /* _NS_PROVIDER_LISTENER__H_ */

#ifdef __cplusplus
}
#endif // __cplusplus
