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

#include <stdlib.h>
#include <string.h>
#include "oic_malloc.h"
#include "ocpayload.h"
#include "payload_logging.h"
#include "psinterface.h"
#include "resourcemanager.h"
#include "utlist.h"
#include "srmresourcestrings.h"
#include "srmutility.h"
#include "amaclresource.h"

#define TAG  "SRM-AMACL"

/** Default cbor payload size. This value is increased in case of CborErrorOutOfMemory.
 * The value of payload size is increased until reaching belox max cbor size. */
static const uint8_t CBOR_SIZE = 255;

/* Max cbor size payload. */
static const uint16_t CBOR_MAX_SIZE = 4400;

/** AMACL Map size - Number of mandatory items. */
static const uint8_t AMACL_MAP_SIZE = 3;

static OicSecAmacl_t *gAmacl = NULL;
static OCResourceHandle gAmaclHandle = NULL;

void DeleteAmaclList(OicSecAmacl_t* amacl)
{
    if (amacl)
    {
        OicSecAmacl_t *amaclTmp1 = NULL, *amaclTmp2 = NULL;
        LL_FOREACH_SAFE(amacl, amaclTmp1, amaclTmp2)
        {
            LL_DELETE(amacl, amaclTmp1);

            // Clean Resources
            for (size_t i = 0; i < amaclTmp1->resourcesLen; i++)
            {
                OICFree(amaclTmp1->resources[i]);
            }
            OICFree(amaclTmp1->resources);

            // Clean Amss
            OICFree(amaclTmp1->amss);

            // Clean Owners
            OICFree(amaclTmp1->owners);

            // Clean Amacl node itself
            OICFree(amaclTmp1);
        }
    }
}

static size_t OicSecAmaclCount(const OicSecAmacl_t *secAmacl)
{
    size_t size = 0;
    for (const OicSecAmacl_t *amacl = secAmacl; amacl; amacl = amacl->next)
    {
        size++;
    }
    return size;
}

OCStackResult AmaclToCBORPayload(const OicSecAmacl_t *amaclS, uint8_t **cborPayload,
                                 size_t *cborSize)
{
    if (NULL == amaclS || NULL == cborPayload || NULL != *cborPayload || NULL == cborSize)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult ret = OC_STACK_ERROR;
    size_t cborLen = *cborSize;
    if (0 == cborLen)
    {
        cborLen = CBOR_SIZE;
    }

    *cborSize = 0;
    *cborPayload = NULL;

    CborEncoder encoder = { {.ptr = NULL }, .end = 0 };
    CborEncoder amaclArray = { {.ptr = NULL }, .end = 0 };
    int64_t cborEncoderResult = CborNoError;

    const OicSecAmacl_t *amacl = amaclS;
    uint8_t *outPayload = (uint8_t *)OICCalloc(1, cborLen);
    VERIFY_NON_NULL(TAG, outPayload, ERROR);
    cbor_encoder_init(&encoder, outPayload, cborLen, 0);

    // Create AMACL Array
    cborEncoderResult |= cbor_encoder_create_array(&encoder, &amaclArray, OicSecAmaclCount(amacl));
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Addding AMACL Array.");

    while (amacl)
    {
        CborEncoder amaclMap = { {.ptr = NULL }, .end = 0 };
        cborEncoderResult |= cbor_encoder_create_map(&amaclArray, &amaclMap, AMACL_MAP_SIZE);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Addding AMACL MAP.");

        // Resources -- Mandatory
        {
            CborEncoder resources = { {.ptr = NULL }, .end = 0};
            cborEncoderResult |= cbor_encode_text_string(&amaclMap, OIC_JSON_RESOURCES_NAME,
                strlen(OIC_JSON_RESOURCES_NAME));
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Addding Resource Name Tag.");
            cborEncoderResult |= cbor_encoder_create_array(&amaclMap, &resources, amacl->resourcesLen);
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Addding Resource Name Array.");

            for (size_t i = 0; i < amacl->resourcesLen; i++)
            {
                cborEncoderResult |= cbor_encode_text_string(&resources, amacl->resources[i],
                    strlen(amacl->resources[i]));
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Addding Resource Name Value in Array.");

            }
            cborEncoderResult |= cbor_encoder_close_container(&amaclMap, &resources);
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Closing Resource Name ");
        }
        // Amss -- Mandatory
        {
            CborEncoder amss = { {.ptr = NULL }, .end = 0 };
            cborEncoderResult |= cbor_encode_text_string(&amaclMap, OIC_JSON_AMSS_NAME,
                strlen(OIC_JSON_AMSS_NAME));
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Addding AMSS Name Tag.");
            cborEncoderResult |= cbor_encoder_create_array(&amaclMap, &amss, amacl->amssLen);
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Addding AMSS Name Array.");
            for (size_t i = 0; i < amacl->amssLen; i++)
            {
                cborEncoderResult |= cbor_encode_byte_string(&amss, amacl->amss[i].id,
                    sizeof(amacl->amss[i].id));
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Addding AMSS Name Value.");
            }
            cborEncoderResult |= cbor_encoder_close_container(&amaclMap, &amss);
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Closing AMSS Array.");
        }
        // Owners -- Mandatory
        {
            cborEncoderResult |= cbor_encode_text_string(&amaclMap, OIC_JSON_OWNERS_NAME,
                strlen(OIC_JSON_OWNERS_NAME));
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Addding Owners Array Tag.");
            CborEncoder owners = { {.ptr = NULL }, .end = 0};
            cborEncoderResult |= cbor_encoder_create_array(&amaclMap, &owners, amacl->ownersLen);
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Addding Owners Array.");

            for (size_t i = 0; i < amacl->ownersLen; i++)
            {
                cborEncoderResult |= cbor_encode_byte_string(&owners, (uint8_t *)amacl->owners[i].id,
                    sizeof(amacl->owners[i].id));
                VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Addding Owners Array Value.");
            }
            cborEncoderResult |= cbor_encoder_close_container(&amaclMap, &owners);
            VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Closing Owners Array.");
        }
        cborEncoderResult |= cbor_encoder_close_container(&amaclArray, &amaclMap);
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Closing AMACL Map.");

        amacl = amacl->next;
    }
    cborEncoderResult |= cbor_encoder_close_container(&encoder, &amaclArray);
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Closing Amacl Array.");

    if (CborNoError == cborEncoderResult)
    {
        *cborPayload = outPayload;
        *cborSize = encoder.ptr - outPayload;
        ret = OC_STACK_OK;
    }

exit:
    if ((CborErrorOutOfMemory == cborEncoderResult) && (cborLen < CBOR_MAX_SIZE))
    {
       // reallocate and try again!
       OICFree(outPayload);
       outPayload = NULL;
       // Since the allocated initial memory failed, double the memory.
       cborLen += encoder.ptr - encoder.end;
       cborEncoderResult = CborNoError;
       ret = AmaclToCBORPayload(amaclS, cborPayload, &cborLen);
       if (OC_STACK_OK == ret)
       {
           *cborSize = cborLen;
           ret = OC_STACK_OK;
       }
    }

    if (CborNoError != cborEncoderResult)
    {
       OICFree(outPayload);
       outPayload = NULL;
       *cborSize = 0;
       *cborPayload = NULL;
       ret = OC_STACK_ERROR;
    }

    return ret;
}

