/* *****************************************************************
 *
 * Copyright 2017 Microsoft. All Rights Reserved.
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

#if defined(__WITH_TLS__) || defined(__WITH_DTLS__)

#include "iotivity_config.h"

#include "experimental/logger.h"
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>
#include "oic_malloc.h"
#include "oic_string.h"
#include "cacommon.h"
#include "experimental/ocrandom.h"
#include "cacommonutil.h"

#include "ocpayload.h"
#include "experimental/payload_logging.h"
#include "pmutility.h"
#include "srmutility.h"
#include "srmresourcestrings.h"

// headers required for mbed TLS
#include "mbedtls/config.h"
#include "mbedtls/platform.h"
#include "mbedtls/error.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/x509_csr.h"
#include "mbedtls/oid.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/oid.h"
#include "mbedtls/pem.h"
#include "mbedtls/base64.h"
#include "mbedtls/asn1write.h"

#ifndef NDEBUG
#include "mbedtls/debug.h"
#include "mbedtls/version.h"
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <fcntl.h>

#include "certhelpers.h"
#include "occertutility.h"

#define TAG "OIC_OCCERTUTILITY"

/**
 * @def PERSONALIZATION_STRING
 * @brief Personalization string for the mbedtls RNG
 */
#define PERSONALIZATION_STRING "IOTIVITY_RND"

#define MAX_URI_QUERY MAX_URI_LENGTH + MAX_QUERY_LENGTH

#define MAX_STRING_LEN 254

/* ASN.1 DER encoding of the EKU for identity certificates */
static const unsigned char s_ekuIdentity[] = {
    0x30, 0x20,
    0x06, 0x08, 0x2B, 0x06, 0x01, 0x05, 0x05, 0x07, 0x03, 0x01, // serverAuth (1.3.6.1.5.5.7.3.1)
    0x06, 0x08, 0x2B, 0x06, 0x01, 0x05, 0x05, 0x07, 0x03, 0x02, // clientAuth (1.3.6.1.5.5.7.3.2)
    0x06, 0x0A, 0x2B, 0x06, 0x01, 0x04, 0x01, 0x82, 0xDE, 0x7C, 0x01, 0x06 // OCF ID OID (1.3.6.1.4.1.44924.1.6)
};

/* ASN.1 DER encoding of the EKU for role certificates (1.3.6.1.4.1.44924.1.7) */
static const unsigned char s_ekuRole[] = { 0x30, 0x0C, 0x06, 0x0A, 0x2B, 0x06, 0x01, 0x04, 0x01, 0x82, 0xDE, 0x7C, 0x01, 0x07 };

/* ASN.1 DER encoding of the EKU for both identity and roles (for use by CAs) */
static const unsigned char s_ekuCA[] = { 0x30, 0x18, 0x06, 0x0A, 0x2B, 0x06, 0x01, 0x04, 0x01, 0x82, 0xDE, 0x7C, 0x01, 0x06, 0x06, 0x0A, 0x2B, 0x06, 0x01, 0x04, 0x01, 0x82, 0xDE, 0x7C, 0x01, 0x07 };

static const char s_ComplianceExtOid[] = MBEDTLS_OID_ISO_IDENTIFIED_ORG MBEDTLS_OID_ORG_DOD "\x01\x04\x01\x83\x91\x56\x01\x00"; // 1.3.6.1.4.1.51414.1.0
static const uint8_t s_ComplianceExtBytes[] = {

  0x30, 0x81, 0x94, // compliance extension sequence

  0x30, 0x09, // version sequence (9 bytes)
  0x02, 0x01, 0x02, 0x02, 0x01, 0x00, 0x02, 0x01, 0x00, // [2, 0, 0]

  0x30, 0x6C, // security profile sequence (108 bytes)

    0x0C, 0x19, // utf8_string, 25 bytes long, '1.3.6.1.4.1.51414.0.0.1.0' (baseline)
    0x31, 0x2E, 0x33, 0x2E, 0x36, 0x2E, 0x31, 0x2E, 0x34, 0x2E,
    0x31, 0x2E, 0x35, 0x31, 0x34, 0x31, 0x34, 0x2E, 0x30, 0x2E, 0x30, 0x2E, 0x31, 0x2E, 0x30,

    0x0C, 0x19, // utf8_string, 25 bytes long, '1.3.6.1.4.1.51414.0.0.2.0' (black)
    0x31, 0x2E, 0x33, 0x2E, 0x36, 0x2E, 0x31, 0x2E, 0x34, 0x2E,
    0x31, 0x2E, 0x35, 0x31, 0x34, 0x31, 0x34, 0x2E, 0x30, 0x2E, 0x30, 0x2E, 0x32, 0x2E, 0x30,

    0x0C, 0x19, // utf8_string, 25 bytes long, '1.3.6.1.4.1.51414.0.0.3.0' (blue)
    0x31, 0x2E, 0x33, 0x2E, 0x36, 0x2E, 0x31, 0x2E, 0x34, 0x2E,
    0x31, 0x2E, 0x35, 0x31, 0x34, 0x31, 0x34, 0x2E, 0x30, 0x2E, 0x30, 0x2E, 0x33, 0x2E, 0x30,

    0x0C, 0x19, // utf8_string, 25 bytes long, '1.3.6.1.4.1.51414.0.0.4.0' (purple)
    0x31, 0x2E, 0x33, 0x2E, 0x36, 0x2E, 0x31, 0x2E, 0x34, 0x2E,
    0x31, 0x2E, 0x35, 0x31, 0x34, 0x31, 0x34, 0x2E, 0x30, 0x2E, 0x30, 0x2E, 0x34, 0x2E, 0x30,

  0x0C, 0x0F, // urf8_string 15 bytes long (device name)
  0x49, 0x6F, 0x54, 0x69, 0x76, 0x69, 0x74, 0x79, 0x20, 0x53, // 'IoTivity Server'
  0x65, 0x72, 0x76, 0x65, 0x72,

  0x0C, 0x08, // urf8_string 8 bytes long (device manufacturer)
  0x49, 0x6F, 0x54, 0x69, 0x76, 0x69, 0x74, 0x79 // 'IoTivity'
};


