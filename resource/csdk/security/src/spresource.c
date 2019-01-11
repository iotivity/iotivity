//******************************************************************
// Copyright 2018 Cable Television Laboratories, Inc.
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
// See the License for the specific lan guage governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include "iotivity_config.h"

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "ocstack.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "ocpayload.h"
#include "ocpayloadcbor.h"
#include "experimental/payload_logging.h"
#include "psinterface.h"
#include "resourcemanager.h"
#include "spresource.h"
#include "deviceonboardingstate.h"
#include "srmutility.h"
#include "srmresourcestrings.h"

#define TAG  "OIC_SRM_SP"

#define OIC_CBOR_SP_NAME "sp"

static OCResourceHandle    gSpHandle  = NULL;
static OicSecSp_t         *gSp        = NULL;

// Default sp values
// char * gSupportedProfiles[] = { "1.3.6.1.4.1.51414.0.0.1.0", "1.3.6.1.4.1.51414.0.0.2.0", "1.3.6.1.4.1.51414.0.0.3.0", "1.3.6.1.4.1.51414.0.0.4.0" };
char * gSupportedProfiles[] = { "1.3.6.1.4.1.51414.0.0.1.0" };
OicSecSp_t gDefaultSp =
{
    // 4,                      // supportedLen
    1,
    gSupportedProfiles,        // supportedProfiles
    "1.3.6.1.4.1.51414.0.0.1.0", // currentProfile
};

bool gAllProps[SP_PROPERTY_COUNT] = { true, true };

// Default cbor payload size. This value is increased in case of CborErrorOutOfMemory.
// The value of payload size is increased until reaching below max cbor size.
static const uint16_t CBOR_SIZE = 512;

// Max cbor size payload.
static const uint16_t CBOR_MAX_SIZE = 4400;

// Minimum SP map size (rt, if)
static const uint8_t SP_MIN_MAP_SIZE = 2;

OCStackResult SpToCBORPayload(const OicSecSp_t *sp, uint8_t **payload, size_t *size)
{
    bool allProps[SP_PROPERTY_COUNT];
    SetAllSpProps(allProps, true);

    return SpToCBORPayloadPartial(sp, payload, size, allProps);
}

