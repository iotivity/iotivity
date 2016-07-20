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

import org.iotivity.cloud.base.connector.ConnectorPool;
import org.iotivity.cloud.base.server.CoapServer;
import org.iotivity.cloud.ciserver.DeviceServerSystem.CoapDevicePool;
import org.iotivity.cloud.ciserver.resources.DiResource;
import org.iotivity.cloud.ciserver.resources.KeepAliveResource;
import org.iotivity.cloud.ciserver.resources.proxy.Account;
import org.iotivity.cloud.ciserver.resources.proxy.DevicePresence;
import org.iotivity.cloud.ciserver.resources.proxy.MessageQueue;
import org.iotivity.cloud.ciserver.resources.proxy.ResourceDirectory;
import org.iotivity.cloud.ciserver.resources.proxy.ResourceFind;
import org.iotivity.cloud.ciserver.resources.proxy.ResourcePresence;
import org.iotivity.cloud.util.ErrorLogger;
import org.iotivity.cloud.util.FileLogger;
import org.iotivity.cloud.util.Logger;

public class CloudInterfaceServer {

    public static void main(String[] args) throws Exception {

        System.setOut(FileLogger.createLoggingProxy(System.out));

        System.out.println("-----CI SERVER-------");

        if (args.length != 8) {
            Logger.e(
                    "coap server port and RDServer_Address port AccountServer_Address Port MQBroker_Address Port and TLS mode required\n"
                            + "ex) 5683 127.0.0.1 5684 127.0.0.1 5685 127.0.0.1 5686 0\n");
            return;
        }

        boolean tlsMode = Integer.parseInt(args[7]) == 1;

        ConnectorPool.addConnection("rd",
                new InetSocketAddress(args[1], Integer.parseInt(args[2])),
                tlsMode);
        ConnectorPool.addConnection("account",
                new InetSocketAddress(args[3], Integer.parseInt(args[4])),
                tlsMode);
        ConnectorPool.addConnection("mq",
                new InetSocketAddress(args[5], Integer.parseInt(args[6])),
                tlsMode);

        ErrorLogger.Init();

        DeviceServerSystem deviceServer = new DeviceServerSystem();

        Account acHandler = new Account();
        ResourceDirectory rdHandler = new ResourceDirectory();
        ResourceFind resHandler = new ResourceFind();
        ResourcePresence adHandler = new ResourcePresence();
        DevicePresence prsHandler = new DevicePresence();
        MessageQueue mqHandler = new MessageQueue();

        CoapDevicePool devicePool = deviceServer.getDevicePool();

        deviceServer.addResource(acHandler);

        deviceServer.addResource(rdHandler);

        deviceServer.addResource(resHandler);

        deviceServer.addResource(adHandler);

        deviceServer.addResource(prsHandler);

        deviceServer.addResource(mqHandler);

        KeepAliveResource resKeepAlive = new KeepAliveResource(
                new int[] { 1, 2, 4, 8 });

        deviceServer.addResource(resKeepAlive);

        deviceServer.addResource(new DiResource(devicePool));

        deviceServer.addServer(new CoapServer(
                new InetSocketAddress(Integer.parseInt(args[0]))));

        // deviceServer.addServer(new HttpServer(new InetSocketAddress(8080)));

        deviceServer.startSystem(tlsMode);

        resKeepAlive.startSessionChecker();

        Scanner in = new Scanner(System.in);

        System.out.println("press 'q' to terminate");

        while (!in.nextLine().equals("q"));

        in.close();

        System.out.println("Terminating...");

        resKeepAlive.stopSessionChecker();

        deviceServer.stopSystem();

        System.out.println("Terminated");
    }
}
