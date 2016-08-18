package org.iotivity.cloud.ciserver.resources.proxy.account;

import static java.util.concurrent.TimeUnit.SECONDS;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;
import static org.mockito.Mockito.mock;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.concurrent.CountDownLatch;

import org.iotivity.cloud.base.connector.ConnectorPool;
import org.iotivity.cloud.base.device.CoapDevice;
import org.iotivity.cloud.base.device.IRequestChannel;
import org.iotivity.cloud.base.protocols.IRequest;
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

public class AclInviteTest {
    private static final String TEST_RESOURCE_INVITE_URI = Constants.INVITE_FULL_URI;

    private CoapDevice          mockDevice               = mock(
            CoapDevice.class);
    IRequest                    req                      = null;
    ConnectorPool               connectorPool            = null;
    DeviceServerSystem          deviceServerSystem       = new DeviceServerSystem();
    final CountDownLatch        latch                    = new CountDownLatch(
            1);

    @Mock
    IRequestChannel             requestChannel;

    @InjectMocks
    AclInvite                   aclInviteHandler         = new AclInvite();

    @Before
    public void setUp() throws Exception {
        req = null;
        Mockito.doReturn("mockUserId").when(mockDevice).getUserId();
        MockitoAnnotations.initMocks(this);
        deviceServerSystem.addResource(aclInviteHandler);
        // callback mock
        Mockito.doAnswer(new Answer<Object>() {
            @Override
            public CoapRequest answer(InvocationOnMock invocation)
                    throws Throwable {
                Object[] args = invocation.getArguments();
                CoapRequest request = (CoapRequest) args[0];
                System.out.println(
                        "\t----------payload : " + request.getPayloadString());
                System.out.println(
                        "\t----------uripath : " + request.getUriPath());
                System.out.println(
                        "\t---------uriquery : " + request.getUriQuery());
                req = request;
                latch.countDown();
                return null;
            }
        }).when(requestChannel).sendRequest(Mockito.any(IRequest.class),
                Mockito.any(CoapDevice.class));
    }

    @Test
    public void testPostRequestReceived() throws Exception {
        System.out.println(
                "\t--------------OnRequestReceived(AS) Post Message Test------------");

        HashMap<String, Object> payloadData = new HashMap<>();
        HashMap<String, Object> invite = new HashMap<>();
        invite.put("gid", "g0001");
        invite.put("mid", "u0001");
        payloadData.put("invite", Arrays.asList(invite));
        Cbor<HashMap<Object, Object>> cbor = new Cbor<>();
        IRequest request = MessageBuilder.createRequest(RequestMethod.POST,
                TEST_RESOURCE_INVITE_URI, null, ContentFormat.APPLICATION_CBOR,
                cbor.encodingPayloadToCbor(payloadData));
        aclInviteHandler.onRequestReceived(mockDevice, request);

        assertTrue(latch.await(1L, SECONDS));
        assertTrue(cbor.parsePayloadFromCbor(req.getPayload(), HashMap.class)
                .containsKey("uid"));
        assertTrue(cbor.parsePayloadFromCbor(req.getPayload(), HashMap.class)
                .containsKey("invite"));
        ArrayList<HashMap<String, Object>> getinvite = (ArrayList<HashMap<String, Object>>) cbor
                .parsePayloadFromCbor(req.getPayload(), HashMap.class)
                .get("invite");
        assertTrue(getinvite.get(0).containsKey("gid"));
        assertTrue(getinvite.get(0).containsKey("mid"));
        assertEquals(req.getUriPath(), TEST_RESOURCE_INVITE_URI);
    }

    @Test
    public void testGetRequestReceived() throws Exception {
        System.out.println(
                "\t--------------OnRequestReceived(AS) Get Message Test------------");

        IRequest request = MessageBuilder.createRequest(RequestMethod.GET,
                TEST_RESOURCE_INVITE_URI, null, null, null);
        aclInviteHandler.onRequestReceived(mockDevice, request);

        assertTrue(latch.await(1L, SECONDS));
        assertTrue(req.getUriQueryMap().containsKey("uid"));
        assertEquals(req.getUriPath(), TEST_RESOURCE_INVITE_URI);
    }

    @Test
    public void testDeleteRequestReceived() throws Exception {
        System.out.println(
                "\t--------------OnRequestReceived(AS) Delete Message Test------------");

        IRequest request = MessageBuilder.createRequest(RequestMethod.DELETE,
                TEST_RESOURCE_INVITE_URI, "gid=g0001", null, null);
        aclInviteHandler.onRequestReceived(mockDevice, request);

        assertTrue(latch.await(1L, SECONDS));
        assertTrue(req.getUriQueryMap().containsKey("gid"));
        assertTrue(req.getUriQueryMap().containsKey("uid"));
        assertEquals(req.getUriPath(), TEST_RESOURCE_INVITE_URI);
    }
}
