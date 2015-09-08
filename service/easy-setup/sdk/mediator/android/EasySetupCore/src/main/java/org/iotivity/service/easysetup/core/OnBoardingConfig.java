package org.iotivity.service.easysetup.core;

/**
 * It contains configuration details about the connectivity type between the Enrollee device & Mediator device in order to perform on-boarding.
 */

public interface OnBoardingConfig {

    /**
     * It provides constants for connectivity types used for on-boarding Enrollee device
     */
    public static enum ConnType {
        WiFi,
        BLE,
    }

    /**
     * Gives configuration object specific to the on-boarding connectivity of the enrolling device.
     *
     * @return instance object of configuration according to connectivity type
     */
    public Object getConfig();

    /**
     * Gives connectivity type of on-boarding device
     *
     * @return on-boarding connectivity type
     */
    public ConnType getConnType();


}
