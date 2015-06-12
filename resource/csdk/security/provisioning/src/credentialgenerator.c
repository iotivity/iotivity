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

#include "provisioningmanager.h"
#include "credentialgenerator.h"
#include "oic_malloc.h"
#include "logger.h"
#include "credresource.h"
#include "ocrandom.h"
#include "base64.h"
#define TAG "SPProvisionAPI"
#define KEY_LENGTH 16

SPResult SPGeneratePairWiseCredentials(OicSecCredType_t type, const OicUuid_t *ptDeviceId,
                                       const OicUuid_t *firstDeviceId,
                                       const OicUuid_t *secondDeviceId,
                                       OicSecCred_t **firstCred,
                                       OicSecCred_t **secondCred)
{

    if (NULL == ptDeviceId || NULL == firstDeviceId || NULL == secondDeviceId)
    {
        return SP_RESULT_INVALID_PARAM;
    }
    uint8_t privData[KEY_LENGTH] = {0,};
    OCFillRandomMem(privData, KEY_LENGTH);

    uint32_t outLen = 0;
    char base64Buff[B64ENCODE_OUT_SAFESIZE(sizeof(privData)) + 1] = {};
    B64Result b64Ret = b64Encode(privData, sizeof(privData), base64Buff,
                                sizeof(base64Buff), &outLen);
    if (B64_OK != b64Ret)
    {
        OC_LOG(ERROR, TAG, "Error while encoding key");
        return SP_RESULT_INTERNAL_ERROR;
    }

    // TODO currently owner array is 1. only provisioning tool's id.
    OicSecCred_t *tempFirstCred =  GenerateCredential(secondDeviceId, type, NULL, base64Buff, 1,
                                   ptDeviceId);
    if (NULL == tempFirstCred)
    {
        OC_LOG(ERROR, TAG, "Error while generating credential.");
        return SP_RESULT_INTERNAL_ERROR;
    }
    // TODO currently owner array is 1. only provisioning tool's id.
    OicSecCred_t *tempSecondCred =  GenerateCredential(firstDeviceId, type, NULL, base64Buff, 1,
                                    ptDeviceId);
    if (NULL == tempSecondCred)
    {
        DeleteCredList(tempFirstCred);
        OC_LOG(ERROR, TAG, "Error while generating credential.");
        return SP_RESULT_INTERNAL_ERROR;
    }
    *firstCred = tempFirstCred;
    *secondCred = tempSecondCred;
    return SP_RESULT_SUCCESS;
}
