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
#include "iotivity_config.h"

#include <mutex>
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <regex.h>
#include "oic_string.h"
#include "mpmErrorCode.h"
#include "hue_auth_spec.h"
#include "hue_file.h"
#include "hue_defs.h"
#include "curlClient.h"
#include "cJSON.h"
#include "experimental/logger.h"

#define TAG "HUE_AUTH"

using namespace OC::Bridging;

/**
 * This is the global context of the Hue specific authorization module.  There
 * can only be one instance of this in a process boundary and that is the
 * case for a decoupled plugin.  So a global context is the best solution. It
 * is the only global structure required for the implementation of the Hue
 * specific authorization module.
 */
typedef struct HueAuthCtxTag
{
    bool structInitialized;

    /** client called the created method */
    bool created;

    /** store for the client as it is opaque for the authorization module */
    MPMPluginCtx *pluginCtx;

    /** call back functions on the client */
    addAuthBridgeCallback addAuthBridge;
    RemoveAuthBridgeCallback removeAuthBridge;

    pthread_mutex_t discoveredLock;
} HueAuthCtx;

/**
 *  NOTE: There is an assumption that global data structures are initialized to 0
 *       when they are loaded into memory.  If this is not the case then we are
 *       going have to figure out how to make this behavior happen.  The code
 *       depends on this structure being set to zero at the beginning of time.
 */
HueAuthCtx g_hueAuthCtx;

/**
 * Global variable for storing discovered bridges and
 * and mutex lock for it.
 */
std::mutex g_discoveredBridgesLock;
std::vector<HueDiscoveredCtx> g_discoveredBridges;

/**
 * Function used to initialize the global context.  It depends on the fact
 * that global variables initialize to all 0s.
 */
static void initializeHueAuthCtx();

static MPMResult parseHueBridgeDiscovery(std::string response);

/**
 * Helper function to parse one bridge out of the discovery response
 * @param [in] macAddrString  MAC address of the bridge
 * @param [in] ipAddrString   IP address of the bridge
 *
 * @return MPM_RESULT_OK on success else one of the error codes.
 */
static MPMResult parseOneHueBridge(char *macAddrString, char *ipAddrString);

/**
 * Based on a single context structure find a client in the clientID list.
 *
 * @param [in] discoveredCtx   Bridge context
 * @param [in] clientId        Client Id of the bridge
 *
 * @return: true  - clientId exists else false
 */
static bool discoveredFindClientID(HueDiscoveredCtx *discoveredCtx, const char *clientID);


MPMResult hueInit(MPMPluginCtx *ctx, addAuthBridgeCallback addBridgeCb,
                  RemoveAuthBridgeCallback removeBridgeCb)
{
    OIC_LOG(INFO, TAG, " Initializing hue contexts ");
    initializeHueAuthCtx();

    if ((g_hueAuthCtx.created == false) && (g_hueAuthCtx.structInitialized == true))
    {
        g_hueAuthCtx.addAuthBridge = addBridgeCb;
        g_hueAuthCtx.removeAuthBridge = removeBridgeCb;
        g_hueAuthCtx.pluginCtx = ctx;
        g_hueAuthCtx.created = true;
    }
    readAuthorizedBridgeFile();
    return MPM_RESULT_OK;
}

/**
 * The intention is to send a request to the Hue bridges on the network to identify
 * them. This function blindly makes a request for the bridges on a network.  The
 * doGetReqest is a synchronous function call; so it may take a while.  When
 * the doGetRequest returns it has a discovery payload which is then parsed
 * by the parseHueBridgeDiscovery function.  It is in the parsing of each bridge
 * found where you will see the callbacks to either the authorize module.
 *
 * @return MPM_RESULT_OK on success else MPM_RESULT_INTERNAL_ERROR
 */
MPMResult DiscoverRemoteBridges()
{
    MPMResult result = MPM_RESULT_INTERNAL_ERROR;

    CurlClient cc = CurlClient(CurlClient::CurlMethod::GET, BRIDGE_NUPNP_URI)
                    .addRequestHeader(CURL_HEADER_ACCEPT_JSON);
    int curlCode = cc.send();
    if (curlCode != MPM_RESULT_OK)
    {
        OIC_LOG_V(ERROR, TAG, " Error discovering remote HUE Bridges - %d ", curlCode);
        return MPM_RESULT_INTERNAL_ERROR;
    }
    else
    {
        std::string response = cc.getResponseBody();
        result = parseHueBridgeDiscovery(response);
    }

    return (result);
}

