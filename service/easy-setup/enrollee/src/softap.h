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

#include "escommon.h"
#include "networkhandler.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file
 *
 * This file contains platform agnostic API for creation of Soft AP
 * @Note :  Some of the APIs of this file need to be modified by the OEM according to the device
 *          configuration
 */

/**
 * This function verify if the Soft AP is created at Enrollee device.
 * @return ::True, if Soft AP is created at Enrollee device.
 * @Note : This API needs to be modified by the OEM according to the device configuration
 */

bool ESSoftapAtEnrollee();

/**
 * This function verify if the Soft AP is created at Mediator device.
 * @return ::True, if Soft AP is created at Mediator device.
 * @Note : This API needs to be modified by the OEM according to the device configuration
 */

bool ESSoftapAtMediator();


/**
 * This function finds which Device (Enrollee or Mediator) is creating Soft AP.
 * @return ::DeviceRole (ENROLLEE or MEDIATOR)
 * @Note : This API needs to be modified by the OEM according to the device configuration
 */

DeviceRole ESSoftapHost();

/**
 * This function Initializes the EasySetup. This API must be called prior to invoking any other API
 *
 * @param ssid                  SSID of the target SoftAP network to which the Enrollee is
 *                              connecting.
 * @param passwd                Password of the target SoftAP network to which the Enrollee is
 *                              connecting
 * @param ESEnrolleeNetworkEventCallback Callback function for result update
 * @return ::void.
 */
void ESCreateSoftap(const char * ssid, const char* passwd, ESEnrolleeNetworkEventCallback cb);


#ifdef __cplusplus
}
#endif

#endif /* EASYSETUP_ENROLLEE_SOFTAP_H__ */



