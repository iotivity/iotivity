/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/
package org.iotivity.service.easysetup.mediator;

public class EnrolleeOnBoardingInfo {

    private String  IpAddr;
    private String  HWAddr;
    private String  Device;
    private boolean isReachable;
    private boolean isAdditionNotified;
    private boolean isRemovalNotified;

    public EnrolleeOnBoardingInfo(String ipAddr, String hWAddr, String device,
            boolean isReachable, boolean isRemovalNotified,
            boolean isAdditionNotified) {
        this.IpAddr = ipAddr;
        this.HWAddr = hWAddr;
        this.Device = device;
        this.isReachable = isReachable;
        this.isRemovalNotified = isRemovalNotified;
        this.isAdditionNotified = isAdditionNotified;
    }

    public String getIpAddr() {
        return IpAddr;
    }

    public void setIpAddr(String ipAddr) {
        IpAddr = ipAddr;
    }

    public String getHWAddr() {
        return HWAddr;
    }

    public void setHWAddr(String hWAddr) {
        HWAddr = hWAddr;
    }

    public String getDevice() {
        return Device;
    }

    public void setDevice(String device) {
        Device = device;
    }

    public boolean isReachable() {
        return isReachable;
    }

    public void setReachable(boolean isReachable) {
        this.isReachable = isReachable;
    }

    public boolean isRemovalNotified() {
        return isRemovalNotified;
    }

    public void setRemovalNotified(boolean isRemovalNotified) {
        this.isRemovalNotified = isRemovalNotified;
    }

    public boolean isAdditionNotified() {
        return isAdditionNotified;
    }

    public void setAdditionNotified(boolean isAdditionNotified) {
        this.isAdditionNotified = isAdditionNotified;
    }

}