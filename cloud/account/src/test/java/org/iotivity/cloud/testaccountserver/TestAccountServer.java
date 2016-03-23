package org.iotivity.cloud.testaccountserver;

import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.SimpleChannelInboundHandler;

import java.net.InetSocketAddress;
import java.nio.charset.StandardCharsets;

import org.junit.Test;
import org.iotivity.cloud.accountserver.Constants;
import org.iotivity.cloud.accountserver.resources.AccountResource;
import org.iotivity.cloud.accountserver.resources.AuthResource;
import org.iotivity.cloud.base.CoapClient;
import org.iotivity.cloud.base.CoapServer;
import org.iotivity.cloud.base.ResourceManager;
import org.iotivity.cloud.base.protocols.coap.CoapRequest;
import org.iotivity.cloud.base.protocols.coap.CoapResponse;
import org.iotivity.cloud.base.protocols.coap.enums.CoapMethod;
import org.iotivity.cloud.util.JSONUtil;

public class TestAccountServer {

    private AuthResource    authResource    = new AuthResource();
    private AccountResource accountResource = new AccountResource();

    private CoapServer      coapServer      = null;
    private CoapClient      coapClient      = null;
    private static String   sessionCode     = null;

    static class CoapClientHandler extends
            SimpleChannelInboundHandler<CoapResponse> {

        ChannelHandlerContext connectCtx = null;

        @Override
        public void channelActive(ChannelHandlerContext ctx) throws Exception {
            connectCtx = ctx;
        }

        @Override
        protected void channelRead0(ChannelHandlerContext arg0,
                CoapResponse arg1) throws Exception {

            if (arg1.getTokenString().equals("1111")) {

                String json = arg1.getPayloadString();

                sessionCode = JSONUtil.parseJSON(json, "session");
            }

        }
    }

    public void startServer() throws Exception {

        coapServer = new CoapServer();

        ResourceManager resourceManager = new ResourceManager();
        coapServer.addHandler(resourceManager);

        resourceManager.registerResource(new AuthResource());
        resourceManager.registerResource(new AccountResource());

        coapServer.startServer(new InetSocketAddress(5685));
    }

    public ChannelHandlerContext startClient() throws Exception {

        coapClient = new CoapClient();

        CoapClientHandler coapHandler = new CoapClientHandler();
        coapClient.addHandler(coapHandler);

        coapClient.startClient(new InetSocketAddress("127.0.0.1", 5685));

        return coapHandler.connectCtx;
    }

    @Test
    public void testHandleRegisterRequest() throws Exception {

        System.out.println("Write your authCode before test by following url.");
        System.out
                .println("https://github.com/login?return_to=%2Flogin%2Foauth%2Fauthorize%3Fclient_id%3Dea9c18f540323b0213d0%26redirect_uri%3Dhttp%253A%252F%252Fwww.example.com%252Foauth_callback%252F");

        String authCode = "7243699de9726d05e74c"; // write your authCode here.
        String authServer = "github";

        String json = "{\"authcode\":\"" + authCode + "\",\"authprovider\":\""
                + authServer + "\"}";

        CoapRequest request = new CoapRequest(CoapMethod.POST);
        request.setUriPath(Constants.AUTH_URI);
        request.setUriQuery("reqtype=register");
        request.setToken("1111".getBytes(StandardCharsets.UTF_8));
        request.setPayload(json.getBytes(StandardCharsets.UTF_8));

        startServer();
        ChannelHandlerContext ctx = startClient();
        authResource.onRequestReceived(ctx, request);

        coapClient.stopClient();
        coapServer.stopServer();
    }

    @Test
    public void testHandlerLoginReqeust() throws Exception {

        String json = "{\"session\":\"" + sessionCode + "\"}";

        CoapRequest request = new CoapRequest(CoapMethod.POST);
        request.setUriPath(Constants.AUTH_URI);
        request.setUriQuery("reqtype=login");
        request.setToken("1234".getBytes(StandardCharsets.UTF_8));
        request.setPayload(json.getBytes(StandardCharsets.UTF_8));

        startServer();
        ChannelHandlerContext ctx = startClient();
        authResource.onRequestReceived(ctx, request);

        coapClient.sendRequest(request);

        coapClient.stopClient();
        coapServer.stopServer();
    }

    @Test
    public void testHandlerPublishReqeust() throws Exception {

        String userId = "eyedglen";
        String deviceId = "d0001";
        String json = "{\"userid\":\"" + userId + "\",\"deviceId\":\""
                + deviceId + "\"}";

        CoapRequest request = new CoapRequest(CoapMethod.POST);
        request.setUriPath(Constants.ACCOUNT_URI);
        request.setUriQuery("reqtype=publish");
        request.setToken("1234".getBytes(StandardCharsets.UTF_8));
        request.setPayload(json.getBytes(StandardCharsets.UTF_8));

        startServer();
        ChannelHandlerContext ctx = startClient();
        accountResource.onRequestReceived(ctx, request);

        coapClient.stopClient();
        coapServer.stopServer();
    }

    @Test
    public void testHandlerFindReqeust() throws Exception {

        String userId = "eyedglen";
        String json = "{\"userid\":\"" + userId + "\"}";

        CoapRequest request = new CoapRequest(CoapMethod.GET);
        request.setUriPath(Constants.ACCOUNT_URI);
        request.setUriQuery("reqtype=find");
        request.setToken("1234".getBytes(StandardCharsets.UTF_8));
        request.setPayload(json.getBytes(StandardCharsets.UTF_8));

        startServer();
        ChannelHandlerContext ctx = startClient();
        accountResource.onRequestReceived(ctx, request);

        coapClient.stopClient();
        coapServer.stopServer();
    }
}
