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

#include "oic_malloc.h"
#include "policyengine.h"
#include "resourcemanager.h"
#include "securevirtualresourcetypes.h"
#include "srmresourcestrings.h"
#include "logger.h"
#include "aclresource.h"
#include "srmutility.h"
#include "doxmresource.h"
#include <string.h>

#define TAG PCF("SRM-PE")

/**
 * Return the uint16_t CRUDN permission corresponding to passed CAMethod_t.
 */
uint16_t GetPermissionFromCAMethod_t(const CAMethod_t method)
{
    uint16_t perm = 0;
    switch(method)
    {
        case CA_GET:
            perm = (uint16_t)PERMISSION_READ;
            break;
        case CA_POST: // For now we treat all PUT & POST as Write
        case CA_PUT:  // because we don't know if resource exists yet.
            perm = (uint16_t)PERMISSION_WRITE;
            break;
        case CA_DELETE:
            perm = (uint16_t)PERMISSION_DELETE;
            break;
        default: // if not recognized, must assume requesting full control
            perm = (uint16_t)PERMISSION_FULL_CONTROL;
            break;
    }
    return perm;
}

/**
 * @brief Compares two OicUuid_t structs.
 * @return true if the two OicUuid_t structs are equal, else false.
 */
bool UuidCmp(OicUuid_t *firstId, OicUuid_t *secondId)
{
    // TODO use VERIFY macros to check for null when they are merged.
    if(NULL == firstId || NULL == secondId)
    {
        return false;
    }
    for(int i = 0; i < UUID_LENGTH; i++)
    {
        if(firstId->id[i] != secondId->id[i])
        {
            return false;
        }
    }
    return true;
}


/**
 * Set the state and clear other stateful context vars.
 */
void SetPolicyEngineState(PEContext_t *context, const PEState_t state)
{
    // Clear stateful context variables.
    OICFree(context->subject);
    context->subject = NULL;
    OICFree(context->resource);
    context->resource = NULL;
    context->permission = 0x0;
    context->matchingAclFound = false;
    context->retVal = ACCESS_DENIED_POLICY_ENGINE_ERROR;

    // Set state.
    context->state = state;
}

/**
 * @brief Compare the request's subject to DevOwner.
 *
 * @return true if context->subjectId == GetDoxmDevOwner(), else false
 */
bool IsRequestFromDevOwner(PEContext_t *context)
{
    bool retVal = false;
    OicUuid_t owner;

    if(OC_STACK_OK == GetDoxmDevOwnerId(&owner))
    {
        retVal = UuidCmp(context->subject, &owner);
    }

    return retVal;
}

/**
 * Bitwise check to see if 'permission' contains 'request'.
 * @param   permission  The allowed CRUDN permission.
 * @param   request     The CRUDN permission being requested.
 * @return true if 'permission' bits include all 'request' bits.
 */
