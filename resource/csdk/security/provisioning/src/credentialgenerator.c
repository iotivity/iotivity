/* *****************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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
#include <string.h>
#include "credentialgenerator.h"
#include "base64.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "ocpayload.h"
#include "payload_logging.h"
#include "credresource.h"
#include "ocrandom.h"
#include "srmutility.h"
#include "stdbool.h"
#include "securevirtualresourcetypes.h"
#ifdef __WITH_X509__
#include "ck_manager.h"

#define CHAIN_LEN (2) //TODO: replace by external define or a runtime value
#endif  //__WITH_X509__

#define TAG "SRPAPI-CG"

static const char OIC_JSON_CRV_NAME[] = "crv";
static const char OIC_JSON_KTY_NAME[] = "kty";
static const char OIC_JSON_CERTIFICATE_NAME[] = "x5c";
static const char OIC_JSON_D_NAME[] = "d";
static const char kty[] = "EC";
static const char crv[] = "P-256";
static const uint8_t CertMapSize = 3;

OCStackResult PMGeneratePairWiseCredentials(OicSecCredType_t type, size_t keySize,
        const OicUuid_t *ptDeviceId, const OicUuid_t *firstDeviceId,
        const OicUuid_t *secondDeviceId, OicSecCred_t **firstCred, OicSecCred_t **secondCred)
{
    if (NULL == ptDeviceId || NULL == firstDeviceId || NULL != *firstCred || \
        NULL == secondDeviceId || NULL != *secondCred)
    {
        OIC_LOG(INFO, TAG, "Invalid params");
        return OC_STACK_INVALID_PARAM;
    }
    if(!(keySize == OWNER_PSK_LENGTH_128 || keySize == OWNER_PSK_LENGTH_256))
    {
        OIC_LOG(INFO, TAG, "Invalid key size");
        return OC_STACK_INVALID_PARAM;
    }
    OCStackResult res = OC_STACK_ERROR;
    OicSecCred_t *tempFirstCred = NULL;
    OicSecCred_t *tempSecondCred = NULL;

    size_t privDataKeySize = keySize;

    uint8_t *privData = (uint8_t *)OICCalloc(privDataKeySize, sizeof(uint8_t));
    VERIFY_NON_NULL(TAG, privData, ERROR);

    OCFillRandomMem(privData, privDataKeySize);

    // TODO: currently owner array is 1. only provisioning tool's id.
    tempFirstCred =  GenerateCredential(secondDeviceId, type, NULL, privData, 1, ptDeviceId);
    VERIFY_NON_NULL(TAG, tempFirstCred, ERROR);

    // TODO: currently owner array is 1. only provisioning tool's id.
    tempSecondCred =  GenerateCredential(firstDeviceId, type, NULL, privData, 1, ptDeviceId);
    VERIFY_NON_NULL(TAG, tempSecondCred, ERROR);

    *firstCred = tempFirstCred;
    *secondCred = tempSecondCred;
    res = OC_STACK_OK;

exit:
    OICFree(privData);

    if(res != OC_STACK_OK)
    {
        OICFree(tempFirstCred);
        OICFree(tempSecondCred);
        *firstCred = NULL;
        *secondCred = NULL;
    }

    return res;
}

#ifdef __WITH_X509__
/**
 * Function to compose JSON Web Key (JWK) string from a certificate and a public key.
 *
 * @param certificateChain    Array of Base64 encoded certificate strings.
 * @param chainLength         Number of the certificates in certificateChain.
 * @param payload Valid JWK CBOR on success, or NULL on fail.

 */
