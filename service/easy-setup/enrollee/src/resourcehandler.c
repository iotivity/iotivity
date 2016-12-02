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

#include "ocpayload.h"
#include "oic_string.h"
#include "oic_malloc.h"
#include "cautilinterface.h"

/**
 * @var ES_RH_TAG
 * @brief Logging tag for module name.
 */
#define ES_RH_TAG "ES_RH"
//-----------------------------------------------------------------------------
// Private variables
//-----------------------------------------------------------------------------

/**
 * @var gProvResource
 * @brief Structure for holding the Provisioning status and target information required to
 * connect to the target network
 */
static ProvResource gProvResource;
static WiFiResource gWiFiResource;
static CloudResource gCloudResource;
static DevConfResource gDevConfResource;

//-----------------------------------------------------------------------------
// Private internal function prototypes
//-----------------------------------------------------------------------------
OCEntityHandlerResult OCEntityHandlerCb(OCEntityHandlerFlag flag, OCEntityHandlerRequest *ehRequest,
        void *callback);
OCEntityHandlerResult ProcessGetRequest(OCEntityHandlerRequest *ehRequest, OCRepPayload** payload);
OCEntityHandlerResult ProcessPutRequest(OCEntityHandlerRequest *ehRequest, OCRepPayload** payload);
OCEntityHandlerResult ProcessPostRequest(OCEntityHandlerRequest *ehRequest, OCRepPayload** payload);
void updateProvResource(OCEntityHandlerRequest* ehRequest, OCRepPayload* input);
void updateWiFiResource(OCRepPayload* input);
void updateCloudResource(OCRepPayload* input);
void updateDevConfResource(OCRepPayload* input);
const char *getResult(OCStackResult result);

ESWiFiCB gWifiRsrcEvtCb = NULL;
ESCloudCB gCloudRsrcEvtCb = NULL;
ESDevConfCB gDevConfRsrcEvtCb = NULL;

ESReadUserdataCb gReadUserdataCb = NULL;
ESWriteUserdataCb gWriteUserdataCb = NULL;

bool CompareResourceInterface(char *from, char *iface)
{
    char *str = OICStrdup(from);
    char *ptr = strtok(str, ";");

    if(ptr == NULL)
    {
        return false;
    }

    do
    {
        if(strstr(ptr, ".if."))
        {
            char *if_ptr = NULL;
            if_ptr = strtok(ptr, "=");
            if_ptr = strtok(NULL, "=");

            if(!strcmp(if_ptr, iface))
            {
                return true;
            }
        }

    } while ((ptr = strtok(NULL, ";")));

    return false;
}

ESResult SetCallbackForUserData(ESReadUserdataCb readCb, ESWriteUserdataCb writeCb)
{
    if(!readCb && !writeCb)
    {
        OIC_LOG(INFO, ES_RH_TAG, "Both of callbacks for user data are null");
        return ES_ERROR;
    }
    gReadUserdataCb = readCb;
    gWriteUserdataCb = writeCb;
    return ES_OK;
}

void RegisterWifiRsrcEventCallBack(ESWiFiCB cb)
{
    gWifiRsrcEvtCb = cb;
}

void RegisterCloudRsrcEventCallBack(ESCloudCB cb)
{
    gCloudRsrcEvtCb = cb;
}

void RegisterDevConfRsrcEventCallBack(ESDevConfCB cb)
{
    gDevConfRsrcEvtCb = cb;
}

void UnRegisterResourceEventCallBack()
{
    if (gWifiRsrcEvtCb)
    {
        gWifiRsrcEvtCb = NULL;
    }
    if (gCloudRsrcEvtCb)
    {
        gCloudRsrcEvtCb = NULL;
    }
    if (gDevConfRsrcEvtCb)
    {
        gDevConfRsrcEvtCb = NULL;
    }
}

OCStackResult initProvResource(bool isSecured)
{
    gProvResource.status = ES_STATE_INIT;
    gProvResource.lastErrCode = ES_ERRCODE_NO_ERROR;

    OCStackResult res = OC_STACK_ERROR;
    if (isSecured)
    {
        res = OCCreateResource(&gProvResource.handle, OC_RSRVD_ES_RES_TYPE_PROV,
        OC_RSRVD_INTERFACE_DEFAULT,
        OC_RSRVD_ES_URI_PROV, OCEntityHandlerCb,
        NULL, OC_DISCOVERABLE | OC_OBSERVABLE | OC_SECURE);
    }else
    {
        res = OCCreateResource(&gProvResource.handle, OC_RSRVD_ES_RES_TYPE_PROV,
        OC_RSRVD_INTERFACE_DEFAULT,
        OC_RSRVD_ES_URI_PROV, OCEntityHandlerCb,
        NULL, OC_DISCOVERABLE | OC_OBSERVABLE);
    }
    if(res)
    {
        OIC_LOG_V(INFO, ES_RH_TAG, "Created Prov resource with result: %s", getResult(res));
        return res;
    }

    res = OCBindResourceTypeToResource(gProvResource.handle, OC_RSRVD_ES_RES_TYPE_COL);
    if(res)
    {
        OIC_LOG_V(INFO, ES_RH_TAG, "Binding Resource type with result: %s", getResult(res));
        return res;
    }

    res = OCBindResourceInterfaceToResource(gProvResource.handle, OC_RSRVD_INTERFACE_LL);
    if(res)
    {
        OIC_LOG_V(INFO, ES_RH_TAG, "Binding Resource interface with result: %s", getResult(res));
        return res;
    }
    res = OCBindResourceInterfaceToResource(gProvResource.handle, OC_RSRVD_INTERFACE_BATCH);
    if(res)
    {
        OIC_LOG_V(INFO, ES_RH_TAG, "Binding Resource interface with result: %s", getResult(res));
        return res;
    }

    OIC_LOG_V(INFO, ES_RH_TAG, "Created Prov resource with result: %s", getResult(res));
    return res;
}

OCStackResult initWiFiResource(bool isSecured)
{
    OCStackResult res = OC_STACK_ERROR;

    gWiFiResource.supportedFreq = WIFI_BOTH;
    gWiFiResource.supportedMode[0] = WIFI_11A;
    gWiFiResource.supportedMode[1] = WIFI_11B;
    gWiFiResource.supportedMode[2] = WIFI_11G;
    gWiFiResource.supportedMode[3] = WIFI_11N;
    gWiFiResource.numMode = 4;
    gWiFiResource.authType = NONE_AUTH;
    gWiFiResource.encType = NONE_ENC;
    OICStrcpy(gWiFiResource.ssid, sizeof(gWiFiResource.ssid), "");
    OICStrcpy(gWiFiResource.cred, sizeof(gWiFiResource.cred), "");

    if (isSecured)
    {
        res = OCCreateResource(&gWiFiResource.handle, OC_RSRVD_ES_RES_TYPE_WIFI,
        OC_RSRVD_INTERFACE_DEFAULT,
        OC_RSRVD_ES_URI_WIFI, OCEntityHandlerCb,
        NULL, OC_DISCOVERABLE | OC_OBSERVABLE | OC_SECURE);
    }else
    {
        res = OCCreateResource(&gWiFiResource.handle, OC_RSRVD_ES_RES_TYPE_WIFI,
        OC_RSRVD_INTERFACE_DEFAULT,
        OC_RSRVD_ES_URI_WIFI, OCEntityHandlerCb,
        NULL, OC_DISCOVERABLE | OC_OBSERVABLE);
    }

    OIC_LOG_V(INFO, ES_RH_TAG, "Created WiFi resource with result: %s", getResult(res));
    return res;

}

