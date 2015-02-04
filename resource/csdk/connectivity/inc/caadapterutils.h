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
 * @file caadapterutils.h
 * @brief This file contains common utility function for CA transport adaptors.
 */

#ifndef _CA_ADAPTER_UTILS_H_
#define _CA_ADAPTER_UTILS_H_

#include "cacommon.h"
#include "logger.h"
#include "pdu.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @def VERIFY_NON_NULL
 * @brief Macro to verify the validity of input argument
 */
#define VERIFY_NON_NULL(arg, log_tag, log_message) \
    if (NULL == arg ){ \
        OIC_LOG_V(ERROR, log_tag, "Invalid input:%s", log_message); \
        return CA_STATUS_INVALID_PARAM; \
    } \

/**
 * @def VERIFY_NON_NULL_RET
 * @brief Macro to verify the validity of input argument
 */
#define VERIFY_NON_NULL_RET(arg, log_tag, log_message,ret) \
    if (NULL == arg ){ \
        OIC_LOG_V(ERROR, log_tag, "Invalid input:%s", log_message); \
        return ret; \
    } \

/**
 * @def VERIFY_NON_NULL_VOID
 * @brief Macro to verify the validity of input argument
 */
#define VERIFY_NON_NULL_VOID(arg, log_tag, log_message) \
    if (NULL == arg ){ \
        OIC_LOG_V(ERROR, log_tag, "Invalid input:%s", log_message); \
        return; \
    } \

/**
 * @def IPV4_ADDR_ONE_OCTECT_LEN
 * @brief Macro to allocate memory for ipv4 address in the form of uint8_t.
 */
#define IPV4_ADDR_ONE_OCTECT_LEN 4

/**
 * @brief To log the PDU data
 */
void CALogPDUData(coap_pdu_t *pdu);

/**
 * @fn CAAdapterCreateLocalEndpoint
 * @brief Create CALocalConnectivity_t instance.
 */
CALocalConnectivity_t *CAAdapterCreateLocalEndpoint(CAConnectivityType_t type,
        const char *address);

/**
 * @fn CAAdapterCopyLocalEndpoint
 * @brief Create CALocalConnectivity_t duplicate instance.
 */
CALocalConnectivity_t *CAAdapterCopyLocalEndpoint(const CALocalConnectivity_t *connectivity);

/**
 * @fn CAAdapterFreeLocalEndpoint
 * @brief Deallocate CALocalConnectivity_t instance.
 */
void CAAdapterFreeLocalEndpoint(CALocalConnectivity_t *localEndPoint);

/**
 * @fn CAAdapterCreateRemoteEndpoint
 * @brief Allocate CARemoteEndpoint_t instance.
 */
CARemoteEndpoint_t *CAAdapterCreateRemoteEndpoint(CAConnectivityType_t type,
        const char *address, const char *resourceUri);

/**
 * @fn CAAdapterCopyRemoteEndpoint
 * @brief Create CARemoteEndpoint_t duplicate instance.
 */
CARemoteEndpoint_t *CAAdapterCopyRemoteEndpoint(
    const CARemoteEndpoint_t *remoteEndpoint);

/**
 * @fn CAAdapterFreeRemoteEndpoint
 * @brief Deallocate CARemoteEndpoint_t instance.
 */
void CAAdapterFreeRemoteEndpoint(CARemoteEndpoint_t *remoteEndPoint);

/**
 * @fn CAParseIPv4AddressInternal
 * @brief   To parse the IP address and port from "ipaddress:port"
 * @param   ipAddrStr   [IN]   IP address to be parsed
 * @param   ipAddr      [OUT]  Parsed IP address
 * @param   ipAddr      [IN]   Buffer length for parsed IP address
 * @param   port        [OUT]  Parsed Port number
 * @return  #CA_STATUS_OK or Appropriate error code
 */
CAResult_t CAParseIPv4AddressInternal(const char *ipAddrStr, uint8_t *ipAddr,
                                      size_t ipAddrLen, uint16_t *port);

/**
 * @fn CAAdapterIsSameSubnet
 * @brief Check if two ip address belong to same subnet
 */
bool CAAdapterIsSameSubnet(const char *ipAddress1, const char *ipAddress2,
                           const char *netMask);
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif  // _CA_ADAPTER_UTILS_H_

