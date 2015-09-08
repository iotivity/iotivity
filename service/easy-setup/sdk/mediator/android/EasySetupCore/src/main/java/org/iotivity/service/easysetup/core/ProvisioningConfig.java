package org.iotivity.service.easysetup.core;

/**
 * It contains configuration details about the the target network where Enrollee device is going to be enrolled.
 */
public interface ProvisioningConfig {

    /**
     * It provides constants for connectivity types of target network
     */
    public static enum ConnType {
        WiFi,
        BT
    }

    /**
     * Gives the instance of the configuration object created according to the connectivity type of target network.
     *
     * @return instance object of configuration according to connectivity type of target network
     */
    Object getConfig();


    /**
     * Gives connectivity type of target network
     *
     * @return Connectivity type of target network
     */
    ConnType getConnType();

}
