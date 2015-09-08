package org.iotivity.service.easysetup.impl;

import org.iotivity.service.easysetup.core.ProvisioningConfig;
import org.iotivity.service.easysetup.mediator.EnrolleeInfo;

import android.net.wifi.WifiConfiguration;

/**
 * This class contains on provisioning configuration information for for target network.
 * It implements ProvisioningConfig interface and provide configuration object specific to WiFi target network
 */
public class WiFiProvConfig implements ProvisioningConfig {

    private final ConnType mConnType = ProvisioningConfig.ConnType.WiFi;

    private final String mSsId;
    private final String mPassword;

    public WiFiProvConfig(String ssid, String pass) {
        mSsId = ssid;
        mPassword = pass;
    }

    @Override
    public Object getConfig() {
        return this;
    }

    @Override
    public ConnType getConnType() {
        // TODO Auto-generated method stub
        return mConnType;
    }

    public String getSsId() {
        return mSsId;
    }

    public String getPassword() {
        return mPassword;
    }

}
