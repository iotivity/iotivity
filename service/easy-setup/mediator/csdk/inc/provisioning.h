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

#ifndef __ES_PROVISIONING_HANDLER_H_
#define __ES_PROVISIONING_HANDLER_H_

#include "octypes.h"

#include "logger.h"
#include "ocstack.h"
#include "escommon.h"

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------
#define EASY_SETUP_DEFAULT_CONTEXT_VALUE 0x99

/**
 * List of methods that can be inititated from the client
 */
OCStackResult InitProvisioningHandler();

OCStackResult TerminateProvisioningHandler();

OCStackResult GetProvisioningStatus(OCQualityOfService qos, const char *query,
        const OCDevAddr *destination);

OCStackResult StartProvisioningProcess(const ProvConfig *netInfo,
        WiFiOnboadingConnection *onboardConn, OCProvisioningStatusCB provisioningStatusCallback,
        char *findResQuery);

void StopProvisioningProcess();

/**
 * Internal worker functions.
 */
OCStackResult TriggerNetworkConnection(OCQualityOfService qos, const char *query,
        const char *resUri, OCDevAddr *destination, int /*pauseBeforeStart*/);

OCStackApplicationResult TriggerNetworkConnectionResponse(void *ctx, OCDoHandle handle,
        OCClientResponse *clientResponse);

OCStackApplicationResult ProvisionEnrolleeResponse(void *ctx, OCDoHandle handle,
        OCClientResponse *clientResponse);

OCStackResult ProvisionEnrollee(OCQualityOfService qos, const char *query, const char *resUri,
        OCDevAddr *destination, int pauseBeforeStart);

OCStackApplicationResult GetProvisioningStatusResponse(void *ctx, OCDoHandle handle,
        OCClientResponse *clientResponse);

OCStackResult InvokeOCDoResource(const char *query, OCMethod method, const OCDevAddr *dest,
        OCQualityOfService qos, OCClientResponseHandler cb, OCRepPayload *payload,
        OCHeaderOption *options, uint8_t numOptions);

OCStackApplicationResult FindProvisioningResourceResponse(void *ctx, OCDoHandle handle,
        OCClientResponse *clientResponse);

void *FindProvisioningResource(void *data);

ProvisioningInfo *PrepareProvisioingStatusCB(OCClientResponse *clientResponse,
        ProvStatus provStatus);

/**
 * Internal Util functions.
 */
void LogProvisioningResponse(OCRepPayloadValue * val);

bool ConfigEnrolleeObject(const ProvConfig *netInfo, WiFiOnboadingConnection *onboardConn);

bool ClearMemory();

void SuccessCallback(OCClientResponse * clientResponse);

void ErrorCallback(ProvStatus status);

bool ValidateEnrolleeResponse(OCClientResponse * clientResponse);

bool ValidateFindResourceResponse(OCClientResponse * clientResponse);

bool ValidateEnrolleeBasicResponse(OCClientResponse * clientResponse);

ProvisioningInfo *GetCallbackObjectOnSuccess(OCClientResponse *clientResponse,
        ProvStatus provStatus);

ProvisioningInfo *GetCallbackObjectOnError(ProvStatus status);

ProvisioningInfo *CreateCallBackObject();

bool ResetProgress();

bool SetProgress(OCProvisioningStatusCB provisioningStatusCallback);

bool InProgress();

bool ValidateEasySetupParams(const ProvConfig *netInfo, WiFiOnboadingConnection *onboardConn,
        OCProvisioningStatusCB provisioningStatusCallback);

bool IsSetupStopped();

#endif //__ES_PROVISIONING_HANDLER_H_

