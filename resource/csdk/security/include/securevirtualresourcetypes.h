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

/**
 * Data type definitions for all oic.sec.* types defined in the
 * OIC Security Specification.
 *
 * Note that throughout, ptrs are used rather than arrays.  There
 * are two primary reasons for this:
 * 1) The Spec defines many structures with optional fields, so pre-
 *    allocating these would be wasteful.
 * 2) There are in many cases arrays of Strings or arrays of Structs,
 *    which could not be defined as variable length arrays (e.g. array[])
 *    without breaking from the structure order and definition in the Spec.
 *
 * The primary drawback to this decision is that marshalling functions
 * will have to be written by hand to marshal these structures (e.g. to/from
 * Persistent Storage, or across memory boundaries).
 *
 * Last reconciled against Spec v0.92.
 */

//
// TODO WARNING: this code hasn't been tested except to compile!
// TODO NHS REMOVE this comment after code has been tested!
//

#ifndef OC_SECURITY_RESOURCE_TYPES_H
#define OC_SECURITY_RESOURCE_TYPES_H

#include <stdint.h> // for uint8_t typedef

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @enum    CABool_t
 * @brief   Boolean value used for specifying the success or failure.
 */
typedef enum
{
    SRM_FALSE = 0,   /**< False */
    SRM_TRUE = 1     /**< True */
} SRMBool_t;

/**
 * @brief   Values used to create bit-maskable enums for single-value
 *          response with embedded code.
 */
#define ACCESS_GRANTED_DEF            (1 << 0)
#define ACCESS_DENIED_DEF             (1 << 1)
#define INSUFFICIENT_PERMISSION_DEF   (1 << 2)
#define SUBJECT_NOT_FOUND_DEF         (1 << 3)
#define RESOURCE_NOT_FOUND_DEF        (1 << 4)
#define REASON_MASK_DEF               (1 << 5)

/**
 * @brief   Response type for all Action requests from CA layer;
 *          may include a reason code.
 *
 * To extract codes use GetReasonCode function on SRMAccessResponse:
 *
 * SRMAccessResponse_t response = SRMRequestHandler(obj, info);
 * if(SRM_TRUE == IsAccessGranted(response)) {
 *     SRMAccessResponseReasonCode_t reason = GetReasonCode(response);
 *     switch(reason) {
 *         case INSUFFICIENT_PERMISSION:
 *         ...etc.
 *     }
 * }
 */
typedef enum {
    ACCESS_GRANTED = ACCESS_GRANTED_DEF,
    ACCESS_DENIED = ACCESS_DENIED_DEF,
    ACCESS_DENIED_INSUFFICIENT_PERMISSION = ACCESS_DENIED_DEF
        | INSUFFICIENT_PERMISSION_DEF,
    ACCESS_DENIED_SUBJECT_NOT_FOUND = ACCESS_DENIED_DEF
        | SUBJECT_NOT_FOUND_DEF,
    ACCESS_DENIED_RESOURCE_NOT_FOUND = ACCESS_DENIED_DEF
        | RESOURCE_NOT_FOUND_DEF,
} SRMAccessResponse_t;

/**
 * Reason code for SRMAccessResponse.
 */
typedef enum {
    NO_REASON_GIVEN = 0,
    INSUFFICIENT_PERMISSION = INSUFFICIENT_PERMISSION_DEF,
    SUBJECT_NOT_FOUND = SUBJECT_NOT_FOUND_DEF,
    RESOURCE_NOT_FOUND = RESOURCE_NOT_FOUND_DEF,
} SRMAccessResponseReasonCode_t;

static inline SRMAccessResponseReasonCode_t GetReasonCode(
    SRMAccessResponse_t response)
{
    // switch(reponse) {
    //   case ACCESS_ALLOWED:
    //   case ACCESS_DENIED:
    //     return NO_REASON_GIVEN;
    //   case ACCESS_DENIED_INSUFFICIENT_PERMISSION:
    //     return
    // }

    SRMAccessResponseReasonCode_t reason = response | REASON_MASK_DEF;
    return reason;
}

static inline SRMBool_t IsAccessGranted(SRMAccessResponse_t response) {
    if(ACCESS_GRANTED == (response | ACCESS_GRANTED)) {
        return SRM_TRUE;
    } else {
        return SRM_FALSE;
    }
}

typedef struct OicSecAcl OicSecAcl_t;

typedef struct OicSecAmacl OicSecAmacl_t;

typedef struct OicSecCred OicSecCred_t;

/**
 * @brief   /oic/sec/credtype (Credential Type) data type.
 *          From OIC Security Spec v0.92:
 *              0:  no security mode
 *              1:  symmetric pair-wise key
 *              2:  symmetric group key
 *              4:  asymmetric key
 *              8:  signed asymmetric key (aka certificate)
 *              16: PIN /password
 */
typedef uint16_t OicSecCredType_t;

typedef struct OicSecDoxm OicSecDoxm_t;

typedef struct OicSecJwk OicSecJwk_t;

typedef struct OicSecPstat OicSecPstat_t;

typedef struct OicSecRole OicSecRole_t;

typedef struct OicSecSacl OicSecSacl_t;

typedef struct OicSecSvc OicSecSvc_t;

typedef struct OicUuid OicUuid_t;

