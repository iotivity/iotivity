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

#ifndef __NEST_H__
#define __NEST_H__

#include <time.h>
#include <stdio.h>
#include <vector>
#include <stdint.h>
#include "oic_malloc.h"
#include "oic_string.h"
#include "nest_defs.h"
#include "nest_thermostat.h"
#include <string.h>

using namespace std;

///
/// @brief This class encapsulates basic Nest functionality and
/// the ability to enumerate and retrieve instances of Nest
/// devices.
///
class Nest
{
public:

    ///
    /// @brief Holds an access token.
    ///
#pragma pack(push, 1)
    typedef struct _ACCESS_TOKEN
    {
        char accessToken[NEST_ACCESS_TOKEN_LEN]; // the access token used
        // in all REST calls
        char expires[NEST_ACCESS_TOKEN_EXPIRY];  // UTC time when token expires
        time_t acquiredTime;     // the time the token was aquired
        // (epoc time-January,1,1970)
        uint32_t grantTime;        // the time the token is valid for
        // (seconds)

        _ACCESS_TOKEN()
         {
        	bzero(accessToken,sizeof(accessToken));
        	bzero(expires,sizeof(expires));
        	acquiredTime=0;
        	grantTime=0;
         }
        _ACCESS_TOKEN(const char * a_token)
        {
            OICStrcpy(accessToken, NEST_ACCESS_TOKEN_LEN-1, a_token);
            memset(expires, 0, NEST_ACCESS_TOKEN_EXPIRY);
            acquiredTime = 0;
            grantTime = 0;
        }
    } ACCESS_TOKEN;
#pragma pack(pop)

    typedef enum
    {
        eAWUndefined = 0,                   // undefined
        eAWHome,                            // home
        eAWAway,                            // away
        eAWMax                              // max
    } AWAY_MODE;

    typedef struct _META_INFO
    {
        std::string homeName;           // describes the name/address
        std::string countryCode;        // country
        std::string timeZone;           // time zone (e.g. America/Los Angeles
        std::string structId;           // structure id
        AWAY_MODE awayMode;           // home / away
    } META_INFO;

    Nest(const ACCESS_TOKEN &accessToken);

    Nest();

    virtual ~Nest()
    { }

    ///
    /// Returns a previously acquired access token value to the Nest client.
    ///
    /// @param none.
    ///
    std::string getTok();

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
    /// OAuth 2.0 authorization with the Nest API backend.
    ///
    int getAccessToken(std::string &authorizationCode, ACCESS_TOKEN &accessToken,
    		           std::string &nest_client_id, std::string &nest_client_secret);

    ///
    /// Sets a previously acquired access token structure to the Nest client.
    ///
    /// @param token is a reference to a valid access token.
    ///
    void setAccessToken(const ACCESS_TOKEN &token);

    ///
    /// Returns the authorization state of the client.
    ///
    /// @returns true if the client is authorized to use the devices, otherwise false.
    ///
    /// @note use the getAuthorizationUrl to get the required authorizationURL to get
    /// user approval and an authorizationCode that can be used to exchange for a long
    /// term access token.
    ///
    bool isAuthorized();

    ///
    /// Returns the device list of thermostats.
    ///
    /// @return a SMART_DEV_OK on success, or another SMART_DEV_ on error. On
    /// success the devices vector will contain the authorized/found thermostats.
    ///
    int getThermostats(std::vector<std::shared_ptr<NestThermostat> > &devices);

    ///
    /// Sets the home/away mode.
    ///
    /// @param AWAY_MODE is a reference to the desired away/home mode.
    ///
    /// @return SMART_DEV_RESULT_OK on success, or another SMART_DEV_RESULT on
    /// error.
    ///
    int setAwayMode(const AWAY_MODE &awayMode);

private:

    int parseDevJsonResponse(std::string &json, std::vector<std::shared_ptr<NestThermostat> > &device);

    int parseStructureJsonResponse(std::string &json, META_INFO &meta);

    AWAY_MODE getAwayMode(const char *value) const;

    ACCESS_TOKEN m_accessToken;
    META_INFO m_metaInfo;
    bool m_isAuthorized;
};

#endif /* __NEST_H__ */
