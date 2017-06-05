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

#ifndef __HONEYWELL_H__
#define __HONEYWELL_H__

#include <time.h>
#include <stdio.h>
#include <vector>
#include <stdint.h>
#include <typeinfo>
#include "honeywellDefsLyric.h"

#include "honeywellThermostat.h"
#include "../honeywellHelpers.h"
#include <pthread.h> // pthread_mutex_t

using namespace std;

///
/// @brief This class encapsulates basic Honeywell functionality and
/// the ability to enumerate and retrieve instances of Honeywell
/// devices.
///
class Honeywell
{
public:
///
/// @brief Holds an access token.
///
#pragma pack(push, 1)
    typedef struct _ACCESS_TOKEN
    {
        char accessToken[HONEYWELL_ACCESS_TOKEN_BUFSIZE]; // the access token used
        char refreshToken[HONEYWELL_REFRESH_TOKEN_BUFSIZE];
        // in all REST calls
        char expires[HONEYWELL_ACCESS_TOKEN_EXPIRY];  // UTC time when token expires
        time_t acquiredTime;                          // the time the token was aquired
        // (Epoch time-January,1,1970)
        uint32_t grantTime;                           // the time the token is valid for
        // (seconds)
        int userId;                                   // ID issued by honeywell after token exchange
    } ACCESS_TOKEN;
#pragma pack(pop)

#pragma pack(push, 1)
    typedef struct _CLIENT_ID_SECRET
    {
        char honeywell_clientId[HONEYWELL_CLIENT_ID_BUFFSIZE];
        char honeywell_client_secret[HONEYWELL_CLIENT_AND_SECRET_64_BUFFSIZE];
    } CLIENT_ID_SECRET;
#pragma pack(pop)
    typedef std::vector<std::string> curlHeadersV;

    /// Initialize object with valid access token.
    Honeywell(const ACCESS_TOKEN &accessToken, const CLIENT_ID_SECRET &clientIdSecret);

    /// Basic object initialization.
    Honeywell();

    virtual ~Honeywell();

    ///
    /// Attempts to obtain an access token given the authorization code. The application
    /// should have retrieved the authorization code OOB by following the OAuth flow
    /// after loading the URI returned from getAuthoriizationUrl() method.
    ///
    /// @param authorizationCode is a reference to a code retrieved from
    ///
    /// @param accessCode is a reference to an ACCESS_TOKEN structure returned
    /// upon success.
    ///
    /// OAuth 2.0 authorization with the Honeywell API backend.
    ///
    virtual int getAccessToken(std::string &authorizationCode, ACCESS_TOKEN &accessToken) = 0;

    /// deauthrorize a token
    int deauthorizateToken();

    ///
    /// Sets a previously acquired access token structure to the Honeywell client.
    ///
    /// @param token is a reference to a valid access token.
    ///
    void setAccessToken(const ACCESS_TOKEN &token);

    ///
    /// Sets the Client Id and Client secret of the user read from the .cnf file.
    ///
    /// @param token is a reference to a valid client id and client secret.
    ///
    void setClientIdAndSecret(const CLIENT_ID_SECRET &clientIdAndSecret);

    ///
    /// Returns the authorization state of the client.
    ///
    /// @returns true if the client is authorized to use the devices, otherwise false.
    ///
    /// @note use the getAuthorizationUrl to get the required authorizationURL to get
    /// user approval and an authorizationCode that can be used to exchange for a long
    /// term access token.
    ///
    virtual bool isAuthorized() = 0;

    ///
    /// Returns the device list of thermostats.
    ///
    /// @return a SMART_DEV_OK on success, or another SMART_DEV_ on error. On
    /// success the devices vector will contain the authorized/found thermostats.
    ///
    virtual int getThermostats(std::vector<LyricThermostatSharedPtr> &thermostats) = 0;

    /// Safely logs a string to OIC_LOG output.
    ///
    /// @param stringData - (input) String buffer to print.
    /// @param filename - (input optional) If non-null, specifies an output file to store the passed string.
    ///        File output is not truncated like OIC_LOG output is. Use this to capture JSON responses from cloud.
    void dumpResponseString(const std::string &stringData, const std::string &fileName);

    /// Sets temperature of selected honeywell thermostat.
    ///
    /// @param - Vector of honeywell thermostat devices.
    /// @data - (input) Contains target temperature data to set.
    /// @uri - (input optional) Resource URI of thermostat to target. (If not specified, first discovered
    ///        device is used.)
    ///
    /// @return MPM_RESULT_OK on success, error otherwise.
    virtual MPMResult setTemperature(LyricThermostatSharedPtr thermostat, const THERMOSTAT data,
                                     const std::string uri) = 0;

    /// Returns index of current "preferred" device. (In cases where we force a specific device to be used
    /// in tests or demos.
    ///
    /// @return index of preferred device in device array.
    int getPreferredDeviceIndex()
    { return m_preferredDeviceIndex; }

    /// Returns status of the mutex that protects the list of discovered devices.
    bool isDeviceChangeMutexInitialized()
    { return m_deviceChangeMutexInitialized; }

protected:

    /// Locks mutex that protects cloud-based activity.
    ///
    /// @return - true on success, false on failure.
    bool lockCloudAccess();

    /// Unlocks mutex that protects cloud-based activity.
    ///
    /// @return - true on success, false on failure.
    bool unlockCloudAccess();

    ACCESS_TOKEN m_accessToken; /// access token of current user
    CLIENT_ID_SECRET m_clientIdAndSecret;
    bool m_isAuthorized; /// whether user is authorized to access honeywell devices
    int m_preferredDeviceIndex; /// cloud array index of preferred device
    pthread_mutex_t m_cloudAccessMutex; /// mutex that serializes honeywell cloud transactions
    bool m_accessTokenMutexInitialized; // status of mutex that protects the current access token
    MPMResult manageMutexes(bool initialize);

    void initializeAccessToken();
    void initializeClientIdSecret();
    bool m_deviceChangeMutexInitialized; // status of mutex that protects device array
    bool m_getInProgress; // value indicating whether a GET operation is underway
};

#endif /* __HONEYWELL_H__ */