OCStackResult CBORPayloadToAmacl(const uint8_t *cborPayload, size_t size,
                                 OicSecAmacl_t **secAmacl)
{
    if (NULL == cborPayload || NULL == secAmacl || NULL != *secAmacl)
    {
        return OC_STACK_INVALID_PARAM;
    }

    *secAmacl = NULL;

    OCStackResult ret = OC_STACK_ERROR;

    CborValue amaclCbor = { .parser = NULL };
    CborParser parser = { .end = NULL };
    CborError cborFindResult = CborNoError;
    int cborLen = size;
    if (0 == size)
    {
        cborLen = CBOR_SIZE;
    }
    cbor_parser_init(cborPayload, cborLen, 0, &parser, &amaclCbor);

    OicSecAmacl_t *headAmacl = NULL;

    CborValue amaclArray = { .parser = NULL };
    cborFindResult = cbor_value_enter_container(&amaclCbor, &amaclArray);
    VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Entering Amacl Array.");

    while (cbor_value_is_valid(&amaclArray))
    {
        CborValue amaclMap = { .parser = NULL };
        cborFindResult = cbor_value_enter_container(&amaclArray, &amaclMap);
        VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Entering Amacl Map.");

        OicSecAmacl_t *amacl = (OicSecAmacl_t *) OICCalloc(1, sizeof(*amacl));
        VERIFY_NON_NULL(TAG, amacl, ERROR);

        while (cbor_value_is_valid(&amaclMap))
        {
            char *name = NULL;
            size_t len = 0;
            cborFindResult = cbor_value_dup_text_string(&amaclMap, &name, &len, NULL);
            VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Entering Amacl Name.");
            cborFindResult = cbor_value_advance(&amaclMap);
            VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Entering Amacl Advance.");

            CborType type = cbor_value_get_type(&amaclMap);

            // Resources -- Mandatory
            if (0 == strcmp(OIC_JSON_RESOURCES_NAME, name))
            {
                CborValue resources = { .parser = NULL  };
                cborFindResult = cbor_value_get_array_length(&amaclMap, &amacl->resourcesLen);
                VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Finding Resource Name Array Len.");

                cborFindResult = cbor_value_enter_container(&amaclMap, &resources);
                VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Entering Amacl Array.");

                amacl->resources = (char **) OICCalloc(amacl->resourcesLen, sizeof(*amacl->resources));
                VERIFY_NON_NULL(TAG, amacl->resources, ERROR);
                int i = 0;
                while (cbor_value_is_text_string(&resources))
                {
                    cborFindResult = cbor_value_dup_text_string(&resources, &amacl->resources[i++],
                        &len, NULL);
                    VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Finding Resource Name Value.");
                    cborFindResult = cbor_value_advance(&resources);
                    VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Advancing Resource Name.");
                }
            }

            // Amss -- Mandatory
            if (0 == strcmp(OIC_JSON_AMSS_NAME, name))
            {
                CborValue amss = { .parser = NULL };
                cborFindResult = cbor_value_get_array_length(&amaclMap, &amacl->amssLen);
                VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Entering AMSS Name Array Len.");
                cborFindResult = cbor_value_enter_container(&amaclMap, &amss);
                VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Entering AMSS Name Container.");
                int i = 0;
                amacl->amss = (OicUuid_t *)OICCalloc(amacl->amssLen, sizeof(*amacl->amss));
                VERIFY_NON_NULL(TAG, amacl->amss, ERROR);
                while (cbor_value_is_valid(&amss))
                {
                    uint8_t *amssId = NULL;
                    cborFindResult = cbor_value_dup_byte_string(&amss, &amssId, &len, NULL);
                    VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Adding AMSS Id.");
                    cborFindResult = cbor_value_advance(&amss);
                    VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Advancing AMSS.");
                    memcpy(amacl->amss[i].id, amssId, len);
                    OICFree(amssId);
                }
            }

            // Owners -- Mandatory
            if (0 == strcmp(OIC_JSON_OWNERS_NAME, name))
            {
                CborValue owners = { .parser = NULL };
                cborFindResult = cbor_value_get_array_length(&amaclMap, &amacl->ownersLen);
                VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Adding Owner Name Len.");
                cborFindResult = cbor_value_enter_container(&amaclMap, &owners);
                VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Container Owner Array.");
                int i = 0;
                amacl->owners = (OicUuid_t *)OICCalloc(amacl->ownersLen, sizeof(*amacl->owners));
                VERIFY_NON_NULL(TAG, amacl->owners, ERROR);
                while (cbor_value_is_valid(&owners))
                {
                    uint8_t *owner = NULL;
                    cborFindResult = cbor_value_dup_byte_string(&owners, &owner, &len, NULL);
                    VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Adding Owners Array Value.");
                    cborFindResult = cbor_value_advance(&owners);
                    VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Advancing Owners Array");
                    memcpy(amacl->owners[i].id, owner, len);
                    OICFree(owner);
                }
            }
            if (CborMapType != type && cbor_value_is_valid(&amaclMap))
            {
                cborFindResult = cbor_value_advance(&amaclMap);
                VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Advancing Amacl Map.");
            }
            OICFree(name);
        }

        amacl->next = NULL;
        if (NULL == headAmacl)
        {
            headAmacl = amacl;
        }
        else
        {
            OicSecAmacl_t *temp = headAmacl;
            while (temp->next)
            {
                temp = temp->next;
            }
            temp->next = amacl;
        }
        if (cbor_value_is_valid(&amaclArray))
        {
            cborFindResult = cbor_value_advance(&amaclArray);
            VERIFY_CBOR_SUCCESS(TAG, cborFindResult, "Failed Advancing AMACL Array.");
        }
    }
    *secAmacl = headAmacl;
    ret = OC_STACK_OK;

exit:
    if (CborNoError != cborFindResult)
    {
        DeleteAmaclList(headAmacl);
        headAmacl = NULL;
        ret = OC_STACK_ERROR;
    }
    return ret;
}

