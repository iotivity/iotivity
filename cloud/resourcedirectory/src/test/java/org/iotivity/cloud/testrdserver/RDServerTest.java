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
package org.iotivity.cloud.testrdserver;

import java.net.InetSocketAddress;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedHashMap;
import org.iotivity.cloud.base.CoapClient;
import org.iotivity.cloud.base.CoapServer;
import org.iotivity.cloud.base.ResourceManager;
import org.iotivity.cloud.base.protocols.coap.CoapRequest;
import org.iotivity.cloud.base.protocols.coap.CoapResponse;
import org.iotivity.cloud.base.protocols.coap.enums.CoapMethod;
import org.iotivity.cloud.rdserver.Constants;
import org.iotivity.cloud.rdserver.resources.ResourceDirectoryResource;
import org.iotivity.cloud.util.Cbor;
import org.iotivity.cloud.util.JSONUtil;
import org.junit.Test;

import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.SimpleChannelInboundHandler;

public class RDServerTest {

    private ResourceDirectoryResource rdResource = new ResourceDirectoryResource();
    private CoapServer                coapServer = null;
    private CoapClient                coapClient = null;

    static class CoapClientHandler
            extends SimpleChannelInboundHandler<CoapResponse> {

        ChannelHandlerContext connectCtx = null;

        @Override
        public void channelActive(ChannelHandlerContext ctx) throws Exception {
            connectCtx = ctx;
        }

        @Override
        protected void channelRead0(ChannelHandlerContext arg0,
                CoapResponse arg1) throws Exception {
            // TODO Auto-generated method stub

        }
    }

    public void startServer() throws Exception {
        coapServer = new CoapServer();
        ResourceManager resourceManager = new ResourceManager();
        coapServer.addHandler(resourceManager);
        resourceManager.registerResource(new ResourceDirectoryResource());
        coapServer.startServer(new InetSocketAddress(5683));
    }

    public ChannelHandlerContext startClient() throws Exception {
        coapClient = new CoapClient();
        CoapClientHandler coapHandler = new CoapClientHandler();
        coapClient.addHandler(coapHandler);
        coapClient.startClient(new InetSocketAddress("127.0.0.1", 5683));
        if (coapHandler.connectCtx == null) {
                throw new IllegalArgumentException("connectCtx is null");
        }
        return coapHandler.connectCtx;
    }

    public CoapRequest makePublishPayload() throws Exception {

        CoapRequest request = new CoapRequest(CoapMethod.POST);
        request.setUriPath(Constants.RD_URI);
        request.setUriQuery("rt=oic.wk.rdpub");
        request.setToken("1234".getBytes(StandardCharsets.UTF_8));

        ArrayList<Object> payload = new ArrayList<Object>();

        HashMap<Object, Object> tags = new HashMap<Object, Object>();
        tags.put("di", "98f7483c-5a31-4161-ba7e-9c13e0d");
        tags.put("bm", (int) 1);
        tags.put("ttl", (int) 86400);

        ArrayList<LinkedHashMap<Object, Object>> publishLinks = new ArrayList<LinkedHashMap<Object, Object>>();
        LinkedHashMap<Object, Object> link = new LinkedHashMap<Object, Object>();
        link.put("href", "/a/light");
        ArrayList<String> rt = new ArrayList<String>();
        ArrayList<String> itf = new ArrayList<String>();
        ArrayList<String> mt = new ArrayList<String>();
        rt.add("core.light");
        link.put("rt", rt);

        itf.add("oic.if.baseline");
        link.put("if", itf);

        mt.add("application/json");
        link.put("mt", mt);

        link.put("ins", 1);

        publishLinks.add(link);

        payload.add(tags);
        payload.add(publishLinks);

        Cbor<ArrayList<Object>> cbor = new Cbor<ArrayList<Object>>();

        request.setPayload(cbor.encodingPayloadToCbor(payload));

        return request;
    }

    @Test
    public void testHandlePostRequest() throws Exception {

        startServer();
        ChannelHandlerContext ctx = startClient();

        rdResource.handlePostRequest(ctx, makePublishPayload());

        coapClient.stopClient();
        coapServer.stopServer();

    }

