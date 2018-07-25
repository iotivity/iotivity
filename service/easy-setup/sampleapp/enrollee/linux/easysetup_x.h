/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 *
 *
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
 ******************************************************************/

#include "easysetup.h"
#include "escommon.h"

#ifndef EASYSETUPX_ENROLLEE_H__
#define EASYSETUPX_ENROLLEE_H__

#define MAXLEN_STRING 100

typedef struct UserProperties_t
{
    int userValue_int;                 /*!< User-specific property in WiFi resource **/
    char userValue_str[MAXLEN_STRING]; /*!< User-specific property in DevConf resource **/
} UserProperties;

#define USERPROPERTY_KEY_INT "x.user.property.int"
#define USERPROPERTY_KEY_STR "x.user.property.str"

/**
 * API to parse user-specific properties from received POST request.
 *
 * @param payload         Represents a received POST request
 * @param resourceType    Used to distinguish which resource the received property belongs to
 * @param userdata        User-specific data you want to deliver to desired users
 */
void ReadUserdataCb(OCRepPayload* payload, char* resourceType, void** userdata);

/**
 * API to set user-specific properties to a response going back to a client.
 *
 * @param payload          Represents a response
 * @param resourceType     Used to distinguish which resource the received property belongs to
 */
void WriteUserdataCb(OCRepPayload* payload, char* resourceType);

/**
 * set user properties
 *
 * @param prop  user properties
 */
void SetUserProperties(const UserProperties *prop);


#endif /* EASYSETUPX_ENROLLEE_H__ */
