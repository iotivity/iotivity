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

#ifndef IOTVT_SRM_PE_H
#define IOTVT_SRM_PE_H

#include "ocstack.h"
#include "logger.h"
#include "securevirtualresourcetypes.h"
#include "cainterface.h"
#include <stdlib.h>
#include <stdint.h>


typedef enum PEState
{
    STOPPED = 0,
    AWAITING_REQUEST,
    BUSY
} PEState_t;

typedef struct PEContext
{
    PEState_t   state;
    OicUuid_t   *subject;
    char        *resource;
    uint16_t    permission;
    bool        matchingAclFound;
    SRMAccessResponse_t retVal;
} PEContext_t;

/**
 * Check whether a request should be allowed.
 *
 * @param   context     Pointer to Policy Engine context to use.
 * @param   subjectId   Pointer to Id of the requesting entity.
 * @param   resource    Pointer to URI of Resource being requested.
 * @param   permission  Requested permission.
 *
 * @return  ACCESS_GRANTED if request should go through,
 *          otherwise some flavor of ACCESS_DENIED
 */
SRMAccessResponse_t CheckPermission(
    PEContext_t     *context,
    const OicUuid_t *subjectId,
    const char      *resource,
    const uint16_t  requestedPermission);

/**
 * Initialize the Policy Engine. Call this before calling CheckPermission().
 * TODO Eventually this and DeInit() need to be called from a new
 *      "SRMInit(SRMContext_t *)" function, TBD after BeachHead.
 * @param   context     Pointer to Policy Engine context to initialize.
 * @return  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult InitPolicyEngine(PEContext_t *context);

/**
 * De-Initialize the Policy Engine. Call this before exiting to allow Policy
 * Engine to do cleanup on context.
 * @param   context     Pointer to Policy Engine context to de-initialize.
 * @return  none
 */
void DeInitPolicyEngine(PEContext_t *context);

/**
 * Return the uint16_t CRUDN permission corresponding to passed CAMethod_t.
 */
uint16_t GetPermissionFromCAMethod_t(const CAMethod_t method);

#endif //IOTVT_SRM_PE_H
