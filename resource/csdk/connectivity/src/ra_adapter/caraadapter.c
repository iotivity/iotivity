/******************************************************************
//
// Copyright 2015 Intel Mobile Communications GmbH All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
******************************************************************/

#include "caraadapter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "caadapterutils.h"
#include "camutex.h"
#include "uarraylist.h"
#include "logger.h"
#include "oic_malloc.h"
#include "ra_xmpp.h"

/**
 * @def RA_ADAPTER_TAG
 * @brief Logging tag for module name
 */
#define RA_ADAPTER_TAG "RA_ADAP"

/**
 * @var g_networkPacketCallback
 * @brief Network Packet Received Callback to CA
 */
static CANetworkPacketReceivedCallback g_networkPacketCallback = NULL;

/**
 * @var g_networkChangeCallback
 * @brief Network Changed Callback to CA
 */
static CANetworkChangeCallback g_networkChangeCallback = NULL;

/**
 * @var CARAXmppData
 * @brief Holds XMPP data information.
 */
typedef struct
{
    xmpp_context_t context;
    xmpp_handle_t handle;
    xmpp_connection_callback_t connection_callback;
    xmpp_connection_handle_t connection_handle;
    xmpp_message_context_t message_context;
    xmpp_message_callback_t message_callback;
    CANetworkStatus_t connection_status;
} CARAXmppData;

static CARAXmppData *g_xmppData = NULL;

static void CARANotifyNetworkChange(const char *address, CANetworkStatus_t status);

static void CARAXmppConnectedCB(void * const param, xmpp_error_code_t result,
        const char *const bound_jid,
        xmpp_connection_handle_t connection);

static void CARAXmppDisonnectedCB(void * const param, xmpp_error_code_t result,
        xmpp_connection_handle_t connection);

static void CARAXmppMessageSentCB(void * const param, xmpp_error_code_t result,
        const void *const recipient, const void *const msg, size_t messageOctets);

static void CARAXmppMessageReceivedCB(void * const param, xmpp_error_code_t result,
        const void *const sender, const void *const msg, size_t messageOctets);

void CARANotifyNetworkChange(const char *address, CANetworkStatus_t status)
{
    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CARANotifyNetworkChange IN");

    CALocalConnectivity_t *localEndpoint = CAAdapterCreateLocalEndpoint(CA_RA, address);
    if (!localEndpoint)
    {
        OIC_LOG(ERROR, RA_ADAPTER_TAG, "localEndpoint creation failed!");
        return;
    }

    if (g_networkChangeCallback)
    {
        g_networkChangeCallback(localEndpoint, status);
    }
    else
    {
        OIC_LOG(ERROR, RA_ADAPTER_TAG, "g_networkChangeCallback is NULL");
    }

    CAAdapterFreeLocalEndpoint(localEndpoint);

    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CARANotifyNetworkChange OUT");
}

void CARAXmppConnectedCB(void * const param, xmpp_error_code_t result,
        const char *const bound_jid,
        xmpp_connection_handle_t connection)
{
    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CARAXmppConnectedCB IN");

    if (XMPP_ERR_OK == result)
    {
        g_xmppData->connection_status = CA_INTERFACE_UP;
        g_xmppData->connection_handle = connection;
        g_xmppData->message_callback.on_received = CARAXmppMessageReceivedCB;
        g_xmppData->message_callback.on_sent = CARAXmppMessageSentCB;
        g_xmppData->message_context = xmpp_message_context_create(g_xmppData->connection_handle,
                g_xmppData->message_callback);
    }
    else
    {
        g_xmppData->connection_status = CA_INTERFACE_DOWN;
        OIC_LOG_V(ERROR, RA_ADAPTER_TAG, "XMPP connected callback status: %d", result);
        // TODO: what?
    }
    // Notify network change to CA
    //TODO: update jabber id to a valid one
    CARANotifyNetworkChange("some jabber ID", g_xmppData->connection_status);

    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CARAXmppConnectedCB OUT");
}

void CARAXmppDisonnectedCB(void * const param, xmpp_error_code_t result,
        xmpp_connection_handle_t connection)
{
    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CARAXmppDisonnectedCB IN");

    g_xmppData->connection_status = CA_INTERFACE_DOWN;
    xmpp_message_context_destroy(g_xmppData->message_context);

    // TODO: Stop what else?

    // Notify network change to CA
    //TODO: update jabber id to a valid one
    CARANotifyNetworkChange("some jabber ID", g_xmppData->connection_status);

    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CARAXmppDisonnectedCB OUT");
}

void CARAXmppMessageSentCB(void * const param, xmpp_error_code_t result,
        const void *const recipient, const void *const msg, size_t messageOctets)
{
    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CARAXmppMessageSentCB IN");
    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CARAXmppMessageSentCB OUT");
}