static OCEntityHandlerResult HandleAmaclGetRequest (const OCEntityHandlerRequest * ehRequest)
{
    // Convert Amacl data into JSON for transmission
    size_t size = 0;
    uint8_t *cborPayload = NULL;
    OCStackResult res = AmaclToCBORPayload(gAmacl, &cborPayload, &size);

    OCEntityHandlerResult ehRet = (res == OC_STACK_OK) ? OC_EH_OK : OC_EH_ERROR;

    // Send response payload to request originator
    SendSRMCBORResponse(ehRequest, ehRet, cborPayload, size);

    OICFree(cborPayload);

    OIC_LOG_V (DEBUG, TAG, "%s RetVal %d", __func__ , ehRet);
    return ehRet;
}

static OCEntityHandlerResult HandleAmaclPostRequest (const OCEntityHandlerRequest * ehRequest)
{
    OCEntityHandlerResult ehRet = OC_EH_ERROR;

    // Convert CBOR Amacl data into binary. This will also validate the Amacl data received.
    uint8_t *payload = ((OCSecurityPayload *) ehRequest->payload)->securityData1;
    size_t size = ((OCSecurityPayload *) ehRequest->payload)->payloadSize;
    if (payload)
    {
        OicSecAmacl_t *newAmacl = NULL;
        OCStackResult res = CBORPayloadToAmacl(payload, size, &newAmacl);
        if (newAmacl && OC_STACK_OK == res)
        {
            // Append the new Amacl to existing Amacl
            LL_APPEND(gAmacl, newAmacl);
            size_t size = 0;
            // Convert Amacl data into JSON for update to persistent storage.
            uint8_t *cborPayload = NULL;
            res = AmaclToCBORPayload(gAmacl, &cborPayload, &size);
            if (cborPayload && (OC_STACK_OK == res) &&
                (OC_STACK_OK == UpdateSecureResourceInPS(OIC_JSON_AMACL_NAME, cborPayload, size)))
            {
                ehRet = OC_EH_RESOURCE_CREATED;
            }
            OICFree(cborPayload);
        }
        OICFree(payload);
    }

    // Send payload to request originator
    SendSRMCBORResponse(ehRequest, ehRet, NULL, 0);

    OIC_LOG_V(DEBUG, TAG, "%s RetVal %d", __func__ , ehRet);
    return ehRet;
}

