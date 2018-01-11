/******************************************************************
 * Copyright 2018 Samsung Electronics All Rights Reserved.
 * <p/>
 * <p/>
 * <p/>
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * <p/>
 * http://www.apache.org/licenses/LICENSE-2.0
 * <p/>
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************/

package org.iotivity.test.es.common;

import android.content.Context;

import org.iotivity.base.OcResource;
import org.iotivity.service.easysetup.mediator.DeviceProp;
import org.iotivity.service.easysetup.mediator.ESException;
import org.iotivity.service.easysetup.mediator.EasySetup;
import org.iotivity.service.easysetup.mediator.GetEnrolleeStatus;
import org.iotivity.service.easysetup.mediator.GetStatusCallback;
import org.iotivity.service.easysetup.mediator.RemoteEnrollee;

import org.iotivity.testcase.IoTivityLog;
import org.iotivity.testcase.IoTivityTc;

public class ESHelperCommon {
    private static final String TAG = "ESHelperCommon";

    protected static Context    s_helperContext;

    public ESHelperCommon(IoTivityTc iotivityTcObj) {
        s_helperContext = iotivityTcObj.getInstrumentation().getTargetContext();
    }

    public EasySetup getEasySetupInstance() {
        return EasySetup.getInstance(s_helperContext);
    }
}