static const char s_cplSecurityClaimsExtOid[] = MBEDTLS_OID_ISO_IDENTIFIED_ORG MBEDTLS_OID_ORG_DOD "\x01\x04\x01\x83\x91\x56\x01\x01"; // 1.3.6.1.4.1.51414.1.1
static const uint8_t s_cplSecurityClaimsExtBytes[] = {
    0x30, 0x1A, // sequence of length 26
    0x06, 0x0B, 0x2B, 0x06, 0x01, 0x04, 0x01, 0x83, 0x91, 0x56, 0x01, 0x01, 0x00, // OID 1.3.6.1.4.1.51414.1.1.0 (claim secure boot)
    0x06, 0x0B, 0x2B, 0x06, 0x01, 0x04, 0x01, 0x83, 0x91, 0x56, 0x01, 0x01, 0x01  // OID 1.3.6.1.4.1.51414.1.1.1 (claim hw backed credential)
};

static const char s_cplAttributesExtOid[] = MBEDTLS_OID_ISO_IDENTIFIED_ORG MBEDTLS_OID_ORG_DOD "\x01\x04\x01\x83\x91\x56\x01\x02"; // 1.3.6.1.4.1.51414.1.2
static const uint8_t s_cplAttributesExtBytes[] = {
    0x30, 0x20, // sequence of length 32

    0x0C, 0x0E, // utf8_string, 14 bytes long
    0X31, 0X2E, 0X33, 0X2E, 0X36, 0X2E, 0X31, 0X2E, 0X34, 0X2E, 0X31, 0X2E, 0X37, 0X31,  // '1.3.6.1.4.1.71'

    0x0C, 0x09, // utf8_string, 9 bytes long
    0X44, 0X69, 0X73, 0X63, 0X6F, 0X76, 0X65, 0X72, 0X79, // 'Discovery'

    0x0C, 0x03, // utf8_string, 3 bytes long
    0X31, 0X2E, 0X30 // '1.0'
};


OCStackResult OC_CALL OCGenerateRandomSerialNumber(char **serial, size_t *serialLen)
{
    int ret = 0;
    OCStackResult res = OC_STACK_ERROR;
    unsigned char random[20]; /* Per RFC 5280, 20 octets is the maximum length of a serial number. */
    mbedtls_mpi serialMpi;

    VERIFY_NOT_NULL_RETURN(TAG, serial, ERROR, OC_STACK_INVALID_PARAM);
    VERIFY_NOT_NULL_RETURN(TAG, serialLen, ERROR, OC_STACK_INVALID_PARAM);

    mbedtls_mpi_init(&serialMpi);
    memset(serial, 0, sizeof(*serial));

    VERIFY_SUCCESS(TAG, OCGetRandomBytes(random, sizeof(random)), ERROR);

    /* Per RFC 5280, 20 octets is the maximum length of a serial number. In ASN.1, if the highest-order
     * bit is set it causes a padding octet to be written, which would be 21 and non-compliant.
     * Therefore, always clear the highest-order bit. Integers in ASN.1 are always big-Endian.
     */
    random[0] &= 0x7F;

    /* Import into a large integer object and then output as a string. */
    ret = mbedtls_mpi_read_binary(&serialMpi, random, sizeof(random));
    VERIFY_SUCCESS(TAG, 0 == ret, ERROR);

    /* Get the needed string length and allocate. */
    ret = mbedtls_mpi_write_string(&serialMpi, 10, NULL, 0, serialLen);
    VERIFY_SUCCESS(TAG, ret == MBEDTLS_ERR_MPI_BUFFER_TOO_SMALL, ERROR);
    *serial = OICCalloc(1, *serialLen);
    VERIFY_NOT_NULL(TAG, *serial, ERROR);

    /* Do the write for real. */
    ret = mbedtls_mpi_write_string(&serialMpi, 10, *serial, *serialLen, serialLen);
    VERIFY_SUCCESS(TAG, 0 == ret, ERROR);

    res = OC_STACK_OK;

exit:

    if (OC_STACK_OK != res)
    {
        OICFree(*serial);
        *serial = NULL;
        *serialLen = 0;
    }
    mbedtls_mpi_free(&serialMpi);

    return res;
}

