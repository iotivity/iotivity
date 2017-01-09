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

import org.iotivity.base.OcException;

import java.util.LinkedList;
import java.util.List;

/**
 * SimpleServer
 * <p/>
 * SimpleServer is a sample OIC server application. It creates a Light and waits
 * for the incoming client calls to handle various request scenarios.
 */
public class SimpleServer {

    private List<ServerLight> lights = new LinkedList<>();

    public SimpleServer() {
        createNewLightResource(ServerLight.DEFAULT_RESOURCE_URI, ServerLight.DEFAULT_RESOURCE_NAME);

        msg("Waiting for the requests...");
        printLine();
    }

    protected void createNewLightResource(String resourceUri, String resourceName) {
        msg("Creating a light");
        ServerLight light = new ServerLight(this, resourceUri, resourceName, false, 0);
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

    public void stopSimpleServer() {
        for (ServerLight light : lights) {
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

    // ******************************************************************************
    // End of the OIC specific code
    // ******************************************************************************

    private static final String TAG = SimpleServer.class.getSimpleName();

    protected void msg(final String text) {
        SimpleClientServer.msg(TAG, text);
    }

    protected void msgError(final String tag, final String text) {
        SimpleClientServer.msgError(tag, text);
    }

    private void printLine() {
        SimpleClientServer.printLine(TAG);
    }
}
