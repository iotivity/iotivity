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

#ifndef _NS_PROVIDER_SUBSCRIPTION_H_
#define _NS_PROVIDER_SUBSCRIPTION_H_

#include "logger.h"
#include "ocstack.h"
#include "ocpayload.h"
#include "NSCommon.h"
#include "NSConstants.h"
#include "NSQueueScheduler.h"
#include "NSProviderResource.h"

static NSAccessPolicy NSSubscriptionAccepter;

NSResult NSSetSubscriptionAcceptPolicy(NSAccessPolicy policy);
int NSGetSubscriptionAccepter();
NSResult NSSendAccessPolicyResponse(OCEntityHandlerRequest *entityHandlerRequest);
void NSHandleSubscription(OCEntityHandlerRequest *entityHandlerRequest, int state);
void NSHandleUnsubscription(OCEntityHandlerRequest *entityHandlerRequest);
void NSAskAcceptanceToUser(OCEntityHandlerRequest *entityHandlerRequest);
NSResult NSSendSubscriptionResponse(OCEntityHandlerRequest *entityHandlerRequest,
        NSAccessPolicy accepter, bool accepted);

#endif /* _NS_PROVIDER_SUBSCRIPTION_H_ */