OCStackResult OC_CALL OCGenerateKeyPair(char **publicKey, size_t *publicKeyLen,
                                        char **privateKey, size_t *privateKeyLen)
{
    int ret = 0;
    mbedtls_pk_context keyPair;
    unsigned char buf[2048];

    mbedtls_pk_init(&keyPair);

    VERIFY_NOT_NULL_RETURN(TAG, publicKey, ERROR, OC_STACK_INVALID_PARAM);
    VERIFY_NOT_NULL_RETURN(TAG, publicKeyLen, ERROR, OC_STACK_INVALID_PARAM);
    VERIFY_NOT_NULL_RETURN(TAG, privateKey, ERROR, OC_STACK_INVALID_PARAM);
    VERIFY_NOT_NULL_RETURN(TAG, privateKeyLen, ERROR, OC_STACK_INVALID_PARAM);

    *publicKey = NULL;
    *publicKeyLen = 0;
    *privateKey = NULL;
    *privateKeyLen = 0;

    ret = OCInternalGenerateKeyPair(&keyPair);
    if (ret != 0)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to generate key pair: %d", ret);
        goto exit;
    }

    ret = mbedtls_pk_write_pubkey_pem(&keyPair, buf, sizeof(buf));
    if (ret != 0)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to export public key as PEM: %d", ret);
        goto exit;
    }

    *publicKeyLen = strlen((char *)buf) + 1;
    *publicKey = OICCalloc(1, *publicKeyLen);
    if (NULL == *publicKey)
    {
        OIC_LOG(ERROR, TAG, "Could not allocate memory for public key");
        ret = -1;
        goto exit;
    }
    memcpy(*publicKey, buf, *publicKeyLen);

    ret = mbedtls_pk_write_key_pem(&keyPair, buf, sizeof(buf));
    if (ret != 0)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to export private key as PEM: %d", ret);
        goto exit;
    }

    *privateKeyLen = strlen((char *)buf) + 1;
    *privateKey = OICCalloc(1, *privateKeyLen);
    if (NULL == *privateKey)
    {
        OIC_LOG(ERROR, TAG, "Could not allocate memory for private key");
        ret = -1;
        goto exit;
    }
    memcpy(*privateKey, buf, *privateKeyLen);

exit:

    mbedtls_pk_free(&keyPair);

    OICClearMemory(buf, sizeof(buf));

    if (ret != 0)
    {
        OICFree(*publicKey);
        OICClearMemory(*privateKey, *privateKeyLen);
        OICFree(*privateKey);

        *publicKey = NULL;
        *publicKeyLen = 0;
        *privateKey = NULL;
        *privateKeyLen = 0;

        return OC_STACK_ERROR;
    }
    else
    {
        return OC_STACK_OK;
    }
}

typedef enum {
    CERT_TYPE_ROOT_CA,
    CERT_TYPE_INTERMEDIATE_CA,
    CERT_TYPE_IDENTITY,
    CERT_TYPE_ROLE
} CertificateType_t;


// write basic constraints to a cert
// Same as mbedtls_x509write_crt_set_basic_constraints, with the added ability to set `critical` flag
static OCStackResult OCWriteBasicConstraints( mbedtls_x509write_cert *ctx, int is_ca, int max_pathlen, int critical)
{

    int ret;
    char mbedErrorBuf[256];
    unsigned char buf[9];
    unsigned char *c = buf + sizeof(buf);
    size_t len = 0;

    memset( buf, 0, sizeof(buf) );

    if( is_ca && max_pathlen > 127 )
        return( MBEDTLS_ERR_X509_BAD_INPUT_DATA );

    if( is_ca )
    {
        if( max_pathlen >= 0 )
        {
            MBEDTLS_ASN1_CHK_ADD( len, mbedtls_asn1_write_int( &c, buf, max_pathlen ) );
        }
        MBEDTLS_ASN1_CHK_ADD( len, mbedtls_asn1_write_bool( &c, buf, 1 ) );
    }

    MBEDTLS_ASN1_CHK_ADD( len, mbedtls_asn1_write_len( &c, buf, len ) );
    MBEDTLS_ASN1_CHK_ADD( len, mbedtls_asn1_write_tag( &c, buf, MBEDTLS_ASN1_CONSTRUCTED |
                                                       MBEDTLS_ASN1_SEQUENCE ) );

    ret = mbedtls_x509write_crt_set_extension( ctx, MBEDTLS_OID_BASIC_CONSTRAINTS,
                                                   MBEDTLS_OID_SIZE( MBEDTLS_OID_BASIC_CONSTRAINTS ),
                                                   critical, buf + sizeof(buf) - len, len );
    LOG_MBED_ERROR(TAG, ret, mbedErrorBuf, sizeof(ret), ERROR);
    return (0 == ret) ? OC_STACK_OK : OC_STACK_ERROR;
}


