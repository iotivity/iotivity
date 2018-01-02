//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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

#include "resourcehandler.h"
#include "internal/es_util.h"

#include "ocpayload.h"
#include "oic_string.h"
#include "oic_malloc.h"
#include "cautilinterface.h"
#include "experimental/payload_logging.h"

// TODO: Remove this flag and enable the code it guards.
// This is a temporary workaround to ignore the failure of OCLinksPayloadArrayCreate
// in some cases. This allows the response to still be made, even though links property will be
// missing.
// Bug: IOT-2762
#define ES_IGNORE_OCLinksPayloadArrayCreate_FAILIURE

/**
 * @var ES_RH_TAG
 * @brief Logging tag for module name.
 */
#define ES_RH_TAG "ES_RH"

#define EASY_SETUP_RES  0
#define WIFI_CONF_RES   1
#define CLOUD_CONF_RES  2
#define DEV_CONF_RES    3

/**
 * @brief  To determine the inclusion/exclusion of resources in batch response.
 */
typedef enum
{
    RES_EXCLUDE = 0,        /**< Indicates the exclusion of the resource **/
    RES_INCLUDE = 1,        /**< Indicates the inclusion of the resource with its representation**/
    RES_INCLUDE_EMPTY_REP = 2   /**< Indicates the inclusion of the resource with empty representation **/
} ES_BATCH_UPDATE_RESPONSE;

//-----------------------------------------------------------------------------
// Private variables
//-----------------------------------------------------------------------------

/**
 * @var g_ESEasySetupResource
 * @brief Structure for holding the Provisioning status
 */
EasySetupResource g_ESEasySetupResource;
WiFiConfResource g_ESWiFiConfResource;
CoapCloudConfResource g_ESCoapCloudConfResource;
DevConfResource g_ESDevConfResource;

//-----------------------------------------------------------------------------
// Private internal function prototypes
//-----------------------------------------------------------------------------
OCEntityHandlerResult OCEntityHandlerCb(OCEntityHandlerFlag flag, OCEntityHandlerRequest *ehRequest,
        void *callback);
OCEntityHandlerResult ProcessGetRequest(OCEntityHandlerRequest *ehRequest, OCRepPayload** payload);
OCEntityHandlerResult ProcessPutRequest(OCEntityHandlerRequest *ehRequest, OCRepPayload** payload);
OCEntityHandlerResult ProcessPostRequest(OCEntityHandlerRequest *ehRequest, OCRepPayload** payload);
OCEntityHandlerResult updateEasySetupResource(OCEntityHandlerRequest* ehRequest, OCRepPayload* input,
        ES_BATCH_UPDATE_RESPONSE batch_update_rsrc_arr[4]);
void updateEasySetupConnectProperty(OCRepPayload* input);
OCEntityHandlerResult updateWiFiConfResource(OCRepPayload* input);
void updateCoapCloudConfResource(OCRepPayload* input);
OCEntityHandlerResult updateDevConfResource(OCRepPayload* input);
bool isAuthTypeSupported(WIFI_AUTHTYPE authType);
bool isEncTypeSupported(WIFI_ENCTYPE encType);
const char *getResult(OCStackResult result);

ESConnectRequestCB gConnectRequestEvtCb = NULL;
ESWiFiConfCB gWifiConfRsrcEvtCb = NULL;
ESCoapCloudConfCB gCoapCloudConfRsrcEvtCb = NULL;
ESDevConfCB gDevConfRsrcEvtCb = NULL;

ESReadUserdataCb gReadUserdataCb = NULL;
ESWriteUserdataCb gWriteUserdataCb = NULL;

void GetInterfaceNameFromQuery(const char *query, char **iface)
{
    if (!iface)
    {
        return;
    }
    *iface = NULL;
    char *str = OICStrdup(query);
    char *ptr = strtok(str, ";");

    while (ptr)
    {
        if (strstr(ptr, ".if."))
        {
            char *if_ptr = NULL;
            if_ptr = strtok(ptr, "=");
            if_ptr = strtok(NULL, "=");

            *iface = OICStrdup(if_ptr);
            break;
        }
        ptr = strtok(NULL, ";");
    }

    OICFree(str);
}

bool CompareResourceInterface(const char *from, const char *iface)
{
    char *if_ptr = NULL;
    GetInterfaceNameFromQuery(from, &if_ptr);
    if (!if_ptr)
    {
        return false;
    }
    if (!strcmp(if_ptr, iface))
    {
        OICFree(if_ptr);
        return true;
    }
    OICFree(if_ptr);

    return false;
}

ESResult SetCallbackForUserData(ESReadUserdataCb readCb, ESWriteUserdataCb writeCb)
{
    if (!readCb && !writeCb)
    {
        OIC_LOG(DEBUG, ES_RH_TAG, "Both of callbacks for user data are null");
        return ES_ERROR;
    }
    gReadUserdataCb = readCb;
    gWriteUserdataCb = writeCb;
    return ES_OK;
}

void RegisterWifiRsrcEventCallBack(ESWiFiConfCB cb)
{
    gWifiConfRsrcEvtCb = cb;
}

void RegisterCloudRsrcEventCallBack(ESCoapCloudConfCB cb)
{
    gCoapCloudConfRsrcEvtCb = cb;
}

void RegisterDevConfRsrcEventCallBack(ESDevConfCB cb)
{
    gDevConfRsrcEvtCb = cb;
}

void RegisterConnectRequestEventCallBack(ESConnectRequestCB cb)
{
    gConnectRequestEvtCb = cb;
}

void UnRegisterResourceEventCallBack(void)
{
    if (gWifiConfRsrcEvtCb)
    {
        gWifiConfRsrcEvtCb = NULL;
    }
    if (gCoapCloudConfRsrcEvtCb)
    {
        gCoapCloudConfRsrcEvtCb = NULL;
    }
    if (gDevConfRsrcEvtCb)
    {
        gDevConfRsrcEvtCb = NULL;
    }
    if (gConnectRequestEvtCb)
    {
        gConnectRequestEvtCb = NULL;
    }
}

OCStackResult initEasySetupResource(bool isSecured)
{
    g_ESEasySetupResource.status = ES_STATE_INIT;
    g_ESEasySetupResource.lastErrCode = ES_ERRCODE_NO_ERROR;
    for (int i = 0; i < NUM_CONNECT_TYPE; ++i)
    {
        g_ESEasySetupResource.connectRequest[i] = ES_CONNECT_NONE;
    }
    g_ESEasySetupResource.numRequest = 0;

    OCStackResult res = OC_STACK_ERROR;
    if (isSecured)
    {
        res = OCCreateResource(&g_ESEasySetupResource.handle, OC_RSRVD_ES_RES_TYPE_EASYSETUP,
        OC_RSRVD_INTERFACE_DEFAULT,
        OC_RSRVD_ES_URI_EASYSETUP, OCEntityHandlerCb,
        NULL, OC_DISCOVERABLE | OC_OBSERVABLE | OC_SECURE);
    }
    else
    {
        res = OCCreateResource(&g_ESEasySetupResource.handle, OC_RSRVD_ES_RES_TYPE_EASYSETUP,
        OC_RSRVD_INTERFACE_DEFAULT,
        OC_RSRVD_ES_URI_EASYSETUP, OCEntityHandlerCb,
        NULL, OC_DISCOVERABLE | OC_OBSERVABLE);
    }
    if (res != OC_STACK_OK)
    {
        OIC_LOG_V(ERROR, ES_RH_TAG, "Created EasySetup resource with result: %s", getResult(res));
        return res;
    }

    res = OCBindResourceTypeToResource(g_ESEasySetupResource.handle, OC_RSRVD_ES_RES_TYPE_COL);
    if (res != OC_STACK_OK)
    {
        OIC_LOG_V(ERROR, ES_RH_TAG, "Binding Resource type with result: %s", getResult(res));
        return res;
    }

    res = OCBindResourceInterfaceToResource(g_ESEasySetupResource.handle, OC_RSRVD_INTERFACE_LL);
    if (res != OC_STACK_OK)
    {
        OIC_LOG_V(ERROR, ES_RH_TAG, "Binding Resource interface with result: %s", getResult(res));
        return res;
    }
    res = OCBindResourceInterfaceToResource(g_ESEasySetupResource.handle, OC_RSRVD_INTERFACE_BATCH);
    if (res != OC_STACK_OK)
    {
        OIC_LOG_V(ERROR, ES_RH_TAG, "Binding Resource interface with result: %s", getResult(res));
        return res;
    }

    OIC_LOG_V(DEBUG, ES_RH_TAG, "Created EasySetup resource with result: %s", getResult(res));
    return res;
}

OCStackResult initWiFiConfResource(bool isSecured)
{
    OCStackResult res = OC_STACK_ERROR;

    g_ESWiFiConfResource.supportedFreq[0] = WIFI_24G;
    g_ESWiFiConfResource.supportedFreq[1] = WIFI_5G;
    g_ESWiFiConfResource.numSupportedFreq=2;
    g_ESWiFiConfResource.supportedMode[0] = WIFI_11A;
    g_ESWiFiConfResource.supportedMode[1] = WIFI_11B;
    g_ESWiFiConfResource.supportedMode[2] = WIFI_11G;
    g_ESWiFiConfResource.supportedMode[3] = WIFI_11N;
    g_ESWiFiConfResource.numMode = 4;
    g_ESWiFiConfResource.authType = NONE_AUTH;
    g_ESWiFiConfResource.encType = NONE_ENC;
    OICStrcpy(g_ESWiFiConfResource.ssid, sizeof(g_ESWiFiConfResource.ssid), "");
    OICStrcpy(g_ESWiFiConfResource.cred, sizeof(g_ESWiFiConfResource.cred), "");

    if (isSecured)
    {
        res = OCCreateResource(&g_ESWiFiConfResource.handle, OC_RSRVD_ES_RES_TYPE_WIFICONF,
        OC_RSRVD_INTERFACE_DEFAULT,
        OC_RSRVD_ES_URI_WIFICONF, OCEntityHandlerCb,
        NULL, OC_DISCOVERABLE | OC_OBSERVABLE | OC_SECURE);
    }
    else
    {
        res = OCCreateResource(&g_ESWiFiConfResource.handle, OC_RSRVD_ES_RES_TYPE_WIFICONF,
        OC_RSRVD_INTERFACE_DEFAULT,
        OC_RSRVD_ES_URI_WIFICONF, OCEntityHandlerCb,
        NULL, OC_DISCOVERABLE | OC_OBSERVABLE);
    }

    if (res != OC_STACK_OK)
    {
        OIC_LOG_V(ERROR, ES_RH_TAG, "Failed to create WiFiConf resource with result: %s",
            getResult(res));
        return res;
    }

    res = OCBindResourceInterfaceToResource(g_ESWiFiConfResource.handle,
    OC_RSRVD_INTERFACE_READ_WRITE);
    if (res != OC_STACK_OK)
    {
        OIC_LOG_V(ERROR, ES_RH_TAG, "Binding Resource interface with result: %s", getResult(res));
        return res;
    }

    OIC_LOG_V(DEBUG, ES_RH_TAG, "Created WiFiConf resource with result: %s", getResult(res));
    return res;

}

