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

package gen.server.handlers;

import gen.cm.MyCMDevice;
import gen.cm.ResourceType;

import java.util.List;

import OC.Cm.Server.ResourceHandler.IResourceHandler;
import OC.Cm.Server.ResourceHandler.ResourceHandlerFactory;

public class CMResourceHandlerFactory extends ResourceHandlerFactory {
    public List<ResourceType> resourceType = null;
    public MyCMDevice         device;

    public CMResourceHandlerFactory() {

        registerResource(ResourceType.RT_AVSource.getValue(),
                "/devices/{deviceId}/avsources/{avsourceId}");
        registerResource(ResourceType.RT_AVSources.getValue(),
                "/devices/{deviceId}/avsources");
        registerResource(
                ResourceType.RT_AccessPoint.getValue(),
                "/devices/{deviceId}/configuration/networks/{networkId}/wifi/accesspoints/{accesspointId}");
        registerResource(ResourceType.RT_AccessPoints.getValue(),
                "/devices/{deviceId}/configuration/networks/{networkId}/wifi/accesspoints");
        registerResource(ResourceType.RT_Action.getValue(),
                "/actions/{actionId}");
        registerResource(ResourceType.RT_Actions.getValue(), "/actions");
        registerResource(ResourceType.RT_Alarm.getValue(),
                "/devices/{deviceId}/alarms/{alarmId}");
        registerResource(ResourceType.RT_Alarms.getValue(),
                "/devices/{deviceId}/alarms");
        registerResource(ResourceType.RT_Audio.getValue(),
                "/devices/{deviceId}/audio");
        registerResource(ResourceType.RT_Camera.getValue(),
                "/devices/{deviceId}/camera");
        registerResource(ResourceType.RT_Capability.getValue(), "/capability");
        registerResource(ResourceType.RT_Capture.getValue(),
                "/devices/{deviceId}/camera/captures/{captureId}");
        registerResource(ResourceType.RT_Captures.getValue(),
                "/devices/{deviceId}/camera/captures");
        registerResource(ResourceType.RT_Configuration.getValue(),
                "/devices/{deviceId}/configuration");
        registerResource(ResourceType.RT_Consumable.getValue(),
                "/devices/{deviceId}/consumables/{consumableId}");
        registerResource(ResourceType.RT_Consumables.getValue(),
                "/devices/{deviceId}/consumables");
        registerResource(ResourceType.RT_CustomRecipe.getValue(),
                "/devices/{deviceId}/oven/customrecipes/{customrecipeId}");
        registerResource(ResourceType.RT_CustomRecipes.getValue(),
                "/devices/{deviceId}/oven/customrecipes");
        registerResource(ResourceType.RT_DRLC.getValue(),
                "/devices/{deviceId}/drlc");
        registerResource(ResourceType.RT_DefrostReservation.getValue(),
                "/devices/{deviceId}/fridge/defrostreservations/{defrostreservationId}");
        registerResource(ResourceType.RT_DefrostReservations.getValue(),
                "/devices/{deviceId}/fridge/defrostreservations");
        registerResource(ResourceType.RT_Detection.getValue(),
                "/devices/{deviceId}/detection");
        registerResource(ResourceType.RT_Device.getValue(),
                "/devices/{deviceId}");
        registerResource(ResourceType.RT_Devices.getValue(), "/devices");
        registerResource(ResourceType.RT_Door.getValue(),
                "/devices/{deviceId}/doors/{doorId}");
        registerResource(ResourceType.RT_Doors.getValue(),
                "/devices/{deviceId}/doors");
        registerResource(ResourceType.RT_Energy.getValue(), "/energy");
        registerResource(ResourceType.RT_EnergyConsumption.getValue(),
                "/devices/{deviceId}/energyconsumption");
        registerResource(ResourceType.RT_Food.getValue(),
                "/foodmanager/foodlist/{foodlistId}");
        registerResource(ResourceType.RT_FoodImage.getValue(),
                "/foodmanager/foodimages/{foodimagesId}");
        registerResource(ResourceType.RT_FoodImages.getValue(),
                "/foodmanager/foodimages");
        registerResource(ResourceType.RT_FoodList.getValue(),
                "/foodmanager/foodlist");
        registerResource(ResourceType.RT_FoodManager.getValue(), "/foodmanager");
        registerResource(ResourceType.RT_Fridge.getValue(),
                "/devices/{deviceId}/fridge");
        registerResource(ResourceType.RT_Group.getValue(), "/groups/{groupId}");
        registerResource(ResourceType.RT_Groups.getValue(), "/groups");
        registerResource(ResourceType.RT_Humidity.getValue(),
                "/devices/{deviceId}/humidity");
        registerResource(ResourceType.RT_Information.getValue(),
                "/devices/{deviceId}/information");
        registerResource(ResourceType.RT_Level.getValue(),
                "/devices/{deviceId}/level");
        registerResource(ResourceType.RT_Light.getValue(),
                "/devices/{deviceId}/light");
        registerResource(ResourceType.RT_LongPollingNotification.getValue(),
                "/longpollingnotification");
        registerResource(ResourceType.RT_Message.getValue(),
                "/devices/{deviceId}/messages/{messageId}");
        registerResource(ResourceType.RT_Messages.getValue(),
                "/devices/{deviceId}/messages");
        registerResource(ResourceType.RT_Mode.getValue(),
                "/devices/{deviceId}/mode");
        registerResource(ResourceType.RT_Movement.getValue(),
                "/devices/{deviceId}/movement");
        registerResource(ResourceType.RT_Network.getValue(),
                "/devices/{deviceId}/configuration/networks/{networkId}");
        registerResource(ResourceType.RT_Networks.getValue(),
                "/devices/{deviceId}/configuration/networks");
        registerResource(ResourceType.RT_Notification.getValue(),
                "/notifications/{notificationId}");
        registerResource(ResourceType.RT_Notifications.getValue(),
                "/notifications");
        registerResource(ResourceType.RT_Operation.getValue(),
                "/devices/{deviceId}/operation");
        registerResource(ResourceType.RT_Oven.getValue(),
                "/devices/{deviceId}/oven");
        registerResource(ResourceType.RT_Pattern.getValue(),
                "/patterns/{patternId}");
        registerResource(ResourceType.RT_Patterns.getValue(), "/patterns");
        registerResource(ResourceType.RT_Record.getValue(),
                "/records/{recordId}");
        registerResource(ResourceType.RT_Records.getValue(), "/records");
        registerResource(ResourceType.RT_Remote.getValue(),
                "/devices/{deviceId}/configuration/remote");
        registerResource(ResourceType.RT_Sensor.getValue(),
                "/devices/{deviceId}/sensors/{sensorId}");
        registerResource(ResourceType.RT_Sensors.getValue(),
                "/devices/{deviceId}/sensors");
        registerResource(ResourceType.RT_Shopping.getValue(),
                "/foodmanager/shoppinglist/{shoppinglistId}");
        registerResource(ResourceType.RT_ShoppingList.getValue(),
                "/foodmanager/shoppinglist");
        // registerResource(ResourceType.RT_Subscription.getValue(),
        // "/subscriptions/{subscriptionId}");
        // registerResource(ResourceType.RT_Subscriptions.getValue(),
        // "/subscriptions");
        registerResource(ResourceType.RT_Subscription.getValue(),
                "/sub/{subscriptionId}");
        registerResource(ResourceType.RT_Subscriptions.getValue(), "/sub");
        registerResource(ResourceType.RT_Temperature.getValue(),
                "/devices/{deviceId}/temperatures/{temperatureId}");
        registerResource(ResourceType.RT_Temperatures.getValue(),
                "/devices/{deviceId}/temperatures");
        registerResource(ResourceType.RT_Thermostat.getValue(),
                "/devices/{deviceId}/thermostat");
        registerResource(ResourceType.RT_Time.getValue(),
                "/devices/{deviceId}/configuration/time");
        registerResource(ResourceType.RT_UsageLimit.getValue(),
                "/energy/usagelimits/{usagelimitId}");
        registerResource(ResourceType.RT_UsageLimits.getValue(),
                "/energy/usagelimits");
        registerResource(ResourceType.RT_Usages.getValue(), "/energy/usages");
        registerResource(ResourceType.RT_Version.getValue(),
                "/devices/{deviceId}/information/versions/{versionId}");
        registerResource(ResourceType.RT_Versions.getValue(),
                "/devices/{deviceId}/information/versions");
        registerResource(ResourceType.RT_Washer.getValue(),
                "/devices/{deviceId}/washer");
        registerResource(ResourceType.RT_WiFi.getValue(),
                "/devices/{deviceId}/configuration/networks/{networkId}/wifi");
        registerResource(ResourceType.RT_Wind.getValue(),
                "/devices/{deviceId}/wind");
    }

