/******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
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

package org.iotivity.service.rc;

import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import org.iotivity.base.ModeType;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;

import android.content.Context;
import android.content.res.AssetManager;
import android.util.Log;

import org.iotivity.service.rc.RCClient;
import static org.iotivity.service.rc.RCTestAppStaticUtil.*;

public class RCTestAppUtilityFunctions {
    private static final String LOG_TAG = "RCTestApp : RCTestAppUtilityFunctions";

    public static boolean copyFilesToDevice(Context context) {
        boolean isSuccess = true;
        AssetManager assetManager = context.getAssets();
        String[] files = null;
        try {
            files = assetManager.list("lib");
        } catch (IOException e) {
            Log.e(LOG_TAG, e.getMessage());
            isSuccess = false;
        }

        for (String filename : files) {
            InputStream in = null;
            OutputStream out = null;
            try {
                in = assetManager.open("lib/" + filename);
                out = new FileOutputStream(context.getFilesDir().getParent()
                        + "/files/" + filename);
                copyIndividualFile(in, out);
                in.close();
                in = null;
                out.flush();
                out.close();
                out = null;
            } catch (Exception e) {
                Log.e(LOG_TAG, e.getMessage());
                isSuccess = false;
            }
        }

        return isSuccess;
    }

    private static void copyIndividualFile(InputStream in, OutputStream out)
            throws IOException {

        Log.d(LOG_TAG, "copyIndividualFile");
        byte[] buffer = new byte[2048];
        int read;
        while ((read = in.read(buffer)) != -1) {
            out.write(buffer, 0, read);
        }
    }

    public static void rcConfigurePlatform(Context appContext) {
        OcPlatform.Configure(new PlatformConfig(appContext,
                ServiceType.IN_PROC, ModeType.CLIENT_SERVER, "0.0.0.0", 0,
                QualityOfService.LOW));
        Log.i(LOG_TAG, "Configuration done successfully");
    }

    public static void waitInSecond(int time) {
        int count = 0;
        while (!RCClient.isServerResponse) {

            Log.d(TAG, "\nWaiting For Server Repsonse..........\n");

            if (count++ == time) {
                Log.d(TAG, "Timeout for server response");
                break;
            }

            try {
                Thread.sleep(WAITING_TIME_IN_SECOND);
            } catch (InterruptedException ex) {
                Log.d(TAG, ex.getMessage());
                Thread.currentThread().interrupt();
            }
        }
    }
}