OCStackResult initCoapCloudConfResource(bool isSecured)
{
    OCStackResult res = OC_STACK_ERROR;

    OICStrcpy(g_ESCoapCloudConfResource.authCode, sizeof(g_ESCoapCloudConfResource.authCode), "");
    OICStrcpy(g_ESCoapCloudConfResource.accessToken, sizeof(g_ESCoapCloudConfResource.accessToken),
            "");
    g_ESCoapCloudConfResource.accessTokenType = NONE_OAUTH_TOKENTYPE;
    OICStrcpy(g_ESCoapCloudConfResource.authProvider,
            sizeof(g_ESCoapCloudConfResource.authProvider), "");
    OICStrcpy(g_ESCoapCloudConfResource.ciServer, sizeof(g_ESCoapCloudConfResource.ciServer), "");

    if (isSecured)
    {
        res = OCCreateResource(&g_ESCoapCloudConfResource.handle,
                OC_RSRVD_ES_RES_TYPE_COAPCLOUDCONF,
                OC_RSRVD_INTERFACE_DEFAULT,
                OC_RSRVD_ES_URI_COAPCLOUDCONF, OCEntityHandlerCb,
                NULL, OC_DISCOVERABLE | OC_OBSERVABLE | OC_SECURE);
    }
    else
    {
        res = OCCreateResource(&g_ESCoapCloudConfResource.handle,
                OC_RSRVD_ES_RES_TYPE_COAPCLOUDCONF,
                OC_RSRVD_INTERFACE_DEFAULT,
                OC_RSRVD_ES_URI_COAPCLOUDCONF, OCEntityHandlerCb,
                NULL, OC_DISCOVERABLE | OC_OBSERVABLE);
    }

    if (res != OC_STACK_OK)
    {
        OIC_LOG_V(ERROR, ES_RH_TAG, "Failed to create CoapCloudConf resource with result: %s",
            getResult(res));
        return res;
    }

    res = OCBindResourceInterfaceToResource(g_ESCoapCloudConfResource.handle,
            OC_RSRVD_INTERFACE_READ_WRITE);
    if (res != OC_STACK_OK)
    {
        OIC_LOG_V(ERROR, ES_RH_TAG, "Binding Resource interface with result: %s", getResult(res));
        return res;
    }

    OIC_LOG_V(DEBUG, ES_RH_TAG, "Created CoapCloudConf resource with result: %s", getResult(res));
    return res;
}

OCStackResult initDevConfResource(bool isSecured)
{
    OCStackResult res = OC_STACK_ERROR;

    OICStrcpy(g_ESDevConfResource.devName, sizeof(g_ESDevConfResource.devName), "");

    if (isSecured)
    {
        res = OCCreateResource(&g_ESDevConfResource.handle, OC_RSRVD_ES_RES_TYPE_DEVCONF,
        OC_RSRVD_INTERFACE_DEFAULT,
        OC_RSRVD_ES_URI_DEVCONF, OCEntityHandlerCb,
        NULL, OC_DISCOVERABLE | OC_SECURE);
    }
    else
    {
        res = OCCreateResource(&g_ESDevConfResource.handle, OC_RSRVD_ES_RES_TYPE_DEVCONF,
        OC_RSRVD_INTERFACE_DEFAULT,
        OC_RSRVD_ES_URI_DEVCONF, OCEntityHandlerCb,
        NULL, OC_DISCOVERABLE);
    }

    if (res != OC_STACK_OK)
    {
        OIC_LOG_V(ERROR, ES_RH_TAG, "Failed to create DevConf resource with result: %s",
            getResult(res));
        return res;
    }

    res = OCBindResourceInterfaceToResource(g_ESDevConfResource.handle, OC_RSRVD_INTERFACE_READ);
    if (res != OC_STACK_OK)
    {
        OIC_LOG_V(ERROR, ES_RH_TAG, "Binding Resource interface with result: %s", getResult(res));
        return res;
    }

    OIC_LOG_V(DEBUG, ES_RH_TAG, "Created DevConf resource with result: %s", getResult(res));
    return res;

}

OCEntityHandlerResult updateEasySetupResource(OCEntityHandlerRequest* ehRequest,
    OCRepPayload* input, ES_BATCH_UPDATE_RESPONSE batch_update_rsrc_arr[4])
{
    OIC_LOG_V(DEBUG, ES_RH_TAG, "g_ESEasySetupResource.status %d", g_ESEasySetupResource.status);

    char *iface_name = NULL;
    GetInterfaceNameFromQuery(ehRequest->query, &iface_name);
    if (!iface_name)
    {
        iface_name = OICStrdup(OC_RSRVD_INTERFACE_DEFAULT);
        if(NULL == iface_name)
        {
            OIC_LOG_V(ERROR, ES_RH_TAG, "Failed to duplicate string.");
            return OC_EH_INTERNAL_SERVER_ERROR;
        }
    }

    OCEntityHandlerResult ehResult = OC_EH_OK;

    if (!strcmp(iface_name, OC_RSRVD_INTERFACE_BATCH))
    {
        batch_update_rsrc_arr[EASY_SETUP_RES] = RES_EXCLUDE;
        batch_update_rsrc_arr[WIFI_CONF_RES] = RES_EXCLUDE;
        batch_update_rsrc_arr[CLOUD_CONF_RES] = RES_EXCLUDE;
        batch_update_rsrc_arr[DEV_CONF_RES] = RES_EXCLUDE;

        bool hasError = false;
        // When Provisioning resource has a POST with BatchInterface
        // Parsing POST request on Batch Interface cosidering same format as GET using batch.
        OCRepPayload *children = input;
        while(children)
        {
            char* uri = children->uri;
            if (NULL == uri)
            {
                OIC_LOG(DEBUG, ES_RH_TAG, "No URI found in request");
            }
            else
            {
                OIC_LOG_V(DEBUG, ES_RH_TAG, "Request URI [%s]", uri);
            }

            OCRepPayload *repPayload = NULL;
            OCRepPayloadGetPropObject(children, OC_RSRVD_REPRESENTATION, &repPayload);
            if (NULL == repPayload)
            {
                OIC_LOG(ERROR, ES_RH_TAG, "repPayload is null!");
                children = children->next;
                continue;
            }

            // If uri is NULL, rep is applied to all resources in collection;
            // otherwise its applied to specific target resources.
            if (NULL == uri || 0 == strlen(uri) || 0 == strcmp(uri, OC_RSRVD_ES_URI_EASYSETUP))
            {
                // If payload has read-only properties, then the request is considered as a bad request.
                if (!OCRepPayloadIsNull(children, OC_RSRVD_ES_PROVSTATUS) ||
                    !OCRepPayloadIsNull(children, OC_RSRVD_ES_LAST_ERRORCODE))
                {
                    OIC_LOG(ERROR, ES_RH_TAG, "Read-only property cannot be updated.");
                    // "rep" field of EasySetup resource in the response will be empty.
                    batch_update_rsrc_arr[EASY_SETUP_RES] = RES_INCLUDE_EMPTY_REP;
                    hasError = true;
                }
                else
                {
                    updateEasySetupConnectProperty(repPayload);
                    batch_update_rsrc_arr[EASY_SETUP_RES] = RES_INCLUDE;
                }
            }

            if (NULL == uri || 0 == strlen(uri)
                || 0 == strcmp(uri, OC_RSRVD_ES_URI_WIFICONF))
            {
                if (updateWiFiConfResource(repPayload) != OC_EH_OK)
                {
                    OIC_LOG(ERROR, ES_RH_TAG, "Failed to update WiFiConf resource.");
                    hasError = true;
                    // As there is a problem in updating the WiFiConf resource,
                    // corresponding "rep" field in the response will be empty.
                    batch_update_rsrc_arr[WIFI_CONF_RES] = RES_INCLUDE_EMPTY_REP;
                }
                else
                {
                    batch_update_rsrc_arr[WIFI_CONF_RES] = RES_INCLUDE;
                }
            }

            if (NULL == uri ||  0 == strlen(uri)
                || 0 == strcmp(uri, OC_RSRVD_ES_URI_COAPCLOUDCONF))
            {
                updateCoapCloudConfResource(repPayload);
                batch_update_rsrc_arr[CLOUD_CONF_RES] = RES_INCLUDE;
            }

            if (NULL == uri ||  0 == strlen(uri)
                || 0 == strcmp(uri, OC_RSRVD_ES_URI_DEVCONF))
            {
                if (updateDevConfResource(repPayload) != OC_EH_OK)
                {
                    OIC_LOG(ERROR, ES_RH_TAG, "Failed to update DevConf resource.");
                    hasError = true;
                    // As there is a problem in updating the DevConf resource,
                    // corresponding "rep" field in the response will be empty.
                    batch_update_rsrc_arr[DEV_CONF_RES] = RES_INCLUDE_EMPTY_REP;
                }
                else
                {
                    batch_update_rsrc_arr[DEV_CONF_RES] = RES_INCLUDE;
                }
            }

            children = children->next;
            OCRepPayloadDestroy(repPayload);
        }

        if (hasError)
        {
           ehResult = OC_EH_BAD_REQ;
        }
    }
    else if (!strcmp(iface_name, OC_RSRVD_INTERFACE_DEFAULT))
    {
        OIC_LOG(DEBUG, ES_RH_TAG, "Handling POST request on default interface");
        // If payload has read-only properties, then the request is considered as a bad request.
        if (!OCRepPayloadIsNull(input, OC_RSRVD_ES_PROVSTATUS) ||
            !OCRepPayloadIsNull(input, OC_RSRVD_ES_LAST_ERRORCODE))
        {
            OIC_LOG(ERROR, ES_RH_TAG, "Read-only property cannot be updated.");
            ehResult = OC_EH_BAD_REQ;
        }
        else
        {
            updateEasySetupConnectProperty(input);
        }
    }

    OICFree(iface_name);

    OIC_LOG(DEBUG, ES_RH_TAG, "updateEasySetupResource exit");
    return ehResult;
}

