/******************************************************************
*
* Copyright 2018 Open Connectivity Foundation All Rights Reserved.
*
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

#include "SampleResourceFactory.h"
#include "IUTSimulatorUtils.h"

std::shared_ptr< SampleResource > SampleResourceFactory::createResource(
    const std::string& resourceUri,
    const std::string& resourceType,
    const std::string& resourceInterface,
    const uint8_t& resourceProperty)
{
    std::shared_ptr< SampleResource > resource;

    if (resourceType == SWITCH_RESOURCE_TYPE)
    {
        resource = std::make_shared< SampleResource >(
            resourceUri,
            SWITCH_RESOURCE_TYPE,
            (resourceInterface.empty() ? SWITCH_RESOURCE_INTERFACE : resourceInterface),
            resourceProperty,
            IUTSimulatorUtils::createRepresentation({
                {ON_OFF_KEY, true}
            })
        );

        return resource;
    }
    else if (resourceType == CON_RESOURCE_TYPE)
    {
        resource = std::make_shared< SampleResource >(
            resourceUri,
            CON_RESOURCE_TYPE,
            (resourceInterface.empty() ? CON_RESOURCE_INTERFACE : resourceInterface),
            resourceProperty,
            IUTSimulatorUtils::createRepresentation({
                {NAME_KEY, string(SampleResource::getDeviceInfo().deviceName)},
                {LOCATION_KEY, vector<double>{LATTITUDE_VALUE, LONGITUDE_VALUE}},
                {CURRENCY_KEY, string(CURRENCY_VALUE)},
                {DEFAULT_LANGUAGE_KEY, string(DEFAULT_LANGUAGE_VALUE)}
            })
        );

        return resource;
    }
    else if (resourceType == MEDIA_SOURCE_LIST_RESOURCE_TYPE)
    {
        resource = std::make_shared< SampleResource >(
            resourceUri,
            MEDIA_SOURCE_LIST_RESOURCE_TYPE,
            (resourceInterface.empty() ? MEDIA_SOURCE_LIST_RESOURCE_INTERFACE : resourceInterface),
            resourceProperty,
            IUTSimulatorUtils::createRepresentation({
                {SOURCE_KEY, vector< OCRepresentation >
                    {
                        IUTSimulatorUtils::createRepresentation({
                            {SOURCE_NAME_KEY, string("HDMI-CEC")},
                            {SOURCE_NUMBER_KEY, 1},
                            {SOURCE_TYPE_KEY, string("videoPlusAudio")},
                            {STATUS_KEY, true},
                        }),
                        IUTSimulatorUtils::createRepresentation({
                            {SOURCE_NAME_KEY, string("dualRCA")},
                            {SOURCE_NUMBER_KEY, 1},
                            {SOURCE_TYPE_KEY, string("audioOnly")},
                            {STATUS_KEY, false},
                        })
                    }
                }
            })
        );

        return resource;
    }
    else if (resourceType == AUDIO_RESOURCE_TYPE)
    {
        resource = std::make_shared< SampleResource >(
            resourceUri,
            AUDIO_RESOURCE_TYPE,
            (resourceInterface.empty() ? AUDIO_RESOURCE_INTERFACE : resourceInterface),
            resourceProperty,
            IUTSimulatorUtils::createRepresentation({
                {MUTE_KEY, true},
                {VOLUME_KEY, 10}
            })
        );

        return resource;
    }
    else if (resourceType == TEMPERATURE_RESOURCE_TYPE)
    {
        resource = std::make_shared< SampleResource >(
            resourceUri,
            TEMPERATURE_RESOURCE_TYPE,
            (resourceInterface.empty() ? TEMPERATURE_RESOURCE_INTERFACE : resourceInterface),
            resourceProperty,
            IUTSimulatorUtils::createRepresentation({
                {UNITS_KEY, string(UNITS_VALUE)},
                {RANGE_KEY, vector<double>{LOWEST_TEMPERATURE_VALUE, HIGHEST_TEMPERATURE_VALUE}},
                {TEMPERATURE_KEY, DEFAULT_TEMPERATURE_VALUE}
            })
        );
        resource->setAsReadOnly(UNITS_KEY);
        resource->setAsReadOnly(RANGE_KEY);

        return resource;
    }
    else if (resourceType == DIMMING_RESOURCE_TYPE)
    {
        resource = std::make_shared< SampleResource >(
            resourceUri,
            DIMMING_RESOURCE_TYPE,
            (resourceInterface.empty() ? DIMMING_RESOURCE_INTERFACE : resourceInterface),
            resourceProperty,
            IUTSimulatorUtils::createRepresentation({
                {DIMMING_SETTING_KEY, DIMMING_VALUE}
            })
        );

        return resource;
    }
    else if (resourceType == CHROMA_RESOURCE_TYPE)
    {
        resource = std::make_shared< SampleResource >(
            resourceUri,
            CHROMA_RESOURCE_TYPE,
            (resourceInterface.empty() ? CHROMA_RESOURCE_INTERFACE : resourceInterface),
            resourceProperty,
            IUTSimulatorUtils::createRepresentation({
                {CHROMA_CSC_KEY, vector<double>{CSC_X_VALUE, CSC_Y_VALUE}},
                {CHROMA_CT_KEY, CT_VALUE},
                {CHROMA_HUE_KEY, HUE_VALUE},
                {CHROMA_SATURATION_KEY, SATURATION_VALUE}
            })
        );

        return resource;
    }
    else if (resourceType == AIR_FLOW_RESOURCE_TYPE)
    {
        resource = std::make_shared< SampleResource >(
            resourceUri,
            AIR_FLOW_RESOURCE_TYPE,
            (resourceInterface.empty() ? AIR_FLOW_RESOURCE_INTERFACE : resourceInterface),
            resourceProperty,
            IUTSimulatorUtils::createRepresentation({
                {DIRECTION_KEY, string(DEFAULT_DIRECTION_VALUE)},
                {SPEED_KEY, DEFAULT_SPEED_VALUE},
                {RANGE_KEY, vector<int>{LOWEST_AIR_FLOW_VALUE, HIGHEST_AIR_FLOW_VALUE}}
            })
        );
        resource->setAsReadOnly(RANGE_KEY);

        return resource;
    }
    else if (resourceType == TIMER_RESOURCE_TYPE)
    {
        resource = std::make_shared< SampleResource >(
            resourceUri,
            TIMER_RESOURCE_TYPE,
            (resourceInterface.empty() ? TIMER_RESOURCE_INTERFACE : resourceInterface),
            resourceProperty,
            IUTSimulatorUtils::createRepresentation({
                {TIMER_HOUR_KEY, DEFAULT_TIME_VALUE},
                {TIMER_MINUTE_KEY, DEFAULT_TIME_VALUE + DEFAULT_TIME_VALUE + DEFAULT_TIME_VALUE},
                {TIMER_SECOND_KEY, DEFAULT_TIME_VALUE + DEFAULT_TIME_VALUE + DEFAULT_TIME_VALUE},
                {TIMER_RESET_KEY, false}
            })
        );

        return resource;
    }
    else if (resourceType == SWING_RESOURCE_TYPE)
    {
        resource = std::make_shared< SampleResource >(
            resourceUri,
            SWING_RESOURCE_TYPE,
            (resourceInterface.empty() ? SWING_RESOURCE_INTERFACE : resourceInterface),
            resourceProperty,
            IUTSimulatorUtils::createRepresentation({
                {SWING_STATE_KEY, SWING_STATE_VALUE},
                {SWING_MOVEMENT_KEY, string(SWING_MOVEMENT_VALUE)},
                {SWING_SUPPOTED_DIRECTION_KEY, vector<string>{SWING_MOVEMENT_VALUE, SWING_MOVEMENT_OTHER_VALUE}}
            })
        );
        resource->setAsReadOnly(SWING_SUPPOTED_DIRECTION_KEY);

        return resource;
    }
    else if (resourceType == Device_TYPE_LIGHT)
    {
        resource = std::make_shared< SampleResource >(
            resourceUri,
            Device_TYPE_LIGHT,
            (resourceInterface.empty() ? LIGHT_DEVICE_INTERFACE : resourceInterface),
            resourceProperty,
            IUTSimulatorUtils::createRepresentation({
                {NAME_KEY, string(ENGLISH_NAME_VALUE)},
                {DEVICE_ID_KEY, string("")},
                {ICV_KEY, string(CORE_SPEC_VERSION)},
                {DMV_KEY, string(RESOURCE_TYPE_SPEC_VERSION) + "," + string(SMART_HOME_SPEC_VERSION)},
                {PIID_KEY, string(PLATFORM_ID)}
            })
        );
        resource->setAsReadOnly(NAME_KEY);
        resource->setAsReadOnly(DEVICE_ID_KEY);
        resource->setAsReadOnly(ICV_KEY);
        resource->setAsReadOnly(DMV_KEY);
        resource->setAsReadOnly(PIID_KEY);

        return resource;
    }
    else if (resourceType == BRIGHTNESS_RESOURCE_TYPE)
    {
        resource = std::make_shared< SampleResource >(
            resourceUri,
            BRIGHTNESS_RESOURCE_TYPE,
            (resourceInterface.empty() ? BRIGHTNESS_RESOURCE_INTERFACE : resourceInterface),
            resourceProperty,
            IUTSimulatorUtils::createRepresentation({
                {BRIGHTNESS_KEY, DEFAULT_BRIGHTNESS_VALUE}
            })
        );

        return resource;
    }
    else if (resourceType == RESOURCE_TYPE_LIGHT)
    {
        resource = std::make_shared< SampleResource >(
            resourceUri,
            RESOURCE_TYPE_LIGHT,
            (resourceInterface.empty() ? ACTUATOR_INTERFACE : resourceInterface),
            resourceProperty
        );

        return resource;
    }
    else if (resourceType == RESOURCE_TYPE_FAN)
    {
        resource = std::make_shared< SampleResource >(
            resourceUri,
            RESOURCE_TYPE_FAN,
            (resourceInterface.empty() ? ACTUATOR_INTERFACE : resourceInterface),
            resourceProperty
        );

        return resource;
    }
    else if (resourceType == MAINTENANCE_RESOURCE_TYPE)
    {
        resource = std::make_shared< MntResource >(
            MN_MAINTENANCE_URI,
            MAINTENANCE_RESOURCE_TYPE,
            (resourceInterface.empty() ? MAINTENANCE_RESOURCE_INTERFACE : resourceInterface),
            resourceProperty,
            IUTSimulatorUtils::createRepresentation({
                {FACTORY_KEY, false},
                {REBOOT_KEY, false},
                {HTML_ERR_KEY, HTML_ERR_DEFAULT_VALUE}
            })
        );

        return resource;
    }
    else if (resourceType == NETWORK_MONITORING_RESOURCE_TYPE)
    {
        resource = std::make_shared< NmonResource >(
            NMON_NETWORK_MONITORING_URI,
            NETWORK_MONITORING_RESOURCE_TYPE,
            (resourceInterface.empty() ? NETWORK_MONITORING_RESOURCE_INTERFACE : resourceInterface),
            resourceProperty,
            IUTSimulatorUtils::createRepresentation({
                {RESET_KEY, false},
                {COLLECTING_VALUES, false},
                {IANA_NETWORK_CONNECTION_KEY, IANA_NETWORK_CONNECTION_DEFAULT_VALUE}
            })
        );

        return resource;
    }
    else
    {
        throw std::invalid_argument("Resource Type: \"" + resourceType + "\" is not handled!");
    }
}

std::shared_ptr< SampleResource > SampleResourceFactory::createResource(
    const std::string& resourceUri,
    const std::string& resourceType,
    const uint8_t& resourceProperty)
{
    return SampleResourceFactory::createResource(resourceUri, resourceType, "", resourceProperty);
}