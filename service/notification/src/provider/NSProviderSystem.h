/******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
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

#ifndef _NS_PROVIDER_SYSTEM__H_
#define _NS_PROVIDER_SYSTEM__H_

#include <string.h>
#include "experimental/logger.h"
#include "NSCommon.h"
#include "NSConstants.h"
#include "oic_malloc.h"
#include "NSStructs.h"
#include "NSUtil.h"

/**
 * set provider connection state.
 *
 * @param state    connection sstate to set.
 */
void NSSetProviderConnectionState(NSConnectionState state);

/**
 * get provider connection state.
 *
 * @return connection state.
 */
NSConnectionState NSGetProviderConnectionState(void);

/**
 * Initialize provider information.
 *
 * @param userInfo     user information to intialize.
 */
void NSInitProviderInfo(const char * userInfo);

/**
 * Deintialize provider information.
 */
void NSDeinitProviderInfo(void);

/**
 * get provider information
 *
 * @return provider information.
 */
NSProviderInfo * NSGetProviderInfo(void);

/**
 * get user information
 *
 * @return user information.
 */
const char * NSGetUserInfo(void);

/**
 * get policy
 *
 * @return policy value.
 */
bool NSGetPolicy(void);

/**
 * set policy
 *
 * @param policy   policy value to set.
 */
void NSSetPolicy(bool policy);

/**
 * get resource security value.
 *
 * @return resource security value.
 */
bool NSGetResourceSecurity(void);

/**
 * set resource security value.
 *
 * @param secured    true value for security.
 */
void NSSetResourceSecurity(bool secured);

#ifdef WITH_MQ
/**
 * set MQ server information
 *
 * @param serverUri           server uri
 * @param devAddr             device address
 */
void NSSetMQServerInfo(const char * serverUri, OCDevAddr * devAddr);

/**
 * get MQ server information.
 *
 * @return MQ server information.
 */
NSMQServerInfo * NSGetMQServerInfo();
#endif

#endif /* _NS_PROVIDER_SYSTEM__H_ */
