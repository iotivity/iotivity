package org.iotivity.service.easysetup.core;

/**
 * This interface facilitates Application to get progress & result of Easy setup
 * process Easy setup process.
 */
public interface EasySetupStatus {

    /**
     * This method is called back when easy setup process is finished. Easy
     * setup process status can read from the APIs given in Enrollee class.This
     * method is called on worker thread, UI operations to be executed on main /
     * Ui Thread.
     *
     * @param enrolledevice Device to be enrolled in network through Easy setup process
     */
    public void onFinished(EnrolleeDevice enrolledevice);

    /**
     * This method is called back to give intermediate progress on the easy
     * setup process This method is called on worker thread, UI operations to be
     * executed on main / Ui Thread.
     *
     * @param state Gives state of the device changed during easy setup process
     */
    public void onProgress(EnrolleeState state);

}
