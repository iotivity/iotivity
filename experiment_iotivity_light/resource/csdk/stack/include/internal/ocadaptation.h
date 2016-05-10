//******************************************************************
//
// Copyright 2016 Intel Corporation All Rights Reserved.
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

/**
 * @file
 *
 * This file describes the platform adaptation interface
 */

#ifndef OC_ADAPTATION_H_
#define OC_ADAPTATION_H_

#include "ocmessages.h"

OCStackResult CAInitialize();

void CATerminate();

void CAHandleRequestResponse();

void CASendPDU(CAMessageInfo_t *messageInfo);

void OCHandleIncoming(CAMessageInfo_t *messageInfo);

#if defined(WITH_CLIENT) || defined(WITH_PRESENCE)
void CAGenerateToken(OCToken_t *token, int tokenLength);
#endif //  defined(WITH_CLIENT) || defined(WITH_PRESENCE)

#ifdef WITH_CLIENT
uint32_t GetTicks(uint32_t afterMilliSeconds);
#endif // WITH_CLIENT

#endif /* OC_ADAPTATION_H_ */

void *OCMalloc(MAType_t type);

void *OCCalloc(MAType_t type);

void OCFree(void *buf, MAType_t type);
