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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdint.h>
#include <sstream>

#include "easysetupmgr.h"

//Use ipv4addr for both InitDiscovery and InitDeviceDiscovery
char ipv4addr[IPV4_ADDR_SIZE] = { 0 };

static OCProvisioningStatusCB cbData = NULL;

OCStackResult InitEasySetupManager() {
    OCStackResult result = OC_STACK_ERROR;

    if (InitProvisioningHandler() == OC_STACK_OK) {
        result = OC_STACK_OK;
        OIC_LOG(DEBUG, TAG, "InitProvisioningHandler returned Success");
    } else {
        result = OC_STACK_ERROR;
        OIC_LOG_V(ERROR, TAG, "InitProvisioningHandler returned error = %s",
                result);
    }

    return result;
}

OCStackResult TerminateEasySetupManager() {
    return TerminateProvisioningHandler();
}

OCStackResult RegisterProvisioningStausCallback(
        OCProvisioningStatusCB provisioningStatusCallback) {
    OCStackResult result = OC_STACK_OK;

    if(provisioningStatusCallback != NULL)
    {
        cbData = provisioningStatusCallback;
    }
    else
    {
        result = OC_STACK_ERROR;
        OIC_LOG(ERROR, TAG, "provisioningStatusCallback is NULL");
    }

    return result;
}

void UnRegisterProvisioningStausCallback() {
    if (cbData) {
        cbData = NULL;
    }
}

OCStackResult ProvisionEnrollee(const EnrolleeNWProvInfo_t *netInfo)
{
    return StartProvisioningProcess(netInfo, cbData);
}

OCStackResult StopEnrolleeProvisioning(OCConnectivityType connectivityType) {
    OCStackResult result = OC_STACK_OK;

    //TODO: Have to handle the transport specific easy setup termination
    StopProvisioningProcess();

    return result;
}

