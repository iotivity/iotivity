package org.iotivity.cloud.ciserver.resources.proxy.account;

import static java.util.concurrent.TimeUnit.SECONDS;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;
import static org.mockito.Mockito.mock;

import java.util.concurrent.CountDownLatch;

import org.iotivity.cloud.base.OCFConstants;
import org.iotivity.cloud.base.connector.ConnectorPool;
import org.iotivity.cloud.base.device.CoapDevice;
import org.iotivity.cloud.base.device.IRequestChannel;
import org.iotivity.cloud.base.protocols.IRequest;
import org.iotivity.cloud.base.protocols.MessageBuilder;
import org.iotivity.cloud.base.protocols.coap.CoapRequest;
import org.iotivity.cloud.base.protocols.enums.RequestMethod;
import org.iotivity.cloud.ciserver.DeviceServerSystem;
import org.junit.Before;
import org.junit.Test;
import org.mockito.InjectMocks;
import org.mockito.Mock;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.mockito.invocation.InvocationOnMock;
import org.mockito.stubbing.Answer;

public class AclTest {
    private static final String TEST_RESOURCE_ACI_URI = "/"
            + OCFConstants.PREFIX_WELL_KNOWN + "/" + OCFConstants.PREFIX_OCF
            + "/" + OCFConstants.ACL_URI;
    private CoapDevice          mockDevice            = mock(CoapDevice.class);
    IRequest                    req                   = null;
    ConnectorPool               connectorPool         = null;
    DeviceServerSystem          deviceServerSystem    = new DeviceServerSystem();
    final CountDownLatch        latch                 = new CountDownLatch(1);

    @Mock
    IRequestChannel             requestChannel;

    @InjectMocks
    Acl                         aclHandler            = new Acl();

    @Before
    public void setUp() throws Exception {
        req = null;
        MockitoAnnotations.initMocks(this);
        deviceServerSystem.addResource(aclHandler);
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
    public void testOnDefaultRequestReceived() throws Exception {
        System.out.println(
                "\t--------------OnRequestReceived(AS)Test------------");

        IRequest request = MessageBuilder.createRequest(RequestMethod.POST,
                TEST_RESOURCE_ACI_URI, null, null, null);
        aclHandler.onRequestReceived(mockDevice, request);

        assertTrue(latch.await(1L, SECONDS));
        assertEquals(req.getUriPath(), TEST_RESOURCE_ACI_URI);
    }
}