OCStackResult SpToCBORPayloadPartial(const OicSecSp_t *sp,
                                     uint8_t **payload, size_t *size,
                                     const bool *propertiesToInclude)
{
    if (NULL == sp || NULL == payload || NULL != *payload || NULL == size)
    {
        return OC_STACK_INVALID_PARAM;
    }

    size_t cborLen = *size;
    if (0 == cborLen)
    {
        cborLen = CBOR_SIZE;
    }

    *payload = NULL;
    *size = 0;

    OCStackResult ret = OC_STACK_ERROR;
    int64_t cborEncoderResult = CborNoError;

    CborEncoder encoder;
    CborEncoder spMap;

    // Calculate map size
    size_t spMapSize = SP_MIN_MAP_SIZE;
    for (int i = 0; i < SP_PROPERTY_COUNT; i++)
    {
        if (propertiesToInclude[i])
        {
            spMapSize++;
        }
    }

    uint8_t *outPayload = (uint8_t *)OICCalloc(1, cborLen);
    VERIFY_NOT_NULL_RETURN(TAG, outPayload, ERROR, OC_STACK_ERROR);

    cbor_encoder_init(&encoder, outPayload, cborLen, 0);
    cborEncoderResult = cbor_encoder_create_map(&encoder, &spMap, spMapSize);
    VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborEncoderResult, "Failed Adding sp Map.");

    // supportedprofiles
    if (propertiesToInclude[SP_SUPPORTED_PROFILES])
    {
        VERIFY_OR_LOG_AND_EXIT(TAG, (0 < sp->supportedLen),
            "List of supported security profiles can't be empty", ERROR);

        cborEncoderResult = cbor_encode_text_string(
            &spMap, OIC_JSON_SUPPORTED_SP_NAME, strlen(OIC_JSON_SUPPORTED_SP_NAME));
        VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborEncoderResult,
            "Failed Adding supportedprofiles Name Tag.");
        OIC_LOG_V(DEBUG, TAG, "%s encoded sp %s tag.", __func__, OIC_JSON_SUPPORTED_SP_NAME);

        CborEncoder supportedProfiles;
        cborEncoderResult = cbor_encoder_create_array(&spMap, &supportedProfiles, sp->supportedLen);
        VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborEncoderResult, "Failed creating supported_types Array.");
        OIC_LOG_V(DEBUG, TAG, "%s created sp supportedprofiles array.", __func__);
        for(size_t i = 0; i < sp->supportedLen; i++)
        {
            char* curProfile = sp->supportedProfiles[i];
            cborEncoderResult = cbor_encode_text_string(&supportedProfiles, curProfile, strlen(curProfile));
            VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborEncoderResult, "Failed Adding supportedprofiles Value.");
            OIC_LOG_V(DEBUG, TAG, "%s encoded sp supportedprofiles value %s.", __func__, curProfile);
        }

        cborEncoderResult = cbor_encoder_close_container(&spMap, &supportedProfiles);
        VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborEncoderResult, "Failed Closing supportedProfiles.");
        OIC_LOG_V(DEBUG, TAG, "%s closed sp supportedprofiles map.", __func__);
    }

    // current profile
    if (propertiesToInclude[SP_CURRENT_PROFILE])
    {
        cborEncoderResult = cbor_encode_text_string(
            &spMap, OIC_JSON_CURRENT_SP_NAME, strlen(OIC_JSON_CURRENT_SP_NAME));
        VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborEncoderResult, "Failed Adding currentprofile Name Tag.");
        OIC_LOG_V(DEBUG, TAG, "%s encoded sp %s tag.", __func__, OIC_JSON_CURRENT_SP_NAME);

        cborEncoderResult = cbor_encode_text_string(
            &spMap, sp->currentProfile, strlen(sp->currentProfile));
        VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborEncoderResult, "Failed Adding supportedprofiles Value.");
        OIC_LOG_V(DEBUG, TAG, "%s encoded sp currentprofile value %s.", __func__, sp->currentProfile);
    }

    // rt (mandatory)

    CborEncoder rtArray;
    cborEncoderResult = cbor_encode_text_string(
        &spMap, OIC_JSON_RT_NAME, strlen(OIC_JSON_RT_NAME));
    VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborEncoderResult, "Failed Addding RT Name Tag.");
    cborEncoderResult = cbor_encoder_create_array(&spMap, &rtArray, 1);
    VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborEncoderResult, "Failed Addding RT Value.");
    for (size_t i = 0; i < 1; i++)
    {
        cborEncoderResult = cbor_encode_text_string(&rtArray, OIC_RSRC_TYPE_SEC_SP,
                strlen(OIC_RSRC_TYPE_SEC_SP));
        VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborEncoderResult, "Failed Adding RT Value.");
    }
    cborEncoderResult = cbor_encoder_close_container(&spMap, &rtArray);
    VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborEncoderResult, "Failed Closing RT.");

    // if (mandatory)

    CborEncoder ifArray;
    cborEncoderResult = cbor_encode_text_string(&spMap, OIC_JSON_IF_NAME,
       strlen(OIC_JSON_IF_NAME));
    VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborEncoderResult, "Failed Addding IF Name Tag.");
    cborEncoderResult = cbor_encoder_create_array(&spMap, &ifArray, 1);
    VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborEncoderResult, "Failed Addding IF Value.");
    for (size_t i = 0; i < 1; i++)
    {
        cborEncoderResult = cbor_encode_text_string(&ifArray, OC_RSRVD_INTERFACE_DEFAULT,
                strlen(OC_RSRVD_INTERFACE_DEFAULT));
        VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborEncoderResult, "Failed Adding IF Value.");
    }
    cborEncoderResult = cbor_encoder_close_container(&spMap, &ifArray);
    VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborEncoderResult, "Failed Closing IF.");

    cborEncoderResult = cbor_encoder_close_container(&encoder, &spMap);
    VERIFY_CBOR_SUCCESS_OR_OUT_OF_MEMORY(TAG, cborEncoderResult, "Failed Closing SP Map.");

    if (CborNoError == cborEncoderResult)
    {
        *size = cbor_encoder_get_buffer_size(&encoder, outPayload);
        *payload = outPayload;
        ret = OC_STACK_OK;
    }

exit:
    if ((CborErrorOutOfMemory == cborEncoderResult) && (cborLen < CBOR_MAX_SIZE))
    {
        // reallocate and try again!
        OICFree(outPayload);
        outPayload = NULL;

        // Since the allocated initial memory failed, double the memory.
        cborLen += cbor_encoder_get_buffer_size(&encoder, encoder.end);
        cborEncoderResult = CborNoError;
        ret = SpToCBORPayloadPartial(sp, payload, &cborLen, propertiesToInclude);
        if (OC_STACK_OK == ret)
        {
            *size = cborLen;
        }
    }

    if ((CborNoError != cborEncoderResult) || (OC_STACK_OK != ret))
    {
        OICFree(outPayload);
        outPayload = NULL;
        *payload = NULL;
        *size = 0;
        ret = OC_STACK_ERROR;
    }

    return ret;
}

