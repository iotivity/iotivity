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
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

package org.iotivity.test.ic.tc.helper;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;

import java.util.List;
import java.util.EnumSet;
import java.util.LinkedList;
import java.util.ArrayList;

import org.iotivity.base.ModeType;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;
import org.iotivity.base.OcAccountManager;
import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcException;
import org.iotivity.base.ErrorCode;
import org.iotivity.base.OcResourceHandle;
import org.iotivity.base.ResourceProperty;
import org.iotivity.base.OcConnectivityType;

import org.iotivity.test.ic.tc.helper.ICHelperStaticUtil;
import org.iotivity.test.ic.tc.helper.OcAccountManagerAdapter;
import org.iotivity.test.ri.common.RIHelperCommon;
import org.iotivity.testcase.IoTivityLog;
import org.iotivity.testcase.IoTivityTc;

public class ICHelper extends RIHelperCommon {

    /**************************
     * Declare class variables
     *************************/

    public final static String      TAG                      = "ICTest";

    private boolean                 mOnPutCalled            = false;
    private static OcResourceHandle slocalLightResourceHandle = null;
    private static final String     IC_KEY_HUMIDITY          = "humidity";

    private static final boolean          CALLBACK_INVOKED         = true;
    private static final boolean          CALLBACK_NOT_INVOKED     = false;
    private static boolean          mIsCbInvoked             = CALLBACK_NOT_INVOKED;
    private final int                     DELAY_SHORT              = 1000;
    private final int                     CALLBACK_TIMEOUT         = 20;

    public int                      mPostHum                = 0;
    public Boolean                  mPostCompleted          = false;
    public static final int         OCSTACK_OK               = 0;
    public static final int         OCSTACK_ERROR            = 255;
    public static final int         IC_MAX_WAIT_TIME         = 10;
    public static final int         IC_MIN_WAIT_TIME         = 5;
    public static final int         BUFFER_SIZE              = 1024;

    public static String            sUUID;
    public static String            sAccessToken;
    public static String            sRefreshToken;
    public OcAccountManager         mAccountManagertest;

    public static String filePath = "";
    private enum LogLevel {
        INFO, ERROR, DEBUG
    };

    private static OcAccountManagerAdapter mOcAccountManagerAdapter = new OcAccountManagerAdapter();

    public ICHelper(IoTivityTc iotivityTcObj) {
        super(iotivityTcObj);
    }

    private boolean waitTillCBInvoke() {
        IoTivityLog.i(TAG, "Checking if Callback is Invoked or Not");
        int count = 0;

        while (!mIsCbInvoked) {

            try {
                Thread.sleep(DELAY_SHORT);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

            count++;
            IoTivityLog.i(TAG, "Waiting for = " + count + " second/s");

            if (count > CALLBACK_TIMEOUT) {
                return CALLBACK_NOT_INVOKED;
            }
        }

        return CALLBACK_INVOKED;
    }

    public static boolean createResource() {
        if (slocalLightResourceHandle == null) {
            try {
                slocalLightResourceHandle = OcPlatform.registerResource(
                        ICHelperStaticUtil.Resource_URI, // resource
                                      // URI
                        ICHelperStaticUtil.RESOURCE_TYPE, // resource type name
                        ICHelperStaticUtil.RESOURCE_INTERFACE, // using default interface
                        null, // use default entity handler
                        EnumSet.of(ResourceProperty.DISCOVERABLE,
                                ResourceProperty.OBSERVABLE));
                ICHelperStaticUtil.sResourceHandleList.add(slocalLightResourceHandle);
                IoTivityLog.i(TAG, "Create Local Resource is success.");
                return true;
            } catch (OcException e) {
                IoTivityLog.e(TAG, e.toString());
            }
        }
        return false;
    }
}
