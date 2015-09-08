package org.iotivity.service.easysetup.impl;

import org.iotivity.service.easysetup.core.EnrolleeDevice;
import org.iotivity.service.easysetup.core.OnBoardingConfig;
import org.iotivity.service.easysetup.core.ProvisioningConfig;

import android.content.Context;

/**
 * This a factory class provides the native implementation of the various Enrollee devices.
 * Application can make use of Enrollee factory if it does not want to create its own Enrollee devices.
 */
public class EnrolleeDeviceFactory {

    Context mContext;

    /**
     * This method create & returns instance of EnrolleeDeviceFactory
     *
     * @param context This is Android Application context
     */
    public static EnrolleeDeviceFactory newInstance(Context context) {
        return new EnrolleeDeviceFactory(context);
    }

    private EnrolleeDeviceFactory(Context context) {
        mContext = context;
    }

    /**
     * This method create & returns instance of Enrollee device of supported configuration
     *
     * @param onboardingConfig Contains details about the connectivity to be established between the Enrollee device & Mediator device in order to perform on-boarding
     * @param provConfig       Contains details about the network to which Enrollee device is going to connect.
     * @return Instance of the Enrollee device created natively.
     */

    public EnrolleeDevice newEnrolleeDevice(OnBoardingConfig onboardingConfig, ProvisioningConfig provConfig) {

        EnrolleeDevice enrolleeDevice;

        if (onboardingConfig.getConnType() == OnBoardingConfig.ConnType.WiFi) {
            enrolleeDevice = new EnrolleeDeviceWiFiOnboarding(mContext, onboardingConfig, provConfig);
        } else {
            throw new IllegalArgumentException("OnBoarding configuration is not supported");
        }

        return enrolleeDevice;
    }

}
