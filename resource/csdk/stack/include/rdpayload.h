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

#ifndef OC_RDPAYLOAD_H_
#define OC_RDPAYLOAD_H_

#include <cbor.h>
#include "octypes.h"
#include "logger.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define OIC_RD_PUBLISH_TTL 86400

#define OIC_RD_DEFAULT_RESOURCE 2

#define DEFAULT_MESSAGE_TYPE "application/json"

/**
 * Converts RD payload from structure to CBOR format. It creates the outPayload
 * which is then transmitted over the wire.
 *
 * @param rdPayload Contains structure holding values of OCRDPayload.
 * @param outPayload The payload in the CBOR format converting OCRDPayload
 * structure.
 * @param size Length of the payload.
 *
 * @return ::CborNoError returns if successful and other Cbor error in  case of error.
 * failed in creating CBOR.
 */
int64_t OCRDPayloadToCbor(const OCRDPayload *rdPayload, uint8_t *outPayload, size_t *size);

/**
 * Converts CBOR to OCRDPayload.
 *
 * @param rdCBORPayload Payload received from other end in CBOR format.
 * @param outPayload Parsing the values from CBOR into OCRDPayload structure.
 *
 * @return ::OC_STACK_OK returns if successful and OC_STACK_ERROR returns if
 * failed in parsing CBOR.
 */
OCStackResult OCRDCborToPayload(const CborValue *cborPayload, OCPayload **outPayload);

/**
 * Initializes RD payload structure.
 *
 * @param payloadType Defines whether payload is RD_PAYLOAD_TYPE_DISCOVERY or
 * RD_PAYLOAD_TYPE_PUBLISH.
 *
 * @return Allocated memory for the OCRDPayload and NULL in case if failed to
 * allocate memory
 */
OCRDPayload *OCRDPayloadCreate();

#ifdef RD_CLIENT
/**
 * Initializes RD Publish payload structure.
 *
 * @param id An unique identifier of publishing device.
 * @param resourceHandles The handle of registered resource.
 * @param nHandles The number of registered resource handles.
 * @param ttl Time to live of the published resource.
 *
 * @return Allocated memory for the OCRDPayload and NULL in case if
 * failed to allocate memory.
 */
OCRDPayload *OCRDPublishPayloadCreate(const unsigned char *id,
                                      OCResourceHandle *resourceHandles, uint8_t nHandles,
                                      uint64_t ttl);
#endif

/**
 * Initializes RD Discovery payload structure and sets the bias factor.
 *
 * @param name Name of the discovery device payload.
 * @param identity Device identity of the discovery device.
 * @param biasFactor Value specifies the selection factor. It is weigthage of
 * CPU, Memory, Network, Load and Power capability of the RD server.
 *
 * @return Allocated memory for the OCRDDiscoveryPayload and NULL in case if
 * failed to allocate memory.
 */
OCRDDiscoveryPayload *OCRDDiscoveryPayloadCreate(const char *name, const char *identity, int biasFactor);

/**
 * Free memory allocation of the RDPayload and its internal structure.
 *
 * @param payload Pointer to already allocated memory for OCRDPayload.
 */
void OCRDPayloadDestroy(OCRDPayload *payload);

/**
 * Copies tag paramter to creates OCTagsPayload.
 *
 * @param deviceName The device name as set during enrollment.
 * @param id The device UUID.
 * @param ttl Time to leave for the . Used only in resource directory.
 *
 * @retun Allocated memory for OCTagsPayload or else NULL in case of error.
 */
OCTagsPayload* OCCopyTagsResources(const char *deviceName, const unsigned char *id, uint64_t ttl);

/**
 * Copies link resource to create LinksPayload.
 *
 * @param href URI of the resource
 * @param rel Relation
 * @param rt Array of String pointing to resource types.
 * @param itf Array of String pointing to interface
 * @param p Whether to observe or not.
 * @param title Title
 * @param anchor URI
 * @param ins Unique value per link.
 * @param ttl Time to live for this link.
 * @param mt Media Type

 * @retun Allocated memory for OCLinksResource or else NULL in case of error.
 */
OCLinksPayload* OCCopyLinksResources(const char *href, const char *rel, OCStringLL *rt,
                                     OCStringLL *itf, uint8_t p, const char *title,
                                     const char *anchor, uint8_t ins, uint64_t ttl,
                                     OCStringLL *mt);

/**
 * Creates a resource collection object.
 *
 * @param tags Pointer pointing to tags payload.
 * @param links Pointer pointing to links payload.
 *
 * @return Memory allocation for OCResourceCollectionPayload, else NULL.
 */
OCResourceCollectionPayload* OCCopyCollectionResource(OCTagsPayload *tags, OCLinksPayload *links);

/**
 * Destroys tags payload including internal structure allocated
 *
 * @param tags - Allocated memory of the tags payload.
 */
void OCFreeTagsResource(OCTagsPayload *tags);

/**
 * Destroys allocated links payload including internal structure allocated.
 *
 * @param links - Allocated memory to the links payload.
 */
void OCFreeLinksResource(OCLinksPayload *links);

/**
 * ResourceCollection payload destroy. Includes free up tags and links structure.
 *
 * @param payload Pointer pointing to allocated memroy of ResourceCollection.
 */
void OCFreeCollectionResource(OCResourceCollectionPayload *payload);

/**
 * Discovery collection payload destroy includes internal structure OCResourceCollectionPayload.
 *
 * @param payload Pointer pointing to allocated memory of OCDiscoveryPayload.
 */
void OCDiscoveryCollectionPayloadDestroy(OCDiscoveryPayload* payload);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* OC_RDPAYLOAD_H_ */
