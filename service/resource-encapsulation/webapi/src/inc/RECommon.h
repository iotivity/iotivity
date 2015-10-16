/* *****************************************************************
*
* Copyright 2015 Samsung Electronics All Rights Reserved.
*
*----------------------------------------------------------------------
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************************/


/**
 * @file
 *
 * This file contains the declaration of RE common apis.
 */

#ifndef RE_COMMON_H
#define RE_COMMON_H


#include <iostream>

#include "RCSRemoteResourceObject.h"
#include "RCSDiscoveryManager.h"
#include "RCSResourceObject.h"
#include "RCSAddress.h"
#include "OCPlatform.h"


/**
* decalring namespace
*/
using namespace OIC::Service;
using namespace OC::OCPlatform;
using namespace OC;


/**
 * This function is used to get Remote resource object that we stored for mapping with handle.
 *
 * @param[in] handle - unique identifier for resource object
 *
 * @return shared pointer to RCSRemoteResourceObject
 */
std::shared_ptr<RCSRemoteResourceObject> GetRCSRemoteResourceObject(std::string handle);



/**
 * This function is used to set remote resource object in map.
 *
 * @param[in]  handle - unique identifier for resource object
 * @param[in]  ptr - pointer to remote resource object
 *
 * @return none
 */
void SetRCSRemoteResourceObject(std::string handle, std::shared_ptr<RCSRemoteResourceObject> &ptr);


/**
 * This function is used to create JavaScript handle
 *
 *
 * @param[in]  uri - unique resource identifier
 * @param[in]  address - resource address
 *
 * @return handle - which is an unique identifier to a resource
 */
std::string CreateJSHandle(std::string address, std::string uri);


#endif //RE_COMMON_H