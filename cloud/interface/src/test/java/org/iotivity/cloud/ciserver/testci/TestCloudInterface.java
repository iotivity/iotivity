package org.iotivity.cloud.ciserver.testci;

import java.net.InetSocketAddress;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedHashMap;

import org.iotivity.cloud.base.CoapClient;
import org.iotivity.cloud.base.CoapServer;
import org.iotivity.cloud.base.ResourceManager;
import org.iotivity.cloud.base.SessionManager;
import org.iotivity.cloud.base.protocols.coap.CoapRequest;
import org.iotivity.cloud.base.protocols.coap.CoapResponse;
import org.iotivity.cloud.base.protocols.coap.enums.CoapMethod;
import org.iotivity.cloud.ciserver.Constants;
import org.iotivity.cloud.ciserver.protocols.CoapAuthHandler;
import org.iotivity.cloud.ciserver.protocols.CoapRelayHandler;
import org.iotivity.cloud.ciserver.resources.KeepAliveResource;
import org.iotivity.cloud.util.Cbor;
import org.iotivity.cloud.util.CoapLogHandler;
import org.junit.Test;

import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.SimpleChannelInboundHandler;

public class TestCloudInterface {

    private SessionManager  sessionManager  = new SessionManager();
    private ResourceManager resourceManager = new ResourceManager();

    private CoapServer        coapServer        = null;
    private CoapClient        coapClient        = null;
    private CoapRelayHandler  coapRelayHandler  = new CoapRelayHandler(
            sessionManager);
    private KeepAliveResource keepAliveResource = new KeepAliveResource(
            sessionManager, new int[] { 1, 2, 4, 8 });

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
            // TODO : receive response
            System.out.println("Get Response");
        }
    }

    public void startServer() throws Exception {

        coapServer = new CoapServer();

        coapServer.addHandler(new CoapLogHandler());
        coapServer.addHandler(coapRelayHandler);

        coapServer.addHandler(resourceManager);
        resourceManager.registerResource(keepAliveResource);

        coapServer.startServer(new InetSocketAddress(5683));
    }

    public ChannelHandlerContext startClient() throws Exception {

        coapClient = new CoapClient();

        CoapClientHandler coapHandler = new CoapClientHandler();
        coapClient.addHandler(coapHandler);

        coapClient.addHandler(new CoapAuthHandler());

        coapClient.startClient(new InetSocketAddress("127.0.0.1", 5683));

        return coapHandler.connectCtx;
    }

    public CoapRequest makePayload(CoapRequest request) throws Exception {
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

    public CoapRequest makeinterval(CoapRequest request) throws Exception {

        HashMap<Object, Object> payload = new HashMap<Object, Object>();
        payload.put("in", 1);

        Cbor<ArrayList<Object>> cbor = new Cbor<ArrayList<Object>>();

        request.setPayload(cbor.encodingPayloadToCbor(payload));

        return request;
    }

    @Test
    public void TestKeepAlivePutInterval() throws Exception {

        CoapRequest request = new CoapRequest(CoapMethod.PUT);
        request.setUriPath(Constants.KEEP_ALIVE_URI);
        request.setToken("1234".getBytes(StandardCharsets.UTF_8));
        makeinterval(request);

        startServer();
        startClient();

        coapClient.sendRequest(request);

        System.out.println("Waiting for KeepAliveTask..");
        Thread.sleep(30000);

        coapClient.stopClient();
        coapServer.stopServer();
    }

    @Test
    public void TestKeepAliveGetFirst() throws Exception {

        CoapRequest request = new CoapRequest(CoapMethod.GET);
        request.setUriPath(Constants.KEEP_ALIVE_URI);
        request.setToken("1234".getBytes(StandardCharsets.UTF_8));

        startServer();
        startClient();

        coapClient.sendRequest(request);

        coapClient.stopClient();
        coapServer.stopServer();
    }

    @Test
    public void TestDiscoveryDevice() throws Exception {

        CoapRequest request = new CoapRequest(CoapMethod.GET);
        request.setUriPath(Constants.WELL_KNOWN_URI);
        request.setUriQuery("rt=oic.wk.rdpub");
        request.setToken("1234".getBytes(StandardCharsets.UTF_8));

        startServer();
        startClient();

        coapClient.sendRequest(request);

        coapClient.stopClient();
        coapServer.stopServer();
    }

    @Test
    public void TestPublishDevice() throws Exception {

        CoapRequest request = new CoapRequest(CoapMethod.POST);
        request.setUriPath(Constants.RD_URI);
        request.setUriQuery("rt=oic.wk.rdpub");
        request.setToken("1234".getBytes(StandardCharsets.UTF_8));
        makePayload(request);

        startServer();
        startClient();

        coapClient.sendRequest(request);

        coapClient.stopClient();
        coapServer.stopServer();
    }

    @Test
    public void TestAuthURI() throws Exception {

        CoapRequest request = new CoapRequest(CoapMethod.GET);
        request.setUriPath(Constants.AUTH_URI);
        request.setUriQuery("rt=oic.wk.rdpub");
        request.setToken("1234".getBytes(StandardCharsets.UTF_8));
        makePayload(request);

        startServer();
        startClient();

        coapClient.sendRequest(request);

        coapClient.stopClient();
        coapServer.stopServer();
    }

    @Test
    public void TestRequestGetMessageToDeviceCIOwner() throws Exception {

        CoapRequest request = new CoapRequest(CoapMethod.GET);
        request.setUriPath(
                "/10.113.64.98/98f7483c-5a31-4161-ba7e-9c13e0d/a/light");
        request.setToken("1234".getBytes(StandardCharsets.UTF_8));
        makePayload(request);

        startServer();
        startClient();

        coapClient.sendRequest(request);

        coapServer.stopServer();
        coapClient.stopClient();
    }

    @Test
    public void TestRequestGetMessageToDeviceNotCIOwner() throws Exception {

        CoapRequest request = new CoapRequest(CoapMethod.GET);
        request.setUriPath(
                "/98f7483c-5a31-4161-ba7e-9c13e0d/a/light");
        request.setToken("1234".getBytes(StandardCharsets.UTF_8));
        makePayload(request);

        startServer();
        startClient();

        coapClient.sendRequest(request);

        coapServer.stopServer();
        coapClient.stopClient();
    }

    @Test
    public void TestRequestPutMessageToDevice() throws Exception {

        CoapRequest request = new CoapRequest(CoapMethod.PUT);
        request.setUriPath(
                "/98f7483c-5a31-4161-ba7e-9c13e0d/a/light");
        request.setToken("1234".getBytes(StandardCharsets.UTF_8));
        makePayload(request);

        startServer();
        startClient();

        coapClient.sendRequest(request);

        coapServer.stopServer();
        coapClient.stopClient();
    }
}
