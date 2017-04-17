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
import org.iotivity.cloud.base.server.HttpServer;
import org.iotivity.cloud.base.server.WebSocketServer;
import org.iotivity.cloud.ciserver.DeviceServerSystem.CoapDevicePool;
import org.iotivity.cloud.ciserver.resources.KeepAliveResource;
import org.iotivity.cloud.ciserver.resources.RouteResource;
import org.iotivity.cloud.ciserver.resources.proxy.account.Account;
import org.iotivity.cloud.ciserver.resources.proxy.account.AccountSession;
import org.iotivity.cloud.ciserver.resources.proxy.account.Acl;
import org.iotivity.cloud.ciserver.resources.proxy.account.AclGroup;
import org.iotivity.cloud.ciserver.resources.proxy.account.AclInvite;
import org.iotivity.cloud.ciserver.resources.proxy.account.Certificate;
import org.iotivity.cloud.ciserver.resources.proxy.account.Crl;
import org.iotivity.cloud.ciserver.resources.proxy.mq.MessageQueue;
import org.iotivity.cloud.ciserver.resources.proxy.rd.DevicePresence;
import org.iotivity.cloud.ciserver.resources.proxy.rd.ResourceDirectory;
import org.iotivity.cloud.ciserver.resources.proxy.rd.ResourceFind;
import org.iotivity.cloud.ciserver.resources.proxy.rd.ResourcePresence;
import org.iotivity.cloud.util.Log;

public class CloudInterfaceServer {

    public static void main(String[] args) throws Exception {

        Log.Init();

        System.out.println("-----CI SERVER-------");

        if (args.length < 8 || args.length > 12) {
            Log.e("\nCoAP-server <Port> and RD-server <Address> <Port> Account-server <Address> <Port> "
                    + "MQ-broker <Address> <Port> HC-proxy <HTTP-port> Websocket-server <Port> and TLS-mode <0|1> are required.\n"
                    + "and WebSocketLog-Server <Address> <Port> (optional)\n"
                    + "ex) 5683 127.0.0.1 5684 127.0.0.1 5685 127.0.0.1 5686 80 8000 0 127.0.0.1 8080\n");
            return;
        }

        // CoAP-TCP server port
        int coapPort = Integer.parseInt(args[0]);
        // HTTP-CoAP proxy server port
        int hcProxyPort = Integer.parseInt(args[7]);
        // CoAP-Websocket server port
        int websocketPort = Integer.parseInt(args[8]);

        boolean hcProxyMode = hcProxyPort > 0;
        boolean websocketMode = websocketPort > 0;

        boolean tlsMode = Integer.parseInt(args[9]) == 1;

        if (args.length >= 11) {
            Log.InitWebLog(args[10], args[11], CloudInterfaceServer.class
                    .getSimpleName().toString());
        }

        ConnectorPool.addConnection("rd", new InetSocketAddress(args[1],
                Integer.parseInt(args[2])), tlsMode);
        ConnectorPool.addConnection("account", new InetSocketAddress(args[3],
                Integer.parseInt(args[4])), tlsMode);
        ConnectorPool.addConnection("mq", new InetSocketAddress(args[5],
                Integer.parseInt(args[6])), tlsMode);

        DeviceServerSystem deviceServer = new DeviceServerSystem();

        Account acHandler = new Account();
        AccountSession acSessionHandler = new AccountSession();
        ResourceDirectory rdHandler = new ResourceDirectory();
        ResourceFind resHandler = new ResourceFind();
        ResourcePresence adHandler = new ResourcePresence();
        DevicePresence prsHandler = new DevicePresence();
        MessageQueue mqHandler = new MessageQueue();
        Acl aclHandler = new Acl();
        AclGroup aclGroupHandler = new AclGroup();
        Certificate certHandler = new Certificate();
        AclInvite aclInviteHandler = new AclInvite();
        Crl crlHandler = new Crl();
        CoapDevicePool devicePool = deviceServer.getDevicePool();

        deviceServer.addResource(acHandler);

        deviceServer.addResource(acSessionHandler);

        deviceServer.addResource(rdHandler);

        deviceServer.addResource(resHandler);

        deviceServer.addResource(adHandler);

        deviceServer.addResource(prsHandler);

        deviceServer.addResource(mqHandler);

        deviceServer.addResource(aclHandler);

        deviceServer.addResource(aclGroupHandler);

        deviceServer.addResource(certHandler);

        deviceServer.addResource(aclInviteHandler);

        deviceServer.addResource(crlHandler);

        KeepAliveResource resKeepAlive = new KeepAliveResource(
                new int[] { 1, 2, 4, 8 });

        deviceServer.addResource(resKeepAlive);

        deviceServer.addResource(new RouteResource(devicePool));

        deviceServer.addServer(new CoapServer(new InetSocketAddress(coapPort)));

        // Add HTTP Server for HTTP-to-CoAP Proxy
        if (hcProxyMode) {
            deviceServer.addServer(new HttpServer(new InetSocketAddress(
                    hcProxyPort)));
        }

        if (websocketMode) {
            deviceServer.addServer(new WebSocketServer(new InetSocketAddress(
                    websocketPort)));
        }

        deviceServer.startSystem(tlsMode);

        resKeepAlive.startSessionChecker(3000, 6000);

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
