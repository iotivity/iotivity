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
 * @file caretransmission.h
 * @brief This file contains common function for retransmission messages.
 */

#ifndef CA_RETRANSMISSION_H_
#define CA_RETRANSMISSION_H_

#include <stdint.h>

#include "cathreadpool.h"
#include "camutex.h"
#include "uarraylist.h"
#include "cacommon.h"

/** CA_IPV4, CA_EDR, CA_LE **/
#define DEFAULT_RETRANSMISSION_TYPE     (CA_IPV4 | CA_EDR | CA_LE)

/** default ACK time is 2 sec.(CoAP) **/
#define DEFAULT_ACK_TIMEOUT_SEC     2

/** default max retransmission trying count is 4.(CoAP) **/
#define DEFAULT_MAX_RETRANSMIT      4

/** check period is 1 sec. **/
#define RETRANSMISSION_CHECK_PERIOD_SEC     1

/** retransmission data send method type**/
typedef CAResult_t (*CADataSendMethod_t)(const CARemoteEndpoint_t *endpoint, const void *pdu,
                                         uint32_t size);

/** retransmission timeout callback type**/
typedef void (*CATimeoutCallback_t)(const CARemoteEndpoint_t *endpoint, const void *pdu,
                                    uint32_t size);

typedef struct
{
    /** retransmission support transport type **/
    CATransportType_t supportType;

    /** retransmission trying count **/
    uint8_t tryingCount;

} CARetransmissionConfig_t;

typedef struct
{
    /** Thread pool of the thread started **/
    ca_thread_pool_t threadPool;

    /** mutex for synchronization **/
    ca_mutex threadMutex;

    /** conditional mutex for synchronization **/
    ca_cond threadCond;

    /** send method for retransmission data **/
    CADataSendMethod_t dataSendMethod;

    /** callback function for retransmit timeout **/
    CATimeoutCallback_t timeoutCallback;

    /** retransmission configure data **/
    CARetransmissionConfig_t config;

    /** Variable to inform the thread to stop **/
    bool isStop;

    /** array list on which the thread is operating. **/
    u_arraylist_t *dataList;

} CARetransmission_t;

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief   Initializes the retransmission context
 * @param   context                     [IN] context for retransmission
 * @param   handle                      [IN] thread pool handle
 * @param   retransmissionSendMethod    [IN] function to be called for retransmission
 * @param   timeoutCallback             [IN] callback for retransmit timeout
 * @param   config                      [IN] configuration for retransmission.
 *                                           if NULL is coming, it will set default values.
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CARetransmissionInitialize(CARetransmission_t *context, ca_thread_pool_t handle,
                                      CADataSendMethod_t retransmissionSendMethod,
                                      CATimeoutCallback_t timeoutCallback,
                                      CARetransmissionConfig_t* config);

/**
 * @brief   Starting the retransmission context
 * @param   context     [IN] context for retransmission
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CARetransmissionStart(CARetransmission_t *context);

/**
 * @brief   Pass the sent pdu data. if retransmission process need, internal thread will wake up and
 *          process the retransmission data
 * @param   context     [IN] context for retransmission
 * @param   endpoint    [IN] endpoint information
 * @param   pdu         [IN] sent pdu binary data
 * @param   size        [IN] sent pdu binary data size
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CARetransmissionSentData(CARetransmission_t* context,
                                    const CARemoteEndpoint_t* endpoint, const void* pdu,
                                    uint32_t size);

/**
 * @brief   Pass the received pdu data. if received pdu is ACK data for the retransmission CON data,
 *          the specified CON data will remove on retransmission list.
 * @param   context             [IN] context for retransmission
 * @param   endpoint            [IN] endpoint information
 * @param   pdu                 [IN] received pdu binary data
 * @param   size                [IN] received pdu binary data size
 * @param   retransmissionPdu   [OUT] pdu data of the request for reset and ack
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CARetransmissionReceivedData(CARetransmission_t *context,
                                        const CARemoteEndpoint_t *endpoint, const void *pdu,
                                        uint32_t size, void **retransmissionPdu);

/**
 * @brief   Stopping the retransmission context
 * @param   context     [IN] context for retransmission
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CARetransmissionStop(CARetransmission_t *context);

/**
 * @brief   Terminating the retransmission context
 * @param   context     [IN] context for retransmission
 * @return  CA_STATUS_OK or ERROR CODES (CAResult_t error codes in cacommon.h)
 */
CAResult_t CARetransmissionDestroy(CARetransmission_t *context);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* CA_RETRANSMISSION_H_ */
