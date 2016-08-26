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
package org.iotivity.cloud.accountserver.resources.account.credprov.cert;

import org.bouncycastle.util.encoders.Base64;
import org.iotivity.cloud.accountserver.Constants;
import org.iotivity.cloud.accountserver.resources.account.AccountResource;
import org.iotivity.cloud.accountserver.resources.credprov.cert.CertificateConstants;
import org.iotivity.cloud.accountserver.resources.credprov.cert.CertificateResource;
import org.iotivity.cloud.accountserver.resources.credprov.cert.CertificateStorage;
import org.iotivity.cloud.base.OICConstants;
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

import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.security.cert.CertificateFactory;
import java.security.cert.X509Certificate;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.CountDownLatch;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;
import static org.mockito.Mockito.mock;

public class CertificateResourceTest {

    private static final String COMMON_NAME = "OU=OCF Root CA, O=Samsung, C=US, CN=uuid:B371C481-38E6-4D47-8320-7688D8A5B58C";
    private static final String CERTIFICATE_URI = OICConstants.CREDPROV_CERT_FULL_URI;
    private static final String DEVICE_ID = "B371C481-38E6-4D47-8320-7688D8A5B58C";
    private CoapDevice mMockDevice = mock(CoapDevice.class);
    private Cbor<HashMap<String, Object>> mCbor = new Cbor<>();
    private IResponse mResponse = null;
    private CountDownLatch mLatch = new CountDownLatch(
            1);
    private CertificateResource certificateResource = new CertificateResource();
    private byte[] csr;

    @Before
    public void setUp() throws Exception {
        csr = GenerateCSR.generatePKCS10(COMMON_NAME, false);
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
    public void testCSRIssueDER() throws Exception {
        getTestMethodName();
        IRequest request = csrRequest(DEVICE_ID, CertificateConstants.DER, csr, RequestMethod.POST, true);
        certificateResource.onDefaultRequestReceived(mMockDevice, request);
        Map<String, Object> payloadData = mCbor
                .parsePayloadFromCbor(mResponse.getPayload(), HashMap.class);
        assertTrue(hashmapCheck(mResponse, Constants.REQ_DEVICE_ID));
        assertTrue(hashmapCheck(mResponse, Constants.CERT_ENCODING));
        assertTrue(hashmapCheck(mResponse, Constants.CERT_DATA));
        assertTrue(hashmapCheck(mResponse, Constants.CERTCHAIN_ENCODING));
        assertTrue(hashmapCheck(mResponse, Constants.CERTCHAIN_DATA));

        CertificateFactory certFactory = CertificateFactory.getInstance("X.509");
        InputStream in = new ByteArrayInputStream((byte[]) payloadData.get(Constants.CERT_DATA));
        X509Certificate cert = (X509Certificate) certFactory.generateCertificate(in);


        assertEquals(COMMON_NAME, cert.getSubjectDN().getName());
        assertEquals(cert.getPublicKey(), GenerateCSR.getPublicKey());
        assertEquals("CN="+CertificateStorage.SUBJECT_NAME, cert.getIssuerDN().getName());
        System.out.println(cert);
        assertTrue(methodCheck(mResponse, ResponseStatus.CHANGED));

    }

    @Test
    public void testCSRIssueDERFailedSignature() throws Exception {
        byte[] csr = GenerateCSR.generatePKCS10(COMMON_NAME, true);
        getTestMethodName();
        IRequest request = csrRequest(DEVICE_ID, CertificateConstants.DER, csr, RequestMethod.POST, true);
        certificateResource.onDefaultRequestReceived(mMockDevice, request);
        assertTrue(methodCheck(mResponse, ResponseStatus.BAD_REQUEST));

    }

    @Test
    public void testReIssueBase64() {
        IRequest request = csrRequest(DEVICE_ID, CertificateConstants.BASE_64, Base64.encode(csr), RequestMethod.POST, true);
        certificateResource.onDefaultRequestReceived(mMockDevice, request);
        assertTrue(methodCheck(mResponse, ResponseStatus.CHANGED));

    }



    @Test
    public void testMethodNotAllowed() {
        IRequest request = csrRequest(DEVICE_ID, CertificateConstants.DER, csr, RequestMethod.GET, true);
        certificateResource.onDefaultRequestReceived(mMockDevice, request);
        assertTrue(methodCheck(mResponse, ResponseStatus.METHOD_NOT_ALLOWED));
    }



    @Test
    public void testBadRequest() {
        IRequest request = csrRequest(DEVICE_ID, CertificateConstants.DER, csr, RequestMethod.POST, false);
        certificateResource.onDefaultRequestReceived(mMockDevice, request);
        assertTrue(methodCheck(mResponse, ResponseStatus.BAD_REQUEST));
    }


    @Test
    public void testBadData() {
        IRequest request = csrRequest(DEVICE_ID, CertificateConstants.DER, null, RequestMethod.POST, true);
        certificateResource.onDefaultRequestReceived(mMockDevice, request);
        assertTrue(methodCheck(mResponse, ResponseStatus.BAD_REQUEST));
    }


    private void getTestMethodName() {
        StackTraceElement[] stacks = new Throwable().getStackTrace();
        StackTraceElement currentStack = stacks[1];
        System.out.println("\t---Test Name : " + currentStack.getMethodName());
    }

    private IRequest csrRequest(String deviceId, String encoding, byte[] data, RequestMethod method, boolean isEncoded) {
        IRequest request;
        HashMap<String, Object> payloadData = new HashMap<>();
        payloadData.put(Constants.REQ_DEVICE_ID, deviceId);
        if (isEncoded) {
            payloadData.put(Constants.CSR_ENCODING, encoding);
        }
        payloadData.put(Constants.CSR_DATA, data);
        request = MessageBuilder.createRequest(method, CERTIFICATE_URI,
                null, ContentFormat.APPLICATION_CBOR,
                mCbor.encodingPayloadToCbor(payloadData));
        return request;
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
