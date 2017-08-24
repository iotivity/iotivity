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

#include <time.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include "honeywellDefsLyric.h"
#include "honeywellLyric.h"
#include "curlClient.h"
#include "rapidjson.h"
#include "document.h"
#include "stringbuffer.h"
#include "writer.h"
#include "experimental/logger.h"
#include <math.h>  // for fmin
#include <sstream> // for stringstream
#include <map>
#include <algorithm> // std::transform
#include "oic_string.h"
#include <fstream>      // std::ifstream

// *** THIS IS THE LYRIC VERSION OF THE FILE ***

using namespace rapidjson;
using namespace OC::Bridging;

#define LOG_TAG "HONEYWELL_LYRIC"

bool HoneywellLyric::isAuthorized()
{
    bool result = false;

    // NOTE: Unlike with Nest, Honeywell authorization is complete by the time
    //       anyone calls this function. In this plugin, you are "authorized"
    //       when you have received a valid access token and user ID from
    //       Honeywell's TCC server, and are recognized as a valid user account
    //       by Honeywell. It DOESN'T mean that you necessarily have access to
    //       any devices. This is determined later in the getThermostats function.

    // do some extra checks to make sure all values are set.
    if (m_isAuthorized)
    {
        if (0 == strlen(m_accessToken.accessToken))
        {
            OIC_LOG(ERROR, LOG_TAG, "m_isAuthorized is true but accessToken is empty!");
        }
        else
        {
            OIC_LOG(INFO, LOG_TAG, "Valid user is authorized to access Honeywell account.");
            result = true;
        }
    }
    return result;
}

