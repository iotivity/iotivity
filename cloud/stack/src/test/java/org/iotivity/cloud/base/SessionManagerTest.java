package org.iotivity.cloud.base;

import static org.junit.Assert.assertNotNull;

import java.net.InetSocketAddress;

import org.iotivity.cloud.base.protocols.coap.CoapResponse;
import org.junit.Test;

import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.SimpleChannelInboundHandler;

public class SessionManagerTest {

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

    @Test
    public void testAddSession() throws Exception {
        SessionManager sessionManager = new SessionManager();
        CoapServer coapServer = new CoapServer();
        CoapClient coapClient = new CoapClient();
        CoapClientHandler coapClientHandler = new CoapClientHandler();
        coapServer.startServer(new InetSocketAddress(5683));
        coapClient.addHandler(coapClientHandler);
        coapClient.startClient(new InetSocketAddress("127.0.0.1", 5683));

        sessionManager.addSession("sampleDid", coapClientHandler.connectCtx);

        coapClient.stopClient();
        coapServer.stopServer();
    }

    @Test
    public void testRemoveSession() throws Exception {
        SessionManager sessionManager = new SessionManager();
        CoapServer coapServer = new CoapServer();
        CoapClient coapClient = new CoapClient();
        CoapClientHandler coapClientHandler = new CoapClientHandler();
        coapServer.startServer(new InetSocketAddress(5683));
        coapClient.addHandler(coapClientHandler);
        coapClient.startClient(new InetSocketAddress("127.0.0.1", 5683));

        sessionManager.addSession("sampleDid", coapClientHandler.connectCtx);
        sessionManager.removeSession("sampleDid");

        coapClient.stopClient();
        coapServer.stopServer();
    }

    @Test
    public void testRemoveSessionByChannel() throws Exception {
        SessionManager sessionManager = new SessionManager();
        CoapServer coapServer = new CoapServer();
        CoapClient coapClient = new CoapClient();
        CoapClientHandler coapClientHandler = new CoapClientHandler();
        coapServer.startServer(new InetSocketAddress(5683));
        coapClient.addHandler(coapClientHandler);
        coapClient.startClient(new InetSocketAddress("127.0.0.1", 5683));

        sessionManager.addSession("sampleDid", coapClientHandler.connectCtx);
        sessionManager.removeSessionByChannel(coapClientHandler.connectCtx);

        coapClient.stopClient();
        coapServer.stopServer();
    }

    @Test
    public void testQuerySession() throws Exception {
        SessionManager sessionManager = new SessionManager();
        CoapServer coapServer = new CoapServer();
        CoapClient coapClient = new CoapClient();
        CoapClientHandler coapClientHandler = new CoapClientHandler();
        coapServer.startServer(new InetSocketAddress(5683));
        coapClient.addHandler(coapClientHandler);
        coapClient.startClient(new InetSocketAddress("127.0.0.1", 5683));

        sessionManager.addSession("sampleDid", coapClientHandler.connectCtx);
        assertNotNull(sessionManager.querySession("sampleDid"));

        coapClient.stopClient();
        coapServer.stopServer();
    }
}
