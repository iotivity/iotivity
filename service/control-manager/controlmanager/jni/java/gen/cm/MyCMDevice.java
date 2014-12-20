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

import gen.server.handlers.CMResourceHandlerFactory;
import gen.xsd.ArrayOfResourceType;
import gen.xsd.Capability;
import gen.xsd.Information;
import gen.xsd.InformationLink;
import gen.xsd.String30;
import gen.xsd.String50;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import OC.Cm.Device;
import OC.Cm.DeviceDomain;
import OC.Cm.DeviceType;
import OC.Cm.Serialization.ISerializable;
import OC.Cm.Serialization.Xsd.UuidType;
import OC.Cm.Server.ResourceHandler.ResourceHandlerFactory;

/**
 * This class provide methods to contain self device information that only used
 * for a device discovery purpose.
 * 
 * @since 1.0.0
 */
public class MyCMDevice extends Device {

    public static final String             APP_ID          = "SelfApp";
    /**
     * VARIABLE CURRENTLY NOT USED. PLEASE DO NOT USE.
     */
    Information                            m_info          = new Information();

    /**
     * VARIABLE CURRENTLY NOT USED. PLEASE DO NOT USE.
     */
    Capability                             capability      = new Capability();

    /**
     * List string variable that contains all the resources supported by the
     * self device.
     */
    private List<String>                   m_resourceTypes = new ArrayList<String>();

    /**
     * Map variable that contains multiple MyCMDevice object corresponds to the
     * specific Key name.
     */
    private static Map<String, MyCMDevice> shpDeviceMap    = new HashMap<String, MyCMDevice>();

    /**
     * String variable that contains SubscriptionLink information.
     */
    String                                 m_subscriptionLink;

    /**
     * Returns a MyCMDevice object. This method is thread safe.
     * 
     * @param name
     *            Unique name of MyCMDevice object to be returned
     * 
     * @return MyCMDevice object.
     * @since 1.0.0
     */
    public static synchronized MyCMDevice getInstance(String name) {
        MyCMDevice device = shpDeviceMap.get(name);

        if (null == device) {
            device = new MyCMDevice();
            shpDeviceMap.put(name, device);
        }

        return device;
    }

    /**
     * Removes specific MyCMDevice object from the shpDeviceMap variable. This
     * method is thread safe.
     * 
     * @param contextName
     *            Unique name of MyCMDevice to be removed from the shpDeviceMap
     *            variable
     * @since 1.0.0
     */
    public static synchronized void destroyInstance(String contextName) {
        MyCMDevice device = shpDeviceMap.get(contextName);

        if (null != device)
            shpDeviceMap.remove(contextName);
    }

    /**
     * Default constructor of MyCMDevice class. Following default values will be
     * used to create a instance. (DeviceType.DeviceType_Unknown,
     * DeviceDomain.DeviceDomain_Local)
     * 
     * @since 1.0.0
     */
    private MyCMDevice() {
        super(DeviceType.DeviceType_Unknown, DeviceDomain.DeviceDomain_Local);
        m_subscriptionLink = "/Subscriptions";
    }

    /**
     * Return all the resources supported by the self device.
     * 
     * @return List of string that contains all the resources supported by the
     *         device
     */
    public List<String> getSupportedResourceTypes() {
        return m_resourceTypes;
    }

