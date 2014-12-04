/******************************************************************
 *
 * Copyright 2014 Samsung Electronics All Rights Reserved.
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

/**
 * @file caethernetcore.h
 * @brief This file contains the APIs for Ethernet communications.
 */
#ifndef __CA_ETHERNETCORE_H_
#define __CA_ETHERNETCORE_H_

#include "cacommon.h"
#include "config.h"
#include "coap.h"
#include "uthreadpool.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef void (*CAPacketReceiveCallback)(char* address, const char* data);

void CAEthernetInitialize(u_thread_pool_t handle);

void CAEthernetTerminate();

int32_t CAEthernetSendUnicastMessage(const char* address, char* data, int32_t length);

int32_t CAEthernetSendMulticastMessage(const char* m_address, char* data);

int32_t CAEthernetStartUnicastServer();

int32_t CAEthernetStartMulticastServer();

int32_t CAEthernetStopUnicastServer();

int32_t CAEthernetStopMulticastServer();

void CAEthernetSetCallback(CAPacketReceiveCallback callback);

void CAEthernetGetLocalAddress(char *addressBuffer);

int32_t CAEthernetSendUnicastMessageImpl(const char *address, const char *data);

int32_t CAEthernetSendMulticastMessageImpl(const char *msg);

CAResult_t CAGetEthernetInterfaceInfo(CALocalConnectivity_t **info, uint32_t* size);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
