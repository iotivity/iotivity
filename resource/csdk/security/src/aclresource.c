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
#include "ocmalloc.h"
#include "resourcemanager.h"
#include "aclresource.h"
#include "utlist.h"
#include "cJSON.h"
#include <stdlib.h>
#include <string.h>

#define TAG  PCF("SRM-ACL")

const char * OIC_RSRC_TYPE_SEC_ACL = "oic.sec.acl";
const char * OIC_RSRC_ACL_URI =  "/oic/sec/acl";

OicSecAcl_t        *gAcl = NULL;
OCResourceHandle    gAclHandle = NULL;

/*
 * This internal method converts ACL data into JSON format.
 */
static char * ToJSON(OicSecAcl_t * acl)
{
    char *jsonStr = NULL;

    /* This is demo code to display JSON marshalling */
    if (acl)
    {
        cJSON *json = cJSON_CreateObject();
        cJSON *jsonAcl = NULL;
        if (json)
        {
            cJSON_AddItemToObject(json, "acl", jsonAcl = cJSON_CreateObject());
            if (jsonAcl)
            {
                cJSON_AddStringToObject(jsonAcl, "sbj",  "1111111111111111");
                cJSON_AddStringToObject(jsonAcl, "rsrc", "/light");
                cJSON_AddNumberToObject(jsonAcl, "per", 0x001F);
                jsonStr = cJSON_Print(json);
            }
            cJSON_Delete(json);
        }
    }
    return jsonStr;
}

/*
 * This internal method is the entity handler for ACL resources.
 */
static OCEntityHandlerResult ACLEntityHandler (OCEntityHandlerFlag flag,
                                        OCEntityHandlerRequest * ehRequest)
{
    OCEntityHandlerResult ret = OC_EH_ERROR;
    char *jsonRsp = NULL;

    /*
     * This method will handle REST request (GET/PUT/POST/DEL) for
     * virtual resources such as: /oic/sec/cred, /oic/sec/acl etc
     */

    if (flag & OC_REQUEST_FLAG)
    {
        /* TODO :  Handle PUT/POST/DEL methods */
        OC_LOG (INFO, TAG, "Flag includes OC_REQUEST_FLAG");
        if (OC_REST_GET == ehRequest->method)
        {
            /* Convert ACL data into JSON for transmission */
            jsonRsp = ToJSON(gAcl);

            /* Send payload to request originator */
            ret = (SendSRMResponse(ehRequest, jsonRsp) == OC_STACK_OK ?
                   OC_EH_OK : OC_EH_ERROR);
        }
    }
    OCFree(jsonRsp);
    return ret;
}


/*
 * This internal method is used to create '/oic/sec/acl' resource.
 */
static OCStackResult CreateACLResource()
{
    OCStackResult ret;

    /* TODO : Does these resources needs to be OC_DISCOVERABLE */
    ret = OCCreateResource(&gAclHandle,
                           OIC_RSRC_TYPE_SEC_ACL,
                           "oic.mi.def",
                           OIC_RSRC_ACL_URI,
                           ACLEntityHandler,
                           OC_DISCOVERABLE | OC_OBSERVABLE);

    if (ret != OC_STACK_OK)
    {
        DeInitACLResource();
    }
    return ret;
}

/**
 * Initialize ACL resource by loading data from persistent storage.
 *
 * @retval  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult InitACLResource()
{
    OCStackResult ret;

    /*
     * TODO : Read ACL resource from PS
     * TODO : Unmarshal JSON to OicSecAcl_t
     */

    /* Instantiate 'oic.sec.acl' */
    ret = CreateACLResource();
    return ret;
}

/**
 * Perform cleanup for ACL resources.
 *
 * @retval  none
 */
void DeInitACLResource()
{
    OCDeleteResource(gAclHandle);
    OicSecAcl_t *aclTmp1 = NULL, *aclTmp2 = NULL;
    LL_FOREACH_SAFE(gAcl, aclTmp1, aclTmp2)
    {
        LL_DELETE(gAcl, aclTmp1);
        OCFree(aclTmp1);
    }
    gAcl = NULL;
}

/**
 * This method is used by PolicyEngine to retrieve ACL for a Subject.
 *
 * @param subjectId - ID of the subject for which ACL is required.
 * @param subjectIdLen - length of subjectId field.
 *
 * @retval  reference to @ref OicSecAcl_t if ACL is found, else NULL
 */
const OicSecAcl_t* GetACLResourceData(const unsigned char* subjectId, size_t subjectIdLen)
{
    OicSecAcl_t *acl = NULL;

    if ( (NULL == subjectId) || (0 == subjectIdLen))
    {
        return NULL;
    }

    LL_FOREACH(gAcl, acl)
    {
        /* TODO : Need to synch on 'Subject' data type */
        if (memcmp(acl->Subject, subjectId, subjectIdLen) == 0)
            return acl;
    }
    return NULL;
}
