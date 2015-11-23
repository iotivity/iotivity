package oic.simulator.clientcontroller.remoteresource;

import org.oic.simulator.DeviceInfo;
import org.oic.simulator.PlatformInfo;

public class DeviceAndPlatformInfo {
    private String       host;
    private DeviceInfo   deviceInfo;
    private PlatformInfo platformInfo;

    public String getHost() {
        return host;
    }

    public void setHost(String host) {
        this.host = host;
    }

    public DeviceInfo getDeviceInfo() {
        return deviceInfo;
    }

    public void setDeviceInfo(DeviceInfo deviceInfo) {
        this.deviceInfo = deviceInfo;
    }

    public PlatformInfo getPlatformInfo() {
        return platformInfo;
    }

    public void setPlatformInfo(PlatformInfo platformInfo) {
        this.platformInfo = platformInfo;
    }
}
