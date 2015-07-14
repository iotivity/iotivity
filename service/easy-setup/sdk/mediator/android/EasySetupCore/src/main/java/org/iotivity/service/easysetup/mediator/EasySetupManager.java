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

import android.content.Context;

public class EasySetupManager {
    private native void InitEasySetup();
    private native void TerminateEasySetup();
    private native void ProvisionEnrollee(String ipAddress, String netSSID,
            String netPWD, int connectivityType);
    private native void StopEnrolleeProvisioning(int connectivityType);
    private static EasySetupManager easySetupManagerInterfaceObj = null;
    private Context                 appContext                   = null;

    private EasySetupManager() {

    }

    /**
     * Function for Getting instance of EasySetupManager object.
     *
     * @return EasySetupManager instance.
     *
     */
    public static synchronized EasySetupManager getInstance() {
        if (null == easySetupManagerInterfaceObj) {
            easySetupManagerInterfaceObj = new EasySetupManager();
        }
        return easySetupManagerInterfaceObj;
    }

    public void initEasySetup() {
        InitEasySetup();
    }

    public void terminateEasySetup() {
        TerminateEasySetup();
    }

    public void provisionIPEnrollee(String ipAddress, String netSSID,
            String netPWD, int connectivityType) {

        ProvisionEnrollee(ipAddress, netSSID, netPWD, connectivityType);
    }

    public void stopEnrolleeProvisioning(int connectivityType) {
        StopEnrolleeProvisioning(connectivityType);
    }

}