// passed authorizationCode is expected to be a 32-character refresh_token string.
// passed accessToken is an empty ACCESS_TOKEN struct. Struct will be populated on
// success and also stored in member m_accessToken.
int HoneywellLyric::getAccessToken(std::string &authorizationCode, ACCESS_TOKEN &accessToken)
{
    OIC_LOG_V(INFO, LOG_TAG, "Honeywell::getAccessToken (Lyric) - Entered...");
    int result = MPM_RESULT_INTERNAL_ERROR;
    char messageHeader[MESSAGE_STRING_SIZE];
    char messageBody[MESSAGE_STRING_SIZE];
    std::string accessTokenLocal;
    std::string errorMessage;
    std::string refreshTokenFile = GetTokenPath(HONEYWELL_TOKEN_FILE);
    std::string oldRefreshTokenFile = GetTokenPath(HONEYWELL_TOKEN_BACKUP);
    std::string expiresIn; // access token ttl in seconds (string for lyric)
    std::string curlResponse;
    std::string fileContents; // contents of loaded token file
    std::vector<std::string> outHeaders;
    CurlClient cc = CurlClient(CurlClient::CurlMethod::POST, ACCESS_TOKEN_URL);
    std::string msgBody;
    // lock mutex
    bool mutexLocked = lockCloudAccess();
    rapidjson::Document accessTokenJsonResponse;

    std::stringstream expiresStream;
    int expiresInt = 0;
    std::ifstream fileExists (refreshTokenFile.c_str());

    if(mutexLocked == false)
    {
        OIC_LOG(ERROR, LOG_TAG, "Cloud is already locked by other get request in progress!");
        result = MPM_RESULT_ALREADY_STARTED;
        return result;
    }

    // form url for initial POST:
    // REFRESHING AN ACCESS TOKEN
    // POST TO URL: ACCESS_TOKEN_URL
    // HEADER: AUTH_HEADER_FORMAT + HONEYWELL_CLIENT_AND_SECRET_64
    sprintf(messageHeader, AUTH_HEADER_FORMAT, m_clientIdAndSecret.honeywell_client_secret);

    if (strlen(messageHeader) >= MAX_LOG_STRING)
    {
        OIC_LOG_V(ERROR, LOG_TAG, "messageHeader is too long to display (%lu)", (unsigned long) strlen(messageHeader));
    }

    // check for expected refresh token length
    if (HONEYWELL_REFRESH_TOKEN_LENGTH != authorizationCode.length())
    {
        OIC_LOG_V(ERROR, LOG_TAG, "authorizationCode length: %lu", (unsigned long) authorizationCode.length());
        if (strncmp(m_accessToken.refreshToken, "", HONEYWELL_REFRESH_TOKEN_LENGTH) != 0)
        {
            authorizationCode = m_accessToken.refreshToken;
            OIC_LOG_V(INFO, LOG_TAG, "authorizationCode = %s", authorizationCode.c_str());
        }
        else if (0 == authorizationCode.length())
        {
            OIC_LOG(INFO, LOG_TAG, "Attempting to load refresh_token from file...");

            // no code passed; load the code from the stored file, if available
            result = LoadFileIntoString(refreshTokenFile.c_str(), fileContents);
            if (MPM_RESULT_OK != result)
            {
                OIC_LOG_V(ERROR, LOG_TAG,
                      "LoadFileIntoString failed with %d attempting to open %s",
                      result,
                      refreshTokenFile.c_str());
                goto CLEANUP;
            }

            // get the refresh code from the file:
            rapidjson:: Document fileValues;
            fileValues.SetObject();
            if (fileValues.Parse(fileContents.c_str()).HasParseError())
            {
                OIC_LOG(ERROR, LOG_TAG,"Parse error in fileContents");
                result = MPM_RESULT_JSON_ERROR;
                goto CLEANUP;
            }
            if (fileValues.HasMember(JSON_REFRESH_TOKEN))
            {
                authorizationCode = fileValues[JSON_REFRESH_TOKEN].GetString();
                result = MPM_RESULT_OK;
            }
            else
            {
                OIC_LOG_V(ERROR, LOG_TAG, "GetJsonString failed attempting to get %s", JSON_REFRESH_TOKEN);
                result = MPM_RESULT_JSON_ERROR;
                goto CLEANUP;
            }
        }
    }

    // BODY: AUTH_REFRESH_BODY_FORMAT + current refresh token
    sprintf(messageBody, AUTH_REFRESH_BODY_FORMAT, authorizationCode.c_str());
    if (strlen(messageBody) >= MAX_LOG_STRING)
    {
        OIC_LOG_V(ERROR, LOG_TAG, "messageBody is too long to display (%lu)", (unsigned long) strlen(messageBody));
    }

    // don't have an access token yet, use default headers
    msgBody.assign(messageBody);
    cc = CurlClient(CurlClient::CurlMethod::POST, ACCESS_TOKEN_URL)
                   .addRequestHeader(messageHeader)
                   .setUserName(authorizationCode)
                   .setRequestBody(msgBody);

    result = (MPMResult) cc.send();

    if (MPM_RESULT_OK != result)
    {
        OIC_LOG_V(ERROR, LOG_TAG, "doPostRequest failed with %lu", (unsigned long) result);
        goto CLEANUP;
    }

    curlResponse = cc.getResponseBody();

    // see if we got an access token back. print error info if honeywell server returned an error.
    dumpResponseString(curlResponse.c_str(), "postAccessTokenResponse.json");
    accessTokenJsonResponse.SetObject();

    if (accessTokenJsonResponse.Parse(curlResponse.c_str()).HasParseError())
    {
        OIC_LOG(ERROR, LOG_TAG, "Parse error");
        result = MPM_RESULT_JSON_ERROR;
        goto CLEANUP;
    }

    if (accessTokenJsonResponse.HasMember("fault"))
    {
        OIC_LOG_V(ERROR, LOG_TAG, "Curl Response is error string = \n %s", curlResponse.c_str());
        result = MPM_RESULT_JSON_ERROR;
        goto CLEANUP;
    }

    if (accessTokenJsonResponse.HasMember(JSON_ACCESS_TOKEN))
    {
        accessTokenLocal = accessTokenJsonResponse[JSON_ACCESS_TOKEN].GetString();
    }

    if (accessTokenJsonResponse.HasMember(JSON_EXPIRES_IN))
    {
        expiresIn = accessTokenJsonResponse[JSON_EXPIRES_IN].GetString();
        OIC_LOG_V(INFO, LOG_TAG, "expires_in: %s seconds (string)", expiresIn.c_str());
        expiresStream.str(expiresIn);
        expiresStream >> expiresInt;
        accessToken.grantTime = (uint32_t) expiresInt;
        OIC_LOG_V(INFO, LOG_TAG, "expires_in: %d seconds (int)", accessToken.grantTime);
    }
    else
    {
        accessToken.grantTime = (uint32_t) HONEYWELL_ACCESS_TOKEN_EXPIRY;
    }

    // we got an access token, so first back up the old token file (if exists) to get ready for new one
    if (fileExists)
    {
        result = CopyFile(refreshTokenFile.c_str(), oldRefreshTokenFile.c_str(), false);
        if (MPM_RESULT_OK != result)
        {
            OIC_LOG_V(ERROR, LOG_TAG, "Failed to copy %s to %s", refreshTokenFile.c_str(), oldRefreshTokenFile.c_str());
            goto CLEANUP;
        }
    }

    // store token in accessToken param
    OIC_LOG_V(INFO, LOG_TAG, "Access token length: %lu chars", (unsigned long) accessTokenLocal.length());
    OICStrcpy(accessToken.accessToken, HONEYWELL_ACCESS_TOKEN_BUFSIZE, accessTokenLocal.c_str());
    OICStrcpy(accessToken.refreshToken, HONEYWELL_REFRESH_TOKEN_BUFSIZE, authorizationCode.c_str());

    // next save the new tokens in the old file's place
    result = SaveStringIntoFile(curlResponse.c_str(), refreshTokenFile.c_str());
    if (MPM_RESULT_OK != result)
    {
        OIC_LOG_V(ERROR, LOG_TAG, "Failed to save tokens into %s", refreshTokenFile.c_str());
        goto CLEANUP;
    }

    // if we're here we're successful. store token info and return successfully.
    m_accessToken = accessToken;
    m_isAuthorized = true;
    result = MPM_RESULT_OK;

    CLEANUP:

    // unlock mutex
    if (mutexLocked)
    {
        unlockCloudAccess();
    }

    return result;
}

