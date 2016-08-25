//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef OC_RESOURCE_DIRECTORY_H_
#define OC_RESOURCE_DIRECTORY_H_

#include "octypes.h"
#include "logger.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifdef RD_CLIENT
/**
 * Publish RD resource to Resource Directory.
 *
 * @param host The address of the RD.
 * @param connectivityType Type of connectivity indicating the interface.
 * @param resourceHandles This is the resource handle which we need to register to RD.
 * @param nHandles The counts of resource handle.
 * @param cbData Asynchronous callback function that is invoked by the stack when
 *               response is received. The callback is generated for each response
 *               received.
 * @param qos Quality of service.
 *
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
OC_EXPORT OCStackResult OCRDPublish(const char *host, OCConnectivityType connectivityType,
                                    OCResourceHandle *resourceHandles, uint8_t nHandles,
                                    OCCallbackData *cbData, OCQualityOfService qos);

/**
 * Publish RD resource to Resource Directory with a specific id.
 *
 * @param host The address of the RD.
 * @param id An unique identifier of publishing device.
 * @param connectivityType Type of connectivity indicating the interface.
 * @param resourceHandles This is the resource handle which we need to register to RD.
 * @param nHandles The counts of resource handle.
 * @param cbData Asynchronous callback function that is invoked by the stack when
 *               response is received. The callback is generated for each response
 *               received.
 * @param qos Quality of service.
 *
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
OC_EXPORT OCStackResult OCRDPublishWithDeviceId(const char *host, const unsigned char *id,
                                                OCConnectivityType connectivityType,
                                                OCResourceHandle *resourceHandles, uint8_t nHandles,
                                                OCCallbackData *cbData, OCQualityOfService qos);

/**
 * Delete RD resource from Resource Directory.
 *
 * @param host The address of the RD.
 * @param connectivityType Type of connectivity indicating the interface.
 * @param resourceHandles This is the resource handle which we need to delete to RD.
 * @param nHandles The counts of resource handle.
 * @param cbData Asynchronous callback function that is invoked by the stack when
 *               response is received. The callback is generated for each response
 *               received.
 * @param qos Quality of service.
 *
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
OC_EXPORT OCStackResult OCRDDelete(const char *host, OCConnectivityType connectivityType,
                                   OCResourceHandle *resourceHandles, uint8_t nHandles,
                                   OCCallbackData *cbData, OCQualityOfService qos);

/**
 * Delete RD resource from Resource Directory.
 *
 * @param host The address of the RD.
 * @param id An unique identifier of publishing device.
 * @param connectivityType Type of connectivity indicating the interface.
 * @param resourceHandles This is the resource handle which we need to delete to RD.
 * @param nHandles The counts of resource handle.
 * @param cbData Asynchronous callback function that is invoked by the stack when
 *               response is received. The callback is generated for each response
 *               received.
 * @param qos Quality of service.
 *
 * @return ::OC_STACK_OK on success, some other value upon failure.
 */
OC_EXPORT OCStackResult OCRDDeleteWithDeviceId(const char *host, const unsigned char *id,
                                               OCConnectivityType connectivityType,
                                               OCResourceHandle *resourceHandles, uint8_t nHandles,
                                               OCCallbackData *cbData, OCQualityOfService qos);
#endif
#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* OC_RESOURCE_DIRECTORY_H_ */