static OCStackResult CreateCertificatePublicJWK(const char *const *certificateChain,
       const size_t chainLength, uint8_t **cborPayload, size_t *size)
{
    OCStackResult ret = OC_STACK_ERROR;
    *cborPayload = NULL;
    *size = 0;

    if (NULL == certificateChain || chainLength == 0)
    {
        OIC_LOG(ERROR, TAG, "Error CreateCertificatePublicJWK: Invalid params");
        return OC_STACK_INVALID_PARAM;
    }

    size_t certChainSize = 0;
    for (size_t i = 0; i < chainLength; ++i)
    {
        if (NULL != certificateChain[i])
        {
            certChainSize += strlen(certificateChain[i]);
        }
        else
        {
            OIC_LOG(ERROR, TAG, "Error CreateCertificatePublicJWK: Invalid params");
            return OC_STACK_INVALID_PARAM;
        }
    }

    // cborArbitraryLen is a value to conver field names and cbor map, cbor array.
    size_t cborArbitraryLen = 255;
    size_t cborLen = certChainSize + cborArbitraryLen;
    int64_t cborEncoderResult = CborNoError;
    CborEncoder encoder = { .end = 0 };
    uint8_t *outPayload = (uint8_t *)OICCalloc(1, cborLen);
    VERIFY_NON_NULL(TAG, outPayload, ERROR);
    cbor_encoder_init(&encoder, outPayload, cborLen, 0);

    CborEncoder credMap = { .end = 0 };
    cborEncoderResult |= cbor_encoder_create_map(&encoder, &credMap, CertMapSize);
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding Cred Map.");

    cborEncoderResult |= cbor_encode_text_string(&credMap, OIC_JSON_KTY_NAME,
            strlen(OIC_JSON_KTY_NAME));
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding ");
    cborEncoderResult |= cbor_encode_text_string(&credMap, kty, strlen(kty));
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding ");

    cborEncoderResult |= cbor_encode_text_string(&credMap, OIC_JSON_CRV_NAME,
            strlen(OIC_JSON_CRV_NAME));
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding ");
    cborEncoderResult |= cbor_encode_text_string(&credMap, crv, strlen(crv));
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding ");

    cborEncoderResult |= cbor_encode_text_string(&credMap, OIC_JSON_CERTIFICATE_NAME,
            strlen(OIC_JSON_CERTIFICATE_NAME));
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding Certificate Name.");
    CborEncoder certs = { {.ptr = NULL }, .end = 0 };
    cborEncoderResult |= cbor_encoder_create_array(&credMap, &certs, chainLength);
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding Certificate Array.");
    for (size_t i = 0; i < chainLength; i++)
    {
        cborEncoderResult |= cbor_encode_byte_string(&certs, (uint8_t *)certificateChain[i], strlen(certificateChain[i]));
        VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding Certificate Chain.");
    }
    cborEncoderResult |= cbor_encoder_close_container(&credMap, &certs);
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Closing Certificate Chain.");

    cborEncoderResult |= cbor_encoder_close_container(&encoder, &credMap);
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Closing Cred Map.");

    *cborPayload = outPayload;
    *size = encoder.ptr - outPayload;
    ret = OC_STACK_OK;

exit:
    return ret;
}

/**
 * Function to compose JWK string from a private key.
 *
 * @param privateKey to be converted to CBOR.
 * @ Valid JWK string on success, or NULL on fail.
 */
OCStackResult CreateCertificatePrivateJWK(const char *privateKey, uint8_t **cborPayload,
        size_t *size)
{
    *cborPayload = NULL;
    *size = 0;
    OCStackResult ret = OC_STACK_INVALID_PARAM;
    VERIFY_NON_NULL(TAG, privateKey, ERROR);

    // cborArbitraryLen is a value to conver field names and cbor map, cbor array.
    size_t cborArbitraryLen = 255;
    size_t cborLen = strlen(privateKey) + cborArbitraryLen;
    int64_t cborEncoderResult = CborNoError;
    ret = OC_STACK_ERROR;
    uint8_t *outPayload = (uint8_t *)OICCalloc(1, cborLen);
    VERIFY_NON_NULL(TAG, outPayload, ERROR);

    CborEncoder encoder = { .end = 0 };
    cbor_encoder_init(&encoder, outPayload, cborLen, 0);

    CborEncoder credMap = { .end = 0 };
    cborEncoderResult |= cbor_encoder_create_map(&encoder, &credMap, 3);
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding ");

    cborEncoderResult |= cbor_encode_text_string(&credMap, OIC_JSON_KTY_NAME,
            strlen(OIC_JSON_KTY_NAME));
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding ");
    cborEncoderResult |= cbor_encode_text_string(&credMap, kty, strlen(kty));
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding ");

    cborEncoderResult |= cbor_encode_text_string(&credMap, OIC_JSON_CRV_NAME,
            strlen(OIC_JSON_CRV_NAME));
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding ");
    cborEncoderResult |= cbor_encode_text_string(&credMap, crv, strlen(crv));
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding ");

    cborEncoderResult |= cbor_encode_text_string(&credMap, OIC_JSON_D_NAME,
            strlen(OIC_JSON_D_NAME));
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding D tag.");
    cborEncoderResult |= cbor_encode_byte_string(&credMap, (uint8_t *)privateKey, strlen(privateKey));
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Adding D Value.");

    cborEncoderResult |= cbor_encoder_close_container(&encoder, &credMap);
    VERIFY_CBOR_SUCCESS(TAG, cborEncoderResult, "Failed Closing Cred Map.");

    *cborPayload = outPayload;
    *size = encoder.ptr - outPayload;
    ret = OC_STACK_OK;

exit:
    return ret;
}

