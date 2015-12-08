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
import oic.ctt.network.OICResponseData;
import oic.ctt.network.OICHelper.MessageParameters;
import oic.ctt.testhelper.TestSpec;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.mockito.Mockito;
import org.ws4d.coap.interfaces.CoapResponse;
import org.ws4d.coap.messages.CoapPacketType;

public class OICResponseDataTest {

    OICResponseData oicResponseData;

    @BeforeClass
    public static void setUpBeforeClass() throws Exception {
    }

    @AfterClass
    public static void tearDownAfterClass() throws Exception {
    }

    @Before
    public void setUp() throws Exception {
        oicResponseData = new OICResponseData();
    }

    @After
    public void tearDown() throws Exception {
    }

    @TestSpec(author = "SK Tuhin Islam(sk.islam@samsung.com)",
            last_updated = "2015-09-08",
            since = "2015-09-08",
            objective = "Test 'getProtocolType' positively",
            target = "public OICClient.Protocol getProtocolType()",
            procedure = "Call setOptionSet()",
            expected = "getProtocolType() will be called without exception and get protocol type as COAP")
    @Test
    public void getProtocolType_P() {
        try {
            assertEquals("Failed to get protocol type",
                    OICClient.Protocol.COAP, oicResponseData.getProtocolType());
        } catch (Exception ex) {
            fail("Failed to call getProtocolType method without exception :"
                    + ex.getMessage());
        }
    }

    @TestSpec(author = "SK Tuhin Islam(sk.islam@samsung.com)",
            last_updated = "2015-09-08",
            since = "2015-09-08",
            objective = "Test 'getOptionSet' positively",
            target = "public void getOptionSet()",
            procedure = "Call setOptionSet()",
            expected = "getOptionSet() will be called without exception")
    @Test
    public void getOptionSet_P() {
        try {
            assertNull("Failed to get protocol type",
                    oicResponseData.getOptionSet());
        } catch (Exception ex) {
            fail("Failed to call getOptionSet method without exception :"
                    + ex.getMessage());
        }
    }

    @TestSpec(author = "SK Tuhin Islam(sk.islam@samsung.com)",
            last_updated = "2015-09-08",
            since = "2015-09-08",
            objective = "Test 'setOptionSet' positively",
            target = "public void setOptionSet(String optionSet)",
            procedure = "Call setOptionSet()",
            expected = "setOptionSet() will be called without exception")
    @Test
    public void setOptionSet_P() {
        try {
            oicResponseData.setOptionSet("OptionSetString");
        } catch (Exception ex) {
            fail("Failed to call setOptionSet method without exception :"
                    + ex.getMessage());
        }
    }

    @TestSpec(author = "SK Tuhin Islam(sk.islam@samsung.com)",
            last_updated = "2015-09-08",
            since = "2015-09-08",
            objective = "Test 'setProtocolType' positively",
            target = "public void setProtocolType(OICClient.Protocol protocolType)",
            test_data = "COAP",
            procedure = "Call setProtocolType()",
            expected = "setProtocolType() will be called without exception")
    @Test
    public void setProtocolType_P() {
        try {
            oicResponseData.setProtocolType(OICClient.Protocol.COAP);
        } catch (Exception ex) {
            fail("Failed to call setProtocolType method without exception :"
                    + ex.getMessage());
        }
    }

    @TestSpec(author = "SK Tuhin Islam(sk.islam@samsung.com)",
            last_updated = "2015-09-08",
            since = "2015-09-08",
            objective = "Test 'convertRequestCoap' positively",
            target = "public static OICResponseData convertResponseCoap(CoapResponse coapResponse)",
            test_data = "coapResponse",
            procedure = "Call convertRequestCoap()",
            expected = "convertRequestCoap() will be called without exception and should not return null")
    @Test
    public void convertResponseCoap_P() {
        String testString = "testString";
        byte[] testByte = testString.getBytes();
        CoapResponse coapResponse = Mockito.mock(CoapResponse.class);
        Mockito.when(coapResponse.getPacketType()).thenReturn(
                CoapPacketType.NON);
        Mockito.when(coapResponse.getSourceIP()).thenReturn("107/109/214/213");
        Mockito.when(coapResponse.getSourcePort()).thenReturn(6298);
        Mockito.when(coapResponse.getResponseCodeString()).thenReturn(
                "testData");
        Mockito.when(coapResponse.getHeaderOptions()).thenReturn("testData");
        Mockito.when(coapResponse.getPayload()).thenReturn(testByte);
        Mockito.when(coapResponse.getToken()).thenReturn(testByte);
        Mockito.when(coapResponse.getMessageID()).thenReturn(1);

        try {
            assertNotNull(OICResponseData.convertResponseCoap(coapResponse));
        } catch (Exception ex) {
            fail("Failed to call convertRequestCoap method without exception :"
                    + ex.getMessage());
        }
    }

