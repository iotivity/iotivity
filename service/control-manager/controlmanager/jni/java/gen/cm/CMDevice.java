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

import gen.xsd.Capability;
import gen.xsd.Elements;

import java.util.ArrayList;
import java.util.List;

import OC.Cm.Device;
import OC.Cm.DeviceDomain;
import OC.Cm.DeviceType;
import OC.Cm.Serialization.ISerializable;

public class CMDevice extends Device {
    /**
     * boolean flag which indicates whether capability is loaded or not using
     * load device description
     */
    public boolean      isCapabilityLoaded = false;

    /**
     * boolean flag which indicates whether devices is loaded or not using load
     * device description
     */
    public boolean      isDevicesLoaded    = false;

    /**
     * Capability class variable
     */
    Capability          m_capabilities     = new Capability();

    /**
     * String variable that contains SubscriptionLink information.
     */
    public String       m_subscriptionsLink;

    /**
     * List string variable that contains all the resources supported by the
     * device.
     */
    public List<String> m_resourceTypes    = new ArrayList<String>();

    /**
     * Returns all the supported resource types list string.
     * 
     * @return Supported resource types list string
     */
    public List<String> getSupportedResourceTypes() {
        return m_resourceTypes;
    }

    /**
     * Constructor of SHPDevice class.
     * 
     * @param type
     *            Type of device (Air Conditioner, Washer, Dryer, etc.)
     * @param domain
     *            Device domain to which this device belongs (Local, Remote,
     *            All)
     * @since 1.0.0
     */
    public CMDevice(DeviceType type, DeviceDomain domain) {
        super(type, domain);

        isDevicesLoaded = false;
        isCapabilityLoaded = false;
    }

    /**
     * Returns subscriptions link.
     * 
     * @return Subscriptions link string
     * @since 1.0.0
     */
    @Override
    public String getSubscriptionsLink() {
        return m_subscriptionsLink;
    }

    /**
     * Configures the current instance of device object with specified device
     * description.
     * 
     * @param serializable
     *            Device description to be set
     * 
     * @return True if device description is successfully set / False if device
     *         description is failed to set
     * @since 1.0.0
     */
    @Override
    public boolean loadDeviceDescription(ISerializable serializable) {
        String elementName = serializable.getElementName();

        if (0 == elementName.compareTo(Elements.EN_CAPABILITY)) {
            m_capabilities = (Capability) serializable;

            try {
                if (m_capabilities.devicesLink.href.isEmpty() == false)
                    setDeviceDescriptionUri(m_capabilities.devicesLink.href);
            } catch (Exception e) {
                return false;
            }

            if (m_capabilities.subscriptionsLink.href.isEmpty() == false)
                m_subscriptionsLink = m_capabilities.subscriptionsLink.href;

            try {
                setDeviceDescriptionLoaded(getNativeHandle(), false);
            } catch (Exception e) {
                e.printStackTrace();
            }

            isCapabilityLoaded = true;
            return true;
        } else if (0 == elementName.compareTo(Elements.EN_DEVICE)) {
            gen.xsd.Device device = (gen.xsd.Device) serializable;

            if (device.Resources != null) {
                for (gen.xsd.ResourceType r : device.Resources.members) {
                    m_resourceTypes.add(r.value);
                }
            }

            try {
                setDeviceDescriptionLoaded(getNativeHandle(), true);
            } catch (Exception e) {
                e.printStackTrace();
            }
            isDevicesLoaded = true;
            return true;
        } else if (0 == elementName.compareTo(Elements.EN_DEVICES)) {
            gen.xsd.Devices devices = (gen.xsd.Devices) serializable;

            if (devices.device.size() <= 0)
                return false;

            gen.xsd.Device device = (gen.xsd.Device) devices.device.get(0);

            if (device.Resources != null) {
                for (gen.xsd.ResourceType r : device.Resources.members) {
                    m_resourceTypes.add(r.value);
                }
            }

            try {

                setDeviceType(getDeviceType(device.Type.value));

                if (device.Uuid.value != null) {
                    setUUID(device.Uuid.value);
                }
                setDeviceDescriptionLoaded(getNativeHandle(), true);
            } catch (Exception e) {
                e.printStackTrace();
            }

            isDevicesLoaded = true;
            return true;
        }

        return false;
    }

