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

package org.iotivity.service.es.test.helper;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import org.iotivity.base.ModeType;
import org.iotivity.base.OcException;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcProvisioning;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;

import android.content.Context;
import android.util.Log;

public class ESOICStackHelper {

    private static final String DATABASES               = "databases";
    private static final String FILES                   = "files";
    private static String       filePath                = "";
    public static final String  OIC_CLIENT_JSON_DB_FILE = "oic_svr_db_client.dat";
    public static final String  OIC_SQL_DB_FILE         = "PDM.db";
    private static final int    BUFFER_SIZE             = 0;
    private static final String TAG                     = "ProvisionInit : ";

    public static boolean copyJsonFromAsset(Context context) {
        InputStream inputStream = null;
        OutputStream outputStream = null;
        int length;
        byte[] buffer = new byte[BUFFER_SIZE];
        try {
            inputStream = context.getAssets().open(OIC_CLIENT_JSON_DB_FILE);
            File file = new File(filePath);
            if (!(file.exists() && file.isDirectory())) {
                file.mkdirs();
            }
            outputStream = new FileOutputStream(
                    filePath + OIC_CLIENT_JSON_DB_FILE);
            while ((length = inputStream.read(buffer)) != -1) {
                outputStream.write(buffer, 0, length);
            }
        } catch (NullPointerException e) {
            Log.e(TAG, e.getMessage());
            return false;
        } catch (FileNotFoundException e) {
            Log.e(TAG, e.getMessage());
            return false;
        } catch (IOException e) {
            Log.e(TAG, e.getMessage());
            return false;
        } finally {
            if (inputStream != null) {
                try {
                    inputStream.close();
                } catch (IOException e) {
                    Log.e(TAG, e.getMessage());
                    return false;
                }
            }
            if (outputStream != null) {
                try {
                    outputStream.close();
                } catch (IOException e) {
                    Log.e(TAG, e.getMessage());
                    return false;
                }
            }
        }
        return true;
    }

    public static void initOICStack(Context context) {
        filePath = context.getFilesDir().getPath() + "/";
        PlatformConfig cfg = new PlatformConfig(context, ServiceType.IN_PROC,
                ModeType.CLIENT_SERVER, "0.0.0.0", 0, QualityOfService.LOW,
                filePath + OIC_CLIENT_JSON_DB_FILE);
        try {
            OcPlatform.Configure(cfg);

            String sqlDbPath = context.getFilesDir().getAbsolutePath()
                    .replace(FILES, DATABASES) + File.separator;
            File file = new File(sqlDbPath);
            if (!(file.isDirectory())) {
                file.mkdirs();
                Log.d(TAG, sqlDbPath);
            }
            Log.d(TAG, sqlDbPath);
            OcProvisioning.provisionInit(sqlDbPath + OIC_SQL_DB_FILE);
        } catch (OcException e) {
            Log.e(TAG, e.getMessage());
            return;
        } catch (UnsatisfiedLinkError e) {
            Log.d(TAG, e.getMessage());
            return;
        }
    }
}