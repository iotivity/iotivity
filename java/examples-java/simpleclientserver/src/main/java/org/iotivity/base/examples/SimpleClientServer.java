/*
 *******************************************************************
 *
 * Copyright 2017 Intel Corporation.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
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
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

package org.iotivity.base.examples;

import org.iotivity.base.ModeType;
import org.iotivity.base.OcException;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;

import java.net.URISyntaxException;
import java.util.LinkedList;
import java.util.List;

/**
 * SimpleClientServer
 * <p/>
 * SimpleClientServer is a sample OIC client/server application. Return code is
 * 0 if no errors detected.
 */
public class SimpleClientServer {

    private static final String TAG = SimpleClientServer.class.getSimpleName();

    public static void main(String[] args) {
        String path = "";
        // This assumes the oic_svr_db_server.dat file is in the same location as the SimpleServer.jar file
        try {
            path = SimpleServer.class.getProtectionDomain().getCodeSource().getLocation().toURI().getPath();
            path = path.substring(0, path.lastIndexOf('/'));
        } catch (URISyntaxException e) {
            msg(TAG, e.getMessage() + " unable to find local file path.");
        }

        PlatformConfig platformConfig = new PlatformConfig(ServiceType.IN_PROC, ModeType.CLIENT_SERVER, "0.0.0.0", 0,
                QualityOfService.LOW,
                path + "/oic_svr_db_server.dat");
        msg(TAG, "Configuring platform.");
        OcPlatform.Configure(platformConfig);

        SimpleServer server = new SimpleServer();
        Object waitLock = new Object();
        SimpleClient client = new SimpleClient(waitLock);

        int returnCode = -1;
        synchronized (waitLock) {
            try {
                waitLock.wait(60 * 1000);
                if (client.isRunning()) {
                    msgError(TAG, "Failed to complete on time!");
                } else {
                    returnCode = 0;
                }
            } catch (InterruptedException e) {
                msgError(TAG, e.toString());
            }
        }

        server.stopSimpleServer();
        System.exit(returnCode);
    }

    private static String getMessage(String tag, String text, boolean isError) {
        return "[" + (isError ? "E" : "O") + "]" + tag + " | " + text;
    }

    protected static void msg(final String tag, final String text) {
        System.out.println(getMessage(tag, text, false));
    }

    protected static void msgError(final String tag, final String text) {
        System.out.println(getMessage(tag, text, true));
    }

    protected static void printLine(final String tag) {
        msg(tag, "------------------------------------------------------------------------");
    }
}