/**
 * Static method to extract a supportedprofiles array from spMap.
 *
 * @param [i] spMap             sp map positioned at supportedprofiles array
 * @param [o] supportedProfiles allocated and extracted list of supported profile names
 * @param [o] supportedLen      length of the extracted supportedProfiles array
 *
 * @return ::OC_STACK_OK for Success, otherwise some error value.
 *
 * @note: this fxn allocates the supportedProfiles array, free with OICFree when done
*/
static OCStackResult SupportedProfilesFromCBOR(CborValue *spMap,
                                               char*** supportedProfiles,
                                               size_t* supportedLen)
{
    OCStackResult ret = OC_STACK_ERROR;
    CborError cborResult = CborNoError;
    *supportedProfiles = NULL;
    *supportedLen = 0;

    size_t readLen = 0;

    cborResult = cbor_value_get_array_length(spMap, supportedLen);
    VERIFY_OR_LOG_AND_EXIT(TAG, (CborNoError == cborResult) && (0 != supportedLen),
        "Failed to find sp supportedprofiles array length", ERROR);

    *supportedProfiles = (char**)OICCalloc(*supportedLen, sizeof(char*));
    VERIFY_NOT_NULL(TAG, *supportedProfiles, ERROR);

    CborValue supportedProfilesCbor;
    cborResult = cbor_value_enter_container(spMap, &supportedProfilesCbor);
    VERIFY_CBOR_SUCCESS(TAG, cborResult, "Failed to enter SP supportedprofiles array");

    size_t numProfilesExtracted = 0;
    for(size_t i = 0;
       cbor_value_is_valid(&supportedProfilesCbor) &&
       cbor_value_is_text_string(&supportedProfilesCbor); i++)
    {
        // Extract the current profile name
        cborResult = cbor_value_dup_text_string(
            &supportedProfilesCbor, &((*supportedProfiles)[i]), &readLen, NULL);
        VERIFY_CBOR_SUCCESS(TAG, cborResult,
            "Failed to extract SP security profile name from supportedprofiles.");
        numProfilesExtracted++;

        // advance to the next profile
        cborResult = cbor_value_advance(&supportedProfilesCbor);
        VERIFY_CBOR_SUCCESS(TAG, cborResult, "Failed to advance to the SP next security profile name.");
    }

    // Make sure we extracted the entire contents of the cbor array
    VERIFY_OR_LOG_AND_EXIT(TAG, (*supportedLen == numProfilesExtracted),
        "Not all of the profiles from SP supportedprofiles were extracted", ERROR);

    ret = OC_STACK_OK;

exit:

    if ((CborNoError != cborResult) && (NULL != *supportedProfiles))
    {
        OICFree(supportedProfiles);
        *supportedProfiles = NULL;
        *supportedLen = 0;
        ret = OC_STACK_ERROR;
    }

    return ret;
}

/**
 * Static method to extract the currentprofile from a spMap, and determine the
 * corresponding index into the supportedProfiles array
 *
 * @param [i] spMap             sp map positioned at supportedprofiles map
 * @param [i] supportedProfiles array of supported profile names
 * @param [i] supportedLen      length of supportedProfiles array
 *
 * @param [o] currentProfile     current profile
 *                              NULL on error
 *
 * @return ::OC_STACK_OK for Success, otherwise error value.
 */
static OCStackResult CurrentProfileFromCBOR(CborValue *spMap, char **currentProfile)
{
    OCStackResult ret = OC_STACK_ERROR;
    CborError cborResult = CborNoError;
    *currentProfile = NULL;

    size_t readLen = 0;

    // extract the current profile name
    cborResult = cbor_value_dup_text_string(spMap, currentProfile, &readLen, NULL);
    VERIFY_CBOR_SUCCESS(TAG, cborResult, "Failed to extract SP current profile name.");

    ret = OC_STACK_OK;

exit:
    return ret;
}

