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
#include "svcresource.h"
#include "amaclresource.h"
#include "psinterface.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "logger.h"
#include "utlist.h"
#include "srmutility.h"
#include "ocpayload.h"
#include <string.h>

#define TAG PCF("SRM-RM")

/**
 * This method is used by all secure resource modules to send responses to REST queries.
 *
 * @param ehRequest pointer to entity handler request data structure.
 * @param ehRet result code from entity handler.
 * @param rspPayload response payload in JSON.
 *
 * @retval  OC_STACK_OK for Success, otherwise some error value
 */

OCStackResult SendSRMResponse(const OCEntityHandlerRequest *ehRequest,
        OCEntityHandlerResult ehRet, const OCRepPayload *rspPayload)
{
    OC_LOG (INFO, TAG, PCF("SRM sending SRM response"));
    OCEntityHandlerResponse response =
        { ehRequest->requestHandle, ehRequest->resource, ehRet, 0, 0, { },{ 0 }, 0 };
    OCStackResult ret = OC_STACK_ERROR;
    if (ehRequest)
    {
        if (rspPayload)
        {
            response.payload = (OCPayload*) rspPayload;
            response.payload->type = PAYLOAD_TYPE_REPRESENTATION;
        }
        ret = OCDoResponse(&response);
    }
    OCRepPayloadDestroy((OCRepPayload*)rspPayload);
    return ret;
}


/**
 * Initialize all secure resources ( /oic/sec/cred, /oic/sec/acl, /oic/sec/pstat etc).
 *
 * @retval  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult InitSecureResources( )
{
    //Read data from persistent storage.
    OicSvr_t svr = {.aclPayload = NULL};
    OCStackResult ret = ReadSVDataFromPS(&svr);
    if (ret != OC_STACK_OK)
    {
        OC_LOG (INFO, TAG, PCF("ReadSVDataFromPS failed"));
    }
    /*
     * doxm resource should be initialized first as it contains the DeviceID
     * which MAY be used during initialization of other resources.
     */
    ret = InitDoxmResource(svr.doxmPayload, svr.doxmSize);
    if(OC_STACK_OK == ret)
    {
        ret = InitPstatResource(svr.pstatPayload, svr.pstatSize);

    }
    if(OC_STACK_OK == ret)
    {
        ret = InitACLResource(svr.aclPayload, svr.aclSize);
    }
    if(OC_STACK_OK == ret)
    {
        ret = InitCredResource(svr.credPayload, svr.credSize);
    }

 /*
  * TODO: CBOR
  * if(OC_STACK_OK == ret)
    {
        ret = InitCredResource();
    }
    if(OC_STACK_OK == ret)
    {
        ret = InitSVCResource();
    }
    if(OC_STACK_OK == ret)
    {
        ret = InitAmaclResource();
    }
  */

    if (OC_STACK_OK != ret)
    {
        //TODO: Update the default behavior if one of the SVR fails
        DestroySecureResources();
    }
    OICFree(svr.doxmPayload);
    OICFree(svr.aclPayload);
    OICFree(svr.credPayload);
    OICFree(svr.pstatPayload);

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
