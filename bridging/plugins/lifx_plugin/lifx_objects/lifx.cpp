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

#define __STDC_FORMAT_MACROS
#include "stringbuffer.h"
#include "writer.h"
#include "curlClient.h"
#include "iotivity_config.h"
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include "document.h"
#include <memory>
#include <stdexcept>
#include <iostream>
#include "lifx.h"
#include "experimental/logger.h"

#define TAG "LIFX"
using namespace OC::Bridging;

MPMResult parseCloudResponse(const std::string response)
{
    rapidjson::Document doc;
    doc.SetObject();

    if (doc.Parse(response.c_str()).HasParseError())
    {
        OIC_LOG(ERROR, TAG, "Parse error in set Power");
        return MPM_RESULT_JSON_ERROR;
    }

    if (doc.HasMember("error"))
    {
        throw std::runtime_error(doc["error"].GetString());
    }

    if (doc.HasMember("results"))
    {
        const rapidjson::Value &results = doc["results"];

        if (results.Size() != 1)
        {
            OIC_LOG(INFO, TAG, "Multi element results. Only considering the first");
        }

        for (rapidjson::SizeType i = 0; i < results.Size(); ++i)
        {
            // This'll only consider the first one anyway as we only expect 1 element
            // This'll have to be changed when we can change multiple light states at once.
            std::string status = results[i]["status"].GetString();

            return status == "ok" ? MPM_RESULT_OK : MPM_RESULT_INTERNAL_ERROR;
        }
    }
    return MPM_RESULT_OK;
}

MPMResult static parseLightsFromCloudResponse(const std::string response, const std::string user,
        std::vector<std::shared_ptr<LifxLight>> &parsedLights)
{
    rapidjson::Document lightsJsonResponse;
    lightsJsonResponse.SetObject();

    if (lightsJsonResponse.Parse<0>(response.c_str()).HasParseError())
    {
        OIC_LOG_V(ERROR, TAG, "Error parsing JSON:\n%s", response.c_str());
        return MPM_RESULT_JSON_ERROR;
    }

    if (!lightsJsonResponse.IsArray())
    {
        OIC_LOG_V(ERROR, TAG, "Response is not an array.\n%s", response.c_str());
        return MPM_RESULT_JSON_ERROR;
    }
    if (lightsJsonResponse.Empty())
    {
        OIC_LOG(ERROR, TAG, "Response is empty");
        return MPM_RESULT_JSON_ERROR;
    }

    int i = 0;

    for (rapidjson::Value::ConstValueIterator itr = lightsJsonResponse.Begin();
         itr != lightsJsonResponse.End(); ++itr)
    {
        if (lightsJsonResponse[i].IsObject())
        {
            LifxLight::lightConfig cfg;
            cfg.id = lightsJsonResponse[i]["id"].GetString();
            cfg.uuid = lightsJsonResponse[i]["uuid"].GetString();
            cfg.label = lightsJsonResponse[i]["label"].GetString();

            LifxLight::lightState state;

            std::string powerStr  = lightsJsonResponse[i]["power"].GetString();

            state.power = powerStr == "on";

            state.brightness = lightsJsonResponse[i]["brightness"].GetDouble();
            state.connected = lightsJsonResponse[i]["connected"].GetBool();
            state.secondsSinceLastSeen = lightsJsonResponse[i]["seconds_since_seen"].GetDouble();

            std::shared_ptr<LifxLight> light = std::make_shared<LifxLight>(state, cfg, user);

            parsedLights.push_back(light);

            ++i;
        }
        else
        {
            OIC_LOG(ERROR, TAG, "Json blob doesn't have an expected object to be parsed.");
            return MPM_RESULT_INTERNAL_ERROR;
        }
    }

    return MPM_RESULT_OK;
}