void updateEasySetupConnectProperty(OCRepPayload* input)
{
    int64_t *connect_req = NULL;
    size_t dimensions[MAX_REP_ARRAY_DEPTH] = { 0 };
    if (OCRepPayloadGetIntArray(input, OC_RSRVD_ES_CONNECT, &connect_req, dimensions))
    {
        ESConnectRequest* connectRequest = (ESConnectRequest*)OICMalloc(sizeof(ESConnectRequest));
        if (!connectRequest)
        {
            OIC_LOG(ERROR, ES_RH_TAG, "Failed to allocate Payload");
            return;
        }

        int cntRequest = 0;
        for (unsigned int i = 0 ; i < NUM_CONNECT_TYPE ; ++i)
        {
            g_ESEasySetupResource.connectRequest[i] = ES_CONNECT_NONE;
            connectRequest->connect[i] = ES_CONNECT_NONE;

            if (i < dimensions[0] && connect_req[i] >= ES_CONNECT_NONE)
            {
                g_ESEasySetupResource.connectRequest[cntRequest] = connect_req[i];
                connectRequest->connect[cntRequest] = connect_req[i];
                OIC_LOG_V(DEBUG, ES_RH_TAG, "g_ESEasySetupResource.connectType[%d] : %d",
                                                    cntRequest, g_ESEasySetupResource.connectRequest[cntRequest]);
                cntRequest++;
            }
        }
        connectRequest->numRequest = cntRequest;
        g_ESEasySetupResource.numRequest = cntRequest;

        if (g_ESEasySetupResource.connectRequest[0] != ES_CONNECT_NONE)
        {
            OIC_LOG(DEBUG, ES_RH_TAG, "Send ConnectRequest Callback To ES");

            //@todo Need to check appropriateness of gWiFiData
            if (gConnectRequestEvtCb != NULL)
            {
                gConnectRequestEvtCb(ES_OK, connectRequest);
            }
            else
            {
                OIC_LOG(ERROR, ES_RH_TAG, "gConnectRequestEvtCb is NULL");
            }
        }
    }
}

OCEntityHandlerResult updateWiFiConfResource(OCRepPayload* input)
{
    // If payload has read-only properties, then the request is considered as a bad request.
    if (!OCRepPayloadIsNull(input, OC_RSRVD_ES_SUPPORTEDWIFIMODE) ||
        !OCRepPayloadIsNull(input, OC_RSRVD_ES_SUPPORTEDWIFIFREQ) ||
        !OCRepPayloadIsNull(input, OC_RSRVD_ES_SUPPORTEDWIFIAUTHTYPE) ||
        !OCRepPayloadIsNull(input, OC_RSRVD_ES_SUPPORTEDWIFIENCTYPE))
    {
        OIC_LOG(ERROR, ES_RH_TAG, "Read-only property cannot be updated.");
        return OC_EH_BAD_REQ;
    }

    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    ESWiFiConfData* wiFiData = (ESWiFiConfData*) OICMalloc(sizeof(ESWiFiConfData));
    if (wiFiData == NULL)
    {
        OIC_LOG(DEBUG, ES_RH_TAG, "OICMalloc is failed");
        return ehResult;
    }

    char* ssid = NULL;
    char* cred = NULL;
    char *authType = NULL;
    char *encType = NULL;
    memset(wiFiData->ssid, 0, OIC_STRING_MAX_VALUE);
    memset(wiFiData->pwd, 0, OIC_STRING_MAX_VALUE);
    wiFiData->authtype = NONE_AUTH;
    wiFiData->enctype = NONE_AUTH;
    wiFiData->userdata = NULL;

    bool validAuthType = false;
    if (OCRepPayloadGetPropString(input, OC_RSRVD_ES_AUTHTYPE, &authType))
    {
        WIFI_AUTHTYPE tmp;
        validAuthType = WiFiAuthTypeStringToEnum(authType, &tmp);
        if (validAuthType && isAuthTypeSupported(tmp))
        {
            wiFiData->authtype = tmp;
            OIC_LOG_V(INFO_PRIVATE, ES_RH_TAG, "g_ESWiFiConfResource.authType %u",
                    wiFiData->authtype);
        }
        else
        {
            OIC_LOG(ERROR, ES_RH_TAG, "AuthType is not supported.");
            ehResult = OC_EH_BAD_REQ;
            goto EXIT;
        }
    }

    bool validEncType = false;
    if (OCRepPayloadGetPropString(input, OC_RSRVD_ES_ENCTYPE, &encType))
    {
        WIFI_ENCTYPE tmp;
        validEncType = WiFiEncTypeStringToEnum(encType, &tmp);
        if (validEncType && isEncTypeSupported(tmp))
        {
            wiFiData->enctype = tmp;
            OIC_LOG_V(INFO_PRIVATE, ES_RH_TAG, "g_ESWiFiConfResource.encType %u",
                    wiFiData->enctype);
        }
        else
        {
            OIC_LOG(ERROR, ES_RH_TAG, "EncType is not supported.");
            ehResult = OC_EH_BAD_REQ;
            goto EXIT;
        }
    }

    if (validAuthType)
    {
        g_ESWiFiConfResource.authType = wiFiData->authtype;
    }

    if (validEncType)
    {
        g_ESWiFiConfResource.encType = wiFiData->enctype;
    }

    if (OCRepPayloadGetPropString(input, OC_RSRVD_ES_SSID, &ssid))
    {
        OICStrcpy(g_ESWiFiConfResource.ssid, sizeof(g_ESWiFiConfResource.ssid), ssid);
        OICStrcpy(wiFiData->ssid, sizeof(wiFiData->ssid), ssid);
        OIC_LOG_V(INFO_PRIVATE, ES_RH_TAG, "g_ESWiFiConfResource.ssid : %s",
                g_ESWiFiConfResource.ssid);
    }

    if (OCRepPayloadGetPropString(input, OC_RSRVD_ES_CRED, &cred))
    {
        OICStrcpy(g_ESWiFiConfResource.cred, sizeof(g_ESWiFiConfResource.cred), cred);
        OICStrcpy(wiFiData->pwd, sizeof(wiFiData->pwd), cred);
        OIC_LOG_V(INFO_PRIVATE, ES_RH_TAG, "g_ESWiFiConfResource.cred %s",
                g_ESWiFiConfResource.cred);
    }

    if (gReadUserdataCb)
    {
        gReadUserdataCb(input, OC_RSRVD_ES_RES_TYPE_WIFICONF, &wiFiData->userdata);
    }

    if (ssid || cred || validAuthType || validEncType)
    {
        OIC_LOG(DEBUG, ES_RH_TAG, "Send WiFiConfRsrc Callback To ES");

        //@todo Need to check appropriateness of gWiFiData
        if (gWifiConfRsrcEvtCb != NULL)
        {
            gWifiConfRsrcEvtCb(ES_OK, wiFiData);
        }
        else
        {
            OIC_LOG(ERROR, ES_RH_TAG, "gWifiConfRsrcEvtCb is NULL");
        }
    }

    if (OC_STACK_NO_OBSERVERS == OCNotifyAllObservers(g_ESWiFiConfResource.handle, OC_HIGH_QOS))
    {
        OIC_LOG(DEBUG, ES_RH_TAG, "WiFiConf resource doesn't have any observer.");
    }

    if (OC_STACK_NO_OBSERVERS == OCNotifyAllObservers(g_ESEasySetupResource.handle, OC_HIGH_QOS))
    {
        OIC_LOG(DEBUG, ES_RH_TAG, "EasySetup resource doesn't have any observer.");
    }

    ehResult = OC_EH_OK;

EXIT:
    OICFree(encType);
    OICFree(authType);
    OICFree(cred);
    OICFree(ssid);
    OICFree(wiFiData);
    return ehResult;
}

void updateCoapCloudConfResource(OCRepPayload* input)
{
    ESCoapCloudConfData* cloudData = (ESCoapCloudConfData*)OICMalloc(sizeof(ESCoapCloudConfData));

    if (cloudData == NULL)
    {
        OIC_LOG(DEBUG, ES_RH_TAG, "OICMalloc is failed");
        return;
    }

    memset(cloudData->authCode, 0, OIC_STRING_MAX_VALUE);
    memset(cloudData->accessToken, 0, OIC_STRING_MAX_VALUE);
    g_ESCoapCloudConfResource.accessTokenType = NONE_OAUTH_TOKENTYPE;
    memset(cloudData->authProvider, 0, OIC_STRING_MAX_VALUE);
    memset(cloudData->ciServer, 0, OIC_URI_STRING_MAX_VALUE);
    cloudData->userdata = NULL;

    char *authCode = NULL;
    if (OCRepPayloadGetPropString(input, OC_RSRVD_ES_AUTHCODE, &authCode))
    {
        OICStrcpy(g_ESCoapCloudConfResource.authCode, sizeof(g_ESCoapCloudConfResource.authCode), authCode);
        OICStrcpy(cloudData->authCode, sizeof(cloudData->authCode), authCode);
        OIC_LOG_V(INFO_PRIVATE, ES_RH_TAG, "g_ESCoapCloudConfResource.authCode %s", g_ESCoapCloudConfResource.authCode);
    }

    char *accessToken = NULL;
    if (OCRepPayloadGetPropString(input, OC_RSRVD_ES_ACCESSTOKEN, &accessToken))
    {
        OICStrcpy(g_ESCoapCloudConfResource.accessToken, sizeof(g_ESCoapCloudConfResource.accessToken), accessToken);
        OICStrcpy(cloudData->accessToken, sizeof(cloudData->accessToken), accessToken);
        OIC_LOG_V(INFO_PRIVATE, ES_RH_TAG, "g_ESCoapCloudConfResource.accessToken %s", g_ESCoapCloudConfResource.accessToken);
    }

    int64_t accessTokenType = -1;
    if (OCRepPayloadGetPropInt(input, OC_RSRVD_ES_ACCESSTOKEN_TYPE, &accessTokenType))
    {
        g_ESCoapCloudConfResource.accessTokenType = accessTokenType;
        cloudData->accessTokenType = g_ESCoapCloudConfResource.accessTokenType;
        OIC_LOG_V(INFO_PRIVATE, ES_RH_TAG, "g_ESCoapCloudConfResource.accessTokenType %d", g_ESCoapCloudConfResource.accessTokenType);
    }

    char *authProvider = NULL;
    if (OCRepPayloadGetPropString(input, OC_RSRVD_ES_AUTHPROVIDER, &authProvider))
    {
        OICStrcpy(g_ESCoapCloudConfResource.authProvider, sizeof(g_ESCoapCloudConfResource.authProvider), authProvider);
        OICStrcpy(cloudData->authProvider, sizeof(cloudData->authProvider), authProvider);
        OIC_LOG_V(INFO_PRIVATE, ES_RH_TAG, "g_ESCoapCloudConfResource.authServerUrl %s", g_ESCoapCloudConfResource.authProvider);
    }

    char *ciServer = NULL;
    if (OCRepPayloadGetPropString(input, OC_RSRVD_ES_CISERVER, &ciServer))
    {
        OICStrcpy(g_ESCoapCloudConfResource.ciServer, sizeof(g_ESCoapCloudConfResource.ciServer), ciServer);
        OICStrcpy(cloudData->ciServer, sizeof(cloudData->ciServer), ciServer);
        OIC_LOG_V(INFO_PRIVATE, ES_RH_TAG, "g_ESCoapCloudConfResource.ciServer %s", g_ESCoapCloudConfResource.ciServer);
    }

    if (gReadUserdataCb)
    {
        gReadUserdataCb(input, OC_RSRVD_ES_RES_TYPE_COAPCLOUDCONF, &cloudData->userdata);
    }

    if (authCode || accessToken || authProvider || ciServer)
    {
        OIC_LOG(DEBUG, ES_RH_TAG, "Send CoapCloudConfRsrc Callback To ES");

        //@todo Need to check appropriateness of gCloudData
        if (gCoapCloudConfRsrcEvtCb != NULL)
        {
            gCoapCloudConfRsrcEvtCb(ES_OK, cloudData);
        }
        else
        {
            OIC_LOG(DEBUG, ES_RH_TAG, "gCoapCloudConfRsrcEvtCb is NULL");
        }
    }

    if (OC_STACK_NO_OBSERVERS == OCNotifyAllObservers(g_ESCoapCloudConfResource.handle, OC_HIGH_QOS))
    {
        OIC_LOG(DEBUG, ES_RH_TAG, "CoapCloudConf resource doesn't have any observer.");
    }

    if (OC_STACK_NO_OBSERVERS == OCNotifyAllObservers(g_ESEasySetupResource.handle, OC_HIGH_QOS))
    {
        OIC_LOG(DEBUG, ES_RH_TAG, "EasySetup resource doesn't have any observer.");
    }

    OICFree(cloudData);
}

