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
#include "ocmalloc.h"
#include "logger.h"

#define TAG "SPProvisionAPI"


SPResult SPGenerateCredentials(OicSecCredType_t type, const SPDevInfo_t *pDevInfo,
                               OicUuid_t *deviceId,
                               OicSecCred_t **cred)
{
    switch (type)
    {
        case SYMMETRIC_PAIR_WISE_KEY:
            {
                // TODO after getting more information.
                OicSecCred_t *credInstance = (OicSecCred_t *) OCMalloc(sizeof (OicSecCred_t));
                if (NULL == credInstance)
                {
                    OC_LOG(ERROR, TAG, "Error while allocating memory.");
                    return SP_RESULT_MEM_ALLOCATION_FAIL;
                }
                //credInstance->CredID = // TODO value from SRM DB.
                credInstance->subject = pDevInfo->deviceId;
                credInstance->credType = type;
                memcpy(credInstance->owners, deviceId, UUID_LENGTH); // in accordance with beach head version.
                cred = &credInstance;
                break;
            }
        case SYMMETRIC_GROUP_KEY:
            {
                //TODO
                break;
            }
        case ASYMMETRIC_KEY:
            {
                //TODO
                break;
            }
        case CERTIFICATE:
            {
                //TODO
                break;
            }
        case PIN:
            {
                //TODO
                break;
            }
        default:
            {
                OC_LOG(ERROR, TAG, "Invalid option.");
                return SP_RESULT_INVALID_PARAM;
            }
    }
    return SP_RESULT_INTERNAL_ERROR;
}
