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

#ifndef SRP_SECURERESOURCEPROVIDER_H
#define SRP_SECURERESOURCEPROVIDER_H

#include "ocstack.h"
#include "securevirtualresourcetypes.h"
#include "pmtypes.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * API to send ACL information to resource.
 *
 * @param[in] selectedDeviceInfo Selected target device.
 * @param[in] acl ACL to provision.
 * @param[in] resultCallback callback provided by API user, callback will be called when
 *            provisioning request recieves a response from resource server.
 * @return  SP_SUCCESS in case of success and other value otherwise.
 */
OCStackResult SRPProvisionACL(void *ctx, const OCProvisionDev_t *selectedDeviceInfo,
                                        OicSecAcl_t *acl, OCProvisionResultCB resultCallback);

/**
 * API to provision credential to devices.
 *
 * @param[in] type Type of credentials to be provisioned to the device.
 * @param[in] pDev1 Pointer to PMOwnedDeviceInfo_t instance,respresenting resource to be provsioned.
   @param[in] pDev2 Pointer to PMOwnedDeviceInfo_t instance,respresenting resource to be provsioned.
 * @param[in] resultCallback callback provided by API user, callback will be called when
 *            provisioning request recieves a response from first resource server.
 * @return  OC_STACK_OK in case of success and other value otherwise.
 */
OCStackResult SRPProvisionCredentials(void *ctx,OicSecCredType_t type, size_t keySize,
                                      const OCProvisionDev_t *pDev1,
                                      const OCProvisionDev_t *pDev2,
                                      OCProvisionResultCB resultCallback);
#ifdef __cplusplus
}
#endif
#endif //SRP_SECURERESOURCEPROVIDER_H
