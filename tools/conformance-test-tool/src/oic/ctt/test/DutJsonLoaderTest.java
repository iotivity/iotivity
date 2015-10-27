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

package oic.ctt.test;

import static org.junit.Assert.*;

import java.util.HashMap;

import oic.ctt.DUTResource;
import oic.ctt.DutJsonLoader;
import oic.ctt.IDutLoader;
import oic.ctt.testhelper.*;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Rule;
import org.junit.Test;

public class DutJsonLoaderTest extends ConformanceTestHelper {

    @Rule
    public ReportGenerator               reportGenerator = new ReportGenerator();

    private IDutLoader                   iDutLoader      = null;
    private HashMap<String, DUTResource> mapDUTResource  = null;

    @BeforeClass
    public static void setUpBeforeClass() throws Exception {
    }

    @AfterClass
    public static void tearDownAfterClass() throws Exception {
    }

    @Before
    public void setUp() throws Exception {
        iDutLoader = new DutJsonLoader(dutPath);
        mapDUTResource = null;
    }

    @After
    public void tearDown() throws Exception {
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-18
     * @reviewer
     * @since 2015-08-18
     * @see none
     * @objective Test 'loadDutResources' positively
     * @target public HashMap<String, DUTResource> loadDutResources()
     * @test_data none
     * @pre_condition none
     * @procedure Call loadDutResources()
     * @post_condition none
     * @expected loadDutResources() will be called without exception and size
     *           resource will be 4
     */

    @Test
    public void loadDutResources_P() {
        try {
            mapDUTResource = iDutLoader.loadDutResources();
            assertEquals("Failed to get all resources ", 4,
                    mapDUTResource.size());
        } catch (Exception ex) {
            fail("failed to call loadDutResources method with the following exception :"
                    + ex.getMessage());
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-18
     * @reviewer
     * @since 2015-08-18
     * @see none
     * @objective Test 'loadDUTId' positively
     * @target public String loadDUTId()
     * @test_data none
     * @pre_condition none
     * @procedure Call loadDUTId()
     * @post_condition none
     * @expected loadDUTId() will be called without exception and get proper
     *           DUTID
     */

    @Test
    public void loadDUTId_P() {
        try {
            assertEquals("Failed to get or invalid DUTID ", dutID,
                    iDutLoader.loadDUTId());
        } catch (Exception ex) {
            fail("failed to call loadDUTId method with the following exception :"
                    + ex.getMessage());
        }
    }

}
