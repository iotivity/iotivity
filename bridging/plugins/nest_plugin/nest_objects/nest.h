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
#include "nestDefs.h"
#include "nestThermostat.h"
#include <string.h>

using namespace std;

/**
 * This class encapsulates basic Nest functionality and
 * the ability to enumerate and retrieve instances of Nest
 * devices.
 */
class Nest
{
    public:

        /*
         * Holds an access token.
         */
#pragma pack(push, 1)
        typedef struct _ACCESS_TOKEN
        {
            /** The access token used in all REST calls */
            char accessToken[NEST_ACCESS_TOKEN_LEN];

            /** UTC time when token expires */
            char expires[NEST_ACCESS_TOKEN_EXPIRY];

            /** The time the token was acquired (epoc time-January,1,1970) */
            time_t acquiredTime;

            /** The time the token is valid for (seconds) */
            uint32_t grantTime;

            _ACCESS_TOKEN()
            {
                bzero(accessToken, sizeof(accessToken));
                bzero(expires, sizeof(expires));
                acquiredTime = 0;
                grantTime = 0;
            }

            _ACCESS_TOKEN(const std::string &a_token)
            {
                OICStrcpy(accessToken, NEST_ACCESS_TOKEN_LEN - 1, a_token.c_str());
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
            AWAY_MODE awayMode;             // home / away
        } META_INFO;

        Nest(const ACCESS_TOKEN &accessToken);

        Nest();

        virtual ~Nest()
        { }

        /**
         * Returns a previously acquired access token value to the Nest client.
         *
         * @return access token as string
         */
        std::string getTok();

        /**
         * Attempts to obtain an access token given the authorization code.
         *
         * @param[in] authorizationCode            A reference to an authorization code retrieved
         *                                         OOB
         * @param[out] accessToken                 A valid ACCESS_TOKEN structure appropriately
         *                                         filled
         * @param[in] nest_client_id               A client id associated with user's cloud account
         * @param[in] nest_client_secret           A client secret code associated with user's
         *                                         cloud account
         *
         * @return MPM_RESULT_OK if ok, appropriate error code as defined in mpmErrorCode.h on error
         */
        MPMResult getAccessToken(std::string &authorizationCode, ACCESS_TOKEN &accessToken,
                                 std::string &nest_client_id, std::string &nest_client_secret);

        /**
         * Sets a previously acquired access token structure to the Nest client.
         *
         * @param token          A reference to a valid access token.
         */
        void setAccessToken(const ACCESS_TOKEN &token);

        /**
         * Returns the authorization state of the client.
         *
         * @return true if the client is authorized to use the devices, otherwise false.
         *
         */
        bool isAuthorized();

        /**
         * Returns the device list of thermostats.
         *
         * @param[in,out] devices        A vector reference to hold the found thermostat details
         *
         * @return MPM_RESULT_OK if ok, appropriate error code as defined in mpmErrorCode.h on
         *  error. On success the devices vector will contain the authorized/found thermostats.
         */
        MPMResult getThermostats(std::vector<std::shared_ptr<NestThermostat> > &devices);

        /**
         * Sets the home/away mode.
         *
         * @param awayMode            A reference to the desired away/home mode.
         *
         * @return MPM_RESULT_OK if ok, appropriate error code as defined in mpmErrorCode.h
         */
        MPMResult setAwayMode(const AWAY_MODE &awayMode);

    private:

        MPMResult parseDevJsonResponse(std::string &json,
                                       std::vector<std::shared_ptr<NestThermostat> > &device);

        MPMResult parseStructureJsonResponse(std::string &json, META_INFO &meta);

        AWAY_MODE getAwayMode(const std::string &value) const;

        ACCESS_TOKEN m_accessToken;
        META_INFO m_metaInfo;
        bool m_isAuthorized;
};

#endif /* __NEST_H__ */