    /**
     * To set a supported resource information, user should repeatedly call this
     * method as many times as the number of resources that are supported by the
     * device.
     * 
     * @param resource
     *            Supported resource to be added in the m_resourceTypes private
     *            variable.
     * 
     * @return True if given device resource is successfully added / False if
     *         given device resource is not added
     * @since 1.0.0
     */
    public boolean setSupportedResourceType(String resource) {
        gen.xsd.ResourceType rt = new gen.xsd.ResourceType();
        rt.value = resource;

        if (false == rt.validateContent()) {
            System.out
                    .println("setSupportedResourceType() - Resource is invalid to set");
            return false;
        }

        if (-1 == m_resourceTypes.indexOf(resource)) {
            m_resourceTypes.add(resource);
            try {
                setSupportedResource(resource);
            } catch (Exception e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
        }

        return true;
    }

    /**
     * METHOD CURRENTLY NOT USED. PLEASE DO NOT USE.
     * 
     * @since 1.0.0
     */
    @Override
    public boolean loadDeviceDescription(ISerializable serializable) {
        return true;
    }

    /**
     * Returns a subscription link of the self device.
     * 
     * @return Subscription link of the self device
     * @since 1.0.0
     */
    @Override
    public String getSubscriptionsLink() {
        return m_subscriptionLink;
    }

    /**
     * METHOD CURRENTLY NOT USED. PLEASE DO NOT USE.
     * 
     * @since 1.0.0
     */
    @Override
    public ISerializable getDeviceDescription() {
        return null;
    }

    /**
     * Sets IP address of the self device.
     * 
     * @param address
     *            IP address of the device
     * @since 1.0.0
     */
    public void setAddress(String address) {
        try {
            super.setAddress(address);

            m_subscriptionLink = "/subscriptions";
            setDeviceDescriptionUri("/devices");
        } catch (Exception e) {
        }
    }

    /**
     * METHOD CURRENTLY NOT USED. PLEASE DO NOT USE.
     * 
     * @since 1.0.0
     */
    public boolean getDevices(gen.xsd.Devices devices) {
        try {
            gen.xsd.Device m_device = new gen.xsd.Device();
            m_device.connected = true;
            m_device.Description = new String50();
            m_device.Description.value = "OIC Controller";
            m_device.Name = new String30();
            m_device.Name.value = "Controller";
            m_device.Id = new String50();
            m_device.Id.value = "0";
            m_device.Uuid = new UuidType();
            m_device.Uuid.value = getUUID();
            m_device.Resources = new ArrayOfResourceType();
            if (m_resourceTypes.size() > 0) {

                for (Iterator<String> it = m_resourceTypes.iterator(); it
                        .hasNext();) {
                    gen.xsd.ResourceType resType = new gen.xsd.ResourceType();
                    resType.value = it.next();
                    m_device.Resources.members.add(resType);
                }
            } else {

                gen.xsd.ResourceType resType = new gen.xsd.ResourceType();
                resType.value = "Capability";
                m_device.Resources.members.add(resType);

                gen.xsd.ResourceType resType1 = new gen.xsd.ResourceType();
                resType1.value = "Information";
                m_device.Resources.members.add(resType1);

                gen.xsd.ResourceType resType2 = new gen.xsd.ResourceType();
                resType2.value = "Device";
                m_device.Resources.members.add(resType2);

                gen.xsd.ResourceType resType3 = new gen.xsd.ResourceType();
                resType3.value = "Devices";
                m_device.Resources.members.add(resType3);

            }

            m_device.Type = new gen.xsd.DeviceType();
            m_device.Type.value = "Smartphone";
            m_device.informationLink = new InformationLink();
            m_device.informationLink.href = "/devices/1/information";

            devices.device.add(m_device);
        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
            return false;
        }

        return true;
    }

    /**
     * Returns a capability information written in the given Capability object.
     * 
     * @param caps
     *            Capability object to be written
     * 
     * @return True if capability information is successfully written to the
     *         Capability object / False if given device resource is not added
     * @since 1.0.0
     */
    public boolean getCapability(Capability caps) {
        if (null == caps)
            return false;

        try {
            caps.devicesLink = new gen.xsd.DevicesLink();
            caps.devicesLink.href = getDeviceDescriptionUri();

            caps.subscriptionsLink = new gen.xsd.SubscriptionsLink();
            caps.subscriptionsLink.href = getSubscriptionsLink();
        } catch (Exception e) {
        }

        return true;
    }

    /**
     * METHOD CURRENTLY NOT USED. PLEASE DO NOT USE.
     * 
     * @since 1.0.0
     */
    public Information getInformation() {
        return m_info;
    }

    /**
     * Returns a ResourceHandlerFactory object of the self device. Depending on
     * the supported resource type, different number and type of resource will
     * be initiated and registered. Hence, it's important to call a
     * setSupportedResourceType() method first before calling this method.
     * 
     * @return Instantiated ResourceHandlerFactory object.
     * @since 1.0.0
     */
    public ResourceHandlerFactory getResourceHandlerFactory() {
        CMResourceHandlerFactory factory = new CMResourceHandlerFactory();
        List<ResourceType> resourceTypes = new ArrayList<ResourceType>();

        for (String s : m_resourceTypes) {
            if (s.equalsIgnoreCase("AVSource")) {
                resourceTypes.add(ResourceType.RT_AVSource);
            } else if (s.equalsIgnoreCase("AVSources")) {
                resourceTypes.add(ResourceType.RT_AVSources);
            } else if (s.equalsIgnoreCase("AccessPoint")) {
                resourceTypes.add(ResourceType.RT_AccessPoint);
            } else if (s.equalsIgnoreCase("AccessPoints")) {
                resourceTypes.add(ResourceType.RT_AccessPoints);
            } else if (s.equalsIgnoreCase("Action")) {
                resourceTypes.add(ResourceType.RT_Action);
            } else if (s.equalsIgnoreCase("Actions")) {
                resourceTypes.add(ResourceType.RT_Actions);
            } else if (s.equalsIgnoreCase("Alarm")) {
                resourceTypes.add(ResourceType.RT_Alarm);
            } else if (s.equalsIgnoreCase("Alarms")) {
                resourceTypes.add(ResourceType.RT_Alarms);
            } else if (s.equalsIgnoreCase("Audio")) {
                resourceTypes.add(ResourceType.RT_Audio);
            } else if (s.equalsIgnoreCase("Camera")) {
                resourceTypes.add(ResourceType.RT_Camera);
            } else if (s.equalsIgnoreCase("Capability")) {
                resourceTypes.add(ResourceType.RT_Capability);
            } else if (s.equalsIgnoreCase("Capture")) {
                resourceTypes.add(ResourceType.RT_Capture);
            } else if (s.equalsIgnoreCase("Captures")) {
                resourceTypes.add(ResourceType.RT_Captures);
            } else if (s.equalsIgnoreCase("Configuration")) {
                resourceTypes.add(ResourceType.RT_Configuration);
            } else if (s.equalsIgnoreCase("Consumable")) {
                resourceTypes.add(ResourceType.RT_Consumable);
            } else if (s.equalsIgnoreCase("Consumables")) {
                resourceTypes.add(ResourceType.RT_Consumables);
            } else if (s.equalsIgnoreCase("CustomRecipe")) {
                resourceTypes.add(ResourceType.RT_CustomRecipe);
            } else if (s.equalsIgnoreCase("CustomRecipes")) {
                resourceTypes.add(ResourceType.RT_CustomRecipes);
            } else if (s.equalsIgnoreCase("DRLC")) {
                resourceTypes.add(ResourceType.RT_DRLC);
            } else if (s.equalsIgnoreCase("DefrostReservation")) {
                resourceTypes.add(ResourceType.RT_DefrostReservation);
            } else if (s.equalsIgnoreCase("DefrostReservations")) {
                resourceTypes.add(ResourceType.RT_DefrostReservations);
            } else if (s.equalsIgnoreCase("Detection")) {
                resourceTypes.add(ResourceType.RT_Detection);
            } else if (s.equalsIgnoreCase("Device")) {
                resourceTypes.add(ResourceType.RT_Device);
            } else if (s.equalsIgnoreCase("Devices")) {
                resourceTypes.add(ResourceType.RT_Devices);
            } else if (s.equalsIgnoreCase("Door")) {
                resourceTypes.add(ResourceType.RT_Door);
            } else if (s.equalsIgnoreCase("Doors")) {
                resourceTypes.add(ResourceType.RT_Doors);
            } else if (s.equalsIgnoreCase("Energy")) {
                resourceTypes.add(ResourceType.RT_Energy);
            } else if (s.equalsIgnoreCase("EnergyConsumption")) {
                resourceTypes.add(ResourceType.RT_EnergyConsumption);
            } else if (s.equalsIgnoreCase("Food")) {
                resourceTypes.add(ResourceType.RT_Food);
            } else if (s.equalsIgnoreCase("FoodImage")) {
                resourceTypes.add(ResourceType.RT_FoodImage);
            } else if (s.equalsIgnoreCase("FoodImages")) {
                resourceTypes.add(ResourceType.RT_FoodImages);
            } else if (s.equalsIgnoreCase("FoodList")) {
                resourceTypes.add(ResourceType.RT_FoodList);
            } else if (s.equalsIgnoreCase("FoodManager")) {
                resourceTypes.add(ResourceType.RT_FoodManager);
            } else if (s.equalsIgnoreCase("Fridge")) {
                resourceTypes.add(ResourceType.RT_Fridge);
            } else if (s.equalsIgnoreCase("Group")) {
                resourceTypes.add(ResourceType.RT_Group);
            } else if (s.equalsIgnoreCase("Groups")) {
                resourceTypes.add(ResourceType.RT_Groups);
            } else if (s.equalsIgnoreCase("Humidity")) {
                resourceTypes.add(ResourceType.RT_Humidity);
            } else if (s.equalsIgnoreCase("Information")) {
                resourceTypes.add(ResourceType.RT_Information);
            } else if (s.equalsIgnoreCase("Level")) {
                resourceTypes.add(ResourceType.RT_Level);
            } else if (s.equalsIgnoreCase("Light")) {
                resourceTypes.add(ResourceType.RT_Light);
            } else if (s.equalsIgnoreCase("LongPollingNotification")) {
                resourceTypes.add(ResourceType.RT_LongPollingNotification);
            } else if (s.equalsIgnoreCase("Message")) {
                resourceTypes.add(ResourceType.RT_Message);
            } else if (s.equalsIgnoreCase("Messages")) {
                resourceTypes.add(ResourceType.RT_Messages);
            } else if (s.equalsIgnoreCase("Mode")) {
                resourceTypes.add(ResourceType.RT_Mode);
            } else if (s.equalsIgnoreCase("Movement")) {
                resourceTypes.add(ResourceType.RT_Movement);
            } else if (s.equalsIgnoreCase("Network")) {
                resourceTypes.add(ResourceType.RT_Network);
            } else if (s.equalsIgnoreCase("Networks")) {
                resourceTypes.add(ResourceType.RT_Networks);
            } else if (s.equalsIgnoreCase("Operation")) {
                resourceTypes.add(ResourceType.RT_Operation);
            } else if (s.equalsIgnoreCase("Oven")) {
                resourceTypes.add(ResourceType.RT_Oven);
            } else if (s.equalsIgnoreCase("Pattern")) {
                resourceTypes.add(ResourceType.RT_Pattern);
            } else if (s.equalsIgnoreCase("Patterns")) {
                resourceTypes.add(ResourceType.RT_Patterns);
            } else if (s.equalsIgnoreCase("Record")) {
                resourceTypes.add(ResourceType.RT_Record);
            } else if (s.equalsIgnoreCase("Records")) {
                resourceTypes.add(ResourceType.RT_Records);
            } else if (s.equalsIgnoreCase("Remote")) {
                resourceTypes.add(ResourceType.RT_Remote);
            } else if (s.equalsIgnoreCase("Sensor")) {
                resourceTypes.add(ResourceType.RT_Sensor);
            } else if (s.equalsIgnoreCase("Sensors")) {
                resourceTypes.add(ResourceType.RT_Sensors);
            } else if (s.equalsIgnoreCase("Shopping")) {
                resourceTypes.add(ResourceType.RT_Shopping);
            } else if (s.equalsIgnoreCase("ShoppingList")) {
                resourceTypes.add(ResourceType.RT_ShoppingList);
            } else if (s.equalsIgnoreCase("Temperature")) {
                resourceTypes.add(ResourceType.RT_Temperature);
            } else if (s.equalsIgnoreCase("Temperatures")) {
                resourceTypes.add(ResourceType.RT_Temperatures);
            } else if (s.equalsIgnoreCase("Thermostat")) {
                resourceTypes.add(ResourceType.RT_Thermostat);
            } else if (s.equalsIgnoreCase("Time")) {
                resourceTypes.add(ResourceType.RT_Time);
            } else if (s.equalsIgnoreCase("UsageLimit")) {
                resourceTypes.add(ResourceType.RT_UsageLimit);
            } else if (s.equalsIgnoreCase("UsageLimits")) {
                resourceTypes.add(ResourceType.RT_UsageLimits);
            } else if (s.equalsIgnoreCase("Usages")) {
                resourceTypes.add(ResourceType.RT_Usages);
            } else if (s.equalsIgnoreCase("Version")) {
                resourceTypes.add(ResourceType.RT_Version);
            } else if (s.equalsIgnoreCase("Versions")) {
                resourceTypes.add(ResourceType.RT_Versions);
            } else if (s.equalsIgnoreCase("Washer")) {
                resourceTypes.add(ResourceType.RT_Washer);
            } else if (s.equalsIgnoreCase("WiFi")) {
                resourceTypes.add(ResourceType.RT_WiFi);
            } else if (s.equalsIgnoreCase("Wind")) {
                resourceTypes.add(ResourceType.RT_Wind);
            }
        }

        factory.setSupportedResources(resourceTypes);
        factory.device = this;

        return factory;
    }

}