OCStackResult CBORPayloadToSp(const uint8_t *cborPayload,
                              const size_t size,
                              OicSecSp_t **secSp,
                              bool *decodedProperties)
{


    if (NULL == cborPayload || NULL == secSp || NULL != *secSp || 0 == size)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult ret = OC_STACK_ERROR;
    CborError cborResult = CborNoError;

    CborParser parser = { .end = NULL };
    CborValue spCbor = { .parser = NULL };
    CborValue spMap = { .parser = NULL, .ptr = NULL, .remaining = 0, .extra = 0, .type = 0, .flags = 0 };

    OicSecSp_t *sp = NULL;

    if (NULL != decodedProperties)
    {
        SetAllSpProps(decodedProperties, false);
    }

    // init cbor parser
    cbor_parser_init(cborPayload, size, 0, &parser, &spCbor);

    // allocate sp struct
    sp = (OicSecSp_t *)OICCalloc(1, sizeof(OicSecSp_t));
    VERIFY_NOT_NULL(TAG, sp, ERROR);

    // Enter sp map
    cborResult = cbor_value_enter_container(&spCbor, &spMap);
    VERIFY_CBOR_SUCCESS(TAG, cborResult, "Failed to enter the SP map");

    // loop over all cbor entries looking for known keys
    char* tagName = NULL;
    size_t tagLen = 0;
    while (cbor_value_is_valid(&spMap))
    {
        // Determine the next tag and advance to the corresponding value
        CborType type = cbor_value_get_type(&spMap);
        if (type == CborTextStringType && cbor_value_is_text_string(&spMap))
        {
            cborResult = cbor_value_dup_text_string(&spMap, &tagName, &tagLen, NULL);
            VERIFY_CBOR_SUCCESS(TAG, cborResult, "Failed to find next tag name in SP Map.");
            cborResult = cbor_value_advance(&spMap);
            VERIFY_CBOR_SUCCESS(TAG, cborResult, "Failed Advancing Value in SP Map.");
        }

        if(NULL != tagName)
        {
            // supportedprofiles
            if (strcmp(tagName, OIC_JSON_SUPPORTED_SP_NAME) == 0)
            {
                ret = SupportedProfilesFromCBOR(
                    &spMap, &sp->supportedProfiles, &sp->supportedLen);
                VERIFY_OR_LOG_AND_EXIT(TAG, OC_STACK_OK == ret,
                    "Failed to extract list of supported profiles", ERROR);
                if (NULL != decodedProperties)
                {
                    decodedProperties[SP_SUPPORTED_PROFILES] = true;
                }
            }

            // current profile
            else if (strcmp(tagName, OIC_JSON_CURRENT_SP_NAME) == 0)
            {
                ret = CurrentProfileFromCBOR(&spMap, &sp->currentProfile);
                VERIFY_OR_LOG_AND_EXIT(TAG, (OC_STACK_OK == ret) && (NULL != sp->currentProfile),
                    "Failed to extract SP current profile", ERROR);
                if (NULL != decodedProperties)
                {
                    decodedProperties[SP_CURRENT_PROFILE] = true;
                }
            }

            // advance to the next tag
            if (cbor_value_is_valid(&spMap))
            {
                cborResult = cbor_value_advance(&spMap);
                VERIFY_CBOR_SUCCESS(TAG, cborResult, "Failed Advancing SP Map.");
            }

            // cbor using malloc directly, so free() used instead of OICFree
            free(tagName);
            tagName = NULL;
        }
    }

    *secSp = sp;
    ret = OC_STACK_OK;

exit:
    if (CborNoError != cborResult)
    {
        OIC_LOG(ERROR, TAG, "CBORPayloadToSp failed");
        DeleteSpBinData(sp);
        sp = NULL;
        *secSp = NULL;
        ret = OC_STACK_ERROR;
    }

    return ret;
}

void DeleteSupportedProfiles(size_t supportedLen, char** supportedProfiles)
{
    if ((0 < supportedLen) && (NULL != supportedProfiles))
    {
        for(size_t i = 0; i < supportedLen; i++)
        {
            if(NULL != supportedProfiles[i])
            {
                OICFree(supportedProfiles[i]);
            }
        }
    }
    if (NULL != supportedProfiles)
    {
        OICFree(supportedProfiles);
    }
}

void DeleteSpBinData(OicSecSp_t* sp)
{
    if ((NULL != sp) && (sp != &gDefaultSp))
    {
        DeleteSupportedProfiles(sp->supportedLen, sp->supportedProfiles);
        sp->supportedLen = 0;
        sp->supportedProfiles = NULL;

        if (NULL != sp->currentProfile)
        {
            OICFree(sp->currentProfile);
        }
        sp->currentProfile = NULL;
    }
}

bool RequiredSpPropsPresentAndValid(OicSecSp_t* sp, bool *propertiesPresent)
{
    bool requiredPropsPresentAndValid = false;

    VERIFY_OR_LOG_AND_EXIT(TAG, (true == propertiesPresent[SP_SUPPORTED_PROFILES]),
        "Required SP property supportedprofiles not present", WARNING);

    VERIFY_OR_LOG_AND_EXIT(TAG, ((NULL != sp->supportedProfiles) && (0 < sp->supportedLen)),
        "Required SP property supportedprofiles list is empty", WARNING);

    VERIFY_OR_LOG_AND_EXIT(TAG, (true == propertiesPresent[SP_CURRENT_PROFILE]),
        "Required SP property currentprofile not present", WARNING);

    VERIFY_OR_LOG_AND_EXIT(TAG, (NULL != sp->currentProfile),
        "Required SP property currentprofile is invalid", WARNING);

    VERIFY_OR_LOG_AND_EXIT(TAG, (0 <= ProfileIdx(sp->supportedLen, sp->supportedProfiles, sp->currentProfile)),
        "Currentprofile is not contained in supportedprofiles list", WARNING);

    requiredPropsPresentAndValid = true;

exit:

    return requiredPropsPresentAndValid;
}

/**
 * Get the default value.
 *
 * @return the gDefaultSp pointer.
 */