    public int getDeviceType(String typeStr) {
        String DeviceType_Strings[] = { "Unknown", "Gateway", "Smartphone",
                "Tablet", "TV", "Refrigerator", "Kimchi_Refrigerator",
                "Washer", "Dryer", "Air_Conditioner", "System_Air_Conditioner",
                "Dishwasher", "Oven", "Microwave_Oven", "Cooktop",
                "Robot_Cleaner", "Light", "IP_Camera", "Door_Lock",
                "Door_Phone", "Thermostat", "Water_Heater", "Smart_Plug",
                "Generic_Sensor", "Motion_Detector", "Heater", "Coffee_Pot",
                "Iron", "Vacuum_Cleaner", "Zigbee_Bridge", "Zwave_Bridge" };

        for (int i = 0; i < DeviceType_Strings.length; i++) {
            if (DeviceType_Strings[i].compareToIgnoreCase(typeStr) == 0) {
                return i;
            }
        }

        return 0;
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
     * METHOD CURRENTLY NOT USED. PLEASE DO NOT USE.
     */
    public OC.Cm.Client.Resource.Resource createResource(
            gen.cm.ResourceType resourceType) {
        String address;
        String protocol;
        long context;

        try {
            address = getAddress();
            context = getContext();
            protocol = getProtocol();
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
        if (resourceType == ResourceType.RT_AccessPoint) {
            return new gen.client.resource.AccessPointResource(context,
                    protocol, address);
        } else if (resourceType == ResourceType.RT_AccessPoints) {
            return new gen.client.resource.AccessPointsResource(context,
                    protocol, address);
        } else if (resourceType == ResourceType.RT_Capability) {
            return new gen.client.resource.CapabilityResource(context,
                    protocol, address);
        }

        else if (resourceType == ResourceType.RT_Configuration) {
            return new gen.client.resource.ConfigurationResource(context,
                    protocol, address);
        }

        else if (resourceType == ResourceType.RT_Detection) {
            return new gen.client.resource.DetectionResource(context, protocol,
                    address);
        } else if (resourceType == ResourceType.RT_Device) {
            return new gen.client.resource.DeviceResource(context, protocol,
                    address);
        } else if (resourceType == ResourceType.RT_Devices) {
            return new gen.client.resource.DevicesResource(context, protocol,
                    address);
        } else if (resourceType == ResourceType.RT_Door) {
            return new gen.client.resource.DoorResource(context, protocol,
                    address);
        } else if (resourceType == ResourceType.RT_Doors) {
            return new gen.client.resource.DoorsResource(context, protocol,
                    address);
        } else if (resourceType == ResourceType.RT_Energy) {
            return new gen.client.resource.EnergyResource(context, protocol,
                    address);
        } else if (resourceType == ResourceType.RT_EnergyConsumption) {
            return new gen.client.resource.EnergyConsumptionResource(context,
                    protocol, address);
        }

        else if (resourceType == ResourceType.RT_Humidity) {
            return new gen.client.resource.HumidityResource(context, protocol,
                    address);
        } else if (resourceType == ResourceType.RT_Information) {
            return new gen.client.resource.InformationResource(context,
                    protocol, address);
        } else if (resourceType == ResourceType.RT_Level) {
            return new gen.client.resource.LevelResource(context, protocol,
                    address);
        } else if (resourceType == ResourceType.RT_Light) {
            return new gen.client.resource.LightResource(context, protocol,
                    address);
        } else if (resourceType == ResourceType.RT_LongPollingNotification) {
            return new gen.client.resource.LongPollingNotificationResource(
                    context, protocol, address);
        } else if (resourceType == ResourceType.RT_Mode) {
            return new gen.client.resource.ModeResource(context, protocol,
                    address);
        } else if (resourceType == ResourceType.RT_Movement) {
            return new gen.client.resource.MovementResource(context, protocol,
                    address);
        } else if (resourceType == ResourceType.RT_Network) {
            return new gen.client.resource.NetworkResource(context, protocol,
                    address);
        } else if (resourceType == ResourceType.RT_Networks) {
            return new gen.client.resource.NetworksResource(context, protocol,
                    address);
        } else if (resourceType == ResourceType.RT_Notification) {
            return new gen.client.resource.NotificationResource(context,
                    protocol, address);
        } else if (resourceType == ResourceType.RT_Notifications) {
            return new gen.client.resource.NotificationsResource(context,
                    protocol, address);
        } else if (resourceType == ResourceType.RT_Operation) {
            return new gen.client.resource.OperationResource(context, protocol,
                    address);
        }

        else if (resourceType == ResourceType.RT_Subscription) {
            return new gen.client.resource.SubscriptionResource(context,
                    protocol, address);
        } else if (resourceType == ResourceType.RT_Subscriptions) {
            return new gen.client.resource.SubscriptionsResource(context,
                    protocol, address);
        } else if (resourceType == ResourceType.RT_Temperature) {
            return new gen.client.resource.TemperatureResource(context,
                    protocol, address);
        } else if (resourceType == ResourceType.RT_Temperatures) {
            return new gen.client.resource.TemperaturesResource(context,
                    protocol, address);
        } else if (resourceType == ResourceType.RT_Thermostat) {
            return new gen.client.resource.ThermostatResource(context,
                    protocol, address);
        } else if (resourceType == ResourceType.RT_Time) {
            return new gen.client.resource.TimeResource(context, protocol,
                    address);
        } else if (resourceType == ResourceType.RT_UsageLimit) {
            return new gen.client.resource.UsageLimitResource(context,
                    protocol, address);
        } else if (resourceType == ResourceType.RT_UsageLimits) {
            return new gen.client.resource.UsageLimitsResource(context,
                    protocol, address);
        } else if (resourceType == ResourceType.RT_Usages) {
            return new gen.client.resource.UsagesResource(context, protocol,
                    address);
        } else if (resourceType == ResourceType.RT_Version) {
            return new gen.client.resource.VersionResource(context, protocol,
                    address);
        } else if (resourceType == ResourceType.RT_Versions) {
            return new gen.client.resource.VersionsResource(context, protocol,
                    address);
        }

        else if (resourceType == ResourceType.RT_WiFi) {
            return new gen.client.resource.WiFiResource(context, protocol,
                    address);
        }

        return null;
    }

}