MPMResult HoneywellLyric::setTemperature(LyricThermostatSharedPtr thermostat, const THERMOSTAT data,
                                         const std::string uri)
{
    MPMResult result = MPM_RESULT_OK;
    std::string newThermostatMode; // new mode to choose
    char messageUrl[MESSAGE_STRING_SIZE];
    char messageHeader[MESSAGE_STRING_SIZE];
    THERMOSTAT devicesThermostat;
    std::string mode;
    std::string curlResponse;
    long lastResponse = 0;
    bool changeMode = false;
    // lock mutex
    bool mutexLocked = lockCloudAccess();
    std::string changeableValues;
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer( sb );

    if (uri.length() <= 0)
    {
        OIC_LOG(ERROR, LOG_TAG, "uri is NULL");
    }

    OIC_LOG_V(INFO, LOG_TAG, "uri = %s", uri.c_str());
    // format uri with device ID and required setpoints
    // PUT URL: CHANGEABLE_VALUES_FORMAT + deviceId
    // HEADERS: ACCT_HEADER_FORMAT, CONTENT_TYPE_JSON
    // TODO - Come up with a better way of handling the device ID- necessary for multi thermostat support.
    thermostat->get(devicesThermostat);

    // DEBUG: Dump device info data
    dump_details(data, "data"); // caller data (cool and heat setpoint only)
    dump_details(devicesThermostat, "devicesThermostat"); // device data

    sprintf(messageUrl,
            CHANGEABLE_VALUES_FORMAT,
            devicesThermostat.devInfo.deviceIdStr.c_str(),
            m_clientIdAndSecret.honeywell_clientId,
            devicesThermostat.devInfo.locationId);
    OIC_LOG_V(INFO, LOG_TAG, "changeableValues messageUrl: %s", messageUrl);

    snprintf(messageHeader, MESSAGE_STRING_SIZE, ACCT_HEADER_FORMAT, m_accessToken.accessToken);

    CurlClient cc = CurlClient(CurlClient::CurlMethod::POST, messageUrl);

    // change the thermostat mode if indoor temp exceeds thresholds. otherwise, leave mode alone.
    if (devicesThermostat.ambientTempF > data.coolSetpointF)
    {
        mode.assign(THERMOSTAT_MODE_COOL);
        changeMode = true;
    }
    else if (devicesThermostat.ambientTempF < data.heatSetpointF)
    {
        mode.assign(THERMOSTAT_MODE_HEAT);
        changeMode = true;
    }

    changeableValues = thermostat->getChangeableValues();

    rapidjson:: Document values;
    values.SetObject();
    if (values.Parse(changeableValues.c_str()).HasParseError())
    {
        OIC_LOG(ERROR, LOG_TAG,"Parse error in set temperature");
        result = MPM_RESULT_JSON_ERROR;
        goto CLEANUP;
    }

    if (changeMode)
    {
        if(values.HasMember(JSON_MODE))
        {
            values[JSON_MODE].SetString(mode.c_str(), mode.length());
        }
    }

    if(values.HasMember(HONEYWELL_HEAT_SETPOINT))
    {
       values[HONEYWELL_HEAT_SETPOINT].SetDouble(data.heatSetpointF);
    }

    if(values.HasMember(HONEYWELL_COOL_SETPOINT))
    {
       values[HONEYWELL_COOL_SETPOINT].SetDouble(data.coolSetpointF);
    }

    values.Accept( writer );
    changeableValues = sb.GetString();
    OIC_LOG_V(INFO, LOG_TAG, "New Value = %s", changeableValues.c_str());

    cc = CurlClient(CurlClient::CurlMethod::POST, messageUrl)
                   .addRequestHeader(messageHeader)
                   .addRequestHeader(CONTENT_TYPE_JSON)
                   .setUserName(m_accessToken.accessToken)
                   .setRequestBody(changeableValues);

    result = (MPMResult) cc.send();
    if (MPM_RESULT_OK != result)
    {
        OIC_LOG_V(ERROR, LOG_TAG, "Post Request (account info) failed with %lu", (unsigned long) result);
        result = MPM_RESULT_INTERNAL_ERROR;
        goto CLEANUP;
    }

    curlResponse = cc.getResponseBody();

    // check result
    lastResponse = cc.getLastResponseCode();
    if (lastResponse != CHANGEABLE_VALUES_PUT_SUCCESS)
    {
        OIC_LOG_V(ERROR, LOG_TAG,
              "Response (%d) didn't match expected response (%d).",
              (int) lastResponse,
              (int) CHANGEABLE_VALUES_PUT_SUCCESS);
        goto CLEANUP;
    }
    else
    {
        OIC_LOG(INFO, LOG_TAG, "changeableValues change succeeded.");
    }

    // update current device item
    thermostat->setTemperature(data);
    thermostat->setChangeableValues(changeableValues);

    CLEANUP:
    // unlock mutex
    if (mutexLocked)
    {
        unlockCloudAccess();
    }

    return result;
}