/**
 * This internal method is the entity handler for Amacl resources and
 * will handle REST request (GET/PUT/POST/DEL) for them.
 */
static OCEntityHandlerResult AmaclEntityHandler (OCEntityHandlerFlag flag,
                                                 OCEntityHandlerRequest * ehRequest,
                                                 void* callbackParameter)
{
    (void) callbackParameter;
    OCEntityHandlerResult ehRet = OC_EH_ERROR;

    if (!ehRequest)
    {
        return ehRet;
    }

    if (flag & OC_REQUEST_FLAG)
    {
        OIC_LOG (DEBUG, TAG, "Flag includes OC_REQUEST_FLAG");
        switch (ehRequest->method)
        {
            case OC_REST_GET:
                ehRet = HandleAmaclGetRequest(ehRequest);
                break;

            case OC_REST_POST:
                ehRet = HandleAmaclPostRequest(ehRequest);
                break;

            default:
                ehRet = OC_EH_ERROR;
                SendSRMCBORResponse(ehRequest, ehRet, NULL, 0);
        }
    }

    return ehRet;
}

/**
 * This internal method is used to create '/oic/sec/amacl' resource.
 */
static OCStackResult CreateAmaclResource()
{
    OCStackResult ret = OCCreateResource(&gAmaclHandle,
                                         OIC_RSRC_TYPE_SEC_AMACL,
                                         OIC_MI_DEF,
                                         OIC_RSRC_AMACL_URI,
                                         AmaclEntityHandler,
                                         NULL,
                                         OC_OBSERVABLE);

    if (OC_STACK_OK != ret)
    {
        OIC_LOG (FATAL, TAG, "Unable to instantiate Amacl resource");
        DeInitAmaclResource();
    }
    return ret;
}

OCStackResult InitAmaclResource()
{
    OCStackResult ret = OC_STACK_ERROR;

    uint8_t *data = NULL;
    size_t size = 0;
    ret = GetSecureVirtualDatabaseFromPS(OIC_JSON_AMACL_NAME, &data, &size);

    // If database read failed
    if (OC_STACK_OK != ret)
    {
        OIC_LOG(DEBUG, TAG, "ReadSVDataFromPS failed");
    }
    if (data)
    {
        // Read AMACL resource from PS
        ret = CBORPayloadToAmacl(data, size, &gAmacl);
        OICFree(data);
    }

    // Instantiate 'oic/sec/amacl' resource
    ret = CreateAmaclResource();

    if (OC_STACK_OK != ret)
    {
        DeInitAmaclResource();
    }
    return ret;
}

void DeInitAmaclResource()
{
    OCDeleteResource(gAmaclHandle);
    gAmaclHandle = NULL;

    DeleteAmaclList(gAmacl);
    gAmacl = NULL;
}

OCStackResult AmaclGetAmsDeviceId(const char *resource, OicUuid_t *amsDeviceId)
{
    OicSecAmacl_t *amacl = NULL;

    VERIFY_NON_NULL(TAG, resource, ERROR);
    VERIFY_NON_NULL(TAG, amsDeviceId, ERROR);

    LL_FOREACH(gAmacl, amacl)
    {
        for(size_t i = 0; i < amacl->resourcesLen; i++)
        {
            if (0 == strncmp((amacl->resources[i]), resource, strlen(amacl->resources[i])))
            {
                //Returning the ID of the first AMS service for the resource
                memcpy(amsDeviceId, &amacl->amss[0], sizeof(*amsDeviceId));
                return OC_STACK_OK;
            }
        }
    }

exit:
    return OC_STACK_ERROR;
}