static OicSecSp_t* GetSpDefault()
{
    return &gDefaultSp;
}

static bool ValidateQuery(const char * query)
{
    OIC_LOG (DEBUG, TAG, "In ValidateQuery");
    if(NULL == gSp)
    {
        return false;
    }

    bool bInterfaceQry = false;      // does querystring contains 'if' query ?
    bool bInterfaceMatch = false;    // does 'if' query matches with oic.if.baseline ?

    OicParseQueryIter_t parseIter = {.attrPos = NULL};

    ParseQueryIterInit((unsigned char*)query, &parseIter);

    while (GetNextQuery(&parseIter))
    {
        if (strncasecmp((char *)parseIter.attrPos, OC_RSRVD_INTERFACE, parseIter.attrLen) == 0)
        {
            bInterfaceQry = true;
            if ((strncasecmp((char *)parseIter.valPos, OC_RSRVD_INTERFACE_DEFAULT, parseIter.valLen) == 0))
            {
                bInterfaceMatch = true;
            }
        }
    }
    return (bInterfaceQry ? bInterfaceMatch: true);
}

/**
 * The entity handler determines how to process a GET request.
 */
static OCEntityHandlerResult HandleSpGetRequest (const OCEntityHandlerRequest * ehRequest)
{
    OCEntityHandlerResult ehRet = OC_EH_OK;

    OIC_LOG(INFO, TAG, "HandleSpGetRequest  processing GET request");

    //Checking if Get request is a query.
    if (ehRequest->query)
    {
        OIC_LOG_V(DEBUG,TAG,"query:%s",ehRequest->query);
        OIC_LOG(DEBUG, TAG, "HandlePstatGetRequest processing query");
        if (!ValidateQuery(ehRequest->query))
        {
            ehRet = OC_EH_ERROR;
        }
    }

    /*
     * For GET or Valid Query request return pstat resource CBOR payload.
     * For non-valid query return NULL payload.
     * A device will 'always' have a default Pstat, so PstatToCBORPayload will
     * return valid pstat resource json.
     */
    size_t size = 0;
    uint8_t *payload = NULL;
    if (ehRet == OC_EH_OK)
    {
        if(OC_STACK_OK != SpToCBORPayload(gSp, &payload, &size))
        {
            OIC_LOG_V(WARNING, TAG, "%s SpToCBORPayload failed.", __func__);
        }
    }

    // Send response payload to request originator
    ehRet = ((SendSRMResponse(ehRequest, ehRet, payload, size)) == OC_STACK_OK) ?
                   OC_EH_OK : OC_EH_ERROR;
    OICFree(payload);

    LogSp(gSp, DEBUG, TAG, "SP resource being sent in response to GET:");

    return ehRet;
}

static bool UpdatePersistentStorage(OicSecSp_t *sp)
{
    bool bRet = false;

    size_t size = 0;
    uint8_t *cborPayload = NULL;
    OCStackResult ret = SpToCBORPayload(sp, &cborPayload, &size);
    if (OC_STACK_OK == ret)
    {
        if (OC_STACK_OK == UpdateSecureResourceInPS(OIC_JSON_SP_NAME, cborPayload, size))
        {
            bRet = true;
        }
        OICFree(cborPayload);
    }

    return bRet;
}

static char** SpSupportedProfilesDup(size_t supportedLen, char** supportedProfilesSrc)
{
    OCStackResult status = OC_STACK_ERROR;
    char** supportedProfilesDup = NULL;

    VERIFY_OR_LOG_AND_EXIT(TAG, (0 < supportedLen),
        "sp supported profiles duplicate: invalid length for supportedprofiles array", ERROR);

    VERIFY_OR_LOG_AND_EXIT(TAG, (NULL != supportedProfilesSrc),
        "sp  supported profiles duplicate: supportedprofiles array not present", ERROR);

    // allocate and populate list of supported profiles
    supportedProfilesDup = (char**)OICCalloc(supportedLen, sizeof(char*));
    VERIFY_NOT_NULL(TAG, supportedProfilesDup, ERROR);

    for(size_t i = 0; i < supportedLen; i++)
    {
        if (NULL != supportedProfilesSrc[i])
        {
            supportedProfilesDup[i] = OICStrdup(supportedProfilesSrc[i]);
            VERIFY_NOT_NULL(TAG, supportedProfilesDup[i], ERROR);
        }
        else
        {
            OIC_LOG_V(WARNING, TAG, "SP supported profiles entry %lu is null, skipping", (unsigned long)i);

        }
    }

    status = OC_STACK_OK;

exit:
    if (OC_STACK_OK != status)
    {
        if (NULL != supportedProfilesDup)
        {
            DeleteSupportedProfiles(supportedLen, supportedProfilesDup);
        }
        return NULL;
    }

    return supportedProfilesDup;
}