static OCStackResult GenerateCertificate(
    CertificateType_t certType,
    const char *subject,
    const char *subjectPublicKey,
    const char *issuerCert,
    const char *issuerPrivateKey,
    const char *serial,
    const char *notValidBefore,
    const char *notValidAfter,
    const char *role,
    const char *authority,
    OCByteString *certificate)
{
    OCStackResult res = OC_STACK_INVALID_PARAM;
    int ret = 0;
    mbedtls_x509write_cert outCertCtx;
    mbedtls_pk_context subjKeyCtx;
    mbedtls_pk_context issKeyCtx;
    mbedtls_x509_crt issCertCtx;
    mbedtls_mpi serialMpi;
    mbedtls_x509_general_names names;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;

    char buf[2048];
    char mbedErrorBuf[256];


    if (NULL == subjectPublicKey || NULL == issuerPrivateKey || NULL == subject || NULL == serial ||
        NULL == notValidBefore || NULL == notValidAfter)
    {
        return OC_STACK_INVALID_PARAM;
    }

    mbedtls_x509write_crt_init(&outCertCtx);
    mbedtls_pk_init(&subjKeyCtx);
    mbedtls_pk_init(&issKeyCtx);
    mbedtls_x509_crt_init(&issCertCtx);
    mbedtls_mpi_init(&serialMpi);
    memset(&names, 0, sizeof(names));
    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_entropy_init(&entropy);
    memset(certificate, 0, sizeof(*certificate));

    ret = mbedtls_mpi_read_string(&serialMpi, 10, serial);
    LOG_MBED_ERROR(TAG, ret, mbedErrorBuf, sizeof(mbedErrorBuf), ERROR);
    VERIFY_OR_LOG_AND_EXIT(TAG, 0 == ret, "Could not parse serial number for internal cert generation", ERROR);

    ret = mbedtls_pk_parse_public_key(&subjKeyCtx, (const uint8_t *)subjectPublicKey, strlen(subjectPublicKey) + 1);
    LOG_MBED_ERROR(TAG, ret, mbedErrorBuf, sizeof(mbedErrorBuf), ERROR);
    VERIFY_OR_LOG_AND_EXIT(TAG, 0 == ret, "Could not parse public key for internal cert generation",  ERROR);

    ret = mbedtls_pk_parse_key(&issKeyCtx, (const uint8_t *)issuerPrivateKey, strlen(issuerPrivateKey) + 1, NULL, 0);
    LOG_MBED_ERROR(TAG, ret, mbedErrorBuf, sizeof(mbedErrorBuf), ERROR);
    VERIFY_OR_LOG_AND_EXIT(TAG, 0 == ret, "Could not parse private key for internal cert generation",  ERROR);

    /* If issuerCert is NULL, then the cert will be self-signed. */
    if (NULL != issuerCert)
    {
        ret = mbedtls_x509_crt_parse(&issCertCtx, (const uint8_t *)issuerCert, strlen(issuerCert) + 1);
        LOG_MBED_ERROR(TAG, ret, mbedErrorBuf, sizeof(mbedErrorBuf), ERROR);
        VERIFY_OR_LOG_AND_EXIT(TAG, 0 == ret, "Could not parse issuer cert for internal cert generation",  ERROR);
    }

    ret = mbedtls_x509write_crt_set_validity(&outCertCtx, notValidBefore, notValidAfter);
    LOG_MBED_ERROR(TAG, ret, mbedErrorBuf, sizeof(mbedErrorBuf), ERROR);
    VERIFY_OR_LOG_AND_EXIT(TAG, 0 == ret, "Could not write validity time for internal cert generation",  ERROR);

    mbedtls_x509write_crt_set_version(&outCertCtx, MBEDTLS_X509_CRT_VERSION_3);
    mbedtls_x509write_crt_set_md_alg(&outCertCtx, MBEDTLS_MD_SHA256);

    res = OC_STACK_ERROR;

    ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func,
        &entropy, (const uint8_t *)PERSONALIZATION_STRING, sizeof(PERSONALIZATION_STRING));
    LOG_MBED_ERROR(TAG, ret, mbedErrorBuf, sizeof(mbedErrorBuf), ERROR);
    VERIFY_OR_LOG_AND_EXIT(TAG, 0 == ret, "Could not generate seed for internal cert generation",  ERROR);
    mbedtls_ctr_drbg_set_prediction_resistance(&ctr_drbg, MBEDTLS_CTR_DRBG_PR_ON);

    ret = mbedtls_x509write_crt_set_serial(&outCertCtx, &serialMpi);
    LOG_MBED_ERROR(TAG, ret, mbedErrorBuf, sizeof(mbedErrorBuf), ERROR);
    VERIFY_OR_LOG_AND_EXIT(TAG, 0 == ret, "Could not write serial number for internal cert generation",  ERROR);

    ret = mbedtls_x509write_crt_set_subject_name(&outCertCtx, subject);
    LOG_MBED_ERROR(TAG, ret, mbedErrorBuf, sizeof(mbedErrorBuf), ERROR);
    VERIFY_OR_LOG_AND_EXIT(TAG, 0 == ret, "Could not write subject name for internal cert generation",  ERROR);

    if (NULL != issuerCert)
    {
        // mbedtls_x509_dn_gets returns the number of bytes written to buf.
        ret = mbedtls_x509_dn_gets(buf, sizeof(buf), &issCertCtx.subject);
        VERIFY_OR_LOG_AND_EXIT(TAG, 0 < ret,  "Could not parse subject name from issuer for internal cert generation", ERROR);
        ret = mbedtls_x509write_crt_set_issuer_name(&outCertCtx, buf);
    }
    else
    {
        /* If self-signed, use the same contents of subject for the issuer name. */
        ret = mbedtls_x509write_crt_set_issuer_name(&outCertCtx, subject);
    }
    LOG_MBED_ERROR(TAG, ret, mbedErrorBuf, sizeof(mbedErrorBuf), ERROR);
    VERIFY_OR_LOG_AND_EXIT(TAG, 0 == ret, "Could not write issuer name for internal cert generation",  ERROR);

    mbedtls_x509write_crt_set_subject_key(&outCertCtx, &subjKeyCtx);

    mbedtls_x509write_crt_set_issuer_key(&outCertCtx, &issKeyCtx);

    /*
     * mbedtls max_pathlen behaviour
     * CA Cert: Expects RFC5280_val as encoding input
     *          Provides RFC5280_val+1 as decoding output, where 0 = not present
     * EE Cert: Does not encode
     *          Provides 0 as decoding output
    */

    if (CERT_TYPE_ROOT_CA == certType)
    {
        res = OCWriteBasicConstraints(&outCertCtx, 1, -1, 1);
        VERIFY_OR_LOG_AND_EXIT(TAG, OC_STACK_OK == res, "Could not write basic constraints for internal root ca cert generation",  ERROR);
        ret = mbedtls_x509write_crt_set_key_usage(&outCertCtx,
            MBEDTLS_X509_KU_KEY_CERT_SIGN | MBEDTLS_X509_KU_CRL_SIGN);
        LOG_MBED_ERROR(TAG, ret, mbedErrorBuf, sizeof(mbedErrorBuf), ERROR);
        VERIFY_OR_LOG_AND_EXIT(TAG, 0 == ret, "Could not write key usage for internal root ca cert generation",  ERROR);
    }
    else if (CERT_TYPE_INTERMEDIATE_CA == certType)
    {
        res = OCWriteBasicConstraints(&outCertCtx, 1, 0, 1);
        VERIFY_OR_LOG_AND_EXIT(TAG, OC_STACK_OK == res, "Could not write basic constraints for internal intermediate ca cert generation",  ERROR);
        ret = mbedtls_x509write_crt_set_key_usage(&outCertCtx,
            MBEDTLS_X509_KU_KEY_CERT_SIGN | MBEDTLS_X509_KU_CRL_SIGN);
        LOG_MBED_ERROR(TAG, ret, mbedErrorBuf, sizeof(mbedErrorBuf), ERROR);
        VERIFY_OR_LOG_AND_EXIT(TAG, 0 == ret, "Could not write key usage for internal intermediate ca cert generation",  ERROR);
    }
    else
    {
        res = OCWriteBasicConstraints(&outCertCtx, 0, -1, 0);
        VERIFY_OR_LOG_AND_EXIT(TAG, OC_STACK_OK == res, "Could not write basic constraints for internal root ee cert generation",  ERROR);
        ret = mbedtls_x509write_crt_set_key_usage(&outCertCtx,
            MBEDTLS_X509_KU_DIGITAL_SIGNATURE | MBEDTLS_X509_KU_KEY_AGREEMENT);
        LOG_MBED_ERROR(TAG, ret, mbedErrorBuf, sizeof(mbedErrorBuf), ERROR);
        VERIFY_OR_LOG_AND_EXIT(TAG, 0 == ret, "Could not write key usage for internal ee cert generation",  ERROR);
    }

    switch (certType)
    {
    case CERT_TYPE_ROLE:
        ret = mbedtls_x509write_crt_set_extension(&outCertCtx,
            MBEDTLS_OID_EXTENDED_KEY_USAGE, MBEDTLS_OID_SIZE(MBEDTLS_OID_EXTENDED_KEY_USAGE),
            0,
            s_ekuRole, sizeof(s_ekuRole));
        LOG_MBED_ERROR(TAG, ret, mbedErrorBuf, sizeof(mbedErrorBuf), ERROR);
        VERIFY_OR_LOG_AND_EXIT(TAG, 0 == ret,  "Could not write eku for internal role cert generation", ERROR);
        ret = snprintf(buf, sizeof(buf), "CN=%s%s%s", role, (NULL != authority) ? ",OU=" : "", (NULL != authority) ? authority : "");
        // To prevent sign-compare warning sizeof(buf) is cast to int. This is safe because the max size of buf fits into int.
        // Note ret value from snprintf may be negative if there was an error so it should not be cast to size_t.
        VERIFY_OR_LOG_AND_EXIT(TAG, ret < (int)sizeof(buf),  "snprintf error during internal cert generation", ERROR);
        names.next = NULL;
        names.general_name.name_type = MBEDTLS_X509_GENERALNAME_DIRECTORYNAME;
        ret = mbedtls_x509_string_to_names(&names.general_name.directory_name, buf);
        LOG_MBED_ERROR(TAG, ret, mbedErrorBuf, sizeof(mbedErrorBuf), ERROR);
        VERIFY_OR_LOG_AND_EXIT(TAG, 0 == ret,  "Could not parse direcotry name for internal role cert generation", ERROR);

        ret = mbedtls_x509write_crt_set_subject_alt_names(&outCertCtx, &names);
        LOG_MBED_ERROR(TAG, ret, mbedErrorBuf, sizeof(mbedErrorBuf), ERROR);
        VERIFY_OR_LOG_AND_EXIT(TAG, 0 == ret,  "Could not write subject alt names for internal role cert generation", ERROR);
        break;

    case CERT_TYPE_IDENTITY:
        ret = mbedtls_x509write_crt_set_extension(&outCertCtx,
            MBEDTLS_OID_EXTENDED_KEY_USAGE, MBEDTLS_OID_SIZE(MBEDTLS_OID_EXTENDED_KEY_USAGE),
            0,
            s_ekuIdentity, sizeof(s_ekuIdentity));
        LOG_MBED_ERROR(TAG, ret, mbedErrorBuf, sizeof(mbedErrorBuf), ERROR);
        VERIFY_OR_LOG_AND_EXIT(TAG, 0 == ret,  "Could not write eku for internal ee cert generation", ERROR);

        // optional extensions

        // FILE *fp = fopen("./cpl-ext.der", "wb");
        // fwrite(s_cplAttributesExtBytes, sizeof(s_cplAttributesExtBytes), 1, fp );
        // fclose(fp);

        ret = mbedtls_x509write_crt_set_extension(
                    &outCertCtx,
                    s_cplAttributesExtOid,
                    MBEDTLS_OID_SIZE(s_cplAttributesExtOid), 0,
                    (const unsigned char*)s_cplAttributesExtBytes, sizeof(s_cplAttributesExtBytes));
        LOG_MBED_ERROR(TAG, ret, mbedErrorBuf, sizeof(mbedErrorBuf), ERROR);
        VERIFY_OR_LOG_AND_EXIT(TAG, 0 == ret, "Problem writing certified product list extension", ERROR);

        // fp = fopen("./claims-ext.der", "wb");
        // fwrite(s_cplSecurityClaimsExtBytes, sizeof(s_cplSecurityClaimsExtBytes), 1, fp );
        // fclose(fp);

        ret = mbedtls_x509write_crt_set_extension(
                    &outCertCtx,
                    s_cplSecurityClaimsExtOid,
                    MBEDTLS_OID_SIZE(s_cplSecurityClaimsExtOid), 0,
                    (const unsigned char*)s_cplSecurityClaimsExtBytes, sizeof(s_cplSecurityClaimsExtBytes));
        LOG_MBED_ERROR(TAG, ret, mbedErrorBuf, sizeof(mbedErrorBuf), ERROR);
        VERIFY_OR_LOG_AND_EXIT(TAG, 0 == ret, "Problem writing certified product list extension", ERROR);

        // fp = fopen("./compliance-ext.der", "wb");
        // fwrite(s_ComplianceExtBytes, sizeof(s_ComplianceExtBytes), 1, fp );
        // fclose(fp);

        ret = mbedtls_x509write_crt_set_extension(
                    &outCertCtx,
                    s_ComplianceExtOid,
                    MBEDTLS_OID_SIZE(s_ComplianceExtOid), 0,
                    (const unsigned char*)s_ComplianceExtBytes, sizeof(s_ComplianceExtBytes));
        LOG_MBED_ERROR(TAG, ret, mbedErrorBuf, sizeof(mbedErrorBuf), ERROR);
        VERIFY_OR_LOG_AND_EXIT(TAG, 0 == ret, "Problem writing certified product list extension", ERROR);
        break;


    case CERT_TYPE_ROOT_CA:
    case CERT_TYPE_INTERMEDIATE_CA:
        ret = mbedtls_x509write_crt_set_extension(&outCertCtx,
            MBEDTLS_OID_EXTENDED_KEY_USAGE, MBEDTLS_OID_SIZE(MBEDTLS_OID_EXTENDED_KEY_USAGE),
            0,
            s_ekuCA, sizeof(s_ekuCA));
        LOG_MBED_ERROR(TAG, ret, mbedErrorBuf, sizeof(mbedErrorBuf), ERROR);
        VERIFY_OR_LOG_AND_EXIT(TAG, 0 == ret, "Could not write eku for internal root/int ca cert generation",  ERROR);
        break;

    default:
        assert(false);
        VERIFY_OR_LOG_AND_EXIT(TAG, false, "Unkown cert type during internal cert generation", ERROR);
    }

    ret = mbedtls_x509write_crt_pem(&outCertCtx, (uint8_t *)buf, sizeof(buf), mbedtls_ctr_drbg_random, &ctr_drbg);
    LOG_MBED_ERROR(TAG, ret, mbedErrorBuf, sizeof(mbedErrorBuf), ERROR);
    VERIFY_OR_LOG_AND_EXIT(TAG, 0 == ret, "Could not generate pem buffer for internal cert generation",  ERROR);

    certificate->len = strlen(buf) + 1;
    certificate->bytes = (uint8_t *)OICCalloc(1, certificate->len);
    VERIFY_NOT_NULL(TAG, certificate->bytes, ERROR);
    memcpy(certificate->bytes, buf, certificate->len);

    res = OC_STACK_OK;

