/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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

#ifndef SIMULATOR_ERROR_CODES_H_
#define SIMULATOR_ERROR_CODES_H_

#include <iostream>

typedef enum
{
    /** STACK error codes - START */
    SIMULATOR_OK = 0,                      /**< Success */
    SIMULATOR_RESOURCE_CREATED,            /**< Resource created */
    SIMULATOR_RESOURCE_DELETED,            /**< Resource deleted */
    SIMULATOR_CONTINUE,                    /**< Continue */
    SIMULATOR_RESOURCE_CHANGED,            /**< Resource changed */
    SIMULATOR_INVALID_URI = 20,            /**< Invalid URI */
    SIMULATOR_INVALID_QUERY,               /**< Invalid query */
    SIMULATOR_INVALID_IP,                  /**< Invalid IP */
    SIMULATOR_INVALID_PORT,                /**< Invalid port */
    SIMULATOR_INVALID_CALLBACK,            /**< Invalid callback */
    SIMULATOR_INVALID_METHOD,              /**< Invalid method */
    SIMULATOR_INVALID_PARAM,               /**< Invalid parameter */
    SIMULATOR_INVALID_OBSERVE_PARAM,       /**< Invalid observe parameter */
    SIMULATOR_NO_MEMORY,                   /**< No memory */
    SIMULATOR_COMM_ERROR,                  /**< Comm error */
    SIMULATOR_TIMEOUT,                     /**< Timeout */
    SIMULATOR_ADAPTER_NOT_ENABLED,         /**< Adapter not enabled */
    SIMULATOR_NOTIMPL,                     /**< Not impl */
    SIMULATOR_NO_RESOURCE,                 /**< No resource */
    SIMULATOR_RESOURCE_ERROR,              /**< Resource error */
    SIMULATOR_SLOW_RESOURCE,               /**< Slow resource */
    SIMULATOR_DUPLICATE_REQUEST,           /**< Duplicate request */
    SIMULATOR_NO_OBSERVERS,                /**< No observers */
    SIMULATOR_OBSERVER_NOT_FOUND,          /**< Observer not found */
    SIMULATOR_VIRTUAL_DO_NOT_HANDLE,       /**< Virtual do not handle */
    SIMULATOR_INVALID_OPTION,              /**< Invalid option */
    SIMULATOR_MALFORMED_RESPONSE,          /**< Malformed response */
    SIMULATOR_PERSISTENT_BUFFER_REQUIRED,  /**< Persistent buffer required */
    SIMULATOR_INVALID_REQUEST_HANDLE,      /**< Invalid request handle */
    SIMULATOR_INVALID_DEVICE_INFO,         /**< Invalid device information */
    SIMULATOR_INVALID_JSON,                /**< Invalid json */
    SIMULATOR_UNAUTHORIZED_REQ,            /**< Unauthorized request */
    SIMULATOR_TOO_LARGE_REQ,               /**< Large request */
    SIMULATOR_PDM_IS_NOT_INITIALIZED,      /**< PDM is not initialized */
    SIMULATOR_DUPLICATE_UUID,              /**< Duplicate UUID */
    SIMULATOR_INCONSISTENT_DB,             /**< Inconsistent DB */
    SIMULATOR_AUTHENTICATION_FAILURE,      /**< Authentication failure */
#ifdef WITH_PRESENCE
    SIMULATOR_PRESENCE_STOPPED = 128,      /**< Presence stopped */
    SIMULATOR_PRESENCE_TIMEOUT,            /**< Presence timeout */
    SIMULATOR_PRESENCE_DO_NOT_HANDLE,      /**< Presence do not handle */
#endif
    /** STACK error codes - END */

    /** Simulator specific error codes - START */
    SIMULATOR_INVALID_TYPE,                /**< Invalid type */
    SIMULATOR_NOT_SUPPORTED,               /**< Not supported */
    SIMULATOR_OPERATION_NOT_ALLOWED,       /**< Operation not allowed */
    SIMULATOR_OPERATION_IN_PROGRESS,       /**< Operation in progress */

    SIMULATOR_INVALID_RESPONSE_CODE,       /**< Invalid response code */
    SIMULATOR_UKNOWN_PROPERTY,             /**< Unknown property */
    SIMULATOR_TYPE_MISMATCH,               /**< Type mismatch */
    SIMULATOR_BAD_VALUE,                   /**< Bad value */
    SIMULATOR_BAD_OBJECT,                  /**< Bad object */
    SIMULATOR_BAD_SCHEMA,                  /**< Bad schema */
    /** Simulator specific error codes - END */

    SIMULATOR_ERROR = 255                  /**< Error */
} SimulatorResult;
#endif //SIMULATOR_ERROR_CODES_H_
