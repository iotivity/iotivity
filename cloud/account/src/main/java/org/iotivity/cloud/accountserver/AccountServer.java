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
package org.iotivity.cloud.accountserver;

import java.net.InetSocketAddress;
import java.util.Scanner;
import java.util.concurrent.TimeUnit;

import org.iotivity.cloud.accountserver.resources.account.SecAccountResource;
import org.iotivity.cloud.accountserver.resources.account.session.SecSessionResource;
import org.iotivity.cloud.accountserver.resources.account.tokenrefresh.SecTokenRefreshResource;
import org.iotivity.cloud.base.healthcheck.HealthHolder;
import org.iotivity.cloud.base.healthcheck.SimpleHealthHolder;
import org.iotivity.cloud.base.server.SimpleHttpServer;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.iotivity.cloud.accountserver.db.AccountDBManager;
import org.iotivity.cloud.accountserver.resources.account.AccountResource;
import org.iotivity.cloud.accountserver.resources.account.session.SessionResource;
import org.iotivity.cloud.accountserver.resources.account.tokenrefresh.TokenRefreshResource;
import org.iotivity.cloud.accountserver.resources.acl.group.GroupResource;
import org.iotivity.cloud.accountserver.resources.acl.id.AclResource;
import org.iotivity.cloud.accountserver.resources.acl.invite.InviteResource;
import org.iotivity.cloud.accountserver.resources.acl.verify.AclVerifyResource;
import org.iotivity.cloud.accountserver.resources.credprov.cert.CertificateResource;
import org.iotivity.cloud.accountserver.resources.credprov.crl.CrlResource;
import org.iotivity.cloud.base.ServerSystem;
import org.iotivity.cloud.base.resource.CloudPingResource;
import org.iotivity.cloud.base.server.CoapServer;

/**
 *
 * This class is in charge of running of account server.
 *
 */
public class AccountServer {
    private final static Logger     Log                 = LoggerFactory.getLogger(AccountServer.class);
    private static int              coapServerPort;
    private static boolean          tlsMode;
    private static String           databaseHost;
    private static String           webLogHost;
    private static boolean          healthCheckEnabled;

    public static void main(String[] args) throws Exception {
        Log.info("Starting Account Server");

        if (!parseConfiguration(args)) {
            Log.error("\nCoAP-server <Port> Database <Address> <Port> TLS-mode <0|1> are required.\n"
                    + "ex) " + Constants.DEFAULT_COAP_PORT
                    + " 127.0.0.1 27017 0\n");
            return;
        }

        AccountDBManager.createInstance(databaseHost);
        HealthHolder healthHolder =  new SimpleHealthHolder(TimeUnit.SECONDS,100);
        ServerSystem serverSystem = new ServerSystem();
        serverSystem.addResource(new CloudPingResource(healthHolder));
        serverSystem.addResource(new AccountResource());
        serverSystem.addResource(new SessionResource());
        serverSystem.addResource(new TokenRefreshResource());
        serverSystem.addResource(new GroupResource());
        serverSystem.addResource(new AclResource());
        serverSystem.addResource(new AclVerifyResource());
        serverSystem.addResource(new CertificateResource());
        serverSystem.addResource(new CrlResource());
        serverSystem.addResource(new AclResource());
        serverSystem.addResource(new InviteResource());

        //change in version 2.0.0
        serverSystem.addResource(new SecAccountResource());
        serverSystem.addResource(new SecTokenRefreshResource());
        serverSystem.addResource(new SecSessionResource());

        serverSystem.addServer(
                new CoapServer(new InetSocketAddress(coapServerPort)));

        if(healthCheckEnabled){
            serverSystem.addServer(new SimpleHttpServer(new InetSocketAddress(8000),healthHolder));
        }

        serverSystem.startSystem(tlsMode);

        Scanner in = new Scanner(System.in, "UTF-8");

        System.out.println("press 'q' to terminate");

        while (!in.nextLine().equals("q"));

        in.close();

        System.out.println("Terminating...");

        serverSystem.stopSystem();

        System.out.println("Terminated");
    }

    private static boolean parseConfiguration(String[] args) {
        // configuration provided by arguments
        if (args.length == 4) {
            coapServerPort = Integer.parseInt(args[0]);
            databaseHost = args[1] + ":" + args[2];
            tlsMode = Integer.parseInt(args[3]) == 1;
            healthCheckEnabled = false;
            return true;
        }
        // configuration provided by docker env
        String tlsModeEnv = System.getenv("TLS_MODE");
        if (tlsModeEnv != null) {
            coapServerPort = Integer.parseInt(System.getenv("COAP_PORT"));
            databaseHost = System.getenv("MONGODB_ADDRESS") + ":"
                    + System.getenv("MONGODB_PORT");
            tlsMode = Integer.parseInt(tlsModeEnv) == 1;
            healthCheckEnabled = Integer.parseInt(System.getenv("HEALTH_CHECK_ENABLED")) == 1;
            return true;
        }
        return false;
    }
}
