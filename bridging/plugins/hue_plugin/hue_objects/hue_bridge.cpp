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

#include "hue_bridge.h"
#include "curlClient.h"
#include "experimental/logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include "stringbuffer.h"
#include "writer.h"

#define TAG "HUE_BRIDGE"

using namespace OC::Bridging;
HueBridge::HueBridge(hue_bridge_data_t data) :
    m_bridgeData(data)
{
}

HueBridge::~HueBridge()
{
    lightsFound.clear();
}

MPMResult HueBridge::parseBridgeConfig(std::string json)
{
    if (json.empty())
    {
        return MPM_RESULT_INTERNAL_ERROR;
    }
    rapidjson::Document doc;
    doc.SetObject();
    if (doc.Parse<0>(json.c_str()).HasParseError())
    {
        return MPM_RESULT_JSON_ERROR;
    }
    if (!JsonHelper::getMember(doc, BRIDGE_NAME, m_bridgeData.name))
    {
        OIC_LOG(ERROR, TAG, "Bridge Name is missing " );
    }
    if (!JsonHelper::getMember(doc, BRIDGE_ID, m_bridgeData.id))
    {
        OIC_LOG(ERROR, TAG, "Bridge id is missing " );
    }
    if (!JsonHelper::getMember(doc, BRIDGE_IP, m_bridgeData.ip))
    {
        OIC_LOG(ERROR, TAG, "Bridge ip is missing " );
    }
    if (!JsonHelper::getMember(doc, BRIDGE_MAC, m_bridgeData.mac))
    {
        OIC_LOG(ERROR, TAG, "Bridge Mac is missing " );
    }
    if (!JsonHelper::getMember(doc, BRIDGE_SW, m_bridgeData.swVersion))
    {
        OIC_LOG(ERROR, TAG, "Bridge SoftWare Version is missing " );
    }
    return MPM_RESULT_OK;
}

MPMResult HueBridge::getBridgeConfigFromCloud()
{
    rapidjson::Document doc;
    std::string discoveryUri;

    discoveryUri = m_curlQuery + "/config";
    CurlClient cc = CurlClient(CurlClient::CurlMethod::GET, discoveryUri)
                    .addRequestHeader(CURL_HEADER_ACCEPT_JSON);
    int curlCode = cc.send();
    if (curlCode != MPM_RESULT_OK)
    {
        OIC_LOG_V(ERROR, TAG, "GET request for light failed with error %d", curlCode);
        return MPM_RESULT_INTERNAL_ERROR;
    }
    std::string response = cc.getResponseBody();

    if(response.empty())
    {
        OIC_LOG(ERROR, TAG, "Config response is empty ");
        return MPM_RESULT_INTERNAL_ERROR;
    }
    parseBridgeConfig(response);
    return MPM_RESULT_OK;
}


MPMResult HueBridge::discoverHueLights()
{
    rapidjson::Document doc;
    std::string discoveryUri;

    discoveryUri = m_curlQuery + "/lights/";
    CurlClient cc = CurlClient(CurlClient::CurlMethod::GET, discoveryUri)
                    .addRequestHeader(CURL_HEADER_ACCEPT_JSON);
    int curlCode = cc.send();
    if (curlCode != MPM_RESULT_OK)
    {
        OIC_LOG_V(ERROR, TAG, "GET request for light failed with error %d", curlCode);
        return MPM_RESULT_INTERNAL_ERROR;
    }
    std::string response = cc.getResponseBody();
    doc.SetObject();
    if (doc.Parse<0>(response.c_str()).HasParseError())
    {
        OIC_LOG_V(ERROR, TAG, "Json error in response %s", response.c_str());
        return MPM_RESULT_JSON_ERROR;
    }

    if (doc.IsObject())
    {
        lightsFound.clear();
        std::string lightUri;
        std::string lightData;
        for (rapidjson::Value::ConstMemberIterator itr = doc.MemberBegin(); itr != doc.MemberEnd(); itr++)
        {
            rapidjson::StringBuffer sb;
            rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

            lightUri = itr->name.GetString();
            doc[lightUri.c_str()].Accept(writer);
            lightData = sb.GetString();
            std::shared_ptr<HueLight> light1 = std::make_shared<HueLight>(discoveryUri + lightUri,
                                               m_bridgeData.ip, m_bridgeData.id, lightUri, lightData);
            lightsFound.push_back(light1);
        }
    }

    return MPM_RESULT_OK;
}

/**
 * returns the lightsFound in lights.
 *
 * @param[in,out] lights is a vector that will be copied with the linked lights.
 */

void HueBridge::getScannedLights(HueLight::lights &lights)
{
    lights = lightsFound;
}


void HueBridge::fillLightDetails(std::shared_ptr<HueLight> light)
{
    lightsFound.push_back(light);
}
