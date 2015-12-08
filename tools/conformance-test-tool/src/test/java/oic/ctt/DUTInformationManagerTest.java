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

package oic.ctt;

import static org.junit.Assert.*;
import oic.ctt.DUTInformationManager;
import oic.ctt.testhelper.*;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Rule;
import org.junit.Test;

public class DUTInformationManagerTest extends ConformanceTestHelper {

    @Rule
    public ReportGenerator        reportGenerator       = new ReportGenerator();

    private DUTInformationManager dutInformationManager = null;

    @BeforeClass
    public static void setUpBeforeClass() throws Exception {
    }

    @AfterClass
    public static void tearDownAfterClass() throws Exception {
    }

    @Before
    public void setUp() throws Exception {
        dutInformationManager = DUTInformationManager.getInstance();
    }

    @After
    public void tearDown() throws Exception {
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-18
     * @reviewer
     * @since 2015-08-18
     * @see none
     * @objective Test 'createDUTInformation' positively
     * @target public boolean createDUTInformation(String dutID, String dutPath)
     * @test_data dutID dutPath
     * @pre_condition none
     * @procedure Call createDUTInformation()
     * @post_condition none
     * @expected createDUTInformation() will be called without exception and
     *           return true
     */

    @Test
    public void createDUTInformation_P() {
        try {
            assertTrue("Failed to create DUTY Information",
                    dutInformationManager.createDUTInformation(dutID, dutPath));
        } catch (Exception ex) {
            fail("failed to call createDUTInformation method with the following exception :"
                    + ex.getMessage());
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-18
     * @reviewer
     * @since 2015-08-18
     * @see none
     * @objective Test 'createDUTInformation' negatively
     * @target public boolean createDUTInformation(String dutID, String dutPath)
     * @test_data invalidDUTID invalidDUTPath
     * @test_data invalidDUTID dutPath
     * @test_data dutID invalidDUTPath
     * @test_data null null
     * @pre_condition none
     * @procedure Call createDUTInformation()
     * @post_condition none
     * @expected createDUTInformation() will be called without exception and
     *           return false
     */

    @Test
    public void createDUTInformation_N() {
        try {
            assertFalse("Failed to create DUTY Information",
                    dutInformationManager.createDUTInformation(invalidDUTID,
                            invalidDUTPath));
            assertFalse("Failed to create DUTY Information",
                    dutInformationManager.createDUTInformation(null, null));
        } catch (Exception ex) {
            fail("failed to call createDUTInformation method with the following exception :"
                    + ex.getMessage());
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-18
     * @reviewer
     * @since 2015-08-18
     * @see none
     * @objective Test 'getDUTInformation' positively
     * @target public boolean getDUTInformation(String dutID)
     * @test_data dutID
     * @pre_condition none
     * @procedure Call getDUTInformation()
     * @post_condition none
     * @expected getDUTInformation() will be called without exception
     */

    @Test
    public void getDUTInformation_P() {
        try {
            dutInformationManager.getDUTInformation(dutID);
        } catch (Exception ex) {
            fail("failed to call getDUTInformation method with the following exception :"
                    + ex.getMessage());
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-18
     * @reviewer
     * @since 2015-08-18
     * @see none
     * @objective Test 'removeDUTInformation' positively
     * @target public void removeDUTInformation(String dutID)
     * @test_data dutID
     * @pre_condition none
     * @procedure Call getDUTInformation()
     * @post_condition none
     * @expected getDUTInformation() will be called without exception
     */

    @Test
    public void removeDUTInformation_P() {
        try {
            dutInformationManager.removeDUTInformation(dutID);
        } catch (Exception ex) {
            fail("failed to call removeDUTInformation method with the following exception :"
                    + ex.getMessage());
        }
    }
}