void CARAXmppMessageReceivedCB(void * const param, xmpp_error_code_t result,
        const void *const sender, const void *const msg, size_t messageOctets)
{
    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CARAXmppMessageReceivedCB IN");

    if (g_networkPacketCallback)
    {
        VERIFY_NON_NULL_VOID(sender, RA_ADAPTER_TAG, "sender is NULL");

        VERIFY_NON_NULL_VOID(msg, RA_ADAPTER_TAG, "message is NULL");

        // CA is freeing this memory
        // TODO: determine if implicit conversion to char* is appropriate here
        CARemoteEndpoint_t *endPoint = CAAdapterCreateRemoteEndpoint(CA_RA, sender, NULL);
        if (!endPoint)
        {
            OIC_LOG(ERROR, RA_ADAPTER_TAG, "EndPoint creation failed!");
            return;
        }

        void *buf = OICCalloc(messageOctets + 1, sizeof(char));
        if (!buf)
        {
            OIC_LOG(ERROR, RA_ADAPTER_TAG, "Memory Allocation failed!");
            CAAdapterFreeRemoteEndpoint(endPoint);
            return;
        }
        memcpy(buf, msg, messageOctets);
        if (g_networkPacketCallback)
        {
            g_networkPacketCallback(endPoint, buf, messageOctets);
        }
        else
        {
            OICFree(buf);
            CAAdapterFreeRemoteEndpoint(endPoint);
        }
    }

    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CARAXmppMessageReceivedCB OUT");
}

CAResult_t CAInitializeRA(CARegisterConnectivityCallback registerCallback,
                                CANetworkPacketReceivedCallback networkPacketCallback,
                                CANetworkChangeCallback netCallback, ca_thread_pool_t handle)
{
    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CAInitializeRA IN");
    VERIFY_NON_NULL(registerCallback, RA_ADAPTER_TAG, "registerCallback");
    VERIFY_NON_NULL(networkPacketCallback, RA_ADAPTER_TAG, "networkPacketCallback");
    VERIFY_NON_NULL(netCallback, RA_ADAPTER_TAG, "netCallback");
    VERIFY_NON_NULL(handle, RA_ADAPTER_TAG, "thread pool handle");

    g_networkChangeCallback = netCallback;
    g_networkPacketCallback = networkPacketCallback;

    CAConnectivityHandler_t raHandler = {};
    raHandler.startAdapter = CAStartRA;
    raHandler.startListenServer = CAStartRAListeningServer;
    raHandler.startDiscoveryServer = CAStartRADiscoveryServer;
    raHandler.sendData = CASendRAUnicastData;
    raHandler.sendDataToAll = CASendRAMulticastData;
    raHandler.GetnetInfo = CAGetRAInterfaceInformation;
    raHandler.readData = CAReadRAData;
    raHandler.stopAdapter = CAStopRA;
    raHandler.terminate = CATerminateRA;
    registerCallback(raHandler, CA_RA);

    OIC_LOG(INFO, RA_ADAPTER_TAG, "CAInitializeRA OUT");
    return CA_STATUS_OK;
}

void CATerminateRA()
{
    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CATerminateRA IN");

    CAStopRA();

    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CATerminateRA OUT");
}

CAResult_t CAStartRA()
{
    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CAStartRA IN");

    if (!g_xmppData)
    {
        g_xmppData = (CARAXmppData *) OICCalloc(1, sizeof(*g_xmppData));
        if (!g_xmppData)
        {
            OIC_LOG(ERROR, RA_ADAPTER_TAG, "g_xmppData Calloc  failed");
            return CA_MEMORY_ALLOC_FAILED;
        }
        g_xmppData->handle.abstract_handle = NULL;
    }

    if (g_xmppData->handle.abstract_handle)
    {
        OIC_LOG(WARNING, RA_ADAPTER_TAG, "RA adapter has already been started");
        return CA_STATUS_OK;
    }

    // Initialize context and handle structures
    xmpp_context_init(&g_xmppData->context);
    g_xmppData->handle = xmpp_startup(&g_xmppData->context);

    // Initialize host and identity structures
    xmpp_host_t host;
    xmpp_identity_t identity;
    xmpp_host_init(&host, XMPP_SERVER_HOST, XMPP_SERVER_PORT, XMPP_SERVER_DOMAIN, XMPP_PROTOCOL_XMPP);
    xmpp_identity_init(&identity, "user", "password", "jabberId", XMPP_NO_IN_BAND_REGISTER);

    // Wire up connection callbacks and call API to connect to XMPP server
    g_xmppData->connection_callback.on_connected = CARAXmppConnectedCB;
    g_xmppData->connection_callback.on_disconnected = CARAXmppDisonnectedCB;
    xmpp_error_code_t ret = xmpp_connect(g_xmppData->handle, &host,
            &identity, g_xmppData->connection_callback);

    // Destroy host and identity structures as they are only required to establish initial connection
    xmpp_identity_destroy(&identity);
    xmpp_host_destroy(&host);

    if (XMPP_ERR_OK != ret)
    {
        OIC_LOG_V(ERROR, RA_ADAPTER_TAG, "Failed to initiate XMPP connection, status: %d", ret);
        // TODO: do we need to call CAStopRA in this case or will CA call it?
        return CA_STATUS_FAILED;
    }

    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CAStartRA OUT");
    return CA_STATUS_OK;
}

