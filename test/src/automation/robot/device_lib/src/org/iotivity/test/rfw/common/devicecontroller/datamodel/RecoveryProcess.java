/******************************************************************
 *
 * Copyright 2018 Samsung Electronics All Rights Reserved.
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
 *
 ******************************************************************/

package org.iotivity.test.rfw.common.devicecontroller.datamodel;

import org.iotivity.test.rfw.common.devicecontroller.datamodel.RecoveryType;

public class RecoveryProcess {
    public String getProcessName() {
        return processName;
    }

    public void setProcessName(String processName) {
        this.processName = processName;
    }

    public String getProcessCommand() {
        return processCommand;
    }

    public void setProcessCommand(String processCommand) {
        this.processCommand = processCommand;
    }

    public String getDeviceId() {
        return deviceId;
    }

    public void setDeviceId(String deviceId) {
        this.deviceId = deviceId;
    }

    public RecoveryType getType() {
        return type;
    }

    public void setType(RecoveryType type) {
        this.type = type;
    }

    private String       processName;
    private String       processCommand;
    private String       deviceId;
    private RecoveryType type;

    public RecoveryProcess() {
        processName = "";
        processCommand = "";
        deviceId = "";
        type = RecoveryType.Linux;
    }
}
