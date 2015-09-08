package org.iotivity.service.easysetup.core;

public class IpConnection implements ConnectionInterface {

    private boolean mIsConnected;
    private String mIp;

    public void setConnectivity(boolean connected) {
        mIsConnected = connected;
    }

    public void setIp(String ip) {
        mIp = ip;
    }

    public String getIp() {
        return mIp;
    }

    @Override
    public String getDesc() {
        // TODO Auto-generated method stub
        return "Description";
    }

    @Override
    public boolean isConnected() {
        // TODO Auto-generated method stub
        return mIsConnected;
    }

    @Override
    public Object getConnection() {
        // TODO Auto-generated method stub
        return this;
    }

}
