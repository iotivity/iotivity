//******************************************************************
//
// Copyright 2015 Intel Mobile Communications GmbH All Rights Reserved.
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

#ifndef SECURITYRESOURCEMANAGER_H_
#define SECURITYRESOURCEMANAGER_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Register request and response callbacks.
 *          Requests and responses are delivered in these callbacks .
 * @param   ReqHandler   [IN] Request callback ( for GET,PUT ..etc)
 * @param   RespHandler  [IN] Response Handler Callback
 * @return
 *     OC_STACK_OK    - no errors
 *     OC_STACK_INVALID_PARAM - invalid parameter
 */
OCStackResult SRMRegisterHandler(CARequestCallback ReqHandler, CAResponseCallback RespHandler);

#ifdef __cplusplus
}
#endif

#endif /* SECURITYRESOURCEMANAGER_H_ */
