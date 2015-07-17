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

#ifndef OC_RESOURCEHANDLER_H
#define OC_RESOURCEHANDLER_H

#include "ocstack.h"
#include "ocstackinternal.h"
#include "ocserverrequest.h"

/**
 * Common JSON string components used by the stack to build JSON strings.
 * These details are exposed in ocstackconfig.h file in the form of documentation.
 * Remember to update the documentation there if these are changed.
 */
#define OC_JSON_PREFIX                     "{\"oic\":["
#define OC_JSON_PREFIX_LEN                 (sizeof(OC_JSON_PREFIX) - 1)
#define OC_JSON_SUFFIX                     "]}"
#define OC_JSON_SUFFIX_LEN                 (sizeof(OC_JSON_SUFFIX) - 1)
#define OC_JSON_SEPARATOR                  ','
#define OC_JSON_SEPARATOR_STR              ","
#define OC_KEY_VALUE_DELIMITER             "="

/**
 * Static values for various JSON attributes.
 */
#define OC_RESOURCE_OBSERVABLE   1
#define OC_RESOURCE_SECURE       1

/**
 * OIC Virtual resources supported by every OIC device.
 */
typedef enum
{
    OC_UNKNOWN_URI =0,
    OC_WELL_KNOWN_URI,          ///< "/oic/res"
    OC_DEVICE_URI,              ///< "/oic/d"
    OC_PLATFORM_URI,            ///< "/oic/p"
    OC_RESOURCE_TYPES_URI,      ///< "/oic/res/types/d"
#ifdef WITH_PRESENCE
    OC_PRESENCE,                ///< "/oic/ad"
#endif
} OCVirtualResources;

/**
 * The type of query a request/response message is.
 */
typedef enum
{
    STACK_RES_DISCOVERY_NOFILTER = 0,
    STACK_RES_DISCOVERY_IF_FILTER,
    STACK_RES_DISCOVERY_RT_FILTER,
    STACK_DEVICE_DISCOVERY_DI_FILTER,
    STACK_DEVICE_DISCOVERY_DN_FILTER
} StackQueryTypes;

/**
 * The type of handling required to handle a request.
 */
typedef enum
{
    OC_RESOURCE_VIRTUAL = 0,
    OC_RESOURCE_NOT_COLLECTION_WITH_ENTITYHANDLER,
    OC_RESOURCE_NOT_COLLECTION_DEFAULT_ENTITYHANDLER,
    OC_RESOURCE_COLLECTION_WITH_ENTITYHANDLER,
    OC_RESOURCE_COLLECTION_DEFAULT_ENTITYHANDLER,
    OC_RESOURCE_DEFAULT_DEVICE_ENTITYHANDLER,
    OC_RESOURCE_NOT_SPECIFIED
} ResourceHandling;

/**
 * Default entity handler (ie. callback) to be used for resources with
 * no entity handler.
 */
OCEntityHandlerResult defaultResourceEHandler(OCEntityHandlerFlag flag,
        OCEntityHandlerRequest * request, void* callbackParam);

/**
 * Find and retrieve pointer to a resource associated with a specific resource
 * URI.
 */
OCResource *FindResourceByUri(const char* resourceUri);

/**
 * Returns true if the specificed resource URI aligns with a pre-existing
 * virtual resource; returns false otherwise.
 */
bool IsVirtualResource(const char* resourceUri);

/**
 * Parameter @ref handling returns by-reference the type of resource handling
 * required by the internal stack based on the specified @ref request.
 */
OCStackResult DetermineResourceHandling (const OCServerRequest *request,
                                         ResourceHandling *handling,
                                         OCResource **resource);

/**
 * Processes the specified @ref request based on the type of resource handling
 * @ref resHandling.
 */
OCStackResult ProcessRequest(ResourceHandling resHandling,
                             OCResource *resource,
                             OCServerRequest *request);

/**
 * Internal API used to save all of the platform's information for use in platform
 * discovery requests.
 */
OCStackResult SavePlatformInfo(OCPlatformInfo info);

/**
 * Internal API used to save all of the device's information for use in platform
 * discovery requests.
 * The device name is received from the appliation.
 * The deviceID, spec version and data model verson are initialized by the stack.
 */
OCStackResult SaveDeviceInfo(OCDeviceInfo info);

/**
 * Internal API used to clear the platform information.
 */
void DeletePlatformInfo();

/**
 * Internal API used to clear the device information.
 */
void DeleteDeviceInfo();

/*
 * Prepare payload for resource representation.
 */
OCStackResult BuildResponseRepresentation(const OCResource *resourcePtr,
                    OCRepPayload** payload);

/**
 * Prepares a Payload for response.
 */
OCStackResult BuildVirtualResourceResponse(const OCResource *resourcePtr,
                                           OCDiscoveryPayload* payload,
                                           OCDevAddr *endpoint);

/**
 * A helper function that Maps an @ref OCEntityHandlerResult type to an
 * @ref OCStackResult type.
 */
OCStackResult EntityHandlerCodeToOCStackCode(OCEntityHandlerResult ehResult);

#endif //OC_RESOURCEHANDLER_H

