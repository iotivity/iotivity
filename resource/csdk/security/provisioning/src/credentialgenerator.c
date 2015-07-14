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
#include "oic_malloc.h"
#include "logger.h"
#include "credresource.h"
#include "ocrandom.h"
#include "base64.h"
#include "stdbool.h"
#include "securevirtualresourcetypes.h"

#define TAG "SRPAPI-CG"

/**
 * Macro to verify success of operation.
 *        eg: PM_VERIFY_SUCCESS(TAG, OC_STACK_OK == foo(), OC_STACK_ERROR, ERROR);
 * @note Invoking function must define "bail:" label for goto functionality to work correctly and
 *       must define "OCStackResult res" for setting error code.
 * */
#define PM_VERIFY_SUCCESS(tag, op, errCode, logLevel) { if (!(op)) \
                       {OC_LOG((logLevel), tag, PCF(#op " failed!!")); res = errCode; goto bail;} }
/**
 * Macro to verify argument is not equal to NULL.
 *        eg: PM_VERIFY_NON_NULL(TAG, ptrData, ERROR);
 * @note Invoking function must define "bail:" label for goto functionality to work correctly.
 * */
#define PM_VERIFY_NON_NULL(tag, arg, errCode, logLevel) { if (NULL == (arg)) \
                   { OC_LOG((logLevel), tag, PCF(#arg " is NULL")); res = errCode; goto bail;} }

OCStackResult PMGeneratePairWiseCredentials(OicSecCredType_t type, size_t keySize,
                                    const OicUuid_t *ptDeviceId,
                                    const OicUuid_t *firstDeviceId, const OicUuid_t *secondDeviceId,
                                    OicSecCred_t **firstCred, OicSecCred_t **secondCred)
{
    if (NULL == ptDeviceId || NULL == firstDeviceId || NULL != *firstCred || \
        NULL == secondDeviceId || NULL != *secondCred)
    {
        OC_LOG(INFO, TAG, "Invalid params");
        return OC_STACK_INVALID_PARAM;
    }
    if(!(keySize == OWNER_PSK_LENGTH_128 || keySize == OWNER_PSK_LENGTH_256))
    {
        OC_LOG(INFO, TAG, "Invalid key size");
        return OC_STACK_INVALID_PARAM;
    }
    OCStackResult res = OC_STACK_ERROR;
    uint8_t* privData = NULL;
    char* base64Buff = NULL;
    OicSecCred_t *tempFirstCred = NULL;
    OicSecCred_t *tempSecondCred = NULL;

    size_t privDataKeySize = keySize;

    privData = (uint8_t*) OICCalloc(privDataKeySize,sizeof(uint8_t));
    PM_VERIFY_NON_NULL(TAG, privData, OC_STACK_NO_MEMORY, ERROR);

    OCFillRandomMem(privData,privDataKeySize);

    uint32_t outLen = 0;

    base64Buff = (char*) OICCalloc(B64ENCODE_OUT_SAFESIZE(privDataKeySize) + 1, sizeof(char));
    PM_VERIFY_NON_NULL(TAG, base64Buff, OC_STACK_NO_MEMORY, ERROR);
    int memReq = (B64ENCODE_OUT_SAFESIZE(privDataKeySize) + 1) * sizeof(char);
    B64Result b64Ret = b64Encode(privData, privDataKeySize*sizeof(uint8_t), base64Buff,
                                 memReq, &outLen);
    PM_VERIFY_SUCCESS(TAG, B64_OK == b64Ret, OC_STACK_ERROR, ERROR);

    // TODO: currently owner array is 1. only provisioning tool's id.
    tempFirstCred =  GenerateCredential(secondDeviceId, type, NULL, base64Buff, 1, ptDeviceId);
    PM_VERIFY_NON_NULL(TAG, tempFirstCred, OC_STACK_ERROR, ERROR);

    // TODO: currently owner array is 1. only provisioning tool's id.
    tempSecondCred =  GenerateCredential(firstDeviceId, type, NULL, base64Buff, 1, ptDeviceId);
    PM_VERIFY_NON_NULL(TAG, tempSecondCred, OC_STACK_ERROR, ERROR);

    *firstCred = tempFirstCred;
    *secondCred = tempSecondCred;
    res = OC_STACK_OK;

bail:
    OICFree(privData);
    OICFree(base64Buff);

    if(res != OC_STACK_OK)
    {
        OICFree(tempFirstCred);
        OICFree(tempSecondCred);
        *firstCred = NULL;
        *secondCred = NULL;
    }

    return res;
}
