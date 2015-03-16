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

#ifndef OC_SECURITY_CONFIG_H
#define OC_SECURITY_CONFIG_H

#include <stdint.h>

#define DTLS_PSK_ID_LEN 16
#define DTLS_PSK_PSK_LEN 16

#define OCSecConfigVer_1 1 /**< Initial version supporting PSK Credentials */
#define OCSecConfigVer_CurrentVersion OCSecConfigVer_1

typedef enum{
    OC_BLOB_TYPE_PSK = 1,   /**< Security blob holding PSK data */
} OCBlobType;


/**
 * Credentials of a peer device. Includes identity and the associated PSK.
 */
typedef struct
{
    unsigned char id[DTLS_PSK_ID_LEN];      /**< identity of the peer device */
    unsigned char psk[DTLS_PSK_PSK_LEN];    /**< psk of the peer device */
} OCDtlsPskCreds;


/**
 * Binary blob containing device identity and the credentials for all devices
 * trusted by this device.
 */
typedef struct
{
    unsigned char identity[DTLS_PSK_ID_LEN]; /**< identity of self */
    uint32_t num;                            /**< number of credentials in this blob */
    OCDtlsPskCreds creds[1];                 /**< list of credentials. Size of this
                                                 array is determined by 'num' variable. */
} OCDtlsPskCredsBlob;


/**
 * Generic definition of a security blob. A security blob can contain
 * info of various types, such as : PSK info, certificates,
 * access control lists(ACL) etc.
 */
typedef struct
{
    uint16_t  type;     /**< Type of blob */
    uint16_t  len;   /**< length of blob data */
    uint8_t   val[1];   /**< A variable size array holding blob data */
} OCSecBlob;


/**
 * This structure defines the security related configuration data for
 * Iotivity applications.
 */
typedef struct
{
   uint16_t    version;  /**< version of the config data */
   uint16_t    numBlob;  /**< number of security blobs in this config data */
   uint8_t     blob[1];  /**< A variable size array holding a blob */
} OCSecConfigData;

/**
 * Interprets @p blob as pointer to a OCSecBlob and advances to
 * the next blob in the OCSecConfigData
 */
#define config_data_next_blob(blob) \
            ((OCSecBlob*)((blob)->val + (blob)->len));

/**
 * Configuration data for security will be stored in below fashion in a
 * flat file on persistent storage.
 *
 *  --------------------------------------------------------------
 *  | OCSecConfigData| OCSecBlob #1 | OCSecBlob #2 | OCSecBlob #3|
 *  --------------------------------------------------------------
 */

#endif //OC_SECURITY_CONFIG_H



