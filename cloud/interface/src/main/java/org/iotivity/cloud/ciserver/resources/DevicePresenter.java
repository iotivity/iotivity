package org.iotivity.cloud.ciserver.resources;

import java.util.Set;

/**
 *  Component for selecting actively connected devices to cloud
 */
public interface DevicePresenter {

    Set<String> getDeviceIds();

}