OCStackResult initCloudServerResource(bool isSecured)
{
    OCStackResult res = OC_STACK_ERROR;

    OICStrcpy(gCloudResource.authCode, sizeof(gCloudResource.authCode), "");
    OICStrcpy(gCloudResource.authProvider, sizeof(gCloudResource.authProvider), "");
    OICStrcpy(gCloudResource.ciServer, sizeof(gCloudResource.ciServer), "");

    if (isSecured)
    {
        res = OCCreateResource(&gCloudResource.handle, OC_RSRVD_ES_RES_TYPE_CLOUDSERVER,
        OC_RSRVD_INTERFACE_DEFAULT,
        OC_RSRVD_ES_URI_CLOUDSERVER, OCEntityHandlerCb,
        NULL, OC_DISCOVERABLE | OC_OBSERVABLE | OC_SECURE);
    }else
    {
        res = OCCreateResource(&gCloudResource.handle, OC_RSRVD_ES_RES_TYPE_CLOUDSERVER,
        OC_RSRVD_INTERFACE_DEFAULT,
        OC_RSRVD_ES_URI_CLOUDSERVER, OCEntityHandlerCb,
        NULL, OC_DISCOVERABLE | OC_OBSERVABLE);
    }

    OIC_LOG_V(INFO, ES_RH_TAG, "Created CloudServer resource with result: %s", getResult(res));
    return res;

}

OCStackResult initDevConfResource(bool isSecured)
{
    OCStackResult res = OC_STACK_ERROR;

    OICStrcpy(gDevConfResource.devName, sizeof(gDevConfResource.devName), "");
    OICStrcpy(gDevConfResource.modelNumber, sizeof(gDevConfResource.modelNumber), "");
    OICStrcpy(gDevConfResource.location, sizeof(gDevConfResource.location), "");
    OICStrcpy(gDevConfResource.country, sizeof(gDevConfResource.country), "");
    OICStrcpy(gDevConfResource.language, sizeof(gDevConfResource.language), "");

    if (isSecured)
    {
        res = OCCreateResource(&gDevConfResource.handle, OC_RSRVD_ES_RES_TYPE_DEVCONF,
        OC_RSRVD_INTERFACE_DEFAULT,
        OC_RSRVD_ES_URI_DEVCONF, OCEntityHandlerCb,
        NULL, OC_DISCOVERABLE | OC_OBSERVABLE | OC_SECURE);
    }else
    {
        res = OCCreateResource(&gDevConfResource.handle, OC_RSRVD_ES_RES_TYPE_DEVCONF,
        OC_RSRVD_INTERFACE_DEFAULT,
        OC_RSRVD_ES_URI_DEVCONF, OCEntityHandlerCb,
        NULL, OC_DISCOVERABLE | OC_OBSERVABLE);
    }

    OIC_LOG_V(INFO, ES_RH_TAG, "Created DevConf resource with result: %s", getResult(res));
    return res;

}

void updateProvResource(OCEntityHandlerRequest* ehRequest, OCRepPayload* input)
{
    OIC_LOG_V(INFO, ES_RH_TAG, "gProvResource.status %d", gProvResource.status);

    if(ehRequest->query)
    {
        if(CompareResourceInterface(ehRequest->query, OC_RSRVD_INTERFACE_BATCH))
        {
            // When Provisioning resource has a POST with BatchInterface
            updateCloudResource(input);
            updateWiFiResource(input);
            updateDevConfResource(input);
        }
    }
}

void updateWiFiResource(OCRepPayload* input)
{
    ESWiFiProvData* wiFiData = (ESWiFiProvData*)OICMalloc(sizeof(ESWiFiProvData));

    if(wiFiData == NULL)
    {
        OIC_LOG(DEBUG, ES_RH_TAG, "OICMalloc is failed");
        return ;
    }

    memset(wiFiData->ssid, 0, OIC_STRING_MAX_VALUE);
    memset(wiFiData->pwd, 0, OIC_STRING_MAX_VALUE);
    wiFiData->authtype = NONE_AUTH;
    wiFiData->enctype = NONE_AUTH;
    wiFiData->userdata = NULL;

    char* ssid = NULL;
    if (OCRepPayloadGetPropString(input, OC_RSRVD_ES_SSID, &ssid))
    {
        OICStrcpy(gWiFiResource.ssid, sizeof(gWiFiResource.ssid), ssid);
        OICStrcpy(wiFiData->ssid, sizeof(wiFiData->ssid), ssid);
        OIC_LOG_V(INFO, ES_RH_TAG, "gWiFiResource.ssid : %s", gWiFiResource.ssid);
    }

    char* cred = NULL;
    if (OCRepPayloadGetPropString(input, OC_RSRVD_ES_CRED, &cred))
    {
        OICStrcpy(gWiFiResource.cred, sizeof(gWiFiResource.cred), cred);
        OICStrcpy(wiFiData->pwd, sizeof(wiFiData->pwd), cred);
        OIC_LOG_V(INFO, ES_RH_TAG, "gWiFiResource.cred %s", gWiFiResource.cred);
    }

    int64_t authType = -1;
    if (OCRepPayloadGetPropInt(input, OC_RSRVD_ES_AUTHTYPE, &authType))
    {
        gWiFiResource.authType = authType;
        wiFiData->authtype = gWiFiResource.authType;
        OIC_LOG_V(INFO, ES_RH_TAG, "gWiFiResource.authType %u", gWiFiResource.authType);
    }

    int64_t encType = -1;
    if (OCRepPayloadGetPropInt(input, OC_RSRVD_ES_ENCTYPE, &encType))
    {
        gWiFiResource.encType = encType;
        wiFiData->enctype = gWiFiResource.encType;
        OIC_LOG_V(INFO, ES_RH_TAG, "gWiFiResource.encType %u", gWiFiResource.encType);
    }

    if(gReadUserdataCb)
    {
        gReadUserdataCb(input, OC_RSRVD_ES_RES_TYPE_WIFI, &wiFiData->userdata);
    }

    if(ssid || cred || authType!= -1 || encType != -1)
    {
        OIC_LOG(INFO, ES_RH_TAG, "Send WiFiRsrc Callback To ES");

        // TODO : Need to check appropriateness of gWiFiData
        if(gWifiRsrcEvtCb != NULL)
        {
            gWifiRsrcEvtCb(ES_OK, wiFiData);
        }
        else
        {
            OIC_LOG(ERROR, ES_RH_TAG, "gWifiRsrcEvtCb is NULL");
        }
    }

    if(OC_STACK_NO_OBSERVERS == OCNotifyAllObservers(gWiFiResource.handle, OC_HIGH_QOS))
    {
        OIC_LOG(INFO, ES_RH_TAG, "Enrollee doesn't have any observers.");
    }

    OICFree(wiFiData);
}

