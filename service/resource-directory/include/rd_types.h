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
#ifndef _RESOURCE_DIRECTORY_TYPES_H_
#define _RESOURCE_DIRECTORY_TYPES_H_

#include "ocpayload.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/** Resource Directory URI used to Discover RD and Publish resources.*/
#define OC_RSRVD_RD_URI                  "/oic/rd"

/** To represent resource type with rd.*/
#define OC_RSRVD_RESOURCE_TYPE_RD        "oic.wk.rd"

/** RD Discovery bias factor type. */
#define OC_RSRVD_RD_DISCOVERY_SEL        "sel"

/** To represent resource type with Publish RD.*/
#define OC_RSRVD_RESOURCE_TYPE_RDPUBLISH "oic.wk.rdPub"

/** Max ADDR SIZE */
#define MAX_ADDR_STR_SIZE (40)

/** Callback function for returning RDDiscovery Result. */
typedef int (* OCRDBiasFactorCB)(char addr[MAX_ADDR_STR_SIZE], uint16_t port);

/** Context structure used sending it as part of the callback context. */
typedef struct
{
    /** Stores the context value of the message sent. */
    void *context;
    /** Pointing to the callback function that OCRDDiscover() received. */
    OCRDBiasFactorCB cbFunc;
} OCRDClientContextCB;

/**
 * Structure holding discovery payload.
 */
typedef struct
{
    /** Value holding the bias factor of the RD. */
    uint8_t sel;
} OCRDDiscoveryPayload;

/**
 * Structure holding RD Links Payload. It is a sub-structure used in
 * OCRDPublishPayload.
 */
typedef struct OCRDLinksPayload
{
    /** Web Link Address of the resource. */
    char *href;
    /** Resource type of the resource. */
    char *rt;
    /** Interace type of the resource. */
    char *itf;
    /** Holding address of the next resource. */
    struct OCRDLinksPayload *next;
} OCRDLinksPayload;

/**
 * Structure holding RD Publish payload.
 */
typedef struct
{
    /** Device Name. */
    OCDeviceInfo deviceName;
    /** Device id. */
    OCIdentity deviceId;
    /** Time to keep holding resource.*/
    uint32_t ttl;
    /** List of resource information that will be stored at RD.*/
    OCRDLinksPayload *links;
} OCRDPublishPayload;

/**
 * Enum values of multiple RD type payload.
 */
typedef enum
{
    /** Value of the RD discovery payload. */
    RD_PAYLOAD_TYPE_DISCOVERY,
    /** Value of the RD publish payload. */
    RD_PAYLOAD_TYPE_PUBLISH,
    /** Value of the RD response in the payload. */
    RD_PAYLOAD_TYPE_RESPONSE
} OCRDPayloadType;

/**
 * RD Payload that will be transmitted over the wire.
 */
typedef struct
{
    /** Used in ocpayload to check type of the packet. This matches other OC payload.*/
    OCPayload base;
    /** Used internally in RD to check the payload whether discovery or publish.*/
    OCRDPayloadType payloadType;
    /** Pointer to the discovery response payload.*/
    OCRDDiscoveryPayload *rdDiscovery;
    /** Pointer to the publish payload.*/
    OCRDPublishPayload *rdPublish;
} OCRDPayload;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif
