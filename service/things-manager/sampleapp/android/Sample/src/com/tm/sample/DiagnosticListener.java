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
package com.tm.sample;

import java.util.Vector;

import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.OcRepresentation;
import org.iotivity.service.tm.IDiagnosticsListener;

import android.util.Log;

/**
 * For receiving Diagnostic APIs callback : BootStrap,Reboot & FactoryReset
 */
public class DiagnosticListener implements IDiagnosticsListener {

    private final String LOG_TAG = this.getClass().getSimpleName();

    @Override
    public void onRebootCallback(Vector<OcHeaderOption> headerOptions,
            OcRepresentation rep, int errorValue) {
        Log.i(LOG_TAG, "Got Callback : onRebootCallback");

    }

    @Override
    public void onFactoryResetCallback(Vector<OcHeaderOption> headerOptions,
            OcRepresentation rep, int errorValue) {
        Log.i(LOG_TAG, "Got Callback : onFactoryResetCallback");

    }

}