void updateCloudResource(OCRepPayload* input)
{
    ESCloudProvData* cloudData = (ESCloudProvData*)OICMalloc(sizeof(ESCloudProvData));

    if(cloudData == NULL)
    {
        OIC_LOG(DEBUG, ES_RH_TAG, "OICMalloc is failed");
        return;
    }

    memset(cloudData->authCode, 0, OIC_STRING_MAX_VALUE);
    memset(cloudData->authProvider, 0, OIC_STRING_MAX_VALUE);
    memset(cloudData->ciServer, 0, OIC_STRING_MAX_VALUE);
    cloudData->userdata = NULL;

    char *authCode = NULL;
    if (OCRepPayloadGetPropString(input, OC_RSRVD_ES_AUTHCODE, &authCode))
    {
        OICStrcpy(gCloudResource.authCode, sizeof(gCloudResource.authCode), authCode);
        OICStrcpy(cloudData->authCode, sizeof(cloudData->authCode), authCode);
        OIC_LOG_V(INFO, ES_RH_TAG, "gCloudResource.authCode %s", gCloudResource.authCode);
    }

    char *authProvider = NULL;
    if (OCRepPayloadGetPropString(input, OC_RSRVD_ES_AUTHPROVIDER, &authProvider))
    {
        OICStrcpy(gCloudResource.authProvider, sizeof(gCloudResource.authProvider), authProvider);
        OICStrcpy(cloudData->authProvider, sizeof(cloudData->authProvider), authProvider);
        OIC_LOG_V(INFO, ES_RH_TAG, "gCloudResource.authServerUrl %s", gCloudResource.authProvider);
    }

    char *ciServer = NULL;
    if (OCRepPayloadGetPropString(input, OC_RSRVD_ES_CISERVER, &ciServer))
    {
        OICStrcpy(gCloudResource.ciServer, sizeof(gCloudResource.ciServer), ciServer);
        OICStrcpy(cloudData->ciServer, sizeof(cloudData->ciServer), ciServer);
        OIC_LOG_V(INFO, ES_RH_TAG, "gCloudResource.ciServer %s", gCloudResource.ciServer);
    }

    if(gReadUserdataCb)
    {
        gReadUserdataCb(input, OC_RSRVD_ES_RES_TYPE_CLOUDSERVER, &cloudData->userdata);
    }

    if(authCode || authProvider || ciServer)
    {
        OIC_LOG(INFO, ES_RH_TAG, "Send CloudRsrc Callback To ES");

        // TODO : Need to check appropriateness of gCloudData
        if(gCloudRsrcEvtCb != NULL)
        {
            gCloudRsrcEvtCb(ES_OK, cloudData);
        }
        else
        {
            OIC_LOG(ERROR, ES_RH_TAG, "gCloudRsrcEvtCb is NULL");
        }
    }

    if(OC_STACK_NO_OBSERVERS == OCNotifyAllObservers(gCloudResource.handle, OC_HIGH_QOS))
    {
        OIC_LOG(INFO, ES_RH_TAG, "cloudResource doesn't have any observers.");
    }

    OICFree(cloudData);
}

void updateDevConfResource(OCRepPayload* input)
{
    ESDevConfProvData* devConfData = (ESDevConfProvData*)OICMalloc(sizeof(ESDevConfProvData));

    if(devConfData == NULL)
    {
        OIC_LOG(DEBUG, ES_RH_TAG, "OICMalloc is failed");
        return;
    }
    memset(devConfData->language, 0, OIC_STRING_MAX_VALUE);
    memset(devConfData->country, 0, OIC_STRING_MAX_VALUE);
    devConfData->userdata = NULL;

    char *location = NULL;
    if (OCRepPayloadGetPropString(input, OC_RSRVD_ES_LOCATION, &location))
    {
        OICStrcpy(gDevConfResource.location, sizeof(gDevConfResource.location), location);
        OICStrcpy(devConfData->location, sizeof(devConfData->location), location);
        OIC_LOG_V(INFO, ES_RH_TAG, "gDevConfResource.location %s", gDevConfResource.location);
    }

    char *country = NULL;
    if (OCRepPayloadGetPropString(input, OC_RSRVD_ES_COUNTRY, &country))
    {
        OICStrcpy(gDevConfResource.country, sizeof(gDevConfResource.country), country);
        OICStrcpy(devConfData->country, sizeof(devConfData->country), country);
        OIC_LOG_V(INFO, ES_RH_TAG, "gDevConfResource.country %s", gDevConfResource.country);
    }

    char *language = NULL;
    if (OCRepPayloadGetPropString(input, OC_RSRVD_ES_LANGUAGE, &language))
    {
        OICStrcpy(gDevConfResource.language, sizeof(gDevConfResource.language), language);
        OICStrcpy(devConfData->language, sizeof(devConfData->language), language);
        OIC_LOG_V(INFO, ES_RH_TAG, "gDevConfResource.language %s", gDevConfResource.language);
    }

    if(gReadUserdataCb)
    {
        gReadUserdataCb(input, OC_RSRVD_ES_RES_TYPE_DEVCONF, &devConfData->userdata);
    }

    if(country || language)
    {
        OIC_LOG(INFO, ES_RH_TAG, "Send DevConfRsrc Callback To ES");

        // TODO : Need to check appropriateness of gDevConfData
        if(gDevConfRsrcEvtCb != NULL)
        {
            gDevConfRsrcEvtCb(ES_OK, devConfData);
        }
        else
        {
            OIC_LOG(ERROR, ES_RH_TAG, "gDevConfRsrcEvtCb is NULL");
        }
    }

    if(OC_STACK_NO_OBSERVERS == OCNotifyAllObservers(gDevConfResource.handle, OC_HIGH_QOS))
    {
        OIC_LOG(INFO, ES_RH_TAG, "devConfResource doesn't have any observers.");
    }

    OICFree(devConfData);
}

