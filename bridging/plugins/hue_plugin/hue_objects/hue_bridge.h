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

#ifndef __HUE_BRIDGE_H__
#define __HUE_BRIDGE_H__

#include <stdio.h>
#include <vector>
#include <string>
#include "hue_light.h"
#include "JsonHelper.h"

/**
 * HueBridge class which stores the bridge configuration and associated lights
 * for each bridge.
 */
class HueBridge
{
    public:
        typedef std::vector<std::shared_ptr<HueLight> > lights;

        virtual ~HueBridge();

        typedef struct hue_bridge_data_tag
        {
            std::string name;
            /* Friendly name of the bridge */
            std::string id;
            /* Unique ID for the bridge */
            std::string channel;
            /* The ZigBee channel in use */
            std::string mac;
            /* MAC address of the bridge */
            std::string ip;
            /* IP address of the bridge */
            std::string gateway;
            /* Gateway IP address */
            std::string timezone;
            /* Current / configured timezone */
            std::string localTime;
            /* Local time */
            std::string swVersion;
            /* Software version */
            std::string apiVersion; /* API version */
        } hue_bridge_data_t;

        HueBridge()
        {
            m_bridgeData.mac.empty();
            m_curlQuery.empty();
            lightsFound.clear();
        }

        HueBridge(hue_bridge_data_t data);

        HueBridge(std::string data);

        MPMResult parseBridgeConfig(std::string json);

        /**
         * sets the bridge MAC
         */
        void setBridgeMAC(std::string strMac)
        {
            m_bridgeData.id = strMac;
        }

        /**
         * sets the lib curl query to get the lights associated with the bridge
         */
        void setBridgeCurlQuery(std::string strQuery)
        {
            m_curlQuery = strQuery;
        }

        /**
         * gets the bridge MAC
         */
        std::string getBridgeMAC(void)
        {
            return  m_bridgeData.id;
        }

        /**
         * Queries the hue bridge for the new lights associated with it.
         *
         * @return MPM_RESULT_OK on success, or another MPM_RESULT_XXX on error.
         */

        MPMResult discoverHueLights();

        MPMResult getBridgeConfigFromCloud();

        void getBridgeConfig(hue_bridge_data_t &data)
        {
            data = m_bridgeData;
        }

        /**
         * Gets all the lights that are associated with the bridge.
         *
         * @param[in] huelights is a vector of HueLight instances. Each
         * one representing a physical light associated with the bridge.
         */
        void getScannedLights( HueLight::lights &lights);


        void fillLightDetails(std::shared_ptr<HueLight> light);


    private:
        hue_bridge_data_t m_bridgeData;
        std::string m_curlQuery;

        /*Vector of lights associated with the bridge*/
        lights lightsFound;
};

#endif /* __HUE_BRIDGE_H__ */
