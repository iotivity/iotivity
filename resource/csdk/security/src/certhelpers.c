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

#include "iotivity_config.h"

#include "logger.h"
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include "oic_malloc.h"
#include "oic_string.h"
#include "cacommon.h"
#include "ocrandom.h"
#include "cacommonutil.h"

#include "ocpayload.h"
#include "payload_logging.h"
#include "pmutility.h"
#include "srmutility.h"

// headers required for mbed TLS
#include "mbedtls/config.h"
#include "mbedtls/platform.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/x509_csr.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/oid.h"

#ifndef NDEBUG
#include "mbedtls/debug.h"
#include "mbedtls/version.h"
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <fcntl.h>

#include "certhelpers.h"

#define TAG "OIC_CERTHELPERS"

/**
 * @def PERSONALIZATION_STRING
 * @brief Personalization string for the mbedtls RNG
 */
#define PERSONALIZATION_STRING "IOTIVITY_RND"

#define MAX_URI_QUERY MAX_URI_LENGTH + MAX_QUERY_LENGTH

#define MAX_STRING_LEN 254

/**
 * Generates elliptic curve keypair.
 *
 * @param[out]  pk    mbedtls public key container
 *
 * @return  0 on success or <0 on error
 */
static int GenerateEccKeyPair(mbedtls_pk_context *pk)
{
    int ret = 0;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;

    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);
    VERIFY_NON_NULL_RET(pk, TAG, "Param pk is NULL", -1);

    // Initialize the DRBG context
    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_entropy_init(&entropy);
    ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func,
                                &entropy, (const unsigned char *)PERSONALIZATION_STRING, sizeof(PERSONALIZATION_STRING));

    if (0 > ret)
    {
        OIC_LOG_V(ERROR, TAG, "Seed initialization failed! %d", ret);
        OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
        goto exit;
    }
    mbedtls_ctr_drbg_set_prediction_resistance(&ctr_drbg, MBEDTLS_CTR_DRBG_PR_ON);
    ret = mbedtls_pk_setup(pk, mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY));
    if (0 > ret)
    {
        OIC_LOG_V(ERROR, TAG, "mbedtls_pk_setup error %d", ret);
        OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
        goto exit;
    }
    ret = mbedtls_ecp_gen_key(MBEDTLS_ECP_DP_SECP256R1, mbedtls_pk_ec(*pk), mbedtls_ctr_drbg_random, &ctr_drbg);
    if (0 > ret)
    {
        OIC_LOG(ERROR, TAG, "mbedtls_ecp_gen_keypair error");
        OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
        goto exit;
    }

exit:

    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);

    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
    return 0;
}

/**
 * Generates certificate signing request for given key pair.
 *
 * @param[in]  subject        CSR Subject names should contain a comma-separated list of OID types and values:
 *                            e.g. "C=UA,O=ABC,CN=uuid:32323232-3232-3232-3232-323232323232"
 * @param[in]  key            Public/private key pair to generate for.
 * @param[in]  encoding       Encoding to use; valid values are OIC_ENCODING_PEM and OIC_ENCODING_DER
 * @param[out] csr            certificate signing request in the requested encoding
 *
 * @return  0 on success or <0 on error
 */