OCRepPayload* constructResponseOfWiFi(char *interface)
{
    OCRepPayload* payload = OCRepPayloadCreate();
    if (!payload)
    {
        OIC_LOG(ERROR, ES_RH_TAG, "Failed to allocate Payload");
        return NULL;
    }

    if(gWiFiResource.handle == NULL)
    {
        OIC_LOG(ERROR, ES_RH_TAG, "WiFi resource is not created");
        return NULL;
    }

    OIC_LOG(INFO, ES_RH_TAG, "constructResponse wifi res");
    OCRepPayloadSetUri(payload, OC_RSRVD_ES_URI_WIFI);

    OCRepPayload* repPayload = NULL;
    OCRepPayload* tempPayload = NULL;
    if(!strcmp(interface, OC_RSRVD_INTERFACE_BATCH))
    {
        repPayload = OCRepPayloadCreate();
        if (!repPayload)
        {
            OIC_LOG(ERROR, ES_RH_TAG, "Failed to allocate Payload");
            return NULL;
        }

        tempPayload = payload;
        payload = repPayload;

        size_t interfacesDimensions[MAX_REP_ARRAY_DEPTH] = {1, 0, 0};
        char **interfaces = (char **)OICMalloc(3 * sizeof(char*));

        interfaces[0] = OICStrdup(OC_RSRVD_INTERFACE_DEFAULT);

        OCRepPayloadSetStringArray(payload, OC_RSRVD_ES_INTERFACE, (char **)interfaces, interfacesDimensions);

        size_t resourceTypesDimensions[MAX_REP_ARRAY_DEPTH] = {1, 0, 0};
        char **resourceTypes = (char **)OICMalloc(2 * sizeof(char*));

        resourceTypes[0] = OICStrdup(OC_RSRVD_ES_RES_TYPE_WIFI);

        OCRepPayloadSetStringArray(payload, OC_RSRVD_ES_RES_TYPE, (char **)resourceTypes, resourceTypesDimensions);
    }
    else
    {
        OCRepPayloadAddInterface(payload, OC_RSRVD_INTERFACE_DEFAULT);
        OCRepPayloadAddResourceType(payload, OC_RSRVD_ES_RES_TYPE_WIFI);
    }

    size_t dimensions[MAX_REP_ARRAY_DEPTH] = {gWiFiResource.numMode, 0, 0};
    int64_t *modes_64 = (int64_t *)OICMalloc(gWiFiResource.numMode * sizeof(int64_t));
    for(int i = 0 ; i < gWiFiResource.numMode ; ++i)
    {
        modes_64[i] = gWiFiResource.supportedMode[i];
    }
    OCRepPayloadSetIntArray(payload, OC_RSRVD_ES_SUPPORTEDWIFIMODE, (int64_t *)modes_64, dimensions);

    OCRepPayloadSetPropInt(payload, OC_RSRVD_ES_SUPPORTEDWIFIFREQ, gWiFiResource.supportedFreq);
    OCRepPayloadSetPropString(payload, OC_RSRVD_ES_SSID, gWiFiResource.ssid);
    OCRepPayloadSetPropString(payload, OC_RSRVD_ES_CRED, gWiFiResource.cred);
    OCRepPayloadSetPropInt(payload, OC_RSRVD_ES_AUTHTYPE, (int) gWiFiResource.authType);
    OCRepPayloadSetPropInt(payload, OC_RSRVD_ES_ENCTYPE, (int) gWiFiResource.encType);

    if(gWriteUserdataCb)
    {
        gWriteUserdataCb(payload, OC_RSRVD_ES_RES_TYPE_WIFI);
    }

    if(!strcmp(interface, OC_RSRVD_INTERFACE_BATCH))
    {
        payload = tempPayload;
        OCRepPayloadSetPropObject(payload, OC_RSRVD_REPRESENTATION, repPayload);
    }

    return payload;
}

OCRepPayload* constructResponseOfCloud(char *interface)
{
    OCRepPayload* payload = OCRepPayloadCreate();
    if (!payload)
    {
        OIC_LOG(ERROR, ES_RH_TAG, "Failed to allocate Payload");
        return NULL;
    }

    if(gCloudResource.handle == NULL)
    {
        OIC_LOG(ERROR, ES_RH_TAG, "CloudServer resource is not created");
        return NULL;
    }

    OIC_LOG(INFO, ES_RH_TAG, "constructResponse cloudserver res");
    OCRepPayloadSetUri(payload, OC_RSRVD_ES_URI_CLOUDSERVER);

    OCRepPayload* repPayload = NULL;
    OCRepPayload* tempPayload = NULL;
    if(!strcmp(interface, OC_RSRVD_INTERFACE_BATCH))
    {
        repPayload = OCRepPayloadCreate();
        if (!repPayload)
        {
            OIC_LOG(ERROR, ES_RH_TAG, "Failed to allocate Payload");
            return NULL;
        }

        tempPayload = payload;
        payload = repPayload;

        size_t interfacesDimensions[MAX_REP_ARRAY_DEPTH] = {1, 0, 0};
        char **interfaces = (char **)OICMalloc(3 * sizeof(char*));

        interfaces[0] = OICStrdup(OC_RSRVD_INTERFACE_DEFAULT);

        OCRepPayloadSetStringArray(payload, OC_RSRVD_ES_INTERFACE, (char **)interfaces, interfacesDimensions);

        size_t resourceTypesDimensions[MAX_REP_ARRAY_DEPTH] = {1, 0, 0};
        char **resourceTypes = (char **)OICMalloc(2 * sizeof(char*));

        resourceTypes[0] = OICStrdup(OC_RSRVD_ES_RES_TYPE_CLOUDSERVER);

        OCRepPayloadSetStringArray(payload, OC_RSRVD_ES_RES_TYPE, (char **)resourceTypes, resourceTypesDimensions);
    }
    else
    {
        OCRepPayloadAddInterface(payload, OC_RSRVD_INTERFACE_DEFAULT);
        OCRepPayloadAddResourceType(payload, OC_RSRVD_ES_RES_TYPE_CLOUDSERVER);
    }

    OCRepPayloadSetPropString(payload, OC_RSRVD_ES_AUTHCODE, gCloudResource.authCode);
    OCRepPayloadSetPropString(payload, OC_RSRVD_ES_AUTHPROVIDER, gCloudResource.authProvider);
    OCRepPayloadSetPropString(payload, OC_RSRVD_ES_CISERVER, gCloudResource.ciServer);

    if(gWriteUserdataCb)
    {
        gWriteUserdataCb(payload, OC_RSRVD_ES_RES_TYPE_CLOUDSERVER);
    }

    if(!strcmp(interface, OC_RSRVD_INTERFACE_BATCH))
    {
        payload = tempPayload;
        OCRepPayloadSetPropObject(payload, OC_RSRVD_REPRESENTATION, repPayload);
    }

    return payload;
}

OCRepPayload* constructResponseOfDevConf(char *interface)
{
    OCRepPayload* payload = OCRepPayloadCreate();
    if (!payload)
    {
        OIC_LOG(ERROR, ES_RH_TAG, "Failed to allocate Payload");
        return NULL;
    }

    if(gDevConfResource.handle == NULL)
    {
        OIC_LOG(ERROR, ES_RH_TAG, "DevConf resource is not created");
        return NULL;
    }

    OIC_LOG(INFO, ES_RH_TAG, "constructResponse devconf res");
    OCRepPayloadSetUri(payload, OC_RSRVD_ES_URI_DEVCONF);

    OCRepPayload* repPayload = NULL;
    OCRepPayload* tempPayload = NULL;
    if(!strcmp(interface, OC_RSRVD_INTERFACE_BATCH))
    {
        repPayload = OCRepPayloadCreate();
        if (!repPayload)
        {
            OIC_LOG(ERROR, ES_RH_TAG, "Failed to allocate Payload");
            return NULL;
        }

        tempPayload = payload;
        payload = repPayload;

        size_t interfacesDimensions[MAX_REP_ARRAY_DEPTH] = {1, 0, 0};
        char **interfaces = (char **)OICMalloc(3 * sizeof(char*));

        interfaces[0] = OICStrdup(OC_RSRVD_INTERFACE_DEFAULT);

        OCRepPayloadSetStringArray(payload, OC_RSRVD_ES_INTERFACE, (char **)interfaces, interfacesDimensions);

        size_t resourceTypesDimensions[MAX_REP_ARRAY_DEPTH] = {1, 0, 0};
        char **resourceTypes = (char **)OICMalloc(2 * sizeof(char*));

        resourceTypes[0] = OICStrdup(OC_RSRVD_ES_RES_TYPE_DEVCONF);

        OCRepPayloadSetStringArray(payload, OC_RSRVD_ES_RES_TYPE, (char **)resourceTypes, resourceTypesDimensions);
    }
    else
    {
        OCRepPayloadAddInterface(payload, OC_RSRVD_INTERFACE_DEFAULT);
        OCRepPayloadAddResourceType(payload, OC_RSRVD_ES_RES_TYPE_DEVCONF);
    }

    OCRepPayloadSetPropString(payload, OC_RSRVD_ES_DEVNAME, gDevConfResource.devName);
    OCRepPayloadSetPropString(payload, OC_RSRVD_ES_MODELNUMBER, gDevConfResource.modelNumber);
    OCRepPayloadSetPropString(payload, OC_RSRVD_ES_LOCATION, gDevConfResource.location);
    OCRepPayloadSetPropString(payload, OC_RSRVD_ES_LANGUAGE, gDevConfResource.language);
    OCRepPayloadSetPropString(payload, OC_RSRVD_ES_COUNTRY, gDevConfResource.country);

    if(gWriteUserdataCb)
    {
        gWriteUserdataCb(payload, OC_RSRVD_ES_RES_TYPE_DEVCONF);
    }

    if(!strcmp(interface, OC_RSRVD_INTERFACE_BATCH))
    {
        payload = tempPayload;
        OCRepPayloadSetPropObject(payload, OC_RSRVD_REPRESENTATION, repPayload);
    }

    return payload;
}

