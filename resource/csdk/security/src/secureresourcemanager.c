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
#include "secureresourcemanager.h"
#include "resourcemanager.h"
#include "credresource.h"
#include "policyengine.h"
#include <string.h>

#define TAG  PCF("SRM")

//Request Callback handler
static CARequestCallback gRequestHandler = NULL;
//Response Callback handler
static CAResponseCallback gResponseHandler = NULL;
//Error Callback handler
static CAErrorCallback gErrorHandler = NULL;
//Persistent Storage callback handler for open/read/write/close/unlink
static OCPersistentStorage *gPersistentStorageHandler =  NULL;
//Provisioning response callback
static SPResponseCallback gSPResponseHandler = NULL;

/**
 * A single global Policy Engine context will suffice as long
 * as SRM is single-threaded.
 */
PEContext_t g_policyEngineContext;

/**
 * @brief function to register provisoning API's response callback.
 * @param respHandler response handler callback.
 */
void SRMRegisterProvisioningResponseHandler(SPResponseCallback respHandler)
{
    gSPResponseHandler = respHandler;
}
/**
 * @brief   Handle the request from the SRM.
 * @param   endPoint       [IN] Endpoint object from which the response is received.
 * @param   requestInfo    [IN] Information for the request.
 * @return  NONE
 */
void SRMRequestHandler(const CAEndpoint_t *endPoint, const CARequestInfo_t *requestInfo)
{
    OC_LOG(INFO, TAG, PCF("Received request from remote device"));

    if (!endPoint || !requestInfo)
    {
        OC_LOG(ERROR, TAG, PCF("Invalid arguments"));
        return;
    }

    // Copy the subjectID
    OicUuid_t subjectId = {};
    memcpy(subjectId.id, endPoint->identity.id, sizeof(subjectId.id));

    //Check the URI has the query and skip it before checking the permission
    char *uri = strstr(requestInfo->info.resourceUri, "?");
    int position = 0;
    if (uri)
    {
        position = uri - requestInfo->info.resourceUri;
    }
    if (position > MAX_URI_LENGTH)
    {
        OC_LOG(ERROR, TAG, PCF("URI length is too long"));
        return;
    }
    SRMAccessResponse_t response = ACCESS_DENIED;
    if (position > 0)
    {
        char newUri[MAX_URI_LENGTH + 1];
        strncpy(newUri, requestInfo->info.resourceUri, (position));
        newUri[position] = '\0';
        //Skip query and pass the newUri.
        response = CheckPermission(&g_policyEngineContext, &subjectId,
                              newUri,
                              GetPermissionFromCAMethod_t(requestInfo->method));
    }
    else
    {
        //Pass resourceUri if there is no query info.
        response = CheckPermission(&g_policyEngineContext, &subjectId,
                              requestInfo->info.resourceUri,
                              GetPermissionFromCAMethod_t(requestInfo->method));
    }
    if (IsAccessGranted(response) && gRequestHandler)
    {
        return (gRequestHandler(endPoint, requestInfo));
    }

    // Form a 'access deny' or 'Error' response and send to peer
    CAResponseInfo_t responseInfo = {};
    memcpy(&responseInfo.info, &(requestInfo->info), sizeof(responseInfo.info));
    responseInfo.info.payload = NULL;
    if (!gRequestHandler)
    {
        responseInfo.result = CA_INTERNAL_SERVER_ERROR;
    }
    else
    {
        /*
         * TODO Enhance this logic more to decide between
         * CA_UNAUTHORIZED_REQ or CA_FORBIDDEN_REQ depending
         * upon SRMAccessResponseReasonCode_t
         */
        responseInfo.result = CA_UNAUTHORIZED_REQ;
    }

    if (CA_STATUS_OK != CASendResponse(endPoint, &responseInfo))
    {
        OC_LOG(ERROR, TAG, PCF("Failed in sending response to a unauthorized request!"));
    }
}

/**
 * @brief   Handle the response from the SRM.
 * @param   endPoint     [IN] The remote endpoint.
 * @param   responseInfo [IN] Response information from the endpoint.
 * @return  NONE
 */
void SRMResponseHandler(const CAEndpoint_t *endPoint, const CAResponseInfo_t *responseInfo)
{
    OC_LOG(INFO, TAG, PCF("Received response from remote device"));

    // isProvResponse flag is to check whether response is catered by provisioning APIs or not.
    // When token sent by CA response matches with token generated by provisioning request,
    // gSPResponseHandler returns true and response is not sent to RI layer. In case
    // gSPResponseHandler is null and isProvResponse is false response then the response is for
    // RI layer.
    bool isProvResponse = false;

    if (gSPResponseHandler)
    {
        isProvResponse = gSPResponseHandler(endPoint, responseInfo);
    }
    if (!isProvResponse && gResponseHandler)
    {
        gResponseHandler(endPoint, responseInfo);
    }
}


/**
 * @brief   Handle the error from the SRM.
 * @param   endPoint  [IN] The remote endpoint.
 * @param   errorInfo [IN] Error information from the endpoint.
 * @return  NONE
 */
void SRMErrorHandler(const CAEndpoint_t *endPoint, const CAErrorInfo_t *errorInfo)
{
    OC_LOG(INFO, TAG, PCF("Received error from remote device"));
    if (gErrorHandler)
    {
        gErrorHandler(endPoint, errorInfo);
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
OCStackResult SRMRegisterHandler(CARequestCallback reqHandler,
                                 CAResponseCallback respHandler,
                                 CAErrorCallback errHandler)
{
    OC_LOG(INFO, TAG, PCF("SRMRegisterHandler !!"));
    if( !reqHandler || !respHandler || !errHandler)
    {
        OC_LOG(ERROR, TAG, PCF("Callback handlers are invalid"));
        return OC_STACK_INVALID_PARAM;
    }
    gRequestHandler = reqHandler;
    gResponseHandler = respHandler;
    gErrorHandler = errHandler;


#if defined(__WITH_DTLS__)
    CARegisterHandler(SRMRequestHandler, SRMResponseHandler, SRMErrorHandler);
#else
    CARegisterHandler(reqHandler, respHandler, errHandler);
#endif /* __WITH_DTLS__ */
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


/**
 * @brief   Initialize all secure resources ( /oic/sec/cred, /oic/sec/acl, /oic/sec/pstat etc).
 * @retval  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult SRMInitSecureResources()
{
    // TODO: temporarily returning OC_STACK_OK every time until default
    // behavior (for when SVR DB is missing) is settled.
    InitSecureResources();

#if defined(__WITH_DTLS__)
    CARegisterDTLSCredentialsHandler(GetDtlsPskCredentials);
#endif // (__WITH_DTLS__)

    return OC_STACK_OK;
}

/**
 * @brief   Perform cleanup for secure resources ( /oic/sec/cred, /oic/sec/acl, /oic/sec/pstat etc).
 * @retval  none
 */
void SRMDeInitSecureResources()
{
    DestroySecureResources();
}

/**
 * @brief   Initialize Policy Engine.
 * @return  OC_STACK_OK for Success, otherwise some error value.
 */
OCStackResult SRMInitPolicyEngine()
{
    return InitPolicyEngine(&g_policyEngineContext);
}

/**
 * @brief   Cleanup Policy Engine.
 * @return  none
 */
void SRMDeInitPolicyEngine()
{
    return DeInitPolicyEngine(&g_policyEngineContext);
}
