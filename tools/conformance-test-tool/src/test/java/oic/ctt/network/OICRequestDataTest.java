//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= 
package oic.ctt.network;

import static org.junit.Assert.*;
import oic.ctt.network.OICClient;
import oic.ctt.network.OICHelper;
import oic.ctt.network.OICObserver;
import oic.ctt.network.OICRequestData;
import oic.ctt.network.OICHelper.MessageParameters;
import oic.ctt.testhelper.*;

import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.mockito.Mockito;
import org.mockito.internal.util.reflection.Whitebox;
import org.ws4d.coap.interfaces.CoapRequest;
import org.ws4d.coap.messages.CoapPacketType;

public class OICRequestDataTest {

    @Rule
    public ReportGenerator reportGenerator = new ReportGenerator();

    private OICObserver    oicObserver;
    private CoapRequest    coapRequest;
    private OICRequestData oicRequestData;

    @Before
    public void setUp() throws Exception {
        oicRequestData = new OICRequestData();
        oicObserver = new OICObserver();
        coapRequest = Mockito.mock(CoapRequest.class);
        Whitebox.setInternalState(oicObserver, "mCoapRequest", coapRequest);
    }

    @TestSpec(author = "SK Tuhin Islam(sk.islam@samsung.com)",
            last_updated = "2015-08-26",
            since = "2015-08-26",
            objective = "Test 'getProtocolType' positively",
            target = "public OICClient.Protocol getProtocolType()",
            procedure = "Call setOptionSet()",
            expected = "getProtocolType() will be called without exception and get protocol type as COAP")
    @Test
    public void getProtocolType_P() {
        try {
            assertEquals("Failed to get protocol type",
                    OICClient.Protocol.COAP, oicRequestData.getProtocolType());
        } catch (Exception ex) {
            fail("Failed to call getProtocolType method without exception :"
                    + ex.getMessage());
        }
    }

    @TestSpec(author = "SK Tuhin Islam(sk.islam@samsung.com)",
            last_updated = "2015-08-26",
            since = "2015-08-26",
            objective = "Test 'getOptionSet' positively",
            target = "public void getOptionSet()",
            procedure = "Call setOptionSet()",
            expected = "getOptionSet() will be called without exception")
    @Test
    public void getOptionSet_P() {
        try {
            assertNull("Failed to get protocol type",
                    oicRequestData.getOptionSet());
        } catch (Exception ex) {
            fail("Failed to call getOptionSet method without exception :"
                    + ex.getMessage());
        }
    }

    @TestSpec(author = "SK Tuhin Islam(sk.islam@samsung.com)",
            last_updated = "2015-08-26",
            since = "2015-08-26",
            objective = "Test 'setOptionSet' positively",
            target = "public void setOptionSet(String optionSet)",
            procedure = "Call setOptionSet()",
            expected = "setOptionSet() will be called without exception")
    @Test
    public void setOptionSet_P() {
        try {
            oicRequestData.setOptionSet("OptionSetString");
        } catch (Exception ex) {
            fail("Failed to call setOptionSet method without exception :"
                    + ex.getMessage());
        }
    }

    @TestSpec(author = "SK Tuhin Islam(sk.islam@samsung.com)",
            last_updated = "2015-09-01",
            since = "2015-09-01",
            objective = "Test 'setProtocolType' positively",
            target = "public void setProtocolType(OICClient.Protocol protocolType)",
            test_data = "COAP",
            procedure = "Call setProtocolType()",
            expected = "setProtocolType() will be called without exception")
    @Test
    public void setProtocolType_P() {
        try {
            oicRequestData.setProtocolType(OICClient.Protocol.COAP);
        } catch (Exception ex) {
            fail("Failed to call setProtocolType method without exception :"
                    + ex.getMessage());
        }
    }

    @TestSpec(author = "SK Tuhin Islam(sk.islam@samsung.com)",
            last_updated = "2015-09-01",
            since = "2015-09-01",
            objective = "Test 'setRequestValue' positively",
            target = "public void setRequestValue(MessageParameters key, String value)",
            test_data = "destAddress destAddressValue "
                    + "destPort destPortValue" + "mId mIdValue "
                    + "msgType msgTypeValue" + "payload payloadValue"
                    + "responseCode responseCodeValue"
                    + "seqNumber seqNumberValue" + "srcAddress srcAddressValue"
                    + "srcPort srcPortValue" + "token tokenValye",
            procedure = "Call setRequestValue()",
            expected = "setRequestValue() will be called without exception")
    @Test
    public void setRequestValue_P() {
        try {
            oicRequestData
                    .setRequestValue(OICHelper.MessageParameters.destAddress,
                            "destAddressValue");
            oicRequestData.setRequestValue(
                    OICHelper.MessageParameters.destPort, "destPortValue");
            oicRequestData.setRequestValue(OICHelper.MessageParameters.mId,
                    "mIdValue");
            oicRequestData.setRequestValue(OICHelper.MessageParameters.msgType,
                    "msgTypeValue");
            oicRequestData.setRequestValue(OICHelper.MessageParameters.payload,
                    "payloadValue");
            oicRequestData
                    .setRequestValue(OICHelper.MessageParameters.requestCode,
                            "requestCodeValue");
            oicRequestData.setRequestValue(
                    OICHelper.MessageParameters.responseCode,
                    "responseCodeValue");
            oicRequestData.setRequestValue(
                    OICHelper.MessageParameters.seqNumber, "seqNumberValue");
            oicRequestData.setRequestValue(
                    OICHelper.MessageParameters.srcAddress, "srcAddressValue");
            oicRequestData.setRequestValue(OICHelper.MessageParameters.srcPort,
                    "srcPortValue");
            oicRequestData.setRequestValue(OICHelper.MessageParameters.token,
                    "destAddressValue");
        } catch (Exception ex) {
            fail("Failed to call setRequestValue method without exception :"
                    + ex.getMessage());
        }
    }

