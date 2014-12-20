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

package OC.Cm;

import gen.cm.ResourceType;

import java.util.List;

public abstract class Device {

    private long   nativeHandle;
    public boolean m_device_description_loaded;

    private native long constructNative(int type, int domain);

    private native void deleteNative(long dev);

    private native void setProtocol(long nativeHandle, String protocol)
            throws Exception;

    private native void setContext(long nativeHandle, long context)
            throws Exception;

    private native long getContext(long nativeHandle) throws Exception;

    private native String getAddress(long config) throws Exception;

    private native String getProtocol(long config) throws Exception;

    private native void setAddress(long config, final String address)
            throws Exception;

    private native String getDeviceDescriptionUri(long config) throws Exception;

    private native void setDeviceDescriptionUri(long config,
            final String deviceResourceUri) throws Exception;

    private native OC.Cm.DeviceType getDeviceType(long config) throws Exception;

    protected native void setDeviceType(long config, int deviceType)
            throws Exception;

    private native OC.Cm.DeviceDomain getDomain(long config) throws Exception;

    private native void setDomain(long config, int domain) throws Exception;

    private native String getUUID(long config) throws Exception;

    protected native void setUUID(long config, final String uuid)
            throws Exception;

    private native boolean isDeviceDescriptionLoaded(long config)
            throws Exception;

    protected native boolean setDeviceDescriptionLoaded(long config,
            boolean loaded) throws Exception;

    private native String getDescription(long config) throws Exception;

    private native void setDescription(long config, final String description)
            throws Exception;

    private native String getManufacturer(long config) throws Exception;

    private native void setManufacturer(long config, final String manufacturer)
            throws Exception;

    private native String getDeviceSubType(long config) throws Exception;

    private native void setDeviceSubType(long config, final String deviceSubType)
            throws Exception;

    private native String getModelID(long config) throws Exception;

    private native void setModelID(long config, final String modelID)
            throws Exception;

    private native String getSerialNumber(long config) throws Exception;

    private native void setSerialNumber(long config, final String serialNumber)
            throws Exception;

    private native String getSalesLocation(long config) throws Exception;

    private native void setSalesLocation(long config, final String salesLocation)
            throws Exception;

    private native List<String> getSupportedResources(long config)
            throws Exception;

    private native void setSupportedResource(long config, String resourceName)
            throws Exception;

    private native boolean deRegisterFromSHS(long config) throws Exception;

    private native boolean getDeviceOnlineStatus(long config) throws Exception;

    private native void setTokenGrantStatus(long dev, boolean state)
            throws Exception;

    private native void updateDeviceTokenList(long dev, long[] deviceTokenList)
            throws Exception;

    private native boolean initiateTokenRequest(long dev, String ip, String port);

    private native boolean initiateTokenRequestUsingUUID(long dev, String uuid,
            boolean fromSimulator);

    private native String getMyDeviceToken(long dev) throws Exception;

    private native void setMyDeviceToken(long dev, String token)
            throws Exception;

    private native String getPeerTokenFromUUIDMap(long dev, String uuid)
            throws Exception;

    private native void setPeerTokenIntoUUIDMap(long dev, String uuid,
            String token) throws Exception;

    @Override
    protected void finalize() throws Throwable {
        this.destroy();
        super.finalize();
    }

    public void destroy() {
        if (nativeHandle != 0) {
            this.deleteNative(nativeHandle);
            nativeHandle = 0;
        }
    }

    public void onNativeDelete() {
        nativeHandle = 0;
    }

    protected Device(OC.Cm.DeviceType type, OC.Cm.DeviceDomain domain) {
        int Dtype = (int) (type.getValue());

        int Ddomain = (int) (domain.getValue());

        nativeHandle = constructNative(Dtype, Ddomain);
    }

    Device(long handle) {
        nativeHandle = handle;
    }

    public long getNativeHandle() {
        return nativeHandle;
    }

    public String getAddress() throws Exception {
        return getAddress(nativeHandle);
    }

    public void setAddress(final String address) throws Exception {
        this.setAddress(nativeHandle, address);
    }

    public String getDeviceDescriptionUri() throws Exception {
        return this.getDeviceDescriptionUri(nativeHandle);
    }

    public void setDeviceDescriptionUri(final String deviceResourceUri)
            throws Exception {
        this.setDeviceDescriptionUri(nativeHandle, deviceResourceUri);
    }

    public OC.Cm.DeviceType getDeviceType() throws Exception {
        return getDeviceType(nativeHandle);

    }

    public void setDeviceType(int deviceType) throws Exception {
        this.setDeviceType(nativeHandle, deviceType);
    }

    public OC.Cm.DeviceDomain getDomain() throws Exception {
        return getDomain(nativeHandle);
    }

    public void setDomain(int domain) throws Exception {
        this.setDomain(nativeHandle, domain);
    }

    public String getUUID() throws Exception {
        return this.getUUID(nativeHandle);
    }

