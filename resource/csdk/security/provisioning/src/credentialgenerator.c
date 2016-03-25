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
//Certificate-related functions
#define CERT_LEN_PREFIX (3)
#define BYTE_SIZE (8) //bits

#define CHAIN_LEN (2) //TODO: replace by external define or a runtime value
#endif  //__WITH_X509__

#define TAG "SRPAPI-CG"

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
    OicSecKey_t privKey = {privData, keySize};

    OCFillRandomMem(privData, privDataKeySize);

    // TODO: currently owner array is 1. only provisioning tool's id.
    tempFirstCred =  GenerateCredential(secondDeviceId, type, NULL, &privKey, ptDeviceId);
    VERIFY_NON_NULL(TAG, tempFirstCred, ERROR);

    // TODO: currently owner array is 1. only provisioning tool's id.
    tempSecondCred =  GenerateCredential(firstDeviceId, type, NULL, &privKey, ptDeviceId);
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
static void writeCertPrefix(uint8_t *prefix, uint32_t certLen)
{
    for (size_t i = 0; i < CERT_LEN_PREFIX; ++i)
    {
        prefix[i] = (certLen >> (BYTE_SIZE * (CERT_LEN_PREFIX - 1 - i))) & 0xFF;
    }
}

static uint32_t appendCert2Chain(uint8_t *appendPoint, uint8_t *cert, size_t len)
{
    uint32_t ret = 0;
    VERIFY_NON_NULL(TAG, appendPoint, ERROR);
    VERIFY_NON_NULL(TAG, cert, ERROR);

    memcpy(appendPoint + CERT_LEN_PREFIX, cert, len);
    writeCertPrefix(appendPoint, len);

    ret = len + CERT_LEN_PREFIX;
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
static OCStackResult GenerateCertificateAndKeys(const OicUuid_t * subject, OicSecCert_t * certificateChain,
        OicSecKey_t * privKey)
{
    if (NULL == subject || NULL == certificateChain || NULL == privKey)
    {
        return  OC_STACK_INVALID_PARAM;
    }
    certificateChain->data = NULL;
    privKey->data = NULL;

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

    uint8_t numCert = 0;
    if (PKI_SUCCESS != GetCAChain(&numCert , cert + 1))
    {
        OIC_LOG(ERROR, TAG, "Error getting CA certificate chain.");
        return OC_STACK_ERROR;
    }

    numCert ++;
    uint32_t len = 0;
    for (size_t i = 0; i < numCert; i++)
    {
        certificateChain->data = (uint8_t *) OICRealloc(certificateChain->data,
                                                        len + cert[i].len + CERT_LEN_PREFIX);
        if (NULL == certificateChain->data)
        {
            OIC_LOG(ERROR, TAG, "Error while memory allocation");
            return OC_STACK_ERROR;
        }

        uint32_t appendedLen = appendCert2Chain(certificateChain->data + len,
                                                cert[i].data, cert[i].len);
        if (0 == appendedLen)
        {
            OIC_LOG(ERROR, TAG, "Error while certifiacate chain creation.");
            OICFree(certificateChain->data);
            certificateChain->len = 0;
            return OC_STACK_ERROR;
        }
        len += appendedLen;
    }
    certificateChain->len = len;
    privKey->data = (uint8_t*) OICMalloc(PRIVATE_KEY_SIZE);
    if (NULL == privKey->data)
    {
        OIC_LOG(ERROR, TAG, "Error while memory allocation");
        OICFree(certificateChain->data);
        certificateChain->len = 0;
        privKey->len = 0;
        return OC_STACK_ERROR;
    }
    memcpy(privKey->data, privKeyData, PRIVATE_KEY_SIZE);
    privKey->len = PRIVATE_KEY_SIZE;

    return OC_STACK_OK;
}

OCStackResult PMGenerateCertificateCredentials(const OicUuid_t *ptDeviceId,
        const OicUuid_t *deviceId, OicSecCred_t **const cred)
{
    if (NULL == ptDeviceId || NULL == deviceId || NULL == cred)
    {
        return OC_STACK_INVALID_PARAM;
    }
    OicSecCert_t certificateChain;
    OicSecKey_t privKey;
    if (OC_STACK_OK != GenerateCertificateAndKeys(deviceId, &certificateChain, &privKey))
    {
        OIC_LOG(ERROR, TAG, "Error while generating credential data.");
        return OC_STACK_ERROR;
    }

    *cred = GenerateCredential(deviceId, SIGNED_ASYMMETRIC_KEY, &certificateChain,
                              &privKey, ptDeviceId);
    return OC_STACK_OK;
}
#endif // __WITH_X509__