OCRepPayload* constructResponseOfProv(OCEntityHandlerRequest *ehRequest)
{
    OCRepPayload* payload = OCRepPayloadCreate();
    if (!payload)
    {
        OIC_LOG(ERROR, ES_RH_TAG, "Failed to allocate Payload");
        return NULL;
    }

    // Requested interface is Link list interface
    //if(ehRequest->query && CompareResourceInterface(ehRequest->query, OC_RSRVD_INTERFACE_LL))
    if(!ehRequest->query ||
        (ehRequest->query && !strcmp(ehRequest->query, "")) ||
        (ehRequest->query && CompareResourceInterface(ehRequest->query, OC_RSRVD_INTERFACE_LL)) ||
        (ehRequest->query && CompareResourceInterface(ehRequest->query, OC_RSRVD_INTERFACE_DEFAULT)))
    {
        const OCRepPayload *arrayPayload[3] = {NULL};

        int childResCnt = 0;

        if(gWiFiResource.handle != NULL)
        {
            OCRepPayload *add = OCRepPayloadCreate();
            if(!add)
            {
                OIC_LOG(ERROR, ES_RH_TAG, "Failed to allocate Payload");
                return NULL;
            }

            size_t dimensions[MAX_REP_ARRAY_DEPTH] = {1, 0, 0};
            char **resourceType = NULL;
            resourceType = (char **)OICMalloc(sizeof(char *) * 1);
            char **resourceInterface = NULL;
            resourceInterface = (char **)OICMalloc(sizeof(char *) * 1);

            if(!resourceType || !resourceInterface)
            {
                OIC_LOG(ERROR, ES_RH_TAG, "Failed to allocate Payload");
                return NULL;
            }

            resourceType[0] = OICStrdup(OC_RSRVD_ES_RES_TYPE_WIFI);
            resourceInterface[0] = OICStrdup(OC_RSRVD_INTERFACE_DEFAULT);

            add->base.type = PAYLOAD_TYPE_REPRESENTATION;
            OCRepPayloadSetPropString(add, OC_RSRVD_HREF, OC_RSRVD_ES_URI_WIFI);
            OCRepPayloadSetStringArray(add, OC_RSRVD_RESOURCE_TYPE,
                                            (const char **)resourceType, dimensions);
            OCRepPayloadSetStringArray(add, OC_RSRVD_INTERFACE,
                                            (const char **)resourceInterface, dimensions);

            OCResourceProperty p = OCGetResourceProperties((OCResourceHandle *)gWiFiResource.handle);
            OCRepPayload *policy = OCRepPayloadCreate();
            if (!policy)
            {
                OIC_LOG(ERROR, ES_RH_TAG, "Failed to allocate Payload");
                return NULL;
            }

            OCRepPayloadSetPropInt(policy, OC_RSRVD_BITMAP,
                                    ((p & OC_DISCOVERABLE) | (p & OC_OBSERVABLE)));
            if (p & OC_SECURE)
            {
                OCRepPayloadSetPropBool(policy, OC_RSRVD_SECURE, p & OC_SECURE);
                uint16_t securePort = CAGetAssignedPortNumber(ehRequest->devAddr.adapter,
                                                                    ehRequest->devAddr.flags);
                OCRepPayloadSetPropInt(policy, OC_RSRVD_HOSTING_PORT, securePort);
            }

            OCRepPayloadSetPropObject(add, OC_RSRVD_POLICY, policy);

            arrayPayload[childResCnt++] = add;
        }

        if(gDevConfResource.handle != NULL)
        {
            OCRepPayload *add = OCRepPayloadCreate();
            if(!add)
            {
                OIC_LOG(ERROR, ES_RH_TAG, "Failed to allocate Payload");
                return NULL;
            }

            size_t dimensions[MAX_REP_ARRAY_DEPTH] = {1, 0, 0};
            char **resourceType = NULL;
            resourceType = (char **)OICMalloc(sizeof(char *) * 1);
            char **resourceInterface = NULL;
            resourceInterface = (char **)OICMalloc(sizeof(char *) * 1);

            if(!resourceType || !resourceInterface)
            {
                OIC_LOG(ERROR, ES_RH_TAG, "Failed to allocate Payload");
                return NULL;
            }

            resourceType[0] = OICStrdup(OC_RSRVD_ES_RES_TYPE_DEVCONF);
            resourceInterface[0] = OICStrdup(OC_RSRVD_INTERFACE_DEFAULT);

            add->base.type = PAYLOAD_TYPE_REPRESENTATION;
            OCRepPayloadSetPropString(add, OC_RSRVD_HREF, OC_RSRVD_ES_URI_DEVCONF);
            OCRepPayloadSetStringArray(add, OC_RSRVD_RESOURCE_TYPE,
                                            (const char **)resourceType, dimensions);
            OCRepPayloadSetStringArray(add, OC_RSRVD_INTERFACE,
                                            (const char **)resourceInterface, dimensions);

            OCResourceProperty p = OCGetResourceProperties((OCResourceHandle *)gDevConfResource.handle);
            OCRepPayload *policy = OCRepPayloadCreate();
            if (!policy)
            {
                OIC_LOG(ERROR, ES_RH_TAG, "Failed to allocate Payload");
                return NULL;
            }

            OCRepPayloadSetPropInt(policy, OC_RSRVD_BITMAP,
                                    ((p & OC_DISCOVERABLE) | (p & OC_OBSERVABLE)));
            if (p & OC_SECURE)
            {
                OCRepPayloadSetPropBool(policy, OC_RSRVD_SECURE, p & OC_SECURE);
                uint16_t securePort = CAGetAssignedPortNumber(ehRequest->devAddr.adapter,
                                                                    ehRequest->devAddr.flags);
                OCRepPayloadSetPropInt(policy, OC_RSRVD_HOSTING_PORT, securePort);
            }

            OCRepPayloadSetPropObject(add, OC_RSRVD_POLICY, policy);

            arrayPayload[childResCnt++] = add;
        }

        if(gCloudResource.handle != NULL)
        {
            OCRepPayload *add = OCRepPayloadCreate();
            if(!add)
            {
                OIC_LOG(ERROR, ES_RH_TAG, "Failed to allocate Payload");
                return NULL;
            }

            size_t dimensions[MAX_REP_ARRAY_DEPTH] = {1, 0, 0};
            char **resourceType = NULL;
            resourceType = (char **)OICMalloc(sizeof(char *) * 1);
            char **resourceInterface = NULL;
            resourceInterface = (char **)OICMalloc(sizeof(char *) * 1);

            if(!resourceType || !resourceInterface)
            {
                OIC_LOG(ERROR, ES_RH_TAG, "Failed to allocate Payload");
                return NULL;
            }

            resourceType[0] = OICStrdup(OC_RSRVD_ES_RES_TYPE_CLOUDSERVER);
            resourceInterface[0] = OICStrdup(OC_RSRVD_INTERFACE_DEFAULT);

            add->base.type = PAYLOAD_TYPE_REPRESENTATION;
            OCRepPayloadSetPropString(add, OC_RSRVD_HREF, OC_RSRVD_ES_URI_CLOUDSERVER);
            OCRepPayloadSetStringArray(add, OC_RSRVD_RESOURCE_TYPE,
                                            (const char **)resourceType, dimensions);
            OCRepPayloadSetStringArray(add, OC_RSRVD_INTERFACE,
                                            (const char **)resourceInterface, dimensions);

            OCResourceProperty p = OCGetResourceProperties((OCResourceHandle *)gCloudResource.handle);
            OCRepPayload *policy = OCRepPayloadCreate();
            if (!policy)
            {
                OIC_LOG(ERROR, ES_RH_TAG, "Failed to allocate Payload");
                return NULL;
            }

            OCRepPayloadSetPropInt(policy, OC_RSRVD_BITMAP,
                                    ((p & OC_DISCOVERABLE) | (p & OC_OBSERVABLE)));
            if (p & OC_SECURE)
            {
                OCRepPayloadSetPropBool(policy, OC_RSRVD_SECURE, p & OC_SECURE);
                uint16_t securePort = CAGetAssignedPortNumber(ehRequest->devAddr.adapter,
                                                                    ehRequest->devAddr.flags);
                OCRepPayloadSetPropInt(policy, OC_RSRVD_HOSTING_PORT, securePort);
            }

            OCRepPayloadSetPropObject(add, OC_RSRVD_POLICY, policy);

            arrayPayload[childResCnt++] = add;
        }

        size_t dimensions[MAX_REP_ARRAY_DEPTH] = {childResCnt, 0, 0};

        if(!ehRequest->query ||
            (ehRequest->query && !strcmp(ehRequest->query, "")) ||
            (ehRequest->query && CompareResourceInterface(ehRequest->query, OC_RSRVD_INTERFACE_DEFAULT)))
        {
            OIC_LOG(INFO, ES_RH_TAG, "constructResponse prov res");
            OCRepPayloadSetUri(payload, OC_RSRVD_ES_URI_PROV);
            OCRepPayloadAddInterface(payload, OC_RSRVD_INTERFACE_DEFAULT);
            OCRepPayloadAddInterface(payload, OC_RSRVD_INTERFACE_LL);
            OCRepPayloadAddInterface(payload, OC_RSRVD_INTERFACE_BATCH);
            OCRepPayloadAddResourceType(payload, OC_RSRVD_ES_RES_TYPE_PROV);
            OCRepPayloadAddResourceType(payload, OC_RSRVD_ES_RES_TYPE_COL);

            OCRepPayloadSetPropInt(payload, OC_RSRVD_ES_PROVSTATUS, gProvResource.status);
            OCRepPayloadSetPropInt(payload, OC_RSRVD_ES_LAST_ERRORCODE, gProvResource.lastErrCode);

            OCRepPayloadSetPropObjectArray(payload, OC_RSRVD_ES_LINKS, arrayPayload, dimensions);
        }
        else    // link list interface
        {
            OCRepPayloadSetPropObjectArray(payload, OC_RSRVD_ES_LINKS, arrayPayload, dimensions);
        }
    } else if (
        ehRequest->query && CompareResourceInterface(ehRequest->query, OC_RSRVD_INTERFACE_BATCH))

    {
        OIC_LOG(INFO, ES_RH_TAG, "constructResponse prov res");
        OCRepPayloadSetUri(payload, OC_RSRVD_ES_URI_PROV);

        OCRepPayload* repPayload = NULL;

        repPayload = OCRepPayloadCreate();
        if (!repPayload)
        {
            OIC_LOG(ERROR, ES_RH_TAG, "Failed to allocate Payload");
            return NULL;
        }

        size_t interfacesDimensions[MAX_REP_ARRAY_DEPTH] = {3, 0, 0};
        char **interfaces = (char **)OICMalloc(3 * sizeof(char*));

        interfaces[0] = OICStrdup(OC_RSRVD_INTERFACE_DEFAULT);
        interfaces[1] = OICStrdup(OC_RSRVD_INTERFACE_LL);
        interfaces[2] = OICStrdup(OC_RSRVD_INTERFACE_BATCH);

        OCRepPayloadSetStringArray(repPayload, OC_RSRVD_ES_INTERFACE, (char **)interfaces, interfacesDimensions);

        size_t resourceTypesDimensions[MAX_REP_ARRAY_DEPTH] = {2, 0, 0};
        char **resourceTypes = (char **)OICMalloc(2 * sizeof(char*));

        resourceTypes[0] = OICStrdup(OC_RSRVD_ES_RES_TYPE_PROV);
        resourceTypes[1] = OICStrdup(OC_RSRVD_ES_RES_TYPE_COL);

        OCRepPayloadSetStringArray(repPayload, OC_RSRVD_ES_RES_TYPE, (char **)resourceTypes, resourceTypesDimensions);

        OCRepPayloadSetPropInt(repPayload, OC_RSRVD_ES_PROVSTATUS, gProvResource.status);
        OCRepPayloadSetPropInt(repPayload, OC_RSRVD_ES_LAST_ERRORCODE, gProvResource.lastErrCode);

        OCRepPayloadSetPropObject(payload, OC_RSRVD_REPRESENTATION, repPayload);
    }

    if(gWriteUserdataCb)
    {
        gWriteUserdataCb(payload, OC_RSRVD_ES_RES_TYPE_PROV);
    }

    if(ehRequest->query)
    {
        if(CompareResourceInterface(ehRequest->query, OC_RSRVD_INTERFACE_BATCH))
        {// When Provisioning resource has a GET with BatchInterface
            OCRepPayload* head = payload;
            OCRepPayload* nextPayload = NULL;

            nextPayload = constructResponseOfWiFi(OC_RSRVD_INTERFACE_BATCH);
            if(nextPayload != NULL)
            {
                payload->next = nextPayload;
                payload = payload->next;
            }

            nextPayload = constructResponseOfCloud(OC_RSRVD_INTERFACE_BATCH);
            if(nextPayload != NULL)
            {
                payload->next = nextPayload;
                payload = payload->next;
            }

            nextPayload = constructResponseOfDevConf(OC_RSRVD_INTERFACE_BATCH);
            if(nextPayload != NULL)
            {
                payload->next = nextPayload;
            }

            payload = head;
        }
    }

    return payload;
}


