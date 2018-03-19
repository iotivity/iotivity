/* *****************************************************************
 *
 * Copyright 2017 Microsoft
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

/**
 * @file
 *
 * This file defines the event object.
 */

#ifndef OC_EVENT_H_
#define OC_EVENT_H_

#include "iotivity_commontypes.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef struct oc_event_t *oc_event;

/**
 * Creates a new event.
 *
 * @return  Reference to newly created event, NULL on allocation failure.
 */
oc_event oc_event_new(void);

/**
 * Frees the oc_event.
 *
 * @param[in]  event  Optional event to be freed.
 */
void oc_event_free(oc_event event);

/**
 * Waits infinitely for the event to be signaled.
 *
 * @param[in]  event  Event to wait on.
 */
void oc_event_wait(oc_event event);

/**
 * Waits for the event to be signaled or timed out.
 *
 * @param[in]  event         Event to wait on.
 * @param[in]  milliseconds  Timeout in milliseconds.
 * @return  OC_WAIT_SUCCESS  Event was signaled before timeout expired.
 *          OC_WAIT_TIMEDOUT Timeout interval elapsed.
 */
OCWaitResult_t oc_event_wait_for(oc_event event, uint32_t milliseconds);

/**
 * Signals the event.
 *
 * @param[in]  event  Event to signal.
 */
void oc_event_signal(oc_event event);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OC_EVENT_H_ */