exit:

    if (OC_STACK_OK != res)
    {
        OICFree(certificate->bytes);
        certificate->bytes = NULL;
        certificate->len = 0;
    }

    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
    mbedtls_asn1_free_named_data_list(&names.general_name.directory_name);
    mbedtls_mpi_free(&serialMpi);
    mbedtls_x509_crt_free(&issCertCtx);
    mbedtls_pk_free(&issKeyCtx);
    mbedtls_pk_free(&subjKeyCtx);
    mbedtls_x509write_crt_free(&outCertCtx);

    return res;
}

OCStackResult OC_CALL OCGenerateRootCACertificate(
    const char *subject,
    const char *subjectPublicKey,
    const char *issuerCert,
    const char *issuerPrivateKey,
    const char *serial,
    const char *notValidBefore,
    const char *notValidAfter,
    char **certificate,
    size_t *certificateLen)
{
    OCStackResult res = OC_STACK_OK;
    OCByteString byteStr = { 0 };

    res = GenerateCertificate(
        CERT_TYPE_ROOT_CA,
        subject,
        subjectPublicKey,
        issuerCert,
        issuerPrivateKey,
        serial,
        notValidBefore,
        notValidAfter,
        NULL,
        NULL,
        &byteStr);

    if (OC_STACK_OK == res)
    {
        *certificate = (char *)byteStr.bytes;
        *certificateLen = byteStr.len;
    }

    return res;
}

