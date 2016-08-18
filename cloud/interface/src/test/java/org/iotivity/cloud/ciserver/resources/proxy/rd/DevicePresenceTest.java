package org.iotivity.cloud.ciserver.resources.proxy.rd;

import static java.util.concurrent.TimeUnit.SECONDS;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;
import static org.mockito.Mockito.mock;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.concurrent.CountDownLatch;

import org.iotivity.cloud.base.connector.ConnectorPool;
import org.iotivity.cloud.base.device.CoapDevice;
import org.iotivity.cloud.base.device.IRequestChannel;
import org.iotivity.cloud.base.exception.ClientException;
import org.iotivity.cloud.base.protocols.IRequest;
import org.iotivity.cloud.base.protocols.IResponse;
import org.iotivity.cloud.base.protocols.MessageBuilder;
import org.iotivity.cloud.base.protocols.coap.CoapRequest;
import org.iotivity.cloud.base.protocols.enums.ContentFormat;
import org.iotivity.cloud.base.protocols.enums.Observe;
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

public class DevicePresenceTest {
    public static final String DEVICE_PRS_REQ_URI = Constants.DEVICE_PRESENCE_FULL_URI;
    public static final String DEVICE_LIST_KEY    = "devices";
    public static final String RES_PRS_URI        = Constants.RESOURCE_PRESENCE_FULL_URI;
    private String             di                 = "B371C481-38E6-4D47-8320-7688D8A5B58C";
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
    DevicePresence             prsHandler         = new DevicePresence();

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.initMocks(this);
        deviceServerSystem.addResource(prsHandler);
        Mockito.doReturn("mockDeviceId").when(mockDevice).getDeviceId();
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

    // @InjectMocks for testSpecificDeviceonResponseReceived
    IRequest                             requestSpecificDevice = makePresenceSpecificDevice(
            "device1");
    @InjectMocks
    DevicePresence.AccountReceiveHandler SpecificDeviceHandler = prsHandler.new AccountReceiveHandler(
            requestSpecificDevice, mockDevice);

    @Test
    public void testSpecificDeviceonResponseReceived() throws ClientException {
        System.out.println(
                "\t--------------onResponseReceived(RD) Device Presence (specific deivce) Test------------");
        IResponse response = responseFromAccountServer();
        SpecificDeviceHandler.onResponseReceived(response);
        HashMap<String, List<String>> queryMap = req.getUriQueryMap();
        assertTrue(req.getObserve() == Observe.SUBSCRIBE);
        assertTrue(queryMap.get("di").contains("device1"));
        assertTrue(req.getMethod() == RequestMethod.GET);
    }

    // @InjectMocks for testEntireDeviceonResponseReceived
    IRequest                             requestEntireDevices = makePresenceEntireDevice();
    @InjectMocks
    DevicePresence.AccountReceiveHandler EntireDeviceHandler  = prsHandler.new AccountReceiveHandler(
            requestEntireDevices, mockDevice);

    @Test
    public void testEntireDeviceonResponseReceived() throws ClientException {
        System.out.println(
                "\t--------------onResponseReceived(RD) Device Presence (entire deivces) Test------------");
        IResponse response = responseFromAccountServer();
        EntireDeviceHandler.onResponseReceived(response);
        HashMap<String, List<String>> queryMap = req.getUriQueryMap();
        assertTrue(req.getMethod() == RequestMethod.GET);
        assertTrue(queryMap.get("di").contains("device1"));
        assertTrue(queryMap.get("di").contains("device2"));
        assertTrue(queryMap.get("di").contains("device3"));
        assertTrue(req.getObserve() == Observe.SUBSCRIBE);
    }

    @Test
    public void testDevicePresenceObserverSpecificDeviceOnRequestReceived()
            throws Exception {
        System.out.println(
                "\t--------------OnRequestReceived(RD) Device Presence (observe specific device) Test------------");
        IRequest request = makePresenceSpecificDevice(di);
        deviceServerSystem.onRequestReceived(mockDevice, request);
        assertTrue(latch.await(1L, SECONDS));
        HashMap<String, List<String>> queryMap = req.getUriQueryMap();
        assertTrue(queryMap.containsKey("mid"));
        assertTrue(req.getMethod().equals(RequestMethod.GET));
        assertEquals(req.getUriPath(), Constants.GROUP_FULL_URI + "/null");
    }

