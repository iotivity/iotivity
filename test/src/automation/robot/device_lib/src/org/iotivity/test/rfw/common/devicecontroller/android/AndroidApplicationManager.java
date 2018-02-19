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

package org.iotivity.test.rfw.common.devicecontroller.android;

import org.iotivity.test.rfw.common.devicecontroller.interfaces.ILogObservable;
import org.iotivity.test.rfw.common.devicecontroller.interfaces.ILogObserver;
import org.iotivity.test.rfw.common.devicecontroller.interfaces.IProcessManager;

import org.iotivity.test.rfw.common.devicecontroller.android.AndroidADBManager;

class AndroidApplicationManager implements IProcessManager {
    public AndroidADBManager androidADBManager;

    public AndroidApplicationManager(AndroidADBManager androidADBManager) {
        this.androidADBManager = androidADBManager;
    }

    @Override
    public boolean createProcess(String serviceName, String className) {
        this.androidADBManager.startService(serviceName, className);
        return false;
    }

    @Override
    public boolean removeProcess(String serviceName) {
        this.androidADBManager.stopService(serviceName);
        return false;
    }

    @Override
    public boolean inputProcessCommand(String processName, String command) {
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
    public boolean isProcessRunning(String processName) {
        return this.androidADBManager.isRunningApp(processName);
    }

    @Override
    public boolean removeAllProcess() {
        return false;
    }

    @Override
    public ILogObservable getObserverable(String processName) {
        return null;
    }
}
