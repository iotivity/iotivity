package org.iotivity.service.easysetup.impl;

import org.iotivity.service.easysetup.core.EnrolleeDevice;
import org.iotivity.service.easysetup.core.OnBoardingConfig;
import org.iotivity.service.easysetup.core.ProvisioningConfig;

import android.content.Context;

public class EnrolleeDeviceFactory {

    Context mContext;

    public static EnrolleeDeviceFactory newInstance(Context context) {
        return new EnrolleeDeviceFactory(context);
    }

    private EnrolleeDeviceFactory (Context context) {
        mContext = context;
    }

    public EnrolleeDevice newEnrolleeDevice(OnBoardingConfig onboardingConfig, ProvisioningConfig provConfig){

        EnrolleeDevice enrolleeDevice;

        if (onboardingConfig.getConnType()== OnBoardingConfig.ConnType.WiFi) {
            enrolleeDevice =  new EnrolleeDeviceWiFiOnboarding(mContext, onboardingConfig, provConfig);
        }else {
            throw new IllegalArgumentException("OnBoarding configuration is not supported");
        }

        return enrolleeDevice;
    }

}
