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
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

#ifndef INCLUDE_CASIMULATOR_H_
#define INCLUDE_CASIMULATOR_H_

#include "cacommon.h"

int initialize(void);

#ifdef __WITH_DTLS__
int registerDtlsHandler(void);
#endif

int selectNetwork();

int startServer(void);

int startClient(void);

int handleMessage(void);

void processRequestResponse(void);

void requestHandler(const CAEndpoint_t* endPoint, const CARequestInfo_t* requestInfo);

void responseHandler(const CAEndpoint_t* endPoint, const CAResponseInfo_t* responseInfo);

void errorHandler(const CAEndpoint_t *rep, const CAErrorInfo_t* errorInfo);

int registerMessageHandler(void);

void customWait(int seconds);

void clearDisplay();

#endif
