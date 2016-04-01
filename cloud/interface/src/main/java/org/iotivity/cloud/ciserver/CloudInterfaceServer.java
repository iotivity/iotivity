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
package org.iotivity.cloud.ciserver;

import java.net.InetSocketAddress;
import java.util.Scanner;

import org.iotivity.cloud.base.CoapServer;
import org.iotivity.cloud.base.ResourceManager;
import org.iotivity.cloud.base.SessionManager;
import org.iotivity.cloud.ciserver.protocols.CoapAuthHandler;
import org.iotivity.cloud.ciserver.protocols.CoapRelayHandler;
import org.iotivity.cloud.ciserver.resources.KeepAliveResource;
import org.iotivity.cloud.util.CoapLogHandler;
import org.iotivity.cloud.util.Logger;

public class CloudInterfaceServer {

    public static void main(String[] args) throws Exception {

        System.out.println("-----CI SERVER-------");

        if (args.length != 5) {
            Logger.e(
                    "coap server port and RDServer_Address port AccountServer_Address Port required\n"
                            + "ex) 5683 127.0.0.1 5684 127.0.0.1 5685\n");
            return;
        }

        ResourceManager resourceManager = null;
        SessionManager sessionManager = null;
        CoapServer coapServer = null;

        CoapRelayHandler relayHandler = null;
        CoapAuthHandler authHandler = null;

        KeepAliveResource keepAliveResource = null;

        coapServer = new CoapServer();

        sessionManager = new SessionManager();

        resourceManager = new ResourceManager();

        relayHandler = new CoapRelayHandler(sessionManager);

        authHandler = new CoapAuthHandler();

        keepAliveResource = new KeepAliveResource(sessionManager,
                new int[] { 1, 2, 4, 8 });

        coapServer.addHandler(new CoapLogHandler());

        coapServer.addHandler(authHandler);

        coapServer.addHandler(relayHandler);

        coapServer.addHandler(resourceManager);

        resourceManager.registerResource(keepAliveResource);

        authHandler.startHandler(args[3], Integer.parseInt(args[4]));

        relayHandler.startHandler(args[1], Integer.parseInt(args[2]), args[3],
                Integer.parseInt(args[4]));

        coapServer
                .startServer(new InetSocketAddress(Integer.parseInt(args[0])));

        keepAliveResource.startSessionChecker();

        Scanner in = new Scanner(System.in, "UTF-8");

        System.out.println("press 'q' to terminate");

        while (!in.nextLine().equals("q"));

        in.close();

        System.out.println("Terminating...");

        keepAliveResource.stopSessionChecker();

        coapServer.stopServer();

        relayHandler.stopHandler();

        authHandler.stopHandler();

        System.out.println("Terminated");
    }
}
