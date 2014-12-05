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
 * @file cawificore.h
 * @brief This file contains the APIs for Wi-Fi communications.
 */
#ifndef __CA_WIFICORE_H_
#define __CA_WIFICORE_H_

#include "cacommon.h"
#include "config.h"
#include "coap.h"
#include "uthreadpool.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef void (*CAPacketReceiveCallback)(char* address, const int port, const char* data,
        uint32_t length);

void CAWiFiSetCallback(CAPacketReceiveCallback callback);

void CAWiFiInitMutex();

void CAWiFiInitialize(u_thread_pool_t handle);

int32_t CABindUnicastSocket();

void CAWiFiTerminate();

int32_t CAWiFiSendUnicastMessage(const char* address, const int port, char* data, uint32_t length);

int32_t CAWiFiSendMulticastMessage(const char* m_address, char* data, uint32_t dataLen);

int32_t CAWiFiStartUnicastServer();

int32_t CAWiFiStartMulticastServer();

int32_t CAWiFiStopUnicastServer();

int32_t CAWiFiStopMulticastServer();

void CAGetLocalAddress(char *addressBuffer);

int32_t CASendUnicastMessageImpl(const char *address, const int port, const char *data,
        uint32_t len);

int32_t CASendMulticastMessageImpl(const char *msg, uint32_t len);

CAResult_t CAGetWIFIInterfaceInfo(CALocalConnectivity_t **info, uint32_t* size);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