OCEntityHandlerResult updateDevConfResource(OCRepPayload* input)
{
    ESDevConfData* devConfData = (ESDevConfData*)OICMalloc(sizeof(ESDevConfData));

    if (devConfData == NULL)
    {
        OIC_LOG(DEBUG, ES_RH_TAG, "OICMalloc is failed");
        return OC_EH_ERROR;
    }
    devConfData->userdata = NULL;

    if (gReadUserdataCb)
    {
        gReadUserdataCb(input, OC_RSRVD_ES_RES_TYPE_DEVCONF, &devConfData->userdata);
    }

    // If a writable property in oic.r.devconf is added later,
    // a condition for calling a resistered callback should be implemented also.
    if (devConfData->userdata != NULL)
    {
        OIC_LOG(DEBUG, ES_RH_TAG, "Send DevConfRsrc Callback To ES");

        //@todo : Need to check appropriateness of gDevConfData
        if (gDevConfRsrcEvtCb != NULL)
        {
            gDevConfRsrcEvtCb(ES_OK, devConfData);
        }
        else
        {
            OIC_LOG(DEBUG, ES_RH_TAG, "gDevConfRsrcEvtCb is NULL");
        }
    }

    if (OC_STACK_NO_OBSERVERS == OCNotifyAllObservers(g_ESDevConfResource.handle, OC_HIGH_QOS))
    {
        OIC_LOG(DEBUG, ES_RH_TAG, "devConfResource doesn't have any observer.");
    }

    OICFree(devConfData);
    return OC_EH_OK;
}

OCRepPayload* constructResponseOfWiFiConf(char *interface, ES_BATCH_UPDATE_RESPONSE resp)
{
    if (!strcmp(interface, OC_RSRVD_INTERFACE_BATCH) && RES_EXCLUDE == resp)
    {
        OIC_LOG(DEBUG, ES_RH_TAG, "Excluding WiFiConf resource from the batch response.");
        return NULL;
    }

    OCRepPayload* payload = OCRepPayloadCreate();
    if (!payload)
    {
        OIC_LOG(ERROR, ES_RH_TAG, "Failed to allocate Payload");
        return NULL;
    }

    if (g_ESWiFiConfResource.handle == NULL)
    {
        OIC_LOG(ERROR, ES_RH_TAG, "WiFiConf resource is not created");
        return NULL;
    }

    OIC_LOG(DEBUG, ES_RH_TAG, "constructResponse WiFiConf res");

    OCRepPayload* repPayload = NULL;
    OCRepPayload* tempPayload = NULL;
    if (!strcmp(interface, OC_RSRVD_INTERFACE_BATCH))
    {
        OCRepPayloadSetUri(payload, OC_RSRVD_ES_URI_WIFICONF);

        repPayload = OCRepPayloadCreate();
        if (!repPayload)
        {
            OIC_LOG(ERROR, ES_RH_TAG, "Failed to allocate Payload");
            return NULL;
        }

        tempPayload = payload;
        payload = repPayload;

        if (RES_INCLUDE == resp)
        {
            size_t interfacesDimensions[MAX_REP_ARRAY_DEPTH] = {2, 0, 0};
            char **interfaces = (char **)OICMalloc(3 * sizeof(char*));
            if (!interfaces)
            {
                OIC_LOG(ERROR, ES_RH_TAG, "Failed to allocate Payload");
                return NULL;
            }

            interfaces[0] = OICStrdup(OC_RSRVD_INTERFACE_DEFAULT);
            interfaces[1] = OICStrdup(OC_RSRVD_INTERFACE_READ_WRITE);

            OCRepPayloadSetStringArray(payload, OC_RSRVD_ES_INTERFACE, (const char **)interfaces, interfacesDimensions);

            size_t resourceTypesDimensions[MAX_REP_ARRAY_DEPTH] = {1, 0, 0};
            char **resourceTypes = (char **)OICMalloc(2 * sizeof(char*));
            if (!resourceTypes)
            {
                OIC_LOG(ERROR, ES_RH_TAG, "Failed to allocate Payload");
                return NULL;
            }

            resourceTypes[0] = OICStrdup(OC_RSRVD_ES_RES_TYPE_WIFICONF);

            OCRepPayloadSetStringArray(payload, OC_RSRVD_ES_RES_TYPE, (const char **)resourceTypes, resourceTypesDimensions);
        }
    }
    else
    {
        OCRepPayloadAddInterface(payload, OC_RSRVD_INTERFACE_DEFAULT);
        OCRepPayloadAddInterface(payload, OC_RSRVD_INTERFACE_READ_WRITE);
        OCRepPayloadAddResourceType(payload, OC_RSRVD_ES_RES_TYPE_WIFICONF);
    }

    if (RES_INCLUDE == resp)
    {
        // Do not add Read Only Properties when using OC_RSRVD_INTERFACE_READ_WRITE
        if (strcmp(interface, OC_RSRVD_INTERFACE_READ_WRITE) != 0)
        {
            size_t dimensionsModes[MAX_REP_ARRAY_DEPTH] = { g_ESWiFiConfResource.numMode, 0, 0 };
            const char *modes[NUM_WIFIMODE] = { 0, };
            for (int i = 0; i < g_ESWiFiConfResource.numMode; ++i)
            {
                modes[i] = WiFiModeEnumToString(g_ESWiFiConfResource.supportedMode[i]);
            }
            OCRepPayloadSetStringArray(payload, OC_RSRVD_ES_SUPPORTEDWIFIMODE, &modes[0],
                    dimensionsModes);

            size_t dimensionsFreq[MAX_REP_ARRAY_DEPTH] = { g_ESWiFiConfResource.numSupportedFreq, 0, 0 };
            const char *freq[NUM_WIFIFREQ] = { 0, };
            for (int i = 0; i < g_ESWiFiConfResource.numSupportedFreq; ++i)
            {
                freq[i] = WiFiFreqEnumToString(g_ESWiFiConfResource.supportedFreq[i]);
            }
            OCRepPayloadSetStringArray(payload, OC_RSRVD_ES_SUPPORTEDWIFIFREQ, freq, dimensionsFreq);

            size_t dimensionsAuthType[MAX_REP_ARRAY_DEPTH] = { g_ESWiFiConfResource.numSupportedAuthType, 0,
                    0 };
            const char *authType[NUM_WIFIAUTHTYPE] = { 0, };
            for (int i = 0; i < g_ESWiFiConfResource.numSupportedAuthType; ++i)
            {
                authType[i] = WiFiAuthTypeEnumToString(g_ESWiFiConfResource.supportedAuthType[i]);
            }
            OCRepPayloadSetStringArray(payload, OC_RSRVD_ES_SUPPORTEDWIFIAUTHTYPE, authType,
                    dimensionsAuthType);

            size_t dimensionsEncType[MAX_REP_ARRAY_DEPTH] =
                    { g_ESWiFiConfResource.numSupportedEncType, 0, 0 };
            const char *encType[NUM_WIFIENCTYPE] = { 0, };
            for (int i = 0; i < g_ESWiFiConfResource.numSupportedEncType; ++i)
            {
                encType[i] = WiFiEncTypeEnumToString(g_ESWiFiConfResource.supportedEncType[i]);
            }
            OCRepPayloadSetStringArray(payload, OC_RSRVD_ES_SUPPORTEDWIFIENCTYPE, encType,
                    dimensionsEncType);
        }

        OCRepPayloadSetPropString(payload, OC_RSRVD_ES_SSID, g_ESWiFiConfResource.ssid);
        OCRepPayloadSetPropString(payload, OC_RSRVD_ES_CRED, g_ESWiFiConfResource.cred);
        OCRepPayloadSetPropString(payload, OC_RSRVD_ES_AUTHTYPE,
                WiFiAuthTypeEnumToString(g_ESWiFiConfResource.authType));
        OCRepPayloadSetPropString(payload, OC_RSRVD_ES_ENCTYPE,
                WiFiEncTypeEnumToString(g_ESWiFiConfResource.encType));

        if (gWriteUserdataCb)
        {
            gWriteUserdataCb(payload, OC_RSRVD_ES_RES_TYPE_WIFICONF);
        }
    }
    if (!strcmp(interface, OC_RSRVD_INTERFACE_BATCH))
    {
        payload = tempPayload;
        OCRepPayloadSetPropObject(payload, OC_RSRVD_REPRESENTATION, repPayload);
    }

    return payload;
}

