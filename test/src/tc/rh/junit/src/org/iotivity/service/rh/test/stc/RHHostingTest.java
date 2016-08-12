/******************************************************************
*
* Copyright 2016 Samsung Electronics All Rights Reserved.
*
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

package org.iotivity.service.rh.test.stc;

import org.iotivity.ResourceHosting.ResourceHosting;
import org.iotivity.base.ModeType;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;
import org.iotivity.service.rh.test.helper.RHHelper;    
 
import static org.iotivity.service.rh.test.helper.RHHelper.RequestType;
import android.test.InstrumentationTestCase;
import android.util.Log;

public class RHHostingTest extends InstrumentationTestCase {
  RHHelper	m_RHHelper;
  
  protected void setUp() throws Exception {
		super.setUp();
		
		m_RHHelper = new RHHelper();
	    PlatformConfig cfg = new PlatformConfig(getInstrumentation().getTargetContext(), ServiceType.IN_PROC, ModeType.CLIENT, "0.0.0.0",0, QualityOfService.LOW);
		OcPlatform.Configure(cfg);
	}

	protected void tearDown() throws Exception {
		super.tearDown();
		m_RHHelper = null;
	}
 
  /**
   * @since 			2015-04-29
   * @see 				none
   * @objective 		test if resource host can host resource
   * @target 			OcPlatform.findResource()
   * @test_data 		none
   * @pre_condition 	1. Run resource 
   * 					2. Run resource host 
   * 					3. Configure OCPlatform
   * @procedure 		Call findResource API to check if resource is found
   * @post_condition 	stop resource & resource host
   * @expected 			resource should be found
   */
  public void testFindHostableResource_SQV_P() {
	  
	try
	{
		assertTrue("Resource not found..!",m_RHHelper.findResource("", RHHelper.RH_FIND_RESOURCE_QUERY));
	  
		m_RHHelper.waitInSecond(RHHelper.RH_MAX_WAIT_TIME);
		assertTrue("TempHumSensor is not found..!",m_RHHelper.verifyTempHumFirstSensor());
	}
	catch(Exception ex)
	{
		ex.printStackTrace();
		fail("Exception occurred : " + ex.getLocalizedMessage());
	}
	  
  }

  /**
   * @since 			2015-04-29
   * @see 				none
   * @objective 		test if resource host can host multiple resources
   * @target 			OcPlatform.findResource()
   * @test_data 		none
   * @pre_condition 	1. Run two resources 
   * 					2. Run resource host 
   * 					3. Configure OCPlatform
   * @procedure 		Call findResource API to check if resource is found
   * @post_condition 	stop resource & resource host
   * @expected 			resource should be found
   */
  public void testFindMultipleHostableResource_SQV_P() {
	try
	{
	  assertTrue("Resource not found..!",m_RHHelper.findResource("", RHHelper.RH_FIND_RESOURCE_QUERY));
	  
	  m_RHHelper.waitInSecond(RHHelper.RH_MAX_WAIT_TIME);

	  assertTrue("verifyTempHumFirstSensor is not verified..!",m_RHHelper.verifyTempHumFirstSensor());
	  
	  m_RHHelper.waitInSecond(RHHelper.RH_MAX_WAIT_TIME);
	  
	  assertTrue("verifyTempHumSecondSensor is not verified..!",m_RHHelper.verifyTempHumSecondSensor());
	}
	catch(Exception ex)
	{
		ex.printStackTrace();
		fail("Exception occurred : " + ex.getLocalizedMessage());
	}
  }

  /**
   * @since 			2015-04-29
   * @see 				none
   * @objective 		test if resource host can handle observe request
   * @target 			OcResource.observe(ObserveType, QueryParamsMap, onObsreve);
   * @test_data 		none
   * @pre_condition 	1. Run resource 
   * 					2. Run resource host 
   * 					3. Configure OCPlatform
   * @procedure		 	1. Call find resource 
   * 					2. Check if resource is found 
   * 					3. Send observe request
   * @post_condition 	stop resource & resource host
   * @expected 			Data should come from resource
   */
  public void testSendObserveRequest_SQV_P() {
    
    try
    {
		assertTrue("Resource not found..!",m_RHHelper.findResource("", RHHelper.RH_FIND_RESOURCE_QUERY));
	  
		m_RHHelper.waitForResource(RHHelper.RH_MAX_WAIT_TIME);

		assertTrue("verifyTempHumFirstSensor is not verified..!",m_RHHelper.verifyTempHumFirstSensor());
	  
		assertTrue("Observed resource is null",m_RHHelper.startObserve(RHHelper.m_FirstResource));
	  
		m_RHHelper.waitForCallback(RHHelper.RH_MAX_WAIT_TIME, RequestType.OBSERVE);
	  
		assertTrue("Expected data is miss matched with actual data",m_RHHelper.verifyObserveDataFromFirstSensor());
	}
	catch(Exception ex)
	{
		ex.printStackTrace();
		fail("Exception occurred : " + ex.getLocalizedMessage());
	}
  }

  /**
   * @since 			2015-04-30
   * @see 				none
   * @objective 		test if resource host can handle get request
   * @target 			OcResource.get(ObserveType, QueryParamsMap, onGet);
   * @test_data 		none
   * @pre_condition 	1. Run resource 
   * 					2. Run resource host 
   * 					3. Configure OCPlatform
   * @procedure 		1. Call find resource 
   * 					2. Check if resource is found 
   * 					3. Send get request to resource
   * @post_condition 	stop resource & resource host
   * @expected 			Data should come from resource
   */
  public void testSendGetRequest_SQV_P() {
    
    try
    {
		Boolean isResourceFound  = m_RHHelper.findResource("", RHHelper.RH_FIND_RESOURCE_QUERY);
		Thread.sleep(RHHelper.RH_THREAD_SLEEP_MAX);
		
		assertTrue("Resource not found",isResourceFound);
		  
		m_RHHelper.waitForResource(RHHelper.RH_MAX_WAIT_TIME);
		assertTrue("verifyTempHumFirstSensor is not verified..!",m_RHHelper.verifyTempHumFirstSensor());

		assertTrue("Get resource miss matched",m_RHHelper.startGet(RHHelper.m_FirstResource));
		  
		m_RHHelper.waitForCallback(RHHelper.RH_MAX_WAIT_TIME, RequestType.GET);
		assertTrue("Expected data is miss matched with actual data",m_RHHelper.verifyObserveDataFromFirstSensor());
	}
	catch(Exception ex)
	{
		ex.printStackTrace();
		fail("Exception occurred : " + ex.getLocalizedMessage());
	}
  }
  
  /**
   * @since 			2015-04-30
   * @see 				none
   * @objective 		test if resource host can handle observe request
   * @target 			OcResource.observe(ObserveType, QueryParamsMap, onObsreve);
   * @test_data 		none
   * @pre_condition 	1. Run resource 
   * 					2. Run resource host 
   * 					3. Configure OCPlatform
   * @procedure 		1. Call find resource 
   * 					2. Check if resource is found 
   * 					3. Send observe request to both resources
   * @post_condition 	stop resource & resource host
   * @expected 			Data should come from both resources
   */
  public void testSendObserveRequestToMultipleResource_SQV_P() {
    
    try
    {
		assertTrue("Resource not found..!",m_RHHelper.findResource("", RHHelper.RH_FIND_RESOURCE_QUERY));
		  
		m_RHHelper.waitForResource(RHHelper.RH_MAX_WAIT_TIME);
		assertTrue("verifyTempHumFirstSensor is not verified..!",m_RHHelper.verifyTempHumFirstSensor());
		  
		m_RHHelper.waitForResource(RHHelper.RH_MAX_WAIT_TIME);
		assertTrue("verifyTempHumSecondSensor is not verified..!",m_RHHelper.verifyTempHumSecondSensor());
		  
		assertTrue("Observed first resource is null",m_RHHelper.startObserve(RHHelper.m_FirstResource));
		assertTrue("Observed second resource is null",m_RHHelper.startObserve(RHHelper.m_SecondResource));

		m_RHHelper.waitForResource(RHHelper.RH_MAX_WAIT_TIME);
		assertTrue("Expected data is miss matched with actual data",m_RHHelper.verifyObserveDataFromFirstSensor());
		assertTrue("Expected data is miss matched with actual data",m_RHHelper.verifyObserveDataFromSecondSensor());
	}
	catch(Exception ex)
	{
		ex.printStackTrace();
		fail("Exception occurred : " + ex.getLocalizedMessage());
	}
  }

  /**
   * @since 			2015-04-30
   * @see 				none
   * @objective 		test if resource host can handle put request
   * @target 			OcResource.post(ObserveType, QueryParamsMap, onPost)
   * @test_data 		none
   * @pre_condition 	1. Run resource 
   * 					2. Run resource host 
   * 					3. Configure OCPlatform
   * @procedure 		1. Call find resource 
   * 					2. Check if resource is found 
   * 					3. Send post request to resource 
   * 					4. send get request to resource to verify data
   * @post_condition 	stop resource & resource host
   * @expected 			resource parameter should be change
   */
  public void testSendPostRequest_SQV_P() {
    
    try
    {
	  assertTrue("Resource not found..!",m_RHHelper.findResource("", RHHelper.RH_FIND_RESOURCE_QUERY));
	  
	  m_RHHelper.waitInSecond(RHHelper.RH_MAX_WAIT_TIME);
	  assertTrue("verifyTempHumFirstSensor is not verified..!",m_RHHelper.verifyTempHumFirstSensor());
	  
	  m_RHHelper.waitInSecond(RHHelper.RH_MAX_WAIT_TIME);
	  assertTrue("Post requested resource is null..",m_RHHelper.startPost(RHHelper.m_FirstResource));
	  
	  m_RHHelper.waitForCallback(RHHelper.RH_MAX_WAIT_TIME, RequestType.POST);
	  assertTrue("Get requested resource is null..",m_RHHelper.startGet(RHHelper.m_FirstResource));
	  
      m_RHHelper.waitForCallback(RHHelper.RH_MAX_WAIT_TIME, RequestType.GET);
      assertTrue("Expected data is miss matched with actual data",m_RHHelper.verifyObserveDataFromFirstSensor(m_RHHelper.m_PostTemp,m_RHHelper.m_PostHum));
	}
	catch(Exception ex)
	{
		ex.printStackTrace();
		fail("Exception occurred : " + ex.getLocalizedMessage());
	}
  }

  /**
   * @since 			2015-04-30
   * @see 				none
   * @objective 		test if resource host can handle put request to multiple resources
   * @target 			OcResource.post(ObserveType, QueryParamsMap, onPost)
   * @test_data 		none
   * @pre_condition 	1. Run two resource 
   * 					2. Run resource host 
   * 					3. Configure OCPlatform
   * @procedure 		1. Call find resource 
   * 					2. Check if resources are found 
   * 					3. Send post request to both resource 4. Send get request 
   * 					   to both resource to verify data
   * @post_condition	stop resource & resource host
   * @expected 			Resource parameter should be change for both resource
   */
  public void testSendPostRequestToMultipleResource_SQV_P() {
    
    try
    {
	  Boolean isResourceFound  = m_RHHelper.findResource("", RHHelper.RH_FIND_RESOURCE_QUERY);
	  Thread.sleep(RHHelper.RH_THREAD_SLEEP_MAX);
	  assertTrue("Resource is not found by findResource API",isResourceFound);
	  
	  
	  m_RHHelper.waitInSecond(RHHelper.RH_MAX_WAIT_TIME);
	  assertTrue("verifyTempHumFirstSensor is not verified..!",m_RHHelper.verifyTempHumFirstSensor());
    
	  m_RHHelper.waitForResource(RHHelper.RH_MAX_WAIT_TIME);
	  assertTrue("verifyTempHumSecondSensor is not verified..!",m_RHHelper.verifyTempHumSecondSensor());

	  m_RHHelper.initiateData();
	  assertTrue("Post requested resource is null..",m_RHHelper.startPost(RHHelper.m_FirstResource));
	  assertTrue("Post requested resource is null..",m_RHHelper.startPost(RHHelper.m_SecondResource));

	  m_RHHelper.waitForCallback(RHHelper.RH_MIN_WAIT_TIME, RequestType.POST);
	  assertTrue("Get requested resource is null..",m_RHHelper.startGet(RHHelper.m_FirstResource));
    
	  m_RHHelper.waitInSecond(RHHelper.RH_MIN_WAIT_TIME);
	  assertTrue("Expected data is miss matched with actual data",m_RHHelper.verifyObserveDataFromFirstSensor(m_RHHelper.m_PostTemp,m_RHHelper.m_PostHum));
    
	  m_RHHelper.waitInSecond(RHHelper.RH_MIN_WAIT_TIME);
	  assertTrue("Get requested resource is null..",m_RHHelper.startGet(RHHelper.m_SecondResource));

	  m_RHHelper.waitInSecond(RHHelper.RH_MIN_WAIT_TIME);
	  assertTrue("Expected data is miss matched with actual data",m_RHHelper.verifyObserveDataFromSecondSensor(m_RHHelper.m_PostTemp,m_RHHelper.m_PostHum));
	}
	catch(Exception ex)
	{
		ex.printStackTrace();
		fail("Exception occurred : " + ex.getLocalizedMessage());
	}
  }

  /**
   * @since 			2015-04-30
   * @see 				none
   * @objective 		test if resource host do not host non hostable resource
   * @target 			OcResource.findResource()
   * @test_data 		none
   * @pre_condition 	1. Run non hostable resource 
   * 					2. Run resource host 
   * 					3. Configure OCPlatform
   * @procedure 		1. Call find resource 
   * 					2. Check if non hostable resource is found
   * @post_condition 	stop resource & resource host
   * @expected 			non hostable resource should not be found
   */
   
  public void testRHDoNotHostNonHostableResource_DICC_N() {
    
    try
    {
		Boolean isResourceFound  = m_RHHelper.findResource("", RHHelper.RH_FIND_RESOURCE_QUERY);
		Thread.sleep(RHHelper.RH_THREAD_SLEEP_MAX);
		assertTrue("Resource is not found by findResource API",isResourceFound);
		
		Boolean isSensorVarified = m_RHHelper.verifySensor(RHHelper.m_curResourceNonHostable,RHHelper.RH_NON_HOSTABLE_RESOURCE_URI);
		Thread.sleep(RHHelper.RH_THREAD_SLEEP_MAX);
		assertFalse("Non Hostable resource is hosted",isSensorVarified);
	
	}
	catch(Exception ex)
	{
		ex.printStackTrace();
		fail("Exception occurred : " + ex.getLocalizedMessage());
	}
  }
  
}
