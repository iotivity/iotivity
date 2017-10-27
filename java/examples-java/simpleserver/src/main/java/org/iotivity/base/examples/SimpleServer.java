/*
 *******************************************************************
 *
 * Copyright 2015 Intel Corporation.
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
 * SimpleServer
 * <p/>
 * SimpleServer is a sample OIC server application.
 * It creates a Light and waits for the incoming client calls to handle
 * various request scenarios.
 */
public class SimpleServer {

    static List<Light> lights = new LinkedList<>();

    /**
     * A local method to configure and initialize platform, and then create a light resource.
     */
    private static void startSimpleServer() {
        String path = "";
        // This assumes the oic_svr_db_server.dat file is in the same location as the SimpleServer.jar file
        try {
            path = SimpleServer.class.getProtectionDomain().getCodeSource().getLocation().toURI().getPath();
            path = path.substring(0, path.lastIndexOf('/'));
        } catch (URISyntaxException e) {
            msg(e.getMessage() + " unable to find local file path.");
        }

        PlatformConfig platformConfig = new PlatformConfig(
                ServiceType.IN_PROC,
                ModeType.SERVER,
                "0.0.0.0", // By setting to "0.0.0.0", it binds to all available interfaces
                0,         // Uses randomly available port
                QualityOfService.LOW,
                path + "/oic_svr_db_server.dat",
                path + "/simpleserver_introspection.dat"
        );

        msg("Configuring platform.");
        OcPlatform.Configure(platformConfig);

        createNewLightResource("/a/light", "John's light");

        msg("Waiting for the requests...");
        printLine();
    }

    public static void createNewLightResource(String resourceUri, String resourceName){
        msg("Creating a light");
        Light light = new Light(
                resourceUri,     //URI
                resourceName,    //name
                false,           //state
                0                //power
        );
        msg(light.toString());

        msg("Registering light as a resource");
        try {
            light.registerResource();
        } catch (OcException e) {
            msgError(TAG, e.toString());
            msg("Failed to register a light resource");
        }
        lights.add(light);
    }

    private static void stopSimpleServer() {
        for (Light light : lights) {
            try {
                light.unregisterResource();
            } catch (OcException e) {
                msgError(TAG, e.toString());
                msg("Failed to unregister a light resource");
            }
        }
        lights.clear();

        msg("All created resources have been unregistered");
        printLine();
    }

    //******************************************************************************
    // End of the OIC specific code
    //******************************************************************************

    private final static String TAG = SimpleServer.class.getSimpleName();

    public static void main(String[] args) {
        Runtime.getRuntime().addShutdownHook(new Thread() {
            public void run() {
                stopSimpleServer();
            }
        });

        startSimpleServer();
        while (true) {
            sleep(1);
        }
    }

    public static void sleep(int seconds) {
        try {
            Thread.sleep(seconds * 1000);
        } catch (InterruptedException e) {
            e.printStackTrace();
            msgError(TAG, e.toString());
        }
    }

    public static void msg(final String text) {
        System.out.println("[O]" + TAG + " | " + text);
    }

    public static void msg(final String tag, final String text) {
        System.out.println("[O]" + tag + " | " + text);
    }

    public static void msgError(final String tag ,final String text) {
        System.out.println("[E]" + tag + " | " + text);
    }

    public static void printLine() {
        msg("------------------------------------------------------------------------");
    }

}