OCRepPayload* constructResponseOfCoapCloudConf(char *interface, ES_BATCH_UPDATE_RESPONSE resp)
{
    if (!strcmp(interface, OC_RSRVD_INTERFACE_BATCH) && RES_EXCLUDE == resp)
    {
        OIC_LOG(DEBUG, ES_RH_TAG, "Excluding CoapCloudConf resource from the batch response.");
        return NULL;
    }

    OCRepPayload* payload = OCRepPayloadCreate();
    if (!payload)
    {
        OIC_LOG(ERROR, ES_RH_TAG, "Failed to allocate Payload");
        return NULL;
    }

    if (g_ESCoapCloudConfResource.handle == NULL)
    {
        OIC_LOG(ERROR, ES_RH_TAG, "CoapCloudConf resource is not created");
        return NULL;
    }

    OIC_LOG(DEBUG, ES_RH_TAG, "constructResponse CoapCloudConf res");

    OCRepPayload* repPayload = NULL;
    OCRepPayload* tempPayload = NULL;
    if (!strcmp(interface, OC_RSRVD_INTERFACE_BATCH))
    {
        OCRepPayloadSetUri(payload, OC_RSRVD_ES_URI_COAPCLOUDCONF);

        repPayload = OCRepPayloadCreate();
        if (!repPayload)
        {
            OIC_LOG(ERROR, ES_RH_TAG, "Failed to allocate Payload");
            return NULL;
        }

        tempPayload = payload;
        payload = repPayload;

        if (RES_INCLUDE == resp)
        {
            size_t interfacesDimensions[MAX_REP_ARRAY_DEPTH] = {2, 0, 0};
            char **interfaces = (char **)OICMalloc(3 * sizeof(char*));
            if (!interfaces)
            {
                OIC_LOG(ERROR, ES_RH_TAG, "Failed to allocate Payload");
                return NULL;
            }

            interfaces[0] = OICStrdup(OC_RSRVD_INTERFACE_DEFAULT);
            interfaces[1] = OICStrdup(OC_RSRVD_INTERFACE_READ_WRITE);

            OCRepPayloadSetStringArray(payload, OC_RSRVD_ES_INTERFACE, (const char **)interfaces, interfacesDimensions);

            size_t resourceTypesDimensions[MAX_REP_ARRAY_DEPTH] = {1, 0, 0};
            char **resourceTypes = (char **)OICMalloc(2 * sizeof(char*));
            if (!resourceTypes)
            {
                OIC_LOG(ERROR, ES_RH_TAG, "Failed to allocate Payload");
                return NULL;
            }

            resourceTypes[0] = OICStrdup(OC_RSRVD_ES_RES_TYPE_COAPCLOUDCONF);

            OCRepPayloadSetStringArray(payload, OC_RSRVD_ES_RES_TYPE, (const char **)resourceTypes, resourceTypesDimensions);
        }
    }
    else
    {
        OCRepPayloadAddInterface(payload, OC_RSRVD_INTERFACE_DEFAULT);
        OCRepPayloadAddInterface(payload, OC_RSRVD_INTERFACE_READ_WRITE);
        OCRepPayloadAddResourceType(payload, OC_RSRVD_ES_RES_TYPE_COAPCLOUDCONF);
    }

    if (RES_INCLUDE == resp)
    {
        OCRepPayloadSetPropString(payload, OC_RSRVD_ES_AUTHCODE, g_ESCoapCloudConfResource.authCode);
        OCRepPayloadSetPropString(payload, OC_RSRVD_ES_ACCESSTOKEN, g_ESCoapCloudConfResource.accessToken);
        OCRepPayloadSetPropInt(payload, OC_RSRVD_ES_ACCESSTOKEN_TYPE, (int)g_ESCoapCloudConfResource.accessTokenType);
        OCRepPayloadSetPropString(payload, OC_RSRVD_ES_AUTHPROVIDER, g_ESCoapCloudConfResource.authProvider);
        OCRepPayloadSetPropString(payload, OC_RSRVD_ES_CISERVER, g_ESCoapCloudConfResource.ciServer);

        if (gWriteUserdataCb)
        {
            gWriteUserdataCb(payload, OC_RSRVD_ES_RES_TYPE_COAPCLOUDCONF);
        }
    }

    if (!strcmp(interface, OC_RSRVD_INTERFACE_BATCH))
    {
        payload = tempPayload;
        OCRepPayloadSetPropObject(payload, OC_RSRVD_REPRESENTATION, repPayload);
    }

    return payload;
}

OCRepPayload* constructResponseOfDevConf(char *interface, ES_BATCH_UPDATE_RESPONSE resp)
{
    if (!strcmp(interface, OC_RSRVD_INTERFACE_BATCH) && RES_EXCLUDE == resp)
    {
        OIC_LOG(DEBUG, ES_RH_TAG, "Excluding DevConf resource from the batch response.");
        return NULL;
    }

    OCRepPayload* payload = OCRepPayloadCreate();
    if (!payload)
    {
        OIC_LOG(ERROR, ES_RH_TAG, "Failed to allocate Payload");
        return NULL;
    }

    if (g_ESDevConfResource.handle == NULL)
    {
        OIC_LOG(ERROR, ES_RH_TAG, "DevConf resource is not created");
        return NULL;
    }

    OIC_LOG(DEBUG, ES_RH_TAG, "constructResponse DevConf res");

    OCRepPayload* repPayload = NULL;
    OCRepPayload* tempPayload = NULL;
    if (!strcmp(interface, OC_RSRVD_INTERFACE_BATCH))
    {
        OCRepPayloadSetUri(payload, OC_RSRVD_ES_URI_DEVCONF);

        repPayload = OCRepPayloadCreate();
        if (!repPayload)
        {
            OIC_LOG(ERROR, ES_RH_TAG, "Failed to allocate Payload");
            return NULL;
        }

        tempPayload = payload;
        payload = repPayload;

        if (RES_INCLUDE == resp)
        {
            size_t interfacesDimensions[MAX_REP_ARRAY_DEPTH] = {2, 0, 0};
            char **interfaces = (char **)OICMalloc(3 * sizeof(char*));
            if (!interfaces)
            {
                OIC_LOG(ERROR, ES_RH_TAG, "Failed to allocate Payload");
                return NULL;
            }

            interfaces[0] = OICStrdup(OC_RSRVD_INTERFACE_DEFAULT);
            interfaces[1] = OICStrdup(OC_RSRVD_INTERFACE_READ);

            OCRepPayloadSetStringArray(payload, OC_RSRVD_ES_INTERFACE, (const char **)interfaces, interfacesDimensions);

            size_t resourceTypesDimensions[MAX_REP_ARRAY_DEPTH] = {1, 0, 0};
            char **resourceTypes = (char **)OICMalloc(2 * sizeof(char*));
            if (!resourceTypes)
            {
                OIC_LOG(ERROR, ES_RH_TAG, "Failed to allocate Payload");
                return NULL;
            }

            resourceTypes[0] = OICStrdup(OC_RSRVD_ES_RES_TYPE_DEVCONF);

            OCRepPayloadSetStringArray(payload, OC_RSRVD_ES_RES_TYPE, (const char **)resourceTypes, resourceTypesDimensions);
        }
    }
    else
    {
        OCRepPayloadAddInterface(payload, OC_RSRVD_INTERFACE_DEFAULT);
        OCRepPayloadAddInterface(payload, OC_RSRVD_INTERFACE_READ);
        OCRepPayloadAddResourceType(payload, OC_RSRVD_ES_RES_TYPE_DEVCONF);
    }

    if (RES_INCLUDE == resp)
    {
        OCRepPayloadSetPropString(payload, OC_RSRVD_ES_DEVNAME, g_ESDevConfResource.devName);

        if (gWriteUserdataCb)
        {
            gWriteUserdataCb(payload, OC_RSRVD_ES_RES_TYPE_DEVCONF);
        }
    }

    if (!strcmp(interface, OC_RSRVD_INTERFACE_BATCH))
    {
        payload = tempPayload;
        OCRepPayloadSetPropObject(payload, OC_RSRVD_REPRESENTATION, repPayload);
    }

    return payload;
}

