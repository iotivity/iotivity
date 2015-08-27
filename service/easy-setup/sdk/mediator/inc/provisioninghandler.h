//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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


#ifndef __PROVISIONING_HANDLER_H_
#define __PROVISIONING_HANDLER_H_


#include "octypes.h"

#include "logger.h"
#include "ocstack.h"
#include "common.h"

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------
#define TAG "provisioninghandler"
#define DEFAULT_CONTEXT_VALUE 0x99
#ifndef MAX_LENGTH_IPv4_ADDR
#define MAX_LENGTH_IPv4_ADDR 16
#endif

//-----------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------

/**
 * List of methods that can be inititated from the client
 */
OCStackResult InitProvisioningHandler();

OCStackResult TerminateProvisioningHandler();

void listeningFunc(void*);

OCStackApplicationResult ProvisionEnrolleeResponse(void* ctx, OCDoHandle handle,
        OCClientResponse * clientResponse);

OCStackResult ProvisionEnrollee(OCQualityOfService qos, const char* query, const char* resUri);

OCStackApplicationResult GetProvisioningStatusResponse(void* ctx,
        OCDoHandle handle, OCClientResponse * clientResponse);

OCStackResult InvokeOCDoResource(const char* query, OCMethod method,
        OCQualityOfService qos, OCClientResponseHandler cb,OCRepPayload * request,
        OCHeaderOption * options, uint8_t numOptions);

OCStackResult GetProvisioningStatus(OCQualityOfService qos, const char* query);

OCStackResult StartProvisioningProcess(const EnrolleeNWProvInfo_t *netInfo,
        OCProvisioningStatusCB provisioningStatusCallback);


void StopProvisioningProcess();

OCStackApplicationResult SubscribeProvPresenceCallback(void* ctx, OCDoHandle handle,
        OCClientResponse* clientResponse);

OCStackResult SubscribeProvPresence(OCQualityOfService qos, const char* requestURI);

OCStackApplicationResult FindProvisioningResourceResponse(void* ctx,
        OCDoHandle handle, OCClientResponse * clientResponse);

void FindProvisioningResource(void *data);

//Invoke Provisioning Status Callback
ProvisioningInfo* PrepareProvisioingStatusCB(OCClientResponse * clientResponse,
            ProvStatus provStatus);

#endif

