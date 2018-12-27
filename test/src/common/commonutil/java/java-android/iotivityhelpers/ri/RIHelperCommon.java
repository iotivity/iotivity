/******************************************************************
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
 ******************************************************************/

package org.iotivity.test.ri.common;

import android.content.Context;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;

import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;
import java.util.Arrays;
import java.util.ArrayList;
import java.util.EnumSet;
import java.util.List;
import java.util.LinkedList;
import java.util.Map;

import org.iotivity.base.ModeType;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;

import org.iotivity.testcase.IoTivityLog;
import org.iotivity.testcase.IoTivityTc;
import org.iotivity.test.braodcast.TestBroadCast;

public class RIHelperCommon {
    protected static Context    s_helperContext;
    public static final String  CONFIG_FILE_PATH                 = "/data/local/tmp/";
    private static final String DATABASES                        = "databases";
    private static final String FILES                            = "files";
    private static final String TAG                              = "RIHelperCommon";
    private static final int    BUFFER_SIZE                      = 1024;
    public static String        s_filePath                       = "";
    public static String        s_sqLPath                        = "";
    public static final String  DIR_NAME_FILE                    = "files";
    public static final String  DIR_NAME_DATABASES               = "databases";

    private static final String JUSTWORKS_SERVER_UNOWNED_CBOR_01 = "oic_svr_db_server_justworks.dat";
    private static final String JUSTWORKS_SERVER_UNOWNED_CBOR_02 = "oic_svr_db_server.dat";
    private static final String PRECONFIG_SERVER_UNOWNED_CBOR_01 = "oic_svr_db_server_preconfpin.dat";
    private static final String JUSTWORKS_SERVER_01              = "oic_svr_db_server.dat";
    private static final String JUSTWORKS_SERVER_02              = "oic_svr_db_server.dat";
    private static final String UNOWNED_DIR                      = "unowned";
    private static final String KILL_ALL_SERVER                  = "kill all server";
    public static final String  START_JUSTWORKS_SERVER_01        = "./iotivity_pm_server "
            + JUSTWORKS_SERVER_UNOWNED_CBOR_01 + " 1";
    public static final String  START_JUSTWORKS_SERVER_02        = "./iotivity_pm_server "
            + JUSTWORKS_SERVER_UNOWNED_CBOR_02 + " 1";
    public static final String  START_PRE_CONFIG_SERVER_01       = "./iotivity_pm_server "
            + PRECONFIG_SERVER_UNOWNED_CBOR_01 + " 3";
    public static final String  START_RE_SERVER                  = "./iotivity_re_server";
    public static final String  PROVISION_DB_FILE                = "./Pdm.db";
    public static final String  DEVICE_PROP_CBOR_FILE            = "./device_properties.dat";

    private TestBroadCast       mTestBroadCast;

    protected RIHelperCommon(IoTivityTc iotivityTcObj) {
        s_helperContext = iotivityTcObj.getInstrumentation().getTargetContext();
        s_filePath = s_helperContext.getFilesDir().getPath();

        s_sqLPath = s_helperContext.getFilesDir().getAbsolutePath()
                .replace(FILES, DATABASES) + File.separator;
        mTestBroadCast = new TestBroadCast(s_helperContext);
    }

    public boolean configClientServerPlatform() {
        PlatformConfig cfg = new PlatformConfig(s_helperContext,
                ServiceType.IN_PROC, ModeType.CLIENT_SERVER, "0.0.0.0", 0,
                QualityOfService.HIGH);
        OcPlatform.Configure(cfg);
        IoTivityLog.i(TAG, "Platform Configuration with Context Succesful");
        return true;
    }

    public void configClientServerPlatform(String fileName) {
        PlatformConfig cfg = new PlatformConfig(s_helperContext,
                ServiceType.IN_PROC, ModeType.CLIENT_SERVER, "0.0.0.0", // bind
                                                                        // to
                                                                        // all
                                                                        // available
                // interfaces
                0, QualityOfService.LOW, s_filePath + fileName);
        OcPlatform.Configure(cfg);
        IoTivityLog.i(TAG, "Platform Configuration with Context Succesful");
    }

