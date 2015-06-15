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
 * @file routingmanagerutility.h
 * @brief This file contains the utility functions used by Routing manager and CA.
 */
#ifndef ROUTING_MANAGER_UTILITY_H_
#define ROUTING_MANAGER_UTILITY_H_

#include "cacommon.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @def MAX_ADDR_LEN
 * @brief Maximum source or destination address length added to Route Option.
 */
#define MAX_ADDR_LEN 24

/**
 * @struct RMRouteOption_t
 * @brief  This structure is used to hold the hopcount, source and destination address.
 */
typedef struct
{
    uint16_t hopCount;                /**< HopCount */
    char srcAddr[MAX_ADDR_LEN];    /**< Source address of the Message */
    char destAddr[MAX_ADDR_LEN];      /**< Destination address of the Message */
} RMRouteOption_t;

/**
 * @brief   Adds an empty Route option to the Header options.
 * @param   options     [IN,OUT]    Header options present in the Request/response message.
 * @param   numOptions  [IN,OUT]    Number of options present in the message
 * @return  #CA_STATUS_OK or Appropriate error code.
 */
CAResult_t RMAddEmptyRouteOption(CAHeaderOption_t **options, uint8_t *numOptions);

/**
 * @brief   Adds the destination address to the Route options.  If Route option is already present,
 *          it adds the destination address information to Route option else creates a new Route
 *          option with the destination address info.
 * @param   endpoint    [IN]        Destination address
 * @param   options     [IN,OUT]    Header options present in the Request/response message.
 * @param   numOptions  [IN,OUT]    Number of options present in the message
 * @return  #CA_STATUS_OK or Appropriate error code.
 */
CAResult_t RMAddDestToRouteOption(const CARemoteEndpoint_t *endpoint,
                                  CAHeaderOption_t **options, uint8_t *numOptions);

/**
 * @brief   Adds the source address to the Route options.  If Route option is already present,
 *          it adds the source address information to Route option else creates a new Route
 *          option with the source address info.
 * @param   endpoint    [IN]        Source address
 * @param   options     [IN,OUT]    Header options present in the Request/response message.
 * @param   numOptions  [IN,OUT]    Number of options present in the message
 * @return  #CA_STATUS_OK or Appropriate error code.
 */
CAResult_t RMAddSourceToRouteOption(const CARemoteEndpoint_t *endpoint,
                                    CAHeaderOption_t **options, uint8_t *numOptions);

/**
 * @brief   Removes the Route Option from the header options.
 * @param   options     [IN,OUT]    Header options present in request/response message.
 * @param   numOptions  [IN,OUT]    Number of options present in request/response message.
 * @return  #CA_STATUS_OK or Appropriate error code.
 */
CAResult_t RMRemoveRouteOption(CAHeaderOption_t *options, uint8_t *numOptions);

/**
 * @brief   Parses the Route option andd check for destination address.  If destination address is
 *          present, it is copied to the endpoint.
 * @param   options     [IN]    Header options present in request/response message.
 * @param   numOptions  [IN]    Number of options present in request/response message.
 * @param   endpoint    [OUT]   Destination address parsed from the Route option.
 * @return  #CA_STATUS_OK or Appropriate error code.
 */
CAResult_t RMGetDestinationAddress(const CAHeaderOption_t *options, const uint8_t numOptions,
                                   CARemoteEndpoint_t *endpoint);

/**
 * @brief   Gets the index of the routing option if present.
 * @param   options     [IN]    Header options present in request/response message.
 * @param   numOptions  [IN]    Number of options present in request/response message.
 * @param   index       [OUT]   Index of the route option present in Header options.
 * @return  NONE
 */
void RMGetRouteOptionIndex(const CAHeaderOption_t *options, uint8_t numOptions,
                           int8_t *index);

/**
 * @brief To create a Routing option from the CARouteOption_t structure.
 * @param   optValue    [IN]    Routing information.
 * @param   options     [OUT]   Routing information in the form of Header options.
 * @return  #CA_STATUS_OK or Appropriate error code.
 */
CAResult_t RMCreateRouteOption(const RMRouteOption_t *optValue, CAHeaderOption_t *options);

/**
 * @brief To parse the routing option from the Headeroptions.
 * @param   options    [IN]    Routing information in the form of Header options.
 * @param   optValue   [OUT]   Route information after parsing.
 * @return  #CA_STATUS_OK or Appropriate error code.
 */
CAResult_t RMParseRouteOption(const CAHeaderOption_t *options, RMRouteOption_t *optValue);

/**
 * @brief  Get the Information of the address that was sent in Route Option.  The address
 *         format sent in Route Option for IPV4 is "I:<ipAddress>:<port>". For EDR and BLE
 *         I is replaced with B and L respectively.
 * @param   addr        [IN]   Address in the form of routing options.
 * @param   addrInfo    [OUT]  IPV4/EDR/BLE address parsed from addr.
 * @return  Transport type parsed from addr.  -1 in case of Error.
 */
CATransportType_t RMGetAddressInfo(const char *addr, CAAddress_t *addrInfo);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ROUTING_MANAGER_H_ */
