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

package oic.ctt.network.test;

import static org.junit.Assert.*;
import oic.ctt.network.OICObserver;
import oic.ctt.testhelper.*;

import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.mockito.Mockito;
import org.mockito.internal.util.reflection.Whitebox;
import org.ws4d.coap.interfaces.CoapRequest;
import org.ws4d.coap.interfaces.CoapServerChannel;

public class OICObserverTest {

    @Rule
    public ReportGenerator reportGenerator = new ReportGenerator();
    private OICObserver    oicObserver;
    private CoapRequest    coapRequest;

    @Before
    public void setUp() throws Exception {
        oicObserver = new OICObserver();
        coapRequest = Mockito.mock(CoapRequest.class);
        Whitebox.setInternalState(oicObserver, "mCoapRequest", coapRequest);
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-25
     * @reviewer
     * @since 2015-08-25
     * @see none
     * @objective Test 'setCoapServerChannel' positively
     * @target public void setCoapServerChannel(CoapServerChannel
     *         mCoapServerChannel)
     * @test_data coapServerChannel
     * @pre_condition none
     * @procedure Call setCoapServerChannel()
     * @post_condition none
     * @expected setCoapServerChannel() will be called without exception
     */

    @Test
    public void setCoapServerChannel_P() {
        CoapServerChannel coapServerChannel = Mockito
                .mock(CoapServerChannel.class);
        try {
            oicObserver.setCoapServerChannel(coapServerChannel);
        } catch (Exception ex) {
            fail("failed to call setCoapServerChannel method without exception :"
                    + ex.getMessage());
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-25
     * @reviewer
     * @since 2015-08-25
     * @see none
     * @objective Test 'setCoapServerChannel' negatively
     * @target public void setCoapServerChannel(CoapServerChannel
     *         mCoapServerChannel)
     * @test_data null
     * @pre_condition none
     * @procedure Call setCoapServerChannel()
     * @post_condition none
     * @expected setCoapServerChannel() will be called without exception
     */

    @Test
    public void setCoapServerChannel_N() {
        try {
            oicObserver.setCoapServerChannel(null);
        } catch (Exception ex) {
            fail("failed to call setCoapServerChannel method without exception :"
                    + ex.getMessage());
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-25
     * @reviewer
     * @since 2015-08-25
     * @see none
     * @objective Test 'getCoapServerChannel' positively
     * @target public void setCoapServerChannel(CoapServerChannel
     *         mCoapServerChannel)
     * @test_data getCoapServerChannel
     * @pre_condition none
     * @procedure Call getCoapServerChannel()
     * @post_condition none
     * @expected getCoapServerChannel() will be called without exception
     */

    @Test
    public void getCoapServerChannel_P() {
        try {
            oicObserver.getCoapServerChannel();
        } catch (Exception ex) {
            fail("failed to call setCoapServerChannel method without exception :"
                    + ex.getMessage());
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-25
     * @reviewer
     * @since 2015-08-25
     * @see none
     * @objective Test 'getCoapServerChannel' positively
     * @target public void setCoapServerChannel(CoapServerChannel
     *         mCoapServerChannel)
     * @test_data getCoapServerChannel
     * @pre_condition Mock mCoapRequest object of OICObserver class with
     *                coapRequest mock object
     * @procedure Call getCoapServerChannel()
     * @post_condition none
     * @expected getCoapServerChannel() will be called without exception
     */

    @Test
    public void getObserverIP_P() {
        Mockito.when(coapRequest.getSourceIP()).thenReturn("107/109/7/114");
        try {
            assertEquals("109", oicObserver.getObserverIP());
        } catch (Exception ex) {
            fail("failed to call setCoapServerChannel method without exception :"
                    + ex.getMessage());
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-25
     * @reviewer
     * @since 2015-08-25
     * @see none
     * @objective Test 'getObserverPort' positively
     * @target public int getObserverPort()
     * @test_data none
     * @pre_condition none
     * @procedure Call getObserverPort()
     * @post_condition none
     * @expected getObserverPort() will be called without exception
     */

    @Test
    public void getObserverPort_P() {
        try {
            assertEquals(0, oicObserver.getObserverPort());

        } catch (Exception ex) {
            fail("failed to call getObserverPort method without exception :"
                    + ex.getMessage());
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-25
     * @reviewer
     * @since 2015-08-25
     * @see none
     * @objective Test 'getSeqNumber' positively
     * @target public int getSeqNumber()
     * @test_data none
     * @pre_condition none
     * @procedure Call getSeqNumber()
     * @post_condition none
     * @expected getSeqNumber() will be called without exception and sequence
     *           number is equal to 5
     */

    @Test
    public void getSeqNumber_P() {
        try {
            assertEquals("Fail to get sequence number", 5,
                    oicObserver.getSeqNumber());
        } catch (Exception ex) {
            fail("failed to call getSeqNumber method without exception :"
                    + ex.getMessage());
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-25
     * @reviewer
     * @since 2015-08-25
     * @see none
     * @objective Test 'setSeqNumber' positively
     * @target public void setSeqNumber()
     * @test_data none
     * @pre_condition none
     * @procedure Call setSeqNumber()
     * @post_condition none
     * @expected setSeqNumber() will be called without exception
     */

    @Test
    public void setSeqNumber_P() {
        try {
            oicObserver.setSeqNumber(7);
        } catch (Exception ex) {
            fail("failed to call setSeqNumber method without exception :"
                    + ex.getMessage());
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-25
     * @reviewer
     * @since 2015-08-25
     * @see none
     * @objective Test 'getCoapRequest' positively
     * @target public CoapRequest getCoapRequest()
     * @test_data none
     * @pre_condition none
     * @procedure Call getCoapRequest()
     * @post_condition none
     * @expected getCoapRequest() will be called without exception
     */

    @Test
    public void getCoapRequest_P() {
        try {
            assertNotNull("Fail to get sequence number",
                    oicObserver.getCoapRequest());
        } catch (Exception ex) {
            fail("failed to call getCoapRequest method without exception :"
                    + ex.getMessage());
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-25
     * @reviewer
     * @since 2015-08-25
     * @see none
     * @objective Test 'getReliablity' positively
     * @target public boolean getReliablity()
     * @test_data none
     * @pre_condition none
     * @procedure Call getReliablity()
     * @post_condition none
     * @expected getReliablity() will be called without exception
     */

    @Test
    public void getReliablity_P() {
        try {
            assertEquals(false, oicObserver.getReliablity());
        } catch (Exception ex) {
            fail("failed to call getReliablity method without exception :"
                    + ex.getMessage());
        }
    }

}
