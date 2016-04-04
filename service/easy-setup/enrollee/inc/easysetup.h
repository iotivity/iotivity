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


#ifndef EASYSETUP_ENROLLEE_H__
#define EASYSETUP_ENROLLEE_H__

#include "escommon.h"

/**
 * @file
 *
 * This file contains Enrollee APIs
 */

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/*
 * Callback function for updating the Enrollee OnBoarding and Provisioning status result
 * to the application
 *
 * @param esResult ESResult provides the current state of the Enrollee Device
 */
typedef void (*ESEnrolleeEventCallback)(ESResult esResult, ESEnrolleeState enrolleeState);

/**
 * This function Initializes the EasySetup. This API must be called prior to invoking any other API
 *
 * @param networkType       NetworkType on which OnBoarding has to be performed.
 * @param ssid              SSID of the target SoftAP network to which the Enrollee is connecting.
 * @param passwd            Password of the target SoftAP network to which the Enrollee is
 *                          connecting
 * @param isSecured         True if the Enrollee is operating in secured mode.
 * @param eventCallback     ESEnrolleeEventCallback for for updating the Enrollee OnBoarding status
 *                          result to the application
 * @return ::ES_OK on success, some other value upon failure.
 */
ESResult ESInitEnrollee(OCConnectivityType networkType, const char *ssid, const char *passwd,
                                bool isSecured, ESEnrolleeEventCallback eventCallback);

/**
 * This function performs initialization of Provisioning and Network resources needed for EasySetup
 * process.
 * @return ::ES_OK on success, some other value upon failure.
 */
ESResult ESInitProvisioning();

/**
 * This function performs termination of Provisioning and Network resources.
 * Also terminates the IoTivity core stack.
 *
 * @return ::ES_OK on success, some other value upon failure.
 */
ESResult ESTerminateEnrollee();

#ifdef __cplusplus
}
#endif // __cplusplus


#endif /* EASYSETUP_ENROLLEE_H__ */

