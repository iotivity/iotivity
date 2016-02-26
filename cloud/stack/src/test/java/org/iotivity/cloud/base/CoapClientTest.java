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

import org.iotivity.cloud.base.protocols.coap.CoapRequest;
import org.iotivity.cloud.base.protocols.coap.enums.CoapMethod;
import org.junit.Test;

import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.SimpleChannelInboundHandler;

public class CoapClientTest {

    private static class CoapHandler
            extends SimpleChannelInboundHandler<CoapRequest> {
        @Override
        protected void channelRead0(ChannelHandlerContext ctx, CoapRequest msg)
                throws Exception {
            // TODO Auto-generated method stub
        }
    }

    @Test
    public void testAddHandler() throws Exception {
        CoapServer coapServer = new CoapServer();
        coapServer.startServer(new InetSocketAddress(5683));
        CoapClient coapClient = new CoapClient();
        coapClient.startClient(new InetSocketAddress("127.0.0.1", 5683));
        coapClient.addHandler(new CoapHandler());
        coapClient.stopClient();
        coapServer.stopServer();
    }

    @Test
    public void testAddHandlerSendRequest() throws Exception {
        CoapServer coapServer = new CoapServer();
        coapServer.startServer(new InetSocketAddress(5683));

        CoapClient coapClient = new CoapClient();
        coapClient.startClient(new InetSocketAddress("127.0.0.1", 5683));
        coapClient.addHandler(new CoapHandler());

        CoapRequest request = new CoapRequest(CoapMethod.GET);
        coapClient.sendRequest(request);

        CoapRequest request2 = new CoapRequest(CoapMethod.GET);
        request2.setToken("1234".getBytes());
        coapClient.sendRequest(request2);

        CoapRequest request3 = new CoapRequest(CoapMethod.GET);
        request3.setPayload("sample1".getBytes());
        coapClient.sendRequest(request3);

        CoapRequest request4 = new CoapRequest(CoapMethod.GET);
        request4.setToken("5576".getBytes());
        request4.setPayload("sample2".getBytes());
        coapClient.sendRequest(request4);

        CoapRequest request5 = new CoapRequest(CoapMethod.GET);
        request5.setToken("565761".getBytes());
        coapClient.sendRequest(request5);

        CoapRequest request6 = new CoapRequest(CoapMethod.GET);
        coapClient.sendRequest(request6);

        CoapRequest request7 = new CoapRequest(CoapMethod.GET);
        coapClient.sendRequest(request7);

        coapClient.stopClient();
        coapServer.stopServer();
    }

    @Test
    public void testStartStopClient() throws Exception {
        CoapServer coapServer = new CoapServer();
        coapServer.startServer(new InetSocketAddress(5683));
        CoapClient coapClient = new CoapClient();
        coapClient.startClient(new InetSocketAddress("127.0.0.1", 5683));
        coapClient.stopClient();
        coapServer.stopServer();
    }

    @Test
    public void testSendRequest() throws Exception {
        CoapServer coapServer = new CoapServer();
        coapServer.startServer(new InetSocketAddress(5683));
        CoapClient coapClient = new CoapClient();
        coapClient.startClient(new InetSocketAddress("127.0.0.1", 5683));
        CoapRequest request = new CoapRequest(CoapMethod.GET);
        coapClient.sendRequest(request);
        coapClient.stopClient();
        coapServer.stopServer();
    }
}
