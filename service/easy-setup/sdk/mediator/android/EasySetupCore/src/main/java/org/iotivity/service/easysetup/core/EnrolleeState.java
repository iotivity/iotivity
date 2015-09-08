package org.iotivity.service.easysetup.core;


/**
 *
 * The device being enrolled into the network will fall into any of the below mentioned states
 *
 */

public enum EnrolleeState {

    // Device will be there in this state by default
    DEVICE_ON_BOARDING_STATE,

    // Device will move to this state once the on boarding is done
    DEVICE_PROVISIONING_STATE,

    // Device will move to this state after successful provisioning into the network
    DEVICE_PROVISIONING_SUCCESS_STATE,

    // Device is not able to provisioned
    DEVICE_PROVISIONING_FAILED_STATE,

    // The below state is arbitrary one, any time device can come into this state
    //Device will move to this state if the ownership transfer initiated  by the Application
    DEVICE_OWNERSHIP_TRANSFER_STATE,

    // The below state is arbitrary one, any time device can come into this state
    // Device will move to this state once the Application factory reset the device
    DEVICE_FACTORY_RESET_STATE,



}
