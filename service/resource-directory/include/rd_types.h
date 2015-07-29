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

#define OC_RSRVD_RD_DISCOVERY_SEL        "sel"

/**
 * Structure holding discovery payload.
 */
typedef struct
{
    /** Value holding the bias factor of the RD. */
    uint8_t sel;
} OCRDDiscoveryPayload;

/**
 * Enum values of multiple RD type payload.
 */
typedef enum
{
    /** Value pf the RD discovery payload. */
    RD_PAYLOAD_TYPE_DISCOVERY
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
} OCRDPayload;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif
