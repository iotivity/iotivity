/* *****************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * *****************************************************************/
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200112L
#endif

#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "ocstack.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "logger.h"
#include "cJSON.h"
#include "utlist.h"
#include "ocpayload.h"

#include "securevirtualresourcetypes.h"
#include "srmresourcestrings.h" //@note: SRM's internal header
#include "doxmresource.h"       //@note: SRM's internal header
#include "pstatresource.h"      //@note: SRM's internal header

#include "pmtypes.h"
#include "pmutility.h"

#define TAG ("PM-UTILITY")

/**
 * Function to search node in linked list that matches given IP and port.
 *
 * @param[in] pList         List of OCProvisionDev_t.
 * @param[in] addr          address of target device.
 * @param[in] port          port of remote server.
 *
 * @return pointer of OCProvisionDev_t if exist, otherwise NULL
 */
OCProvisionDev_t* GetDevice(OCProvisionDev_t **ppDevicesList, const char* addr, const uint16_t port)
{
    if(NULL == addr || NULL == *ppDevicesList)
    {
        OC_LOG_V(ERROR, TAG, "Invalid Input parameters in [%s]\n", __FUNCTION__);
        return NULL;
    }

    OCProvisionDev_t *ptr = NULL;
    LL_FOREACH(*ppDevicesList, ptr)
    {
        if( strcmp(ptr->endpoint.addr, addr) == 0 && port == ptr->endpoint.port)
        {
            return ptr;
        }
    }

    return NULL;
}


/**
 * Add device information to list.
 *
 * @param[in] pList         List of OCProvisionDev_t.
 * @param[in] addr          address of target device.
 * @param[in] port          port of remote server.
 * @param[in] adapter       adapter type of endpoint.
 * @param[in] doxm          pointer to doxm instance.
 *
 * @return OC_STACK_OK for success and errorcode otherwise.
 */
OCStackResult AddDevice(OCProvisionDev_t **ppDevicesList, const char* addr, const uint16_t port,
                               OCTransportAdapter adapter, OicSecDoxm_t *doxm)
{
    if (NULL == addr)
    {
        return OC_STACK_INVALID_PARAM;
    }

    OCProvisionDev_t *ptr = GetDevice(ppDevicesList, addr, port);
    if(!ptr)
    {
        ptr = (OCProvisionDev_t *)OICCalloc(1, sizeof (OCProvisionDev_t));
        if (NULL == ptr)
        {
            OC_LOG(ERROR, TAG, "Error while allocating memory for linkedlist node !!");
            return OC_STACK_NO_MEMORY;
        }

        OICStrcpy(ptr->endpoint.addr, MAX_ADDR_STR_SIZE, addr);
        ptr->endpoint.port = port;
        ptr->doxm = doxm;
        ptr->securePort = DEFAULT_SECURE_PORT;
        ptr->endpoint.adapter = adapter;
        ptr->next = NULL;

        LL_PREPEND(*ppDevicesList, ptr);
    }

    return OC_STACK_OK;
}

/**
 * Function to set secure port information from the given list of devices.
 *
 * @param[in] pList         List of OCProvisionDev_t.
 * @param[in] addr          address of target device.
 * @param[in] port          port of remote server.
 * @param[in] secureport    secure port information.
 *
 * @return OC_STACK_OK for success and errorcode otherwise.
 */
OCStackResult UpdateSecurePortOfDevice(OCProvisionDev_t **ppDevicesList, const char *addr, uint16_t port,
                                        uint16_t securePort)
{
    OCProvisionDev_t *ptr = GetDevice(ppDevicesList, addr, port);

    if(!ptr)
    {
        OC_LOG(ERROR, TAG, "Can not find device information in the discovery device list");
        return OC_STACK_ERROR;
    }

    ptr->securePort = securePort;

    return OC_STACK_OK;
}

/**
 * This function deletes list of provision target devices
 *
 * @param[in] pList         List of OCProvisionDev_t.
 */
void DeleteDeviceList(OCProvisionDev_t **ppDevicesList)
{
    if(*ppDevicesList)
    {
        OCProvisionDev_t *del = NULL, *tmp = NULL;
        LL_FOREACH_SAFE(*ppDevicesList, del, tmp)
        {
            LL_DELETE(*ppDevicesList, del);

            DeleteDoxmBinData(del->doxm);
            DeletePstatBinData(del->pstat);
            OICFree(del);
        }
    }
}

/**
 * Timeout implementation for secure discovery. When performing secure discovery,
 * we should wait a certain period of time for getting response of each devices.
 *
 * @param[in]  waittime  Timeout in seconds.
 * @return OC_STACK_OK on success otherwise error.
 */