    @Test
    public void testHandleGetRequest_notExistVaule() throws Exception {

        CoapRequest request = new CoapRequest(CoapMethod.GET);
        request.setUriPath(Constants.RD_URI);
        request.setUriQuery("rt=core.light");
        request.setToken("1234".getBytes(StandardCharsets.UTF_8));

        startServer();
        ChannelHandlerContext ctx = startClient();

        rdResource.handleGetRequest(ctx, request);

        coapClient.stopClient();
        coapServer.stopServer();
    }

    @Test
    public void testHandleGetRequest_existValue() throws Exception {

        CoapRequest request = new CoapRequest(CoapMethod.GET);
        request.setUriPath(Constants.RD_URI);
        request.setUriQuery("rt=core.light");
        request.setToken("1234".getBytes(StandardCharsets.UTF_8));

        startServer();
        ChannelHandlerContext ctx = startClient();

        rdResource.handlePostRequest(ctx, makePublishPayload());

        rdResource.handleGetRequest(ctx, request);

        coapClient.stopClient();
        coapServer.stopServer();
    }

    @Test
    public void testHandleGetRequestBySt_existValue() throws Exception {

        CoapRequest request = new CoapRequest(CoapMethod.GET);
        request.setUriPath(Constants.RD_URI);
        request.setUriQuery("rt=core.light&st=didList");
        request.setToken("1234".getBytes(StandardCharsets.UTF_8));

        HashMap<Object, Object> data = new HashMap<Object, Object>();
        ArrayList<String> didList = new ArrayList<String>();
        didList.add("98f7483c-5a31-4161-ba7e-9c13e0d");
        data.put("devices", didList);
        String payload = JSONUtil.writeJSON(data);
        if (payload != null) {
            request.setPayload(payload.getBytes(StandardCharsets.UTF_8));
        }
        else {
                throw new IllegalArgumentException("payload writeJson error");
        }

        startServer();
        ChannelHandlerContext ctx = startClient();

        rdResource.handlePostRequest(ctx, makePublishPayload());

        rdResource.handleGetRequest(ctx, request);

        coapClient.stopClient();
        coapServer.stopServer();
    }

    @Test
    public void testHandleDeleteRequestByDi_notExistVaule() throws Exception {

        CoapRequest request = new CoapRequest(CoapMethod.DELETE);
        request.setUriPath(Constants.RD_URI);
        request.setUriQuery("di=98f7483c-5a31-4161-ba7e-9c13e0d");
        request.setToken("1234".getBytes(StandardCharsets.UTF_8));

        startServer();
        ChannelHandlerContext ctx = startClient();

        rdResource.handlePostRequest(ctx, makePublishPayload());

        rdResource.handleDeleteRequest(ctx, request);

        coapClient.stopClient();
        coapServer.stopServer();
    }

    @Test
    public void testHandleDeleteRequestByDi_existVaule() throws Exception {

        CoapRequest request = new CoapRequest(CoapMethod.DELETE);
        request.setUriPath(Constants.RD_URI);
        request.setUriQuery("di=98f7483c-5a31-4161-ba7e-9c13e0d");
        request.setToken("1234".getBytes(StandardCharsets.UTF_8));

        startServer();
        ChannelHandlerContext ctx = startClient();

        rdResource.handleDeleteRequest(ctx, request);

        coapClient.stopClient();
        coapServer.stopServer();
    }

    @Test
    public void testHandleDeleteRequestByIns_notExistVaule() throws Exception {

        CoapRequest request = new CoapRequest(CoapMethod.DELETE);
        request.setUriPath(Constants.RD_URI);
        request.setUriQuery("di=98f7483c-5a31-4161-ba7e-9c13e0d&ins=1");
        request.setToken("1234".getBytes(StandardCharsets.UTF_8));

        startServer();
        ChannelHandlerContext ctx = startClient();

        rdResource.handleDeleteRequest(ctx, request);

        coapClient.stopClient();
        coapServer.stopServer();
    }

    @Test
    public void testHandleDeleteRequestByIns_existVaule() throws Exception {

        CoapRequest request = new CoapRequest(CoapMethod.DELETE);
        request.setUriPath(Constants.RD_URI);
        request.setUriQuery("di=98f7483c-5a31-4161-ba7e-9c13e0d&ins=1");
        request.setToken("1234".getBytes(StandardCharsets.UTF_8));

        startServer();
        ChannelHandlerContext ctx = startClient();

        rdResource.handlePostRequest(ctx, makePublishPayload());

        rdResource.handleDeleteRequest(ctx, request);

        coapClient.stopClient();
        coapServer.stopServer();
    }
}
