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

import java.util.List;
import java.util.Map;

/**
 * This class holds the details required to configure SHP
 */
public class Configuration {

    public long nativeHandle;

    private native long constructNative() throws Exception;

    private native void deleteNative(long config);

    private native void reset(long config);

    private native void setDeviceFactory(long config, long deviceFactory)
            throws Exception;

    private native void setSerializableDataFactory(long config,
            long serializableDataFactory) throws Exception;

    private native void setResourceHandlerFactory(long config,
            long handlerFactory) throws Exception;

    private native void addDataFormat(long config, String contentType,
            OC.Cm.Serialization.ISerializer serializer,
            OC.Cm.Serialization.IDeserializer deserializer) throws Exception;

    private native void setServerConnector(long config, long connector)
            throws Exception;

    private native void setClientConnector(long config, long connector)
            throws Exception;

    private native List<OC.Cm.Connector.Client.IClientConnector> getClientConnectors(
            long config) throws Exception;

    private native Map<String, OC.Cm.Serialization.IDeserializer> getDeSerializers(
            long config) throws Exception;

    private native OC.Cm.DeviceFinder.DeviceFactory getDeviceFactory(long config)
            throws Exception;

    private native OC.Cm.Server.ResourceHandler.ResourceHandlerFactory getResourceHandlerFactory(
            long config) throws Exception;

    private native OC.Cm.Serialization.SerializableDataFactory getSerializableDataFactory(
            long config) throws Exception;

    private native Map<String, OC.Cm.Serialization.ISerializer> getSerializers(
            long config) throws Exception;

    private native List<OC.Cm.Connector.Server.IServerConnector> getServerConnectors(
            long config) throws Exception;

    private native OC.Cm.ApplicationType getAppType(long config)
            throws Exception;

    // private native void setAppType(long config,int appType) throws Exception;
    // // Removed from native
    private native String getSubscriptionDbPath(long config) throws Exception;

    private native void setSubscriptionDbPath(long config, String dbPath)
            throws Exception;

    private native void setRAConfigPath(long config, String raConfigPath)
            throws Exception;

    private native String getRAConfigPath(long config);

    private native void setRAConfigPath(long config, String raConfigPath,
            String privateKey) throws Exception;

    private native String getRAPrivateKey(long config);

    private native void setRemoteAccessEnable(long config, boolean enable);

    private native long getRemoteAccessConfig(long config);

    private native void setWifiDetails(long config, long serializable)
            throws Exception;

    public Configuration() throws Exception {
        nativeHandle = constructNative();
    }

    public Configuration(long handle) {
        nativeHandle = handle;
    }

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

    public void reset() {
        this.reset(nativeHandle);
    }

    public void setDeviceFactory(OC.Cm.DeviceFinder.DeviceFactory deviceFactory)
            throws Exception {
        this.setDeviceFactory(nativeHandle, deviceFactory.getNativeHandle());
        m_deviceFactory = deviceFactory;
    }

    public void setSerializableDataFactory(
            OC.Cm.Serialization.SerializableDataFactory serializableDataFactory)
            throws Exception {
        this.setSerializableDataFactory(nativeHandle,
                serializableDataFactory.getNativeHandle());
        m_serializableDataFactory = serializableDataFactory;
    }

    public void setResourceHandlerFactory(
            OC.Cm.Server.ResourceHandler.ResourceHandlerFactory handlerFactory)
            throws Exception {
        this.setResourceHandlerFactory(nativeHandle,
                handlerFactory.getNativeHandle());
        m_resourceHandlerFactory = handlerFactory;
    }

    public void addDataFormat(String contentType,
            OC.Cm.Serialization.ISerializer serializer,
            OC.Cm.Serialization.IDeserializer deserializer) throws Exception {
        this.addDataFormat(nativeHandle, contentType, serializer, deserializer);
    }

    public void setServerConnector(
            OC.Cm.Connector.Server.IServerConnector connector) throws Exception {
        this.setServerConnector(nativeHandle, connector.getNativeHandle());
    }

    public void setClientConnector(
            OC.Cm.Connector.Client.IClientConnector connector) throws Exception {
        this.setClientConnector(nativeHandle, connector.getNativeHandle());
    }

    public List<OC.Cm.Connector.Client.IClientConnector> getClientConnectors()
            throws Exception {
        return getClientConnectors(nativeHandle);
    }

    public Map<String, OC.Cm.Serialization.IDeserializer> getDeSerializers()
            throws Exception {
        return getDeSerializers(nativeHandle);
    }

    public OC.Cm.DeviceFinder.DeviceFactory getDeviceFactory() throws Exception {
        return getDeviceFactory(nativeHandle);
    }

    public OC.Cm.Server.ResourceHandler.ResourceHandlerFactory getResourceHandlerFactory()
            throws Exception {
        return getResourceHandlerFactory(nativeHandle);

    }

    public OC.Cm.Serialization.SerializableDataFactory getSerializableDataFactory()
            throws Exception {
        return getSerializableDataFactory(nativeHandle);

    }

    public Map<String, OC.Cm.Serialization.ISerializer> getSerializers()
            throws Exception {
        return getSerializers(nativeHandle);

    }

    public List<OC.Cm.Connector.Server.IServerConnector> getServerConnectors()
            throws Exception {
        return getServerConnectors(nativeHandle);
    }

    public OC.Cm.ApplicationType getAppType() throws Exception {
        return getAppType(nativeHandle);
    }

    /*
     * public void setAppType(int appType) throws Exception {
     * this.setAppType(nativeHandle, appType); }
     */

    public String getSubscriptionDbPath() throws Exception {
        return getSubscriptionDbPath(nativeHandle);
    }

    public void setSubscriptionDbPath(String dbPath) throws Exception {
        this.setSubscriptionDbPath(nativeHandle, dbPath);
    }

    public void setRAConfigPath(String raConfigPath) throws Exception {
        this.setRAConfigPath(nativeHandle, raConfigPath);
    }

    public String getRAConfigPath() throws Exception {
        return this.getRAConfigPath(nativeHandle);
    }

    public void setRAConfigPath(String raConfigPath, String privateKey)
            throws Exception {
        this.setRAConfigPath(nativeHandle, raConfigPath, privateKey);
    }

    public String getRAPrivateKey() throws Exception {
        return this.getRAPrivateKey(nativeHandle);
    }

    public void setRemoteAccessEnable(boolean enable) throws Exception {
        this.setRemoteAccessEnable(nativeHandle, enable);
    }

    public long getRemoteAccessConfig() throws Exception {
        return this.getRemoteAccessConfig(nativeHandle);
    }

    public void setWifiDetails(OC.Cm.Serialization.ISerializable serializable)
            throws Exception {
        this.setWifiDetails(nativeHandle, serializable.getNativeHandle());
    }

    private OC.Cm.DeviceFinder.DeviceFactory                    m_deviceFactory;

    private OC.Cm.Server.ResourceHandler.ResourceHandlerFactory m_resourceHandlerFactory;

    private OC.Cm.Serialization.SerializableDataFactory         m_serializableDataFactory;

    // private Map<String,Sec.Shp.Serialization.ISerializer> m_serializers;

    // private Map<String, Sec.Shp.Serialization.IDeserializer> m_deSerializers;

    // private List<Sec.Shp.Connector.Server.IServerConnector>
    // m_serverConnectors;

    // private List<Sec.Shp.Connector.Client.IClientConnector>
    // m_clientConnectors;

    private OC.Cm.ApplicationType                               m_app_type;

    // private String m_subscriptionDbPath ;*/
}