    @TestSpec(author = "SK Tuhin Islam(sk.islam@samsung.com)",
            last_updated = "2015-09-01",
            since = "2015-09-01",
            objective = "Test 'convertRequestCoap' positively",
            target = "public static OICRequestData convertRequestCoap(CoapRequest request)",
            test_data = "coapRequest",
            procedure = "Call convertRequestCoap()",
            expected = "convertRequestCoap() will be called without exception and should not return null")
    @Test
    public void convertRequestCoap_P() {
        String testString = "testString";
        byte[] testByte = testString.getBytes();
        CoapRequest coapRequest = Mockito.mock(CoapRequest.class);
        Mockito.when(coapRequest.getPacketType())
                .thenReturn(CoapPacketType.NON);
        Mockito.when(coapRequest.getSourceIP()).thenReturn("107/109/214/213");
        Mockito.when(coapRequest.getSourcePort()).thenReturn(6298);
        Mockito.when(coapRequest.getRequestCodeString()).thenReturn("testData");
        Mockito.when(coapRequest.getHeaderOptions()).thenReturn("testData");
        Mockito.when(coapRequest.getPayload()).thenReturn(testByte);
        Mockito.when(coapRequest.getToken()).thenReturn(testByte);
        Mockito.when(coapRequest.getMessageID()).thenReturn(1);

        try {
            assertNotNull(OICRequestData.convertRequestCoap(coapRequest));
        } catch (Exception ex) {
            fail("Failed to call convertRequestCoap method without exception :"
                    + ex.getMessage());
        }
    }

    @TestSpec(author = "SK Tuhin Islam(sk.islam@samsung.com)",
            last_updated = "2015-09-01",
            since = "2015-09-01",
            objective = "Test 'convertRequestCoap' negatively",
            target = "public static OICRequestData convertRequestCoap(CoapRequest request)",
            test_data = "null",
            procedure = "Call convertRequestCoap()",
            expected = "convertRequestCoap() will be called without exception and shpuld return null")
    @Test
    public void convertRequestCoap_N() {
        try {
            assertNull(OICRequestData.convertRequestCoap(null));
        } catch (Exception ex) {
            fail("Failed to call convertRequestCoap method without exception :"
                    + ex.getMessage());
        }
    }

    @TestSpec(author = "SK Tuhin Islam(sk.islam@samsung.com)",
            last_updated = "2015-09-01",
            since = "2015-09-01",
            objective = "Test 'getRequestValue' positively",
            target = "public String getRequestValue(MessageParameters key)",
            test_data = "destAddress " + "destPort" + " mId msgType"
                    + " payload " + "requestCode" + " responseCode"
                    + " seqNumber " + "srcAddress" + " srcPort token",
            procedure = "Call getRequestValue()",
            expected = "getRequestValue() will be called without exception")
    @Test
    public void getRequestValue_P() {
        try {
            assertNull(oicRequestData
                    .getRequestValue(MessageParameters.destPort));
            assertNull(oicRequestData
                    .getRequestValue(MessageParameters.destAddress));
            assertNull(oicRequestData.getRequestValue(MessageParameters.mId));
            assertNull(oicRequestData
                    .getRequestValue(MessageParameters.msgType));
            assertNull(oicRequestData
                    .getRequestValue(MessageParameters.payload));
            assertNull(oicRequestData
                    .getRequestValue(MessageParameters.requestCode));
            assertNull(oicRequestData
                    .getRequestValue(MessageParameters.responseCode));
            assertNull(oicRequestData
                    .getRequestValue(MessageParameters.seqNumber));
            assertNull(oicRequestData
                    .getRequestValue(MessageParameters.srcAddress));
            assertNull(oicRequestData
                    .getRequestValue(MessageParameters.srcPort));
            assertNull(oicRequestData.getRequestValue(MessageParameters.token));
        } catch (Exception ex) {
            fail("Failed to call setRequestValue method without exception :"
                    + ex.getMessage());
        }
    }

}
