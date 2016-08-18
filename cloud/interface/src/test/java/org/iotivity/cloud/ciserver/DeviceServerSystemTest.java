package org.iotivity.cloud.ciserver;

import static java.util.concurrent.TimeUnit.SECONDS;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;
import static org.mockito.Mockito.mock;

import java.util.HashMap;
import java.util.concurrent.CountDownLatch;

import org.iotivity.cloud.base.OICConstants;
import org.iotivity.cloud.base.device.CoapDevice;
import org.iotivity.cloud.base.device.Device;
import org.iotivity.cloud.base.device.IRequestChannel;
import org.iotivity.cloud.base.protocols.IRequest;
import org.iotivity.cloud.base.protocols.IResponse;
import org.iotivity.cloud.base.protocols.MessageBuilder;
import org.iotivity.cloud.base.protocols.coap.CoapRequest;
import org.iotivity.cloud.base.protocols.enums.ContentFormat;
import org.iotivity.cloud.base.protocols.enums.RequestMethod;
import org.iotivity.cloud.base.server.CoapServer;
import org.iotivity.cloud.base.server.HttpServer;
import org.iotivity.cloud.ciserver.DeviceServerSystem.CoapDevicePool;
import org.iotivity.cloud.ciserver.resources.proxy.account.Account;
import org.iotivity.cloud.ciserver.resources.proxy.mq.MessageQueue;
import org.iotivity.cloud.ciserver.resources.proxy.rd.ResourceDirectory;
import org.iotivity.cloud.ciserver.resources.proxy.rd.ResourceFind;
import org.iotivity.cloud.ciserver.resources.proxy.rd.ResourcePresence;
import org.iotivity.cloud.util.Cbor;
import org.junit.Before;
import org.junit.Test;
import org.mockito.InjectMocks;
import org.mockito.Mock;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.mockito.invocation.InvocationOnMock;
import org.mockito.stubbing.Answer;

import io.netty.channel.Channel;
import io.netty.channel.ChannelHandlerContext;
import io.netty.util.Attribute;

public class DeviceServerSystemTest {
    private String                          di                   = "B371C481-38E6-4D47-8320-7688D8A5B58C";
    String                                  userId               = "testuser";
    String                                  accessToken          = "1689c70ffa245effc563017fee36d250";
    private CoapDevice                      mockDevice           = mock(
            CoapDevice.class);
    private Device                          device               = mock(
            Device.class);
    IResponse                               res                  = null;
    IRequest                                req                  = null;
    DeviceServerSystem                      deviceServerSystem   = new DeviceServerSystem();
    final CountDownLatch                    latch                = new CountDownLatch(
            1);

    @Mock
    IRequestChannel                         requestChannel;

