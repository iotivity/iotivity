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

/**
 * @file
 *
 * This file contains the definition, types and interfaces for resource and attributes
 *
 */


#ifndef OCRESOURCE_H_
#define OCRESOURCE_H_

#define MAX_NAME_LENGTH (32)

/** Macro Definitions for observers */

/** Observer not interested. */
#define OC_OBSERVER_NOT_INTERESTED       (0)

/** Observer still interested. */
#define OC_OBSERVER_STILL_INTERESTED     (1)

/** Failed communication. */
#define OC_OBSERVER_FAILED_COMM          (2)

/**
 *Virtual Resource Presence Attributes
 */

/**
 * Forward declarations
 */

struct rsrc_t;

/**
 * Typedefs for stack interface
 * IF here stands for Interface
 */

typedef enum {
    STACK_IF_DEFAULT = 0,
    STACK_IF_LL,
    STACK_IF_BATCH,
    STACK_IF_GROUP,
    STACK_IF_INVALID
} OCStackIfTypes;

/**
 * following structure will be created in occollection.
 */

typedef struct occapability {
    /** Linked list; for multiple capabilities.*/
    struct occapability* next;

    /** It is a name about resource capability. */
    char *capability;

    /** It is mean status of capability. */
    char *status;
} OCCapability;

/**
 * following structure will be created in occollection.
 */

typedef struct ocaction {
    /** linked list; for multiple actions. */
    struct ocaction *next;

    /** Target Uri. It will be used to execute the action. */
    char *resourceUri;

    /** head pointer of a linked list of capability nodes.*/
    OCCapability* head;
} OCAction;

/**
 * following structure will be created in occollection.
 */

typedef struct ocactionset
{
    /** linked list; for list of action set. */
    struct ocactionset *next;

    /** Name of the action set.*/
    char *actionsetName;

    /** Time stamp.*/
    long int timesteps;

    /** Type of action.*/
    unsigned int type;

    /** head pointer of a linked list of Actions.*/
    OCAction* head;
} OCActionSet;

/**
 * Data structure for holding name and data types for each OIC resource.
 */
typedef struct resourcetext {
#ifndef RT_MAX
    /** linked list; for multiple types on resource. */
    struct resourcetext *next;
#endif
    char name[MAX_NAME_LENGTH];
} OCResourceText_t;

/**
 * Data structure for data type and definition for attributes that the resource exposes.
 */
typedef struct attr_t {

#ifndef RA_MAX
    /** Points to next resource in list.*/
    struct attr_t *next;
#endif

    /** The name of the attribute; used to look up the attribute in list.
     *  for a given attribute SHOULD not be changed once assigned.
     */
    const char *attrName;

    /** value of the attribute as string.*/
    char *attrValue;
} OCResourceAttribute;

/**
 * Data structure for holding data type and definition for OIC resource.
 */
typedef struct OCResource {

    /** Points to next resource in list.*/
    struct OCResource *next;

    /** Relative path on the device.*/
    char uri[URI_RESOURCE_LENGTH];

#ifdef RT_MAX
    int nTypes;
    OCResourceText_t rsrcType[RT_MAX];
#else
    /** Resource type(s); linked list.*/
    OCResourceText_t *rsrcType;
#endif

#ifdef RI_MAX
    int nInterfaces;
    OCResourceText_t rsrcInterface[RI_MAX];
#else
    /** Resource interface(s); linked list.*/
    OCResourceText_t *rsrcInterface;
#endif

#ifdef resourceattributeisactive
#ifdef RA_MAX
    int nAttrs;
    OCResourceAttribute rsrcAttributes[RA_MAX];
#else
    /** Resource interface(s); linked list.*/
    OCResourceAttribute *rsrcAttributes;
#endif
#endif

#ifdef WITH_COLLECTIONS
    /** Array of pointers to resources; can be used to represent a container of resources.
     * (i.e. hierarchies of resources) or for reference resources (i.e. for a resource collection).*/
    struct OCResource *rsrcResources[MAX_CONTAINED_RESOURCES];
#endif

#ifdef WITH_RESOURCE_API
    /** Pointer to function that handles the entity bound to the resource.
     *  This handler has to be explicitly defined by the programmer.*/
    OCEntityHandler entityHandler;

    /** Callback parameter.*/
    void *ehCallbackParam;
#endif

    /** Properties on the resource – defines meta information on the resource.
     * (ACTIVE, DISCOVERABLE etc ). */

    OCResourceProperty resourceProperties ;

    /* @note: Methods supported by this resource should be based on the interface targeted
     * i.e. look into the interface structure based on the query request Can be removed here;
     * place holder for the note above.*/
    /* method_t methods; */


    /** Sequence number for observable resources. Per the CoAP standard it is a 24 bit value.*/
    uint32_t sequenceNum;

#ifdef groupsisactive
    /** Pointer of ActionSet which to support group action.*/
    OCActionSet *actionsetHead;
#endif
} OCResource;

#ifdef WITH_PRESENCE
typedef struct PRESENCERESOURCE{
    bool initialized;
    bool active;
    uint32_t presenceTTL;
    OCToken_t token;
    OCResource resource;
} PresenceResource;

extern PresenceResource ThePresenceResource;
#endif // WITH_PRESENCE

#endif /* OCRESOURCE_H_ */
