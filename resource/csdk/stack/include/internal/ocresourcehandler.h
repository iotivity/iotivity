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
 * Attributes used to form a proper OIC conforming JSON message.
 */
#define OC_RSRVD_OC                     "oc"
#define OC_RSRVD_PAYLOAD                "payload"
#define OC_RSRVD_HREF                   "href"
#define OC_RSRVD_REPRESENTATION         "rep"
#define OC_RSRVD_CONTENT_TYPE           "ct"
#define OC_RSRVD_RESOURCE_TYPE          "rt"
#define OC_RSRVD_RESOURCE_TYPE_PRESENCE "core.presence"
#define OC_RSRVD_INTERFACE              "if"
#define OC_RSRVD_DEVICE_ID              "di"
#define OC_RSRVD_DEVICE_NAME            "dn"
#define OC_RSRVD_INTERFACE_DEFAULT      "oc.mi.def"
#define OC_RSRVD_INTERFACE_LL           "oc.mi.ll"
#define OC_RSRVD_INTERFACE_BATCH        "oc.mi.b"
#define OC_RSRVD_INTERFACE_GROUP        "oc.mi.grp"
#define OC_RSRVD_MFG_DATE               "mndt"
#define OC_RSRVD_FW_VERSION             "mnfv"
#define OC_RSRVD_HOST_NAME              "hn"
#define OC_RSRVD_MFG_NAME               "mnmn"
#define OC_RSRVD_MFG_URL                "mnml"
#define OC_RSRVD_MODEL_NUM              "mnmo"
#define OC_RSRVD_PLATFORM_VERSION       "mnpv"
#define OC_RSRVD_SUPPORT_URL            "mnsl"
#define OC_RSRVD_VERSION                "icv"
#define OC_RSRVD_OBSERVABLE             "obs"
#define OC_RSRVD_SECURE                 "sec"
#define OC_RSRVD_HOSTING_PORT           "port"
#define OC_RSRVD_SERVER_INSTANCE_ID     "sid"

/**
 * Common JSON string components used by the stack to build JSON strings.
 */
#define OC_JSON_PREFIX                     "{\"oc\":["
#define OC_JSON_PREFIX_LEN                 (sizeof(OC_JSON_PREFIX) - 1)
#define OC_JSON_SUFFIX                     "]}"
#define OC_JSON_SUFFIX_LEN                 (sizeof(OC_JSON_SUFFIX) - 1)
#define OC_JSON_SEPARATOR                  ','
#define OC_JSON_SEPARATOR_STR              ","

/**
 * Static values for various JSON attributes.
 */
#define OC_RESOURCE_OBSERVABLE   1
#define OC_RESOURCE_SECURE       1

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
        OCEntityHandlerRequest * request);

/**
 * Get string value associated with a virtual resource type.
 */
const char * GetVirtualResourceUri(OCVirtualResources resource);

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
OCStackResult DetermineResourceHandling (OCServerRequest *request,
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
 * Internal API used to save all of the device's information for use in device
 * discovery requests.
 */
OCStackResult SaveDeviceInfo(OCDeviceInfo deviceInfo);

/**
 * Internal API used to clear the device information.
 */
void DeleteDeviceInfo();

/**
 * Prepares a JSON string for response.
 */
OCStackResult BuildVirtualResourceResponse(OCResource *resourcePtr,
                                           uint8_t filterOn,
                                           char *filterValue,
                                           char * out,
                                           uint16_t *remaining,
                                           CAConnectivityType_t connType);

/**
 * A helper function that Maps an @ref OCEntityHandlerResult type to an
 * @ref OCStackResult type.
 */
OCStackResult EntityHandlerCodeToOCStackCode(OCEntityHandlerResult ehResult);

#endif //OC_RESOURCEHANDLER_H

