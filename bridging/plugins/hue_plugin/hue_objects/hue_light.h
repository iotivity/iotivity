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


/* This file contains the C++ representation definition of a Hue light */
#ifndef __HUE_LIGHT_H__
#define __HUE_LIGHT_H__

#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <memory>
#include <typeinfo>
#include "mpmErrorCode.h"
#include "rapidjson.h"
#include "document.h"
#include "hue_defs.h"
#include "messageHandler.h"


/**
 * The HueLight class represents one Hue Light instance. The class
 * provides methods to get/set of the desired configuration of the light.
 *
 * NOTE: A configuration can be build up by calling the desired set* methods
 *       followed by a call to put().
 *
 * For example;
 *    setOn(true);
 *    setHue(128);
 *    setBrightness(131);
 *    put();
 *
 * The representational state of the light is cached in the instance
 * but can be refreshed by a call to get();
 */
class HueLight
{
    public:
        typedef std::vector<std::shared_ptr<HueLight> > lights;

        typedef struct light_state_tag
        {
            uint64_t hue;
            uint64_t bri;
            uint64_t sat;
            /* CSC array. Will contain the x, y coordinates. Both will be in [0.0, 1.0]. */
            double csc[2];
            bool power;
            std::string effect;
            bool reachable;
            std::string alert;
            std::string colorMode;
            uint16_t ct;

            light_state_tag()
            {
                hue = bri = sat = ct = 0;
                power = reachable = 0;
                effect.empty();
                alert.empty();
                colorMode.empty();
            }

            bool operator!=(light_state_tag &state)
            {
                return !(*this == state);
            }

            bool operator==(light_state_tag &state)
            {
                /*we only care about following state changes*/
                return state.hue == hue && state.bri == bri && state.sat == sat && state.power == power;
            }
        } light_state_t;

        typedef struct light_config_tag
        {
            std::string type;
            /** Light type       (Readonly) */
            std::string name;
            /** Friendly name               */
            std::string modelId;
            /** Model ID         (Readonly) */
            std::string uniqueId;
            /** Unique Lamp ID   (Readonly) */
            std::string swversion;
            /** Software version (Readonly) */
            std::string uri; /*URI for this resource created using uniqueId*/
        } light_config_t;


        /**
         * Constructor initializes a light instance
         *
         * @param[in] uri        is the uri for the light instance (e.g. <ip>/api/<user>/light/1)
         * @param[in] json       is the initial JSON state for the  light. If empty,
         *                       the constructor will attempt to get the state directly
         *                       from the light.
         * @param[in] bridge_ip  is the ip address of the hue bridge
         * @param[in] bridge_mac is the mac address of the hue bridge
         * @param[in] short_id   is the light id
         */
        HueLight();

        HueLight(std::string uri, std::string bridge_ip, std::string bridge_mac, std::string short_id,
                 std::string json);

        virtual ~HueLight();

        /**
         * Retrieves the current light state of the Hue light.
         *
         * @param[in] state will hold the light state on success.
         * @param[in] refresh indicates whether to return cached data or not.
         *
         * @return MPM_RESULT_OK on success, or another MPM_RESULT_XXX on error.
         */
        MPMResult getState(light_state_t &state, bool refresh = false);

        /**
         * Sets the current light state of the Hue light.
         *
         * @param[in] state is the desired state of the light.
         *
         * @return  MPM_RESULT_OK on success, or another MPM_RESULT_XXX on error.
         */
        MPMResult setState(light_state_t &state);

        /**
         * Retrieves the configuration information of the Hue light.
         *
         * @param[in,out] config will hold the configuraion data on success.
         *
         * @return MPM_RESULT_OK on success, or another MPM_RESULT_XXX on error.
         */
        MPMResult getConfig(light_config_t &config);


        void setConfig(light_config_t &config);


        std::string getBridgeMac()
        {
            return m_bridge_mac;
        }

        std::string getShortId()
        {
            return m_short_id;
        }

        std::string getUri()
        {
            return m_uri;
        }

    private:
        /**
         * Retrieves the current state of the light. Values read from
         * are cached until a new get() is issued.
         *
         * @return MPM_RESULT_OK on success, or another MPM_RESULT_XXX code on error.
         */
        MPMResult get();

        /**
         * Sets the configured state of the light. Can be called for each setXXX() call
         * or sent in bulk.
         *
         * @param[in] doc is a reference to the JSON object that holds the configuration to
         *                send.
         *
         * @return MPM_RESULT_OK on success, or another MPM_RESULT_XXX code on error.
         */
        MPMResult put(rapidjson::Document &doc);

        /**
         * Retrieves the light state from the JSON representation
         *
         * @param[in,out] doc is a reference to the DOM that contains the JSON rep.
         *
         * @return MPM_RESULT_OK on success, or another MPM_RESULT_XXX on error.
         */
        MPMResult getInternalState(rapidjson::Document &doc);

        /**
         * Retrieves the configuration from the JSON representation
         *
         * @param[in,out] doc is a reference to the DOM that contains the JSON rep.
         *
         * @return MPM_RESULT_OK on success, or another MPM_RESULT_XXX on error.
         */
        MPMResult getInternalConfig(rapidjson::Document &doc);


        MPMResult parseJsonResponse(std::string json);


        /**
         * Generates unique URI for this resource
         */
        std::string generateURI();

        std::string m_uri;
        std::string m_lastCurlResponse;
        std::string m_user;
        std::string m_bridge_ip;
        std::string m_short_id;
        std::string m_bridge_mac;

        light_state_t m_state;
        light_config_t m_config;
        bool m_initialized;
};

typedef std::shared_ptr<HueLight> HueLightSharedPtr;


#endif /* __HUE_LIGHT_H__ */