int HoneywellLyric::getThermostats(std::vector<LyricThermostatSharedPtr> &thermostats)
{
    OIC_LOG(DEBUG, LOG_TAG, "Entered getThermostats.");

    int result = MPM_RESULT_OK;
    std::string accessConfirmed; // json blob indicating which devices to confirm
    std::string thermostatMode;  // current thermostat mode
    THERMOSTAT thermostatData;
    std::string curlResponse;
    char formattedUrl[128];

    // get list of authorized user locations (ACCOUNT_INFO_URL = locations)
    // Unlike Honeywell TCC, this command seems to require the auth and json header,
    // and additionally the API key needs to be included in the query.
    sprintf(formattedUrl, ACCOUNT_INFO_FORMAT, ACCOUNT_INFO_URL, m_clientIdAndSecret.honeywell_clientId);

    CurlClient cc = CurlClient(CurlClient::CurlMethod::GET, formattedUrl);
    std::string thermostatElement; // thermostat element from device data
    char messageHeader[MESSAGE_STRING_SIZE];
    rapidjson::Document thermostatsJsonResponse;
    int i = 0;

    // NOTE: Can't do memsets or memcpys with THERMOSTAT struct because it contains
    //       objects. Doing those things will result in corrupt objects. Added a
    //       constructor that initializes itself instead.
    HoneywellThermostat honeywellThermostat;

    // lock cloud mutex
    bool cloudMutexLocked = lockCloudAccess();

    if(cloudMutexLocked == false)
    {
        OIC_LOG(ERROR, LOG_TAG, "Cloud is already locked by other get Request in progress!");
        return MPM_RESULT_ALREADY_STARTED;
    }

    if (!strlen(m_accessToken.accessToken))
    {
        OIC_LOG(ERROR, LOG_TAG, "Not authorized");
        result = MPM_RESULT_NOT_AUTHORIZED;
        goto CLEANUP;
    }

    if (m_getInProgress)
    {
        OIC_LOG(ERROR, LOG_TAG, "There is already a getThermostats in progress!");
        result = MPM_RESULT_ALREADY_STARTED;
        goto CLEANUP;
    }

    // block other attempts while one is already in progress
    m_getInProgress = true;

    snprintf(messageHeader, MESSAGE_STRING_SIZE, ACCT_HEADER_FORMAT, m_accessToken.accessToken);
    if (strlen(messageHeader) >= MAX_LOG_STRING)
    {
        OIC_LOG_V(ERROR, LOG_TAG, "messageHeader is too long to display (%lu)", (unsigned long) strlen(messageHeader));
    }
    cc = CurlClient(CurlClient::CurlMethod::GET, formattedUrl)
                   .addRequestHeader(messageHeader)
                   .setUserName(m_accessToken.accessToken);

    result = (MPMResult) cc.send();
    if (MPM_RESULT_OK != result)
    {
        OIC_LOG_V(ERROR, LOG_TAG, "doGetRequest (account info) failed with %lu", (unsigned long) result);
        goto CLEANUP;
    }

    curlResponse = cc.getResponseBody();
    dumpResponseString(curlResponse.c_str(), "getAccountInfoResponse.json");

    thermostatsJsonResponse.SetObject();

    if (thermostatsJsonResponse.Parse<0>(curlResponse.c_str()).HasParseError())
    {
        OIC_LOG_V(ERROR, LOG_TAG,"Error parsing JSON:\n%s", curlResponse.c_str());
        result = MPM_RESULT_JSON_ERROR;
        goto CLEANUP;
    }

    if (!thermostatsJsonResponse.IsArray())
    {
        OIC_LOG_V(ERROR, LOG_TAG,"Response is not an array.\n%s", curlResponse.c_str());
        result = MPM_RESULT_JSON_ERROR;
        goto CLEANUP;
    }
    if (thermostatsJsonResponse.Empty())
    {
        OIC_LOG(ERROR, LOG_TAG, "Response is empty");
        result = MPM_RESULT_JSON_ERROR;
        goto CLEANUP;
    }

    for (rapidjson::Value::ConstValueIterator itr = thermostatsJsonResponse.Begin(); itr != thermostatsJsonResponse.End(); ++itr)
    {
        if (thermostatsJsonResponse[i].IsObject())
        {
            thermostatData.devInfo.locationId = thermostatsJsonResponse[i][JSON_LOCATION_ID].GetInt();

            OIC_LOG_V(DEBUG, LOG_TAG, "locationId = %d", thermostatData.devInfo.locationId);
            if (thermostatsJsonResponse[i].HasMember(JSON_DEVICES_ARRAY))
            {
                const rapidjson::Value& devices = thermostatsJsonResponse[i][JSON_DEVICES_ARRAY];

                for (rapidjson::SizeType itr = 0; itr < devices.Size(); itr++)
                {
                    if (devices[itr].HasMember(JSON_DEVICE_ID))
                    {
                        thermostatData.devInfo.deviceIdStr = devices[itr][JSON_DEVICE_ID].GetString();
                    }
                    if (devices[itr].HasMember(JSON_UNIQUE_ID))
                    {
                        // --- get and store macID (string)
                        thermostatData.devInfo.uniqueId = devices[itr][JSON_UNIQUE_ID].GetString();
                        honeywellThermostat.setDeviceUniqueId(thermostatData.devInfo.uniqueId.c_str());
                    }
                    if (devices[itr].HasMember(JSON_THERMOSTAT))
                    {
                        const rapidjson::Value& thrmS = devices[itr][JSON_THERMOSTAT];
                        thermostatData.ambientTempF =  thrmS[JSON_INDOOR_TEMPERATURE].GetDouble();
                        if (thrmS.HasMember(JSON_CHANGEABLE_VALUES))
                        {
                            const rapidjson::Value& changeValue = thrmS[JSON_CHANGEABLE_VALUES];
                            rapidjson::StringBuffer sb;
                            rapidjson::Writer<rapidjson::StringBuffer> writer( sb );
                            thrmS[JSON_CHANGEABLE_VALUES].Accept( writer );
                            std::string changeableValues = sb.GetString();
                            honeywellThermostat.setChangeableValues(changeableValues);
                            thermostatMode = changeValue[JSON_MODE].GetString();
                            thermostatData.heatSetpointF = changeValue[HONEYWELL_HEAT_SETPOINT].GetDouble();
                            thermostatData.coolSetpointF = changeValue[HONEYWELL_COOL_SETPOINT].GetDouble();
                            if (0 == strcasecmp(thermostatMode.c_str(), THERMOSTAT_MODE_COOL))
                            {
                                thermostatData.hvacMode = HVAC_COOL;
                            }
                            else if (0 == strcasecmp(thermostatMode.c_str(), THERMOSTAT_MODE_HEAT))
                            {
                                thermostatData.hvacMode = HVAC_HEAT;
                            }
                            else
                            {
                                thermostatData.hvacMode = HVAC_OFF;
                            }

                           // always compute target temp based on hot and cool threshold
                           thermostatData.targetTempF =
                                          computeTargetTemp(thermostatData.heatSetpointF, thermostatData.coolSetpointF);
                           dump_details(thermostatData, "thermostatData");
                           honeywellThermostat.set(thermostatData);

                           std::shared_ptr<HoneywellThermostat> thermostat = std::make_shared<HoneywellThermostat>(honeywellThermostat);
                           thermostats.push_back(thermostat);
                        }
                    }
                }
            }
        }
        i++;
    }

    CLEANUP:

    m_getInProgress = false;

    if (cloudMutexLocked)
    {
        unlockCloudAccess();
    }

    OIC_LOG_V(INFO, LOG_TAG, "Exiting getThermostats with code %d", result);
    return result;
}
