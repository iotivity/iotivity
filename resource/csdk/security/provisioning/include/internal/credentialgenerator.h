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

#ifndef SP_CREDENTIAL_GENERATOR_H
#define SP_CREDENTIAL_GENERATOR_H

#include "ocstack.h"
#include "securevirtualresourcetypes.h"
#include "provisioningmanager.h"

/**
 * Function to generate credentials according to the type.
 *
 * @param[in]  type      Type of credential.
 * @param[in]  pDevInfo  Instance of structure have device information.
 * @param[in]  deviceId  DeviceID of the provisioning tool.
 * @param[out] cred      Generated credential.
 * @return  SP_SUCCESS on success
 */
SPResult SPGenerateCredentials(OicSecCredType_t type, const SPDevInfo_t *pDevInfo,
                               OicUuid_t *deviceId,
                               OicSecCred_t **cred);

#ifdef __cplusplus
}
#endif
#endif //SP_CREDENTIAL_GENERATOR_H
