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
#include "curl_client.h"
#include "nest_defs.h"
#include "rapidjson.h"
#include "document.h"
#include "stringbuffer.h"
#include "writer.h"
#include "json_helper.h"
#include "logger.h"

using namespace rapidjson;

#define TAG "NEST"

Nest::Nest(const ACCESS_TOKEN &accessToken) : m_accessToken(accessToken), m_isAuthorized(false)
{
    m_metaInfo.homeName="";
    m_metaInfo.countryCode="";
    m_metaInfo.timeZone="";
    m_metaInfo.structId="";
    m_metaInfo.awayMode=eAWUndefined;
}

Nest::Nest() : m_isAuthorized(false)
{
    bzero(m_accessToken.accessToken,sizeof(m_accessToken.accessToken));
    bzero(m_accessToken.expires,sizeof(m_accessToken.expires));
    m_accessToken.acquiredTime=0;
    m_accessToken.grantTime=0;
}

void Nest::setAccessToken(const ACCESS_TOKEN &token)
{
    m_accessToken = token;
}

Nest::AWAY_MODE Nest::getAwayMode(const char *value) const
{
    std::string awayValue(value);

    if (!strncmp(value, NEST_HOME_TAG.c_str(), NEST_HOME_TAG.length()))
    {
        return eAWHome;
    }
    else if (!strncmp(value, NEST_AWAY_TAG.c_str(), NEST_AWAY_TAG.length()))
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

    CurlClient cc = CurlClient("GET", uri)
                .addRequestHeader(CURL_HEADER_ACCEPT_JSON)
                .addRequestHeader(CURL_CONTENT_TYPE_JSON);

    int curlCode = cc.send();
    std::string response = cc.getResponseBody();

    if (curlCode != GW_RESULT_OK)
    {
        OIC_LOG_V(ERROR, TAG,"Curl GET Request operation failed. Error code %d", curlCode);
        return GW_RESULT_INTERNAL_ERROR;
    }

    if (GW_RESULT_OK != parseStructureJsonResponse(response, m_metaInfo))
    {
        OIC_LOG(ERROR, TAG, "Parsing Json response failed on authorization");
        return false;
    }

    return true;
}

int Nest::getAccessToken(std::string &authorizationCode, ACCESS_TOKEN &accessToken,
                         std::string &nest_client_id, std::string &nest_client_secret)
{
    int result = GW_RESULT_OK;

    std::string uri(ACCESS_TOKEN_URL);

    uri += NEST_CLIENT_ID_STR + nest_client_id;
    uri += NEST_CODE_STR + authorizationCode;
    uri += NEST_CLIENT_SECRET_STR + nest_client_secret;
    uri += NEST_AUTH_CODE_STR;

    CurlClient cc = CurlClient("POST", uri)
                 .addRequestHeader(CURL_HEADER_ACCEPT_JSON)
                 .addRequestHeader(CURL_CONTENT_TYPE_JSON);

    int curlCode = cc.send();
    std::string response = cc.getResponseBody();

    if (curlCode != GW_RESULT_OK)
    {
        OIC_LOG_V(ERROR, TAG,"Post for getting access token failed. Error code %d", curlCode);
        return GW_RESULT_INTERNAL_ERROR;
    }

    rapidjson::Document doc;
    doc.SetObject();

    if (!doc.Parse<0>(response.c_str()).HasParseError())
    {
        if (doc.HasMember("error"))
        {
            OIC_LOG(ERROR, TAG, "get error message from nest server");
            return GW_RESULT_INTERNAL_ERROR;
        }
        time_t expires = time(NULL);
        expires += doc[NEST_VALIDITY_TAG.c_str()].GetInt();
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
                doc[NEST_ACCESS_TOKEN_TAG.c_str()].GetString());
        accessToken.grantTime = doc[NEST_VALIDITY_TAG.c_str()].GetInt();
        m_accessToken = accessToken;
        m_isAuthorized = true;
    }
    else
    {
        result = GW_RESULT_JSON_ERROR;
    }

    return result;
}