    public void setUUID(final String uuid) throws Exception {
        this.setUUID(nativeHandle, uuid);
    }

    public boolean isDeviceDescriptionLoaded() throws Exception {
        return isDeviceDescriptionLoaded(nativeHandle);
    }

    public long getContext() throws Exception {
        return this.getContext(nativeHandle);
    }

    public String getProtocol() throws Exception {
        return this.getProtocol(nativeHandle);
    }

    public void setProtocol(String protocol) throws Exception {
        this.setProtocol(nativeHandle, protocol);
    }

    public void setContext(long context) throws Exception {
        this.setContext(nativeHandle, context);
    }

    public OC.Cm.Client.Resource.Resource getResource(
            gen.cm.ResourceType resourceType) {
        // Need to verify the supported resource - Vinay
        return createResource(resourceType);

    }

    public abstract String getSubscriptionsLink();

    public abstract boolean loadDeviceDescription(
            OC.Cm.Serialization.ISerializable serializable);

    public abstract OC.Cm.Serialization.ISerializable getDeviceDescription();

    public String getDescription() throws Exception {
        return this.getDescription(nativeHandle);
    }

    public void setDescription(final String description) throws Exception {
        this.setDescription(nativeHandle, description);
    }

    public String getManufacturer() throws Exception {
        return this.getManufacturer(nativeHandle);
    }

    public void setManufacturer(final String manufacturer) throws Exception {
        this.setManufacturer(nativeHandle, manufacturer);
    }

    public String getDeviceSubType() throws Exception {
        return this.getDeviceSubType(nativeHandle);
    }

    public void setDeviceSubType(final String deviceSubType) throws Exception {
        this.setDeviceSubType(nativeHandle, deviceSubType);
    }

    public String getModelID() throws Exception {
        return this.getUUID(nativeHandle);
    }

    public void setModelID(final String modelID) throws Exception {
        this.setModelID(nativeHandle, modelID);
    }

    public String getSerialNumber() throws Exception {
        return this.getSerialNumber(nativeHandle);
    }

    public void setSerialNumber(final String serialNumber) throws Exception {
        this.setSerialNumber(nativeHandle, serialNumber);
    }

    public String getSalesLocation() throws Exception {
        return this.getSalesLocation(nativeHandle);
    }

    public void setSalesLocation(final String salesLocation) throws Exception {
        this.setSalesLocation(nativeHandle, salesLocation);
    }

    public List<String> getSupportedResources() throws Exception {
        return this.getSupportedResources(nativeHandle);
    }

    public void setSupportedResource(String resourceName) throws Exception {
        this.setSupportedResource(nativeHandle, resourceName);
    }

    public boolean deRegisterFromSHS() throws Exception {
        return deRegisterFromSHS(nativeHandle);
    }

    public boolean getDeviceOnlineStatus() throws Exception {
        return getDeviceOnlineStatus(nativeHandle);
    }

    public void setTokenGrantStatus(boolean status) throws Exception {
        this.setTokenGrantStatus(nativeHandle, status);
    }

    public void updateDeviceTokenList(OC.Cm.DeviceToken[] deviceTokenList)
            throws Exception {
        if (deviceTokenList.length > 0) {
            long[] devtokenlist = new long[deviceTokenList.length];
            int count = 0;
            for (OC.Cm.DeviceToken devtoken : deviceTokenList) {
                devtokenlist[count] = devtoken.getNativeHandle();
                count++;
            }
            this.updateDeviceTokenList(nativeHandle, devtokenlist);
        }
    }

    public boolean startTokenRequest(String IP, String port) {
        try {
            return this.initiateTokenRequest(nativeHandle, IP, port);
        } catch (Exception e) {
            e.printStackTrace();
        }
        return false;
    }

    public boolean startTokenRequestFromSimulator(String uuid) {
        try {
            return this.initiateTokenRequestUsingUUID(nativeHandle, uuid, true);
        } catch (Exception e) {
            e.printStackTrace();
        }
        return false;
    }

    public boolean startTokenRequestUsingUUID(String uuid) {
        try {
            return this
                    .initiateTokenRequestUsingUUID(nativeHandle, uuid, false);
        } catch (Exception e) {
            e.printStackTrace();
        }
        return false;
    }

    public String getMyDeviceToken() {
        String retVal = "";
        try {
            retVal = this.getMyDeviceToken(nativeHandle);
        } catch (Exception e) {
            e.printStackTrace();
        }
        return retVal;
    }

    public void setMyDeviceToken(String token) {
        try {
            this.setMyDeviceToken(nativeHandle, token);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public String getPeerToken(String uuid) {
        String retVal = "";
        try {
            retVal = this.getPeerTokenFromUUIDMap(nativeHandle, uuid);
        } catch (Exception e) {
            e.printStackTrace();
        }
        return retVal;
    }

    public void setPeerToken(String uuid, String token) {
        try {
            this.setPeerTokenIntoUUIDMap(nativeHandle, uuid, token);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

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
