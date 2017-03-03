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

#ifndef __HONEYWELLLYRIC_H__
#define __HONEYWELLLYRIC_H__

#include "honeywell.h"
#include <vector>

using namespace std;

///
/// @brief This class encapsulates basic Honeywell functionality and
/// the ability to enumerate and retrieve instances of Honeywell
/// devices.
///
class HoneywellLyric : public Honeywell
{
public:
    ///
    /// Attempts to obtain an access token given the authorization code.
    ///
    /// @param authorizationCode is a reference to a code retrieved from
    ///
    /// @param accessCode is a reference to an ACCESS_TOKEN structure returned
    /// upon success.
    ///
    /// OAuth 2.0 authorization with the Honeywell API backend.
    ///
    virtual int getAccessToken(std::string &authorizationCode, ACCESS_TOKEN &accessToken);

    ///
    /// Returns the authorization state of the client.
    ///
    /// @returns true if the client is authorized to use the devices, otherwise false.
    ///
    /// @note use the getAuthorizationUrl to get the required authorizationURL to get
    /// user approval and an authorizationCode that can be used to exchange for a long
    /// term access token.
    ///
    virtual bool isAuthorized();

    ///
    /// Returns the device list of thermostats.
    ///
    /// @return a SMART_DEV_OK on success, or another SMART_DEV_ on error. On
    /// success the devices vector will contain the authorized/found thermostats.
    ///
    virtual int getThermostats(std::vector<LyricThermostatSharedPtr> &thermostats);

    /// Sets temperature of selected honeywell thermostat.
    ///
    /// @param - Vector of honeywell thermostat devices.
    /// @data - (input) Contains target temperature data to set.
    /// @uri - (input optional) Resource URI of thermostat to target. (If not specified, first discovered
    ///        device is used.)
    ///
    /// @return MPM_RESULT_OK on success, error otherwise.
    virtual MPMResult setTemperature(LyricThermostatSharedPtr thermostat, const THERMOSTAT data,
                                     const std::string uri);
};

#endif /* __HONEYWELLLYRIC_H__ */
