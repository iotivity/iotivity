package org.iotivity.cloud.ciserver.resources;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;
import static org.mockito.Mockito.mock;

import java.util.ArrayList;
import java.util.HashMap;

import org.iotivity.cloud.base.device.CoapDevice;
import org.iotivity.cloud.base.protocols.IRequest;
import org.iotivity.cloud.base.protocols.IResponse;
import org.iotivity.cloud.base.protocols.MessageBuilder;
import org.iotivity.cloud.base.protocols.coap.CoapResponse;
import org.iotivity.cloud.base.protocols.enums.ContentFormat;
import org.iotivity.cloud.base.protocols.enums.RequestMethod;
import org.iotivity.cloud.base.protocols.enums.ResponseStatus;
import org.iotivity.cloud.util.Cbor;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.mockito.invocation.InvocationOnMock;
import org.mockito.stubbing.Answer;

public class KeepAliveResourceTest {

    KeepAliveResource                                 keepAliveResource;
    CoapDevice                                        mockDevice;
    private Cbor<HashMap<String, ArrayList<Integer>>> mCbor    = new Cbor<>();
    IResponse                                         res;
    int[]                                             interval = { 1, 2, 4, 8 };

    @Before
    public void setUp() throws Exception {
        keepAliveResource = new KeepAliveResource(interval);
        MockitoAnnotations.initMocks(this);
        mockDevice = mock(CoapDevice.class);
        res = null;
        // callback mock
        Mockito.doAnswer(new Answer<Object>() {
            @Override
            public CoapResponse answer(InvocationOnMock invocation)
                    throws Throwable {
                Object[] args = invocation.getArguments();
                CoapResponse resp = (CoapResponse) args[0];
                System.out
                        .println("\t----payload : " + resp.getPayloadString());
                System.out
                        .println("\t----responsestatus : " + resp.getStatus());
                res = resp;
                return null;
            }
        }).when(mockDevice).sendResponse(Mockito.anyObject());
        keepAliveResource.startSessionChecker();
    }

    @After
    public void Endup() throws Exception {
        keepAliveResource.stopSessionChecker();
    }

    @Test
    public void testOnDefaultGetRequestReceived() {
        IRequest request = MessageBuilder.createRequest(RequestMethod.GET,
                "/oic/ping", null);
        keepAliveResource.onDefaultRequestReceived(mockDevice, request);
        HashMap<String, ArrayList<Integer>> payloadData = mCbor
                .parsePayloadFromCbor(res.getPayload(), HashMap.class);
        HashMap<String, ArrayList<Integer>> comparisonData = new HashMap<>();
        ArrayList<Integer> comparisonArray = new ArrayList<>();
        comparisonArray.add(1);
        comparisonArray.add(2);
        comparisonArray.add(4);
        comparisonArray.add(8);
        comparisonData.put("inarray", comparisonArray);
        assertEquals(payloadData, comparisonData);
        assertTrue(methodCheck(res, ResponseStatus.CONTENT));
    }

    @Test
    public void testOnDefaultPutRequestReceived() {
        HashMap<String, Integer> payloadData = new HashMap<>();
        Cbor<HashMap<String, Object>> cbor = new Cbor<>();
        payloadData.put("in", 8);
        IRequest request = MessageBuilder.createRequest(RequestMethod.PUT,
                "/oic/ping", null, ContentFormat.APPLICATION_CBOR,
                cbor.encodingPayloadToCbor(payloadData));
        keepAliveResource.onDefaultRequestReceived(mockDevice, request);
        assertTrue(methodCheck(res, ResponseStatus.VALID));
    }

    // TODO : exception response assertion
    // @Test
    public void testOnDefaultPutEmptyPayloadRequestReceived() {
        HashMap<String, Integer> payloadData = new HashMap<>();
        Cbor<HashMap<String, Object>> cbor = new Cbor<>();
        IRequest request = MessageBuilder.createRequest(RequestMethod.PUT,
                "/oic/ping", null, ContentFormat.APPLICATION_CBOR,
                cbor.encodingPayloadToCbor(payloadData));
        keepAliveResource.onDefaultRequestReceived(mockDevice, request);
        assertTrue(methodCheck(res, ResponseStatus.VALID));
    }

    // @Test
    public void testOnDefaultPutNullPayloadRequestReceived() {
        IRequest request = MessageBuilder.createRequest(RequestMethod.PUT,
                "/oic/ping", null);
        keepAliveResource.onDefaultRequestReceived(mockDevice, request);
        assertTrue(methodCheck(res, ResponseStatus.BAD_REQUEST));
    }

    private boolean methodCheck(IResponse response,
            ResponseStatus responseStatus) {
        if (responseStatus == response.getStatus())
            return true;
        else
            return false;
    }
}
