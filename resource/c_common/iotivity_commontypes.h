/* ****************************************************************
 *
 * Copyright 2017 Microsoft
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

/**
 * @file
 *
 * This file provides generic types used in the c_common layer.
 */

#ifndef IOTIVITY_COMMON_TYPES_H_
#define IOTIVITY_COMMON_TYPES_H_

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * Enums for oc_cond_wait_for and oc_event_wait_for return values.
 */
typedef enum
{
   OC_WAIT_SUCCESS = 0,    /**< Condition or event is signaled. */
   OC_WAIT_INVAL = -1,     /**< Condition or event is invalid. */
   OC_WAIT_TIMEDOUT = -2   /**< Condition or event is timed out. */
} OCWaitResult_t;

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* IOTIVITY_COMMON_TYPES_H_ */