// allocate new SP, setting props to spToDup.  Returns NULL on error
static OicSecSp_t* SpDup(OicSecSp_t* spToDup)
{
    OicSecSp_t *spToRet = NULL;

    OicSecSp_t *dupSp = (OicSecSp_t *)OICCalloc(1, sizeof(OicSecSp_t));
    VERIFY_NOT_NULL(TAG, dupSp, ERROR);

    dupSp->supportedLen = spToDup->supportedLen;
    dupSp->currentProfile = OICStrdup(spToDup->currentProfile);
    VERIFY_NOT_NULL(TAG, dupSp->currentProfile, ERROR);

    dupSp->supportedProfiles = SpSupportedProfilesDup(spToDup->supportedLen, spToDup->supportedProfiles);
    VERIFY_NOT_NULL(TAG, dupSp->supportedProfiles, ERROR);

    spToRet = dupSp;

exit:

    if (NULL == spToRet)
    {
        DeleteSpBinData(dupSp);
    }

    return spToRet;
}

OCStackResult InstallTestSp(OicSecSp_t* testSp)
{
    OCStackResult ret = OC_STACK_ERROR;
    OicSecSp_t* spCopy = SpDup(testSp);
    VERIFY_NOT_NULL(TAG, spCopy, ERROR);

    ret = OC_STACK_OK;

exit:

    if(OC_STACK_OK != ret )
    {
        DeleteSpBinData(spCopy);
    }
    else
    {
        DeleteSpBinData(gSp);
        gSp = spCopy;
    }

    return ret;
}

/**
 * The entity handler determines how to process a POST request.
 * Per the REST paradigm, POST can also be used to update representation of existing
 * resource or create a new resource.
 */
static OCEntityHandlerResult HandleSpPostRequest(OCEntityHandlerRequest *ehRequest)
{
    OIC_LOG_V(DEBUG, TAG, "IN %s", __func__);

    OCEntityHandlerResult ehRet = OC_EH_ERROR;
    OCStackResult ret = OC_STACK_ERROR;

    OicSecSp_t *spIncoming = NULL;
    OicSecSp_t *spUpdate = NULL;

    char** supportedProfilesSrc = NULL;
    char* currentProfileSrc = NULL;

    bool newSupportedProfiles = false;
    bool newCurrentProfile = false;

    const char * dosNames[] = { "RESET", "RFOTM", "RFPRO", "RFNOP", "SRESET" };
    (void) dosNames; // to avoid compiler warnings (some compilers don't recognize use in macros)

    uint8_t *payload = NULL;
    size_t size = 0;
    OicSecDostype_t dos;
    ret = GetDos(&dos);
    VERIFY_OR_LOG_AND_EXIT(TAG, (OC_STACK_OK == ret),
        "Not able to get onboarding state (pstat.dos) for /sp POST request", ERROR);

    OIC_LOG_V(DEBUG, TAG, "/sp POST request, pstat.dos state = %s (%d)", dosNames[dos.state], dos.state);
    if ((DOS_RESET == dos.state) || (DOS_RFNOP == dos.state)) {
        OIC_LOG(ERROR, TAG, "/sp resource is read-only in RESET and RFNOP");
        ehRet = OC_EH_NOT_ACCEPTABLE;
        goto exit;
    }

    VERIFY_OR_LOG_AND_EXIT(TAG, (NULL != ehRequest->payload), "sp POST : no payload supplied ", ERROR);
    VERIFY_OR_LOG_AND_EXIT(TAG, (NULL != gSp), "sp POST : corrupt internal SP resource ", ERROR);

    payload = ((OCSecurityPayload *) ehRequest->payload)->securityData;
    size = ((OCSecurityPayload *) ehRequest->payload)->payloadSize;
    VERIFY_NOT_NULL(TAG, payload, ERROR);

    bool decodedProperties[SP_PROPERTY_COUNT];
    ret = CBORPayloadToSp(payload, size, &spIncoming, decodedProperties);
    VERIFY_OR_LOG_AND_EXIT(TAG, ((OC_STACK_OK == ret) && (NULL != spIncoming)),
        "sp POST : error decoding incoming payload", ERROR);

    newSupportedProfiles = decodedProperties[SP_SUPPORTED_PROFILES];
    newCurrentProfile = decodedProperties[SP_CURRENT_PROFILE];

    spUpdate = (OicSecSp_t *)OICCalloc(1, sizeof(OicSecSp_t));
    VERIFY_NOT_NULL(TAG, spUpdate, ERROR);

    // supported profiles
    spUpdate->supportedLen = newSupportedProfiles ? spIncoming->supportedLen : gSp->supportedLen;
    supportedProfilesSrc = newSupportedProfiles ? spIncoming->supportedProfiles : gSp->supportedProfiles;
    spUpdate->supportedProfiles = SpSupportedProfilesDup(spUpdate->supportedLen, supportedProfilesSrc);
    VERIFY_OR_LOG_AND_EXIT(TAG, (NULL != spUpdate),
        "Problems duplicating supported profiles list for sp POST", WARNING);

    // log the impact of the post on supported profiles
    OIC_LOG_V(DEBUG, TAG, "Supported security profiles to be applied as a result of update (%lu entries, %s by POST)",
             (unsigned long)spUpdate->supportedLen, newSupportedProfiles ? "changed" : "unchanged");
    for (size_t i = 0; i < spUpdate->supportedLen; i++)
    {
        OIC_LOG_V(DEBUG, TAG, "  %lu: %s", (unsigned long)i, spUpdate->supportedProfiles[i]);
    }

    // current profile
    currentProfileSrc = newCurrentProfile ? spIncoming->currentProfile : gSp->currentProfile;
    spUpdate->currentProfile = OICStrdup(currentProfileSrc);
    VERIFY_NOT_NULL(TAG, spUpdate->currentProfile, ERROR);

    // log the impact of the post on current profile
    OIC_LOG_V(DEBUG, TAG, "Active profile to be applied as a result of update (%s by POST): %s",
              newCurrentProfile ? "changed" : "unchanged", spUpdate->currentProfile);

    // make sure current profile is in supported profiles list
    VERIFY_OR_LOG_AND_EXIT(TAG,
        (0 <= ProfileIdx(spUpdate->supportedLen, spUpdate->supportedProfiles, spUpdate->currentProfile)),
        "sp POST : currentprofile is not contained in supportedprofiles list", ERROR);

    // Before we update the sp, lets make sure everthing is valid
    VERIFY_OR_LOG_AND_EXIT(TAG,
        (true == RequiredSpPropsPresentAndValid(spUpdate, gAllProps)),
        "sp POST : update version of security profiles not valid, not updating", ERROR);

    // whew ...
    ehRet = OC_EH_OK;

exit:

    if ((OC_EH_OK == ehRet) && (NULL != spUpdate))
    {
        if( true != UpdatePersistentStorage(spUpdate))
        {
            OIC_LOG_V(DEBUG, TAG, "sp POST : Problems updating persistant storage");
            ehRet = OC_EH_NOT_ACCEPTABLE;

        }
        else
        {
            // replace our internal copy
            DeleteSpBinData(gSp);
            gSp = spUpdate;

            LogSp(gSp, DEBUG, TAG, "State of SP resource after being updated by POST:");

            ehRet = OC_EH_OK;
        }
    }
    else
    {
        ehRet = OC_EH_NOT_ACCEPTABLE;
    }

    if ((OC_EH_OK != ehRet) && (NULL != spUpdate))
    {
        DeleteSpBinData(spUpdate);
    }

    DeleteSpBinData(spIncoming);

    // Send response payload to request originator
    ehRet = ((SendSRMResponse(ehRequest, ehRet, NULL, 0)) == OC_STACK_OK) ?
        OC_EH_OK : OC_EH_ERROR;

    OIC_LOG_V(DEBUG, TAG, "OUT %s", __func__);

    return ehRet;
}

