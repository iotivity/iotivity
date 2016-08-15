package org.iotivity.cloud.ciserver.resources.proxy.account;

import static java.util.concurrent.TimeUnit.SECONDS;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;
import static org.mockito.Mockito.mock;

import java.util.Arrays;
import java.util.HashMap;
import java.util.concurrent.CountDownLatch;

import org.iotivity.cloud.base.OCFConstants;
import org.iotivity.cloud.base.connector.ConnectorPool;
import org.iotivity.cloud.base.device.CoapDevice;
import org.iotivity.cloud.base.device.IRequestChannel;
import org.iotivity.cloud.base.protocols.IRequest;
import org.iotivity.cloud.base.protocols.MessageBuilder;
import org.iotivity.cloud.base.protocols.coap.CoapRequest;
import org.iotivity.cloud.base.protocols.enums.ContentFormat;
import org.iotivity.cloud.base.protocols.enums.RequestMethod;
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

public class AclGroupTest {
    private static final String TEST_RESOURCE_GROUP_URI = "/"
            + OCFConstants.PREFIX_WELL_KNOWN + "/" + OCFConstants.PREFIX_OCF
            + "/" + OCFConstants.ACL_URI + "/" + OCFConstants.GROUP_URI;
    private CoapDevice          mockDevice              = mock(
            CoapDevice.class);
    IRequest                    req                     = null;
    ConnectorPool               connectorPool           = null;
    DeviceServerSystem          deviceServerSystem      = new DeviceServerSystem();
    final CountDownLatch        latch                   = new CountDownLatch(1);

    @Mock
    IRequestChannel             requestChannel;

    @InjectMocks
    AclGroup                    aclGroupHandler         = new AclGroup();

