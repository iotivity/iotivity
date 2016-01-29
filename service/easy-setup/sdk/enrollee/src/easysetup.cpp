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

/**
 * @file
 *
 * This file contains the implementation for EasySetup Enrollee device
 */

#include "easysetup.h"
#include "softap.h"
#include "onboarding.h"
#include "logger.h"
#include "resourceHandler.h"

/**
 * @var ES_ENROLLEE_TAG
 * @brief Logging tag for module name.
 */
#define ES_ENROLLEE_TAG "ES"

//-----------------------------------------------------------------------------
// Private variables
//-----------------------------------------------------------------------------

/**
 * @var targetSsid
 * @brief Target SSID of the Soft Access point to which the device has to connect
 */
static char *targetSsid;

/**
 * @var targetPass
 * @brief Password of the target access point to which the device has to connect
 */
static char *targetPass;

/**
 * @var gEnrolleeStatusCb
 * @brief Fucntion pointer holding the callback for intimation of EasySetup Enrollee status callback
 */
static EventCallback gEnrolleeStatusCb = NULL;

//-----------------------------------------------------------------------------
// Private internal function prototypes
//-----------------------------------------------------------------------------
void OnboardingCallback(ESResult esResult);
void ProvisioningCallback(ESResult esResult);
void OnboardingCallbackTargetNet(ESResult esResult);
bool validateParam(OCConnectivityType networkType, const char *ssid, const char *passwd,
              EventCallback cb);


void OnboardingCallback(ESResult esResult)
{
        OC_LOG_V(DEBUG, ES_ENROLLEE_TAG, "OnboardingCallback with  result = %d", esResult);
        if(esResult == ES_OK)
        {
            gEnrolleeStatusCb(esResult, ES_ON_BOARDED_STATE);
        }
        else
        {
            OC_LOG_V(DEBUG, ES_ENROLLEE_TAG, 
                        "Onboarding is failed callback result is = %d", esResult);
            gEnrolleeStatusCb(esResult, ES_INIT_STATE);
        }
}

void ProvisioningCallback(ESResult esResult)
{
    OC_LOG_V(DEBUG, ES_ENROLLEE_TAG, "ProvisioningCallback with  result = %d", esResult);
    ESResult res = ES_OK;
    if (esResult == ES_RECVTRIGGEROFPROVRES)
    {
        targetSsid = (char *) malloc(MAXSSIDLEN);
        targetPass = (char *) malloc(MAXNETCREDLEN);

        GetTargetNetworkInfoFromProvResource(targetSsid, targetPass);
        gEnrolleeStatusCb(ES_OK, ES_PROVISIONED_STATE);
        OC_LOG(DEBUG, ES_ENROLLEE_TAG, "Connecting with target network");

        // Connecting/onboarding to target network
        ConnectToWiFiNetwork(targetSsid, targetPass, OnboardingCallbackTargetNet);
    }
    else
    {
       OC_LOG_V(DEBUG, ES_ENROLLEE_TAG, "Provisioning is failed callback result is = %d", esResult);
       // Resetting Enrollee to ONBOARDED_STATE as Enrollee is alreday onboarded in previous step
       gEnrolleeStatusCb(ES_OK, ES_ON_BOARDED_STATE);
    }
}

void OnboardingCallbackTargetNet(ESResult esResult)
{
    OC_LOG_V(DEBUG, ES_ENROLLEE_TAG, "OnboardingCallback on target network with result = %d",
                                                                                        esResult);
    if(esResult == ES_OK)
    {
        gEnrolleeStatusCb(esResult, ES_ON_BOARDED_TARGET_NETWORK_STATE);
    }
    else
    {
        OC_LOG_V(DEBUG, ES_ENROLLEE_TAG,
                    "Onboarding is failed on target network and callback result is = %d", esResult);
        // Resetting Enrollee state to the ES_PROVISIONED_STATE
        // as device is already being provisioned with target network creds.
        gEnrolleeStatusCb(esResult, ES_PROVISIONED_STATE);
    }
}

static FILE* server_fopen(const char* /*path*/, const char *mode)
{
    OC_LOG_V(INFO,ES_ENROLLEE_TAG,"oic_svr_db_server open %s",mode);
    FILE *file= fopen("/opt/usr/media/Images/oic_svr_db_server.json", mode);
    if(file==NULL)
    {
        OC_LOG(ERROR,ES_ENROLLEE_TAG,"oic_svr_db_server failed");
    }
    return file;
}

ESResult InitEasySetup(OCConnectivityType networkType, const char *ssid,
                            const char *passwd, EventCallback cb)
{
    OC_LOG(INFO, ES_ENROLLEE_TAG, "InitEasySetup IN");
    if(!validateParam(networkType,ssid,passwd,cb))
    {
        OC_LOG(ERROR, ES_ENROLLEE_TAG,
                            "InitEasySetup::Stopping Easy setup due to invalid parameters");
        return ES_ERROR;
    }

    //Init callback
    gEnrolleeStatusCb = cb;

    // TODO : This onboarding state has to be set by lower layer, as they better
    // knows when actually on-boarding started.
    cb(ES_ERROR,ES_ON_BOARDING_STATE);

    OC_LOG(INFO, ES_ENROLLEE_TAG, "received callback");
    OC_LOG(INFO, ES_ENROLLEE_TAG, "onboarding now..");

    if(!ESOnboard(ssid, passwd, OnboardingCallback))
    {
        OC_LOG(ERROR, ES_ENROLLEE_TAG, "InitEasySetup::On-boarding failed");
        cb(ES_ERROR, ES_INIT_STATE);
        return ES_ERROR;
    }

    OC_LOG(INFO, ES_ENROLLEE_TAG, "InitEasySetup OUT");
    return ES_OK;
}

ESResult TerminateEasySetup()
{
    if(OCStop() != OC_STACK_OK)
    {
        OC_LOG(ERROR, ES_ENROLLEE_TAG, "OCStack stop failed");
        return ES_ERROR;
    }
    else
    {
        OC_LOG(ERROR, ES_ENROLLEE_TAG, "OCStack stop success");
        return ES_OK;
    }
}

ESResult InitProvisioning()
{
    OC_LOG(INFO, ES_ENROLLEE_TAG, "InitProvisioning <<IN>>");

   // Initialize the OC Stack in Server mode
    if (OCInit(NULL, 0, OC_SERVER) != OC_STACK_OK)
    {
        OC_LOG(ERROR, ES_ENROLLEE_TAG, "OCStack init error");
        return ES_ERROR;
    }

    if (CreateProvisioningResource() != OC_STACK_OK)
    {
        OC_LOG(ERROR, ES_ENROLLEE_TAG, "CreateProvisioningResource error");
        return ES_ERROR;
    }

    if (CreateNetworkResource() != OC_STACK_OK)
    {
        OC_LOG(ERROR, ES_ENROLLEE_TAG, "CreateNetworkResource error");
        return ES_ERROR;
    }

    RegisterResourceEventCallBack(ProvisioningCallback);

    OC_LOG(INFO, ES_ENROLLEE_TAG, "InitProvisioning OUT");
    return ES_RESOURCECREATED;
}

bool validateParam(OCConnectivityType networkType, const char *ssid, const char *passwd,
              EventCallback cb)
{
    if (!ssid || !passwd || !cb)
    {
        OC_LOG(ERROR, ES_ENROLLEE_TAG, "validateParam - Invalid parameters");
        return false;
    }
    return true;
}

