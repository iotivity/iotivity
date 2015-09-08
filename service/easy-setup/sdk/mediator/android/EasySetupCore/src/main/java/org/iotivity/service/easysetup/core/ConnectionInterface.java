package org.iotivity.service.easysetup.core;

public interface ConnectionInterface {


    // Get the implementation of the Connection
    public Object getConnection();

    // Any general description on the connectivity.
    public String getDesc();

    public boolean isConnected();

}
