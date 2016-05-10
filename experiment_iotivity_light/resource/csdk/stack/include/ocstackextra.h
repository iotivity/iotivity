//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
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
//

/**
 * @file
 *
 * This file contains APIs for OIC Stack to be implemented.
 */

#ifndef OCSTACK_EXTRA_H_
#define OCSTACK_EXTRA_H_

#include "ocresource.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * This function notify all registered observers that the resource representation has
 * changed. If observation includes a query the client is notified only if the query is valid after
 * the resource representation has changed.
 *
 * @param handle   Handle of resource.
 * @param qos      Desired quality of service for the observation notifications.
 *
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
OCStackResult OCNotifyAllObservers1(OCResource *resource, OCQualityOfService qos);

/**
 * Notify specific observers with updated value of representation.
 * Before this API is invoked by entity handler it has finished processing
 * queries for the associated observers.
 *
 * @param handle                    Handle of resource.
 * @param obsIdList                 List of observation IDs that need to be notified.
 * @param numberOfIds               Number of observation IDs included in obsIdList.
 * @param payload                   Object representing the notification
 * @param qos                       Desired quality of service of the observation notifications.
 *
 * @note: The memory for obsIdList and payload is managed by the entity invoking the API.
 * The maximum size of the notification is 1015 bytes for non-Arduino platforms. For Arduino
 * the maximum size is 247 bytes.
 *
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
OCStackResult OCNotifyListOfObservers1(OCResource *resource,
                                      OCObservationId  *obsIdList,
                                      uint8_t          numberOfIds,
                                      const OCRepPayload *payload,
                                      OCQualityOfService qos);

OCStackResult OCAddResources(OCResource *resource);

void OCNotifyPresenceChange(const OCResource *resource, OCPresenceTrigger trigger);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* OCSTACK_EXTRA_H_ */
