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

#ifndef _RESOURCE_DIRECTORY_CLIENT_H_
#define _RESOURCE_DIRECTORY_CLIENT_H_

// Iotivity Base CAPI
#include "ocstack.h"

#include "rd_types.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * Discovers the resource directory.
 * This function searches a RD server and obtain the bias factor.
 *
 * @param cbBiasFactor callback function invoked when bias factor is returned by
 *                     the Resource Directory Server
 *
 * @return ::OC_STACK_OK upon success, ::OC_STACK_ERROR in case of error.
 */
OCStackResult OCRDDiscover(OCRDBiasFactorCB cbBiasFactor);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //_RESOURCE_DIRECTORY_CLIENT_H_