OCStackResult OC_CALL OCGenerateIntermediateCACertificate(
    const char *subject,
    const char *subjectPublicKey,
    const char *issuerCert,
    const char *issuerPrivateKey,
    const char *serial,
    const char *notValidBefore,
    const char *notValidAfter,
    char **certificate,
    size_t *certificateLen)
{
    OCStackResult res = OC_STACK_OK;
    OCByteString byteStr = { 0 };

    res = GenerateCertificate(
        CERT_TYPE_INTERMEDIATE_CA,
        subject,
        subjectPublicKey,
        issuerCert,
        issuerPrivateKey,
        serial,
        notValidBefore,
        notValidAfter,
        NULL,
        NULL,
        &byteStr);

    if (OC_STACK_OK == res)
    {
        *certificate = (char *)byteStr.bytes;
        *certificateLen = byteStr.len;
    }

    return res;
}

OCStackResult OC_CALL OCGenerateIdentityCertificate(
    const OicUuid_t *subjectUuid,
    const char *subjectPublicKey,
    const char *issuerCert,
    const char *issuerPrivateKey,
    const char *serial,
    const char *notValidBefore,
    const char *notValidAfter,
    char **certificate,
    size_t *certificateLen)
{
    OCStackResult res = OC_STACK_OK;
    OCByteString byteStr = { 0 };
    char uuidStr[UUID_STRING_SIZE] = { 0 } ;
    char subject[sizeof(uuidStr) + sizeof(SUBJECT_PREFIX)] = { 0 } ;

    if (NULL == issuerCert)
    {
        return OC_STACK_INVALID_PARAM;
    }

    if (!OCConvertUuidToString(subjectUuid->id, uuidStr))
    {
        OIC_LOG(ERROR, TAG, "Could not convert UUID");
        return OC_STACK_INVALID_PARAM;
    }

    if (snprintf(subject, sizeof(subject), "%s%s", SUBJECT_PREFIX, uuidStr) == sizeof(subject))
    {
        OIC_LOG(ERROR, TAG, "Could not write subject string");
        return OC_STACK_INVALID_PARAM;
    }

    res = GenerateCertificate(
        CERT_TYPE_IDENTITY,
        subject,
        subjectPublicKey,
        issuerCert,
        issuerPrivateKey,
        serial,
        notValidBefore,
        notValidAfter,
        NULL,
        NULL,
        &byteStr);

    if (OC_STACK_OK == res)
    {
        *certificate = (char *)byteStr.bytes;
        *certificateLen = byteStr.len;
    }

    return res;
}

