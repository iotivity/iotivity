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


#ifndef EASYSETUP_ENROLLEE_ONBOARDING_H__
#define EASYSETUP_ENROLLEE_ONBOARDING_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus:$

#include "networkhandler.h"

/**
 * @file
 *
 * This file contains APIs to on-board ( connect ) Enrollee device into Ad-hoc network
 * @Note :  Some of the APIs of this file need to be modified by the OEM according to the
 *          device configuration
 */

/**
 * This function on-board Enrollee device onto ad-hoc network.
 * @param ssid                              SSID of the target SoftAP network to which the Enrollee
 *                                          is connecting.
 * @param passwd                            Password of the target SoftAP network to which the
 *                                          Enrollee is connecting.
 * @param ESEnrolleeNetworkEventCallback    Callback function for result update
 * @return ::True, if on-boarding is successful.
 */

bool ESOnboard(const char * ssid, const char* passwd, ESEnrolleeNetworkEventCallback cb);

/**
 * This function verify if the on-boarding is through SoftAP.
 * @return ::True, if Soft AP on-boarding enabled.
 * @Note : This API to be modified by the OEM according to the device configuration
 */
bool ESSoftapOnboarding();

/**
 * This function verify if the on-boarding is through Ble beacons.
 * @return ::True, if Ble beacons enabled.
 * @Note : This API to be modified by the OEM according to the device configuration
 */
bool ESBleOnboarding();

#ifdef __cplusplus
}
#endif // __cplusplus


#endif /* EASYSETUP_ENROLLEE_ONBOARDING_H__ */