    @TestSpec(author = "SK Tuhin Islam(sk.islam@samsung.com)",
            last_updated = "2015-09-08",
            since = "2015-09-08",
            objective = "Test 'convertResponseCoap' negatively",
            target = "public static OICResponseData convertResponseCoap(CoapResponse coapResponse)",
            test_data = "null",
            procedure = "Call convertRequestCoap()",
            expected = "convertRequestCoap() will be called without exception and should not return null")
    @Test
    public void convertResponseCoap_N() {
        try {
            assertNull(OICResponseData.convertResponseCoap(null));
        } catch (Exception ex) {
            fail("Failed to call convertRequestCoap method without exception :"
                    + ex.getMessage());
        }
    }

    @TestSpec(author = "SK Tuhin Islam(sk.islam@samsung.com)",
            last_updated = "2015-09-01",
            since = "2015-09-01",
            objective = "Test 'setResponseValue' positively",
            target = "public void setResponseValue(MessageParameters key, String value)",
            test_data = "destAddress destAddressValue "
                    + "destPort destPortValue" + "mId mIdValue "
                    + "msgType msgTypeValue" + "payload payloadValue"
                    + "responseCode responseCodeValue"
                    + "seqNumber seqNumberValue" + "srcAddress srcAddressValue"
                    + "srcPort srcPortValue" + "token tokenValye",
            procedure = "Call setResponseValue()",
            expected = "setResponseValue() will be called without exception")
    @Test
    public void setResponseValue_P() {
        try {
            oicResponseData
                    .setResponseValue(OICHelper.MessageParameters.destAddress,
                            "destAddressValue");
            oicResponseData.setResponseValue(
                    OICHelper.MessageParameters.destPort, "destPortValue");
            oicResponseData.setResponseValue(OICHelper.MessageParameters.mId,
                    "mIdValue");
            oicResponseData.setResponseValue(
                    OICHelper.MessageParameters.msgType, "msgTypeValue");
            oicResponseData.setResponseValue(
                    OICHelper.MessageParameters.payload, "payloadValue");
            oicResponseData.setResponseValue(
                    OICHelper.MessageParameters.responseCode,
                    "requestCodeValue");
            oicResponseData.setResponseValue(
                    OICHelper.MessageParameters.responseCode,
                    "responseCodeValue");
            oicResponseData.setResponseValue(
                    OICHelper.MessageParameters.seqNumber, "seqNumberValue");
            oicResponseData.setResponseValue(
                    OICHelper.MessageParameters.srcAddress, "srcAddressValue");
            oicResponseData.setResponseValue(
                    OICHelper.MessageParameters.srcPort, "srcPortValue");
            oicResponseData.setResponseValue(OICHelper.MessageParameters.token,
                    "tokenValue");
        } catch (Exception ex) {
            fail("Failed to call setResponseValue method without exception :"
                    + ex.getMessage());
        }
    }

    @TestSpec(author = "SK Tuhin Islam(sk.islam@samsung.com)",
            last_updated = "2015-09-01",
            since = "2015-09-01",
            objective = "Test 'getResponseValue' positively",
            target = "public String getResponseValue(MessageParameters key)",
            test_data = "destAddress " + "destPort" + " mId msgType"
                    + " payload " + "requestCode" + " responseCode"
                    + " seqNumber " + "srcAddress" + " srcPort token",
            procedure = "Call getResponseValue()",
            expected = "getResponseValue() will be called without exception")
    @Test
    public void getResponseValue_P() {
        try {
            assertNull(oicResponseData
                    .getResponseValue(MessageParameters.destPort));
            assertNull(oicResponseData
                    .getResponseValue(MessageParameters.destAddress));
            assertNull(oicResponseData.getResponseValue(MessageParameters.mId));
            assertNull(oicResponseData
                    .getResponseValue(MessageParameters.msgType));
            assertNull(oicResponseData
                    .getResponseValue(MessageParameters.payload));
            assertNull(oicResponseData
                    .getResponseValue(MessageParameters.requestCode));
            assertNull(oicResponseData
                    .getResponseValue(MessageParameters.responseCode));
            assertNull(oicResponseData
                    .getResponseValue(MessageParameters.seqNumber));
            assertNull(oicResponseData
                    .getResponseValue(MessageParameters.srcAddress));
            assertNull(oicResponseData
                    .getResponseValue(MessageParameters.srcPort));
            assertNull(oicResponseData
                    .getResponseValue(MessageParameters.token));
        } catch (Exception ex) {
            fail("Failed to call setRequestValue method without exception :"
                    + ex.getMessage());
        }
    }

}
