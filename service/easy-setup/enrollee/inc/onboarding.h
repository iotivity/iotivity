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
 * It will on-board the Enrollee device. This is a template method that has
 * core business logic & negotiation algorithm on various on-boarding methods.
 */
bool ESOnboard(const char * ssid, const char* passwd, NetworkEventCallback cb);

/**
 * It will return true on-boarding type is soft AP.
 */
bool ESSoftapOnboarding();

/**
 * It will return true on-boarding type is BLE.
 */
bool ESBleOnboarding();

#ifdef __cplusplus
}
#endif // __cplusplus


#endif /* EASYSETUP_ENROLLEE_ONBOARDING_H__ */
