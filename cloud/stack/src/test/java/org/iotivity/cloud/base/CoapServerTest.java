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
package org.iotivity.cloud.base;

import java.net.InetSocketAddress;

import org.iotivity.cloud.util.CoapLogHandler;
import org.junit.Test;

public class CoapServerTest {
    @Test
    public void testAddHandler() throws Exception {
        CoapServer server = new CoapServer();
        server.startServer(new InetSocketAddress(5683));
        server.addHandler(new CoapLogHandler());
        server.stopServer();
    }

    @Test
    public void testStartStopServer() throws Exception {
        CoapServer server = new CoapServer();
        server.startServer(new InetSocketAddress(5683));
        server.stopServer();
    }
}