int ProfileIdx( size_t supportedLen, char **supportedProfiles, char *profileName)
{
    if ((NULL != supportedProfiles) && (NULL != profileName))
    {
        for (size_t i = 0; i < supportedLen; i++)
        {
            if ((NULL != supportedProfiles[i]) && (0 == strcmp(profileName, supportedProfiles[i])))
            {
                return (int)i;
            }
        }
    }
    return -1;
}

OCEntityHandlerResult SpEntityHandler(OCEntityHandlerFlag flag,
                                      OCEntityHandlerRequest * ehRequest,
                                      void* callbackParameter)
{
    OC_UNUSED(callbackParameter);
    OCEntityHandlerResult ehRet = OC_EH_ERROR;

    // This method will handle REST request (GET/POST) for /oic/sec/sp
    if (flag & OC_REQUEST_FLAG)
    {
        OIC_LOG(INFO, TAG, "Flag includes OC_REQUEST_FLAG");
        switch (ehRequest->method)
        {
            case OC_REST_GET:
                ehRet = HandleSpGetRequest(ehRequest);
                break;
            case OC_REST_POST:
                ehRet = HandleSpPostRequest(ehRequest);
                break;
            default:
                ehRet = ((SendSRMResponse(ehRequest, ehRet, NULL, 0)) == OC_STACK_OK) ?
                    OC_EH_OK : OC_EH_ERROR;
                break;
        }
    }
    return ehRet;
}

