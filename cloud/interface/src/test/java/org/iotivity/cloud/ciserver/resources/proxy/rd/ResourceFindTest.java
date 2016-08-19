package org.iotivity.cloud.ciserver.resources.proxy.rd;

import static java.util.concurrent.TimeUnit.SECONDS;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;
import static org.mockito.Mockito.mock;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.concurrent.CountDownLatch;

import org.iotivity.cloud.base.OCFConstants;
import org.iotivity.cloud.base.connector.ConnectorPool;
import org.iotivity.cloud.base.device.CoapDevice;
import org.iotivity.cloud.base.device.IRequestChannel;
import org.iotivity.cloud.base.exception.ClientException;
import org.iotivity.cloud.base.protocols.IRequest;
import org.iotivity.cloud.base.protocols.IResponse;
import org.iotivity.cloud.base.protocols.MessageBuilder;
import org.iotivity.cloud.base.protocols.coap.CoapRequest;
import org.iotivity.cloud.base.protocols.coap.CoapResponse;
import org.iotivity.cloud.base.protocols.enums.ContentFormat;
import org.iotivity.cloud.base.protocols.enums.RequestMethod;
import org.iotivity.cloud.base.protocols.enums.ResponseStatus;
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

public class ResourceFindTest {
    private static final String TEST_RESOURCE_FIND_URI = "/"
            + OCFConstants.PREFIX_OIC + "/" + OCFConstants.WELL_KNOWN_URI;
    private String              di                     = "B371C481-38E6-4D47-8320-7688D8A5B58C";
    private CoapDevice          mockDevice             = mock(CoapDevice.class);
    IResponse                   res                    = null;
    IRequest                    req                    = null;
    ConnectorPool               connectorPool          = null;
    DeviceServerSystem          deviceServerSystem     = new DeviceServerSystem();
    final CountDownLatch        latch                  = new CountDownLatch(1);

    @Mock
    IRequestChannel             requestChannel;

    @InjectMocks
    ResourceFind                resHandler             = new ResourceFind();