    @InjectMocks
    DeviceServerSystem.CoapLifecycleHandler coapLifecycleHandler = deviceServerSystem.new CoapLifecycleHandler();
    @InjectMocks
    DeviceServerSystem.CoapAuthHandler      coapAuthHandler      = deviceServerSystem.new CoapAuthHandler();

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.initMocks(this);
    }

    @Before
    public void testAddHttpServer() throws Exception {
        HttpServer httpServer = new HttpServer(null);
        deviceServerSystem.addServer(httpServer);
    }

    @Before
    public void testAddCoapServer() throws Exception {
        CoapServer coapServer = new CoapServer(null);
        deviceServerSystem.addServer(coapServer);
    }

    @Test
    public void testGetDevicePool() throws Exception {
        CoapDevicePool devicePool = deviceServerSystem.getDevicePool();
        if (devicePool != null) {
            System.out.println("devicePool returned :" + devicePool);
        }
    }

    @Test
    public void testAddDevice() throws Exception {
        CoapDevice coapDevice = new CoapDevice(null);
        coapDevice.updateDevice(di, userId, accessToken);
        CoapDevicePool devicePool = deviceServerSystem.getDevicePool();
        devicePool.addDevice(coapDevice);
    }

    @Test
    public void testRemoveNotRegisteredDevice() throws Exception {
        CoapDevice coapDevice = new CoapDevice(null);
        coapDevice.updateDevice(di, userId, accessToken);
        CoapDevicePool devicePool = deviceServerSystem.getDevicePool();
        devicePool.removeDevice(coapDevice);
    }

    @Test
    public void testRemoveDevice() throws Exception {
        CoapDevice coapDevice = new CoapDevice(null);
        coapDevice.updateDevice(di, userId, accessToken);
        CoapDevicePool devicePool = deviceServerSystem.getDevicePool();
        devicePool.addDevice(coapDevice);
        devicePool.removeDevice(coapDevice);
    }

    @Test
    public void testQueryDevice() throws Exception {
        CoapDevice coapDevice = new CoapDevice(null);
        coapDevice.updateDevice(di, userId, accessToken);
        CoapDevicePool devicePool = deviceServerSystem.getDevicePool();
        devicePool.addDevice(coapDevice);
        devicePool.queryDevice(di);
    }

    @Test
    public void testStopSystem() throws Exception {
        deviceServerSystem.stopSystem();
    }

    @Test
    public void testAddAccountResource() {
        Account acHandler = new Account();
        ResourceDirectory rdHandler = new ResourceDirectory();
        ResourceFind resHandler = new ResourceFind();
        ResourcePresence adHandler = new ResourcePresence();
        MessageQueue mqHandler = new MessageQueue();
        deviceServerSystem.addResource(acHandler);
        deviceServerSystem.addResource(rdHandler);
        deviceServerSystem.addResource(resHandler);
        deviceServerSystem.addResource(adHandler);
        deviceServerSystem.addResource(mqHandler);
    }

    @Test
    public void testChannelRead() throws InterruptedException {
        ChannelHandlerContext ctx = mock(ChannelHandlerContext.class);

        // inject mocked coapDevice into the api
        Channel channel = mock(Channel.class);
        Attribute<Device> attribute = mock(Attribute.class);
        Mockito.doReturn(channel).when(ctx).channel();
        Mockito.doReturn(attribute).when(channel).attr(Mockito.any());
        Mockito.doReturn(mockDevice).when(attribute).get();

        IRequest request = MessageBuilder.createRequest(RequestMethod.GET, null,
                null);
        Mockito.doAnswer(new Answer<Object>() {
            @Override
            public Object answer(InvocationOnMock invocation) throws Throwable {

                Object[] args = invocation.getArguments();
                IRequest req = (IRequest) args[0];

                assertEquals(req, request);

                latch.countDown();
                return null;
            }

        }).when(ctx).fireChannelRead(Mockito.anyObject());
        coapLifecycleHandler.channelRead(ctx, request);
        assertTrue(latch.await(1L, SECONDS));

    }

    @Test
    public void coapAuthHandlerAccountChannelReadRequest()
            throws InterruptedException {
        System.out.println(
                "\t--------------coapAuthHandler Account ChannelReadRequest Test------------");
        ChannelHandlerContext ctx = mock(ChannelHandlerContext.class);
        Cbor<HashMap<Object, Object>> cbor = new Cbor<>();
        Channel channel = mock(Channel.class);
        Attribute<Device> attribute = mock(Attribute.class);
        Mockito.doReturn(channel).when(ctx).channel();
        Mockito.doReturn(attribute).when(channel).attr(Mockito.any());

        Mockito.doAnswer(new Answer<Object>() {
            @Override
            public CoapRequest answer(InvocationOnMock invocation)
                    throws Throwable {
                Object[] args = invocation.getArguments();
                CoapRequest req = (CoapRequest) args[0];
                assertEquals(req.getUriPath(), Constants.ACCOUNT_FULL_URI);
                assertTrue(cbor
                        .parsePayloadFromCbor(req.getPayload(), HashMap.class)
                        .containsKey("di"));
                latch.countDown();
                return null;
            }
        }).when(ctx).fireChannelRead(Mockito.any());

        HashMap<String, Object> payloadData = new HashMap<>();
        payloadData.put("di", "sampleDevice");

        IRequest request = MessageBuilder.createRequest(RequestMethod.POST,
                Constants.ACCOUNT_FULL_URI, null,
                ContentFormat.APPLICATION_CBOR,
                cbor.encodingPayloadToCbor(payloadData));
        coapAuthHandler.channelRead(ctx, request);
    }

    @Test
    public void coapAuthHandlerPingChannelReadRequest()
            throws InterruptedException {
        System.out.println(
                "\t--------------coapAuthHandler Ping ChannelReadRequest Test------------");
        ChannelHandlerContext ctx = mock(ChannelHandlerContext.class);
        Cbor<HashMap<Object, Object>> cbor = new Cbor<>();
        Channel channel = mock(Channel.class);
        Attribute<Device> attribute = mock(Attribute.class);
        Mockito.doReturn(channel).when(ctx).channel();
        Mockito.doReturn(attribute).when(channel).attr(Mockito.any());

        Mockito.doAnswer(new Answer<Object>() {
            @Override
            public CoapRequest answer(InvocationOnMock invocation)
                    throws Throwable {
                Object[] args = invocation.getArguments();
                CoapRequest req = (CoapRequest) args[0];
                assertEquals(req.getUriPath(), "/" + OICConstants.PREFIX_OIC
                        + "/" + OICConstants.KEEP_ALIVE_URI);

                latch.countDown();
                return null;
            }
        }).when(ctx).fireChannelRead(Mockito.any());

        IRequest request = MessageBuilder
                .createRequest(RequestMethod.POST,
                        "/" + OICConstants.PREFIX_OIC + "/"
                                + OICConstants.KEEP_ALIVE_URI,
                        null, null, null);
        coapAuthHandler.channelRead(ctx, request);
    }
}