OCStackResult PMTimeout(unsigned short waittime)
{
    struct timespec startTime = {};
    struct timespec currTime  = {};

    OCStackResult res = OC_STACK_OK;
#ifdef _POSIX_MONOTONIC_CLOCK
    int clock_res = clock_gettime(CLOCK_MONOTONIC, &startTime);
#else
    int clock_res = clock_gettime(CLOCK_REALTIME, &startTime);
#endif
    if (0 != clock_res)
    {
        return OC_STACK_ERROR;
    }
    while (OC_STACK_OK == res)
    {
#ifdef _POSIX_MONOTONIC_CLOCK
        clock_res = clock_gettime(CLOCK_MONOTONIC, &currTime);
#else
        clock_res = clock_gettime(CLOCK_REALTIME, &currTime);
#endif
        if (0 != clock_res)
        {
            return OC_STACK_TIMEOUT;
        }
        long elapsed = (currTime.tv_sec - startTime.tv_sec);
        if (elapsed > waittime)
        {
            return OC_STACK_OK;
        }
        res = OCProcess();
    }

    return res;
}

/**
 * Extract secure port information from payload of discovery response.
 *
 * @param[in] jsonStr response payload of /oic/res discovery.
 *
 * @return Secure port
 */
uint16_t GetSecurePortFromJSON(char* jsonStr)
{
    // TODO: Modify error handling
    if (NULL == jsonStr)
    {
        return 0;
    }
    cJSON *jsonProp = NULL;
    cJSON *jsonP = NULL;
    cJSON *jsonPort = NULL;

    cJSON *jsonRoot = cJSON_Parse(jsonStr);
    if(!jsonRoot)
    {
        // TODO: Add error log & return default secure port
        return 0;
    }

    jsonProp = cJSON_GetObjectItem(jsonRoot, "prop");
    if(!jsonProp)
    {
        // TODO: Add error log & return default secure port
        return 0;
    }

    jsonP = cJSON_GetObjectItem(jsonProp, "p");
    if(!jsonP)
    {
        // TODO: Add error log & return default secure port
        return 0;
    }

    jsonPort = cJSON_GetObjectItem(jsonP, "port");
    if(!jsonPort)
    {
        // TODO: Add error log & return default secure port
        return 0;
    }

    return (uint16_t)jsonPort->valueint;
}


/**
 * Callback handler for getting secure port information using /oic/res discovery.
 *
 * @param[in] ctx             user context
 * @param[in] handle          Handle for response
 * @param[in] clientResponse  Response information(It will contain payload)
 *
 * @return OC_STACK_KEEP_TRANSACTION to keep transaction and
 *         OC_STACK_DELETE_TRANSACTION to delete it.
 */
static OCStackApplicationResult SecurePortDiscoveryHandler(void *ctx,
                                OCDoHandle handle, OCClientResponse *clientResponse)
{
    if (ctx == NULL)
    {
        OC_LOG(ERROR, TAG, "Lost List of device information");
        return OC_STACK_KEEP_TRANSACTION;
    }

    if (clientResponse)
    {
        if  (NULL == clientResponse->payload)
        {
            OC_LOG(INFO, TAG, "Skiping Null payload");
        }
        else
        {
            if (PAYLOAD_TYPE_DISCOVERY != clientResponse->payload->type)
            {
                OC_LOG(INFO, TAG, "Wrong payload type");
                return OC_STACK_KEEP_TRANSACTION;
            }

            OCDiscoveryPayload* discover = (OCDiscoveryPayload*) clientResponse->payload;
            uint16_t securePort = 0;

            if (discover && discover->resources && discover->resources->secure)
            {
                securePort = discover->resources->port;
            }
            else
            {
                OC_LOG(INFO, TAG, "Secure Port info is missing");
                return OC_STACK_KEEP_TRANSACTION;
            }

            OCProvisionDev_t** ppDevicesList = (OCProvisionDev_t**) ctx;

            OCStackResult res = UpdateSecurePortOfDevice(ppDevicesList, clientResponse->devAddr.addr,
                                                         clientResponse->devAddr.port, securePort);
            if (OC_STACK_OK != res)
            {
                OC_LOG(ERROR, TAG, "Error while getting secure port.");
                return OC_STACK_KEEP_TRANSACTION;
            }
            OC_LOG(INFO, TAG, "Exiting SecurePortDiscoveryHandler.");
        }

        return  OC_STACK_KEEP_TRANSACTION;
    }
    else
    {
        OC_LOG(INFO, TAG, "Skiping Null response");
    }
    return  OC_STACK_DELETE_TRANSACTION;
}

/**
 * Callback handler for PMDeviceDiscovery API.
 *
 * @param[in] ctx             User context
 * @param[in] handle          Handler for response
 * @param[in] clientResponse  Response information (It will contain payload)
 * @return OC_STACK_KEEP_TRANSACTION to keep transaction and
 *         OC_STACK_DELETE_TRANSACTION to delete it.
 */
