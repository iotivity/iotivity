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

#include "routingmanagerutility.h"
#include "logger.h"
#include "caadapterutils.h"
#include "oic_malloc.h"

/**
 * @def TAG
 * @brief Logging tag for module name.
 */
#define TAG "RM_UTIL"

/**
 * @def MIN_ROUTE_OPTION_LEN
 * @brief Minimum routing option data length is
 *        length of src address(1byte) + length of destination address(1byte) + hop count(2bytes)
 */
#define MIN_ROUTE_OPTION_LEN 4

/**
 * @def ADDRESS_PREFIX_LEN
 * @brief Length of the prefix added to source or destination address.
 */
#define ADDRESS_PREFIX_LEN 2

/**
 * @def IP_ADDRESS_PREFIX
 * @brief prefix added to source or destination IP address.
 */
#define IP_ADDRESS_PREFIX "I:"

/**
 * @def EDR_ADDRESS_PREFIX
 * @brief prefix added to source or destination BT MAC address.
 */
#define EDR_ADDRESS_PREFIX "B:"

/**
 * @def BLE_ADDRESS_PREFIX
 * @brief prefix added to source or destination BLE MAC address.
 */
#define BLE_ADDRESS_PREFIX "L:"

#define DEFAULT_PORT 6298

CAResult_t RMAddEmptyRouteOption(CAHeaderOption_t **options, uint8_t *numOptions)
{
    OIC_LOG(DEBUG, TAG, "IN");
    int8_t index = -1;
    if (options)
    {
        RMGetRouteOptionIndex(*options, *numOptions, &index);
    }

    CAHeaderOption_t *emptyOption = NULL;
    if (-1 < index)
    {
        OIC_LOG(INFO, TAG, "Route option is present");
        return CA_STATUS_OK;
    }
    else
    {
        OIC_LOG(INFO, TAG, "Route option is not present");
        index = *numOptions;
        emptyOption = (CAHeaderOption_t *)OICMalloc(sizeof(CAHeaderOption_t)*(*numOptions + 1));
        if (!emptyOption)
        {
            OIC_LOG(ERROR, TAG, "Malloc failed");
            return CA_MEMORY_ALLOC_FAILED;
        }

        if (options)
        {
            memcpy(emptyOption, *options, sizeof(CAHeaderOption_t) * (*numOptions));
        }
    }


    RMRouteOption_t routeOption = { 0 };
    CAResult_t res = RMCreateRouteOption(&routeOption, &emptyOption[(*numOptions)]);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG_V(ERROR, TAG, "Creation of routing option failed");
        OICFree(emptyOption);
        return res;
    }

    if ( *numOptions == index )
    {
        (*numOptions) = (*numOptions) + 1;
        OICFree(*options);
        *options = emptyOption;
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t RMAddDestToRouteOption(const CAEndpoint_t *endpoint,
                                  CAHeaderOption_t **options, uint8_t *numOptions)
{
    OIC_LOG(DEBUG, TAG, "IN");

    VERIFY_NON_NULL(endpoint, TAG, "endpoint");

    CAHeaderOption_t *destOptions = NULL;
    int8_t index = -1;
    if (options)
    {
        RMGetRouteOptionIndex(*options, *numOptions, &index);
    }

    if (-1 < index)
    {
        OIC_LOG(INFO, TAG, "Route option is present");
        destOptions = *options;
    }
    else
    {
        OIC_LOG(INFO, TAG, "Route option is not present");
        index = *numOptions;
        destOptions = (CAHeaderOption_t *)OICMalloc(sizeof(CAHeaderOption_t)*(*numOptions + 1));
        if (!destOptions)
        {
            OIC_LOG(ERROR, TAG, "Malloc failed");
            return CA_MEMORY_ALLOC_FAILED;
        }

        if (options)
        {
            memcpy(destOptions, *options, sizeof(CAHeaderOption_t) * (*numOptions));
        }
    }


    RMRouteOption_t routeOption = { 0 };

    if (*numOptions != index)
    {
        OIC_LOG(INFO, TAG, "Route option is already present");
        RMParseRouteOption(&destOptions[index], &routeOption);
    }

    if ((CA_ADAPTER_IP == endpoint->destAdapter)
        && (('\0' != endpoint->destAddr[0])))
    {
        sprintf(routeOption.destAddr, "%s%s:%d", IP_ADDRESS_PREFIX,
                endpoint->destAddr, endpoint->destPort);
    }
    else if (CA_ADAPTER_RFCOMM_BTEDR == endpoint->destAdapter
            && ('\0' != endpoint->destAddr[0]))
    {
        strcpy(routeOption.destAddr, EDR_ADDRESS_PREFIX);
        strcat(routeOption.destAddr, endpoint->destAddr);
    }
    else if (CA_ADAPTER_GATT_BTLE == endpoint->destAdapter
            && ('\0' != endpoint->destAddr[0]))
    {
        strcpy(routeOption.destAddr, BLE_ADDRESS_PREFIX);
        strcat(routeOption.destAddr, endpoint->destAddr);
    }

    CAResult_t res = RMCreateRouteOption(&routeOption, destOptions + index);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG_V(ERROR, TAG, "Creation of routing option failed");
        OICFree(destOptions);
        return res;
    }

    if ((*numOptions) == index )
    {
        (*numOptions) = (*numOptions) + 1;
        OICFree(*options);
        *options = destOptions;
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t RMAddSourceToRouteOption(const CAEndpoint_t *endpoint,
                                    CAHeaderOption_t **options, uint8_t *numOptions)
{
    OIC_LOG(DEBUG, TAG, "IN");

    VERIFY_NON_NULL(endpoint, TAG, "endpoint")

    CAHeaderOption_t *srcOptions = NULL;
    int8_t index = -1;
    if (options)
    {
        RMGetRouteOptionIndex(*options, *numOptions, &index);
    }

    if (-1 < index)
    {
        OIC_LOG(INFO, TAG, "Route option is present");
        srcOptions = *options;
    }
    else
    {
        OIC_LOG(INFO, TAG, "Route option is not present");
        index = *numOptions;
        srcOptions = (CAHeaderOption_t *)OICMalloc(sizeof(CAHeaderOption_t)*((*numOptions) + 1));
        if (!srcOptions)
        {
            OIC_LOG(ERROR, TAG, "Malloc failed");
            return CA_MEMORY_ALLOC_FAILED;
        }

        if (options)
        {
            memcpy(srcOptions, *options, sizeof(CAHeaderOption_t) * (*numOptions));
        }
    }


    RMRouteOption_t routeOption = { 0 };

    if (*numOptions != index)
    {
        OIC_LOG(INFO, TAG, "Route option is already present");
        RMParseRouteOption(&srcOptions[index], &routeOption);
    }

    OIC_LOG_V(INFO, TAG, "Option hopcount: %d", routeOption.hopCount);
    OIC_LOG_V(INFO, TAG, "Option Sender addr: %s", routeOption.srcAddr);
    OIC_LOG_V(INFO, TAG, "Option destination addr: %s", routeOption.destAddr);

    if ((CA_ADAPTER_IP == endpoint->adapter)
        && (('\0' != endpoint->addr[0])))
    {
        sprintf(routeOption.srcAddr, "%s%s:%d", IP_ADDRESS_PREFIX, endpoint->addr,
                endpoint->port);
    }
    else if (CA_ADAPTER_RFCOMM_BTEDR == endpoint->adapter
            && ('\0' != endpoint->addr[0]))
    {
        strcpy(routeOption.srcAddr, EDR_ADDRESS_PREFIX);
        strcat(routeOption.srcAddr, endpoint->addr);
    }
    else if (CA_ADAPTER_GATT_BTLE == endpoint->adapter
            && ('\0' != endpoint->addr[0]))
    {
        strcpy(routeOption.srcAddr, BLE_ADDRESS_PREFIX);
        strcat(routeOption.srcAddr, endpoint->addr);
    }

    CAResult_t res = RMCreateRouteOption(&routeOption, srcOptions + index);
    if (CA_STATUS_OK != res)
    {
        OIC_LOG_V(ERROR, TAG, "Creation of routing option failed");
        OICFree(srcOptions);
        return CA_STATUS_FAILED;
    }

    if ((*numOptions) == index)
    {
        (*numOptions) = (*numOptions) + 1;
        OICFree(*options);
        *options = srcOptions;
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t RMRemoveRouteOption(CAHeaderOption_t *options, uint8_t *numOptions)
{
    OIC_LOG(DEBUG, TAG, "IN");
    VERIFY_NON_NULL(options, TAG, "options")
    if (0 >= numOptions)
    {
        OIC_LOG(ERROR, TAG, "Invalid arguement: numOptions");
        return CA_STATUS_FAILED;
    }
    int8_t routeIndex = -1;
    RMGetRouteOptionIndex(options, *numOptions, &routeIndex);

    if (-1 >= routeIndex)
    {
        OIC_LOG(DEBUG, TAG, "No route options present");
        return CA_STATUS_OK;
    }

    uint8_t i = routeIndex;
    for (;i < (*numOptions)-1;i++)
    {
        memcpy(&options[i], &options[i+1], sizeof(CAHeaderOption_t));
    }
    *numOptions = (*numOptions) - 1;
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t RMGetDestinationAddress(const CAHeaderOption_t *options, const uint8_t numOptions,
                                   CAEndpoint_t *endpoint)
{
    OIC_LOG(DEBUG, TAG, "IN");

    int8_t index = -1;
    RMGetRouteOptionIndex(options, numOptions, &index);
    if (-1 >= index)
    {
        OIC_LOG(ERROR, TAG, "Route Option is not present");
        return CA_STATUS_FAILED;
    }

    RMRouteOption_t routeOptions = { 0 };
    RMParseRouteOption(&options[index], &routeOptions);
    if ('\0' == routeOptions.srcAddr[0])
    {
        OIC_LOG(ERROR, TAG, "Source Address is not present");
        return CA_STATUS_FAILED;
    }

    endpoint->destAdapter = RMGetAddressInfo(routeOptions.srcAddr, endpoint->destAddr,
                                             &endpoint->destPort);
    if (-1 == endpoint->destAdapter)
    {
        OIC_LOG(ERROR, TAG, "Unknown transport type");
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

void RMGetRouteOptionIndex(const CAHeaderOption_t *options, uint8_t numOptions, int8_t *index)
{
    OIC_LOG(DEBUG, TAG, "IN");
    uint32_t i;
    for (i = 0; i < numOptions; i++)
    {
        OIC_LOG_V(DEBUG, TAG, "Request- optionID: %d", options[i].optionID);
        OIC_LOG_V(DEBUG, TAG, "Request- Data: %s", options[i].optionData);
        if (CA_OPTION_MESSAGE_SWITCHING == options[i].optionID)
        {
            OIC_LOG_V(INFO, TAG, "Found Option at %d", i);
            *index = i;
            break;
        }
    }
    OIC_LOG(DEBUG, TAG, "OUT");
}

CAResult_t RMCreateRouteOption(const RMRouteOption_t *optValue, CAHeaderOption_t *options)
{
    OIC_LOG(DEBUG, TAG, "IN");
    uint8_t dLen = strlen(optValue->destAddr);
    uint8_t sLen = strlen(optValue->srcAddr);
    uint16_t totalLength = MIN_ROUTE_OPTION_LEN + dLen + sLen;
    char *tempData = (char *)OICCalloc(totalLength, sizeof(char));
    if (NULL == tempData)
    {
        OIC_LOG(ERROR, TAG, "Calloc failed");
        return CA_MEMORY_ALLOC_FAILED;
    }
    memcpy(tempData, &dLen, sizeof(dLen));
    uint16_t count = sizeof(dLen);
    if (0 < dLen)
    {
        memcpy(tempData + count, optValue->destAddr, dLen);
    }

    count = count + dLen;
    memcpy(tempData + count, &sLen, sizeof(sLen));
    count = count + sizeof(sLen);
    if (0 < sLen)
    {
        memcpy(tempData + count, optValue->srcAddr, sLen);
    }
    count = count + sLen;
    memcpy(tempData + count, &optValue->hopCount, sizeof(optValue->hopCount));

    memcpy(options->optionData, tempData, totalLength);

    options->optionID = CA_OPTION_MESSAGE_SWITCHING;
    options->optionLength = totalLength;

    OIC_LOG_V(INFO, TAG, "Option Length is %d", options->optionLength);

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t RMParseRouteOption(const CAHeaderOption_t *options, RMRouteOption_t *optValue)
{
    OIC_LOG(DEBUG, TAG, "IN");
    if (0 >= options->optionLength)
    {
        OIC_LOG(ERROR, TAG, "Option data is not present");
        return CA_STATUS_FAILED;
    }

    uint8_t dLen = 0 ;
    uint16_t count = sizeof(dLen);
    memcpy(&dLen, options->optionData, sizeof(dLen));
    if (0 < dLen)
    {
        memcpy(optValue->destAddr, options->optionData + count, dLen);
    }
    count = count + dLen;

    uint8_t sLen = 0;
    memcpy(&sLen, options->optionData + count, sizeof(sLen));
    count = count + sizeof(sLen);
    if (0 < sLen)
    {
        memcpy(optValue->srcAddr, options->optionData + count, sLen);
    }
    count = count + sLen;
    memcpy(&optValue->hopCount, options->optionData + count, sizeof(optValue->hopCount));

    OIC_LOG_V(INFO, TAG, "Option hopcount is %d", optValue->hopCount);
    OIC_LOG_V(INFO, TAG, "Option Sender Addr is %s", optValue->srcAddr);
    OIC_LOG_V(INFO, TAG, "Option Dest Addr is %s", optValue->destAddr);
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CATransportAdapter_t RMGetAddressInfo(const char *addr, char *addrInfo, uint16_t *port)
{
    OIC_LOG(DEBUG, TAG, "IN");
    VERIFY_NON_NULL(addr, TAG, addr);

    char addrPrefix[ADDRESS_PREFIX_LEN + 1];
    strncpy(addrPrefix, addr, ADDRESS_PREFIX_LEN);
    addrPrefix[ADDRESS_PREFIX_LEN] = '\0';

    if (0 == strcmp(addrPrefix, IP_ADDRESS_PREFIX))
    {
        OIC_LOG(INFO, TAG, "Address is IPV4");
        char *index;
        index = strchr(addr + ADDRESS_PREFIX_LEN, ':');
        if (NULL == index)
        {
            OIC_LOG(ERROR, TAG, "port number is not present");
            *port = DEFAULT_PORT; //Assign default port
            memcpy(addrInfo, addr + ADDRESS_PREFIX_LEN,
                   strlen(addr) - ADDRESS_PREFIX_LEN);
        }
        else
        {
            *port = atoi(index + 1);
            memcpy(addrInfo, addr + ADDRESS_PREFIX_LEN,
                   (index - addr) - ADDRESS_PREFIX_LEN);
        }
        OIC_LOG_V(INFO, TAG, "IP Address is %s and Port is %d", addrInfo,
                  *port);
        return CA_ADAPTER_IP;
    }
    else if (0 == strcmp(addrPrefix, EDR_ADDRESS_PREFIX))
    {
        OIC_LOG(INFO, TAG, "Address is EDR");
        memcpy(addrInfo, addr + ADDRESS_PREFIX_LEN, CA_MACADDR_SIZE - 1);
        OIC_LOG_V(INFO, TAG, "EDR Address is %s", addrInfo);
        return CA_ADAPTER_RFCOMM_BTEDR;
    }
    else if (0 == strcmp(addrPrefix, BLE_ADDRESS_PREFIX))
    {
        OIC_LOG(INFO, TAG, "Address is BLE");
        memcpy(addrInfo, addr + ADDRESS_PREFIX_LEN, CA_MACADDR_SIZE - 1);
        OIC_LOG_V(INFO, TAG, "BLE Address is %s", addrInfo);
        return CA_ADAPTER_GATT_BTLE;
    }

    OIC_LOG(DEBUG, TAG, "OUT");
    return -1;
}
