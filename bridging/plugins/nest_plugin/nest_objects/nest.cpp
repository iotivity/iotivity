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

#include <time.h>
#include <stdio.h>
#include <iostream>
#include "nest.h"
#include "curlClient.h"
#include "nestDefs.h"
#include "rapidjson.h"
#include "document.h"
#include "stringbuffer.h"
#include "writer.h"
#include "JsonHelper.h"
#include "experimental/logger.h"

using namespace rapidjson;
using namespace OC::Bridging;

#define TAG "NEST"

Nest::Nest(const ACCESS_TOKEN &accessToken) : m_accessToken(accessToken), m_isAuthorized(false)
{
    m_metaInfo.awayMode = eAWUndefined;
}

Nest::Nest() : m_isAuthorized(false) {}

void Nest::setAccessToken(const ACCESS_TOKEN &token)
{
    m_accessToken = token;
}

Nest::AWAY_MODE Nest::getAwayMode(const std::string &value) const
{
    if (NEST_HOME_TAG == value)
    {
        return eAWHome;
    }
    else if (NEST_AWAY_TAG == value)
    {
        return eAWAway;
    }

    return eAWUndefined;
}

std::string Nest::getTok()
{
    return m_accessToken.accessToken;
}

bool Nest::isAuthorized()
{
    if (!strlen(m_accessToken.accessToken))
    {
        OIC_LOG(ERROR, TAG, "Length of access token is zero");
        return false;
    }

    std::string uri(NEST_BASE_URL);
    uri += NEST_STRUCTURE_AUTH_STR + std::string(m_accessToken.accessToken);

    OIC_LOG_V(INFO, TAG, "uri: %s", uri.c_str());

    CurlClient cc = CurlClient(CurlClient::CurlMethod::GET, uri)
                    .addRequestHeader(CURL_HEADER_ACCEPT_JSON)
                    .addRequestHeader(CURL_CONTENT_TYPE_JSON);

    int curlCode = cc.send();
    std::string response = cc.getResponseBody();

    OIC_LOG_V(DEBUG, TAG, "The curl response string is: %s", response.c_str());

    if (curlCode != MPM_RESULT_OK)
    {
        OIC_LOG_V(ERROR, TAG, "Curl GET Request operation failed. Error code %d", curlCode);
        return false;
    }

    if (MPM_RESULT_OK != parseStructureJsonResponse(response, m_metaInfo))
    {
        OIC_LOG(ERROR, TAG, "Parsing Json response failed on authorization");
        return false;
    }

    return true;
}

MPMResult Nest::getAccessToken(std::string &authorizationCode, ACCESS_TOKEN &accessToken,
                               std::string &nest_client_id, std::string &nest_client_secret)
{
    MPMResult result = MPM_RESULT_OK;

    std::string uri(ACCESS_TOKEN_URL);

    uri += NEST_CLIENT_ID_STR + nest_client_id;
    uri += NEST_CODE_STR + authorizationCode;
    uri += NEST_CLIENT_SECRET_STR + nest_client_secret;
    uri += NEST_AUTH_CODE_STR;

    CurlClient cc = CurlClient(CurlClient::CurlMethod::POST, uri)
                    .addRequestHeader(CURL_HEADER_ACCEPT_JSON)
                    .addRequestHeader(CURL_CONTENT_TYPE_JSON);

    int curlCode = cc.send();
    std::string response = cc.getResponseBody();

    OIC_LOG_V(DEBUG, TAG, "The curl response string is: %s", response.c_str());

    if (curlCode != MPM_RESULT_OK)
    {
        OIC_LOG_V(ERROR, TAG, "Post for getting access token failed. Error code %d", curlCode);
        return MPM_RESULT_INTERNAL_ERROR;
    }

    rapidjson::Document doc;
    doc.SetObject();

    if (!doc.Parse<0>(response.c_str()).HasParseError())
    {
        if (doc.HasMember("error"))
        {
            OIC_LOG(ERROR, TAG, "get error message from nest server");
            return MPM_RESULT_INTERNAL_ERROR;
        }
        time_t expires = time(NULL);
        expires += doc[NEST_VALIDITY_TAG].GetInt();
        struct tm *expTime = localtime(&expires);
        if (expTime != NULL)
        {
            char *chExpTime = asctime(expTime);
            if (chExpTime != NULL)
            {
                OICStrcpy(accessToken.expires, NEST_ACCESS_TOKEN_EXPIRY - 1, chExpTime);
            }
        }
        OICStrcpy(accessToken.accessToken, NEST_ACCESS_TOKEN_LEN - 1,
                  doc[NEST_ACCESS_TOKEN_TAG].GetString());
        accessToken.grantTime = doc[NEST_VALIDITY_TAG].GetInt();
        m_accessToken = accessToken;
        m_isAuthorized = true;
    }
    else
    {
        result = MPM_RESULT_JSON_ERROR;
    }

    return result;
}

MPMResult Nest::parseStructureJsonResponse(std::string &json, META_INFO &meta)
{
    MPMResult result = MPM_RESULT_NOT_AUTHORIZED;
    rapidjson::Document doc;
    doc.SetObject();

    if (doc.Parse<0>(json.c_str()).HasParseError())
    {
        return MPM_RESULT_JSON_ERROR;
    }

    for (Value::ConstMemberIterator it = doc.MemberBegin(); it != doc.MemberEnd(); it++)
    {
        if (doc.HasMember("error"))
        {
            OIC_LOG(ERROR, TAG, "access token invalid");
            break;
        }
        std::string name = it->name.GetString();
        if (doc[name.c_str()].HasMember(NEST_AWAY_TAG) &&
            doc[name.c_str()].HasMember(NEST_NAME_TAG))
        {
            meta.homeName = it->value[NEST_NAME_TAG].GetString();
            meta.countryCode = it->value[NEST_COUNTRY_TAG].GetString();
            meta.timeZone = it->value[NEST_TIMEZONE_TAG].GetString();
            meta.structId = it->value[NEST_STRUCT_ID_TAG].GetString();
            meta.awayMode =
                getAwayMode(it->value[NEST_AWAY_TAG].GetString());
            m_isAuthorized = true;
            result = MPM_RESULT_OK;
        }
    }

    return result;
}

