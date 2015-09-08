package org.iotivity.service.easysetup.impl;

import org.iotivity.service.easysetup.core.ConnectionInterface;
import org.iotivity.service.easysetup.core.IpConnection;
import org.iotivity.service.easysetup.core.OnBoardingConfig;

import android.net.wifi.WifiConfiguration;

public class WiFiOnBoardingConfig implements OnBoardingConfig{

    private final WifiConfiguration config = new WifiConfiguration();
    private final ConnType mConnType = OnBoardingConfig.ConnType.WiFi;
    private IpConnection mConnection;

    @Override
    public Object getConfig() {
        // TODO Auto-generated method stub
        return config;
    }

    public void setSSId(String ssid) {
        config.SSID = ssid;
    }

    public void setSharedKey(String sharedKey) {
        config.preSharedKey = sharedKey;
    }

    public void setAuthAlgo(int aurthAlgo) {
        config.allowedAuthAlgorithms.set(aurthAlgo);
    }

    public void setKms(int kms) {
        config.allowedKeyManagement.set(kms);
    }

    @Override
    public ConnType getConnType() {
        // TODO Auto-generated method stub
        return mConnType;
    }

    @Override
    public ConnectionInterface getConnection() {
        // TODO Auto-generated method stub
        return mConnection;
    }

    @Override
    public void setConnection(ConnectionInterface conn) {
        mConnection = (IpConnection)conn;
    }


}