OCRepPayload* constructResponseOfEasySetup(OCEntityHandlerRequest *ehRequest,
        ES_BATCH_UPDATE_RESPONSE batch_update_rsrc_arr[4])
{
    OCRepPayload* payload = OCRepPayloadCreate();
    if (!payload)
    {
        OIC_LOG(ERROR, ES_RH_TAG, "Failed to allocate Payload");
        return NULL;
    }

    OIC_LOG_V(DEBUG, ES_RH_TAG, "constructResponseOfEasySetup: qry = %s",
            (ehRequest->query) ? (ehRequest->query) : "null");

    // Requested interface is Link list or baseline interface
     if (!ehRequest->query ||
         (ehRequest->query && !strcmp(ehRequest->query, "")) ||
         (ehRequest->query && CompareResourceInterface(ehRequest->query, OC_RSRVD_INTERFACE_LL)) ||
         (ehRequest->query && CompareResourceInterface(ehRequest->query, OC_RSRVD_INTERFACE_DEFAULT)))
     {
        size_t arraySize;
        OCRepPayload **linkArr = OCLinksPayloadArrayCreate(OC_RSRVD_ES_URI_EASYSETUP, ehRequest,
            true, &arraySize);

#ifdef ES_IGNORE_OCLinksPayloadArrayCreate_FAILIURE
        bool linkArrConstructed = true; // TODO: Remove this when
                                        // ES_IGNORE_OCLinksPayloadArrayCreate_FAILIURE is removed.
#endif

        if (!linkArr || (arraySize == 0))
        {
            OIC_LOG(ERROR, ES_RH_TAG, "Failed to create Easy Setup collections ll response.");

#ifdef ES_IGNORE_OCLinksPayloadArrayCreate_FAILIURE
            linkArrConstructed = false;
#else
            OICFree(linkArr);
            OCRepPayloadDestroy(payload);
            return NULL;
#endif
        }
#ifndef ES_IGNORE_OCLinksPayloadArrayCreate_FAILIURE
        else
        {
#endif
            OIC_LOG(DEBUG, ES_RH_TAG, "Constructed links payload.");

            if (!ehRequest->query || (ehRequest->query && !strcmp(ehRequest->query, ""))
                    || (ehRequest->query
                            && CompareResourceInterface(ehRequest->query,
                                    OC_RSRVD_INTERFACE_DEFAULT)))
            {
                OIC_LOG(DEBUG, ES_RH_TAG, "constructResponse EasySetup res (Default interface)");
                OCRepPayloadAddInterface(payload, OC_RSRVD_INTERFACE_DEFAULT);
                OCRepPayloadAddInterface(payload, OC_RSRVD_INTERFACE_LL);
                OCRepPayloadAddInterface(payload, OC_RSRVD_INTERFACE_BATCH);
                OCRepPayloadAddResourceType(payload, OC_RSRVD_ES_RES_TYPE_EASYSETUP);
                OCRepPayloadAddResourceType(payload, OC_RSRVD_ES_RES_TYPE_COL);

                OCRepPayloadSetPropInt(payload, OC_RSRVD_ES_PROVSTATUS,
                        g_ESEasySetupResource.status);
                OCRepPayloadSetPropInt(payload, OC_RSRVD_ES_LAST_ERRORCODE,
                        g_ESEasySetupResource.lastErrCode);

                if (g_ESEasySetupResource.numRequest > 0)
                {
                    size_t dimensions[MAX_REP_ARRAY_DEPTH] = { g_ESEasySetupResource.numRequest, 0,
                            0 };
                    int64_t *connectRequest = (int64_t *) OICMalloc(
                            g_ESEasySetupResource.numRequest * sizeof(int64_t));
                    if (!connectRequest)
                    {
                        OIC_LOG(ERROR, ES_RH_TAG, "Failed to allocate Payload");
                        return NULL;
                    }

                    for (int i = 0; i < g_ESEasySetupResource.numRequest; ++i)
                    {
                        connectRequest[i] = g_ESEasySetupResource.connectRequest[i];
                    }

                    bool b = OCRepPayloadSetIntArrayAsOwner(payload, OC_RSRVD_ES_CONNECT,
                            (int64_t *) connectRequest, dimensions);
                    if (!b)
                    {
                        OIC_LOG(ERROR, ES_RH_TAG, "Failed to set array value for Connect property");
                        OICFree(connectRequest);
                    }
                }
                else
                {
                    OIC_LOG(DEBUG, ES_RH_TAG, "g_ESEasySetupResource.numRequest is 0");
                    size_t dimensions[MAX_REP_ARRAY_DEPTH] = { 0, 0, 0 };
                    OCRepPayloadSetIntArrayAsOwner(payload, OC_RSRVD_ES_CONNECT, NULL, dimensions);
                }

                if (gWriteUserdataCb)
                {
                    gWriteUserdataCb(payload, OC_RSRVD_ES_RES_TYPE_EASYSETUP);
                }

#ifdef ES_IGNORE_OCLinksPayloadArrayCreate_FAILIURE
                if (linkArrConstructed)
                {
#endif
                    size_t dimensions[MAX_REP_ARRAY_DEPTH] = { arraySize, 0, 0 };
                    OCRepPayloadSetPropObjectArray(payload, OC_RSRVD_LINKS,
                            (const OCRepPayload **) linkArr, dimensions);
                    for (size_t i = 0; i < arraySize; ++i)
                    {
                        OCRepPayloadDestroy(linkArr[i]);
                    }
#ifdef ES_IGNORE_OCLinksPayloadArrayCreate_FAILIURE
                }
#endif
            }
            else // link list interface
            {
#ifdef ES_IGNORE_OCLinksPayloadArrayCreate_FAILIURE
                if (linkArrConstructed)
                {
#endif
                    uint16_t acceptVersion = 0;
                    OCPayloadFormat contentFormat; // not used
                    OCGetRequestPayloadVersion(ehRequest, &contentFormat, &acceptVersion);
                    // acceptVersion("OCF-Accept-Content-Format-Version") is present for OCF Request
                    if (acceptVersion != 0)
                    {
                        for (size_t i = 0; i < arraySize - 1; ++i)
                        {
                            linkArr[i]->next = linkArr[i + 1];
                        }
                        // payload is directly the linkArr array, free earlier allocated memory.
                        OCRepPayloadDestroy(payload);
                        payload = linkArr[0];
                    }
                    else // for backward compatibility with OIC
                    {
                        size_t dimensions[MAX_REP_ARRAY_DEPTH] = { arraySize, 0, 0 };
                        OCRepPayloadSetPropObjectArray(payload, OC_RSRVD_LINKS,
                                (const OCRepPayload **) linkArr, dimensions);
                        for (size_t i = 0; i < arraySize; ++i)
                        {
                            OCRepPayloadDestroy(linkArr[i]);
                        }
                    }
#ifdef ES_IGNORE_OCLinksPayloadArrayCreate_FAILIURE
                }
#endif
            }
#ifndef ES_IGNORE_OCLinksPayloadArrayCreate_FAILIURE
        }
#endif
        OICFree(linkArr);
    }
    else if (ehRequest->query
            && CompareResourceInterface(ehRequest->query, OC_RSRVD_INTERFACE_BATCH))
    {
        OIC_LOG(DEBUG, ES_RH_TAG, "constructResponse EasySetup res (Batch Interface)");
        OCRepPayloadSetUri(payload, OC_RSRVD_ES_URI_EASYSETUP);

        OCRepPayload* repPayload = OCRepPayloadCreate();
        if (!repPayload)
        {
            OIC_LOG(ERROR, ES_RH_TAG, "Failed to allocate Payload");
            return NULL;
        }

        if (batch_update_rsrc_arr[EASY_SETUP_RES] == RES_INCLUDE)
        {
            size_t interfacesDimensions[MAX_REP_ARRAY_DEPTH] = { 3, 0, 0 };
            char **interfaces = (char **) OICMalloc(3 * sizeof(char*));
            if (!interfaces)
            {
                OIC_LOG(ERROR, ES_RH_TAG, "Failed to allocate Payload");
                return NULL;
            }

            interfaces[0] = OICStrdup(OC_RSRVD_INTERFACE_DEFAULT);
            interfaces[1] = OICStrdup(OC_RSRVD_INTERFACE_LL);
            interfaces[2] = OICStrdup(OC_RSRVD_INTERFACE_BATCH);

            OCRepPayloadSetStringArray(repPayload, OC_RSRVD_ES_INTERFACE, (const char **) interfaces,
                    interfacesDimensions);

            size_t resourceTypesDimensions[MAX_REP_ARRAY_DEPTH] = { 2, 0, 0 };
            char **resourceTypes = (char **) OICMalloc(2 * sizeof(char*));
            if (!resourceTypes)
            {
                OIC_LOG(ERROR, ES_RH_TAG, "Failed to allocate Payload");
                return NULL;
            }

            resourceTypes[0] = OICStrdup(OC_RSRVD_ES_RES_TYPE_EASYSETUP);
            resourceTypes[1] = OICStrdup(OC_RSRVD_ES_RES_TYPE_COL);

            OCRepPayloadSetStringArray(repPayload, OC_RSRVD_ES_RES_TYPE, (const char **) resourceTypes,
                    resourceTypesDimensions);

            OCRepPayloadSetPropInt(repPayload, OC_RSRVD_ES_PROVSTATUS, g_ESEasySetupResource.status);
            OCRepPayloadSetPropInt(repPayload, OC_RSRVD_ES_LAST_ERRORCODE,
                    g_ESEasySetupResource.lastErrCode);
            if (g_ESEasySetupResource.numRequest > 0)
            {
                size_t dimensions[MAX_REP_ARRAY_DEPTH] = { g_ESEasySetupResource.numRequest, 0, 0 };
                int64_t *connectRequest = (int64_t *) OICMalloc(
                        g_ESEasySetupResource.numRequest * sizeof(int64_t));
                if (!connectRequest)
                {
                    OIC_LOG(ERROR, ES_RH_TAG, "Failed to allocate Payload");
                    return NULL;
                }

                for (int i = 0; i < g_ESEasySetupResource.numRequest; ++i)
                {
                    connectRequest[i] = g_ESEasySetupResource.connectRequest[i];
                }

                bool b = OCRepPayloadSetIntArrayAsOwner(repPayload, OC_RSRVD_ES_CONNECT,
                        (int64_t *) connectRequest, dimensions);
                if (!b)
                {
                    OIC_LOG(ERROR, ES_RH_TAG, "Failed to set array value for Connect property");
                    OICFree(connectRequest);
                }
            }
            else
            {
                OIC_LOG(DEBUG, ES_RH_TAG, "g_ESEasySetupResource.numRequest is 0");
                size_t dimensions[MAX_REP_ARRAY_DEPTH] = { 0, 0, 0 };
                OCRepPayloadSetIntArrayAsOwner(repPayload, OC_RSRVD_ES_CONNECT, NULL, dimensions);
            }

            if (gWriteUserdataCb)
            {
                gWriteUserdataCb(repPayload, OC_RSRVD_ES_RES_TYPE_EASYSETUP);
            }
        }

        OCRepPayloadSetPropObject(payload, OC_RSRVD_REPRESENTATION, repPayload);

        OCRepPayload* head = payload;
        OCRepPayload* nextPayload = NULL;

        nextPayload = constructResponseOfWiFiConf(OC_RSRVD_INTERFACE_BATCH,
                batch_update_rsrc_arr[WIFI_CONF_RES]);
        if (nextPayload != NULL)
        {
            payload->next = nextPayload;
            payload = payload->next;
        }

        nextPayload = constructResponseOfCoapCloudConf(OC_RSRVD_INTERFACE_BATCH,
                batch_update_rsrc_arr[CLOUD_CONF_RES]);
        if (nextPayload != NULL)
        {
            payload->next = nextPayload;
            payload = payload->next;
        }

        nextPayload = constructResponseOfDevConf(OC_RSRVD_INTERFACE_BATCH,
                batch_update_rsrc_arr[DEV_CONF_RES]);
        if (nextPayload != NULL)
        {
            payload->next = nextPayload;
        }

        payload = head;

    }

    return payload;
}


OCStackResult CreateEasySetupResources(bool isSecured, ESResourceMask resourceMask)
{
    OCStackResult res = OC_STACK_ERROR;
    bool maskFlag = false;

    res = initEasySetupResource(isSecured);
    if (res != OC_STACK_OK)
    {
        // TODO: destroy logic will be added
        OIC_LOG_V(ERROR, ES_RH_TAG, "initEasySetupResource result: %s", getResult(res));

        return res;
    }

    if ((resourceMask & ES_WIFICONF_RESOURCE) == ES_WIFICONF_RESOURCE)
    {
        maskFlag = true;
        res = initWiFiConfResource(isSecured);
        if (res != OC_STACK_OK)
        {
            OIC_LOG_V(ERROR, ES_RH_TAG, "initWiFiConfResource result: %s", getResult(res));
            return res;
        }

        res = OCBindResource(g_ESEasySetupResource.handle, g_ESWiFiConfResource.handle);
        if (res != OC_STACK_OK)
        {
            OIC_LOG_V(ERROR, ES_RH_TAG, "Bind WiFiConfResource result: %s", getResult(res));
            return res;
        }
    }

    if ((resourceMask & ES_COAPCLOUDCONF_RESOURCE) == ES_COAPCLOUDCONF_RESOURCE)
    {
        maskFlag = true;
        res = initCoapCloudConfResource(isSecured);
        if (res != OC_STACK_OK)
        {
            OIC_LOG_V(ERROR, ES_RH_TAG, "initCoapCloudConfResource result: %s", getResult(res));
            return res;
        }

        res = OCBindResource(g_ESEasySetupResource.handle, g_ESCoapCloudConfResource.handle);
        if (res != OC_STACK_OK)
        {
            OIC_LOG_V(ERROR, ES_RH_TAG, "Bind CoapCloudConfResource result: %s", getResult(res));
            return res;
        }
    }

    if ((resourceMask & ES_DEVCONF_RESOURCE) == ES_DEVCONF_RESOURCE)
    {
        maskFlag = true;
        res = initDevConfResource(isSecured);
        if (res != OC_STACK_OK)
        {
            OIC_LOG_V(ERROR, ES_RH_TAG, "initDevConf result: %s", getResult(res));
            return res;
        }

        res = OCBindResource(g_ESEasySetupResource.handle, g_ESDevConfResource.handle);
        if (res != OC_STACK_OK)
        {
            OIC_LOG_V(ERROR, ES_RH_TAG, "Bind DevConfResource result: %s", getResult(res));
            return res;
        }
    }

    if (maskFlag == false)
    {
        OIC_LOG_V(ERROR, ES_RH_TAG, "Invalid ResourceMask");
        return OC_STACK_ERROR;

    }

    OIC_LOG_V(DEBUG, ES_RH_TAG, "Created all resources with result: %s", getResult(res));

    return res;
}