    public static String read(String filename) throws IOException {

        StringBuffer buffer = new StringBuffer();

        String Read = "";
        FileInputStream fis = s_helperContext.openFileInput(filename);
        BufferedReader reader = new BufferedReader(new InputStreamReader(fis));
        if (fis != null) {
            while ((Read = reader.readLine()) != null) {
                buffer.append(Read + "\n");
            }
        }
        fis.close();
        return buffer.toString();
    }

    public boolean copyCborFromAsset(String clientCborDbFile) {
        InputStream inputStream = null;
        OutputStream outputStream = null;
        int length;
        byte[] buffer = new byte[BUFFER_SIZE];
        s_filePath = s_helperContext.getFilesDir().getPath() + "/"; // data/data/<package>/files/

        try {
            inputStream = s_helperContext.getAssets().open(clientCborDbFile);
            File file = new File(s_filePath);
            // check files directory exists
            if (!(file.exists() && file.isDirectory())) {
                file.mkdirs();
            }
            outputStream = new FileOutputStream(s_filePath + clientCborDbFile);
            while ((length = inputStream.read(buffer)) != -1) {
                outputStream.write(buffer, 0, length);
            }
        } catch (NullPointerException e) {
            IoTivityLog.e(TAG, "Null pointer exception " + e.getMessage());
        } catch (FileNotFoundException e) {
            IoTivityLog.e(TAG, "Cbor svr db file not found " + e.getMessage());
        } catch (IOException e) {
            IoTivityLog.e(TAG, clientCborDbFile + " file copy failed");
        } finally {
            if (inputStream != null) {
                try {
                    inputStream.close();
                } catch (IOException e) {
                    IoTivityLog.e(TAG, e.getMessage());
                }
            }
            if (outputStream != null) {
                try {
                    outputStream.close();
                } catch (IOException e) {
                    IoTivityLog.e(TAG, e.getMessage());
                }
            }
        }

        return true;
    }

    public void enableMocking() {
        System.setProperty("dexmaker.dexcache",
                s_helperContext.getCacheDir().getPath());
    }

    public void startREServer(String command) {
        mTestBroadCast.sendBroadCastToTcpClient(command, true);
    }

    public void startSecuredServer(String command) {
        mTestBroadCast.sendBroadCastToTcpClient(command, true);
    }

    public void stopServers() {
        mTestBroadCast.sendBroadCastToTcpClient(KILL_ALL_SERVER, true);
    }

    public void clearAll() {

        File folder = new File(s_filePath);

        File[] listOfFiles = folder.listFiles();

        for (int i = 0; i < listOfFiles.length; i++) {
            if (listOfFiles[i].isFile()) {
                IoTivityLog.i(TAG, "File " + listOfFiles[i].getName());
                listOfFiles[i].delete();
            } else if (listOfFiles[i].isDirectory()) {
                IoTivityLog.i(TAG, "Directory " + listOfFiles[i].getName());
            }
        }

        String filePath = s_helperContext.getFilesDir().getAbsolutePath()
                .replace(DIR_NAME_FILE, DIR_NAME_DATABASES) + File.separator;
        IoTivityLog.i(TAG, "Clearing File from : " + filePath);

        folder = new File(filePath);

        if (!folder.exists()) {
            return;
        }

        listOfFiles = folder.listFiles();

        for (int i = 0; i < listOfFiles.length; i++) {
            if (listOfFiles[i].isFile()) {
                IoTivityLog.i(TAG, "File " + listOfFiles[i].getName());
                listOfFiles[i].delete();
            } else if (listOfFiles[i].isDirectory()) {
                IoTivityLog.i(TAG, "Directory " + listOfFiles[i].getName());
            }
        }
    }

}