/**
 * Function to generate Base64 encoded credential data for device.
 *
 * @param[in]   subject             Device id.
 * @param[out]  certificateChain    Pointer to Array of Base64 encoded certificate strings.
 * @param[out]  chainLength         Pointer to number of the certificates in certificateChain.
 * @param[out]  privKey             Pointer to Base64 encoded private key.
 * @return  OC_STACK_OK on success
 */
static OCStackResult GenerateCertificateAndKeys(const OicUuid_t * subject, char *** const certificateChain,
        size_t * const chainLength, char ** const privKey)
{
    if (NULL == subject || NULL == certificateChain || NULL == chainLength || NULL == privKey)
    {
        return  OC_STACK_INVALID_PARAM;
    }
    *certificateChain = NULL;
    *privKey     = NULL;

    ByteArray pubKeyBA  = BYTE_ARRAY_INITIALIZER;
    ByteArray privKeyBA = BYTE_ARRAY_INITIALIZER;
    ByteArray cert[CHAIN_LEN];

    uint8_t pubKeyData[PUBLIC_KEY_SIZE] = {0};
    uint8_t privKeyData[PRIVATE_KEY_SIZE] = {0};
    uint8_t certData[ISSUER_MAX_CERT_SIZE * CHAIN_LEN] = {0};
    uint8_t subjName[UUID_LENGTH + 1] = {0};

    pubKeyBA.data  = pubKeyData;
    pubKeyBA.len   = PUBLIC_KEY_SIZE;
    privKeyBA.data = privKeyData;
    privKeyBA.len  = PRIVATE_KEY_SIZE;
    for (size_t i = 0; i < CHAIN_LEN; ++i)
    {
        cert[i].data      = certData + ISSUER_MAX_CERT_SIZE * i;
        cert[i].len       = ISSUER_MAX_CERT_SIZE;
    }

    memcpy(subjName, subject->id, UUID_LENGTH);
    subjName[UUID_LENGTH] = '\0';

    if (PKI_SUCCESS != GenerateKeyPair(&privKeyBA, &pubKeyBA))
    {
        OIC_LOG(ERROR, TAG, "Error generating keys.");
        return OC_STACK_ERROR;
    }
    if (PKI_SUCCESS != CKMIssueDeviceCertificate(subjName, NULL, NULL, pubKeyBA.data, cert))
    {
        OIC_LOG(ERROR, TAG, "Error generating certificate.");
        return OC_STACK_ERROR;
    }

    char privB64buf[B64ENCODE_OUT_SAFESIZE(PRIVATE_KEY_SIZE) + 1] = {0};
    uint32_t privB64len = 0;
    if (B64_OK != b64Encode(privKeyBA.data,  privKeyBA.len, privB64buf,
                             B64ENCODE_OUT_SAFESIZE(PRIVATE_KEY_SIZE) + 1, &privB64len))
    {
        OIC_LOG(ERROR, TAG, "Error while encoding key");
        return OC_STACK_ERROR;
    }

    if (PKI_SUCCESS != GetCAChain(chainLength , cert + 1))
    {
        OIC_LOG(ERROR, TAG, "Error getting CA certificate chain.");
        return OC_STACK_ERROR;
    }

    ++(*chainLength);
    *certificateChain = (char **)OICMalloc(sizeof(char *) * (*chainLength));

    OCStackResult ret = OC_STACK_NO_MEMORY;
    if (NULL == *certificateChain)
    {
        goto memclean;
    }


    for (size_t i = 0; i < *chainLength; ++i)
    {
        (*certificateChain)[i] = NULL;

        char certB64buf[B64ENCODE_OUT_SAFESIZE(ISSUER_MAX_CERT_SIZE) + 1] = {0};
        uint32_t certB64len = 0;
        if (B64_OK != b64Encode(cert[i].data, cert[i].len, certB64buf,
                                B64ENCODE_OUT_SAFESIZE(ISSUER_MAX_CERT_SIZE) + 1, &certB64len))
        {
            OIC_LOG(ERROR, TAG, "Error while encoding certificate");
            ret = OC_STACK_ERROR;
            goto memclean;
        }

        (*certificateChain)[i] = (char *) OICMalloc(certB64len + 1);
        if (NULL == (*certificateChain)[i])
        {
            goto memclean;
        }

        memcpy((*certificateChain)[i], certB64buf, certB64len + 1);
    }


    *privKey     = (char *)OICMalloc(privB64len + 1);

    if (NULL == *privKey)
    {
memclean:
        if (NULL != *certificateChain)
        {
            for (size_t i = 0; i < *chainLength; ++i)
            {
                OICFree((*certificateChain)[i]);
            }
        }
        OICFree(*certificateChain);
        *certificateChain = NULL;
        *privKey     = NULL;
        *chainLength = 0;
        if (OC_STACK_NO_MEMORY == ret)
        {
            OIC_LOG(ERROR, TAG, "Error while memory allocation");
        }
        return ret;
    }

    memcpy(*privKey, privB64buf, privB64len + 1);

    return OC_STACK_OK;
}

