//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
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

#ifndef _NS_PROVIDER_NOTIFICATION_H_
#define _NS_PROVIDER_NOTIFICATION_H_

#include <ocstack.h>
#include "logger.h"
#include "NSQueueScheduler.h"
#include "NSProviderListener.h"
#include "NSProviderResource.h"
#include "NSProviderCacheAdapter.h"

NSResult NSRegisterResource();

NSResult NSGetObsIdList(OCObservationId *list);

uint32_t NSGetNumberOfObsList();

NSResult NSPutNotificationMessage(NSMessage *msg, OCResourceHandle *rHandle);

NSResult NSSendNotificationMessage(NSMessage *msg);

#endif /* _NS_PROVIDER_NOTIFICATION_H_ */
