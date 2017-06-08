/******************************************************************
*
* Copyright 2017 Samsung Electronics All Rights Reserved.
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
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.List;
import java.util.EnumSet;
import java.util.LinkedList;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;

import android.R.bool;
import android.R.string;
import android.content.Context;
import android.util.Log;

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
import static org.iotivity.test.ic.tc.helper.ICHelperStaticUtil.*;
import org.iotivity.test.ic.tc.helper.OcAccountManagerAdapter;

public class ICHelper {

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

    public ICHelper() {

    }

    public static void icConfigurePlatform(Context appContext) {
        OcPlatform.Configure(new PlatformConfig(appContext, ServiceType.IN_PROC,
                ModeType.CLIENT_SERVER, "0.0.0.0", 0, QualityOfService.LOW));
        Log.i(TAG, "Configuration done successfully");
    }

    public static String read(String filename, Context c) throws IOException {

        StringBuffer buffer = new StringBuffer();

        String Read = "";
        FileInputStream fis = c.openFileInput(filename);
        BufferedReader reader = new BufferedReader(new InputStreamReader(fis));
        if (fis != null) {
            while ((Read = reader.readLine()) != null) {
                buffer.append(Read + "\n");
            }
        }
        fis.close();
        return buffer.toString();
    }

    private boolean waitTillCBInvoke() {
        Log.i("LogLevel.DEBUG", "Checking if Callback is Invoked or Not");
        int count = 0;

        while (!mIsCbInvoked) {

            try {
                Thread.sleep(DELAY_SHORT);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

            count++;
            Log.i("LogLevel.DEBUG", "Waiting for = " + count + " second/s");

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
                        Resource_URI, // resource
                                      // URI
                        RESOURCE_TYPE, // resource type name
                        RESOURCE_INTERFACE, // using default interface
                        null, // use default entity handler
                        EnumSet.of(ResourceProperty.DISCOVERABLE,
                                ResourceProperty.OBSERVABLE));
                sResourceHandleList.add(slocalLightResourceHandle);
                System.out.println("Create Local Resource is success.");
                return true;
            } catch (OcException e) {
                Log.e(TAG, e.toString());
            }
        }
        return false;
    }

    /**
     * Copy client db Cbor file from assets folder to app data files dir
     */
    public void copyCborFromAsset(Context context, String oicClientCborDbFile) {
        InputStream inputStream = null;
        OutputStream outputStream = null;
        int length;
        byte[] buffer = new byte[BUFFER_SIZE];
        filePath = context.getFilesDir().getPath() + "/"; // data/data/<package>/files/

        try {
            inputStream = context.getAssets().open(oicClientCborDbFile);
            File file = new File(filePath);
            // check files directory exists
            if (!(file.exists() && file.isDirectory())) {
                file.mkdirs();
            }
            outputStream = new FileOutputStream(filePath + oicClientCborDbFile);
            while ((length = inputStream.read(buffer)) != -1) {
                outputStream.write(buffer, 0, length);
            }
        } catch (NullPointerException e) {
            Log.e(TAG, "Null pointer exception " + e.getMessage());
        } catch (FileNotFoundException e) {
            Log.e(TAG, "Cbor svr db file not found " + e.getMessage());
        } catch (IOException e) {
            Log.e(TAG, oicClientCborDbFile + " file copy failed");
        } finally {
            if (inputStream != null) {
                try {
                    inputStream.close();
                } catch (IOException e) {
                    Log.e(TAG, e.getMessage());
                }
            }
            if (outputStream != null) {
                try {
                    outputStream.close();
                } catch (IOException e) {
                    Log.e(TAG, e.getMessage());
                }
            }
        }
    }

}