OCStackResult PMGenerateCertificateCredentials(const OicUuid_t *ptDeviceId,
        const OicUuid_t *deviceId, OicSecCred_t **const cred)
{
    if (NULL == ptDeviceId || NULL == deviceId || NULL == cred)
    {
        return OC_STACK_INVALID_PARAM;
    }
    char **certificateChain = NULL;
    char *privKey = NULL;
    size_t certChainLen = 0;
    if (OC_STACK_OK != GenerateCertificateAndKeys(deviceId, &certificateChain,
            &certChainLen, &privKey))
    {
        OIC_LOG(ERROR, TAG, "Error while generating credential data.");
        return OC_STACK_ERROR;
    }

    uint8_t *publicJWK = NULL;
    size_t len = 0;
    if (OC_STACK_OK == CreateCertificatePublicJWK(certificateChain, certChainLen, &publicJWK, &len))
    {

    }
    uint8_t *privateJWK = NULL;
    size_t len1 = 0;
    if (OC_STACK_OK == CreateCertificatePrivateJWK(privKey, &privateJWK, &len1))
    {

    }
    for (size_t i = 0; i < certChainLen; ++i)
    {
        OICFree(certificateChain[i]);
    }
    OICFree(certificateChain);
    OICFree(privKey);
    if (NULL == publicJWK || NULL == privateJWK)
    {
        OICFree(publicJWK);
        OICFree(privateJWK);
        OIC_LOG(ERROR, TAG, "Error while converting keys to JWK format.");
        return OC_STACK_ERROR;
    }

    OicSecCred_t *tempCred =  GenerateCredential(deviceId, SIGNED_ASYMMETRIC_KEY, publicJWK,
                              privateJWK, 1, ptDeviceId);
    OICFree(publicJWK);
    OICFree(privateJWK);
    if (NULL == tempCred)
    {
        OIC_LOG(ERROR, TAG, "Error while generating credential.");
        return OC_STACK_ERROR;
    }
    *cred = tempCred;
    return OC_STACK_OK;
}
#endif // __WITH_X509__