#define UPNP_REQUEST "M-SEARCH * HTTP/1.1\r\n" \
              "HOST: 239.255.255.250:1900\r\n" \
              "MAN: \"ssdp:discover\"\r\n" \
              "MX: 1000\r\n" \
              "ST: libhue:idl\r\n" \
              "\r\n"

/**
 * This finds all Hue Bridges in the local network by sending a multicast
 * message. No Internet connection is needed.
 *
 * The Hue Bridges are using Simple Service Discovery Protocol (SSDP) and
 * joined the multicast group 239.255.255.250. When I send a SSDP package to
 * this multicast group all Hue bridges will answer to me with a unicast
 * message containing their ID (mac address) and I get the IP address from the
 * normal IP header. Be aware that at least some Hues are answering more than
 * one time, in my testes I got 3 different answer packages, probably to work
 * with different (broken) implementations and these were send two times, each
 * with a little delay, to prevent package loses, combined I got 6 answers from
 * one Hue bridge to one request. Instead of using a library to create and
 * parse the SSDP packages, do it manually because we only want to support Hue
 * bridges and this is a simple protocol, when more features are needed using
 * a real lib would be better.
 *
 * The answer from a Hue bridge with a firmware from October 2015 looks like this:
 *
 * HTTP/1.1 200 OK
 * HOST: 239.255.255.250:1900
 * EXT:
 * CACHE-CONTROL: max-age=100
 * LOCATION: http://192.168.178.241:80/description.xml
 * SERVER: FreeRTOS/7.4.2 UPnP/1.0 IpBridge/1.10.0
 * hue-bridgeid: 001788FFFE09A206
 * ST: upnp:rootdevice
 * USN: uuid:2f402f80-da50-11e1-9b23-00178809a206::upnp:rootdevice
 *
 * source: http://www.developers.meethue.com/documentation/changes-bridge-discovery
 *
 * @return MPM_RESULT_OK on success else MPM_RESULT_INTERNAL_ERROR
 */
