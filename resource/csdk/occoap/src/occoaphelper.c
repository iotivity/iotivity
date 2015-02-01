//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
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

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "occoaphelper.h"
#include "ocstackconfig.h"
#include "logger.h"
#include "coap_time.h"
#include "ocmalloc.h"

#include "cacommon.h"

//-----------------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------------
#define TAG    PCF("OCCoAPHelper")
#define VERIFY_NON_NULL(arg) { if (!arg) {OC_LOG_V(FATAL, TAG, "%s is NULL", #arg); goto exit;} }

//=============================================================================
// Helper Functions
//=============================================================================

OCStackResult isVendorSpecific(uint16_t optionID)
{
    if(optionID >= COAP_VENDOR_OPT_START && optionID <= COAP_MAX_OPT)
    {
        return OC_STACK_OK;
    }
    return OC_STACK_INVALID_OPTION;
}

OCStackResult FormOCResponse(OCResponse * * responseLoc,  ClientCB * cbNode, uint32_t maxAge,
        unsigned char * fullUri, unsigned char * rcvdUri, CAToken_t * rcvdToken,
        OCClientResponse * clientResponse, unsigned char * bufRes)
{
    OCResponse * response = (OCResponse *) OCMalloc(sizeof(OCResponse));
    if (!response)
    {
        return OC_STACK_NO_MEMORY;
    }
    response->cbNode = cbNode;
    response->maxAge = maxAge;
    response->fullUri = fullUri;
    response->rcvdUri = rcvdUri;
    response->rcvdToken = rcvdToken;
    response->clientResponse = clientResponse;
    response->bufRes = bufRes;

    *responseLoc = response;
    return OC_STACK_OK;
}

OCStackResult FormOCClientResponse(OCClientResponse * clientResponse,
        OCStackResult result, OCDevAddr * remote, uint32_t seqNum,
        const unsigned char * resJSONPayload)
{
    clientResponse->sequenceNumber = seqNum;
    clientResponse->result = result;
    clientResponse->addr = remote;
    clientResponse->resJSONPayload = resJSONPayload;

    return OC_STACK_OK;
}

