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

#ifndef _RESOURCE_DIRECTORY_SERVER_H_
#define _RESOURCE_DIRECTORY_SERVER_H_

// Iotivity Base CAPI
#include "ocstack.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
* Starts resource directory.
* This function creates a RD server and create resource /oic/rd.
*
* @return ::OC_STACK_OK upon success, ::OC_STACK_ERROR in case of error.
*/
OCStackResult OCRDStart();

/**
* Stops resource directory.
* This function will stop the resource directory and removes all published
* resources information.
*
* @return ::OC_STACK_OK upon success, ::OC_STACK_ERROR is returned except
* the case that OC_STACK_SUCCESS is returned.
*/
OCStackResult OCRDStop();

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //_RESOURCE_DIRECTORY_H_
