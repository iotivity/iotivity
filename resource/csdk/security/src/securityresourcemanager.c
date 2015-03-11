//******************************************************************
//
// Copyright 2015 Intel Mobile Communications GmbH All Rights Reserved.
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

#include "ocstack.h"
#include "logger.h"
#include "cainterface.h"
#include "securityresourcemanager.h"

#define TAG  PCF("SRM")

//Request Callback handler
static CARequestCallback gRequestHandler = NULL;
//Response Callback handler
static CAResponseCallback gResponseHandler = NULL;
//Persistent Storage callback handler for open/read/write/close/unlink
static OCPersistentStorage *gPersistentStorageHandler = NULL;

/**
 * @brief   Handle the request from the SRM.
 * @param   endPoint       [IN] Endpoint object from which the response is received.
 * @param   requestInfo    [IN] Information for the request.
 * @return  NONE
 */
void SRMRequestHandler(const CARemoteEndpoint_t *endPoint, const CARequestInfo_t *requestInfo)
{
    OC_LOG(INFO, TAG, PCF("Received request from remote device"));
    if (gRequestHandler)
    {
        gRequestHandler(endPoint, requestInfo);
    }
}

/**
 * @brief   Handle the response from the SRM.
 * @param   endPoint     [IN] The remote endpoint.
 * @param   responseInfo [IN] Response information from the endpoint.
 * @return  NONE
 */
void SRMResponseHandler(const CARemoteEndpoint_t *endPoint, const CAResponseInfo_t *responseInfo)
{
    OC_LOG(INFO, TAG, PCF("Received response from remote device"));
    if (gResponseHandler)
    {
        gResponseHandler(endPoint, responseInfo);
    }
}

/**
 * @brief   Register request and response callbacks.
 *          Requests and responses are delivered in these callbacks.
 * @param   reqHandler   [IN] Request handler callback ( for GET,PUT ..etc)
 * @param   respHandler  [IN] Response handler callback.
 * @return
 *     OC_STACK_OK    - No errors; Success
 *     OC_STACK_INVALID_PARAM - invalid parameter
 */
OCStackResult SRMRegisterHandler(CARequestCallback reqHandler, CAResponseCallback respHandler)
{
    OC_LOG(INFO, TAG, PCF("SRMRegisterHandler !!"));
    if(!reqHandler)
    {
        OC_LOG(ERROR, TAG, PCF("ReqHandler is invalid"));
        return OC_STACK_INVALID_PARAM;
    }
    if(!respHandler)
    {
        OC_LOG(ERROR, TAG, PCF("RespHandler is invalid"));
        return OC_STACK_INVALID_PARAM;
    }
    gRequestHandler = reqHandler;
    gResponseHandler = respHandler;

    CARegisterHandler(SRMRequestHandler, SRMResponseHandler);
    return OC_STACK_OK;
}

/**
 * @brief   Register Persistent storage callback.
 * @param   persistentStorageHandler [IN] Pointers to open, read, write, close & unlink handlers.
 * @return
 *     OC_STACK_OK    - No errors; Success
 *     OC_STACK_INVALID_PARAM - Invalid parameter
 */
OCStackResult SRMRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
{
    OC_LOG(INFO, TAG, PCF("SRMRegisterPersistentStorageHandler !!"));
    if(!persistentStorageHandler)
    {
        OC_LOG(ERROR, TAG, PCF("The persistent storage handler is invalid"));
        return OC_STACK_INVALID_PARAM;
    }
    gPersistentStorageHandler = persistentStorageHandler;
    return OC_STACK_OK;
}

/**
 * @brief   Get Persistent storage handler pointer.
 * @return
 *     The pointer to Persistent Storage callback handler
 */

OCPersistentStorage* SRMGetPersistentStorageHandler()
{
    OC_LOG(INFO, TAG, PCF("SRMGetPersistentStorageHandler !!"));
    return gPersistentStorageHandler;
}
