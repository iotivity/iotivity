package org.iotivity.cloud.ciserver.resources.proxy.rd;

import static java.util.concurrent.TimeUnit.SECONDS;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;
import static org.mockito.Mockito.mock;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.concurrent.CountDownLatch;

import org.iotivity.cloud.base.connector.ConnectorPool;
import org.iotivity.cloud.base.device.CoapDevice;
import org.iotivity.cloud.base.device.IRequestChannel;
import org.iotivity.cloud.base.protocols.IRequest;
import org.iotivity.cloud.base.protocols.IResponse;
import org.iotivity.cloud.base.protocols.MessageBuilder;
import org.iotivity.cloud.base.protocols.coap.CoapRequest;
import org.iotivity.cloud.base.protocols.enums.ContentFormat;
import org.iotivity.cloud.base.protocols.enums.RequestMethod;
import org.iotivity.cloud.base.protocols.enums.ResponseStatus;
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

public class ResourceDirectoryTest {
    private static final String TEST_RD_URI        = Constants.RD_FULL_URI;
    public static final String  DEVICE_PRS_REQ_URI = Constants.DEVICE_PRESENCE_FULL_URI;
    public static final String  DEVICE_LIST_KEY    = "devices";
    public static final String  RES_PRS_URI        = Constants.RESOURCE_PRESENCE_FULL_URI;
    // private DeviceServerSystem deviceServerSystem = new DeviceServerSystem();
    private String              di                 = "B371C481-38E6-4D47-8320-7688D8A5B58C";
    String                      userId             = "testuser";
    private CoapDevice          mockDevice         = null;
    IResponse                   res                = null;
    IRequest                    req                = null;
    ConnectorPool               connectorPool      = null;
    DeviceServerSystem          deviceServerSystem = new DeviceServerSystem();
    final CountDownLatch        latch              = new CountDownLatch(1);
    @Mock
    IRequestChannel             requestChannel;

    @InjectMocks
    ResourceDirectory           rdHandler          = new ResourceDirectory();

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.initMocks(this);
        deviceServerSystem.addResource(rdHandler);
        mockDevice = mock(CoapDevice.class);
        // callback requestmsg mock from (IRequestChannel) server
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
                return request;
            }
        }).when(requestChannel).sendRequest(Mockito.any(IRequest.class),
                Mockito.any(CoapDevice.class));

    }

    @Test
    public void testRDResourceOnRequestReceived() throws Exception {
        IRequest request = makeResourcePublishRequest();
        deviceServerSystem.onRequestReceived(mockDevice, request);
        assertTrue(latch.await(1L, SECONDS));
        assertTrue(req.getUriPath().contains(Constants.GROUP_FULL_URI + "/"));
    }

    IRequest                                rdPublishRequest      = makeResourcePublishRequest();

    @InjectMocks
    ResourceDirectory.AccountReceiveHandler accountReceiveHandler = rdHandler.new AccountReceiveHandler(
            rdPublishRequest, mockDevice);

    @Test
    public void testRDResourceOnResponseReceived() throws Exception {
        IRequest request = makeResourcePublishRequest();
        deviceServerSystem.onRequestReceived(mockDevice, request);
        // assertion : request msg to the AS is identical to the request msg
        // from the client
        accountReceiveHandler.onResponseReceived(
                MessageBuilder.createResponse(request, ResponseStatus.CHANGED));
        assertEquals(req, rdPublishRequest);
        assertTrue(latch.await(1L, SECONDS));
    }

    private IRequest makeResourcePublishRequest() {
        HashMap<Object, Object> payload = new HashMap<>();
        payload.put(Constants.DEVICE_ID, di);
        ArrayList<HashMap<Object, Object>> publishLinks = new ArrayList<>();
        HashMap<Object, Object> link = new HashMap<>();
        link.put("href", "/a/light");
        ArrayList<String> rt = new ArrayList<String>();
        rt.add("core.light");
        ArrayList<String> itf = new ArrayList<String>();
        itf.add("oic.if.baseline");
        HashMap<String, Object> policy = new HashMap<>();
        policy.put("bm", 5);
        link.put("rt", rt);
        link.put("if", itf);
        link.put("p", policy);
        publishLinks.add(link);
        payload.put("links", publishLinks);
        Cbor<HashMap<Object, Object>> cbor = new Cbor<>();
        IRequest request = MessageBuilder.createRequest(RequestMethod.POST,
                TEST_RD_URI, "rt" + "=" + "oic.wk.rdpub",
                ContentFormat.APPLICATION_CBOR,
                cbor.encodingPayloadToCbor(payload));
        return request;
    }

}