OCStackResult OC_CALL OCGenerateRoleCertificate(
    const OicUuid_t *subjectUuid,
    const char *subjectPublicKey,
    const char *issuerCert,
    const char *issuerPrivateKey,
    const char *serial,
    const char *notValidBefore,
    const char *notValidAfter,
    const char *role,
    const char *authority,
    char **certificate,
    size_t *certificateLen)
{
    OCStackResult res = OC_STACK_ERROR;
    OCByteString byteStr;
    char uuidStr[UUID_STRING_SIZE] = { 0 };
    char subject[sizeof(uuidStr) + sizeof(SUBJECT_PREFIX)] = { 0 };

    memset(&byteStr, 0, sizeof(byteStr));

    if (NULL == role || NULL == issuerCert)
    {
        return OC_STACK_INVALID_PARAM;
    }

    if (!OCConvertUuidToString(subjectUuid->id, uuidStr))
    {
        OIC_LOG(ERROR, TAG, "Could not convert UUID");
        return OC_STACK_INVALID_PARAM;
    }

    if (snprintf(subject, sizeof(subject), "%s%s", SUBJECT_PREFIX, uuidStr) == sizeof(subject))
    {
        OIC_LOG(ERROR, TAG, "Could not write subject string");
        return OC_STACK_INVALID_PARAM;
    }

    res = GenerateCertificate(
        CERT_TYPE_ROLE,
        subject,
        subjectPublicKey,
        issuerCert,
        issuerPrivateKey,
        serial,
        notValidBefore,
        notValidAfter,
        role,
        authority,
        &byteStr);

    if (OC_STACK_OK == res)
    {
        *certificate = (char *)byteStr.bytes;
        *certificateLen = byteStr.len;
    }

    return res;
}


/* Verify the signature in a CSR is valid. */
static int VerifyCSRSignature(mbedtls_x509_csr* csr)
{
    unsigned char hash[MBEDTLS_MD_MAX_SIZE];

    if (csr->sig_md != MBEDTLS_MD_SHA256)
    {
        OIC_LOG(ERROR, TAG, "Unexpected digest used in CSR\n");
        return -1;
    }

    if ((csr->cri.len == 0) || (csr->cri.p == NULL))
    {
        OIC_LOG(ERROR, TAG, "Missing CertificateRequestInfo field in CSR\n");
        return -1;
    }

    if ((csr->sig.len == 0) || (csr->sig.p == NULL))
    {
        OIC_LOG(ERROR, TAG, "Missing signature field in CSR\n");
        return -1;
    }

    if (MBEDTLS_OID_CMP(MBEDTLS_OID_ECDSA_SHA256, &csr->sig_oid) != 0)
    {
        char buf[256];
        if (mbedtls_oid_get_numeric_string(buf, sizeof(buf), &csr->sig_oid) > 0)
        {
            OIC_LOG_V(ERROR, TAG, "Unexpected signature OID in CSR (got %s)\n", buf);
        }
        else
        {
            OIC_LOG(ERROR, TAG, "Unexpected signature OID in CSR\n");
        }
        return -1;
    }

    if (mbedtls_pk_get_type(&csr->pk) != MBEDTLS_PK_ECKEY)
    {
        OIC_LOG(ERROR, TAG, "Unexpected public key type in CSR\n");
        return -1;
    }

    /* mbedtls_pk_get_bitlen returns the bit length of the curve */
    if (mbedtls_pk_get_bitlen(&csr->pk) != 256)
    {
        OIC_LOG(ERROR, TAG, "Unexpected public length in CSR\n");
        return -1;
    }

    mbedtls_ecp_keypair* ecKey = mbedtls_pk_ec(csr->pk);
    if ((ecKey != NULL) && (ecKey->grp.id != MBEDTLS_ECP_DP_SECP256R1))
    {
        OIC_LOG(ERROR, TAG, "Unexpected curve parameters in CSR\n");
        return -1;
    }

    /* Hash the CertificateRequestInfoField (https://tools.ietf.org/html/rfc2986#section-3) */
    int ret = mbedtls_md(mbedtls_md_info_from_type(csr->sig_md), csr->cri.p, csr->cri.len, hash);
    if (ret != 0)
    {
        OIC_LOG(ERROR, TAG, "Failed to hash CertificateRequestInfoField\n");
        return ret;
    }

    /* the length of hash is determined from csr->sig_md*/
    ret = mbedtls_pk_verify(&csr->pk, csr->sig_md, hash, 0, csr->sig.p, csr->sig.len);

    return ret;
}

