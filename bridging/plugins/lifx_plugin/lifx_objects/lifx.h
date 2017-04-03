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

#ifndef __LIFX_H__
#define __LIFX_H__


#include <vector>
#include <typeinfo>
#include <mpmErrorCode.h>

#define LIFX_BASE_URI           "https://api.lifx.com/v1/lights"
#define LIFX_LIST_LIGHTS_URI    LIFX_BASE_URI "/all"

class LifxLight
{
    public:

        typedef struct _lightState
        {
            double brightness;
            bool power;
            bool connected;
            double secondsSinceLastSeen;
        } lightState;

        typedef struct _lightConfig
        {
            std::string id;
            std::string uuid;
            std::string label;
            _lightConfig() {}
            _lightConfig(std::string light_id, std::string light_uuid, std::string light_label)
            {
                id = light_id;
                uuid = light_uuid;
                label = light_label;
            }
        } lightConfig;

        LifxLight() {}
        virtual ~LifxLight() {}

        LifxLight(lightState &state, lightConfig &config, const std::string &user)
        {
            this->user = user;
            this->state = state;
            this->config = config;
            this->uri = std::string(LIFX_BASE_URI).append("/").append(config.id);
        }

        /*Limit calls to refreshState. LifX API's limit calls
         * from an access token and only allow 60 calls every 60 seconds
         * as made clear @ https://api.developer.lifx.com/docs/rate-limits
         */
        MPMResult refreshState();

        MPMResult setPower(bool power);

        /**
         * Sets the brightness for this bulb.
         * @param[in] brightness Is in the range [0.0, 1.0].
         *        Values outside the interval are set to the nearest endpoint.
         * @return MPM_RESULT_OK if success, else appropriate error code on error
         */
        MPMResult setBrightness(double brightness);

        MPMResult static getLights(const std::string user,
                                   std::vector<std::shared_ptr<LifxLight> > &lights);

        void getUser(std::string &userID)
        {
            userID = user;
        }

        lightState state;
        lightConfig config;
        std::string uri;

    private:
        std::string user;

        MPMResult setState(std::string &setPowerRequest);
};

typedef std::shared_ptr<LifxLight> LifxLightSharedPtr;

#endif // __LIFX_H__
