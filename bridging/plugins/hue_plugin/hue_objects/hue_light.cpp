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

#include "hue_light.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <iostream>
#include <math.h>
#include "stringbuffer.h"
#include "writer.h"
#include "curlClient.h"
#include "JsonHelper.h"
#include "experimental/logger.h"

#define TAG "HUE_LIGHT"
using namespace rapidjson;
using namespace OC::Bridging;

const std::string HUE_LIGHT_URI = "/light/";

HueLight::HueLight()
{
    m_initialized = true;
    m_uri.empty();
    m_lastCurlResponse.empty();
    m_user.empty();
    m_bridge_ip.empty();
    m_short_id.empty();
}

HueLight::HueLight(std::string uri, std::string bridge_ip, std::string bridge_mac,
                   std::string short_id, std::string json) :
    m_uri(uri), m_bridge_ip(bridge_ip), m_short_id(short_id), m_initialized(false)
{
    m_initialized = true;
    m_bridge_mac = bridge_mac;

    if (!json.empty())
    {
        parseJsonResponse(json);
    }
    /*URI - For now URI is in this format /a/light/1, /a/light/2 and so on*/
    m_config.uri = HUE_LIGHT_URI + m_short_id;
}

HueLight::~HueLight()
{
}

MPMResult HueLight::get()
{
    CurlClient cc = CurlClient(CurlClient::CurlMethod::GET, m_uri)
                    .addRequestHeader(CURL_HEADER_ACCEPT_JSON);

    int curlCode = cc.send();

    if (curlCode != MPM_RESULT_OK)
    {
        OIC_LOG_V(ERROR, TAG, "GET request for light failed with error %d", curlCode);
        return MPM_RESULT_INTERNAL_ERROR;
    }

    std::string response = cc.getResponseBody();

    return parseJsonResponse(response);
}

MPMResult HueLight::parseJsonResponse(std::string json)
{
    MPMResult result = MPM_RESULT_OK;
    rapidjson::Document doc;
    doc.SetObject();
    if (doc.Parse<0>(json.c_str()).HasParseError())
    {
        return MPM_RESULT_JSON_ERROR;
    }
    if (MPM_RESULT_OK != getInternalState(doc) ||
        MPM_RESULT_OK != getInternalConfig(doc))
    {
        result = MPM_RESULT_JSON_ERROR;
    }
    return result;
}

MPMResult HueLight::put(rapidjson::Document &doc)
{
    std::string uri = m_uri + "/" + DM_STATE;
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
    doc.Accept(writer);
    std::string jsonRequest = sb.GetString();

    CurlClient cc = CurlClient(CurlClient::CurlMethod::PUT, uri)
                    .addRequestHeader(CURL_HEADER_ACCEPT_JSON)
                    .setRequestBody(jsonRequest);

    int curlCode = cc.send();
    std::string response = cc.getResponseBody();

    if (curlCode != MPM_RESULT_OK)
    {
        OIC_LOG_V(ERROR, TAG, "PUT request for power failed. Error code %d", curlCode);
        return MPM_RESULT_INTERNAL_ERROR;
    }
    return MPM_RESULT_OK;
}

MPMResult HueLight::getInternalState(rapidjson::Document &doc)
{
    if (doc.HasMember(DM_STATE.c_str()) && doc[DM_STATE.c_str()].IsObject())
    {
        for (Value::ConstMemberIterator it = doc[DM_STATE.c_str()].MemberBegin();
             it != doc[DM_STATE.c_str()].MemberEnd(); it++)
        {
            if (!strcmp(it->name.GetString(), DM_STATE_POWERED.c_str()))
            {
                m_state.power = it->value.GetBool();
            }
            else if (!strcmp(it->name.GetString(), DM_STATE_HUE.c_str()))
            {
                m_state.hue = it->value.GetInt();
            }
            else if (!strcmp(it->name.GetString(), DM_STATE_BRI.c_str()))
            {
                m_state.bri = it->value.GetInt();
            }
            else if (!strcmp(it->name.GetString(), DM_STATE_SAT.c_str()))
            {
                m_state.sat = it->value.GetInt();
            }
            else if (!strcmp(it->name.GetString(), DM_STATE_REACHABLE.c_str()))
            {
                m_state.reachable = it->value.GetBool();
            }
            else if (!strcmp(it->name.GetString(), DM_STATE_COLORMODE.c_str()))
            {
                m_state.colorMode = it->value.GetString();
            }
            else if (!strcmp(it->name.GetString(), DM_STATE_ALERT.c_str()))
            {
                m_state.alert = it->value.GetString();
            }
            else if (!strcmp(it->name.GetString(), DM_STATE_CT.c_str()))
            {
                m_state.ct = it->value.GetUint();
            }
            else if (!strcmp(it->name.GetString(), DM_STATE_CSC.c_str()))
            {

                const Value &csc_xy = it->value;
                for (SizeType i = 0; i < csc_xy.Size(); i++)
                {

                    // A third element for "xy" is weird. Defensive if here.
                    if (i >= 2)
                    { break; }
                    m_state.csc[i] = csc_xy[i].GetDouble();
                }
            }
        }

    }
    else
    {
        return MPM_RESULT_JSON_ERROR;
    }
    return MPM_RESULT_OK;
}

