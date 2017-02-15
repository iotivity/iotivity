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

#ifndef __HONEYWELLDEFSLYRIC_H__
#define __HONEYWELLDEFSLYRIC_H__

#include "honeywellDefs.h"

// various constants
#define HONEYWELL_REFRESH_TOKEN_LENGTH  32
#define HONEYWELL_ACCESS_TOKEN_LENGTH   28
#define HONEYWELL_TOKEN_FILE            "lyricToken.json"
#define HONEYWELL_TOKEN_BACKUP          "lyricToken.json.bak"
#define HW_PRODUCT_NAME                 "Lyric Thermostat"
#define HW_AUTH_SERVER_INFO             "Honeywell Lyric Server"
#define HONEYWELL_ACCESS_TOKEN_BUFSIZE  (HONEYWELL_ACCESS_TOKEN_LENGTH + 1)
#define HONEYWELL_REFRESH_TOKEN_BUFSIZE (HONEYWELL_REFRESH_TOKEN_LENGTH + 1)
#define HONEYWELL_ACCESS_TOKEN_EXPIRY   599 // assuming this is seconds?
#define HONEYWELL_BASE_URL "https://api.honeywell.com"
#define HONEYWELL_CLIENT_ID_LENGTH      32
#define HONEYWELL_CLIENT_ID_BUFFSIZE    (HONEYWELL_CLIENT_ID_LENGTH + 1)
#define HONEYWELL_CLIENT_AND_SECRET_64_LENGTH 68
#define HONEYWELL_CLIENT_AND_SECRET_64_BUFFSIZE (HONEYWELL_CLIENT_AND_SECRET_64_LENGTH + 1)

// Example: "https://api.honeywell.com/oauth2/authorize?client_id=0pKks3fVy8JqwsfTPW3TdWNwypdGpmIq&
//           response_type=code&redirect_uri=https://therm-api-wrapper.herokuapp.com/intelLyric"
// Format with AUTHORIZATION_FORMAT, AUTHORIZATION_URL, HONEYWELL_CLIENT_ID, HONEYWELL_REDIRECT_URI
#define AUTH_FORMAT_SIZE 512
#define AUTHORIZATION_FORMAT "%s?client_id=%s&response_type=code&redirect_uri=%s"
#define AUTHORIZATION_URL (HONEYWELL_BASE_URL "/oauth2/authorize")
#define HONEYWELL_REDIRECT_URI "https://therm-api-wrapper.herokuapp.com/intelLyric"

// REFRESHING AN ACCESS TOKEN
// POST TO URL: ACCESS_TOKEN_URL
// HEADER: AUTH_HEADER_FORMAT + HONEYWELL_CLIENT_AND_SECRET_64
// BODY: AUTH_REFRESH_BODY_FORMAT + current refresh token
#define ACCESS_TOKEN_URL (HONEYWELL_BASE_URL "/oauth2/token")
#define AUTH_HEADER_FORMAT "Authorization: Basic %s"
#define AUTH_REFRESH_BODY_FORMAT    "grant_type=refresh_token&refresh_token=%s"
// below is for exchanging auth code instead of refresh token
#define AUTH_AUTHCODE_BODY_FORMAT   "grant_type=authorization_code&code=%d&redirect_uri=%s"

// REQUESTING ACCOUNT INFO
// GET URL: ACCOUNT_INFO_URL
// HEADER: ACCT_HEADER_FORMAT + current access token (from access token response)
// BODY: N/A
#define ACCOUNT_INFO_URL (HONEYWELL_BASE_URL "/v1/locations")
#define ACCT_HEADER_FORMAT "Authorization: Bearer %s"
#define ACCOUNT_INFO_FORMAT "%s?apikey=%s"

// CONFIRMING DEVICE ACCESS
// PUT URL: ACCESS_LIST_FORMAT + ACCESS_LIST_URL + userId
// HEADERS: ACCT_HEADER_FORMAT, CONTENT_TYPE_JSON
// BODY: JSON array with deviceId and accessConfirmed fields in element.
#define CONTENT_TYPE_JSON "Content-Type: application/json"
#define DEVICE_ACCESS_SUCCESS 204

// GETTING DEVICE DETAILS
// GET URL: CHANGEABLE_VALUES_FORMAT + deviceId + HONEYWELL_CLIENT_ID + locationId
// HEADERS: ACCT_HEADER_FORMAT, CONTENT_TYPE_JSON
// BODY: N/A
#define CHANGEABLE_VALUES_FORMAT \
    (HONEYWELL_BASE_URL "/v1/devices/thermostats/%s?apikey=%s&locationId=%i")

// GETTING THERMOSTAT CURRENT STATE/STATUS
#define JSON_INDOOR_TEMPERATURE "indoorTemperature"

// CHANGING THERMOSTAT MODE
#define CHANGEABLE_VALUES_PUT_SUCCESS   200

#endif /* __HONEYWELLDEFSLYRIC_H__ */
