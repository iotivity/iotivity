//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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



#ifndef EASYSETUP_ENROLLEE_SOFTAP_H__
#define EASYSETUP_ENROLLEE_SOFTAP_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


#define SOFTAP_SUCCESS  1
#define SOFTAP_FAILED   0

/**
 * @file
 *
 * This file contains the API declaration for creating & configuration for SoftAP
 */

//This callback signagure may get changed as per the native implementation.
typedef void (*SoftAPCallbackNative)(int result, const char *ip, const char* mac_addr,
                                                                        const char*device_name);
/**
 * This API creats the Soft AP using the platform specific native system calls.
 * @Note This API to be implemented by the OEM if Enrollee device SoC supports SoftAP
 *
 * @param networkType           NetworkType on which OnBoarding has to be performed.
 * @param ssid                  SSID of the target SoftAP network to which the Enrollee is
 *                              connecting.
 * @param passwd                Password of the target SoftAP network to which the Enrollee is
 *                              connecting
 * @param SoftAPCallbackNative  EventCallback for for updating the Enrollee OnBoarding status
 *                              result to the application
 */

void ESCreateSoftapNative(const char * ssid, const char* passwd, SoftAPCallbackNative cb);

#ifdef __cplusplus
}
#endif // __cplusplus


#endif /* EASYSETUP_ENROLLEE_SOFTAP_H__ */



