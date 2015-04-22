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

#include "resourcemanager.h"
#include "securevirtualresourcetypes.h"
#include "aclresource.h"
#include "pstatresource.h"
#include "doxmresource.h"
#include "credresource.h"
#include "ocmalloc.h"
#include "logger.h"
#include "utlist.h"
#include <string.h>

#define TAG PCF("SRM-RM")

/**
 * This method is used by all secure resource modules to send responses to REST queries.
 *
 * @param ehRequest - pointer to entity handler request data structure.
 * @param rspPayload - response payload in JSON.
 *
 * @retval  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult SendSRMResponse(const OCEntityHandlerRequest *ehRequest,
        OCEntityHandlerResult ehRet, const char *rspPayload)
{
    OCEntityHandlerResponse response;
    if (ehRequest)
    {
        response.requestHandle = ehRequest->requestHandle;
        response.resourceHandle = ehRequest->resource;
        response.ehResult = ehRet;
        response.payload = (unsigned char *)rspPayload;
        response.payloadSize = (rspPayload ? strlen(rspPayload) : 0);
        response.persistentBufferFlag = 0;

        return OCDoResponse(&response);
    }
    return OC_STACK_ERROR;
}

/* TODO: This global should be in doxmresource.c once it is defined. */
OicSecOxm_t gOicSecDoxmJustWorks = OIC_JUST_WORKS;
OicSecDoxm_t gDefaultDoxm =
{
    NULL,                   /* OicUrn_t *oxmType */
    0,                      /* size_t oxmTypeLen */
    &gOicSecDoxmJustWorks,  /* uint16_t *oxm */
    1,                      /* size_t oxmLen */
    false,                  /* bool owned */
 // 0,                      /* uint8_t deviceIDFormat */
    {},                     /* OicUuid_t deviceID */
    {},                     /* OicUuid_t owner */
};

/* TODO: These globals should be in pstatresource.c once it is defined. */
OicSecDpom_t gSm = SINGLE_SERVICE_CLIENT_DRIVEN;
OicSecPstat_t gDefaultPstat =
{
    false,                                      /* bool isOwned */
    TAKE_OWNER | BOOTSTRAP_SERVICE | SECURITY_MANAGEMENT_SERVICES \
    | PROVISION_CREDENTIALS | PROVISION_ACLS,   /* OicSecDpm_t cm */
    TAKE_OWNER | BOOTSTRAP_SERVICE | SECURITY_MANAGEMENT_SERVICES \
    | PROVISION_CREDENTIALS | PROVISION_ACLS,   /* OicSecDpm_t tm */
    {},                                         /* OicUuid_t deviceID */
    SINGLE_SERVICE_CLIENT_DRIVEN,               /* OicSecDpom_t om */
    1,                                          /* the number of elts in Sms */
    &gSm,                                       /* OicSecDpom_t        *sms */
    0,                                          /* uint16_t commitHash */
};

/**
 * Initialize all secure resources ( /oic/sec/cred, /oic/sec/acl, /oic/sec/pstat etc).
 *
 * @retval  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult InitSecureResources( )
{
    OCStackResult ret;

    ret = InitACLResource();

    if(OC_STACK_OK == ret)
    {
        ret = InitDoxmResource();
    }

    if(OC_STACK_OK == ret)
    {
        ret = InitPstatResource();
    }

    if(OC_STACK_OK == ret)
    {
        ret = InitCredResource();
    }

    if(OC_STACK_OK != ret)
    {
        /* Something has gone wrong with restoring Secure Virtual Resources.
         * TODO: design secure behavior.
         * For now: Fall back to default un-initialized state.
         */
        /* NOTE TO SACHIN:
         * TODO: Do an OCCreateResource for gDefaultDoxm and gDefaultPstat.
         * TODO: Create the binary SVR DB using gDefaultDoxm and gDefaultPstat.
         */
         ret = OC_STACK_OK;
    }

    return ret;
}

/**
 * Perform cleanup for secure resources ( /oic/sec/cred, /oic/sec/acl, /oic/sec/pstat etc).
 *
 * @retval  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult DestroySecureResources( )
{
    DeInitACLResource();
    DeInitCredResource();
    DeInitDoxmResource();
    DeInitPstatResource();

    return OC_STACK_OK;
}
