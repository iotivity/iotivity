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
package oic.ctt.formatter.test;

import static org.junit.Assert.*;

import java.io.File;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Arrays;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Rule;
import org.junit.Test;

import oic.ctt.formatter.JsonAnalyzer;
import oic.ctt.testhelper.*;

public class JsonAnalyzerTest extends ConformanceTestHelper {

    @Rule
    public ReportGenerator          reportGenerator = new ReportGenerator();

    private JsonAnalyzer            jsonAnalyzer;
    private ArrayList<String>       valuseList;
    private final ArrayList<String> tempValueList   = new ArrayList<String>(
                                                            Arrays.asList(new String[] {
            "/device/light-1", "/device/fan-1", "/device/fan-invisible",
            "/device/light-invisible"
//            , "/device/light-visible" 
            }));
    private final ArrayList<String> finalValueList   = new ArrayList<String>();

    @BeforeClass
    public static void setUpBeforeClass() throws Exception {
    }

    @AfterClass
    public static void tearDownAfterClass() throws Exception {
    }

    @Before
    public void setUp() throws Exception {
        //= this.getClass().getResourceAsStream("/"+dutPath);
        File file = new File(getClass().getResource("/"+dutPath).toURI());
        System.out.println(file.getAbsolutePath().toString());
        jsonAnalyzer = new JsonAnalyzer(file);
        valuseList = null;
    }

