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

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;

import java.util.Arrays;
import java.util.EnumSet;
import java.util.List;
import java.util.ArrayList;
import java.util.Map;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.file.Files;
import java.nio.file.StandardCopyOption;
import java.util.ArrayList;

import org.iotivity.base.ModeType;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;

import org.iotivity.testcase.IoTivityLog;
import org.iotivity.testcase.IoTivityTc;

public class RIHelperCommon {
    private static final String       DATABASES                        = "databases";
    private static final String       FILES                            = "files";
    private static final String       TAG                              = "RIHelper Java";
    public static final String        CONFIG_FILE_PATH                 = "./";

    private static final int          BUFFER_SIZE                      = 1024;
    public static String              s_filePath                       = "";
    public static String              s_sqLPath                        = "";

    private static final String       JUSTWORKS_SERVER_UNOWNED_CBOR_01 = "oic_svr_db_server_justworks.dat";
    private static final String       JUSTWORKS_SERVER_UNOWNED_CBOR_02 = "oic_svr_db_server.dat";
    private static final String       PRECONFIG_SERVER_UNOWNED_CBOR_01 = "oic_svr_db_server_preconfpin.dat";
    private static final String       JUSTWORKS_SERVER_01              = "oic_svr_db_server.dat";
    private static final String       JUSTWORKS_SERVER_02              = "oic_svr_db_server.dat";
    private static final String       UNOWNED_DIR                      = "unowned";

    public static final String        START_JUSTWORKS_SERVER_01        = "./iotivity_pm_server "
            + JUSTWORKS_SERVER_UNOWNED_CBOR_01 + " 1";
    public static final String        START_JUSTWORKS_SERVER_02        = "./iotivity_pm_server "
            + JUSTWORKS_SERVER_UNOWNED_CBOR_02 + " 1";
    public static final String        START_PRE_CONFIG_SERVER_01       = "./iotivity_pm_server "
            + PRECONFIG_SERVER_UNOWNED_CBOR_01 + " 3";
    public static final String        START_RE_SERVER                  = "./iotivity_re_server";
    public static final String        PROVISION_DB_FILE                = "./Pdm.db";
    public static final String        DEVICE_PROP_CBOR_FILE            = "./device_properties.dat";

    private static ArrayList<Process> mProcessVector                   = new ArrayList<Process>();

    public RIHelperCommon(IoTivityTc iotivityTcObj) {
        IoTivityLog.i(TAG, "=Constructor Called");
        s_sqLPath = "";
        s_filePath = "";
    }

    /**
     * @brief Function is for configuring platform
     */
    public void configClientServerPlatform() {
        IoTivityLog.i("RIHelperCommon", "configClientServerPlatform");
        PlatformConfig cfg = new PlatformConfig(ServiceType.IN_PROC,
                ModeType.CLIENT_SERVER, "0.0.0.0", 0, QualityOfService.HIGH);
        OcPlatform.Configure(cfg);
        IoTivityLog.i("RIHelperCommon", "configClientServerPlatform Out");
    }

    public void configClientServerPlatform(String fileName) {
        PlatformConfig cfg = new PlatformConfig(ServiceType.IN_PROC,
                ModeType.CLIENT_SERVER, "0.0.0.0", // bind
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
        FileInputStream fis = new FileInputStream(filename);
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

        return true;
    }

    public void enableMocking() {

    }

    public void startREServer(String executionCommand) {

        new Thread() {
            public void run() {
                IoTivityLog.i(TAG, "Execution Command : " + executionCommand);
                try {
                    Process server = Runtime.getRuntime()
                            .exec(executionCommand.split(" "));
                    mProcessVector.add(server);
                    BufferedReader b = new BufferedReader(
                            new InputStreamReader(server.getInputStream()));
                    String line;
                    while ((line = b.readLine()) != null) {
                        System.out.println(line);
                    }
                } catch (IOException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
            }
        }.start();

    }

    public void startSecuredServer(String executionCommand) {

        new Thread() {
            public void run() {
                IoTivityLog.i(TAG, "Execution Command : " + executionCommand);

                String src = "./" + UNOWNED_DIR + "/"
                        + executionCommand.split(" ")[1];
                String dest = "./" + executionCommand.split(" ")[1];

                try {
                    File filesrc = new File(src);
                    File fileDest = new File(dest);

                    Files.copy(filesrc.toPath(), fileDest.toPath(),
                            StandardCopyOption.REPLACE_EXISTING);

                    Process server = Runtime.getRuntime()
                            .exec(executionCommand.split(" "));
                    mProcessVector.add(server);
                    BufferedReader b = new BufferedReader(
                            new InputStreamReader(server.getInputStream()));
                    String line;
                    while ((line = b.readLine()) != null) {
                        System.out.println(line);
                    }
                } catch (IOException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
            }
        }.start();

    }

    public void stopServers() {
        for (int i = 0; i < mProcessVector.size(); i++) {
            Process p = mProcessVector.get(i);
            if (null != p) {
                if (p.isAlive()) {
                    System.out.println("Killing Is Alive");
                    p.destroy();
                } else {
                    System.out.println("Process Already Dead");
                }
            }
        }
    }

    public void clearAll() {
        try {
            File file = new File(PROVISION_DB_FILE);
            file.delete();

            file = new File(DEVICE_PROP_CBOR_FILE);
            file.delete();

        } catch (Exception e) {
            e.printStackTrace();
        }

    }

}
