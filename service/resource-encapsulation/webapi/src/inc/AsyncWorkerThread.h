/* *****************************************************************
*
* Copyright 2015 Samsung Electronics All Rights Reserved.
*
*----------------------------------------------------------------------
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
******************************************************************************/



/**
 * @file
 *
 * This file contains the declaration of worker thread to call javascript callback apis.
 */

#ifndef ASYNC_WORKER_THREAD_H
#define  ASYNC_WORKER_THREAD_H

#include "CommonUtil.h"
#include <vector>
#include <string>

using namespace std;

typedef enum CBType
{
    RESOURCE_DISCOVERED_CB = 0,
    RESOURCE_STATE_CHANGED_CB,
    RESOURCE_CACHE_UPDADED_CB,
    RESOURCE_ATTR_RECEIVED_CB,

} eCBType;

/**
* This structure will be used for resource status changed event.
*/
struct StatusEventWork
{
    uv_work_t  request;
    Persistent<Function> callback;
    std::string context;
    int status;
};

/**
* This structure will be used for resource discovered event.
*/

struct DiscoveryEventWork
{
    uv_work_t  request;
    Persistent<Function> callback;
    std::string context;
};

/**
*This structure will be used for cache updated and attr received event.
*/

struct AttrChangeEventWork
{
    uv_work_t  request;
    Persistent<Function> callback;
    std::string context;
    Local<Object> attrMap;
};


/**
 * This function is used to call javascript callback on
 * resource discovery
 *
 * @param[in]  req - uv_work_t internal libuv library structure for worker thread.
 * @param[in]  status - worker thread status
 *
 * @return none
 */
void WorkResourceDiscoveredAsyncComplete(uv_work_t *req, int status);


/**
 * This function is used to call javascript callback on
 * resource state change.
 *
 * @param[in]  req - uv_work_t internal libuv library structure for worker thread.
 * @param[in]  status -  worker thread status

 * @return none
 */

void WorkResourceStateChangedAsyncComplete(uv_work_t *req, int status);

/**
 * This function is used to call javascript callback on
 * cache update.
 *
 * @param[in]  uv_work_t internal libuv library structure for worker thread.
 * @param[in]  status   worker thread status

 * @return none
 */
void WorkCacheUpdatedAsyncComplete(uv_work_t *req, int status);


/**
 * This function is used to call javascript callback on attributes
 * received complete.
 *
 * @param[in]  uv_work_t internal libuv library structure for worker thread.
 * @param[in]  status   worker thread status

 * @return none
 */
void WorkAttributesReceivedAsyncComplete(uv_work_t *req, int status);

/**
 * This function is for asynchornous work
 *
 * @param[in]  uv_work_t internal libuv library structure for worker thread.
 *
 * @return none
 */
void WorkAsync(uv_work_t *req);


#endif //ASYNC_WORKER_THREAD_H