int Nest::parseStructureJsonResponse(std::string &json, META_INFO &meta)
{
    int result = GW_RESULT_NOT_AUTHORIZED;
    rapidjson::Document doc;
    doc.SetObject();

    if (doc.Parse<0>(json.c_str()).HasParseError())
    {
        return GW_RESULT_JSON_ERROR;
    }

    for (Value::ConstMemberIterator it = doc.MemberBegin(); it != doc.MemberEnd(); it++)
    {
        if(doc.HasMember("error"))
        {
            OIC_LOG(ERROR, TAG, "access token invalid");
            break;
        }
        std::string name = it->name.GetString();
        if (doc[name.c_str()].HasMember(NEST_AWAY_TAG.c_str()) &&
            doc[name.c_str()].HasMember(NEST_NAME_TAG.c_str()))
        {
            meta.homeName = it->value[NEST_NAME_TAG.c_str()].GetString();
            meta.countryCode = it->value[NEST_COUNTRY_TAG.c_str()].GetString();
            meta.timeZone = it->value[NEST_TIMEZONE_TAG.c_str()].GetString();
            meta.structId = it->value[NEST_STRUCT_ID_TAG.c_str()].GetString();
            meta.awayMode =
                    getAwayMode(it->value[NEST_AWAY_TAG.c_str()].GetString());
            m_isAuthorized = true;
            result = GW_RESULT_OK;
        }
    }

    return result;
}

int Nest::parseDevJsonResponse(std::string &json, std::vector<std::shared_ptr<NestThermostat> > &thermostats)
{
    rapidjson::Document doc;
    doc.SetObject();

    if (doc.Parse<0>(json.c_str()).HasParseError())
    {
        return GW_RESULT_JSON_ERROR;
    }

    if (doc.HasMember(NEST_THERMOSTAT_TAG.c_str())
        && doc[NEST_THERMOSTAT_TAG.c_str()].IsObject())
    {
        // The thermostat object holds each thermostat instance
        for (Value::ConstMemberIterator it = doc[NEST_THERMOSTAT_TAG.c_str()].MemberBegin();
             it != doc[NEST_THERMOSTAT_TAG.c_str()].MemberEnd(); it++)
        {
            // Extract the string representation from each thermostat instance
            std::string json = JsonHelper::toString(it);

            // Create the thermostat object, internally parse the JSON reprsentation
            // and add it to the thermostat vector
            std::shared_ptr<NestThermostat> thermostat = std::make_shared<NestThermostat>(m_accessToken.accessToken, json);
            thermostats.push_back(thermostat);
        }
    }
    else
    {
        return GW_RESULT_NOT_AUTHORIZED;
    }

    return GW_RESULT_OK;
}

int Nest::getThermostats(std::vector<std::shared_ptr<NestThermostat> > &thermostats)
{
    if (!strlen(m_accessToken.accessToken))
    {
        OIC_LOG(ERROR, TAG, "Not authorized");
        return GW_RESULT_NOT_AUTHORIZED;
    }

    int result = GW_RESULT_INTERNAL_ERROR;

    std::string uri(NEST_BASE_URL);
    uri += NEST_DEVICES_AUTH_STR + std::string(m_accessToken.accessToken);

    OIC_LOG_V(INFO, TAG, "GET request Uri: %s", uri.c_str());

    CurlClient cc = CurlClient("GET", uri)
                 .addRequestHeader(CURL_HEADER_ACCEPT_JSON)
                 .addRequestHeader(CURL_CONTENT_TYPE_JSON);

    int curlCode = cc.send();
    std::string response = cc.getResponseBody();

    if (curlCode != GW_RESULT_OK)
    {
        OIC_LOG_V(ERROR, TAG,"GET request failed while getting thermostat information. Error code %d", curlCode);
        return GW_RESULT_INTERNAL_ERROR;
    }

    if ((result = parseDevJsonResponse(response, thermostats)) != GW_RESULT_OK)
    {
        OIC_LOG_V(ERROR, TAG, "GET request failed while parsing thermostat information with result: %d", result);
        return result;
    }

    return GW_RESULT_OK;
}

int Nest::setAwayMode(const AWAY_MODE &awayMode)
{
    int result = GW_RESULT_INTERNAL_ERROR;
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
            result = GW_RESULT_INVALID_DATA;
        }

        if (result != GW_RESULT_INVALID_DATA)
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

                CurlClient cc = CurlClient("PUT", uri)
                             .addRequestHeader(CURL_HEADER_ACCEPT_JSON)
                             .addRequestHeader(CURL_CONTENT_TYPE_JSON)
                             .setRequestBody(request);

                 int curlCode = cc.send();

                 if (curlCode != GW_RESULT_OK)
                 {
                     OIC_LOG_V(ERROR, TAG,"PUT request for set away mode failed. Error code %d", curlCode);
                     return GW_RESULT_INTERNAL_ERROR;
                 }
                 result = GW_RESULT_OK;
            }
            else
            {
                result = GW_RESULT_JSON_ERROR;
            }
        }
    }
    else
    {
        result = GW_RESULT_NOT_AUTHORIZED;
    }

    return result;
}
