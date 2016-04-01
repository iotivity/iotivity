/*
 * //******************************************************************
 * //
 * // Copyright 2016 Samsung Electronics All Rights Reserved.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 * //
 * // Licensed under the Apache License, Version 2.0 (the "License");
 * // you may not use this file except in compliance with the License.
 * // You may obtain a copy of the License at
 * //
 * //      http://www.apache.org/licenses/LICENSE-2.0
 * //
 * // Unless required by applicable law or agreed to in writing, software
 * // distributed under the License is distributed on an "AS IS" BASIS,
 * // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * // See the License for the specific language governing permissions and
 * // limitations under the License.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */
package org.iotivity.cloud.rdserver;

import java.net.InetSocketAddress;
import java.util.Scanner;

import org.iotivity.cloud.base.CoapServer;
import org.iotivity.cloud.base.ResourceManager;
import org.iotivity.cloud.rdserver.resources.ResourceDirectoryResource;
import org.iotivity.cloud.util.Logger;

public class ResourceDirectoryServer {

    public static void main(String[] args) throws Exception {

        System.out.println("-----RD SERVER-----");

        if (args.length != 1) {
            Logger.e("coap server port required");
            return;
        }

        ResourceManager resourceManager = null;

        CoapServer coapServer = null;

        coapServer = new CoapServer();

        resourceManager = new ResourceManager();

        coapServer.addHandler(resourceManager);

        resourceManager.registerResource(new ResourceDirectoryResource());

        coapServer
                .startServer(new InetSocketAddress(Integer.parseInt(args[0])));

        Scanner in = new Scanner(System.in, "UTF-8");

        System.out.println("press 'q' to terminate");

        while (!in.nextLine().equals("q"));

        in.close();

        System.out.println("Terminating...");

        coapServer.stopServer();

        System.out.println("Terminated");
    }
}
