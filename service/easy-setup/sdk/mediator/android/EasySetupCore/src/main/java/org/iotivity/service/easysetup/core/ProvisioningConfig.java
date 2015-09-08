package org.iotivity.service.easysetup.core;


public interface ProvisioningConfig {

    public static enum ConnType {
        WiFi,
        BT
    }

    Object getConfig();
    ConnType getConnType();

}