    @Before
    public void setUp() throws Exception {
        res = null;
        req = null;
        Mockito.doReturn("mockDeviceId").when(mockDevice).getDeviceId();
        MockitoAnnotations.initMocks(this);
        deviceServerSystem.addResource(resHandler);
        // callback mock
        Mockito.doAnswer(new Answer<Object>() {
            @Override
            public CoapResponse answer(InvocationOnMock invocation)
                    throws Throwable {
                Object[] args = invocation.getArguments();
                CoapResponse resp = (CoapResponse) args[0];
                res = resp;
                return resp;
            }
        }).when(mockDevice).sendResponse(Mockito.anyObject());
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

    // @InjectMocks for testSpecificDeviceonResponseReceived
    IRequest                           requestSpecificDevice = MessageBuilder
            .createRequest(RequestMethod.GET, TEST_RESOURCE_FIND_URI,
                    "rt=core.light;di=" + "device1");
    @InjectMocks
    ResourceFind.AccountReceiveHandler specificDeviceHandler = resHandler.new AccountReceiveHandler(
            requestSpecificDevice, mockDevice);

    @Test
    public void testSpecificDeviceonResponseReceived()
            throws ClientException, InterruptedException {
        System.out.println(
                "\t--------------onResponseReceived(RD) Resource Find (specific deivce) Test------------");
        IResponse response = responseFromAccountServer();
        specificDeviceHandler.onResponseReceived(response);
        HashMap<String, List<String>> queryMap = req.getUriQueryMap();
        assertTrue(req.getMethod() == RequestMethod.GET);
        assertTrue(queryMap.get("rt").contains("core.light"));
        assertTrue(queryMap.get("di").contains("device1"));
        assertFalse(queryMap.get("di").contains("device2"));
        assertFalse(queryMap.get("di").contains("device3"));
    }

    // @InjectMocks for testEntireDeviceonResponseReceived
    IRequest                           requestEntireDevices = MessageBuilder
            .createRequest(RequestMethod.GET, TEST_RESOURCE_FIND_URI,
                    "rt=core.light");
    @InjectMocks
    ResourceFind.AccountReceiveHandler handler              = resHandler.new AccountReceiveHandler(
            requestEntireDevices, mockDevice);

    @Test
    public void testEntireDeviceonResponseReceived() throws ClientException {
        System.out.println(
                "\t--------------onResponseReceived(RD) Resource Find (entire deivces) Test------------");
        IResponse response = responseFromAccountServer();
        handler.onResponseReceived(response);
        HashMap<String, List<String>> queryMap = req.getUriQueryMap();
        assertTrue(req.getMethod() == RequestMethod.GET);
        assertTrue(queryMap.get("rt").contains("core.light"));
        assertTrue(queryMap.get("di").contains("device1"));
        assertTrue(queryMap.get("di").contains("device2"));
        assertTrue(queryMap.get("di").contains("device3"));
    }

    // @InjectMocks for testEntireDeviceonResponseReceived
    IRequest                           requestEntireDevicesNoQuery = MessageBuilder
            .createRequest(RequestMethod.GET, TEST_RESOURCE_FIND_URI, null);
    @InjectMocks
    ResourceFind.AccountReceiveHandler entireDevicesNoQueryHandler = resHandler.new AccountReceiveHandler(
            requestEntireDevicesNoQuery, mockDevice);

    @Test
    public void testEntireDeviceNoQueryonResponseReceived()
            throws ClientException {
        System.out.println(
                "\t--------------onResponseReceived(RD) Resource Find (entire deivces, No Query) Test------------");
        IResponse response = responseFromAccountServer();
        entireDevicesNoQueryHandler.onResponseReceived(response);
        HashMap<String, List<String>> queryMap = req.getUriQueryMap();
        assertTrue(req.getMethod() == RequestMethod.GET);
        assertTrue(queryMap.get("di").contains("device1"));
        assertTrue(queryMap.get("di").contains("device2"));
        assertTrue(queryMap.get("di").contains("device3"));
    }

    @Test
    public void testResourceFindEntireDevicesOnRequestReceived()
            throws Exception {
        System.out.println(
                "\t--------------OnRequestReceived(RD) Resource Find (entire deivces) Test------------");
        IRequest request = MessageBuilder.createRequest(RequestMethod.GET,
                TEST_RESOURCE_FIND_URI, "rt=core.light;di=" + di);
        resHandler.onRequestReceived(mockDevice, request);
        HashMap<String, List<String>> queryMap = req.getUriQueryMap();
        assertTrue(latch.await(1L, SECONDS));
        assertTrue(queryMap.containsKey("mid"));
        assertEquals(req.getUriPath(), "/.well-known/ocf/acl/group/null");
    }

    @Test
    public void testResourceFindSpecificDeviceOnRequestReceived()
            throws Exception {
        System.out.println(
                "\t--------------OnRequestReceived(RD) Resource Find (specific deivce) Test------------");
        IRequest request = MessageBuilder.createRequest(RequestMethod.GET,
                TEST_RESOURCE_FIND_URI, "rt=core.light;di=" + di);
        resHandler.onRequestReceived(mockDevice, request);
        HashMap<String, List<String>> queryMap = req.getUriQueryMap();
        // assertion: if the request packet from the CI contains the query
        // which includes device ID and the accesstoken
        assertTrue(latch.await(1L, SECONDS));
        assertTrue(queryMap.containsKey("mid"));
        assertEquals(req.getUriPath(), "/.well-known/ocf/acl/group/null");
    }

    private IResponse responseFromAccountServer() {
        // make response which has "CONTENT" status
        Cbor<HashMap<String, Object>> cbor = new Cbor<>();
        HashMap<String, Object> responsePayload = new HashMap<String, Object>();
        ArrayList<String> deviceList = new ArrayList<String>();
        // assuming that there are three devices in the response msg from the AS
        deviceList.add("device1");
        deviceList.add("device2");
        deviceList.add("device3");
        responsePayload.put("dilist", deviceList);
        responsePayload.put("gid", "g0001");
        responsePayload.put("gmid", "u0001");
        ArrayList<String> midList = new ArrayList<String>();
        midList.add("u0001");
        responsePayload.put("midlist", midList);
        IResponse response = MessageBuilder.createResponse(requestEntireDevices,
                ResponseStatus.CONTENT, ContentFormat.APPLICATION_CBOR,
                cbor.encodingPayloadToCbor(responsePayload));
        return response;
    }
}