OCStackResult CreateEasySetupResources(bool isSecured, ESResourceMask resourceMask)
{
    OCStackResult res = OC_STACK_ERROR;
    bool maskFlag = false;

    res = initProvResource(isSecured);
    if(res != OC_STACK_OK)
    {
        // TODO: destroy logic will be added
        OIC_LOG_V(ERROR, ES_RH_TAG, "initProvResource result: %s", getResult(res));

        return res;
    }

    if((resourceMask & ES_WIFI_RESOURCE) == ES_WIFI_RESOURCE)
    {
        maskFlag = true;
        res = initWiFiResource(isSecured);
        if(res != OC_STACK_OK)
        {
            OIC_LOG_V(ERROR, ES_RH_TAG, "initWiFiResource result: %s", getResult(res));
            return res;
        }

        res = OCBindResource(gProvResource.handle, gWiFiResource.handle);
        if(res != OC_STACK_OK)
        {
            OIC_LOG_V(ERROR, ES_RH_TAG, "Bind WiFiResource result: %s", getResult(res));
            return res;
        }

    }

    if((resourceMask & ES_CLOUD_RESOURCE) == ES_CLOUD_RESOURCE)
    {
        maskFlag = true;
        res = initCloudServerResource(isSecured);
        if(res != OC_STACK_OK)
        {
            OIC_LOG_V(ERROR, ES_RH_TAG, "initCloudResource result: %s", getResult(res));
            return res;
        }

        res = OCBindResource(gProvResource.handle, gCloudResource.handle);
        if(res != OC_STACK_OK)
        {
            OIC_LOG_V(ERROR, ES_RH_TAG, "Bind CloudResource result: %s", getResult(res));
            return res;
        }
    }

    if((resourceMask & ES_DEVCONF_RESOURCE) == ES_DEVCONF_RESOURCE)
    {
        maskFlag = true;
        res = initDevConfResource(isSecured);
        if(res != OC_STACK_OK)
        {
            OIC_LOG_V(ERROR, ES_RH_TAG, "initDevConf result: %s", getResult(res));
            return res;
        }

        res = OCBindResource(gProvResource.handle, gDevConfResource.handle);
        if(res != OC_STACK_OK)
        {
            OIC_LOG_V(ERROR, ES_RH_TAG, "Bind DevConfResource result: %s", getResult(res));
            return res;
        }
    }


    if(maskFlag == false)
    {
        OIC_LOG_V(ERROR, ES_RH_TAG, "Invalid ResourceMask");
        return OC_STACK_ERROR;

    }

    OIC_LOG_V(INFO, ES_RH_TAG, "Created all resources with result: %s", getResult(res));

    return res;
}

