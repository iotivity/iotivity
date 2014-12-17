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
 * @file caretransmission_singlethread.h
 * @brief
 */
#ifndef __CA_RETRANSMISSION_SINGLETHREAD_H_
#define __CA_RETRANSMISSION_SINGLETHREAD_H_

#include <stdint.h>

#include "uarraylist.h"
#include "cacommon.h"

/** CA_ETHERNET, CA_WIFI **/
#define DEFAULT_RETRANSMISSION_TYPE     ((1<<0)|(1<<1))

/** default retransmission trying count is 4. **/
#define DEFAULT_RETRANSMISSION_COUNT    4

/** check period is 1 sec. **/
#define RETRANSMISSION_CHECK_PERIOD     1000000

/** retransmission data send method type**/
typedef CAResult_t (*CADataSendMethod_t)(const CARemoteEndpoint_t *endpoint, void *data,
        uint32_t length);

typedef struct
{
    /** retransmission support connectivity type **/
    CAConnectivityType_t supportType;
    /** retransmission trying count **/
    uint8_t tryingCount;

} CARetransmissionConfig_t;

typedef struct
{
    /** send method for retransmission data **/
    CADataSendMethod_t dataSendMethod;
    /** retransmission configure data **/
    CARetransmissionConfig_t config;
    /** Variable to inform the thread to stop **/
    CABool_t isStop;
    /** array list on which the thread is operating. **/
    u_arraylist_t* dataList;
} CARetransmission_t;

#ifdef __cplusplus
extern "C"
{
#endif

CAResult_t CARetransmissionInitialize(CARetransmission_t* context,
                                            CADataSendMethod_t retransmissionSendMethod,
                                            CARetransmissionConfig_t* config);

CAResult_t CARetransmissionSentData(CARetransmission_t* context,
                                                const CARemoteEndpoint_t* endpoint,
                                                const void* pdu, uint32_t size);

CAResult_t CARetransmissionReceivedData(CARetransmission_t* context,
        const CARemoteEndpoint_t* endpoint, const void* pdu, uint32_t size);

CAResult_t CARetransmissionStop(CARetransmission_t* context);

CAResult_t CARetransmissionDestroy(CARetransmission_t* context);

void CACheckRetransmissionList();

void CARetransmissionBaseRoutine(void *threadValue);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  // __CA_RETRANSMISSION_SINGLETHREAD_H_