MPMResult DiscoverLocalBridges()
{
    int sock;
    int err;
    ssize_t len;
    char recv_buf[500];
    struct sockaddr_in recv_addr;
    socklen_t recv_addr_len;
    struct sockaddr_in address;
    fd_set rfds;
    struct timeval tv;
    regex_t preg;
    regmatch_t pmatch_id[2];
    char err_buff[40];
    char macAddrString[17];
    int i;
    unsigned int j;
    MPMResult result = MPM_RESULT_INTERNAL_ERROR;
    memset(&address, 0, sizeof(sockaddr_in));

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
    {
        perror("socket()");
        OIC_LOG(ERROR, TAG, "socket error");
        return MPM_RESULT_INTERNAL_ERROR;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("239.255.255.250");
    address.sin_port = htons(1900);

    // send the request to the multicast group  239.255.255.250:1900 the
    // Hue Bridges are listening to.
    len = sendto(sock, UPNP_REQUEST, sizeof(UPNP_REQUEST), 0,
                 (struct sockaddr *) &address, sizeof(address));
    if (len == -1)
    {
        perror("sendto()");
        OIC_LOG(ERROR, TAG, "sendto() error");
        goto err_close;
    }

    // Compile a regular expression to search in the answer message for the
    // Hue Bridge ID. The answer is send in a text based protocol, one
    // line is containing an uuid with the mac address at the end. This line
    // looks like this: "USN: uuid:00112233-4455-6677-8899-aabbccddeeff"
    err = regcomp(&preg,
                  "uuid:[0-9a-fA-F]*-[0-9a-fA-F]*-[0-9a-fA-F]*-[0-9a-fA-F]*-([0-9a-fA-F]*)",
                  REG_EXTENDED);
    if (err)
    {
        regerror(err, &preg, err_buff, sizeof(err_buff));
        OIC_LOG_V(ERROR, TAG, "problem in regcomp(): %s\n", err_buff);
        goto err_close;
    }

    FD_ZERO(&rfds);
    FD_SET(sock, &rfds);

    tv.tv_sec = 3;
    tv.tv_usec = 0;

    while (1)
    {
        err = select(sock + 1, &rfds, NULL, NULL, &tv);
        if (err == -1)
        {
            OIC_LOG(ERROR, TAG, "error in select()");
            goto err_regfree;
        }
        if (!FD_ISSET(sock, &rfds))
        {
            break;
        }

        // receive the answer package to the multicast request and also
        // get the address from where we received the package because
        // we also need the IP-address of the Hue bridge.
        recv_addr_len = sizeof(recv_addr);
        memset(&recv_buf, 0x0, sizeof(recv_buf));
        len = recvfrom(sock, recv_buf, sizeof(recv_buf), 0,
                       (struct sockaddr *) &recv_addr, &recv_addr_len);
        if (len == -1)
        {
            OIC_LOG(ERROR, TAG, "error in recvfrom()");
            continue;
        }
        // use the compiled regular expression to get the Hue Bridge
        // ID, the first match is the complete line and the second
        // match is the mac address we are searching for. We store the
        // mac address in lower case, so we have to go through it char
        // by char. After it is parsed add to our internal database.
        memset(pmatch_id, 0x0, sizeof(pmatch_id));
        err = regexec(&preg, recv_buf, 2, pmatch_id, 0);
        if (err)
        {
            regerror(err, &preg, err_buff, sizeof(err_buff));
            OIC_LOG_V(ERROR, TAG, "error in regexec() err_buff=%s", err_buff);
            continue;
        }
        if (pmatch_id[1].rm_so == -1 || pmatch_id[1].rm_eo == -1)
        {
            continue;
        }

        for (i = pmatch_id[1].rm_so, j = 0;
             i < pmatch_id[1].rm_eo && j < sizeof(macAddrString);
             i++, j++)
        {
            // convert the MAC address into a EUI-64 like it is
            // done in IPv6.
            if (j == 6)
            {
                macAddrString[j++] = 'f';
                macAddrString[j++] = 'f';
                macAddrString[j++] = 'f';
                macAddrString[j++] = 'e';
            }
            macAddrString[j] = tolower(recv_buf[i]);
        }
        macAddrString[16] = '\0';
        result = parseOneHueBridge(macAddrString, inet_ntoa(recv_addr.sin_addr));
    }
err_regfree:
    regfree(&preg);
err_close:
    close(sock);

    return (result);
}

/** discovers both local and remote hue bridges */
MPMResult DiscoverHueBridges()
{
    MPMResult result = MPM_RESULT_INTERNAL_ERROR;

    MPMResult resultRemote = DiscoverRemoteBridges();
    MPMResult resultLocal = DiscoverLocalBridges();

    //if either discovery was OK, then return result OK.
    if (resultRemote == MPM_RESULT_OK || resultLocal == MPM_RESULT_OK)
    {
        result = MPM_RESULT_OK;
    }
    return result;
}

MPMResult hueAuthGetHttpPrefix(char *prefix, uint32_t *prefixSize, const char *macAddrString,
                               const char *clientID)
{
    MPMResult result = MPM_RESULT_INVALID_PARAMETER;
    HueDiscoveredCtx discoveredCtx;
    bool bridgeFound = false;
    uint32_t size = 0;

    if ((prefix != NULL) && (prefixSize != NULL) && (macAddrString != NULL) && (clientID != NULL))
    {
        result = MPM_RESULT_NOT_PRESENT;
        bridgeFound = findDiscoveredBridge(macAddrString, &discoveredCtx);
        OIC_LOG_V(DEBUG, TAG, "client id - %s", discoveredCtx.clientIDs);
        if (bridgeFound == true)
        {
            result = MPM_RESULT_NOT_AUTHORIZED;
            if (discoveredFindClientID(&discoveredCtx, clientID) == true)
            {
                result = MPM_RESULT_INSUFFICIENT_BUFFER;

                size = strlen(clientID);
                size += strlen(discoveredCtx.ipAddrString);
                size += strlen("/api/");
                size += 1;

                if (*prefixSize >= size)
                {
                    result = MPM_RESULT_OK;
                    memset(prefix, 0, *prefixSize);
                    strcpy(prefix, discoveredCtx.ipAddrString);
                    strcat(prefix, "/api/");
                    strcat(prefix, clientID);
                }
                else
                {
                    *prefixSize = size;
                }
            }
        }
    }
    return (result);
}

MPMResult hueAuthDestroy()
{
    MPMResult result = MPM_RESULT_INTERNAL_ERROR;
    if (g_hueAuthCtx.structInitialized == true)
    {
        result = MPM_RESULT_OK;

        // Checks all the discovered Bridges and removes the authorized Bridges from the list.

        for (std::vector<HueDiscoveredCtx>::iterator it = g_discoveredBridges.begin() ;
             it != g_discoveredBridges.end(); ++it)
        {
            if (g_hueAuthCtx.removeAuthBridge != NULL)
            {
                g_hueAuthCtx.removeAuthBridge((*it).macAddrString);
            }
        }
        g_discoveredBridges.clear();
    }
    // wipe the rest of the context
    memset(&g_hueAuthCtx, 0, sizeof(HueAuthCtx));
    return (result);
}

void initializeHueAuthCtx()
{
    if (g_hueAuthCtx.structInitialized == false)
    {
        memset(&g_hueAuthCtx, 0, sizeof(HueAuthCtx));
        g_hueAuthCtx.structInitialized = true;
        if (pthread_mutex_init(&(g_hueAuthCtx.discoveredLock), NULL) != 0)
        {
            OIC_LOG(ERROR, TAG, "Unable to initialize global resource mutex.");
            // The context will stay uninitialized in the error case
            memset(&g_hueAuthCtx, 0, sizeof(HueAuthCtx));
        }
    }
}

/**
 * Helper function to parse the discovery and add a bridge to the bridge
 * list if it is not already there.
 *
 * NOTE: parsing data patterns that have the following form
 *      []
 *
 *      [{"id":"001788fffe155cbb","internalipaddress":"172.16.20.133"}]
 *
 *      [{"id":"001788fffe155cbb","internalipaddress":"172.16.20.133"},
 *       {"id":"001788fffe1eeeee","internalipaddress":"172.16.20.134"}]
 *
 * @param[in] payload contents of the discovery request.
 *
 * @returns MPM_RESULT_OK - parsed ok else MPM_RESULT_ERROR
 */
static MPMResult parseHueBridgeDiscovery(std::string payload)
{
    MPMResult result = MPM_RESULT_INVALID_PARAMETER;
    cJSON *object = NULL;
    cJSON *array = NULL;
    int32_t numBridges = 0;
    int32_t index;
    char *macAddrString = NULL;
    char *ipAddrString = NULL;

    if (!payload.empty())
    {
        result = MPM_RESULT_INTERNAL_ERROR;
        array = cJSON_Parse((char *) payload.c_str());
        if (array != NULL)
        {
            numBridges = cJSON_GetArraySize(array);
            for (index = 0; index < numBridges; index++)
            {
                object = cJSON_GetArrayItem(array, index);
                if (object == NULL)
                {
                    // if one element is not OK then bail
                    OIC_LOG(ERROR, TAG, "failed parsing json");
                    break;
                }
                if (cJSON_GetObjectItem(object, "id") != NULL)
                {
                    macAddrString = cJSON_GetObjectItem(object, "id")->valuestring;
                }
                if (cJSON_GetObjectItem(object, "internalipaddress") != NULL)
                {
                    ipAddrString = cJSON_GetObjectItem(object, "internalipaddress")->valuestring;
                }
                result = parseOneHueBridge(macAddrString, ipAddrString);
                if (result != MPM_RESULT_OK)
                {
                    // if one element is not OK then bail
                    break;
                }
            }
            cJSON_Delete(array);
        }
        else
        {
            OIC_LOG(ERROR, TAG, "array returned from call to cJSON_Parse is NULL.");
        }
    }
    else
    {
        OIC_LOG(ERROR, TAG, "payload is empty");
    }
    return (result);
}

static MPMResult parseOneHueBridge(char *macAddrString, char *ipAddrString)
{
    MPMResult result = MPM_RESULT_INTERNAL_ERROR;
    HueDiscoveredCtx discoveredCtx;
    bool bridgeFound = false;
    bool bridgeAuthorized = false;
    hueFile fileBridgeCtx;
    memset(&discoveredCtx, 0, sizeof(HueDiscoveredCtx));

    if ((macAddrString != NULL) && (ipAddrString != NULL))
    {
        // Check if bridge is already there in the discovered list
        bridgeFound = findDiscoveredBridge(macAddrString, &discoveredCtx);
        if (bridgeFound == false)
        {
            OIC_LOG_V(INFO, TAG, "Found bridge %s for the first time", macAddrString);

            // Check if the found bridge is there in the file list. if so then authorize it.
            bridgeAuthorized = findAuthorizedBridge(macAddrString, NULL, fileBridgeCtx);

            // Build discovered bridge structure to add in the discovered list
            OICStrcpy(discoveredCtx.macAddrString, MAX_STRING - 1, macAddrString);
            OICStrcpy(discoveredCtx.ipAddrString, MAX_STRING - 1, ipAddrString);

            // Add the client id of the bridge to discovered list
            if (bridgeAuthorized == true)
            {
                collectAuthorizedClients(macAddrString, (char *) (discoveredCtx.clientIDs),
                                         &(discoveredCtx.numClients));

                OIC_LOG_V(INFO, TAG, "Added  ClientId to the list = %s ", discoveredCtx.clientIDs);
            }
            result = addDiscoveredBridge(discoveredCtx);

            if ((bridgeAuthorized == true) && (g_hueAuthCtx.addAuthBridge != NULL))
            {
                g_hueAuthCtx.addAuthBridge(macAddrString, fileBridgeCtx.clientID);
            }
        }
    }
    else if ((macAddrString == NULL) && (ipAddrString == NULL))
    {
        OIC_LOG(INFO, TAG, "this is empty set case []");
        result = MPM_RESULT_OK;
    }
    else
    {
        result = MPM_RESULT_INTERNAL_ERROR;
    }
    return (result);
}

MPMResult addDiscoveredBridge(HueDiscoveredCtx discoveredCtx)
{
    g_discoveredBridges.push_back(discoveredCtx);
    return MPM_RESULT_OK;
}

bool findDiscoveredBridge(const char *macAddrString, HueDiscoveredCtx *discoveredCtx)
{
    std::lock_guard<std::mutex> lock(g_discoveredBridgesLock);

    for (std::vector<HueDiscoveredCtx>::iterator it = g_discoveredBridges.begin() ;
         it != g_discoveredBridges.end(); ++it)
    {
        std::string bridge_mac(macAddrString);
        if (strcmp(bridge_mac.c_str(), (*it).macAddrString) == 0)
        {
            *discoveredCtx = *it;
            return true;
        }
    }

    return false;
}

bool updateDiscoverBridgeDetails(const char *macAddrString, const char *clientID)
{
    std::lock_guard<std::mutex> lock(g_discoveredBridgesLock);

    for (std::vector<HueDiscoveredCtx>::iterator it = g_discoveredBridges.begin() ;
         it != g_discoveredBridges.end(); ++it)
    {
        if ((strcmp(macAddrString, (*it).macAddrString) == 0) && (strcmp((*it).clientIDs, "") == 0))
        {
            OICStrcpy((*it).clientIDs, MAX_STRING, clientID);
            (*it).numClients = 1;
            return true;
        }
    }

    return false;
}

static bool discoveredFindClientID(HueDiscoveredCtx *discoveredCtx, const char *clientID)
{
    bool clientFound = false;
    uint32_t index = 0;

    if ((discoveredCtx != NULL) && (clientID != NULL))
    {
        // search an array for the client id.
        for (index = 0; index < discoveredCtx->numClients; index++)
        {
            if (strcmp(&(discoveredCtx->clientIDs[index * MAX_STRING]), clientID) == 0)
            {
                clientFound = true;
                break;
            }
            else if (strlen(&(discoveredCtx->clientIDs[index * MAX_STRING])) == 0)
            {
                OIC_LOG(ERROR, TAG, " No More Clients in the array ");
                break;
            }
        }
    }
    return (clientFound);
}