#undef SP_RESOURCE_DISABLE
OCStackResult CreateSpResource()
{
    OCStackResult ret = OC_STACK_OK;

#ifndef SP_RESOURCE_DISABLE
    ret = OCCreateResource(&gSpHandle,
                                         OIC_RSRC_TYPE_SEC_SP,
                                         OC_RSRVD_INTERFACE_DEFAULT,
                                         OIC_RSRC_SP_URI,
                                         SpEntityHandler,
                                         NULL,
                                         OC_SECURE |
                                         OC_DISCOVERABLE);
    if (OC_STACK_OK != ret)
    {
        OIC_LOG(FATAL, TAG, "Unable to instantiate sp resource");
        DeInitSpResource();
    }
#else
    OIC_LOG_V(WARNING, TAG, "/sp Resource disabled in this build; not creating Resource.");
#endif // SP_RESOURCE_DISABLE

    return ret;
}

OCStackResult DeInitSpResource()
{
    if (gSp != &gDefaultSp)
    {
        DeleteSpBinData(gSp);
        gSp = NULL;
    }
    return OCDeleteResource(gSpHandle);
}

OCStackResult InitSpResource()
{
    OCStackResult ret = OC_STACK_ERROR;

    // Read sp resource from PS
    uint8_t *data = NULL;
    size_t size = 0;
    ret = GetSecureVirtualDatabaseFromPS(OIC_CBOR_SP_NAME, &data, &size);

    // If database read failed
    if (OC_STACK_OK != ret)
    {
        OIC_LOG (DEBUG, TAG, "GetSecureVirtualDatabaseFromPS failed trying to read sp data");
    }
    if (data)
    {
        // Read sp resource from PS
        bool decodedProperties[SP_PROPERTY_COUNT];
        ret = CBORPayloadToSp(data, size, &gSp, decodedProperties);
        OICFree(data);

        if (false == RequiredSpPropsPresentAndValid(gSp, decodedProperties))
        {
            OIC_LOG (WARNING, TAG, "One or more required sp properties missing from  initialization database");
        }
    }

    if ((OC_STACK_OK != ret) || !gSp)
    {
        gSp = GetSpDefault();
    }
    VERIFY_NOT_NULL(TAG, gSp, FATAL);

    // Instantiate 'oic.sec.SP'
    ret = CreateSpResource();

exit:

    if (OC_STACK_OK != ret)
    {
        DeInitSpResource();
    }
    else
    {
        LogSp(gSp, DEBUG, TAG, "SP resource after startup initialization");
    }

    return ret;
}

bool IsPropArraySame(bool* spProps1, bool* spProps2)
{
    for (int i = 0; i < SP_PROPERTY_COUNT; i++)
    {
        if ( spProps1[i] != spProps2[i])
        {
            return false;
        }
    }
    return true;
}

bool IsSpSame(OicSecSp_t* sp1, OicSecSp_t* sp2, bool *propertiesToCheck)
{
    if ((NULL == sp1) || (NULL == sp2))
    {
        return false;
    }

    if (true == propertiesToCheck[SP_SUPPORTED_PROFILES] || (NULL == propertiesToCheck))
    {
        if ((NULL == sp1->supportedProfiles) || (NULL == sp2->supportedProfiles) ||
            (sp1->supportedLen != sp2->supportedLen))
        {
            return false;
        }

        // check for 100% overlap in supported profiles lists
        for (size_t i = 0; i < sp1->supportedLen; i++)
        {
            if (0 > ProfileIdx(sp1->supportedLen, sp1->supportedProfiles, sp2->supportedProfiles[i]))
            {
                return false;
            }
        }

    }

    if (true == propertiesToCheck[SP_CURRENT_PROFILE] || (NULL == propertiesToCheck))
    {
        if ((NULL == sp1->currentProfile) || (NULL == sp2->currentProfile) ||
            (0 != strcmp(sp1->currentProfile, sp2->currentProfile)))
        {
            return false;
        }
    }

    return true;
}

void SetAllSpProps(bool* spProps, bool setTo)
{
    for (int i = 0; i < SP_PROPERTY_COUNT; i++)
    {
        spProps[i] = setTo;
    }
}


void LogSp(OicSecSp_t* sp, int level, const char* tag, const char* msg)
{
    // some compilers not flagging the use of level and tag in the logging
    // macros as being used.  This is to get around these compiler warnings
    (void) level;
    (void) tag;
    (void) msg;

    if (NULL != msg)
    {
        OIC_LOG(level, tag, "-------------------------------------------------");
        OIC_LOG_V(level, tag, "%s", msg);
    }

    OIC_LOG(level, tag, "-------------------------------------------------");
    OIC_LOG_V(level, tag, "# security profiles supported: %lu", (unsigned long)sp->supportedLen);
    for (size_t i = 0; i < sp->supportedLen; i++)
    {
        OIC_LOG_V(level, tag, "  %lu: %s", (unsigned long)i, sp->supportedProfiles[i]);
    }
    OIC_LOG_V(level, tag, "Current security profile: %s", sp->currentProfile);
    OIC_LOG(level, tag, "-------------------------------------------------");
}
