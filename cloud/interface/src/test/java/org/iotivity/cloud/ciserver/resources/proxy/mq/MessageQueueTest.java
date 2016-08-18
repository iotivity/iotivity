package org.iotivity.cloud.ciserver.resources.proxy.mq;

import static java.util.concurrent.TimeUnit.SECONDS;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;
import static org.mockito.Mockito.mock;

import java.util.HashMap;
import java.util.concurrent.CountDownLatch;

import org.iotivity.cloud.base.device.CoapDevice;
import org.iotivity.cloud.base.device.IRequestChannel;
import org.iotivity.cloud.base.protocols.IRequest;
import org.iotivity.cloud.base.protocols.IResponse;
import org.iotivity.cloud.base.protocols.MessageBuilder;
import org.iotivity.cloud.base.protocols.coap.CoapRequest;
import org.iotivity.cloud.base.protocols.enums.ContentFormat;
import org.iotivity.cloud.base.protocols.enums.RequestMethod;
import org.iotivity.cloud.ciserver.Constants;
import org.iotivity.cloud.ciserver.DeviceServerSystem;
import org.iotivity.cloud.util.Cbor;
import org.junit.Before;
import org.junit.Test;
import org.mockito.InjectMocks;
import org.mockito.Mock;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.mockito.invocation.InvocationOnMock;
import org.mockito.stubbing.Answer;

public class MessageQueueTest {
    private static final String TEST_MQ_BROKER_URI = Constants.MQ_BROKER_FULL_URI;
    String                      userId             = "testuser";
    private CoapDevice          mockDevice         = null;
    IResponse                   res                = null;
    IRequest                    req                = null;
    DeviceServerSystem          deviceServerSystem;
    final CountDownLatch        latch              = new CountDownLatch(1);
    @Mock
    IRequestChannel             requestChannel;
    @InjectMocks
    MessageQueue                mqHandler          = new MessageQueue();

    @Before
    public void setUp() throws Exception {
        res = null;
        req = null;
        MockitoAnnotations.initMocks(this);
        deviceServerSystem = new DeviceServerSystem();
        deviceServerSystem.addResource(mqHandler);
        mockDevice = mock(CoapDevice.class);
        // callback requestmsg mock from (IRequestChannel) server
        Mockito.doAnswer(new Answer<Object>() {
            @Override
            public CoapRequest answer(InvocationOnMock invocation)
                    throws Throwable {
                Object[] args = invocation.getArguments();
                CoapRequest request = (CoapRequest) args[0];
                req = request;
                latch.countDown();
                return null;
            }
        }).when(requestChannel).sendRequest(Mockito.any(IRequest.class),
                Mockito.any(CoapDevice.class));
    }

    @Test
    public void testMessageQueueResourceOnRequestReceived() throws Exception {
        System.out.println(
                "\t--------------OnRequestReceived(MQ) Topic Creation Test------------");
        IRequest request = null;
        HashMap<Object, Object> tags = new HashMap<Object, Object>();
        tags.put("topic", "DeviceServerSystemtestTopic");
        Cbor<HashMap<Object, Object>> cbor = new Cbor<HashMap<Object, Object>>();
        request = MessageBuilder.createRequest(RequestMethod.PUT,
                TEST_MQ_BROKER_URI, null, ContentFormat.APPLICATION_CBOR,
                cbor.encodingPayloadToCbor(tags));
        deviceServerSystem.onRequestReceived(mockDevice, request);
        // assertion : request msg to the AS is identical to the request msg
        // from the client
        assertTrue(latch.await(1L, SECONDS));
        assertEquals(request, req);
    }

}
