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

#include "routingmanager.h"
#include "routingmanagerutility.h"
#include "logger.h"
#include "caadapterutils.h"
#include "oic_malloc.h"

/**
 * @def TAG
 * @brief Logging tag for module name.
 */
#define TAG "RM"

/**
 * @brief Maximum number of hopes after which multicast packet is dropped. Helps in finding loops.
 *        Set to 1 for current single hop implementation
 */
#define MAX_MULTICAST_HOP_COUNT 1

/**
 * @brief   Callback function registered with CA.  On reception of data, CA sends to this callback
 *          function.  This checks if the route option is present and adds routing information to
 *          to the route option data.
 * @param   message     [IN,OUT] Received coap packet. Route option will be
 *                               added to the coap packet.
 * @param   sender      [IN]     RemoteEndpoint which sent the packet.
 * @param   destination [OUT]    Populated by RM by parsing message, CA then forwards packet to
 *                               "destination".
 * @return  #CA_STATUS_OK or Appropriate error code.
 */
CAResult_t RMRouteMessageHandler(CAInfo_t *message, const CAEndpoint_t *sender,
                                 CAEndpoint_t *destination);

CAResult_t RMInitialize()
{
    OIC_LOG(DEBUG, TAG, "IN");
    CAResult_t ret = CARegisterRoutingMessageHandler(RMRouteMessageHandler);
    OIC_LOG(DEBUG, TAG, "OUT");
    return ret;
}

CAResult_t RMTerminate()
{
    OIC_LOG(DEBUG, TAG, "IN");
    CAResult_t ret = CARegisterRoutingMessageHandler(NULL);
    OIC_LOG(DEBUG, TAG, "OUT");
    return ret;
}

// destination memory is allocated here.
CAResult_t RMRouteMessageHandler(CAInfo_t *info, const CAEndpoint_t *sender,
                                 CAEndpoint_t *destination)
{
    OIC_LOG(DEBUG, TAG, "IN");
    VERIFY_NON_NULL(info, TAG, "info");
    VERIFY_NON_NULL(sender, TAG, "sender");

    // Check if Options are present in the CoAP packet
    if (0 >= info->numOptions)
    {
        OIC_LOG(ERROR, TAG, "No options present");
        return CA_NOT_SUPPORTED;
    }

    // 1.  Parse options and check the following:
    //      a) option ID of Message switching is present - on false return CA_NOT_SUPPORTED
    int8_t routeIndex = -1;
    RMGetRouteOptionIndex(info->options, info->numOptions, &routeIndex);
    if (-1 >= routeIndex)
    {
        OIC_LOG(ERROR, TAG, "No route option present");
        return CA_NOT_SUPPORTED;
    }

    RMRouteOption_t routeOption = { 0 };
    RMParseRouteOption(&info->options[routeIndex], &routeOption);

    //      b) if the hopcount in option > Max Hop count
    if (MAX_MULTICAST_HOP_COUNT <= routeOption.hopCount)
    {
        OIC_LOG_V(ERROR, TAG, "Maximum hop count reached!! hopcount: %d", routeOption.hopCount);
        return CA_STATUS_FAILED;
    }


    //      c) if the option has source option -> return CA_STATUS_FAILED
    if ('\0' != routeOption.srcAddr[0])
    {
        OIC_LOG(ERROR, TAG, "Source Address is present");
        return CA_STATUS_FAILED;
    }

    // 2.  If destination option is present, copy the address to "destination".
    //     Remove the destination option.
    if ('\0' != routeOption.destAddr[0])
    {
        OIC_LOG(INFO, TAG, "Destination route info is present");

        destination->adapter= RMGetAddressInfo(routeOption.destAddr,
                                                      destination->addr, &destination->port);
        memset(routeOption.destAddr, '\0', sizeof(routeOption.destAddr));
    }

    // 3.  Increment the HopCount.
    routeOption.hopCount++;
    RMCreateRouteOption(&routeOption, &info->options[routeIndex]);

    // 4.  Add "source option" to the coap packet from the "sender". Increment the numOptions.
    CAResult_t res = RMAddSourceToRouteOption(sender, &info->options, &info->numOptions);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG(ERROR, TAG, "Adding source option failed");
        return res;
    }

    OIC_LOG_V(INFO, TAG, "Number of options is %d", info->numOptions);
    OIC_LOG_V(INFO, TAG, "Option data length is %d", info->options[routeIndex].optionLength);
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}