// ******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
//
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

package oic.ctt.network.coap;

import static org.junit.Assert.*;

import java.util.ArrayList;

import oic.ctt.network.OICResponseData;
import oic.ctt.network.coap.OICCoapClient;
import oic.ctt.testhelper.*;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Rule;
import org.junit.Test;
import org.mockito.Mockito;
import org.ws4d.coap.interfaces.CoapClientChannel;
import org.ws4d.coap.interfaces.CoapResponse;

import oic.ctt.network.OICHelper;
import oic.ctt.formatter.OCPayloadType;
import oic.ctt.network.OICRequestData;


public class OICCoapClientTest extends ConformanceTestHelper{

    @Rule
    public ReportGenerator reportGenerator = new ReportGenerator();

    CoapClientChannel      channel;
    CoapResponse           response;
    private OICCoapClient  oicCoapClient;

    @BeforeClass
    public static void setUpBeforeClass() throws Exception {
    }

    @AfterClass
    public static void tearDownAfterClass() throws Exception {
    }

    @Before
    public void setUp() throws Exception {
        oicCoapClient = new OICCoapClient();
        channel = Mockito.mock(CoapClientChannel.class);
    }

    @After
    public void tearDown() throws Exception {
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-25
     * @reviewer
     * @since 2015-08-25
     * @see none
     * @objective Test 'getResponse' positively
     * @target public OICResponseData getResponse(String token)
     * @test_data token
     * @pre_condition none
     * @procedure Call getResponse()
     * @post_condition none
     * @expected getResponse() will be called without exception
     */

    @Test
    public void getResponse_P() {
        try {
            assertNull(oicCoapClient.getResponse("token"));
        } catch (Exception ex) {
            fail("failed to call getResponse method without exception");
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-25
     * @reviewer
     * @since 2015-08-25
     * @see none
     * @objective Test 'getResponse' negatively
     * @target public OICResponseData getResponse(String token)
     * @test_data none
     * @pre_condition none
     * @procedure Call getResponse()
     * @post_condition none
     * @expected getResponse() will be called without exception
     */

    @Test
    public void getResponse_N() {
        try {
            assertNull(oicCoapClient.getResponse(null));
        } catch (Exception ex) {
            fail("failed to call getResponse method without exception");
        }
    }
    
    /**
	 * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2015-01-01
	 * @reviewer
	 * @since 2015-01-01
	 * @see none
	 * @objective Test 'setWaitTime' positively
	 * @target public void setWaitTime(int waitTime)
	 * @test_data none
	 * @pre_condition none
	 * @procedure Call setWaitTime(int waitTime)
	 * @post_condition none
	 * @expected setWaitTime() will be called without exception
	 */
    @Test
   	public void setWaitTime_P() {
       	try {
       		oicCoapClient.setWaitTime(5);
           } catch (Exception ex) {
           	ex.printStackTrace();
               fail("Fail"+ ex.getLocalizedMessage());
           }  	
   	}

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-25
     * @reviewer
     * @since 2015-08-25
     * @see none
     * @objective Test 'getResponses' positively
     * @target public ArrayList<OICResponseData> getResponses()
     * @test_data none
     * @pre_condition none
     * @procedure Call getResponses()
     * @post_condition none
     * @expected getResponses() will be called without exception
     */

    @Test
    public void getResponses_P() {
        try {
            ArrayList<OICResponseData> responses = oicCoapClient.getResponses();
            assertNotNull(responses);
            assertEquals(0, responses.size());
        } catch (Exception ex) {
            fail("failed to call getResponses method without exception");
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-25
     * @reviewer
     * @since 2015-08-25
     * @see none
     * @objective Test 'onConnectionFailed' positively
     * @target public void onConnectionFailed(CoapClientChannel channel, boolean
     *         notReachable, boolean resetByServer)
     * @test_data channel, true, true
     * @test_data channel, false, true
     * @test_data channel, true, false
     * @pre_condition none
     * @procedure Call onConnectionFailed()
     * @post_condition none
     * @expected onConnectionFailed() will be called without exception
     */

   @Test
   public void onConnectionFailed_P() {
       try {
           oicCoapClient.onConnectionFailed(channel, true, true);
           oicCoapClient.onConnectionFailed(channel, false, true);
           oicCoapClient.onConnectionFailed(channel, true, false);
       } catch (Exception ex) {
           fail("failed to call onConnectionFailed method without exception");
       }
   }

     /**
     * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2015-12-31
     * @reviewer
     * @since 2015-12-31
     * @see none
     * @objective Test 'getNotifications' positively
     * @target public void getNotifications
     * @test_data none
     * @pre_condition none
     * @procedure Call getNotifications()
     * @post_condition none
     * @expected getNotifications() will be called without exception
     */
    @Test
    public void getNotifications_P() {
	 try {
            ArrayList<OICResponseData> responses = oicCoapClient.getNotifications();
            assertNotNull(responses);
            assertEquals(0, responses.size());
        } catch (Exception ex) {
            fail("failed to call getNotification method without exception");
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-25
     * @reviewer
     * @since 2015-08-25
     * @see none
     * @objective Test 'onConnectionFailed' negatively
     * @target public void onConnectionFailed(CoapClientChannel channel, boolean
     *         notReachable, boolean resetByServer)
     * @test_data null false false
     * @pre_condition none
     * @procedure Call onConnectionFailed()
     * @post_condition none
     * @expected onConnectionFailed() will be called without exception
     */

    @Test
    public void onConnectionFailed_N() {
        try {
            oicCoapClient.onConnectionFailed(null, false, false);
        } catch (Exception ex) {
            fail("failed to call onConnectionFailed method without exception");
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-25
     * @reviewer
     * @since 2015-08-25
     * @see none
     * @objective Test 'onResponse' positively
     * @target public void onResponse(CoapClientChannel channel, CoapResponse
     *         response)
     * @test_data channel onResponse
     * @pre_condition none
     * @procedure Call onResponse()
     * @post_condition none
     * @expected onResponse() will be called without exception
     */

    @Test
    public void onResponse_P() {
        try {

            oicCoapClient.onResponse(channel, response);
        } catch (Exception ex) {
            fail("failed to call onResponse method without exception");
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-25
     * @reviewer
     * @since 2015-08-25
     * @see none
     * @objective Test 'onResponse' negatively
     * @target public void onResponse(CoapClientChannel channel, CoapResponse
     *         response)
     * @test_data channel onResponse
     * @pre_condition none
     * @procedure Call onResponse()
     * @post_condition none
     * @expected onResponse() will be called without exception
     */

    @Test
    public void onResponse_N() {
        try {
            oicCoapClient.onResponse(null, null);
        } catch (Exception ex) {
            fail("failed to call onResponse method without exception");
        }
    }

    /**
     * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2015-12-31
     * @reviewer
     * @since 2015-12-31
     * @see none
     * @objective Test 'discoverResource' positively
     * @target public void discoverResource
     * @test_data uriPath query ip port token
     * @pre_condition none
     * @procedure Call discoverResource()
     * @post_condition none
     * @expected discoverResource() will be called without exception and return a response list of
     *           size 0
     */

    @Test
    public void discoverResource_P() {
        try {
            assertNotNull(oicCoapClient.discoverResource(uriPath, query, ipv4, port, token));
	    assertEquals(0, oicCoapClient.discoverResource(uriPath, query, ipv4, port, token).size());
        } catch (Exception ex) {
            fail("failed to call discoverResource method without exception");
        }
    }

    /**
     * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2015-12-31
     * @reviewer
     * @since 2015-12-31
     * @see none
     * @objective Test 'discoverResource' negatively
     * @target public void discoverResource
     * @test_data null null null -1 null
     * @pre_condition none
     * @procedure Call discoverResource()
     * @post_condition none
     * @expected discoverResource() will be called without exception and return a response list of
     *        size 0
     */

    @Test
    public void discoverResource_N() {
        try {
            assertNotNull(oicCoapClient.discoverResource(null, null, null, -1, null));
	    assertEquals(0, oicCoapClient.discoverResource(null, null, null, -1, null).size());
        } catch (Exception ex) {
            fail("failed to call discoverResource method without exception");
        }
    }

    /**
     * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2015-12-31
     * @reviewer
     * @since 2015-12-31
     * @see none
     * @objective Test 'sendRequest' positively
     * @target public void sendRequest
     * @test_data MessageType Method MessageId token IP Port uriPath query PayLoad PayLoadType
     * @pre_condition none
     * @procedure Call sendRequest()
     * @post_condition none
     * @expected sendRequest() will be called without exception
     */

    @Test
    public void sendRequestWithMessageTypeMethodMessageIDTokenIPPortUriPathQueryPayloadPayloadType_P() {
	try {
	    oicCoapClient.sendRequest(OICHelper.MessageType.NON, OICRequestData.Method.GET,
		OICHelper.getRandomMessageId(),
		token, ipv4, port, uriPath, query, OICHelper.DEFAULT_PAYLOAD, OCPayloadType.PAYLOAD_TYPE_DISCOVERY);
	    oicCoapClient.sendRequest(OICHelper.MessageType.NON, OICRequestData.Method.POST,
		OICHelper.getRandomMessageId(),
		token, ipv4, port, uriPath, query, OICHelper.DEFAULT_PAYLOAD, OCPayloadType.PAYLOAD_TYPE_DISCOVERY);
	    oicCoapClient.sendRequest(OICHelper.MessageType.NON, OICRequestData.Method.PUT,
		OICHelper.getRandomMessageId(),
		token, ipv4, port, uriPath, query, OICHelper.DEFAULT_PAYLOAD, OCPayloadType.PAYLOAD_TYPE_DISCOVERY);
	    oicCoapClient.sendRequest(OICHelper.MessageType.CON, OICRequestData.Method.GET,
		OICHelper.getRandomMessageId(),
		token, ipv4, port, uriPath, query, OICHelper.DEFAULT_PAYLOAD, OCPayloadType.PAYLOAD_TYPE_DISCOVERY);
	    oicCoapClient.sendRequest(OICHelper.MessageType.CON, OICRequestData.Method.POST,
		OICHelper.getRandomMessageId(),
		token, ipv4, port, uriPath, query, OICHelper.DEFAULT_PAYLOAD, OCPayloadType.PAYLOAD_TYPE_DISCOVERY);
	   oicCoapClient.sendRequest(OICHelper.MessageType.CON, OICRequestData.Method.PUT,
		OICHelper.getRandomMessageId(),
		token, ipv4, port, uriPath, query, OICHelper.DEFAULT_PAYLOAD, OCPayloadType.PAYLOAD_TYPE_DISCOVERY);
	    } catch (Exception ex) {
		fail("fail to call sendRequest method without exception");
	}
    }

    /**
     * @author Md Golam Sarwar(sarwar.mgm@samsung.com), last updated: 2015-12-31
     * @reviewer
     * @since 2015-12-31
     * @see none
     * @objective Test 'observeResource' positively
     * @target public void observeResource
     * @test_data MessageType MessageId token IP Port uriPath query 
     * @pre_condition none
     * @procedure Call observeResource()
     * @post_condition none
     * @expected observeResource() will be called without exception 
     */  

   @Test
    public void observeResourceWithMessageTypeMessageIDTokenIPPortUriPathQuery_P() {
	try {
	    oicCoapClient.observeResource(OICHelper.MessageType.NON,
		OICHelper.getRandomMessageId(), token, ipv4, port, uriPath, query);
	    /*oicCoapClient.sendRequest(OICHelper.MessageType.NON, OICRequestData.Method.POST,
		OICHelper.getRandomMessageId(),
		token, ipv4, port, uriPath, query, OICHelper.DEFAULT_PAYLOAD, OCPayloadType.PAYLOAD_TYPE_DISCOVERY);
	    oicCoapClient.sendRequest(OICHelper.MessageType.NON, OICRequestData.Method.PUT,
		OICHelper.getRandomMessageId(),
		token, ipv4, port, uriPath, query, OICHelper.DEFAULT_PAYLOAD, OCPayloadType.PAYLOAD_TYPE_DISCOVERY);
	    oicCoapClient.sendRequest(OICHelper.MessageType.CON, OICRequestData.Method.GET,
		OICHelper.getRandomMessageId(),
		token, ipv4, port, uriPath, query, OICHelper.DEFAULT_PAYLOAD, OCPayloadType.PAYLOAD_TYPE_DISCOVERY);
	    oicCoapClient.sendRequest(OICHelper.MessageType.CON, OICRequestData.Method.POST,
		OICHelper.getRandomMessageId(),
		token, ipv4, port, uriPath, query, OICHelper.DEFAULT_PAYLOAD, OCPayloadType.PAYLOAD_TYPE_DISCOVERY);
	   oicCoapClient.sendRequest(OICHelper.MessageType.CON, OICRequestData.Method.PUT,
		OICHelper.getRandomMessageId(),
		token, ipv4, port, uriPath, query, OICHelper.DEFAULT_PAYLOAD, OCPayloadType.PAYLOAD_TYPE_DISCOVERY);*/
	    } catch (Exception ex) {
		fail("fail to call sendRequest method without exception");
	}
    }


}
