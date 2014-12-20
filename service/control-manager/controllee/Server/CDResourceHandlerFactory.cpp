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

#include "CDResourceHandlerFactory.h"
#include "Elements.h"

#include "ResourceHandlers.h"
#include "ResourceTypeEnum.h"

CDResourceHandlerFactory::CDResourceHandlerFactory()
{
    pSubscriptionFactory = NULL;
    pApplicationFactory = NULL;
    registerResourceHandler(::RT_AVSource,
                            "/devices/{deviceId}/avsources/{avsourceId}");
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
    registerResourceHandler(::RT_Capture,
                            "/devices/{deviceId}/camera/captures/{captureId}");
    registerResourceHandler(::RT_Captures,
                            "/devices/{deviceId}/camera/captures");
    registerResourceHandler(::RT_Configuration,
                            "/devices/{deviceId}/configuration");
    registerResourceHandler(::RT_CustomRecipe,
                            "/devices/{deviceId}/oven/customrecipes/{customrecipeId}");
    registerResourceHandler(::RT_CustomRecipes,
                            "/devices/{deviceId}/oven/customrecipes");
    registerResourceHandler(::RT_DRLC, "/devices/{deviceId}/drlc");
    registerResourceHandler(::RT_DefrostReservation,
                            "/devices/{deviceId}/fridge/defrostreservations/{defrostreservationId}");
    registerResourceHandler(::RT_DefrostReservations,
                            "/devices/{deviceId}/fridge/defrostreservations");
    registerResourceHandler(::RT_Detection, "/devices/{deviceId}/detection");
    registerResourceHandler(::RT_Device, "/devices/{deviceId}");
    registerResourceHandler(::RT_Devices, "/devices");
    registerResourceHandler(::RT_Door, "/devices/{deviceId}/doors/{doorId}");
    registerResourceHandler(::RT_Doors, "/devices/{deviceId}/doors");
    registerResourceHandler(::RT_Energy, "/energy");
    registerResourceHandler(::RT_EnergyConsumption,
                            "/devices/{deviceId}/energyconsumption");
    registerResourceHandler(::RT_Food, "/foodmanager/foodlist/{foodlistId}");
    registerResourceHandler(::RT_FoodImage,
                            "/foodmanager/foodimages/{foodimagesId}");
    registerResourceHandler(::RT_FoodImages, "/foodmanager/foodimages");
    registerResourceHandler(::RT_FoodList, "/foodmanager/foodlist");
    registerResourceHandler(::RT_FoodManager, "/foodmanager");
    registerResourceHandler(::RT_Fridge, "/devices/{deviceId}/fridge");
    registerResourceHandler(::RT_Group, "/groups/{groupId}");
    registerResourceHandler(::RT_Groups, "/groups");
    registerResourceHandler(::RT_Humidity, "/devices/{deviceId}/humidity");
    registerResourceHandler(::RT_Information,
                            "/devices/{deviceId}/information");
    registerResourceHandler(::RT_Level, "/devices/{deviceId}/level");
    registerResourceHandler(::RT_Light, "/devices/{deviceId}/light");
    registerResourceHandler(::RT_LongPollingNotification,
                            "/longpollingnotification");
    registerResourceHandler(::RT_Message,
                            "/devices/{deviceId}/messages/{messageId}");
    registerResourceHandler(::RT_Messages, "/devices/{deviceId}/messages");
    registerResourceHandler(::RT_Mode, "/devices/{deviceId}/mode");
    registerResourceHandler(::RT_Movement, "/devices/{deviceId}/movement");
    registerResourceHandler(::RT_Network,
                            "/devices/{deviceId}/configuration/networks/{networkId}");
    registerResourceHandler(::RT_Networks,
                            "/devices/{deviceId}/configuration/networks");
    registerResourceHandler(::RT_Notification,
                            "/notifications/{notificationId}");
    registerResourceHandler(::RT_Notifications, "/notifications");
    registerResourceHandler(::RT_Operation, "/devices/{deviceId}/operation");
    registerResourceHandler(::RT_Oven, "/devices/{deviceId}/oven");
    registerResourceHandler(::RT_Pattern, "/patterns/{patternId}");
    registerResourceHandler(::RT_Patterns, "/patterns");
    registerResourceHandler(::RT_Record, "/records/{recordId}");
    registerResourceHandler(::RT_Records, "/records");
    registerResourceHandler(::RT_Remote,
                            "/devices/{deviceId}/configuration/remote");
    registerResourceHandler(::RT_Shopping,
                            "/foodmanager/shoppinglist/{shoppinglistId}");
    registerResourceHandler(::RT_ShoppingList, "/foodmanager/shoppinglist");
    registerResourceHandler(::RT_Subscription, "/sub/{subscriptionId}");
    registerResourceHandler(::RT_Subscriptions, "/sub");
    registerResourceHandler(::RT_Temperature,
                            "/devices/{deviceId}/temperatures/{temperatureId}");
    registerResourceHandler(::RT_Temperatures,
                            "/devices/{deviceId}/temperatures");
    registerResourceHandler(::RT_Thermostat, "/devices/{deviceId}/thermostat");
    registerResourceHandler(::RT_Time,
                            "/devices/{deviceId}/configuration/time");
    registerResourceHandler(::RT_UsageLimit,
                            "/energy/usagelimits/{usagelimitId}");
    registerResourceHandler(::RT_UsageLimits, "/energy/usagelimits");
    registerResourceHandler(::RT_Usages, "/energy/usages");
    registerResourceHandler(::RT_Version,
                            "/devices/{deviceId}/information/versions/{versionId}");
    registerResourceHandler(::RT_Versions,
                            "/devices/{deviceId}/information/versions");
    registerResourceHandler(::RT_Washer, "/devices/{deviceId}/washer");
    registerResourceHandler(::RT_WiFi,
                            "/devices/{deviceId}/configuration/networks/{networkId}/wifi");
    registerResourceHandler(::RT_Wind, "/devices/{deviceId}/wind");
    registerResourceHandler(::RT_DummyLight, "/a/light");
}

