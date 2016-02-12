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

//#ifdef __cplusplus
//extern "C" {
//#endif // __cplusplus

/**
 * @file
 *
 * This file contains the configuration for SoftAP
 */

/**
 * It will return true if the Soft AP is created at Enrollee device
 */
bool ESSoftapAtEnrollee();


/**
 * It will return true if the Soft AP is created at Mediator device
 */

bool ESSoftapAtMediator();

/**
 * It will return Device which is creating Soft AP ;  Devices are identified by their role
 * i.e. Enrollee or Mediator.
 */
DeviceRole ESSoftapHost();

/**
 * Handle Soft AP creation
 */
void ESCreateSoftap(const char * ssid, const char* passwd, NetworkEventCallback cb);


//#ifdef __cplusplus
//}
//#endif // __cplusplus


#endif /* EASYSETUP_ENROLLEE_SOFTAP_H__ */



