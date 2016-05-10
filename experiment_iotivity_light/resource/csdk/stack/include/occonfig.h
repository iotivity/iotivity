//******************************************************************
//
// Copyright 2016 Intel Corporation All Rights Reserved.
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

//
// configuration macros for building client and server
//
// These are things potentially needed by everything.
//

#ifndef OCCONFIG_H_
#define OCCONFIG_H_

/** For debug code */
#define WITH_DEBUG  // mostly memory pools at this point

/** For including client */
//#define WITH_CLIENT  // now controlled by scons build option BUILD_CLIENT

/** For including full representation functions for payloads */
#define FULL_REPRESENTATION

/** For including allowing nested payloads */
#define NESTED_PAYLOADS

/** For the feature presence */
#define WITH_PRESENCE

/** For the feature collections */
#define WITH_COLLECTIONS

/** For the resource api */
#define WITH_RESOURCE_API

#ifndef WITH_RESOURCE_API
    #undef WITH_COLLECTIONS
#endif

/** For eliminating malloc in resources (all or none set) */
#define RT_MAX 2  // max resource types
#define RI_MAX 2  // max resource interfaces
//#define RA_MAX 2  // max resource attributes (not used at this time)

#define PAYLOAD_STRUCT_SIZE (1024)

#ifndef COAP_MAX_PDU_SIZE
#define COAP_MAX_PDU_SIZE (1400) /* maximum size of a CoAP PDU */
#endif

/**
 * Maximum length of the URI supported by client/server while processing
 * REST requests/responses.
 */
#define RESOURCE_URI_LENGTH (128)    // resource uri + query
#define URI_RESOURCE_LENGTH (64)     // resource uri resource only
#define URI_QUERY_LENGTH (64)        // resource uri query only
#define MAX_URI_LENGTH URI_RESOURCE_LENGTH // for applications only

/**
 * token length (maximum 8)
 * Shorter length provides shorter coap packets.
 * See CoAP spec for reasons for longer length.
 */
#define USE_TOKEN_LEN (6)

#define MAX_ADAPTERS (1)

/*
 * MEMORY USAGE
 */

/*
 * ADJUST USAGE SIZES -- define constrained memory usage
 */
#define SIMULTANEOUS_MESSAGES (2)
#define MAX_RESOURCES (20)
#define MAX_OBSERVATIONS (40)
#define MAX_COLLECTION_RESOURCES (40)

/*
 * MAXIMUM BUFFERS TACTIC -- pick one of these two
 */
//#define NO_MAXIMUM          // large heap available
#define HARD_MAXIMUMS       // heap size constrained

#ifdef NO_MAXIMUM
#define MAX_message (-1)
#define MAX_payload (-1)
#define MAX_resource (-1)
#define MAX_observer (-1)
#define MAX_collection (-1)
#define MAX_entity (-1)
#define MAX_PDUs (-1)
#endif

#ifdef HARD_MAXIMUMS
#define MAX_message (SIMULTANEOUS_MESSAGES)
#define MAX_payload (2*SIMULTANEOUS_MESSAGES)
#define MAX_resource (MAX_RESOURCES)
#define MAX_observer (MAX_OBSERVATIONS)
#define MAX_collection (MAX_COLLECTION_RESOURCES)
#define MAX_entity (SIMULTANEOUS_MESSAGES)
#define MAX_PDUs (SIMULTANEOUS_MESSAGES)
#endif

/*
 * STARTUP BUFFERS TACTIC -- pick one of these three
 */
//#define NO_ALLOCATION_AT_BEGINNING
#define SOME_ALLOCATION_AT_BEGINNING
//#define MAX_ALLOCATION_AT_BEGINNING

#ifdef NO_ALLOCATION_AT_BEGINNING
#define MAB_message (0)
#define MAB_payload (0)
#define MAB_resource (0)
#define MAB_observer (0)
#define MAB_collection (0)
#define MAB_entity (0)
#define MAB_PDUs (0)
#endif

#ifdef SOME_ALLOCATION_AT_BEGINNING
#define MAB_message (1)
#define MAB_payload (2)
#define MAB_resource (10)
#define MAB_observer (10)
#define MAB_collection (10)
#define MAB_entity (1)
#define MAB_PDUs (1)
#endif

#ifdef MAX_ALLOCATION_AT_BEGINNING
#define MAB_message MAX_messages
#define MAB_payload MAX_payload
#define MAB_resource MAX_resource
#define MAB_observer MAX_observer
#define MAB_collection MAX_collection
#define MAB_entity MAX_entity
#define MAB_PDUs MAX_PDUs
#endif

/*
 * SHARED BUFFERS TACTIC -- share buffers nearly the same size
 */
#define NO_SHARED_BUFFERS
//#define SOME_SHARED_BUFFERS

#ifdef NO_SHARED_BUFFERS
#define MAS_message (MAT_none)
#define MAS_payload (MAT_none)
#define MAS_resource (MAT_none)
#define MAS_observer (MAT_none)
#define MAS_collection (MAT_none)
#define MAS_entity (MAT_none)
#define MAS_PDUs (MAT_none)
#endif

#ifdef SOME_SHARED_BUFFERS
#define MAS_message (MAT_none)
#define MAS_payload (MAT_none)
#define MAS_resource (MAT_none)
#define MAS_observer (MAT_none)
#define MAS_collection (MAT_none)
#define MAS_entity (MAT_none)
#define MAS_PDUs (MAT_none)
#endif

/*
 * BUFFER SIZE DEPENDENCY -- each entry depends on a specific structure
 */
#define MAD_message (sizeof (OCRequestInfo_t))
#define MAD_payload (PAYLOAD_STRUCT_SIZE)
#define MAD_resource (sizeof (OCResource))
#define MAD_observer (sizeof (ResourceObserver))
#define MAD_collection (sizeof (OCServerResponse))
#define MAD_entity (sizeof (OCEntityHandlerRequest))
#define MAD_PDUs (COAP_MAX_PDU_SIZE)

// end of memory configuration

// Miscellaneous definitions
#if (__cplusplus >= 201103L) || defined(__GXX_EXPERIMENTAL_CXX0X__)
    #define SUPPORTS_DEFAULT_CTOR
#endif

#endif // OCCONFIG_H_