/**
 * Generates URI for this resource using the bridge's IP address and resource's unique id
 * All of the URIs for the HUE resources begin with /a/light/.
 * If multiple bridges are discoverd the bridge_ip will give the uniqueness to the URI
 * URI example for light 1 : /a/light/192.168.1.120/1
 * NOTE: today we are not concerned with the URI size, however, in the future we may have to change
 * the size of unique string.
 *
 */
std::string HueLight::generateURI()
{
    return HUE_LIGHT_URI + m_short_id;
}


MPMResult HueLight::getInternalConfig(rapidjson::Document &doc)
{
    if (!JsonHelper::getMember(doc, DM_TYPE, m_config.type))
    {
        OIC_LOG(INFO, TAG, "config type is missing");
    }
    if (!JsonHelper::getMember(doc, DM_NAME, m_config.name))
    {
        OIC_LOG(INFO, TAG, "config  name is missing");
    }
    if (!JsonHelper::getMember(doc, DM_MODEL_ID, m_config.modelId))
    {
        OIC_LOG(INFO, TAG, "config modelId is missing");
    }
    if (!JsonHelper::getMember(doc, DM_UNIQUE_ID, m_config.uniqueId))
    {
        OIC_LOG(INFO, TAG, "config uniqueId is missing");
    }
    if (!JsonHelper::getMember(doc, DM_VERSION, m_config.swversion))
    {
        OIC_LOG(INFO, TAG, "config swversion is missing");
    }
    return MPM_RESULT_OK;
}

MPMResult HueLight::getState(light_state_t &state, bool refresh)
{
    MPMResult result = MPM_RESULT_OK;
    if (m_initialized)
    {
        if (refresh == true)
        {
            result = get();
        }
        if (result == MPM_RESULT_OK)
        {
            /* this is a structure copy */
            state = m_state;
        }
    }
    else
    {
        result = MPM_RESULT_INVALID_DATA;
    }
    return result;
}

MPMResult HueLight::setState(light_state_t &state)
{
    MPMResult result = MPM_RESULT_INVALID_DATA;
    if (m_initialized)
    {
        rapidjson::Document doc;
        doc.SetObject();

        JsonHelper::setMember(doc, DM_STATE_POWERED, state.power);

        // Set other bulb properties only if the bulb is ON.
        if (state.power == true)
        {
            JsonHelper::setMember(doc, DM_STATE_BRI, state.bri);
            JsonHelper::setMember(doc, DM_STATE_HUE, state.hue);
            JsonHelper::setMember(doc, DM_STATE_SAT, state.sat);


            /* The light either considers the hue & saturation pair
            or the xy pair. If both are present in the request, the "xy(csc)" array
            is given priority and the Hue is sets itself to the color in "xy(csc)".

            So, for this implementation of setState,
            If the new "xy(csc)" value is NOT equal to the current "xy(csc)" value,
            ONLY then is the "xy(csc)" set. The hue and sat from the previous state are
            set above as they will be overriden by the "xy(csc)" anyway. */
            if (fabs(state.csc[0] - m_state.csc[0]) > 0.0000005 &&
                fabs(state.csc[1] - m_state.csc[1]) > 0.0000005)
            {
                rapidjson::Value cscArray(rapidjson::kArrayType);
                cscArray.PushBack(state.csc[0], doc.GetAllocator());
                cscArray.PushBack(state.csc[1], doc.GetAllocator());

                JsonHelper::setMember(doc, DM_STATE_CSC, cscArray);
            }
        }


        result = put(doc);
    }
    return result;
}

MPMResult HueLight::getConfig(light_config_t &config)
{
    MPMResult result = MPM_RESULT_INTERNAL_ERROR;

    if (m_initialized)
    {
        /* this is a structure copy */
        config = m_config;
        result = MPM_RESULT_OK;
    }
    return result;
}


void HueLight::setConfig(light_config_t &config)
{
    /* this is a structure copy */
    m_config = config;
}
