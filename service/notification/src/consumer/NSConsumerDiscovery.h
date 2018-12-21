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

#ifndef _NS_CONSUMER_DISCOVERY_H_
#define _NS_CONSUMER_DISCOVERY_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdlib.h>

#include "ocstack.h"
#include "NSStructs.h"

/**
 * Consumer discovery task processing
 *
 * @param notification task information
 */
void NSConsumerDiscoveryTaskProcessing(NSTask *);

/**
 * Notification consumer presence listener
 *
 * @param ctx            void pointer
 * @param handle         OCDoHandle
 * @param clientResponse client response
 *
 * @return oc stack application result
 */
OCStackApplicationResult NSConsumerPresenceListener(void *, OCDoHandle, OCClientResponse *);

/**
 * Provider discover listener
 *
 * @param ctx            void pointer
 * @param handle         OCDoHandle
 * @param clientResponse client response
 *
 * @return oc stack application result
 */
OCStackApplicationResult NSProviderDiscoverListener(void *, OCDoHandle, OCClientResponse *);

/**
 * Introspect provider
 *
 * @param ctx            void pointer
 * @param handle         OCDoHandle
 * @param clientResponse client response
 *
 * @return oc stack application result
 */
OCStackApplicationResult NSIntrospectProvider(void *, OCDoHandle, OCClientResponse *);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _NS_CONSUMER_DISCOVERY_H_
