//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
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

#ifndef ES_ENROLLEE_COMMON_H_
#define ES_ENROLLEE_COMMON_H_

#include "ocstack.h"
#include "octypes.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Data structure for connect request from Mediator
 */
typedef struct
{
    ES_CONNECT_TYPE connect[NUM_CONNECT_TYPE];
    int numRequest;
} ESConnectRequest;

/**
 * @brief Data structure delivered from mediator, which provides WiFi information
 */
typedef struct
{
    char ssid[OIC_STRING_MAX_VALUE];        /**< Ssid of the Enroller**/
    char pwd[OIC_STRING_MAX_VALUE];         /**< Pwd of the Enroller**/
    WIFI_AUTHTYPE authtype;                 /**< Auth type of the Enroller**/
    WIFI_ENCTYPE enctype;                   /**< Encryption type of the Enroller**/
    void *userdata;                         /**< Vender-specific data**/
} ESWiFiConfData;

/**
 * @brief Data structure delivered from mediator, which provides device configuration information
 */
typedef struct
{
    // TODO: variables can be added when some properties in oic.r.devconf resource are specified.
    void *userdata;                         /**< Vender-specific data**/
} ESDevConfData;

/**
 * @brief Data structure delivered from mediator, which provides Cloud server information
 */
typedef struct
{
    char authCode[OIC_STRING_MAX_VALUE];        /**< Auth code issued by OAuth2.0-compatible account server **/
    char accessToken[OIC_STRING_MAX_VALUE];     /**< Access token resolved with an auth code **/
    OAUTH_TOKENTYPE accessTokenType;            /**< Access token type **/
    char authProvider[OIC_STRING_MAX_VALUE];    /**< Auth provider ID **/
    char ciServer[OIC_URI_STRING_MAX_VALUE];    /**< Cloud interface server URL which an Enrollee is going to registered **/
    void *userdata;                             /**< Vender-specific data**/
} ESCoapCloudConfData;

/**
 * @brief Data structure stored for Device property which includes a WiFi and device configuration.
 */
typedef struct
{
    /**
     * @brief Data structure indicating WiFi configuration of Enrollee
     */
    struct
    {
        WIFI_MODE supportedMode[NUM_WIFIMODE];
        uint8_t numSupportedMode;
        WIFI_FREQ supportedFreq[NUM_WIFIFREQ];
        uint8_t numSupportedFreq;
        WIFI_AUTHTYPE supportedAuthType[NUM_WIFIAUTHTYPE];
        uint8_t numSupportedAuthType;
        WIFI_ENCTYPE supportedEncType[NUM_WIFIENCTYPE];
        uint8_t numSupportedEncType;
    } WiFi;

    /**
     * @brief Data structure indicating device configuration of Enrollee
     */
    struct
    {
        char deviceName[OIC_STRING_MAX_VALUE];
    } DevConf;
} ESDeviceProperty;

/**
 * A set of functions pointers for callback functions which are called after provisioning data is
 * received from Mediator.
 */
typedef struct
{
    void (*ConnectRequestCb) (ESConnectRequest *);
    void (*WiFiConfProvCb) (ESWiFiConfData *);
    void (*DevConfProvCb) (ESDevConfData *);
    void (*CoapCloudConfProvCb) (ESCoapCloudConfData *);
} ESProvisioningCallbacks;


#ifdef __cplusplus
}
#endif

#endif //ES_ENROLLEE_COMMON_H_