static OCStackApplicationResult DeviceDiscoveryHandler(void *ctx,
                                OCDoHandle handle, OCClientResponse *clientResponse)
{
    if (ctx == NULL)
    {
        OC_LOG(ERROR, TAG, "Lost List of device information");
        return OC_STACK_KEEP_TRANSACTION;
    }
    if (clientResponse)
    {
        if  (NULL == clientResponse->payload)
        {
            OC_LOG(INFO, TAG, "Skiping Null payload");
            return OC_STACK_KEEP_TRANSACTION;
        }
        if (OC_STACK_OK != clientResponse->result)
        {
            OC_LOG(INFO, TAG, "Error in response");
            return OC_STACK_KEEP_TRANSACTION;
        }
        else
        {
            if (PAYLOAD_TYPE_SECURITY != clientResponse->payload->type)
            {
                OC_LOG(INFO, TAG, "Unknown payload type");
                return OC_STACK_KEEP_TRANSACTION;
            }
            OicSecDoxm_t *ptrDoxm = JSONToDoxmBin(
                            ((OCSecurityPayload*)clientResponse->payload)->securityData);
            if (NULL == ptrDoxm)
            {
                OC_LOG(INFO, TAG, "Ignoring malformed JSON");
                return OC_STACK_KEEP_TRANSACTION;
            }
            else
            {
                OC_LOG(DEBUG, TAG, "Successfully converted doxm json to bin.");

                OCProvisionDev_t **ppDevicesList = (OCProvisionDev_t**) ctx;

                OCStackResult res = AddDevice(ppDevicesList, clientResponse->devAddr.addr,
                        clientResponse->devAddr.port,
                        clientResponse->devAddr.adapter, ptrDoxm);
                if (OC_STACK_OK != res)
                {
                    OC_LOG(ERROR, TAG, "Error while adding data to linkedlist.");
                    DeleteDoxmBinData(ptrDoxm);
                    return OC_STACK_KEEP_TRANSACTION;
                }

                //Try to the unicast discovery to getting secure port
                char query[MAX_QUERY_LENGTH] = { 0, };
                sprintf(query, "%s%s:%d%s",
                        COAP_PREFIX,
                        clientResponse->devAddr.addr, clientResponse->devAddr.port,
                        OC_RSRVD_WELL_KNOWN_URI);

                OCCallbackData cbData;
                cbData.cb = &SecurePortDiscoveryHandler;
                cbData.context = ctx;
                cbData.cd = NULL;
                OCStackResult ret = OCDoResource(NULL, OC_REST_GET, query, 0, 0,
                        CT_ADAPTER_IP, OC_LOW_QOS, &cbData, NULL, 0);
                // TODO: Should we use the default secure port in case of error?
                if(OC_STACK_OK != ret)
                {
                    UpdateSecurePortOfDevice(ppDevicesList, clientResponse->devAddr.addr,
                            clientResponse->devAddr.port, DEFAULT_SECURE_PORT);
                }
                else
                {
                    OC_LOG_V(ERROR, TAG, "OCDoResource with [%s] Success", query);
                }
                OC_LOG(INFO, TAG, "Exiting ProvisionDiscoveryHandler.");
            }

            return  OC_STACK_KEEP_TRANSACTION;
        }
    }
    else
    {
        OC_LOG(INFO, TAG, "Skiping Null response");
        return OC_STACK_KEEP_TRANSACTION;
    }

    return  OC_STACK_DELETE_TRANSACTION;
}

/**
 * Discover owned/unowned devices in the same IP subnet. .
 *
 * @param[in] waittime      Timeout in seconds.
 * @param[in] isOwned       bool flag for owned / unowned discovery
 * @param[in] ppDevicesList        List of OCProvisionDev_t.
 *
 * @return OC_STACK_OK on success otherwise error.
 */
OCStackResult PMDeviceDiscovery(unsigned short waittime, bool isOwned, OCProvisionDev_t **ppDevicesList)
{
    OC_LOG(DEBUG, TAG, "IN PMDeviceDiscovery");

    if (NULL != *ppDevicesList)
    {
        OC_LOG(ERROR, TAG, "List is not null can cause memory leak");
        return OC_STACK_INVALID_PARAM;
    }

    const char DOXM_OWNED_FALSE_MULTICAST_QUERY[] = "/oic/sec/doxm?Owned=FALSE";
    const char DOXM_OWNED_TRUE_MULTICAST_QUERY[] = "/oic/sec/doxm?Owned=TRUE";

    OCCallbackData cbData;
    cbData.cb = &DeviceDiscoveryHandler;
    cbData.context = (void *)ppDevicesList;
    cbData.cd = NULL;
    OCStackResult res = OC_STACK_ERROR;

    const char* query = isOwned ? DOXM_OWNED_TRUE_MULTICAST_QUERY :
                                  DOXM_OWNED_FALSE_MULTICAST_QUERY;

    res = OCDoResource(NULL, OC_REST_DISCOVER, query, 0, 0,
                                     CT_DEFAULT, OC_LOW_QOS, &cbData, NULL, 0);
    if (res != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, "OCStack resource error");
        goto exit;
    }

    //Waiting for each response.
    res = PMTimeout(waittime);
    if(OC_STACK_OK != res)
    {
        OC_LOG(ERROR, TAG, "Failed to wait response for secure discovery.");
        goto exit;
    }

    OC_LOG(DEBUG, TAG, "OUT PMDeviceDiscovery");
exit:
    return res;
}