    @Before
    public void setUp() throws Exception {
        req = null;
        Mockito.doReturn("mockUserId").when(mockDevice).getUserId();
        MockitoAnnotations.initMocks(this);
        deviceServerSystem.addResource(aclGroupHandler);
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
    public void testCreateGroupRequestReceived() throws Exception {
        System.out.println(
                "\t--------------OnRequestReceived(AS) Create Group Test------------");

        HashMap<String, Object> payloadData = new HashMap<>();
        payloadData.put("gtype", "public");
        Cbor<HashMap<Object, Object>> cbor = new Cbor<>();
        IRequest request = MessageBuilder.createRequest(RequestMethod.POST,
                TEST_RESOURCE_GROUP_URI, null, ContentFormat.APPLICATION_CBOR,
                cbor.encodingPayloadToCbor(payloadData));
        aclGroupHandler.onRequestReceived(mockDevice, request);

        assertTrue(latch.await(1L, SECONDS));
        assertTrue(cbor.parsePayloadFromCbor(req.getPayload(), HashMap.class)
                .containsKey("gtype"));
        assertTrue(cbor.parsePayloadFromCbor(req.getPayload(), HashMap.class)
                .containsKey("gmid"));
        assertEquals(req.getUriPath(), TEST_RESOURCE_GROUP_URI);
    }

    @Test
    public void testAddMemberRequestReceived() throws Exception {
        System.out.println(
                "\t--------------OnRequestReceived(AS) Add Member Test------------");

        HashMap<String, Object> payloadData = new HashMap<>();
        Cbor<HashMap<Object, Object>> cbor = new Cbor<>();
        IRequest request = MessageBuilder.createRequest(RequestMethod.POST,
                TEST_RESOURCE_GROUP_URI + "/" + "sampleGroup", null,
                ContentFormat.APPLICATION_CBOR,
                cbor.encodingPayloadToCbor(payloadData));
        aclGroupHandler.onRequestReceived(mockDevice, request);

        assertTrue(latch.await(1L, SECONDS));
        assertTrue(cbor.parsePayloadFromCbor(req.getPayload(), HashMap.class)
                .containsKey("midlist"));
        assertEquals(req.getUriPath(),
                TEST_RESOURCE_GROUP_URI + "/" + "sampleGroup");
    }

    @Test
    public void testAddDeviceRequestReceived() throws Exception {
        System.out.println(
                "\t--------------OnRequestReceived(AS) Add Device Test------------");

        HashMap<String, Object> payloadData = new HashMap<>();
        payloadData.put("dilist", Arrays.asList("sampleDevice"));
        Cbor<HashMap<Object, Object>> cbor = new Cbor<>();
        IRequest request = MessageBuilder.createRequest(RequestMethod.POST,
                TEST_RESOURCE_GROUP_URI + "/" + "sampleGroup", null,
                ContentFormat.APPLICATION_CBOR,
                cbor.encodingPayloadToCbor(payloadData));
        aclGroupHandler.onRequestReceived(mockDevice, request);

        assertTrue(latch.await(1L, SECONDS));
        assertTrue(cbor.parsePayloadFromCbor(req.getPayload(), HashMap.class)
                .containsKey("dilist"));
        assertEquals(req.getUriPath(),
                TEST_RESOURCE_GROUP_URI + "/" + "sampleGroup");
    }

    @Test
    public void testGetGroupListAndGetGroupInfoRequestReceived()
            throws Exception {
        System.out.println(
                "\t--------------OnRequestReceived(AS) Get Group List & Get Group Info Test------------");

        IRequest request = MessageBuilder.createRequest(RequestMethod.GET,
                TEST_RESOURCE_GROUP_URI, null, null, null);
        aclGroupHandler.onRequestReceived(mockDevice, request);

        assertTrue(latch.await(1L, SECONDS));
        assertTrue(req.getUriQueryMap().containsKey("mid"));
        assertEquals(req.getUriPath(), TEST_RESOURCE_GROUP_URI);
    }

    @Test
    public void testDeleteGroupRequestReceived() throws Exception {
        System.out.println(
                "\t--------------OnRequestReceived(AS) Delete Group Test------------");

        IRequest request = MessageBuilder.createRequest(RequestMethod.DELETE,
                TEST_RESOURCE_GROUP_URI, "gid=samplegid", null, null);
        aclGroupHandler.onRequestReceived(mockDevice, request);

        assertTrue(latch.await(1L, SECONDS));
        assertTrue(req.getUriQueryMap().containsKey("gid"));
        assertTrue(req.getUriQueryMap().containsKey("gmid"));
        assertEquals(req.getUriPath(), TEST_RESOURCE_GROUP_URI);
    }

    @Test
    public void testDeleteMemberRequestReceived() throws Exception {
        System.out.println(
                "\t--------------OnRequestReceived(AS) Delete Member Test------------");

        IRequest request = MessageBuilder.createRequest(RequestMethod.DELETE,
                TEST_RESOURCE_GROUP_URI + "/" + "sampleGroup", null, null,
                null);
        aclGroupHandler.onRequestReceived(mockDevice, request);

        assertTrue(latch.await(1L, SECONDS));
        assertTrue(req.getUriQueryMap().containsKey("midlist"));
        assertEquals(req.getUriPath(),
                TEST_RESOURCE_GROUP_URI + "/" + "sampleGroup");
    }

    @Test
    public void testDeleteDeviceRequestReceived() throws Exception {
        System.out.println(
                "\t--------------OnRequestReceived(AS) Delete Member Test------------");

        IRequest request = MessageBuilder.createRequest(RequestMethod.DELETE,
                TEST_RESOURCE_GROUP_URI + "/" + "sampleGroup",
                "dilist=sampledi", null, null);
        aclGroupHandler.onRequestReceived(mockDevice, request);

        assertTrue(latch.await(1L, SECONDS));
        assertTrue(req.getUriQueryMap().containsKey("dilist"));
        assertEquals(req.getUriPath(),
                TEST_RESOURCE_GROUP_URI + "/" + "sampleGroup");
    }
}
