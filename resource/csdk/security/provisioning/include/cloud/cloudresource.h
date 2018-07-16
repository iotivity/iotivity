/* *****************************************************************
 *
 * Copyright 2018 Samsung Electronics All Rights Reserved.
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

#ifndef OC_CLOUD_RESOURCE_TYPES_H
#define OC_CLOUD_RESOURCE_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#define OIC_RSRC_TYPE_SEC_CLOUDCONF             "oic.r.coapcloudconf"
#define OIC_RSRC_CLOUDCONF_URI                  "/CoapCloudConfResURI"
#define OIC_JSON_CLOUDCONF_NAME                 "CoAPCloudConf"
#define OIC_RSRC_CLOUDCONF_TOKEN_REFRESH        "/oic/sec/tokenrefresh"
#define OC_RSRVD_ACCESS_TOKENTYPE               "tokentype"

/**
 * Perform cleanup for Cloud resources.
 *
 * @return ::OC_STACK_OK for Success, otherwise some error value.
 */
OCStackResult DeInitCloudResource();

/**
 * Initialize Cloud resource by loading data from persistent storage.
 *
 * @return ::OC_STACK_OK for Success, otherwise some error value.
 */
OCStackResult InitCloudResource();

/**
* The method is used to create '/oic/cloudconf' resource.
*/
OCStackResult CreateCloudResource();

/**
* Update the persistent storage
*/
bool UpdateCloudPersistentStorage();

/**
* SignOut & release clouds
*/
void CloudsSignOut();

/**
* Release cloud entries
*/
void ResetClouds();

/**
* Delete clouds acount
*/
void DeleteCloudAccount();

#ifdef __cplusplus
}
#endif

#endif // OC_CLOUD_RESOURCE_TYPES_H