    // @InjectMocks for testPresenceDeregisterSpecificDeviceOnRequestReceived
    IRequest                             requestDeregisterSpecificDevice = makeDeregisterSpecificDevice(
            "device1");
    @InjectMocks
    DevicePresence.AccountReceiveHandler DeregisterHandler               = prsHandler.new AccountReceiveHandler(
            requestDeregisterSpecificDevice, mockDevice);

    @Test
    public void testPresenceDeregisterSpecificDeviceOnRequestReceived()
            throws Exception {
        System.out.println(
                "\t--------------OnRequestReceived(RD) Device Presence (deregister specific device) Test------------");
        IResponse response = responseFromAccountServer();
        DeregisterHandler.onResponseReceived(response);
        HashMap<String, List<String>> queryMap = req.getUriQueryMap();
        assertTrue(req.getMethod() == RequestMethod.GET);
        assertTrue(queryMap.get("di").contains("device1"));
        assertTrue(req.getObserve() == Observe.UNSUBSCRIBE);
    }

    @Test
    public void testDevicePresenceObserverEntireDevicesOnRequestReceived()
            throws Exception {
        System.out.println(
                "\t--------------OnRequestReceived(RD) Device Presence (observe entire devices) Test------------");
        IRequest request = makePresenceEntireDevice();
        deviceServerSystem.onRequestReceived(mockDevice, request);
        assertTrue(latch.await(1L, SECONDS));
        assertTrue(req.getMethod().equals(RequestMethod.GET));
        HashMap<String, List<String>> queryMap = req.getUriQueryMap();
        assertTrue(queryMap.containsKey("mid"));
        assertEquals(req.getUriPath(), Constants.GROUP_FULL_URI + "/null");
    }

    public IRequest makePresenceEntireDevice() {
        IRequest request = null;
        request = MessageBuilder.createRequest(RequestMethod.GET,
                DEVICE_PRS_REQ_URI, null);
        ((CoapRequest) request).setObserve(Observe.SUBSCRIBE);
        return request;
    }

    private IRequest makePresenceSpecificDevice(String di) {
        String query = "di=" + di;
        IRequest request = null;
        request = MessageBuilder.createRequest(RequestMethod.GET,
                DEVICE_PRS_REQ_URI, query);
        ((CoapRequest) request).setObserve(Observe.SUBSCRIBE);
        return request;
    }

    private IRequest makeDeregisterSpecificDevice(String di) {
        String query = "di=" + di;
        IRequest request = null;
        request = MessageBuilder.createRequest(RequestMethod.GET,
                DEVICE_PRS_REQ_URI, query);
        ((CoapRequest) request).setObserve(Observe.UNSUBSCRIBE);
        return request;
    }

    private IResponse responseFromAccountServer() {
        // make response which has "CONTENT" status
        Cbor<HashMap<String, Object>> cbor = new Cbor<>();
        HashMap<String, Object> responsePayload = new HashMap<String, Object>();
        ArrayList<String> deviceList = new ArrayList<String>();
        // assuming that there are three devices in the response msg from
        // the AS
        deviceList.add("device1");
        deviceList.add("device2");
        deviceList.add("device3");
        responsePayload.put("dilist", deviceList);
        responsePayload.put("gid", "g0001");
        responsePayload.put("gmid", "u0001");
        ArrayList<String> midList = new ArrayList<String>();
        midList.add("u0001");
        responsePayload.put("midlist", midList);
        IRequest requestFromCitoAs = MessageBuilder.createRequest(
                RequestMethod.GET, Constants.GROUP_FULL_URI + "/g0001",
                "mid=null");
        IResponse response = MessageBuilder.createResponse(requestFromCitoAs,
                ResponseStatus.CONTENT, ContentFormat.APPLICATION_CBOR,
                cbor.encodingPayloadToCbor(responsePayload));
        return response;
    }
}