MPMResult LifxLight::setState(std::string &stateRequest)
{
    if (this->user.empty())
    {
        throw std::runtime_error("Light not created in valid state by constructor. No \"user\" found");
    }

    CurlClient cc = CurlClient(CurlClient::CurlMethod::PUT, uri + "/state")
                    .addRequestHeader(CURL_HEADER_ACCEPT_JSON)
                    .setUserName(user)
                    .setRequestBody(stateRequest);

    OIC_LOG_V(INFO, TAG, "Request %s", stateRequest.c_str());
    OIC_LOG_V(INFO, TAG, "Uri %s", uri.c_str());

    int curlCode = cc.send();
    std::string response = cc.getResponseBody();

    OIC_LOG_V(INFO, TAG, "Response %s", response.c_str());

    if (curlCode != MPM_RESULT_OK)
    {
        OIC_LOG_V(ERROR, TAG, "PUT request for power failed. Error code %d", curlCode);
        return MPM_RESULT_INTERNAL_ERROR;
    }
    // Small sleep to let the light inform the cloud of any state changes.
    sleep(2);
    refreshState();

    return parseCloudResponse(response);
}

MPMResult LifxLight::setPower(bool power)
{
    std::string setPowerRequest = std::string("power=") + (power ? "on" : "off");
    return setState(setPowerRequest);
}

MPMResult LifxLight::setBrightness(double brightness)
{
    if (brightness < 0.0) brightness = 0.0;
    else if (brightness > 1.0) brightness = 1.0;

    std::string setBrightnessRequest = std::string("brightness=") + std::to_string(brightness);

    return setState(setBrightnessRequest);
}

// Limit calls to refreshState. LifX APIs limit calls
// from an access token to 60 calls every 60 seconds
// as made clear @ https://api.developer.lifx.com/docs/rate-limits
MPMResult LifxLight::refreshState()
{
    if (this->user.empty())
    {
        throw std::runtime_error("Light not created in valid state by constructor. No \"user\" found");
    }

    CurlClient cc = CurlClient(CurlClient::CurlMethod::GET, uri)
                    .addRequestHeader(CURL_HEADER_ACCEPT_JSON)
                    .setUserName(user);

    int curlCode = cc.send();

    if (curlCode != MPM_RESULT_OK)
    {
        OIC_LOG_V(ERROR, TAG, "GET request for light failed with error %d", curlCode);
        return MPM_RESULT_INTERNAL_ERROR;
    }

    std::string response = cc.getResponseBody();

    std::vector<std::shared_ptr<LifxLight>> parsedLights;
    MPMResult parseResult = parseLightsFromCloudResponse(response, this->user, parsedLights);

    if (parseResult != MPM_RESULT_OK)
    {
        return parseResult;
    }

    if (parsedLights.size() != 1)
    {
        OIC_LOG_V(ERROR, TAG, "This is irregular! Instead of 1 light, returned %" PRIuPTR "", parsedLights.size());
        return MPM_RESULT_JSON_ERROR;
    }

    std::shared_ptr<LifxLight> light = parsedLights[0]; // Only 1 light expected here.

    if (light->config.uuid != this->config.uuid)
    {
        OIC_LOG_V(ERROR, TAG, "%s in response, when request made by %s", light->config.uuid.c_str(),
                  this->config.uuid.c_str());
        return MPM_RESULT_JSON_ERROR;
    }

    this->state = light->state;
    return MPM_RESULT_OK;
}

MPMResult LifxLight::getLights(const std::string user,
                               std::vector<std::shared_ptr<LifxLight> > &lights)
{
    CurlClient cc = CurlClient(CurlClient::CurlMethod::GET, LIFX_LIST_LIGHTS_URI)
                    .addRequestHeader(CURL_HEADER_ACCEPT_JSON)
                    .setUserName(user);

    int curlCode = cc.send();

    if (curlCode != MPM_RESULT_OK)
    {
        OIC_LOG_V(ERROR, TAG, "GET request for lights failed. Error code %d", curlCode);
        return MPM_RESULT_INTERNAL_ERROR;
    }

    std::string response = cc.getResponseBody();

    return parseLightsFromCloudResponse(response, user, lights);
}