CAResult_t CAStopRA()
{
    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CAStopRA IN");

    if (g_xmppData)
    {
        xmpp_error_code_t ret = xmpp_close(g_xmppData->connection_handle);
        if (XMPP_ERR_OK != ret)
        {
            OIC_LOG_V(ERROR, RA_ADAPTER_TAG, "Failed to close XMPP connection, status: %d", ret);
        }

        //TODO: xmpp_shutdown call blocks until the xmpp close is finished
        //TODO: Make sure CAStopRA can be blocking call
        // Shut down XMPP client
        xmpp_shutdown_xmpp(g_xmppData->handle);
        xmpp_context_destroy(&g_xmppData->context);


        OICFree(g_xmppData);
        g_xmppData = NULL;
    }
    else
    {
        OIC_LOG(WARNING, RA_ADAPTER_TAG, "RA adapter already stopped");
    }

    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CAStopRA OUT");
    return CA_STATUS_OK;
}

int32_t CASendRAUnicastData(const CARemoteEndpoint_t *remoteEndpoint, const void *data,
                                  uint32_t dataLength)
{
    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CASendRAUnicastData IN");

    VERIFY_NON_NULL_RET(remoteEndpoint, RA_ADAPTER_TAG, "remote address is NULL", 0);
    VERIFY_NON_NULL_RET(data, RA_ADAPTER_TAG, "data is NULL", 0);

    if (0 == dataLength)
    {
        OIC_LOG(ERROR, RA_ADAPTER_TAG, "Data length is 0!");
        return 0;
    }

    if (!g_xmppData || CA_INTERFACE_UP != g_xmppData->connection_status)
    {
        OIC_LOG(ERROR, RA_ADAPTER_TAG, "Unable to send XMPP message, RA not connected");
        return -1;
    }

    xmpp_error_code_t res = xmpp_send_message(g_xmppData->message_context,
            remoteEndpoint->addressInfo.RA.jabberID, data, dataLength,
            XMPP_MESSAGE_TRANSMIT_DEFAULT);
    if (XMPP_ERR_OK != res)
    {
        OIC_LOG_V(ERROR, RA_ADAPTER_TAG, "Unable to send XMPP message, status: %d", res);
        return -1;
    }

    OIC_LOG_V(INFO, RA_ADAPTER_TAG, "Successfully dispatched bytes[%d] to addr[%s]",
            dataLength, remoteEndpoint->addressInfo.RA.jabberID);

    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CASendRAUnicastData OUT");
    return dataLength;
}

CAResult_t CAGetRAInterfaceInformation(CALocalConnectivity_t **info, uint32_t *size)
{
    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CAGetRAInterfaceInformation IN");

    VERIFY_NON_NULL(info, RA_ADAPTER_TAG, "info is NULL");
    VERIFY_NON_NULL(size, RA_ADAPTER_TAG, "size is NULL");

    if (!g_xmppData || CA_INTERFACE_UP != g_xmppData->connection_status)
    {
        OIC_LOG(ERROR, RA_ADAPTER_TAG, "Failed to get interface info, RA not Connected");
        return CA_ADAPTER_NOT_ENABLED;
    }

    CALocalConnectivity_t *conInfo = (CALocalConnectivity_t *) OICCalloc(
            1, sizeof(CALocalConnectivity_t));
    if (!conInfo)
    {
        OIC_LOG(ERROR, RA_ADAPTER_TAG, "Malloc Failed");
        return CA_MEMORY_ALLOC_FAILED;
    }

    conInfo[0].type = CA_RA;
    conInfo[0].isSecured = true;
    // TODO: update with real jabber id when available
    strncpy(conInfo[0].addressInfo.RA.jabberID, "jabber id", 9);

    *size = 1;
    *info = conInfo;

    OIC_LOG(DEBUG, RA_ADAPTER_TAG, "CAGetRAInterfaceInformation OUT");
    return CA_STATUS_OK;
}

int32_t CASendRAMulticastData(const void *data, uint32_t dataLength)
{
    OIC_LOG(INFO, RA_ADAPTER_TAG, "RA adapter does not support sending multicast data");
    return 0;
}

CAResult_t CAStartRAListeningServer()
{
    OIC_LOG(INFO, RA_ADAPTER_TAG, "RA adapter does not support listening for multicast data");
    return CA_NOT_SUPPORTED;
}

CAResult_t CAStartRADiscoveryServer()
{
    OIC_LOG(INFO, RA_ADAPTER_TAG, "RA adapter does not support discovery of multicast servers");
    return CA_NOT_SUPPORTED;
}

CAResult_t CAReadRAData()
{
    OIC_LOG(INFO, RA_ADAPTER_TAG, "Read data is not implemented for the RA adapter");
    return CA_NOT_SUPPORTED;
}
