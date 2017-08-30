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

#ifndef _MPMERRORCODE_H_
#define _MPMERRORCODE_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Definitions for error codes for the Bridging
 */

typedef enum
{
    MPM_CB_RESULT_KEEP   = 1,
    MPM_CB_RESULT_DELETE = 2,
    MPM_CB_RESULT_OK     = 3,
    MPM_CB_RESULT_ERROR  = 4
} MPMCbResult;

typedef enum
{
    MPM_RESULT_OK = 0,
    MPM_RESULT_CREATED_FAILED,            /*< Module has failed during create. */

    MPM_RESULT_INVALID_HANDLE,            /*< The handle is invalid. */
    MPM_RESULT_INVALID_PARAMETER,         /*< Parameters are invalid. */
    MPM_RESULT_INTERNAL_ERROR,            /*< Some unknown error occurred. */
    MPM_RESULT_INVALID_VERSION,           /*< Invalid plugin API version. */

    MPM_RESULT_MISSING_API,               /*< One or more expected API is missing in the callback. */
    MPM_RESULT_NOT_IMPLEMENTED,           /*< API function is not implemented. */
    MPM_RESULT_OUT_OF_MEMORY,             /*< Memory allocation failure. */
    MPM_RESULT_ERROR_ADD_MESSAGE_QUEUE,   /*< add message queue failure. */
    MPM_RESULT_LOADLIBRARY_FAILED,
    MPM_RESULT_INSUFFICIENT_BUFFER,
    MPM_RESULT_DUPLICATE_API_CALL,
    MPM_RESULT_FILE_NOT_OPEN,
    MPM_RESULT_FILE_ALREADY_OPEN,
    MPM_RESULT_FILE_NOT_CLOSED,
    MPM_RESULT_NOT_STARTED,               /*< Module has not been started. */
    MPM_RESULT_STARTED_FAILED,            /*< Module has failed during start. */
    MPM_RESULT_ALREADY_STARTED,           /*< Module already started. */
    MPM_RESULT_NOT_STOPPED,
    MPM_RESULT_ALREADY_CREATED,           /*< Module already created. */
    MPM_RESULT_NOT_AUTHORIZED,            /*< Not authorized */
    MPM_RESULT_NOT_PRESENT,               /*< Not present or available */
    MPM_RESULT_NETWORK_ERROR,
    MPM_RESULT_JSON_ERROR,
    MPM_RESULT_MEMORY_ERROR,
    MPM_RESULT_INVALID_DATA,
    MPM_RESULT_INDEX_OUT_OF_BOUNDS,       /*< Specified an index too great for array. */
    MPM_RESULT_UNEXPECTED_RESULT          /*< Result code did not match expected response */
} MPMResult;

#ifdef __cplusplus
}
#endif // #ifdef __cpluscplus

#endif /* _MPMERRORCODEH_ */
