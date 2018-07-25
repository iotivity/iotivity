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

#ifndef _NS_PROVIDER_CALLBACK_RESPONSE__H_
#define _NS_PROVIDER_CALLBACK_RESPONSE__H_

#include <string.h>
#include "experimental/logger.h"
#include "NSCommon.h"
#include "NSConstants.h"
#include "oic_malloc.h"
#include "NSStructs.h"
#include "NSUtil.h"

/**
 * register subscriber request callback
 *
 * @param subscribeRequestCb    subscriber request callback.
 */
void NSRegisterSubscribeRequestCb(NSSubscribeRequestCallback subscribeRequestCb);
/**
 * register sync callback
 *
 * @param syncCb    sync callback.
 */
void NSRegisterSyncCb(NSProviderSyncInfoCallback syncCb);
/**
 * subscriber request callback
 * @param[in] consumer     notification consumer
 */
void NSSubscribeRequestCb(NSConsumer *consumer);
/**
 * sync callback
 * @param[in] sync   notification sync information
 */
void NSSyncCb(NSSyncInfo *sync);

#endif /* _NS_PROVIDER_CALLBACK_RESPONSE__H_ */