static int GenerateCSRForKey(const char *subject, mbedtls_pk_context* key, OicEncodingType_t encoding, OCByteString *csr)
{
    int ret = 0;
    unsigned char buf[2048];
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_x509write_csr req;

    OIC_LOG_V(DEBUG, TAG, "In %s", __func__);
    VERIFY_NON_NULL_RET(subject, TAG, "Param subject is NULL", -1);
    VERIFY_NON_NULL_RET(csr, TAG, "Param csr is NULL", -1);

    if ((OIC_ENCODING_PEM != encoding) && (OIC_ENCODING_DER != encoding))
    {
        OIC_LOG(ERROR, TAG, "Unsupported encoding requested");
        return -1;
    }

    // Initialize all data structures up front so the error path can always free them safely, no
    // matter where the error exit point.
    mbedtls_x509write_csr_init(&req);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_entropy_init(&entropy);
    csr->bytes = NULL;

    // Initialize CSR context
    // Set up MD algorithm, key and subject to CSR
    mbedtls_x509write_csr_set_md_alg(&req, MBEDTLS_MD_SHA256);
    mbedtls_x509write_csr_set_key(&req, key);
    ret = mbedtls_x509write_csr_set_subject_name(&req, subject);
    if (0 > ret)
    {
        OIC_LOG_V(ERROR, TAG, "mbedtls_x509write_csr_set_subject_name error %d", ret);
        goto exit;
    }

    // Initialize the DRBG context
    ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func,
                                &entropy, (const unsigned char *)PERSONALIZATION_STRING, sizeof(PERSONALIZATION_STRING));
    if (0 > ret)
    {
        OIC_LOG_V(ERROR, TAG, "Seed initialization failed! %d", ret);
        goto exit;
    }
    mbedtls_ctr_drbg_set_prediction_resistance(&ctr_drbg, MBEDTLS_CTR_DRBG_PR_ON);

    // Create CSR
    if (OIC_ENCODING_PEM == encoding)
    {
        ret = mbedtls_x509write_csr_pem(&req, buf, sizeof(buf),
                                        mbedtls_ctr_drbg_random, &ctr_drbg);

        // In the PEM case, success is always denoted by a zero return. We have to compute the length.
        if (0 != ret)
        {
            OIC_LOG_V(ERROR, TAG, "mbedtls_x509write_csr_pem error %d", ret);
            goto exit;
        }
        else
        {
            csr->len = strlen((const char *)buf) + 1;
            csr->bytes = (uint8_t *)OICMalloc(csr->len);
            if (NULL == csr->bytes)
            {
                OIC_LOG(ERROR, TAG, "OICMalloc returned NULL on csr allocation");
                ret = MBEDTLS_ERR_X509_ALLOC_FAILED;
                goto exit;
            }
            memcpy(csr->bytes, buf, csr->len);
        }
    }
    else // encoding == OIC_ENCODING_DER
    {
        ret = mbedtls_x509write_csr_der(&req, buf, sizeof(buf),
                                        mbedtls_ctr_drbg_random, &ctr_drbg);

        // In the DER case, ret is the number of bytes written.
        if (0 > ret)
        {
            OIC_LOG_V(ERROR, TAG, "mbedtls_x509write_csr_der error %d", ret);
            goto exit;
        }
        else
        {
            csr->len = ret;
            csr->bytes = (uint8_t *)OICMalloc(csr->len);
            if (NULL == csr->bytes)
            {
                OIC_LOG(ERROR, TAG, "OICMalloc returned NULL on csr allocation");
                ret = MBEDTLS_ERR_X509_ALLOC_FAILED;
                goto exit;
            }
            // DER data gets written to the end of the buffer.
            memcpy(csr->bytes, buf + sizeof(buf) - csr->len, csr->len);
            ret = 0;
        }
    }



exit:

    if (0 > ret)
    {
        OICFree(csr->bytes);
        csr->bytes = NULL;
        csr->len = 0;
    }

    mbedtls_entropy_free(&entropy);
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_x509write_csr_free(&req);

    OICClearMemory(buf, sizeof(buf));

    OIC_LOG_V(DEBUG, TAG, "Out %s", __func__);
    return ret;
}

int OCInternalCSRRequest(const char *subject, mbedtls_pk_context *keyPair, OicEncodingType_t encoding, OCByteString *csr)
{
    return GenerateCSRForKey(subject, keyPair, encoding, csr);
}

int OCInternalGenerateKeyPair(mbedtls_pk_context *keyPair)
{
    return GenerateEccKeyPair(keyPair);
}