OCStackResult DeleteEasySetupResources(void)
{
    OCStackResult res = OC_STACK_ERROR;
    if (g_ESWiFiConfResource.handle != NULL)
    {
        res = OCUnBindResource(g_ESEasySetupResource.handle, g_ESWiFiConfResource.handle);
        if (res != OC_STACK_OK)
        {
            OIC_LOG_V(ERROR, ES_RH_TAG, "Unbind WiFi resource error with result: %s", getResult(res));
        }
    }
    if (g_ESCoapCloudConfResource.handle != NULL)
    {
        res = OCUnBindResource(g_ESEasySetupResource.handle, g_ESCoapCloudConfResource.handle);
        if (res != OC_STACK_OK)
        {
            OIC_LOG_V(ERROR, ES_RH_TAG, "Unbind CloudServer resource error with result: %s", getResult(res));
        }
    }
    if (g_ESDevConfResource.handle != NULL)
    {
        res = OCUnBindResource(g_ESEasySetupResource.handle, g_ESDevConfResource.handle);
        if (res != OC_STACK_OK)
        {
            OIC_LOG_V(ERROR, ES_RH_TAG, "Unbind DevConf resource error with result: %s", getResult(res));
        }
    }

    if (g_ESWiFiConfResource.handle != NULL)
    {
        res = OCDeleteResource(g_ESWiFiConfResource.handle);
        if (res != OC_STACK_OK)
        {
            OIC_LOG_V(ERROR, ES_RH_TAG, "Deleting WiFi resource error with result: %s", getResult(res));
        }
    }

    if (g_ESCoapCloudConfResource.handle != NULL)
    {
        res = OCDeleteResource(g_ESCoapCloudConfResource.handle);
        if (res != OC_STACK_OK)
        {
            OIC_LOG_V(ERROR, ES_RH_TAG, "Deleting CloudServer resource error with result: %s", getResult(res));
        }
    }

    if (g_ESDevConfResource.handle != NULL)
    {
        res = OCDeleteResource(g_ESDevConfResource.handle);
        if (res != OC_STACK_OK)
        {
            OIC_LOG_V(ERROR, ES_RH_TAG, "Deleting DevConf resource error with result: %s", getResult(res));
        }
    }

    if (g_ESEasySetupResource.handle != NULL)
    {
        res = OCDeleteResource(g_ESEasySetupResource.handle);
        if (res != OC_STACK_OK)
        {
            OIC_LOG_V(ERROR, ES_RH_TAG, "Deleting Prov resource error with result: %s", getResult(res));
        }
    }

    return res;
}

static bool isValidESResourceHandle(OCResourceHandle handle)
{
    return ((handle == g_ESEasySetupResource.handle) || (handle == g_ESWiFiConfResource.handle)
            || (handle == g_ESCoapCloudConfResource.handle)
            || (handle == g_ESDevConfResource.handle));
}

OCEntityHandlerResult ProcessGetRequest(OCEntityHandlerRequest *ehRequest, OCRepPayload **payload)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    if (!ehRequest)
    {
        OIC_LOG(ERROR, ES_RH_TAG, "Request is Null");
        return ehResult;
    }
    if (ehRequest->payload && ehRequest->payload->type != PAYLOAD_TYPE_REPRESENTATION)
    {
        OIC_LOG(ERROR, ES_RH_TAG, "Incoming payload not a representation");
        return ehResult;
    }
    if (!isValidESResourceHandle(ehRequest->resource))
    {
        OIC_LOG(ERROR, ES_RH_TAG, "Request does not have a valid Easy Setup Resource handle");
        return ehResult;
    }
    if (CheckEhRequestPayload(ehRequest) != OC_EH_OK)
    {
        OIC_LOG(ERROR, ES_RH_TAG, "Not supported Interface");
        return OC_EH_BAD_REQ;
    }

    OCRepPayload *getResp = NULL;
    *payload = NULL;

    char *iface_name = NULL;
    GetInterfaceNameFromQuery(ehRequest->query, &iface_name);
    if (!iface_name)
    {
        iface_name = OICStrdup(OC_RSRVD_INTERFACE_DEFAULT);
    }

    ES_BATCH_UPDATE_RESPONSE batch_update_rsrc_arr[4] = { RES_INCLUDE, RES_INCLUDE, RES_INCLUDE, RES_INCLUDE };

    if (ehRequest->resource == g_ESEasySetupResource.handle)
    {
            getResp = constructResponseOfEasySetup(ehRequest, batch_update_rsrc_arr);
    }
    else if (ehRequest->resource == g_ESWiFiConfResource.handle)
    {
            getResp = constructResponseOfWiFiConf(iface_name, RES_INCLUDE);
    }
    else if (ehRequest->resource == g_ESCoapCloudConfResource.handle)
    {
            getResp = constructResponseOfCoapCloudConf(iface_name, RES_INCLUDE);
    }
    else if (ehRequest->resource == g_ESDevConfResource.handle)
    {
            getResp = constructResponseOfDevConf(iface_name, RES_INCLUDE);
    }

    OICFree(iface_name);

    if (!getResp)
    {
        OIC_LOG(ERROR, ES_RH_TAG, "constructResponse failed");
        return OC_EH_ERROR;
    }

    *payload = getResp;
    ehResult = OC_EH_OK;

    return ehResult;
}

OCEntityHandlerResult ProcessPostRequest(OCEntityHandlerRequest *ehRequest, OCRepPayload** payload)
{
    OIC_LOG(DEBUG, ES_RH_TAG, "ProcessPostRequest enter");

    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    if (ehRequest->payload && ehRequest->payload->type != PAYLOAD_TYPE_REPRESENTATION)
    {
        OIC_LOG(ERROR, ES_RH_TAG, "Incoming payload not a representation");
        return ehResult;
    }

    OCRepPayload* input = (OCRepPayload*) (ehRequest->payload);
    if (!input)
    {
        OIC_LOG(ERROR, ES_RH_TAG, "Failed to parse");
        return ehResult;
    }

    OIC_LOG_PAYLOAD(DEBUG, (OCPayload *)input);

    ES_BATCH_UPDATE_RESPONSE batch_update_rsrc_arr[4] = { 0 };

    if (ehRequest->resource == g_ESEasySetupResource.handle)
    {
        if (ehRequest->query &&
            strcmp(ehRequest->query, "") &&
            !CompareResourceInterface(ehRequest->query, OC_RSRVD_INTERFACE_BATCH) &&
            !CompareResourceInterface(ehRequest->query, OC_RSRVD_INTERFACE_DEFAULT))
        {
            OIC_LOG(ERROR, ES_RH_TAG, "Not supported Interface");
            return OC_EH_BAD_REQ;
        }
        else
        {
            ehResult = updateEasySetupResource(ehRequest, input, batch_update_rsrc_arr);
            if (ehResult != OC_EH_OK)
            {
                OIC_LOG(ERROR, ES_RH_TAG, "Failed to update EasySetup resource.");
            }
        }
    }
    else if (ehRequest->resource == g_ESWiFiConfResource.handle)
    {
        if (CheckEhRequestPayload(ehRequest) != OC_EH_OK)
        {
            OIC_LOG(ERROR, ES_RH_TAG, "Not supported Interface");
            return OC_EH_BAD_REQ;
        }
        else
        {
            if (updateWiFiConfResource(input) != OC_EH_OK)
            {
                OIC_LOG(ERROR, ES_RH_TAG, "Failed to update WifiConf resource.");
                return OC_EH_BAD_REQ;
            }
        }
    }
    else if (ehRequest->resource == g_ESCoapCloudConfResource.handle)
    {
        if (CheckEhRequestPayload(ehRequest) != OC_EH_OK)
        {
            OIC_LOG(ERROR, ES_RH_TAG, "Not supported Interface");
            return OC_EH_BAD_REQ;
        }
        else
        {
            updateCoapCloudConfResource(input);
        }
    }
    else if (ehRequest->resource == g_ESDevConfResource.handle)
    {
        // Reject POST request for DevConf resource as it has only a single read-only property.
        // TODO As DevConf also supports user-defined properties, we need to find another way
        // to reject the POST requests based on the presence of read-only properties in the payload.
        OIC_LOG(ERROR, ES_RH_TAG, "Failed to update DevConf resource.");
        return OC_EH_BAD_REQ;
    }

    OCRepPayload *getResp = NULL;
    if (ehRequest->resource == g_ESEasySetupResource.handle)
    {
        getResp = constructResponseOfEasySetup(ehRequest, batch_update_rsrc_arr);
    }
    else if (ehRequest->resource == g_ESWiFiConfResource.handle)
    {
        getResp = constructResponseOfWiFiConf(OC_RSRVD_INTERFACE_DEFAULT, RES_INCLUDE);
    }
    else if (ehRequest->resource == g_ESCoapCloudConfResource.handle)
    {
        getResp = constructResponseOfCoapCloudConf(OC_RSRVD_INTERFACE_DEFAULT, RES_INCLUDE);
    }
    else if (ehRequest->resource == g_ESDevConfResource.handle)
    {
        getResp = constructResponseOfDevConf(OC_RSRVD_INTERFACE_DEFAULT, RES_INCLUDE);
    }

    if (!getResp)
    {
        OIC_LOG(ERROR, ES_RH_TAG, "constructResponse failed");
        return OC_EH_ERROR;
    }

    *payload = getResp;
    if (ehRequest->resource != g_ESEasySetupResource.handle)
    {
        ehResult = OC_EH_OK;
    }

    return ehResult;
}

OCEntityHandlerResult ProcessPutRequest(OCEntityHandlerRequest * ehRequest,
        OCRepPayload** payload)
{
    (void) ehRequest;
    (void) payload;
    OCEntityHandlerResult ehResult = OC_EH_BAD_REQ;

    return ehResult;
}

/**
 * This is the entity handler for the registered resource.
 * This is invoked by OCStack whenever it recevies a request for this resource.
 */