OCStackResult DeleteProvisioningResource()
{
    OCStackResult res = OCDeleteResource(gProvResource.handle);
    if (res != OC_STACK_OK)
    {
        OIC_LOG_V(INFO, ES_RH_TAG, "Deleting Prov resource error with result: %s", getResult(res));
    }

    return res;
}

OCStackResult DeleteEasySetupResources()
{
    OCStackResult res = OC_STACK_ERROR;
    if (gWiFiResource.handle != NULL)
    {
        res = OCUnBindResource(gProvResource.handle, gWiFiResource.handle);
        if(res != OC_STACK_OK)
        {
            OIC_LOG_V(ERROR, ES_RH_TAG, "Unbind WiFi resource error with result: %s", getResult(res));
        }
    }
    if (gCloudResource.handle != NULL)
    {
        res = OCUnBindResource(gProvResource.handle, gCloudResource.handle);
        if(res != OC_STACK_OK)
        {
            OIC_LOG_V(ERROR, ES_RH_TAG, "Unbind CloudServer resource error with result: %s", getResult(res));
        }
    }
    if (gDevConfResource.handle != NULL)
    {
        res = OCUnBindResource(gProvResource.handle, gDevConfResource.handle);
        if(res != OC_STACK_OK)
        {
            OIC_LOG_V(ERROR, ES_RH_TAG, "Unbind DevConf resource error with result: %s", getResult(res));
        }
    }

    if (gWiFiResource.handle != NULL)
    {
        res = OCDeleteResource(gWiFiResource.handle);
        if (res != OC_STACK_OK)
        {
            OIC_LOG_V(ERROR, ES_RH_TAG, "Deleting WiFi resource error with result: %s", getResult(res));
        }
    }

    if(gCloudResource.handle != NULL)
    {
        res = OCDeleteResource(gCloudResource.handle);
        if (res != OC_STACK_OK)
        {
            OIC_LOG_V(ERROR, ES_RH_TAG, "Deleting CloudServer resource error with result: %s", getResult(res));
        }
    }

    if(gDevConfResource.handle != NULL)
    {
        res = OCDeleteResource(gDevConfResource.handle);
        if (res != OC_STACK_OK)
        {
            OIC_LOG_V(ERROR, ES_RH_TAG, "Deleting DevConf resource error with result: %s", getResult(res));
        }
    }

    if(gProvResource.handle != NULL)
    {
        res = OCDeleteResource(gProvResource.handle);
        if (res != OC_STACK_OK)
        {
            OIC_LOG_V(ERROR, ES_RH_TAG, "Deleting Prov resource error with result: %s", getResult(res));
        }
    }

    return res;
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

    OCRepPayload *getResp = NULL;
    *payload = NULL;

    if(ehRequest->resource == gProvResource.handle)
    {
        if(ehRequest->query &&
            strcmp(ehRequest->query, "") &&
            !CompareResourceInterface(ehRequest->query, OC_RSRVD_INTERFACE_LL) &&
            !CompareResourceInterface(ehRequest->query, OC_RSRVD_INTERFACE_BATCH) &&
            !CompareResourceInterface(ehRequest->query, OC_RSRVD_INTERFACE_DEFAULT))
        {
            OIC_LOG(ERROR, ES_RH_TAG, "Not supported Interface");
            return OC_EH_BAD_REQ;
        }
        else
        {
            getResp = constructResponseOfProv(ehRequest);
        }
    }
    else if(ehRequest->resource == gWiFiResource.handle)
    {
        if(CheckEhRequestPayload(ehRequest) != OC_EH_OK)
        {
            OIC_LOG(ERROR, ES_RH_TAG, "Not supported Interface");
            return OC_EH_BAD_REQ;
        }
        else
        {
            getResp = constructResponseOfWiFi(OC_RSRVD_INTERFACE_DEFAULT);
        }
    }
    else if(ehRequest->resource == gCloudResource.handle)
    {
        if(CheckEhRequestPayload(ehRequest) != OC_EH_OK)
        {
            OIC_LOG(ERROR, ES_RH_TAG, "Not supported Interface");
            return OC_EH_BAD_REQ;
        }
        else
        {
            getResp = constructResponseOfCloud(OC_RSRVD_INTERFACE_DEFAULT);
        }
    }
    else if(ehRequest->resource == gDevConfResource.handle)
    {
        if(CheckEhRequestPayload(ehRequest) != OC_EH_OK)
        {
            OIC_LOG(ERROR, ES_RH_TAG, "Not supported Interface");
            return OC_EH_BAD_REQ;
        }
        else
        {
            getResp = constructResponseOfDevConf(OC_RSRVD_INTERFACE_DEFAULT);
        }
    }

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
    OIC_LOG(INFO, ES_RH_TAG, "ProcessPostRequest enter");
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

    if(ehRequest->resource == gProvResource.handle)
    {
        if(ehRequest->query &&
            strcmp(ehRequest->query, "") &&
            !CompareResourceInterface(ehRequest->query, OC_RSRVD_INTERFACE_BATCH) &&
            !CompareResourceInterface(ehRequest->query, OC_RSRVD_INTERFACE_DEFAULT))
        {
            OIC_LOG(ERROR, ES_RH_TAG, "Not supported Interface");
            return OC_EH_BAD_REQ;
        }
        else
        {
            updateProvResource(ehRequest, input);
        }
    }
    else if(ehRequest->resource == gWiFiResource.handle)
    {
        if(CheckEhRequestPayload(ehRequest) != OC_EH_OK)
        {
            OIC_LOG(ERROR, ES_RH_TAG, "Not supported Interface");
            return OC_EH_BAD_REQ;
        }
        else
        {
            updateWiFiResource(input);
        }
    }
    else if(ehRequest->resource == gCloudResource.handle)
    {
        if(CheckEhRequestPayload(ehRequest) != OC_EH_OK)
        {
            OIC_LOG(ERROR, ES_RH_TAG, "Not supported Interface");
            return OC_EH_BAD_REQ;
        }
        else
        {
            updateCloudResource(input);
        }
    }
    else if(ehRequest->resource == gDevConfResource.handle)
    {
        if(CheckEhRequestPayload(ehRequest) != OC_EH_OK)
        {
            OIC_LOG(ERROR, ES_RH_TAG, "Not supported Interface");
            return OC_EH_BAD_REQ;
        }
        else
        {
            updateDevConfResource(input);
        }
    }

    OCRepPayload *getResp = NULL;
    if(ehRequest->resource == gProvResource.handle)
    {
        getResp = constructResponseOfProv(ehRequest);
    }
    else if(ehRequest->resource == gWiFiResource.handle)
    {
        getResp = constructResponseOfWiFi(OC_RSRVD_INTERFACE_DEFAULT);
    }
    else if(ehRequest->resource == gCloudResource.handle)
    {
        getResp = constructResponseOfCloud(OC_RSRVD_INTERFACE_DEFAULT);
    }
    else if(ehRequest->resource == gDevConfResource.handle)
    {
        getResp = constructResponseOfDevConf(OC_RSRVD_INTERFACE_DEFAULT);
    }

    if (!getResp)
    {
        OIC_LOG(ERROR, ES_RH_TAG, "constructResponse failed");
        return OC_EH_ERROR;
    }

    *payload = getResp;
    ehResult = OC_EH_OK;

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
            OIC_LOG(INFO, ES_RH_TAG, "Received GET request");
            ehRet = ProcessGetRequest(entityHandlerRequest, &payload);
        }
        else if (OC_REST_PUT == entityHandlerRequest->method)
        {
            OIC_LOG(INFO, ES_RH_TAG, "Received PUT request");

            //PUT request will be handled in the internal implementation
            if (gProvResource.handle != NULL)
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
            OIC_LOG(INFO, ES_RH_TAG, "Received OC_REST_POST from client");
            if (gProvResource.handle != NULL)
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
        response.resourceHandle = entityHandlerRequest->resource;
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
        OIC_LOG(INFO, ES_RH_TAG, "Flag includes OC_OBSERVE_FLAG");

        if (OC_OBSERVE_REGISTER == entityHandlerRequest->obsInfo.action)
        {
            OIC_LOG (INFO, ES_RH_TAG, "Received OC_OBSERVE_REGISTER from Mediator");
        }
        else if (OC_OBSERVE_DEREGISTER == entityHandlerRequest->obsInfo.action)
        {
            OIC_LOG (INFO, ES_RH_TAG, "Received OC_OBSERVE_DEREGISTER from Mediator");
        }
    }
    return ehRet;
}

