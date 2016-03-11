package org.iotivity.cloud.base;

import java.net.InetSocketAddress;

import org.iotivity.cloud.base.SessionManagerTest.CoapClientHandler;
import org.iotivity.cloud.base.protocols.coap.CoapRequest;
import org.iotivity.cloud.base.protocols.coap.enums.CoapMethod;
import org.junit.Test;

import io.netty.channel.ChannelHandlerContext;

public class ResourceManagerTest {

    private static class SampleResource extends Resource {

        public SampleResource() {
            setUri("sampleUri");
        }

        @Override
        public void onRequestReceived(ChannelHandlerContext ctx,
                CoapRequest request) {
            // TODO Auto-generated method stub
        }
    }

    @Test
    public void testChannelRead0ChannelHandlerContextCoapRequest()
            throws Exception {

        ResourceManager resourceManager = new ResourceManager();

        resourceManager.registerResource(new SampleResource());

        CoapServer coapServer = new CoapServer();
        CoapClient coapClient = new CoapClient();
        CoapClientHandler coapClientHandler = new CoapClientHandler();
        coapServer.startServer(new InetSocketAddress(5683));
        coapClient.addHandler(coapClientHandler);
        coapClient.startClient(new InetSocketAddress("127.0.0.1", 5683));

        CoapRequest request = new CoapRequest(CoapMethod.GET);
        request.setUriPath("sampleUri");

        resourceManager.channelRead0(coapClientHandler.connectCtx, request);

        coapClient.stopClient();
        coapServer.stopServer();
    }

    @Test
    public void testRegisterResource() {
        ResourceManager resourceManager = new ResourceManager();

        resourceManager.registerResource(new SampleResource());
    }

    @Test
    public void testUnregisterResource() {
        SampleResource sampleResource = new SampleResource();
        ResourceManager resourceManager = new ResourceManager();

        resourceManager.registerResource(sampleResource);
        resourceManager.unregisterResource(sampleResource);
    }
}