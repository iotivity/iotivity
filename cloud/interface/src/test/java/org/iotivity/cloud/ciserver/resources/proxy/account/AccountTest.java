package org.iotivity.cloud.ciserver.resources.proxy.account;

import static java.util.concurrent.TimeUnit.SECONDS;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;
import static org.mockito.Mockito.mock;

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

public class AccountTest {
    private String             di                 = "B371C481-38E6-4D47-8320-7688D8A5B58C";
    public static final String ACCOUNT_URI        = Constants.ACCOUNT_FULL_URI;
    private String             authProvider       = "github";
    private String             authCode           = "f6b55d8d8a27e0d32ab8";
    String                     userId             = "testuser";
    private CoapDevice         mockDevice         = mock(CoapDevice.class);
    IResponse                  res                = null;
    IRequest                   req                = null;
    ConnectorPool              connectorPool      = null;
    DeviceServerSystem         deviceServerSystem = new DeviceServerSystem();
    final CountDownLatch       latch              = new CountDownLatch(1);
    @Mock
    IRequestChannel            requestChannel;

    @InjectMocks
    Account                    acHandler          = new Account();

    @Before
    public void setUp() throws Exception {
        res = null;
        req = null;
        Mockito.doReturn("mockDeviceUser").when(mockDevice).getUserId();
        MockitoAnnotations.initMocks(this);
        deviceServerSystem.addResource(acHandler);
        Mockito.doAnswer(new Answer<Object>() {
            @Override
            public CoapRequest answer(InvocationOnMock invocation)
                    throws Throwable {
                Object[] args = invocation.getArguments();
                CoapRequest request = (CoapRequest) args[0];
                req = request;
                latch.countDown();
                System.out.println(
                        "\t----------payload : " + request.getPayloadString());
                System.out.println(
                        "\t----------uripath : " + request.getUriPath());
                System.out.println(
                        "\t---------uriquery : " + request.getUriQuery());
                return null;
            }
        }).when(requestChannel).sendRequest(Mockito.any(IRequest.class),
                Mockito.any(CoapDevice.class));
    }

    @Test
    public void testAccountDeviceDeleteOnRequestReceived() throws Exception {
        System.out.println(
                "\t--------------OnRequestReceived Device Delete Test------------");
        // sign up request from the client
        IRequest request = null;
        request = MessageBuilder.createRequest(RequestMethod.DELETE,
                ACCOUNT_URI, "di=device1");
        deviceServerSystem.onRequestReceived(mockDevice, request);
        assertTrue(latch.await(1L, SECONDS));
        assertTrue(req.getUriQueryMap().get("uid").contains("mockDeviceUser"));
        assertTrue(req.getUriQueryMap().get("di").contains("device1"));
        assertTrue(req.getUriPath().equals(ACCOUNT_URI));
    }

    @Test
    public void testAccountResourceOnRequestReceived() throws Exception {
        System.out.println(
                "\t--------------OnRequestReceived Sign Up Test------------");
        // sign up request from the client
        IRequest request = null;
        HashMap<String, String> payloadData = new HashMap<String, String>();
        payloadData.put("authcode", authCode);
        payloadData.put("authprovider", authProvider);
        payloadData.put("di", di);
        Cbor<HashMap<String, String>> cbor = new Cbor<HashMap<String, String>>();
        request = MessageBuilder.createRequest(RequestMethod.POST, ACCOUNT_URI,
                null, ContentFormat.APPLICATION_CBOR,
                cbor.encodingPayloadToCbor(payloadData));
        deviceServerSystem.onRequestReceived(mockDevice, request);
        // assertion : request msg to the AS is identical to the request msg
        // from the client
        assertTrue(latch.await(1L, SECONDS));
        assertEquals(request, req);
    }
}