OCStackResult SetDeviceProperty(ESDeviceProperty *deviceProperty)
{
    OIC_LOG(INFO, ES_RH_TAG, "SetDeviceProperty IN");

    gWiFiResource.supportedFreq = (deviceProperty->WiFi).freq;
    OIC_LOG_V(INFO, ES_RH_TAG, "WiFi Freq : %d", gWiFiResource.supportedFreq);

    int modeIdx = 0;
    while((deviceProperty->WiFi).mode[modeIdx] != WiFi_EOF)
    {
        gWiFiResource.supportedMode[modeIdx] = (deviceProperty->WiFi).mode[modeIdx];
        OIC_LOG_V(INFO, ES_RH_TAG, "WiFi Mode : %d", gWiFiResource.supportedMode[modeIdx]);
        modeIdx ++;
    }
    gWiFiResource.numMode = modeIdx;

    OICStrcpy(gDevConfResource.devName, OIC_STRING_MAX_VALUE, (deviceProperty->DevConf).deviceName);
    OIC_LOG_V(INFO, ES_RH_TAG, "Device Name : %s", gDevConfResource.devName);

    OICStrcpy(gDevConfResource.modelNumber, OIC_STRING_MAX_VALUE,
                                                            (deviceProperty->DevConf).modelNumber);
    OIC_LOG_V(INFO, ES_RH_TAG, "Model Number : %s", gDevConfResource.modelNumber);

    if(OC_STACK_NO_OBSERVERS == OCNotifyAllObservers(gWiFiResource.handle, OC_HIGH_QOS))
    {
        OIC_LOG(INFO, ES_RH_TAG, "wifiResource doesn't have any observers.");
    }

    if(OC_STACK_NO_OBSERVERS == OCNotifyAllObservers(gDevConfResource.handle, OC_HIGH_QOS))
    {
        OIC_LOG(INFO, ES_RH_TAG, "devConfResource doesn't have any observers.");
    }

    OIC_LOG(INFO, ES_RH_TAG, "SetDeviceProperty OUT");
    return OC_STACK_OK;
}

OCStackResult SetEnrolleeState(ESEnrolleeState esState)
{
    OIC_LOG(INFO, ES_RH_TAG, "SetEnrolleeState IN");

    gProvResource.status = esState;
    OIC_LOG_V(INFO, ES_RH_TAG, "Enrollee Status : %d", gProvResource.status);

    if(OC_STACK_NO_OBSERVERS == OCNotifyAllObservers(gProvResource.handle, OC_HIGH_QOS))
    {
        OIC_LOG(INFO, ES_RH_TAG, "provResource doesn't have any observers.");
    }

    OIC_LOG(INFO, ES_RH_TAG, "SetEnrolleeState OUT");
    return OC_STACK_OK;
}

OCStackResult SetEnrolleeErrCode(ESErrorCode esErrCode)
{
    OIC_LOG(INFO, ES_RH_TAG, "SetEnrolleeErrCode IN");

    gProvResource.lastErrCode = esErrCode;
    OIC_LOG_V(INFO, ES_RH_TAG, "Enrollee ErrorCode : %d", gProvResource.lastErrCode);

    if(OC_STACK_NO_OBSERVERS == OCNotifyAllObservers(gProvResource.handle, OC_HIGH_QOS))
    {
        OIC_LOG(INFO, ES_RH_TAG, "provResource doesn't have any observers.");
    }

    OIC_LOG(INFO, ES_RH_TAG, "SetEnrolleeErrCode OUT");
    return OC_STACK_OK;
}

OCEntityHandlerResult CheckEhRequestPayload(OCEntityHandlerRequest *ehRequest)
{
    if( !(ehRequest->query) ||
                (ehRequest->query &&
                (strcmp(ehRequest->query, "") && !CompareResourceInterface(ehRequest->query,
                                                                        OC_RSRVD_INTERFACE_DEFAULT))))
    {
        OIC_LOG(ERROR, ES_RH_TAG, "Not supported Interface");
        return OC_EH_BAD_REQ;
    }
    return OC_EH_OK;
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
