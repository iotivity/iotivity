/*
 * //******************************************************************
 * //
 * // Copyright 2016 Samsung Electronics All Rights Reserved.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 * //
 * // Licensed under the Apache License, Version 2.0 (the "License");
 * // you may not use this file except in compliance with the License.
 * // You may obtain a copy of the License at
 * //
 * //      http://www.apache.org/licenses/LICENSE-2.0
 * //
 * // Unless required by applicable law or agreed to in writing, software
 * // distributed under the License is distributed on an "AS IS" BASIS,
 * // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * // See the License for the specific language governing permissions and
 * // limitations under the License.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */
package org.iotivity.cloud.accountserver.resources.account.credprov.crl;

import org.bouncycastle.jce.provider.BouncyCastleProvider;
import org.iotivity.cloud.accountserver.Constants;
import org.iotivity.cloud.accountserver.resources.credprov.crl.CrlResource;
import org.iotivity.cloud.base.device.CoapDevice;
import org.iotivity.cloud.base.protocols.IRequest;
import org.iotivity.cloud.base.protocols.IResponse;
import org.iotivity.cloud.base.protocols.MessageBuilder;
import org.iotivity.cloud.base.protocols.coap.CoapResponse;
import org.iotivity.cloud.base.protocols.enums.ContentFormat;
import org.iotivity.cloud.base.protocols.enums.RequestMethod;
import org.iotivity.cloud.base.protocols.enums.ResponseStatus;
import org.iotivity.cloud.util.Cbor;
import org.junit.Before;
import org.junit.Test;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations;
import org.mockito.invocation.InvocationOnMock;
import org.mockito.stubbing.Answer;

import java.security.Security;
import java.util.HashMap;
import java.util.concurrent.CountDownLatch;

import static org.junit.Assert.assertTrue;
import static org.mockito.Mockito.mock;


public class CrlResourceTest {

    private CoapDevice mMockDevice = mock(CoapDevice.class);
    private Cbor<HashMap<String, Object>> mCbor = new Cbor<>();
    private IResponse mResponse = null;
    private CountDownLatch mLatch = new CountDownLatch(
            1);
    private CrlResource certificateResource = new CrlResource();
    private static final String CRL_URI = "/oic/credprov/crl";
    private static final String CRL_URI_QUERY = "lu=20160726210000";

    static {
        Security.insertProviderAt(new BouncyCastleProvider(), 0);
    }

    @Before
    public void setUp() throws Exception {
        MockitoAnnotations.initMocks(this);
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
                mResponse = resp;


                mLatch.countDown();
                return null;
            }
        }).when(mMockDevice).sendResponse(Mockito.anyObject());
    }

    @Test
    public void testCrlGet() throws Exception {
        getTestMethodName();
        IRequest request = crlRequest(RequestMethod.GET, CRL_URI, CRL_URI_QUERY);
        certificateResource.onDefaultRequestReceived(mMockDevice, request);
        assertTrue(methodCheck(mResponse, ResponseStatus.CONTENT));
    }

    @Test
    public void testCrlGetNotFound() throws Exception {
        getTestMethodName();
        IRequest request = crlRequest(RequestMethod.GET, null,null);
        certificateResource.onDefaultRequestReceived(mMockDevice, request);
        assertTrue(methodCheck(mResponse, ResponseStatus.NOT_FOUND));
    }

    @Test
    public void testCrlGetBadQuery() throws Exception {
        getTestMethodName();
        IRequest request = crlRequest(RequestMethod.GET, CRL_URI,"lu=1231212asdzfg4123123123123123");
        certificateResource.onDefaultRequestReceived(mMockDevice, request);
        assertTrue(methodCheck(mResponse, ResponseStatus.NOT_FOUND));
    }

    @Test
    public void testCrlPost() throws Exception {
        getTestMethodName();
        String thisUpdate = "20160823000000";
        String nextUpdate = "20161123000000";
        String serialNUmber = "123456";
        IRequest request = crlRequest(RequestMethod.POST, thisUpdate, nextUpdate, serialNUmber);
        certificateResource.onDefaultRequestReceived(mMockDevice, request);
        assertTrue(methodCheck(mResponse, ResponseStatus.CHANGED));
    }

    @Test
    public void testCrlUnParseableDate() throws Exception {
        getTestMethodName();
        String thisUpdate = "l;dkfjg;ls";
        String nextUpdate = "sdfgsdfg";
        String serialNUmber = "123456";
        IRequest request = crlRequest(RequestMethod.POST, thisUpdate, nextUpdate, serialNUmber);
        certificateResource.onDefaultRequestReceived(mMockDevice, request);
        assertTrue(methodCheck(mResponse, ResponseStatus.PRECONDITION_FAILED));
    }

    @Test
    public void testCrlPostPreconditionFailed() throws Exception {
        getTestMethodName();
        String thisUpdate = null;
        String nextUpdate = null;
        String serialNUmber = "123456";
        IRequest request = crlRequest(RequestMethod.POST, thisUpdate, nextUpdate, serialNUmber);
        certificateResource.onDefaultRequestReceived(mMockDevice, request);
    }



    @Test
    public void testCrlBadRequest() throws Exception {
        getTestMethodName();
        IRequest request = crlRequest(RequestMethod.PUT, CRL_URI, null);
        certificateResource.onDefaultRequestReceived(mMockDevice, request);
    }

    private IRequest crlRequest(RequestMethod method, String uri, String query) {
        IRequest request = MessageBuilder.createRequest(method, uri, query);
        return request;
    }

    private IRequest crlRequest(RequestMethod method, String thisUpdate, String nextUpdate, String rcsn) {
        HashMap<String, Object> payloadData = new HashMap<>();
        payloadData.put(Constants.REQ_THIS_UPDATE, thisUpdate);

        payloadData.put(Constants.REQ_NEXT_UPDATE, nextUpdate);
        payloadData.put(Constants.REQ_SERIAL_NUMBER, rcsn);
        IRequest request = MessageBuilder.createRequest(method, CRL_URI,
                null, ContentFormat.APPLICATION_CBOR,
                mCbor.encodingPayloadToCbor(payloadData));
        return request;
    }


    private void getTestMethodName() {
        StackTraceElement[] stacks = new Throwable().getStackTrace();
        StackTraceElement currentStack = stacks[1];
        System.out.println("\t---Test Name : " + currentStack.getMethodName());
    }

    private boolean methodCheck(IResponse response,
                                ResponseStatus responseStatus) {
        if (responseStatus == response.getStatus())
            return true;
        else
            return false;
    }


    private boolean hashmapCheck(IResponse response, String propertyName) {
        HashMap<String, Object> payloadData = mCbor
                .parsePayloadFromCbor(response.getPayload(), HashMap.class);
        if (payloadData.containsKey(propertyName))
            return true;
        else
            return false;
    }
}
