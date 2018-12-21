/* *****************************************************************
 *
 * Copyright 2018 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * *****************************************************************/
#ifndef CLOUD_AUTH_H
#define CLOUD_AUTH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "octhread.h"

/**
 * Cloud status/error
 */
typedef enum
{
    OC_CLOUD_OK = 0,
    OC_CLOUD_PROV = 1,
    OC_CLOUD_SIGNUP = 2,
    OC_CLOUD_SIGNIN = 3,
    OC_CLOUD_SIGNOUT = 4,
    OC_CLOUD_TOKEN_REFRESH0 = 5,
    OC_CLOUD_TOKEN_REFRESH1 = 6,
    OC_CLOUD_TOKEN_REFRESH2 = 7,
    OC_CLOUD_TOKEN_REFRESH3 = 8,
    OC_CLOUD_TOKEN_REFRESH4 = 9,
    /* Error codes */
    OC_CLOUD_ERROR_REFRESHTOKEN = 244,
    OC_CLOUD_ERROR_REDIRECT = 245,
    OC_CLOUD_ERROR_TLS = 246,
    OC_CLOUD_ERROR_INVALID_ACCESS_TOKEN = 247,
    OC_CLOUD_ERROR_UNREACHABLE = 248,
    OC_CLOUD_ERROR_SIGNOUT = 249,
    OC_CLOUD_ERROR_SIGNIN = 250,
    OC_CLOUD_ERROR_CREATE_SESSION = 251,
    OC_CLOUD_ERROR_CHECK_SESSION = 252,
    OC_CLOUD_ERROR_SIGNUP = 253,
    OC_CLOUD_ERROR = 254,
    OC_CLOUD_EXIT = 255
} CloudStatus;

typedef struct
{
    char        *authProvider;
    char        *accessToken;
    char        *refreshToken;
    long        expireSin;
    char        *uid;
} session_t;

typedef struct OicCloud OicCloud_t;

/**
 * /oic/cloudconf
 * Data used by the device to connect to the OCF Cloud.
 */
struct OicCloud
{
    char        *apn;   // Authorization Provider Name
    char        *cis;   // OCF Cloud URL
    char        *at;    // Access Token
    char        *sid;   // Cloud UUID
#if !defined(__MANDATORY__)
    char        *redirectUri;
#endif // __MANDATORY__
    CloudStatus stat;
    char        *accessToken; //sign in access token
    oc_thread   pid;
    session_t   *session;
    OicCloud_t  *next;
};

#define OIC_JSON_CLOUDS_NAME    "clouds"
#define OIC_JSON_CLOUD_APN      "apn"
#define OIC_JSON_CLOUD_CIS      "cis"
#define OIC_JSON_CLOUD_AT       "at"
#define OIC_JSON_CLOUD_SID      "sid"
#define OIC_JSON_CLOUD_CLEC     "clec"

/**
 * Sends Sign UP request to cloud
 *
 * @param[in] cloud
 * @return ::OC_STACK_OK on success,  error indication otherwise
 */
OCStackResult OCCloudSignUp(OicCloud_t *cloud);

/**
 * Sends Sign IN request to cloud
 *
 * @param[in] cloud  cloud pointer
 * @return ::OC_STACK_OK on success,  error indication otherwise
 */
OCStackResult OCCloudSignIn(OicCloud_t *cloud);

/**
 * Sends Sign OUT request to cloud
 *
 * @param[in] cloud  cloud pointer
 * @return ::OC_STACK_OK on success,  error indication otherwise
 */
OCStackResult OCCloudSignOut(OicCloud_t *cloud);

/**
 * Session free function
 *
 * @param[in] cloud   cloud pointer
 * @return  pointer to next cloud entry
 */
OicCloud_t *FreeCloud(OicCloud_t *cloud);

/**
 * Stop update session
 * @param[in] cloud
 */
void StopCloudRefresh(OicCloud_t *cloud);

/**
 * Cloud to CBOR
 *
 * @param[in] clouds  cloud list to convert
 * @param[out] payload  generated CBOR payload
 * @param[out] size  size of generated CBOR payload
 * @return ::OC_STACK_OK on success,  error indication otherwise
 */
OCStackResult CloudToCBORPayload(const OicCloud_t *clouds, uint8_t **payload, size_t *size);

/**
 * Convert Cloud to CBOR for resource
 *
 * @param[in] clouds  cloud list to convert
 * @param[out] payload  generated CBOR payload
 * @param[out] size  size of generated CBOR payload
 * @return ::OC_STACK_OK on success,  error indication otherwise
 */
OCStackResult CloudToCBORPayloadResource(const OicCloud_t *clouds, uint8_t **payload, size_t *size);

/**
 * Convert CBOR to Cloud
 *
 * @param[in] cborPayload  payload to convert
 * @param[in] size  size of payload
 * @param[out] clouds  cloud list to generate
 * @return ::OC_STACK_OK on success,  error indication otherwise
 */
OCStackResult CBORPayloadToCloud(const uint8_t *cborPayload, size_t size, OicCloud_t **clouds);

/**
 * Cloud find
 * Find if cloud is present in list
 *
 * @param[in] list  cloud list
 * @param[in] cloud cloud to find
 * @return  clout, or NULL if not found
 */
OicCloud_t *CloudFind(OicCloud_t *list, const OicCloud_t *cloud);

/**
 * Format the status of a cloud into a string
 *
 * @param[in] cloud cloud pointer
 * @return string indicating status
 */
const char *GetCloudStatus(const OicCloud_t *cloud);

/**
 * Cloud validate
 *
 * @param[in] cloud
 * @return bool true - Ok
 */
bool ValidCloud(OicCloud_t *cloud);

/**
 * Cloud delete
 *
 * @param[in] cloud
 * @return  OCStackResult application result
 */
OCStackResult OCCloudDelete(OicCloud_t *cloud);

/**
 * Cloud copy
 *
 * @param[in] src cloud source
 * @param[in] dst cloud destination
 * @return bool true - Ok
 */
bool CloudCopy(const OicCloud_t *src, OicCloud_t *dst);

#ifdef __cplusplus
}
#endif

#endif // CLOUD_AUTH_H