CDResourceHandlerFactory::~CDResourceHandlerFactory()
{

}

OC::Cm::Server::ResourceHandler::IResourceHandler *
CDResourceHandlerFactory::createResourceHandler(int resourceType)
{
    switch (resourceType)
    {

#if 0 // OIC IGNORE
        case ::RT_AVSource: {return new AVSourceResourceHandler(p_context);}
        case ::RT_AVSources: {return new AVSourcesResourceHandler(p_context);}
        case ::RT_Action: {return new ActionResourceHandler(p_context);}
        case ::RT_Actions: {return new ActionsResourceHandler(p_context);}
        case ::RT_Audio: {return new AudioResourceHandler(p_context);}
        case ::RT_Camera: {return new CameraResourceHandler(p_context);}
        case ::RT_Capture: {return new CaptureResourceHandler(p_context);}
        case ::RT_Captures: {return new CapturesResourceHandler(p_context);}
        case ::RT_CustomRecipe: {return new CustomRecipeResourceHandler(p_context);}
        case ::RT_CustomRecipes: {return new CustomRecipesResourceHandler(p_context);}
        case ::RT_DefrostReservation: {return new DefrostReservationResourceHandler(p_context);}
        case ::RT_DefrostReservations: {return new DefrostReservationsResourceHandler(p_context);}
        case ::RT_Food: {return new FoodResourceHandler(p_context);}
        case ::RT_FoodImage: {return new FoodImageResourceHandler(p_context);}
        case ::RT_FoodImages: {return new FoodImagesResourceHandler(p_context);}
        case ::RT_FoodList: {return new FoodListResourceHandler(p_context);}
        case ::RT_FoodManager: {return new FoodManagerResourceHandler(p_context);}
        case ::RT_Fridge: {return new FridgeResourceHandler(p_context);}
        case ::RT_Group: {return new GroupResourceHandler(p_context);}
        case ::RT_Groups: {return new GroupsResourceHandler(p_context);}
        case ::RT_Oven: {return new OvenResourceHandler(p_context);}
        case ::RT_Pattern: {return new PatternResourceHandler(p_context);}
        case ::RT_Patterns: {return new PatternsResourceHandler(p_context);}
        case ::RT_Remote: {return new RemoteResourceHandler(p_context);}
        case ::RT_Shopping: {return new ShoppingResourceHandler(p_context);}
        case ::RT_ShoppingList: {return new ShoppingListResourceHandler(p_context);}
        case ::RT_Washer: {return new WasherResourceHandler(p_context);}
        case ::RT_Wind: {return new WindResourceHandler(p_context);}
#endif // OIC IGNORE

        case ::RT_AccessPoint:
            {
                return new AccessPointResourceHandler(p_context);
            }
        case ::RT_AccessPoints:
            {
                return new AccessPointsResourceHandler(p_context);
            }

        case ::RT_Alarm:
            {
                return new AlarmResourceHandler(p_context);
            }
        case ::RT_Alarms:
            {
                return new AlarmsResourceHandler(p_context);
            }

        case ::RT_Capability:
            {
                return new CapabilityResourceHandler(p_context);
            }

        case ::RT_Configuration:
            {
                return new ConfigurationResourceHandler(p_context);
            }

        case ::RT_DRLC:
            {
                return new DRLCResourceHandler(p_context);
            }

        case ::RT_Detection:
            {
                return new DetectionResourceHandler(p_context);
            }
        case ::RT_Device:
            {
                return new DeviceResourceHandler(p_context);
            }
        case ::RT_Devices:
            {
                return new DevicesResourceHandler(p_context);
            }
        case ::RT_Door:
            {
                return new DoorResourceHandler(p_context);
            }
        case ::RT_Doors:
            {
                return new DoorsResourceHandler(p_context);
            }
        case ::RT_Energy:
            {
                return new EnergyResourceHandler(p_context);
            }
        case ::RT_EnergyConsumption:
            {
                return new EnergyConsumptionResourceHandler(p_context);
            }

        case ::RT_Humidity:
            {
                return new HumidityResourceHandler(p_context);
            }
        case ::RT_Information:
            {
                return new InformationResourceHandler(p_context);
            }
        case ::RT_Level:
            {
                return new LevelResourceHandler(p_context);
            }
        case ::RT_Light:
            {
                return new LightResourceHandler(p_context);
            }
        case ::RT_LongPollingNotification:
            {
                return new LongPollingNotificationResourceHandler(p_context);
            }
        case ::RT_Message:
            {
                return new MessageResourceHandler(p_context);
            }
        case ::RT_Messages:
            {
                return new MessagesResourceHandler(p_context);
            }
        case ::RT_Mode:
            {
                return new ModeResourceHandler(p_context);
            }
        case ::RT_Movement:
            {
                return new MovementResourceHandler(p_context);
            }
        case ::RT_Network:
            {
                return new NetworkResourceHandler(p_context);
            }
        case ::RT_Networks:
            {
                return new NetworksResourceHandler(p_context);
            }
        case ::RT_Operation:
            {
                return new OperationResourceHandler(p_context);
            }

        case ::RT_Record:
            {
                return new RecordResourceHandler(p_context);
            }
        case ::RT_Records:
            {
                return new RecordsResourceHandler(p_context);
            }
        case ::RT_Remote:
            {
                return new RemoteResourceHandler(p_context);
            }
        case ::RT_Temperature:
            {
                return new TemperatureResourceHandler(p_context);
            }
        case ::RT_Temperatures:
            {
                return new TemperaturesResourceHandler(p_context);
            }
        case ::RT_Thermostat:
            {
                return new ThermostatResourceHandler(p_context);
            }
        case ::RT_Time:
            {
                return new TimeResourceHandler(p_context);
            }
        case ::RT_UsageLimit:
            {
                return new UsageLimitResourceHandler(p_context);
            }
        case ::RT_UsageLimits:
            {
                return new UsageLimitsResourceHandler(p_context);
            }
        case ::RT_Usages:
            {
                return new UsagesResourceHandler(p_context);
            }
        case ::RT_Version:
            {
                return new VersionResourceHandler(p_context);
            }
        case ::RT_Versions:
            {
                return new VersionsResourceHandler(p_context);
            }

        case ::RT_WiFi:
            {
                return new WiFiResourceHandler(p_context);
            }

    }

    return NULL;
}

bool CDResourceHandlerFactory::deleteResourceHandler(
    OC::Cm::Server::ResourceHandler::IResourceHandler *handler,
    int resourceType)
{

    if (handler != NULL)
    {
        delete handler;
        return true;
    }
    return false;
}