    @After
    public void tearDown() throws Exception {
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-19
     * @reviewer
     * @since 2015-08-19
     * @see none
     * @objective Test 'getValue' positively
     * @target public ArrayList<String> getValue(String key)
     * @test_data oc
     * @pre_condition none
     * @procedure Call getValue()
     * @post_condition none
     * @expected getValue() will be called without exception and return string
     *           is equal to matchstring
     */

    @Test
    public void getValue_P() {
        try {
            valuseList = jsonAnalyzer.getValue("oc");
            System.out.println(valuseList.toString());
            assertEquals(getMatchString(matchStringFilePath), valuseList.toString());
        } catch (Exception ex) {
            fail("failed to call loadDutResources method with the following exception :"
                    + ex.getMessage());
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-19
     * @reviewer
     * @since 2015-08-19
     * @see none
     * @objective Test 'getValue' negatively
     * @target public ArrayList<String> getValue(String key)
     * @test_data invalidKey
     * @pre_condition none
     * @procedure Call getValue()
     * @post_condition none
     * @expected getValue() will be called without exception and return a null
     *           list
     */

    @Test
    public void getValue_N() {
        try {
            valuseList = jsonAnalyzer.getValue("invalidKey");
            assertEquals(0, valuseList.size());
        } catch (Exception ex) {
            fail("failed to call loadDutResources method with the following exception :"
                    + ex.getMessage());
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-19
     * @reviewer
     * @since 2015-08-19
     * @see none
     * @objective Test 'getResourceJson' positively
     * @target public ArrayList<String> getResourceJson
     * @test_data oc
     * @pre_condition none
     * @procedure Call getResourceJson()
     * @post_condition none
     * @expected getResourceJson() will be called without exception and no of
     *           resources will be 5
     */

    @Test
    public void getResourceJson_P() {
        try {
            valuseList = jsonAnalyzer.getResourceJson();
            assertEquals(4, valuseList.size());
        } catch (Exception ex) {
            fail("failed to call loadDutResources method with the following exception :"
                    + ex.getMessage());
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-19
     * @reviewer
     * @since 2015-08-19
     * @see none
     * @objective Test 'getJsonString' positively
     * @target public void String getJsonString()
     * @test_data oc
     * @pre_condition none
     * @procedure Call getJsonString()
     * @post_condition none
     * @expected getResourceJson() will be called without exception and return
     *           string is equal to matching string
     */

    @Test
    public void getJsonString_P() {
        try {
            System.out.println(jsonAnalyzer.getJsonString());
            assertEquals(
                    "Returning string does not match with matching string",
                    getMatchString(matchingStringforJsonFile),
                    jsonAnalyzer.getJsonString());
        } catch (Exception ex) {
            fail("failed to call getResourceJson method with the following exception :"
                    + ex.getMessage());
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-19
     * @reviewer
     * @since 2015-08-19
     * @see none
     * @objective Test 'getJsonString' positively
     * @target public void String getJsonString()
     * @test_data oc
     * @pre_condition none
     * @procedure Call getJsonString()
     * @post_condition none
     * @expected getResourceJson() will be called without exception and return
     *           true
     */

    @Test
    public void setJsonString_P() {
        try {//getClass().getResource("/"+dutPath).toURI()
            assertTrue(
                    "Invalid Json string",
                    jsonAnalyzer
                            .setJsonString(getMatchString(matchingStringforJsonFile)));
        } catch (Exception ex) {
            fail("failed to call getResourceJson method with the following exception :"
                    + ex.getMessage());
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-19
     * @reviewer
     * @since 2015-08-19
     * @see none
     * @objective Test 'setJsonString' negatively
     * @target public boolean setJsonString()
     * @test_data invalidJsonString
     * @pre_condition none
     * @procedure Call getJsonString()
     * @post_condition none
     * @expected getResourceJson() will be called without exception and return
     *           false
     */

    @Test
    public void setJsonString_N() {
        try {
            assertFalse("Should return false as input is invalid Json strig",
                    jsonAnalyzer.setJsonString("invalidJsonString"));
        } catch (Exception ex) {
            fail("failed to call getResourceJson method with the following exception :"
                    + ex.getMessage());
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-19
     * @reviewer
     * @since 2015-08-19
     * @see none
     * @objective Test 'getJsonString' positively
     * @target public void String getJsonString()
     * @test_data oc
     * @pre_condition none
     * @procedure Call getJsonString()
     * @post_condition none
     * @expected getResourceJson() will be called without exception and return
     *           true
     */

    @Test
    public void getSingleResourceJsonNode_P() {
        try {
            assertNotNull(jsonAnalyzer.getSingleResourceJsonNode("href",
                    getMatchString(matchingStringforJsonFile)));
        } catch (Exception ex) {
            fail("failed to call getResourceJson method with the following exception :"
                    + ex.getMessage());
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-19
     * @reviewer
     * @since 2015-08-19
     * @see none
     * @objective Test 'getSingleResourceJsonNode' positively
     * @target public String getSingleResourceJsonNode()
     * @test_data invalidValue
     * @pre_condition none
     * @procedure Call getSingleResourceJsonNode()
     * @post_condition none
     * @expected getSingleResourceJsonNode() will be called without exception
     *           and return null
     */

    @Test
    public void getSingleResourceJsonNode_N() {
        try {
            assertNull(jsonAnalyzer.getSingleResourceJsonNode("invalidValue",
                    getMatchString(dutPath)));
        } catch (Exception ex) {
            fail("failed to call getResourceJson method with the following exception :"
                    + ex.getMessage());
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-19
     * @reviewer
     * @since 2015-08-19
     * @see none
     * @objective Test 'getAllValueFromJson' positively
     * @target public void String getJsonString()
     * @test_data href
     * @pre_condition none
     * @procedure Call getAllValueFromJson()
     * @post_condition none
     * @expected getAllValueFromJson() will be called without exception and get
     *           all href resources
     */

    @Test
    public void getAllValueFromJson_P() {
        try {
        	System.out.println(" JSON String = : " + jsonAnalyzer.getJsonString());
             valuseList = jsonAnalyzer.getAllValueFromJson("di");
            System.out.println("Value List = " + valuseList.toString());
            assertEquals(finalValueList.toString(), valuseList.toString());
        } catch (Exception ex) {
            fail("failed to call getAllValueFromJson method with the following exception :"
                    + ex.getMessage());
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-19
     * @reviewer
     * @since 2015-08-19
     * @see none
     * @objective Test 'getAllValueFromJson' positively
     * @target public void String getJsonString()
     * @test_data invalidKey
     * @pre_condition none
     * @procedure Call getAllValueFromJson()
     * @post_condition none
     * @expected getAllValueFromJson() will be called without exception and get
     *           all href resources
     */

    @Test
    public void getAllValueFromJson_N() {
        try {
            valuseList = jsonAnalyzer.getAllValueFromJson("invalidKey");
            assertNotEquals(tempValueList, valuseList);
        } catch (Exception ex) {
            fail("failed to call getAllValueFromJson method with the following exception :"
                    + ex.getMessage());
        }
    }

    /**
     * @author SK Tuhin Islam(sk.islam@samsung.com), last updated: 2015-08-19
     * @reviewer
     * @since 2015-08-19
     * @see none
     * @objective Test 'getAllValueFromJson' positively
     * @target public void String getJsonString()
     * @test_data href
     * @pre_condition none
     * @procedure Call getAllValueFromJson()
     * @post_condition none
     * @expected getAllValueFromJson() will be called without exception and get
     *           all href resources
     */

    @Test
    public void getValueWithHref_P() {
        try {
            valuseList = jsonAnalyzer.getValueWithHref("href", "oc");
            System.out.println(jsonAnalyzer.getValueWithHref("/device/light-1",
                    "oc"));
            // assertEquals(tempValueList, valuseList);
        } catch (Exception ex) {
            fail("failed to call getAllValueFromJson method with the following exception :"
                    + ex.getMessage());
        }
    }
}
