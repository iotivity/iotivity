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

package org.iotivity.test.rfw.common.devicecontroller.tizen;

import org.iotivity.test.rfw.common.devicecontroller.interfaces.ILogObservable;
import org.iotivity.test.rfw.common.devicecontroller.interfaces.ILogObserver;
import org.iotivity.test.rfw.common.devicecontroller.interfaces.IProcessManager;

import org.iotivity.test.rfw.common.devicecontroller.tizen.TizenSDBManager;

class TizenApplicationManager implements IProcessManager {
    TizenSDBManager tizenSDBManager;

    public TizenApplicationManager(TizenSDBManager tizenSDBManager) {
        this.tizenSDBManager = tizenSDBManager;
    }

    @Override
    public boolean createProcess(String serviceName, String className) {
        tizenSDBManager.startService(serviceName, className);
        return true;
    }

    @Override
    public boolean removeProcess(String serviceName) {
        tizenSDBManager.stopService(serviceName);
        return true;
    }

    @Override
    public boolean inputProcessCommand(String processName, String command) {
        return false;
    }

    @Override
    public boolean isProcessRunning(String processName) {
        return false;
    }

    @Override
    public boolean removeAllProcess() {
        return false;
    }

    @Override
    public boolean registerOutput(String processName, ILogObserver observer) {
        return false;
    }

    @Override
    public boolean unregisterOutput(String processName, ILogObserver observer) {
        return false;
    }

    @Override
    public ILogObservable getObserverable(String processName) {
        return null;
    }

}
