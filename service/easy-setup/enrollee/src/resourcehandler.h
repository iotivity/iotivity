/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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

// Do not remove the include below

#include <string.h>
#include "experimental/logger.h"
#include "ocstack.h"
#include "escommon.h"
#include "ESEnrolleeCommon.h"
#include "octypes.h"

#ifndef ES_RESOURCE_HANDLER_H_
#define ES_RESOURCE_HANDLER_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*ESConnectRequestCB) (ESResult, ESConnectRequest *);
typedef void (*ESWiFiConfCB) (ESResult, ESWiFiConfData *);
typedef void (*ESCoapCloudConfCB) (ESResult, ESCoapCloudConfData *);
typedef void (*ESDevConfCB) (ESResult, ESDevConfData *);

typedef void (*ESWriteUserdataCb)(OCRepPayload* payload, char* resourceType);
typedef void (*ESReadUserdataCb)(OCRepPayload* payload, char* resourceType, void** userdata);

/** Structure to represent a Light resource */
typedef struct
{
    OCResourceHandle handle;  /*!< resource handle */
    ProvStatus status;        /*!< provisiong status */
    ESErrorCode lastErrCode;  /*!< last error code */
    ES_CONNECT_TYPE connectRequest[NUM_CONNECT_TYPE]; /*!< connect request */
    int numRequest;           /*!< num of request */
} EasySetupResource;

/** WiFi Configuration Resource */
typedef struct
{
    OCResourceHandle handle;               /*!< resource handle */
    WIFI_MODE supportedMode[NUM_WIFIMODE]; /*!< wifi supported mode */
    uint8_t numMode;                       /*!< the number of device's supported Wi-Fi modes */
    WIFI_FREQ supportedFreq[NUM_WIFIFREQ]; /*!< wifi supported frequency */
    uint8_t numSupportedFreq;              /*!< the number of device's supported Wi-Fi frequency bands */
    WIFI_AUTHTYPE supportedAuthType[NUM_WIFIAUTHTYPE]; /*!< wifi supported auth type */
    uint8_t numSupportedAuthType;          /*!< num of supported auth type */
    WIFI_ENCTYPE supportedEncType[NUM_WIFIENCTYPE]; /*!< supported enc type */
    uint8_t numSupportedEncType;           /*!< num of supported enc type */
    char ssid[OIC_STRING_MAX_VALUE];       /*!< SSID */
    char cred[OIC_STRING_MAX_VALUE];       /*!< credential information */
    WIFI_AUTHTYPE authType;                /*!< auth type */
    WIFI_ENCTYPE encType;                  /*!< encryption type */
} WiFiConfResource;

/** CoAP Cloud Configuration Resource */
typedef struct
{
    OCResourceHandle handle;                 /*!< resource handle */
    char accessToken[OIC_STRING_MAX_VALUE];  /*!< access token */
    OAUTH_TOKENTYPE accessTokenType;         /*!< access token type */
    char authProvider[OIC_STRING_MAX_VALUE]; /*!< auth provider */
    char ciServer[OIC_URI_STRING_MAX_VALUE];
} CoapCloudConfResource;

/** Device configuration resource */
typedef struct
{
    OCResourceHandle handle;                /*!< resource handle */
    char devName[OIC_STRING_MAX_VALUE];     /*!< device name */
    char modelNumber[OIC_STRING_MAX_VALUE]; /*!< device model number */
    char location[OIC_STRING_MAX_VALUE];    /*!< device location */
    char language[OIC_STRING_MAX_VALUE];    /*!< language information */
    char country[OIC_STRING_MAX_VALUE];     /*!< country information */
} DevConfResource;

/* Note: These values of structures are not thread-safety */
extern EasySetupResource g_ESEasySetupResource;
extern WiFiConfResource g_ESWiFiConfResource;
extern CoapCloudConfResource g_ESCoapCloudConfResource;
extern DevConfResource g_ESDevConfResource;

/**
 * Create easy ssetup resources.
 *
 * @param isSecured      check for secure mode.
 * @param resourceMask   resource mask.
 *
 * @return result value.
 */
OCStackResult CreateEasySetupResources(bool isSecured, ESResourceMask resourceMask);

/**
 * Delete easysetup resource.
 *
 * @return result value.
 */
OCStackResult DeleteEasySetupResources(void);

/**
 * Set device property.
 *
 * @param deviceProperty device property.
 *
 * @return success if set property, otherwise error.
 */
OCStackResult SetDeviceProperty(ESDeviceProperty *deviceProperty);

/**
 * Set enrollee state.
 *
 * @param esState easy setup state.
 *
 * @return success if set state.
 */
OCStackResult SetEnrolleeState(ESEnrolleeState esState);

/**
 * Set enrollee error code.
 *
 * @param esErrCode easy setup error code.
 *
 * @return success if set error code.
 */
OCStackResult SetEnrolleeErrCode(ESErrorCode esErrCode);

/**
 * Check entity handler request payload.
 *
 * @param ehRequest entity handler request.
 *
 * @return success if check, otherwise error.
 */
OCEntityHandlerResult CheckEhRequestPayload(OCEntityHandlerRequest *ehRequest);

/**
 * Register wifi resource event callback.
 *
 * @param cb callback.
 */
void RegisterWifiRsrcEventCallBack(ESWiFiConfCB);

/**
 * Register cloud resource event callback.
 *
 * @param cb callback.
 */
void RegisterCloudRsrcEventCallBack(ESCoapCloudConfCB);

/**
 * Register device resource event callback.
 *
 * @param cb callback.
 */
void RegisterDevConfRsrcEventCallBack(ESDevConfCB);

/**
 * Register connect request event callback.
 *
 * @param cb callback.
 */
void RegisterConnectRequestEventCallBack(ESConnectRequestCB cb);

/**
 * UnRegister resource event callback.
 */
void UnRegisterResourceEventCallBack(void);

/**
 * Set userdata callback.
 *
 * @param readCb   read callback.
 * @param writeCb  write callback.
 *
 * @return success if both callbacks are not null, otherwise ERROR.
 */
ESResult SetCallbackForUserData(ESReadUserdataCb readCb, ESWriteUserdataCb writeCb);

#ifdef __cplusplus
}
#endif

#endif //ES_RESOURCE_HANDLER_H_
