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

package org.iotivity.test.rfw.common.devicecontroller.interfaces;

import org.iotivity.test.rfw.common.devicecontroller.interfaces.ILogObservable;
import org.iotivity.test.rfw.common.devicecontroller.interfaces.ILogObserver;

public interface IProcessManager {
    public boolean createProcess(String processName, String processCommand);

    public boolean removeProcess(String processName);

    public boolean inputProcessCommand(String processName, String command);

    public boolean registerOutput(String processName, ILogObserver observer);

    public boolean unregisterOutput(String processName, ILogObserver observer);

    public boolean isProcessRunning(String processName);

    public ILogObservable getObserverable(String processName);

    boolean removeAllProcess();
}