/**
 * @brief   /oic/uuid (Universal Unique Identifier) data type.
 */
#define UUID_LENGTH 128/8 // 128-bit GUID length
struct OicUuid
{
    // <Attribute ID>:<Read/Write>:<Multiple/Single>:<Mandatory?>:<Type>
    //TODO fill in unless this is defined elsewhere?
    uint8_t             Id[UUID_LENGTH];
};

/**
 * @brief   /oic/sec/jwk (JSON Web Key) data type.
 *          See JSON Web Key (JWK)  draft-ietf-jose-json-web-key-41
 */
#define JWK_LENGTH 256/8 // 256 bit key length
struct OicSecJwk
{
    uint8_t             Data[JWK_LENGTH]; //TODO fill in with correct JWK defintion
};

/**
 * @brief   /oic/sec/acl (Access Control List) data type.
 *          Derived from OIC Security Spec v0.92... see spec
 *          for full explanation of each line item.
 */
struct OicSecAcl
{
    // <Attribute ID>:<Read/Write>:<Multiple/Single>:<Mandatory?>:<Type>
    char                *Subject;       // 0:R:S:Y:String
    size_t              ResourcesLen;   // the number of elts in Resources
    char                **Resources;    // 1:R:M:Y:String
    uint16_t            Permission;     // 2:R:S:Y:UINT16
    size_t              PeriodsLen;     // the number of elts in Periods
    char                **Periods;      // 3:R:M*:N:String (<--M*; see Spec)
    char                *Recurrences;   // 5:R:M:N:String
    size_t              OwnersLen;      // the number of elts in Owners
    OicSecSvc_t         *Owners;        // 6:R:M:Y:oic.sec.svc
    OicSecAcl_t         *next;
};

/**
 * @brief   /oic/sec/amacl (Access Manager Service Accesss Control List)
 *          data type.
 *          Derived from OIC Security Spec v0.92... see spec
 *          for full explanation of each line item.
 */
struct OicSecAmacl
{
    // <Attribute ID>:<Read/Write>:<Multiple/Single>:<Mandatory?>:<Type>
    size_t              ResourcesLen;   // the number of elts in Resources
    char                **Resources;    // 0:R:M:Y:String
    size_t              AmssLen;        // the number of elts in Amss
    OicSecSvc_t         *Amss;          // 1:R:M:Y:acl
    size_t              OwnersLen;      // the number of elts in Owners
    OicSecSvc_t         *Owners;        // 2:R:M:Y:oic.sec.svc
};

/**
 * @brief   /oic/sec/cred (Credential) data type.
 *          Derived from OIC Security Spec v0.92... see spec
 *          for full explanation of each line item.
 */
struct OicSecCred
{
    // <Attribute ID>:<Read/Write>:<Multiple/Single>:<Mandatory?>:<Type>
    uint16_t            CredId;         // 0:R:S:Y:UINT16
    OicUuid_t           SubjectId;      // 1:R:S:Y:oic.uuid
    size_t              RoleIdsLen;     // the number of elts in RoleIds
    OicSecRole_t        *RoleIds;       // 2:R:M:N:oic.sec.role
    OicSecCredType_t    CredType;       // 3:R:S:Y:oic.sec.credtype
    OicSecJwk_t         PublicData;     // 5:R:S:N:oic.sec.jwk
    OicSecJwk_t         PrivateData;    // 6:R:S:N:oic.sec.jwk*
    char                *Period;        // 7:R:S:N:String
    size_t              OwnersLen;      // the number of elts in Owners
    OicSecSvc_t         *Owners;        // 8:R:M:Y:oic.sec.svc
    OicSecCred_t        *next;
    //TODO fill in from OIC Security Spec v0.92
};

/**
 * @brief   /oic/sec/doxm (Device Owner Transfer Methods) data type
 */
struct OicSecDoxm
{
    // <Attribute ID>:<Read/Write>:<Multiple/Single>:<Mandatory?>:<Type>
    //TODO fill in from OIC Security Spec v0.92
};

/**
 * @brief   /oic/sec/pstat (Provisioning Status) data type.
 */
struct OicSecPstat
{
    // <Attribute ID>:<Read/Write>:<Multiple/Single>:<Mandatory?>:<Type>
    //TODO fill in from OIC Security Spec v0.92
};

/**
 * @brief   /oic/sec/role (Role) data type.
 */
struct OicSecRole
{
    // <Attribute ID>:<Read/Write>:<Multiple/Single>:<Mandatory?>:<Type>
    // TODO fill in with Role definition
};

/**
 * @brief   /oic/sec/sacl (Signed Access Control List) data type.
 */
struct OicSecSacl_t
{
    // <Attribute ID>:<Read/Write>:<Multiple/Single>:<Mandatory?>:<Type>
    //TODO fill in from OIC Security Spec v0.92
};

/**
 * @brief   /oic/sec/svc (Service requiring a secure connection) data type.
 */
struct OicSecSvc
{
    // <Attribute ID>:<Read/Write>:<Multiple/Single>:<Mandatory?>:<Type>
    //TODO fill in from OIC Security Spec v0.92
};

#ifdef __cplusplus
}
#endif

#endif //OC_SECURITY_RESOURCE_TYPES_H
