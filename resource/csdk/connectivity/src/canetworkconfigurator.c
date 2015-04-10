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

#include <stdlib.h>

#include "canetworkconfigurator.h"
#include "uarraylist.h"
#include "logger.h"
#include "cainterfacecontroller.h"

#define TAG PCF("CA")

static u_arraylist_t *g_selectedNetworkList = NULL;
static uint32_t NETWORK_ETHERNET = CA_ETHERNET;
static uint32_t NETWORK_WIFI = CA_WIFI;
static uint32_t NETWORK_EDR = CA_EDR;
static uint32_t NETWORK_LE = CA_LE;


CAResult_t CAAddNetworkType(CAConnectivityType_t connectivityType)
{
    if (g_selectedNetworkList == NULL)
    {
        OIC_LOG(DEBUG, TAG, "Create network list");

        g_selectedNetworkList = u_arraylist_create();

        if( g_selectedNetworkList == NULL )
        {
            return CA_MEMORY_ALLOC_FAILED;
        }
    }

    CAResult_t res = CA_STATUS_OK;
    switch (connectivityType)
    {
        case CA_ETHERNET:
        {

#ifndef ETHERNET_ADAPTER
            OIC_LOG(DEBUG, TAG, "Add network type(ETHERNET) - Not Supported");
            return CA_NOT_SUPPORTED;
#endif /* ETHERNET_ADAPTER */

            OIC_LOG(DEBUG, TAG, "Add network type(ETHERNET)");
            if (!u_arraylist_contains(g_selectedNetworkList, &NETWORK_ETHERNET))
            {
                res = u_arraylist_add(g_selectedNetworkList, &NETWORK_ETHERNET);
            }
        }
        break;

        case CA_WIFI:
        {

#ifndef WIFI_ADAPTER
            OIC_LOG(DEBUG, TAG, "Add network type(WIFI) - Not Supported");
            return CA_NOT_SUPPORTED;
#endif /* WIFI_ADAPTER */

            OIC_LOG(DEBUG, TAG, "Add network type(WIFI)");
            if (!u_arraylist_contains(g_selectedNetworkList, &NETWORK_WIFI))
            {
                res = u_arraylist_add(g_selectedNetworkList, &NETWORK_WIFI);
            }
        }
        break;

        case CA_EDR:
        {

#ifndef EDR_ADAPTER
            OIC_LOG(DEBUG, TAG, "Add network type(EDR) - Not Supported");
            return CA_NOT_SUPPORTED;
#endif /* EDR_ADAPTER */

            OIC_LOG(DEBUG, TAG, "Add network type(EDR)");
            if (!u_arraylist_contains(g_selectedNetworkList, &NETWORK_EDR))
            {
                res = u_arraylist_add(g_selectedNetworkList, &NETWORK_EDR);
            }
        }
        break;

        case CA_LE:
        {

#ifndef LE_ADAPTER
            OIC_LOG(DEBUG, TAG, "Add network type(LE) - Not Supported");
            return CA_NOT_SUPPORTED;
#endif /* LE_ADAPTER */

            OIC_LOG(DEBUG, TAG, "Add network type(LE)");
            if (!u_arraylist_contains(g_selectedNetworkList, &NETWORK_LE))
            {
                res = u_arraylist_add(g_selectedNetworkList, &NETWORK_LE);
            }
        }
        break;

    }

    if (CA_STATUS_OK != res)
    {
        OIC_LOG_V(ERROR, TAG, "Add arraylist failed[Err code: %d]", res);
        return res;
    }
    // start selected interface adapter
    res = CAStartAdapter((CAConnectivityType_t)connectivityType);

    return res;
}

CAResult_t CARemoveNetworkType(CAConnectivityType_t connectivityType)
{
    uint8_t index;
    CAConnectivityType_t connType;

    if (g_selectedNetworkList == NULL)
    {
        OIC_LOG(DEBUG, TAG, "SelectedNetwork list is NULL");

        return CA_STATUS_FAILED;
    }

    for (index = 0; index < u_arraylist_length(g_selectedNetworkList); index++)
    {
        void* ptrType = u_arraylist_get(g_selectedNetworkList, index);

        if(ptrType == NULL)
        {
            continue;
        }

        connType = *(CAConnectivityType_t *) ptrType;

        if (connectivityType == connType)
        {
            switch (connectivityType)
            {
                case CA_ETHERNET:

#ifndef ETHERNET_ADAPTER
                    OIC_LOG(DEBUG, TAG, "Remove network type(ETHERNET) - Not Supported");
                    return CA_NOT_SUPPORTED;
#else

                    OIC_LOG(DEBUG, TAG, "Remove network type(ETHERNET)");
                    u_arraylist_remove(g_selectedNetworkList, index);
#endif /* ETHERNET_ADAPTER */
                    break;

                case CA_WIFI:

#ifndef WIFI_ADAPTER
                    OIC_LOG(DEBUG, TAG, "Remove network type(WIFI) - Not Supported");
                    return CA_NOT_SUPPORTED;
#else
                    OIC_LOG(DEBUG, TAG, "Remove network type(WIFI)");
                    u_arraylist_remove(g_selectedNetworkList, index);
#endif /* WIFI_ADAPTER */

                    break;

                case CA_EDR:

#ifndef EDR_ADAPTER
                    OIC_LOG(DEBUG, TAG, "Remove network type(EDR) - Not Supported");
                    return CA_NOT_SUPPORTED;
#else
                    OIC_LOG(DEBUG, TAG, "Remove network type(EDR)");
                    u_arraylist_remove(g_selectedNetworkList, index);
#endif /* EDR_ADAPTER */

                    break;

                case CA_LE:

#ifndef LE_ADAPTER
                    OIC_LOG(DEBUG, TAG, "Remove network type(LE) - Not Supported");
                    return CA_NOT_SUPPORTED;
#else
                    OIC_LOG(DEBUG, TAG, "Remove network type(LE)");
                    u_arraylist_remove(g_selectedNetworkList, index);
#endif /* LE_ADAPTER */

                    break;
            }

            // stop selected interface adapter
            CAStopAdapter(connType);
            return CA_STATUS_OK;
        }
    }

    return CA_STATUS_FAILED;
}

u_arraylist_t *CAGetSelectedNetworkList()
{
    OIC_LOG(DEBUG, TAG, "CAGetSelectedNetworkList IN");
    if (g_selectedNetworkList == NULL)
    {
        OIC_LOG(DEBUG, TAG, "SelectedNetwork list is NULL");
        return NULL;
    }
    OIC_LOG(DEBUG, TAG, "CAGetSelectedNetworkList OUT");
    return g_selectedNetworkList;
}

CAResult_t CAGetNetworkInformationInternal(CALocalConnectivity_t **info, uint32_t *size)
{
    OIC_LOG(DEBUG, TAG, "get network information.");

    if (info == NULL || size == NULL)
    {
        OIC_LOG(DEBUG, TAG, "Input parameter is invalid value");

        return CA_STATUS_INVALID_PARAM;
    }

    return CAGetNetworkInfo(info, size);
}

CAResult_t CATerminateNetworkType()
{
    OIC_LOG(DEBUG, TAG, "CATerminateNetworkType()");

    if(g_selectedNetworkList != NULL)
    {
        u_arraylist_free(&g_selectedNetworkList);
    }

    return CA_STATUS_OK;
}

