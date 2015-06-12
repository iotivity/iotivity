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

#ifndef _HOSTING_H_
#define _HOSTING_H_

// Standard API
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Iotivity Base CAPI
#include "ocstack.h"
#include "logger.h"

//#define OC_TRANSPORT OC_ALL
#define OC_TRANSPORT OC_IPV4

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define HOSTING_TAG  PCF("Hosting")

/**
 * Start resource coordinator.
 * This function will create mirrorResourceList and start to discover coordinatee candidate.
 *
 * @return ::OC_STACK_OK upon success, ::OC_STACK_ERROR is returned except the case that OC_STACK_SUCCESS is returned.
 */
OCStackResult OICStartCoordinate();

/**
 * Stop resource coordinator.
 * This function will stop the resource hosting and delete mirrorResourceList used.
 *
 * @return ::OC_STACK_OK upon success, ::OC_STACK_ERROR is returned except the case that OC_STACK_SUCCESS is returned.
 */
OCStackResult OICStopCoordinate();

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //_HOSTING_H_
