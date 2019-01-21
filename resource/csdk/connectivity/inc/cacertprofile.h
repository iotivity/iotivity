//******************************************************************
// Copyright 2018 Cable Television Laboratories, Inc.
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
 * This file contains common utility functions for validating OCF certificate profiles
 */

#ifndef CA_CERT_PROFILE_H_
#define CA_CERT_PROFILE_H_

#if defined(__WITH_DTLS__) || defined (__WITH_TLS__)

#include <mbedtls/x509_crt.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Certificate Types
 */

typedef enum
{
    CERT_CA_ROOT = 0,
    CERT_CA_INT,
    CERT_EE
} CertType;

/**
 * Cert profile return values
 */
typedef enum
{
    CP_STATUS_OK  = 0,       /* Success */
    CP_NO_EE_CERT,           /* No end entity cert in cert chain */
    CP_MUL_EE_CERTS,         /* Multiple end entity certs in cert chain */
    CP_PROFILE_VIOLATION,    /* Certificate(s) don't satisfy OCF profile requirements */
    CP_INVALID_CERT_INPUT,   /* Certificate input is invalid (or null) */
    CP_DATE_ERROR,           /* Problem setting or reading certificate validity dates */
    CP_BUF_TOO_SMALL,        /* Supplied buffer is not long enough for desired operation */
    CP_MUL_CA_CERTS,         /* Multiple root CA certs in cert chain */
    CP_NO_CA_CERT,           /* No root CA cert in cert chain */
    CP_STATUS_FAILED = 255   /* Failure */
} CertProfileResult;

/**
 * Return values for cert chain functions that return a counts or -1 on failure
 */
#define CP_INVALID_CERT_CHAIN -1
#define CP_INVALID_CERT_LIST -1

/**
 * Macro to log an mbedtls error
 * For mbedtls functions that return 0 as non-error
 *
 * @param  log_tag    log tag
 * @param  ret        value returned by mbedtls call
 * @param  log_level  logging level
 * @param  buf        char* buffer for error string processing
 * @param  buf_size   size of bug
 *
 * NOTE: you must include "mbedtls/error.h" in order to use this
 */
#define CP_LOG_MBED_ERROR(log_tag, ret, buf, buf_size, log_level) do{ if (0!=(ret)) { \
    mbedtls_strerror((ret), (buf), (buf_size));                                    \
    OIC_LOG_V((log_level), (log_tag), "mbedtls error:  %s", (buf)); } }while(0)

/**
 * Cert profile violation flags
 */
typedef unsigned long CertProfileViolations;

#define CP_NO_VIOLATIONS                      (0)
#define CP_PROCESSING_ERROR                   (1 << 1)
#define CP_INVALID_SIG_ALGORITHM              (1 << 2)
#define CP_INVALID_VERSION                    (1 << 3)
#define CP_INVALID_PUB_KEY_ALGORITHM          (1 << 4)
#define CP_INVALID_KEY_USAGE_MISSING          (1 << 5)
#define CP_INVALID_KEY_USAGE_EXTRA            (1 << 6)
#define CP_INVALID_BASIC_CONSTRAINTS_CA       (1 << 7)
#define CP_INVALID_BASIC_CONSTRAINTS_PATH_LEN (1 << 8)
#define CP_INVALID_EKU_NO_SERVER_AUTH         (1 << 9)
#define CP_INVALID_EKU_NO_CLIENT_AUTH         (1 << 10)
#define CP_INVALID_EKU_NO_OCF_ID_OID          (1 << 11)
#define CP_INVALID_EKU_INCLUDES_ANY           (1 << 12)
#define CP_INVALID_ISSUER_SUBJ_MISMATCH       (1 << 13)
#define CP_NOT_YET_VALID                      (1 << 14)
#define CP_EXPIRED                            (1 << 15)


/**
 * Validate an end-entity certificate against OCF cert profile requirements
 *
 * @param[in]   cert   end entity cert to validate
 *
 * @return      Success: CP_NO_VIOLATIONS
 *              Errors: 1 or more violation bits set
 */
CertProfileViolations ValidateEndEntityCertProfile(const mbedtls_x509_crt *cert);

/**
 * Validate an intermediate-ca certificate against OCF cert profile requirements
 *
 * @param[in]   cert   end entity ca cert to validate
 *
 * @return      Success: CP_NO_VIOLATIONS
 *              Errors: 1 or more violation bits set
 */
CertProfileViolations ValidateIntermediateCACertProfile(const mbedtls_x509_crt *cert);

/**
 * Validate a root-ca certificate against OCF cert profile requirements
 *
 * @param[in]   cert   intermediate ca cert to validate
 *
 * @return      Success: CP_NO_VIOLATIONS
 *              Errors: 1 or more violation bits set
 */
CertProfileViolations ValidateRootCACertProfile(const mbedtls_x509_crt *cert);

/**
 * Validate a certificate's time window
 *
 * @param[in]   cert   cert to validate
 *
 * @return      Success: CP_NO_VIOLATIONS
 *              Errors: CP_NOT_YET_VALID and/or CP_EXPIRED
 */
CertProfileViolations ValidateCertTimeWindow(const mbedtls_x509_crt *cert);

/**
 * Given a cert chain intended for authentication, validated that the certs
 * within the chain conform to OCF certificate profile requirements.
 *
 * @param[in]   certChain  cert chain intended for authentication
 *
 * @return      Number of certs in the list that did not validate.
 *              0 = success
 *              CP_INVALID_CERT_CHAIN for invalid input chain
 *              > 0 = some number of certs did not validate
 */
int ValidateAuthCertChainProfiles(const mbedtls_x509_crt *certChain);

/**
 * Given a list of root CA certs, validated that the certs
 * within the list conform to OCF certificate profile requirements.
 *
 * @param[in]   certList      Chain chain intended for authentication
 *
 * @return      Number of certs in the list that did not validate.
 *              0 = success
 *              CP_INVALID_CERT_LIST for invalid input cert list
 *              > 0 = some number of certs did not validate
 */
int ValidateRootCACertListProfiles(const mbedtls_x509_crt *certList);

/**
 * Given a list of certificates, validate that they are currently
 * within each certificate's valid time window
 *
 * @param[in]   certList   List of certs for which to check time window
 *
 * @return      number of certs in the list that are NOT in valid time window relative to now
 *              0 = success
 *              CP_INVALID_CERT_LIST for invalid input cert list
 *              > 0 = some number of certs did not validate
 */
int CheckCertListTimeWindows(const mbedtls_x509_crt *certList);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // (__WITH_DTLS__) || defined (__WITH_TLS__)

#endif  /* CA_CERT_PROFILE_H_ */