OCEntityHandlerResult OCEntityHandlerCb(OCEntityHandlerFlag flag,
        OCEntityHandlerRequest* entityHandlerRequest, void *callback)
{
    (void) callback;
    OCEntityHandlerResult ehRet = OC_EH_OK;
    OCEntityHandlerResponse response =
    { 0, 0, OC_EH_ERROR, 0, 0,
    { },
    { 0 }, false };
    OCRepPayload* payload = NULL;

    if (entityHandlerRequest && (flag & OC_REQUEST_FLAG))
    {
        if (OC_REST_GET == entityHandlerRequest->method)
        {
            OIC_LOG(DEBUG, ES_RH_TAG, "Received GET request");
            ehRet = ProcessGetRequest(entityHandlerRequest, &payload);
        }
        else if (OC_REST_PUT == entityHandlerRequest->method)
        {
            OIC_LOG(DEBUG, ES_RH_TAG, "Received PUT request");

            //PUT request will be handled in the internal implementation
            if (g_ESEasySetupResource.handle != NULL)
            {
                ehRet = ProcessPutRequest(entityHandlerRequest, &payload);
            }
            else
            {
                OIC_LOG(ERROR, ES_RH_TAG, "Cannot process put");
                ehRet = OC_EH_ERROR;
            }
        }
        else if (OC_REST_POST == entityHandlerRequest->method)
        {
            OIC_LOG(DEBUG, ES_RH_TAG, "Received OC_REST_POST from client");
            if (g_ESEasySetupResource.handle != NULL)
            {
                ehRet = ProcessPostRequest(entityHandlerRequest, &payload);
            }
            else
            {
                OIC_LOG(ERROR, ES_RH_TAG, "Cannot process put");
                ehRet = OC_EH_ERROR;
            }
        }

        // Format the response.  Note this requires some info about the request
        response.requestHandle = entityHandlerRequest->requestHandle;
        response.ehResult = ehRet;
        //response uses OCPaylod while all get,put methodes use OCRepPayload
        response.payload = (OCPayload*) (payload);
        response.numSendVendorSpecificHeaderOptions = 0;
        memset(response.sendVendorSpecificHeaderOptions, 0,
                sizeof(response.sendVendorSpecificHeaderOptions));
        memset(response.resourceUri, 0, sizeof(response.resourceUri));
        // Indicate that response is NOT in a persistent buffer
        response.persistentBufferFlag = 0;

        // Send the response
        if (OCDoResponse(&response) != OC_STACK_OK)
        {
            OIC_LOG(ERROR, ES_RH_TAG, "Error sending response");
            ehRet = OC_EH_ERROR;
        }
    }
    if (entityHandlerRequest && (flag & OC_OBSERVE_FLAG))
    {
        OIC_LOG(DEBUG, ES_RH_TAG, "Flag includes OC_OBSERVE_FLAG");

        if (OC_OBSERVE_REGISTER == entityHandlerRequest->obsInfo.action)
        {
            OIC_LOG(DEBUG, ES_RH_TAG, "Received OC_OBSERVE_REGISTER from Mediator");
        }
        else if (OC_OBSERVE_DEREGISTER == entityHandlerRequest->obsInfo.action)
        {
            OIC_LOG(DEBUG, ES_RH_TAG, "Received OC_OBSERVE_DEREGISTER from Mediator");
        }
    }
    return ehRet;
}

OCStackResult SetDeviceProperty(ESDeviceProperty *deviceProperty)
{
    OIC_LOG(DEBUG, ES_RH_TAG, "SetDeviceProperty IN");

    if (deviceProperty->WiFi.numSupportedMode > NUM_WIFIMODE
            || deviceProperty->WiFi.numSupportedFreq > NUM_WIFIFREQ
            || deviceProperty->WiFi.numSupportedAuthType > NUM_WIFIAUTHTYPE
            || deviceProperty->WiFi.numSupportedEncType > NUM_WIFIENCTYPE)
    {
        OIC_LOG(ERROR, ES_RH_TAG, "SetDeviceProperty: Invalid Input Param");
        return OC_STACK_INVALID_PARAM;
    }

    g_ESWiFiConfResource.numSupportedFreq = deviceProperty->WiFi.numSupportedFreq;
    for (uint8_t i = 0; i < g_ESWiFiConfResource.numSupportedFreq; ++i)
    {
        g_ESWiFiConfResource.supportedFreq[i] = (deviceProperty->WiFi).supportedFreq[i];
        OIC_LOG_V(INFO_PRIVATE, ES_RH_TAG, "WiFi Freq : %d", g_ESWiFiConfResource.supportedFreq[i]);
    }

    g_ESWiFiConfResource.numMode = deviceProperty->WiFi.numSupportedMode;
    for (uint8_t i = 0; i < g_ESWiFiConfResource.numMode; ++i)
    {
        g_ESWiFiConfResource.supportedMode[i] = (deviceProperty->WiFi).supportedMode[i];
        OIC_LOG_V(INFO_PRIVATE, ES_RH_TAG, "WiFi Mode : %d", g_ESWiFiConfResource.supportedMode[i]);
    }

    g_ESWiFiConfResource.numSupportedAuthType = deviceProperty->WiFi.numSupportedAuthType;
    for (uint8_t i = 0; i < g_ESWiFiConfResource.numSupportedAuthType; ++i)
    {
        g_ESWiFiConfResource.supportedAuthType[i] = (deviceProperty->WiFi).supportedAuthType[i];
        OIC_LOG_V(INFO_PRIVATE, ES_RH_TAG, "WiFi Auth Type : %d",
                g_ESWiFiConfResource.supportedAuthType[i]);
    }

    g_ESWiFiConfResource.numSupportedEncType = deviceProperty->WiFi.numSupportedEncType;
    for (uint8_t i = 0; i < g_ESWiFiConfResource.numSupportedEncType; ++i)
    {
        g_ESWiFiConfResource.supportedEncType[i] = (deviceProperty->WiFi).supportedEncType[i];
        OIC_LOG_V(INFO_PRIVATE, ES_RH_TAG, "WiFi Enc Type : %d",
                g_ESWiFiConfResource.supportedEncType[i]);
    }

    OICStrcpy(g_ESDevConfResource.devName, OIC_STRING_MAX_VALUE,
            (deviceProperty->DevConf).deviceName);
    OIC_LOG_V(INFO_PRIVATE, ES_RH_TAG, "Device Name : %s", g_ESDevConfResource.devName);

    if (OC_STACK_NO_OBSERVERS == OCNotifyAllObservers(g_ESWiFiConfResource.handle, OC_HIGH_QOS))
    {
        OIC_LOG(DEBUG, ES_RH_TAG, "wifiResource doesn't have any observers.");
    }

    if (OC_STACK_NO_OBSERVERS == OCNotifyAllObservers(g_ESDevConfResource.handle, OC_HIGH_QOS))
    {
        OIC_LOG(DEBUG, ES_RH_TAG, "devConfResource doesn't have any observers.");
    }

    OIC_LOG(DEBUG, ES_RH_TAG, "SetDeviceProperty OUT");
    return OC_STACK_OK;
}

OCStackResult SetEnrolleeState(ESEnrolleeState esState)
{
    OIC_LOG(DEBUG, ES_RH_TAG, "SetEnrolleeState IN");

    g_ESEasySetupResource.status = esState;
    OIC_LOG_V(INFO_PRIVATE, ES_RH_TAG, "Enrollee Status : %d", g_ESEasySetupResource.status);

    if (OC_STACK_NO_OBSERVERS == OCNotifyAllObservers(g_ESEasySetupResource.handle, OC_HIGH_QOS))
    {
        OIC_LOG(DEBUG, ES_RH_TAG, "provResource doesn't have any observers.");
    }

    OIC_LOG(DEBUG, ES_RH_TAG, "SetEnrolleeState OUT");
    return OC_STACK_OK;
}

OCStackResult SetEnrolleeErrCode(ESErrorCode esErrCode)
{
    OIC_LOG(DEBUG, ES_RH_TAG, "SetEnrolleeErrCode IN");

    g_ESEasySetupResource.lastErrCode = esErrCode;
    OIC_LOG_V(INFO_PRIVATE, ES_RH_TAG, "Enrollee ErrorCode : %d", g_ESEasySetupResource.lastErrCode);

    if (OC_STACK_NO_OBSERVERS == OCNotifyAllObservers(g_ESEasySetupResource.handle, OC_HIGH_QOS))
    {
        OIC_LOG(DEBUG, ES_RH_TAG, "provResource doesn't have any observers.");
    }

    OIC_LOG(DEBUG, ES_RH_TAG, "SetEnrolleeErrCode OUT");
    return OC_STACK_OK;
}

OCEntityHandlerResult CheckEhRequestPayload(OCEntityHandlerRequest *ehRequest)
{
    if (ehRequest->query && strcmp(ehRequest->query, ""))
    {
        uint8_t numResourceInterfaces = 0;
        OCStackResult res = OCGetNumberOfResourceInterfaces(ehRequest->resource,
                &numResourceInterfaces);
        if (res != OC_STACK_OK)
        {
            OIC_LOG_V(ERROR, ES_RH_TAG, "Unable to get Number of Interfaces: %s", getResult(res));
            return OC_EH_ERROR;
        }
        for (uint8_t i = 0; i < numResourceInterfaces; ++i)
        {
            const char *interfaceName = OCGetResourceInterfaceName(ehRequest->resource, i);
            if (CompareResourceInterface(ehRequest->query, interfaceName))
            {
                return OC_EH_OK;
            }
        }
        OIC_LOG(ERROR, ES_RH_TAG, "Not supported Interface");
        return OC_EH_BAD_REQ;
    }
    return OC_EH_OK;
}

bool isAuthTypeSupported(WIFI_AUTHTYPE authType)
{
    bool supported = false;
    for (uint8_t i = 0; i < g_ESWiFiConfResource.numSupportedAuthType; ++i)
    {
        if (g_ESWiFiConfResource.supportedAuthType[i] == authType)
        {
            supported = true;
            break;
        }
    }
    return supported;
}

bool isEncTypeSupported(WIFI_ENCTYPE encType)
{
    bool supported = false;
    for (uint8_t i = 0; i < g_ESWiFiConfResource.numSupportedEncType; ++i)
    {
        if (g_ESWiFiConfResource.supportedEncType[i] == encType)
        {
            supported = true;
            break;
        }
    }
    return supported;
}

const char *getResult(OCStackResult result)
{
    switch (result)
    {
        case OC_STACK_OK:
            return "OC_STACK_OK";
        case OC_STACK_INVALID_URI:
            return "OC_STACK_INVALID_URI";
        case OC_STACK_INVALID_QUERY:
            return "OC_STACK_INVALID_QUERY";
        case OC_STACK_INVALID_IP:
            return "OC_STACK_INVALID_IP";
        case OC_STACK_INVALID_PORT:
            return "OC_STACK_INVALID_PORT";
        case OC_STACK_INVALID_CALLBACK:
            return "OC_STACK_INVALID_CALLBACK";
        case OC_STACK_INVALID_METHOD:
            return "OC_STACK_INVALID_METHOD";
        case OC_STACK_NO_MEMORY:
            return "OC_STACK_NO_MEMORY";
        case OC_STACK_COMM_ERROR:
            return "OC_STACK_COMM_ERROR";
        case OC_STACK_INVALID_PARAM:
            return "OC_STACK_INVALID_PARAM";
        case OC_STACK_NOTIMPL:
            return "OC_STACK_NOTIMPL";
        case OC_STACK_NO_RESOURCE:
            return "OC_STACK_NO_RESOURCE";
        case OC_STACK_RESOURCE_ERROR:
            return "OC_STACK_RESOURCE_ERROR";
        case OC_STACK_SLOW_RESOURCE:
            return "OC_STACK_SLOW_RESOURCE";
        case OC_STACK_NO_OBSERVERS:
            return "OC_STACK_NO_OBSERVERS";
        case OC_STACK_ERROR:
            return "OC_STACK_ERROR";
        default:
            return "UNKNOWN";
    }
}