    @Override
    public long createResourceHandler(int resourceType) {
        IResourceHandler handler = getResourceHandler(resourceType);
        if (null != handler) {
            return handler.getGlobalRef();
        } else {
            return 0;
        }
    }

    public IResourceHandler getResourceHandler(int resourceType) {

        IResourceHandler handler = null;
        ResourceType type = ResourceType.getResourceType(resourceType);

        if (this.resourceType.contains(type) == false) {
            return null;
        }

        switch (ResourceType.getResourceType(resourceType)) {

            case RT_Capability: {
                handler = new CapabilityResourceHandler();
                ((OC.Cm.Server.ResourceHandler.SyncResourceHandler) (handler))
                        .setDeviceAndResourceDetails(device, "/capability");
                break;
            }
            case RT_Device: {
                handler = new DeviceResourceHandler();
                ((OC.Cm.Server.ResourceHandler.SyncResourceHandler) (handler))
                        .setDeviceAndResourceDetails(device,
                                "/devices/{deviceId}");
                break;
            }
            case RT_Devices: {
                handler = new DevicesResourceHandler();
                ((OC.Cm.Server.ResourceHandler.SyncResourceHandler) (handler))
                        .setDeviceAndResourceDetails(device, "/devices");
                break;
            }
        }

        return handler;
    }

    public void setSupportedResources(List<ResourceType> resourceTypes) {
        this.resourceType = resourceTypes;
    }
}