MPMResult Nest::parseDevJsonResponse(std::string &json,
                                     std::vector<std::shared_ptr<NestThermostat> > &thermostats)
{
    rapidjson::Document doc;
    doc.SetObject();

    if (doc.Parse<0>(json.c_str()).HasParseError())
    {
        return MPM_RESULT_JSON_ERROR;
    }

    if (doc.HasMember(NEST_THERMOSTAT_TAG)
        && doc[NEST_THERMOSTAT_TAG].IsObject())
    {
        // The thermostat object holds each thermostat instance
        for (Value::ConstMemberIterator it = doc[NEST_THERMOSTAT_TAG].MemberBegin();
             it != doc[NEST_THERMOSTAT_TAG].MemberEnd(); it++)
        {
            // Extract the string representation from each thermostat instance
            std::string json = JsonHelper::toString(it);

            // Create the thermostat object, internally parse the JSON reprsentation
            // and add it to the thermostat vector
            std::shared_ptr<NestThermostat> thermostat = std::make_shared<NestThermostat>
                    (m_accessToken.accessToken, json);
            thermostats.push_back(thermostat);
        }
    }
    else
    {
        return MPM_RESULT_NOT_AUTHORIZED;
    }

    return MPM_RESULT_OK;
}

MPMResult Nest::getThermostats(std::vector<std::shared_ptr<NestThermostat> > &thermostats)
{
    if (!strlen(m_accessToken.accessToken))
    {
        OIC_LOG(ERROR, TAG, "Not authorized");
        return MPM_RESULT_NOT_AUTHORIZED;
    }

    MPMResult result = MPM_RESULT_INTERNAL_ERROR;

    std::string uri(NEST_BASE_URL);
    uri += NEST_DEVICES_AUTH_STR + std::string(m_accessToken.accessToken);

    OIC_LOG_V(INFO, TAG, "GET request Uri: %s", uri.c_str());

    CurlClient cc = CurlClient(CurlClient::CurlMethod::GET, uri)
                    .addRequestHeader(CURL_HEADER_ACCEPT_JSON)
                    .addRequestHeader(CURL_CONTENT_TYPE_JSON);

    int curlCode = cc.send();
    std::string response = cc.getResponseBody();

    OIC_LOG_V(DEBUG, TAG, "The curl response string is: %s", response.c_str());

    if (curlCode != MPM_RESULT_OK)
    {
        OIC_LOG_V(ERROR, TAG, "GET request failed while getting thermostat information. Error code %d",
                  curlCode);
        return MPM_RESULT_INTERNAL_ERROR;
    }

    if ((result = parseDevJsonResponse(response, thermostats)) != MPM_RESULT_OK)
    {
        OIC_LOG_V(ERROR, TAG, "GET request failed while parsing thermostat information with result: %d",
                  result);
        return result;
    }

    return MPM_RESULT_OK;
}

MPMResult Nest::setAwayMode(const AWAY_MODE &awayMode)
{
    MPMResult result = MPM_RESULT_INTERNAL_ERROR;
    std::string awayValue;
    std::string request = "{ }";
    rapidjson::Document doc;

    if (m_isAuthorized)
    {
        if (awayMode == eAWAway)
        {
            awayValue = NEST_AWAY_TAG;
        }
        else if (awayMode == eAWHome)
        {
            awayValue = NEST_HOME_TAG;
        }
        else
        {
            // bad request
            result = MPM_RESULT_INVALID_DATA;
        }

        if (result != MPM_RESULT_INVALID_DATA)
        {
            // build the URI
            std::string uri(NEST_BASE_URL);
            uri += NEST_STRUCTURE_AUTH_STR;
            uri += m_accessToken.accessToken;

            // build the request
            if (!doc.Parse<0>(request.c_str()).HasParseError())
            {
                Document::AllocatorType &allocator = doc.GetAllocator();

                JsonHelper::setMember(doc, m_metaInfo.structId, "");
                doc[m_metaInfo.structId.c_str()].SetObject();

                rapidjson::Value val(awayValue.c_str(), allocator);
                JsonHelper::setMember(doc, m_metaInfo.structId, NEST_AWAY_TAG, val);

                // serialize the DOM to a string
                request = JsonHelper::toString(doc);

                CurlClient cc = CurlClient(CurlClient::CurlMethod::PUT, uri)
                                .addRequestHeader(CURL_HEADER_ACCEPT_JSON)
                                .addRequestHeader(CURL_CONTENT_TYPE_JSON)
                                .setRequestBody(request);

                int curlCode = cc.send();

                if (curlCode != MPM_RESULT_OK)
                {
                    OIC_LOG_V(ERROR, TAG, "PUT request for set away mode failed. Error code %d", curlCode);
                    return MPM_RESULT_INTERNAL_ERROR;
                }
                result = MPM_RESULT_OK;
            }
            else
            {
                result = MPM_RESULT_JSON_ERROR;
            }
        }
    }
    else
    {
        result = MPM_RESULT_NOT_AUTHORIZED;
    }

    return result;
}
