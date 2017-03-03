//******************************************************************
//
// Copyright 2017 Intel Mobile Communications GmbH All Rights Reserved.
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
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//


#include <stdint.h>
#include "pluginServer.h"
#include "hue_file.h"
#include <pthread.h>

#ifndef __HUE_AUTH_SPEC_H__
#define __HUE_AUTH_SPEC_H__

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Each bridge discovered is put into a record which is described below
 */
typedef struct HueDiscoveredTag
{
    // The client IDs storage will eventually be in a list when we actually figure
    // out how client applications need to work.  Until we have that data then there
    // is a client array where index 0 holds the primary client.
    uint32_t numClients;
    char clientIDs[MAX_CLIENTS * MAX_STRING];
    char macAddrString[MAX_STRING];
    char ipAddrString[MAX_STRING];
} HueDiscoveredCtx;


/**
 * Callback registered by the HUE plugin back end that allows the plugin specific
 * authorization code to tell the back end that a new user on a HUE bridge can use the bridge.
 *
 * @param[in] *macAddrString In the HUE case this identifier is a 6 byte
 *                            MAX address of the HUE bridge
 * @param[in] *clientId      This is a simple string representing the client application that
 *                            now has permission to use the HUE bridge.
 */
typedef void (*addAuthBridgeCallback)(const char *macAddrString, const char *ClientId);


/**
 * Callback registered by the plugin back end which allow the plugin specific
 * authorization code to remove or revoke a bridge user that tells the back end
 * that a client can no longer use the bridge.
 *
 * @param[in] *macAddrString In the HUE case this identifier is a 6 byte
 *                           MAX address of the HUE bridge
 */
typedef void (*RemoveAuthBridgeCallback)(const char *macAddrString);


/**
 * The intention of this function is to register the callbacks and the
 * context pointer of the plugin. This function must
 * only be called once at plugin startup time.
 *
 * @param[in] ctx  This is the "back end" context which is considered opaque by the Hue
 *             authorization implementation code, it has been provided as a convenience
 *             for the "back end" implementation.
 * @param[in] FoundBridgeCb  Plugin provides a callback for the authorization implementation for the
 *                        case where a new user on a bridge is found
 * @param[in] RemoveBridgeCb Plugin provides a callback for the authorization implementation in the
 *                       case where a a bridge has been deauthorized
 * @return MPM_RESULT_OK on Success else MPM_RESULT_CREATED_FAILED.
 */
MPMResult hueInit(MPMPluginCtx *ctx, addAuthBridgeCallback foundBridgeCb,
                  RemoveAuthBridgeCallback removeBridgeCb);


/**
 * Request a discovery of the HUE bridges. Needs to be called at least once, it does
 * not hurt to call it many times.  Preferably call it the number of times needed.
 * This is an asyncronous call, the return value tells you if the request got out
 * and that is all.
 *
 * @return MPM_RESULT_OK on Success else MPM_RESULT_INTERNAL_ERROR.
 */
MPMResult DiscoverHueBridges();

/**
 * Helper function to obtain the HTTP prefix string needed for libcurl puts and gets
 * but more importantly this function tells the plugin if the application and hub are
 * authorized.
 *
 * The macAddr is used to determine which hub  and the clientId is checked to see if
 * that client application has been authorized to access the bridge.
 *
 * @param[in,out]  *prefix        memory where the prefix is written if bridge authorized
 * @params[in,out] *prefixSize    size of the prefix buffer
 * @params[in]     *macAddrString mac address string of hue bridge
 * @params[in]     *clientId      supplied by IoTivity ultimately, another name for this
 *                                parameter in Iotivity parlance is stack id.
 * @return on success MPM_RESULT_OK else one of the error codes
 *
 */
MPMResult hueAuthGetHttpPrefix(char *prefix, uint32_t *prefixSize, const char *macAddrString,
                               const char *clientId);


/**
 * This function does the opposite of creating the Hue Authorization module
 * it is the last thing called on the Hue Authorization module.
 *
 * @returns MPM_RESULT_OK on success else on of the error codes
 */
MPMResult hueAuthDestroy();

/**
 * This functions adds the discovered bridge to the global vector for internal usage
 * @param[in] discoveredCtx HueDiscoveredCtx structure
 * @return MPM_RESULT_OK on success else one of the error codes
 */
MPMResult addDiscoveredBridge(HueDiscoveredCtx discoveredCtx);

/**
 * find the bridge from the list based on its MAC
 *
 * @param [in]      macAddString   A pointer to an string containing the mac address
 * @param [in,out]  discoveredCtx  A pointer to empty memory that will be a target for a copy of
 *                  the found context. Only copies if the context has been found.
 *
 * @return true -  bridge context found else false
 */
bool findDiscoveredBridge(const char *macAddrString, HueDiscoveredCtx *discoveredCtx);

/*
 * This functions Updated on of the data of the discovered bridge
 * to the global vector for internal usage.
 *
 * @param[in] macAddrString bridge MAC address
 * @param[in] clientId Hue Bridge client id
 *
 * @return True on success else False
 */
bool updateDiscoverBridgeDetails(const char *macAddrString, const char *clientID);


#ifdef __cplusplus
}
#endif

#endif /* __HUE_AUTH_SPEC_H__ */