static inline bool IsPermissionAllowingRequest(const uint16_t permission,
    const uint16_t request)
{
    if(request == (request & permission))
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * Compare the passed subject to the wildcard (aka anonymous) subjectId.
 * @return true if 'subject' is the wildcard, false if it is not.
 */
static inline bool IsWildCardSubject(OicUuid_t *subject)
{
    // Because always comparing to string literal, use strcmp()
    if(0 == memcmp(subject, &WILDCARD_SUBJECT_ID, sizeof(OicUuid_t)))
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * Copy the subject, resource and permission into the context fields.
 */
void CopyParamsToContext(
    PEContext_t     *context,
    const OicUuid_t *subjectId,
    const char      *resource,
    const uint16_t  requestedPermission)
{
    size_t length = 0;

    // Free any existing subject.
    OICFree(context->subject);
    // Copy the subjectId into context.
    context->subject = (OicUuid_t*)OICMalloc(sizeof(OicUuid_t));
    VERIFY_NON_NULL(TAG, context->subject, ERROR);
    memcpy(context->subject, subjectId, sizeof(OicUuid_t));

    // Copy the resource string into context.
    length = strlen(resource) + 1;
    if(0 < length)
    {
        OICFree(context->resource);
        context->resource = (char*)OICMalloc(length);
        VERIFY_NON_NULL(TAG, context->resource, ERROR);
        strncpy(context->resource, resource, length);
        context->resource[length - 1] = '\0';
    }

    // Assign the permission field.
    context->permission = requestedPermission;

exit:
    return;
}

/**
 * Check whether 'resource' is in the passed ACL.
 * @param   resource    The resource to search for.
 * @param   acl         The ACL to check.
 * @return true if 'resource' found, otherwise false.
 */
 bool IsResourceInAcl(const char *resource, const OicSecAcl_t *acl)
 {
    for(size_t n = 0; n < acl->resourcesLen; n++)
    {
        if(0 == strcmp(resource, acl->resources[n]) || // TODO null terms?
         0 == strcmp(WILDCARD_RESOURCE_URI, acl->resources[n]))
        {
            return true;
        }
    }
    return false;
 }

/**
 * Find ACLs containing context->subject.
 * Search each ACL for requested resource.
 * If resource found, check for context->permission.
 * Set context->retVal to result from first ACL found which contains
 * correct subject AND resource.
 *
 * @retval void
 */
void ProcessAccessRequest(PEContext_t *context)
{
    OC_LOG(INFO, TAG, PCF("Entering ProcessAccessRequest()"));
    if(NULL != context)
    {
        const OicSecAcl_t *currentAcl = NULL;
        OicSecAcl_t *savePtr = NULL;

        // Start out assuming subject not found.
        context->retVal = ACCESS_DENIED_SUBJECT_NOT_FOUND;
        do
        {
            OC_LOG(INFO, TAG, PCF("ProcessAccessRequest(): getting ACL..."));
            currentAcl = GetACLResourceData(context->subject, &savePtr);
            if(NULL != currentAcl)
            {
                // Found the subject, so how about resource?
                OC_LOG(INFO, TAG, PCF("ProcessAccessRequest(): \
                    found ACL matching subject."));
                context->retVal = ACCESS_DENIED_RESOURCE_NOT_FOUND;
                OC_LOG(INFO, TAG, PCF("ProcessAccessRequest(): \
                    Searching for resource..."));
                if(IsResourceInAcl(context->resource, currentAcl))
                {
                    OC_LOG(INFO, TAG, PCF("ProcessAccessRequest(): \
                        found matching resource in ACL."));
                    context->matchingAclFound = true;
                    // Found the resource, so it's down to permission.
                    context->retVal = ACCESS_DENIED_INSUFFICIENT_PERMISSION;
                    if(IsPermissionAllowingRequest(currentAcl->permission, \
                        context->permission))
                    {
                        context->retVal = ACCESS_GRANTED;
                    }
                }
            }
            else
            {
                OC_LOG(INFO, TAG, PCF("ProcessAccessRequest(): \
                    no ACL found matching subject ."));
            }
        }
        while((NULL != currentAcl) && (false == context->matchingAclFound));

        if(IsAccessGranted(context->retVal))
        {
            OC_LOG(INFO, TAG, PCF("ProcessAccessRequest(): \
                Leaving ProcessAccessRequest(ACCESS_GRANTED)"));
        }
        else
        {
            OC_LOG(INFO, TAG, PCF("ProcessAccessRequest(): \
                Leaving ProcessAccessRequest(ACCESS_DENIED)"));
        }
    }
    else
    {
        OC_LOG(INFO, TAG, PCF("ProcessAccessRequest(): \
            Leaving ProcessAccessRequest(context is NULL)"));
    }

}

/**
 * Check whether a request should be allowed.
 * @param   context     Pointer to (Initialized) Policy Engine context to use.
 * @param   subjectId   Pointer to Id of the requesting entity.
 * @param   resource    Pointer to URI of Resource being requested.
 * @param   permission  Requested permission.
 * @return  ACCESS_GRANTED if request should go through,
 *          otherwise some flavor of ACCESS_DENIED
 */
SRMAccessResponse_t CheckPermission(
    PEContext_t     *context,
    const OicUuid_t *subjectId,
    const char      *resource,
    const uint16_t  requestedPermission)
{
    SRMAccessResponse_t retVal = ACCESS_DENIED_POLICY_ENGINE_ERROR;

    VERIFY_NON_NULL(TAG, context, ERROR);
    VERIFY_NON_NULL(TAG, subjectId, ERROR);
    VERIFY_NON_NULL(TAG, resource, ERROR);

    // Each state machine context can only be processing one request at a time.
    // Therefore if the context is not in AWAITING_REQUEST state, return error.
    // Otherwise, change to BUSY state and begin processing request.
    if(AWAITING_REQUEST == context->state)
    {
        SetPolicyEngineState(context, BUSY);
        CopyParamsToContext(context, subjectId, resource, requestedPermission);
        // Before doing any processing, check if request coming
        // from DevOwner and if so, always GRANT.
        if(IsRequestFromDevOwner(context))
        {
            context->retVal = ACCESS_GRANTED;
        }
        else
        {
            ProcessAccessRequest(context);
            // If matching ACL not found, and subject != wildcard, try wildcard.
            if((false == context->matchingAclFound) && \
                (false == IsWildCardSubject(context->subject)))
            {
                OICFree(context->subject);
                context->subject = (OicUuid_t*)OICMalloc(sizeof(OicUuid_t));
                VERIFY_NON_NULL(TAG, context->subject, ERROR);
                memcpy(context->subject, &WILDCARD_SUBJECT_ID,
                    sizeof(OicUuid_t));
                ProcessAccessRequest(context); // TODO anonymous subj can result
                                               // in confusing err code return.
            }
        }
    }
    else
    {
        context->retVal = ACCESS_DENIED_POLICY_ENGINE_ERROR;
    }

    // Capture retVal before resetting state for next request.
    retVal = context->retVal;
    SetPolicyEngineState(context, AWAITING_REQUEST);

exit:
    return retVal;
}

/**
 * Initialize the Policy Engine. Call this before calling CheckPermission().
 * @param   context     Pointer to Policy Engine context to initialize.
 * @return  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult InitPolicyEngine(PEContext_t *context)
{
    if(NULL != context)
    {
        SetPolicyEngineState(context, AWAITING_REQUEST);
    }

    return OC_STACK_OK;
}

/**
 * De-Initialize the Policy Engine.  Call this before exiting to allow Policy
 * Engine to do cleanup on context.
 * @param   context     Pointer to Policy Engine context to de-initialize.
 * @return  none
 */
void DeInitPolicyEngine(PEContext_t *context)
{
    if(NULL != context)
    {
        SetPolicyEngineState(context, STOPPED);
    }

    return;
}
