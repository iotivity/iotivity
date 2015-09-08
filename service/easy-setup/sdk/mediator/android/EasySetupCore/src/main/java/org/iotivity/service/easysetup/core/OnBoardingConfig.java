package org.iotivity.service.easysetup.core;


public interface OnBoardingConfig {

    public static enum ConnType {
        WiFi,
        BLE,
    }

    public Object getConfig();
    public ConnType getConnType();
    public ConnectionInterface getConnection();
    public void setConnection(ConnectionInterface conn);

}
