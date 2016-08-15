package org.iotivity.cloud.ciserver.resources;

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
import org.iotivity.cloud.base.protocols.coap.CoapResponse;
import org.iotivity.cloud.base.protocols.enums.ContentFormat;
import org.iotivity.cloud.base.protocols.enums.RequestMethod;
import org.iotivity.cloud.base.protocols.enums.ResponseStatus;
import org.iotivity.cloud.ciserver.DeviceServerSystem;
import org.iotivity.cloud.ciserver.DeviceServerSystem.CoapDevicePool;
import org.iotivity.cloud.ciserver.resources.DiResource.DefaultResponseHandler;
import org.iotivity.cloud.ciserver.resources.DiResource.LinkInterfaceHandler;
import org.iotivity.cloud.util.Cbor;
import org.junit.Before;
import org.junit.Test;
import org.mockito.InjectMocks;
import org.mockito.Mock;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.mockito.invocation.InvocationOnMock;
import org.mockito.stubbing.Answer;

public class DiResourceTest {
    private static final String RELAY_URI              = "/di";
    private static final String RESOURCE_URI           = "/a/light/0";
    private String              diServer               = "resourceServerId";
    private CoapDevice          sourceDevice           = mock(CoapDevice.class);
    private CoapDevice          targetDevice           = mock(CoapDevice.class);
    IResponse                   res                    = null;
    IRequest                    req                    = null;
    DeviceServerSystem          deviceServerSystem     = new DeviceServerSystem();
    final CountDownLatch        latch                  = new CountDownLatch(1);
    IRequestChannel             targetChannel          = mock(
            IRequestChannel.class);

    @Mock
    CoapDevicePool              coapDevicePool;

    @Mock
    IRequestChannel             requestChannel;

    @InjectMocks
    DiResource                  diHandler              = new DiResource(
            coapDevicePool);

    @InjectMocks
    LinkInterfaceHandler        linkInterfaceHandler   = diHandler.new LinkInterfaceHandler(
            "targetDeviceId", sourceDevice);

    @InjectMocks
    DefaultResponseHandler      defaultResponseHandler = diHandler.new DefaultResponseHandler(
            "targetDeviceId", sourceDevice);

    @Before
    public void setUp() throws Exception {
        res = null;
        req = null;
        Mockito.doReturn("sourceDeviceId").when(sourceDevice).getDeviceId();
        Mockito.doReturn("targetDeviceId").when(targetDevice).getDeviceId();
        MockitoAnnotations.initMocks(this);
        deviceServerSystem.addResource(diHandler);
        Mockito.doReturn(targetDevice).when(coapDevicePool)
                .queryDevice(Mockito.anyString());
        Mockito.doReturn(targetChannel).when(targetDevice).getRequestChannel();

        // callback mock
        Mockito.doAnswer(new Answer<Object>() {
            @Override
            public CoapResponse answer(InvocationOnMock invocation)
                    throws Throwable {
                Object[] args = invocation.getArguments();
                CoapResponse resp = (CoapResponse) args[0];
                res = resp;
                latch.countDown();
                return null;
            }
        }).when(sourceDevice).sendResponse(Mockito.anyObject());
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
        }).when(targetChannel).sendRequest(Mockito.any(IRequest.class),
                Mockito.any(CoapDevice.class));
    }

    @Test
    public void testOnDefaultRequestReceived() throws InterruptedException {
        IRequest request = makePutRequest();
        diHandler.onDefaultRequestReceived(sourceDevice, request);
        assertTrue(req.getMethod().equals(RequestMethod.PUT));
        assertTrue(req.getUriPath().equals(RESOURCE_URI));
        assertTrue(latch.await(1L, SECONDS));
    }

    // TODO treat oic.if.ll resource response (on LinkInterfaceHandler)

    @Test
    public void testOnDefaultResponseHandleronResponseReceived()
            throws InterruptedException {
        IResponse response = makeContentResponse();
        defaultResponseHandler.onResponseReceived(response);
        assertEquals(res, response);
        assertTrue(latch.await(1L, SECONDS));
    }

    private IRequest makePutRequest() {
        Cbor<HashMap<String, Object>> cbor = new Cbor<>();
        HashMap<String, Object> payloadData = new HashMap<>();
        payloadData.put("state", true);
        payloadData.put("power", 6);
        IRequest request = MessageBuilder.createRequest(RequestMethod.PUT,
                RELAY_URI + "/" + diServer + RESOURCE_URI, null,
                ContentFormat.APPLICATION_CBOR,
                cbor.encodingPayloadToCbor(payloadData));
        return request;
    }

    private IResponse makeContentResponse() {
        Cbor<HashMap<String, Object>> cbor = new Cbor<>();
        HashMap<String, Object> payloadData = new HashMap<>();
        payloadData.put("state", true);
        payloadData.put("power", 6);
        IResponse response = MessageBuilder.createResponse(makeGetRequest(),
                ResponseStatus.CONTENT, ContentFormat.APPLICATION_CBOR,
                cbor.encodingPayloadToCbor(payloadData));
        return response;
    }

    private IRequest makeGetRequest() {
        IRequest request = MessageBuilder.createRequest(RequestMethod.PUT,
                RELAY_URI + "/" + diServer + RESOURCE_URI, null);
        return request;
    }

}
