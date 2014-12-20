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

package gen.cm;

/**
 * This enum contains list of available resources on this device.
 * 
 * @since 1.0.0
 */
public enum ResourceType {
    RT_Unknown(-1), RT_AVSource(0), RT_AVSources(1), RT_AccessPoint(2), RT_AccessPoints(
            3), RT_Action(4), RT_Actions(5), RT_Alarm(6), RT_Alarms(7), RT_Audio(
            8), RT_Camera(9), RT_Capability(10), RT_Capture(11), RT_Captures(12), RT_Configuration(
            13), RT_Consumable(14), RT_Consumables(15), RT_CustomRecipe(16), RT_CustomRecipes(
            17), RT_DRLC(18), RT_DefrostReservation(19), RT_DefrostReservations(
            20), RT_Detection(21), RT_Device(22), RT_Devices(23), RT_Door(24), RT_Doors(
            25), RT_Energy(26), RT_EnergyConsumption(27), RT_Food(28), RT_FoodImage(
            29), RT_FoodImages(30), RT_FoodList(31), RT_FoodManager(32), RT_Fridge(
            33), RT_Group(34), RT_Groups(35), RT_Humidity(36), RT_Information(
            37), RT_Level(38), RT_Light(39), RT_LongPollingNotification(40), RT_Message(
            41), RT_Messages(42), RT_Mode(43), RT_Movement(44), RT_Network(45), RT_Networks(
            46), RT_Notification(47), RT_Notifications(48), RT_Operation(49), RT_Oven(
            50), RT_Pattern(51), RT_Patterns(52), RT_Record(53), RT_Records(54), RT_Remote(
            55), RT_Sensor(56), RT_Sensors(57), RT_Shopping(58), RT_ShoppingList(
            59), RT_Subscription(60), RT_Subscriptions(61), RT_Temperature(62), RT_Temperatures(
            63), RT_Thermostat(64), RT_Time(65), RT_UsageLimit(66), RT_UsageLimits(
            67), RT_Usages(68), RT_Version(69), RT_Versions(70), RT_Washer(71), RT_WiFi(
            72), RT_Wind(73);

    /**
     * Resource type variable
     */
    private int value;

    /**
     * Constructor of ResourceType enum.
     * 
     * @param value
     *            Type of resource type desired to set
     * @since 1.0.0
     */
    private ResourceType(int value) {
        this.value = value;
    }

    /**
     * Default constructor of ResourceType enum.
     * 
     * @since 1.0.0
     */
    private ResourceType() {
    }

    /**
     * Returns resource type value which represents enum itself.
     * 
     * @return Current resource type value
     * @since 1.0.0
     */
    public int getValue() {
        return value;
    }

    /**
     * Returns ResourceType object which corresponds to the specific resource
     * type value.
     * 
     * @param value
     *            Resource type value which represents specific ResourceType
     *            enum
     * 
     * @return ResourceType object
     * @since 1.0.0
     */
    public static ResourceType getResourceType(int value) {
        for (ResourceType resourceType : ResourceType.values()) {
            if (resourceType.getValue() == value)
                return resourceType;
        }

        return RT_Unknown;
    }
}