OCStackResult OC_CALL OCVerifyCSRSignature(const char* csr)
{
    mbedtls_x509_csr csrObj;

    mbedtls_x509_csr_init(&csrObj);
    int ret = mbedtls_x509_csr_parse(&csrObj, (const unsigned char*)csr, strlen(csr) + 1);
    if (ret < 0)
    {
        OIC_LOG_V(ERROR, TAG, "Couldn't parse CSR: %d", ret);
        mbedtls_x509_csr_free(&csrObj);
        return OC_STACK_ERROR;
    }

    ret = VerifyCSRSignature(&csrObj);

    mbedtls_x509_csr_free(&csrObj);

    if (ret != 0)
    {
        return OC_STACK_ERROR;
    }

    return OC_STACK_OK;
}

OCStackResult OC_CALL OCGetUuidFromCSR(const char* csr, OicUuid_t* uuid)
{
    mbedtls_x509_csr csrObj;

    mbedtls_x509_csr_init(&csrObj);
    int ret = mbedtls_x509_csr_parse(&csrObj, (const unsigned char*)csr, strlen(csr) + 1);
    if (ret < 0)
    {
        OIC_LOG_V(ERROR, TAG, "Couldn't parse CSR: %d", ret);
        mbedtls_x509_csr_free(&csrObj);
        return OC_STACK_ERROR;
    }

    char uuidStr[UUID_STRING_SIZE + sizeof(SUBJECT_PREFIX) - 1] = { 0 };   // Both constants count NULL, subtract one
    ret = mbedtls_x509_dn_gets(uuidStr, sizeof(uuidStr), &csrObj.subject);
    if (ret != (sizeof(uuidStr) - 1))
    {
        OIC_LOG_V(ERROR, TAG, "mbedtls_x509_dn_gets returned length or error: %d, expected %" PRIuPTR, ret, sizeof(uuidStr) - 1);
        mbedtls_x509_csr_free(&csrObj);
        return OC_STACK_ERROR;
    }

    if (!OCConvertStringToUuid(uuidStr + sizeof(SUBJECT_PREFIX) - 1, uuid->id))
    {
        OIC_LOG_V(ERROR, TAG, "Failed to convert UUID: '%s'", uuidStr);
        mbedtls_x509_csr_free(&csrObj);
        return OC_STACK_ERROR;
    }

    if (memcmp(uuid->id, &WILDCARD_SUBJECT_ID, sizeof(uuid->id)) == 0)
    {
        OIC_LOG(ERROR, TAG, "Invalid UUID in CSR: '*'");
        mbedtls_x509_csr_free(&csrObj);
        return OC_STACK_ERROR;
    }

    mbedtls_x509_csr_free(&csrObj);
    return OC_STACK_OK;
}

OCStackResult OC_CALL OCGetPublicKeyFromCSR(const char* csr, char** publicKey)
{
    mbedtls_x509_csr csrObj;

    mbedtls_x509_csr_init(&csrObj);
    int ret = mbedtls_x509_csr_parse(&csrObj, (const unsigned char*)csr, strlen(csr) + 1);
    if (ret < 0)
    {
        OIC_LOG_V(ERROR, TAG, "Couldn't parse CSR: %d", ret);
        mbedtls_x509_csr_free(&csrObj);
        return OC_STACK_ERROR;
    }

    char subjectPublicKey[500] = { 0 };
    ret = mbedtls_pk_write_pubkey_pem(&csrObj.pk, (unsigned char*)subjectPublicKey, sizeof(subjectPublicKey));
    if (ret != 0)
    {
        OIC_LOG_V(ERROR, TAG, "Failed to write subject public key as PEM: %d", ret);
        mbedtls_x509_csr_free(&csrObj);
        return OC_STACK_ERROR;
    }

    size_t pkLen = strlen(subjectPublicKey) + 1;
    *publicKey = (char*) OICCalloc(1, pkLen);
    if (*publicKey == NULL)
    {
        OIC_LOG(ERROR, TAG, "Failed to allocate memory for public key");
        mbedtls_x509_csr_free(&csrObj);
        return OC_STACK_ERROR;
    }

    memcpy(*publicKey, subjectPublicKey, pkLen);
    mbedtls_x509_csr_free(&csrObj);

    return OC_STACK_OK;
}

OCStackResult OC_CALL OCConvertDerCSRToPem(const char* derCSR, size_t derCSRLen, char** pemCSR)
{
    const char* pemHeader = "-----BEGIN CERTIFICATE REQUEST-----\n";
    const char* pemFooter = "-----END CERTIFICATE REQUEST-----\n";

    /* Get the length required for output*/
    size_t pemCSRLen;
    int ret = mbedtls_pem_write_buffer(pemHeader,
        pemFooter,
        (const unsigned char*)derCSR,
        derCSRLen,
        NULL,
        0,
        &pemCSRLen);
    if (ret != MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL)
    {
        OIC_LOG_V(ERROR, TAG, "Couldn't convert CSR into PEM, failed getting required length: %d", ret);
        return OC_STACK_ERROR;
    }

    *pemCSR = OICCalloc(1, pemCSRLen + 1);
    if (*pemCSR == NULL)
    {
        OIC_LOG(ERROR, TAG, "Failed to allocate memory for PEM CSR");
        return OC_STACK_ERROR;
    }

    /* Try the conversion */
    ret = mbedtls_pem_write_buffer(pemHeader, pemFooter,
        (const unsigned char *)derCSR,
        derCSRLen,
        (unsigned char*) *pemCSR,
        pemCSRLen,
        &pemCSRLen);
    if (ret < 0)
    {
        OIC_LOG_V(ERROR, TAG, "Couldn't convert CSR into PEM, failed getting required length: %d", ret);
        OICFree(*pemCSR);
        *pemCSR = NULL;
        return OC_STACK_ERROR;
    }

    return OC_STACK_OK;
}

#endif /* defined(__WITH_TLS__) || defined(__WITH_DTLS__) */
