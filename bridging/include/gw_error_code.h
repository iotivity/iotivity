//******************************************************************
//
// Copyright 2017 Intel Mobile Communications GmbH All Rights Reserved.
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

#ifndef __GW_ERROR_CODE_H__
#define __GW_ERROR_CODE_H__

#include <stdint.h>
/*
 * Definitions for error codes for the gateway
 */
typedef uint32_t GW_RESULT;

typedef uint32_t GW_CB_RESULT;

#define GW_CB_RESULT_KEEP                             1u
#define GW_CB_RESULT_DELETE                           2u
#define GW_CB_RESULT_OK                               3u
#define GW_CB_RESULT_ERROR                            4u

#define GW_RESULT_OK                                  0u
#define GW_RESULT_NOT_CREATED                         1u   /*< Module has not been created. */
#define GW_RESULT_CREATED_FAILED                      2u   /*< Module has failed during create. */
#define GW_RESULT_INVALID_HANDLE                      3u   /*< The handle is invalid. */
#define GW_RESULT_INVALID_PARAMETER                   4u   /*< Parameters are invalid. */
#define GW_RESULT_INTERNAL_ERROR                      5u   /*< Some unknown error occurred. */
#define GW_RESULT_INVALID_VERSION                     6u   /*< Invalid plugin API version. */
#define GW_RESULT_MISSING_API                         7u   /*< One or more expected API is missing in the callback. */
#define GW_RESULT_NOT_IMPLEMENTED                     8u   /*< API function is not implemented. */
#define GW_RESULT_OUT_OF_MEMORY                       9u   /*< Memory allocation failure. */
#define GW_RESULT_ERROR_ADD_MESSAGE_QUEUE             10u  /*< add message queue failure. */
#define GW_RESULT_LOADLIBRARY_FAILED                  11u
#define GW_RESULT_INSUFFICIENT_BUFFER                 12u
#define GW_RESULT_DUPLICATE_API_CALL                  13u
#define GW_RESULT_FILE_NOT_OPEN                       14u
#define GW_RESULT_FILE_ALREADY_OPEN                   15u
#define GW_RESULT_FILE_NOT_CLOSED                     16u
#define GW_RESULT_NOT_STARTED                         17u   /*< Module has not been started. */
#define GW_RESULT_STARTED_FAILED                      18u   /*< Module has failed during start. */
#define GW_RESULT_ALREADY_STARTED                     19u   /*< Module already started. */
#define GW_RESULT_NOT_STOPPED                         20u
#define GW_RESULT_ALREADY_CREATED                     21u   /*< Module already created. */
#define GW_RESULT_NOT_AUTHORIZED                      22u   /*< Not authorized */
#define GW_RESULT_NOT_PRESENT                         23u   /*< Not present or available */
#define GW_RESULT_ALREADY_AUTH                        24u
#define GW_RESULT_DEAUTH_FAILED                       25u
#define GW_RESULT_ALREADY_DEAUTH                      26u
#define GW_RESULT_NETWORK_ERROR                       27u
#define GW_RESULT_JSON_ERROR                          28u
#define GW_RESULT_MEMORY_ERROR                        29u
#define GW_RESULT_INVALID_DATA                        30u
#define GW_RESULT_INDEX_OUT_OF_BOUNDS                 31u   /*< Specified an index too great for array. */
#define GW_RESULT_UNEXPECTED_RESULT                   32u   /*< Result code did not match expected response */

#endif /* __GW_ERROR_CODE_H__ */
