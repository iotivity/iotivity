//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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



#include "CMResourceHandlerFactory.h"
#include "../xsd/Elements.h"

#include "ResourceHandlers.h"
#include "ResourceTypeEnum.h"

CMResourceHandlerFactory::CMResourceHandlerFactory()
{

    registerResourceHandler(::RT_AVSource, "/devices/{deviceId}/avsources/{avsourceId}");
    registerResourceHandler(::RT_AVSources, "/devices/{deviceId}/avsources");
    registerResourceHandler(::RT_AccessPoint,
                            "/devices/{deviceId}/configuration/networks/{networkId}/wifi/accesspoints/{accesspointId}");
    registerResourceHandler(::RT_AccessPoints,
                            "/devices/{deviceId}/configuration/networks/{networkId}/wifi/accesspoints");
    registerResourceHandler(::RT_Action, "/actions/{actionId}");
    registerResourceHandler(::RT_Actions, "/actions");
    registerResourceHandler(::RT_Alarm, "/devices/{deviceId}/alarms/{alarmId}");
    registerResourceHandler(::RT_Alarms, "/devices/{deviceId}/alarms");
    registerResourceHandler(::RT_Audio, "/devices/{deviceId}/audio");
    registerResourceHandler(::RT_Camera, "/devices/{deviceId}/camera");
    registerResourceHandler(::RT_Capability, "/capability");
    registerResourceHandler(::RT_Capture, "/devices/{deviceId}/camera/captures/{captureId}");
    registerResourceHandler(::RT_Captures, "/devices/{deviceId}/camera/captures");
    registerResourceHandler(::RT_Configuration, "/devices/{deviceId}/configuration");
    registerResourceHandler(::RT_CustomRecipe,
                            "/devices/{deviceId}/oven/customrecipes/{customrecipeId}");
    registerResourceHandler(::RT_CustomRecipes, "/devices/{deviceId}/oven/customrecipes");
    registerResourceHandler(::RT_DRLC, "/devices/{deviceId}/drlc");
    registerResourceHandler(::RT_DefrostReservation,
                            "/devices/{deviceId}/fridge/defrostreservations/{defrostreservationId}");
    registerResourceHandler(::RT_DefrostReservations, "/devices/{deviceId}/fridge/defrostreservations");
    registerResourceHandler(::RT_Detection, "/devices/{deviceId}/detection");
    registerResourceHandler(::RT_Device, "/devices/{deviceId}");
    registerResourceHandler(::RT_Devices, "/devices");
    registerResourceHandler(::RT_Door, "/devices/{deviceId}/doors/{doorId}");
    registerResourceHandler(::RT_Doors, "/devices/{deviceId}/doors");
    registerResourceHandler(::RT_Energy, "/energy");
    registerResourceHandler(::RT_EnergyConsumption, "/devices/{deviceId}/energyconsumption");
    registerResourceHandler(::RT_Food, "/foodmanager/foodlist/{foodlistId}");
    registerResourceHandler(::RT_FoodImage, "/foodmanager/foodimages/{foodimagesId}");
    registerResourceHandler(::RT_FoodImages, "/foodmanager/foodimages");
    registerResourceHandler(::RT_FoodList, "/foodmanager/foodlist");
    registerResourceHandler(::RT_FoodManager, "/foodmanager");
    registerResourceHandler(::RT_Fridge, "/devices/{deviceId}/fridge");
    registerResourceHandler(::RT_Group, "/groups/{groupId}");
    registerResourceHandler(::RT_Groups, "/groups");
    registerResourceHandler(::RT_Humidity, "/devices/{deviceId}/humidity");
    registerResourceHandler(::RT_Information, "/devices/{deviceId}/information");
    registerResourceHandler(::RT_Level, "/devices/{deviceId}/level");
    registerResourceHandler(::RT_Light, "/devices/{deviceId}/light");
    registerResourceHandler(::RT_LongPollingNotification, "/longpollingnotification");
    registerResourceHandler(::RT_Message, "/devices/{deviceId}/messages/{messageId}");
    registerResourceHandler(::RT_Messages, "/devices/{deviceId}/messages");
    registerResourceHandler(::RT_Mode, "/devices/{deviceId}/mode");
    registerResourceHandler(::RT_Movement, "/devices/{deviceId}/movement");
    registerResourceHandler(::RT_Network, "/devices/{deviceId}/configuration/networks/{networkId}");
    registerResourceHandler(::RT_Networks, "/devices/{deviceId}/configuration/networks");
    registerResourceHandler(::RT_Notification, "/notifications/{notificationId}");
    registerResourceHandler(::RT_Notifications, "/notifications");
    registerResourceHandler(::RT_Operation, "/devices/{deviceId}/operation");
    registerResourceHandler(::RT_Oven, "/devices/{deviceId}/oven");
    registerResourceHandler(::RT_Pattern, "/patterns/{patternId}");
    registerResourceHandler(::RT_Patterns, "/patterns");
    registerResourceHandler(::RT_Record, "/records/{recordId}");
    registerResourceHandler(::RT_Records, "/records");
    registerResourceHandler(::RT_Remote, "/devices/{deviceId}/configuration/remote");
    registerResourceHandler(::RT_Shopping, "/foodmanager/shoppinglist/{shoppinglistId}");
    registerResourceHandler(::RT_ShoppingList, "/foodmanager/shoppinglist");
//registerResourceHandler(::RT_Subscription, "/subscriptions/{subscriptionId}");
//registerResourceHandler(::RT_Subscriptions, "/subscriptions");
    registerResourceHandler(::RT_Subscription, "/sub/{subscriptionId}");
    registerResourceHandler(::RT_Subscriptions, "/sub");
    registerResourceHandler(::RT_Temperature, "/devices/{deviceId}/temperatures/{temperatureId}");
    registerResourceHandler(::RT_Temperatures, "/devices/{deviceId}/temperatures");
    registerResourceHandler(::RT_Thermostat, "/devices/{deviceId}/thermostat");
    registerResourceHandler(::RT_Time, "/devices/{deviceId}/configuration/time");
    registerResourceHandler(::RT_UsageLimit, "/energy/usagelimits/{usagelimitId}");
    registerResourceHandler(::RT_UsageLimits, "/energy/usagelimits");
    registerResourceHandler(::RT_Usages, "/energy/usages");
    registerResourceHandler(::RT_Version, "/devices/{deviceId}/information/versions/{versionId}");
    registerResourceHandler(::RT_Versions, "/devices/{deviceId}/information/versions");
    registerResourceHandler(::RT_Washer, "/devices/{deviceId}/washer");
    registerResourceHandler(::RT_WiFi, "/devices/{deviceId}/configuration/networks/{networkId}/wifi");
    registerResourceHandler(::RT_Wind, "/devices/{deviceId}/wind");
    registerResourceHandler(::RT_DummyLight, "/a/light");
}

CMResourceHandlerFactory::~CMResourceHandlerFactory()
{

}

OC::Cm::Server::ResourceHandler::IResourceHandler *
CMResourceHandlerFactory::createResourceHandler(int resourceType)
{
    switch (resourceType)
    {
        case ::RT_Capability: { return new CapabilityResourceHandler(p_context) ;}
        case ::RT_Device: { return new DeviceResourceHandler(p_context) ;}
        case ::RT_Devices: { return new DevicesResourceHandler(p_context) ;}
    }

    return NULL ;
}
