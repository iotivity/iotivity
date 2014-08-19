//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
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

#ifndef OC_RESOURCE_H
#define OC_RESOURCE_H

#include "ocstack.h"
#include "ocstackinternal.h"

#define OC_RSRVD_OC                     "oc"
#define OC_RSRVD_PAYLOAD                "payload"
#define OC_RSRVD_HREF                   "href"
#define OC_RSRVD_RESOURCE_TYPE          "rt"
#define OC_RSRVD_INTERFACE              "if"
#define OC_RSRVD_DEFAULT                "oc.mi.def"
#define OC_RSRVD_INTERFACE_LL           "oc.mi.ll"
#define OC_RSRVD_INTERFACE_BATCH        "oc.mi.b"
#define OC_RSRVD_OBSERVABLE             "obs"

#define OC_JSON_PREFIX                     "{\"oc\":["
#define OC_JSON_PREFIX_LEN                 (sizeof(OC_JSON_PREFIX) - 1)
#define OC_JSON_SUFFIX                     "]}"
#define OC_JSON_SUFFIX_LEN                 (sizeof(OC_JSON_SUFFIX) - 1)
#define OC_JSON_SEPARATOR                  ','

#define OC_RESOURCE_OBSERVABLE   1

typedef enum {
    STACK_RES_DISCOVERY_NOFILTER = 0,
    STACK_RES_DISCOVERY_IF_FILTER,
    STACK_RES_DISCOVERY_RT_FILTER
} StackQueryTypes;

typedef enum {
    OC_RESOURCE_VIRTUAL = 0,
    OC_RESOURCE_NOT_COLLECTION_WITH_ENTITYHANDLER,
    OC_RESOURCE_NOT_COLLECTION_DEFAULT_ENTITYHANDLER,
    OC_RESOURCE_COLLECTION_WITH_ENTITYHANDLER,
    OC_RESOURCE_COLLECTION_DEFAULT_ENTITYHANDLER,
    OC_RESOURCE_NOT_SPECIFIED
} ResourceHandling;

OCEntityHandlerResult defaultResourceEHandler(OCEntityHandlerFlag flag, OCEntityHandlerRequest * request);

const char * GetVirtualResourceUri( OCVirtualResources resource);
OCResource *FindResourceByUri(const char* resourceUri);
uint8_t IsVirtualResource(const char* resourceUri);

OCStackResult DetermineResourceHandling (OCRequest *request,
                                         ResourceHandling *handling,
                                         OCResource **resource);

OCStackResult
BuildJSONResponse(ResourceHandling resHandling, OCResource *resource, OCRequest *request);

OCEntityHandlerResult
BuildObsJSONResponse(OCResource *resource, OCEntityHandlerRequest *ehRequest);

OCStackResult
BuildDiscoveryResponse(OCResource *resourcePtr, uint8_t filterOn,
                        char *filterValue, char * out, uint16_t *remaining);

#endif //OC_RESOURCE_H
