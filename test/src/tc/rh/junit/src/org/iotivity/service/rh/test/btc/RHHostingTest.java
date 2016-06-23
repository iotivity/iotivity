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

package org.iotivity.service.rh.test.btc;

import org.iotivity.service.rh.test.helper.*;
import org.iotivity.base.ModeType;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;

import org.iotivity.ResourceHosting.ResourceHosting;

import android.test.InstrumentationTestCase;

public class RHHostingTest extends InstrumentationTestCase {
  
  RHHelper	m_RHHelper;
  
  protected void setUp() throws Exception {
		super.setUp();
		
		m_RHHelper = new RHHelper();
	    PlatformConfig cfg = new PlatformConfig(getInstrumentation().getTargetContext(),ServiceType.IN_PROC, ModeType.CLIENT_SERVER,"0.0.0.0", 0, QualityOfService.LOW);
		OcPlatform.Configure(cfg);
	}

	protected void tearDown() throws Exception {
		super.tearDown();
		m_RHHelper = null;
	}

  /**
   * @since 			2015-04-20
   * @see 				none
   * @objective 		test OICStartCoordinate API Possitively
   * @target 			void OICStartCoordinate();
   * @test_data 		none
   * @pre_condition 	none
   * @procedure 		Call OICStartCoordinate API
   * @post_condition 	stop coordinate
   * @expected 			API should return OCSTACK_OK 
   */
  public void testOICCoordinatorStart_SRC_P() {
	  try
	  {
		  assertEquals("OICStartCoordinate failed.", RHHelper.OCSTACK_OK, m_RHHelper.coordinatorStart());
		  m_RHHelper.coordinatorStop();
	  }
	  catch(Exception ex)
	  {
		  ex.printStackTrace();
		  fail("Exception occurred: " + ex.getLocalizedMessage());
	  }
  }

  /**
   * @since 			2015-04-20
   * @see 				none
   * @objective 		test OICStopCoordinate API positively
   * @target 			void OICStopCoordinate();
   * @test_data 		none
   * @pre_condition 	start coordinate
   * @procedure 		Call coordinatorStop and check return value
   * @post_condition 	none
   * @expected 			API should return OCSTACK_OK
   */
  public void testOICCoordinatorStop_SRC_P() {
	  try
	  {
		  m_RHHelper.coordinatorStart();
		  assertEquals("OICCoordinatorStop failed.", RHHelper.OCSTACK_OK, m_RHHelper.coordinatorStop());
	  }
	  catch(Exception ex)
	  {
		  ex.printStackTrace();
		  fail("Exception occurred : " + ex.getLocalizedMessage());

	  }
  }

/**
   * @since 			2015-04-20
   * @see 				none
   * @objective 		test OICStopCoordinate API without initialize and Start cordinate 
   * 					that should return OCSTACK_ERROR(255)
   * @target 			void OICStopCoordinate();
   * @test_data 		none
   * @pre_condition 	start coordinate
   * @procedure 		call coordinatorStop
   * @post_condition 	none
   * @expected 			API should not return OCSTACK_OK as we not initialized and didn't start yet
   */
  public void testOICCoordinatorStopWithoutStartCoordinate_ECRC_N() {
	  try
	  {
		  assertEquals("OICCoordinatorStop failed.", RHHelper.RESOURCEHOSTING_DO_NOT_THREADRUNNING, m_RHHelper.coordinatorStop());
	  }
	  catch(Exception ex)
	  {
		  ex.printStackTrace();
		  fail("Exception occurred : " + ex.getLocalizedMessage());
	  }
  }
  
  
  /**
   * @since 			2016-02-22
   * @see 				none
   * @objective 		test ResourceHostingInit API positive way
   * @target 			void ResourceHostingInit();
   * @test_data 		valid IpAddress
   * @pre_condition 	none
   * @procedure 		Perform ResourceHostingInit API
   * @post_condition 	Perform ResourceHostingTerminate()
   * @expected 			ResourceHostingInit API will success
   */
  public void testOICResourceHostingInit_SRC_P() {
	  try
	  {
		  assertEquals("Initialization failed!", RHHelper.OCSTACK_OK, m_RHHelper.initOICStack());
		  m_RHHelper.resourceHostingTerminate();
	  }
	  catch(Exception ex)
	  {
			ex.printStackTrace();
			fail("Exception occurred due to init OIC Resource Hosting: " + ex.getLocalizedMessage());
	  }    
  }

  /**
   * @since 			2016-02-23
   * @see 				none
   * @objective 		test OICResourceHostingInitialization API with empty address
   * @target 			void ResourceHostingInit();
   * @test_data 		Empty string as Value
   * @pre_condition 	none
   * @procedure 		Call ResourceHostingInit API
   * @post_condition 	Perform ResourceHostingTerminate()
   * @expected 			ResourceHostingInit API will success
   */
  public void testOICResourceHostingInitwith_ESV_N() {

	  try
	  {
		  ResourceHosting resourceHosting = new ResourceHosting();

		  assertEquals("Initialization should failed with empty adress value !", RHHelper.OCSTACK_OK, resourceHosting.ResourceHostingInit(RHHelper.RESOURCEHOSTING_EMPTY_STRING));
		  resourceHosting.ResourceHostingTerminate();
	  }
	  catch(Exception ex)
	  {
		  ex.printStackTrace();
		  fail("Exception occurred due to init OIC Resource Hosting: " + ex.getLocalizedMessage());
	  }    
  }

  /**
   * @see 				none
   * @objective 		test OICResourceHostingInitialization API negative way
   * @target 			void ResourceHostingInit();
   * @test_data 		NULL value
   * @pre_condition 	none
   * @procedure 		Call ResourceHostingInit API
   * @post_condition 	Perform ResourceHostingTerminate()
   * @expected 			ResourceHostingInit API will not success and will return OCSTACK_ERROR
   */
  public void testOICResourceHostingInitwith_NV_N() {

	  try
	  {
		  ResourceHosting resourceHosting = new ResourceHosting();

		  assertEquals("Initialization should failed with empty adress value !", RHHelper.OCSTACK_ERROR, resourceHosting.ResourceHostingInit(RHHelper.RESOURCEHOSTING_NULL_VALUE));
		  resourceHosting.ResourceHostingTerminate();
	  }
	  catch(Exception ex)
	  {
		  ex.printStackTrace();
		  fail("Exception occurred due to init OIC Resource Hosting: " + ex.getLocalizedMessage());
	  }    
  }

  /**
   * @since 			2016-02-24
   * @see 				none
   * @objective 		test OICResourceHostingTerminate API without initialize  
   * @target 			void ResourceHostingTerminate();
   * @test_data 		none
   * @pre_condition 	initialization of oc resources
   * @procedure 		Call ResourceHostingTerminate API
   * @post_condition 	none
   * @expected 			API should return OCSTACK_ERROR as we not initialized by ResourceHostingInit
   */
  public void testOICResourceHostingTerminate_SRC_P() {
	  try
	  {
		  m_RHHelper.initOICStack();
		  assertEquals("OICResourceHostingTerminate should not success as without initialization!", RHHelper.OCSTACK_OK, m_RHHelper.resourceHostingTerminate());
	  }
	  catch(Exception ex)
	  {
		  ex.printStackTrace();
		  fail("Exception occurred due to Terminate OIC Resource Hosting: " + ex.getLocalizedMessage());
	  }    
  }
  
  /**
   * @since 			2015-04-20
   * @see 				none
   * @objective 		test OICStartCoordinate API negative way
   * @target 			void OICStartCoordinate()
   * @test_data 		none
   * @pre_condition 	none
   * @procedure 		Call OICStartCoordinate API multiple times
   * @post_condition 	stop co-ordination
   * @expected 			OICStartCoordinate API will success 1st time due to 
   * 					2nd time start it will not success
   */
  public void testStartCoordinateMultipleTimesWithoutInitializingResource_SRCC_N() {
	try
	{
		  assertEquals("coordinatorStart is failed.",RHHelper.OCSTACK_ERROR,m_RHHelper.coordinatorStart());
		  assertEquals("coordinatorStart is failed.",RHHelper.RESOURCEHOSTING_DO_NOT_THREADRUNNING,m_RHHelper.coordinatorStart());
		  m_RHHelper.coordinatorStop();
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
   * @objective 		Call OICStopCoordinate API after successfully calling
   *            		OICStartCoordinate & OICStopCoordinate API once
   * @target 			void OICStopCoordinate()
   * @test_data 		none
   * @pre_condition 	start co-ordination
   * @procedure 		Perform OICStopCoordinate()
   * @post_condition 	none
   * @expected 			Stop API will success 1st time due to 
   * 					2nd time start it will not success
   */
   public void testStopCoordinateMultipleTimesWithoutInitializingResource_SRCC_N() {
	try
	{
		assertEquals("coordinatorStart is failed.",RHHelper.OCSTACK_ERROR,m_RHHelper.coordinatorStart());
		assertEquals("coordinatorStop is failed.",RHHelper.OCSTACK_OK,m_RHHelper.coordinatorStop());
		assertEquals("coordinatorStop Should return RESOURCEHOSTING_DO_NOT_THREADRUNNING due to call multiple times.",RHHelper.RESOURCEHOSTING_DO_NOT_THREADRUNNING,m_RHHelper.coordinatorStop());
	}
	catch(Exception ex)
	{
		ex.printStackTrace();
		fail("Exception occurred : " + ex.getLocalizedMessage());
	}		  
  }  
  
  /**
   * @since 2016-02-26
   * @see none
   * @objective test OICResourceHostingInit() API multiple times.
   * @target void OICResourceHostingInit();
   * @test_data none
   * @pre_condition none
   * @procedure 1. Call OICResourceHostingInit() 2. Repeat step 1~2 ten times.
   * @post_condition Perform ResourceHostingTerminate()
   * @expected OICStartCoordinate and OICStopCoordinate will success each time.
   */
  public void testRHTerminateMultipleTimesWithSingleInit_ECRC_DSCC_N() {
	  try
	  {
		  assertEquals("Initialization failed!", RHHelper.OCSTACK_OK, m_RHHelper.initOICStack());
		  assertEquals("Resource termination failed!", RHHelper.OCSTACK_OK, m_RHHelper.resourceHostingTerminate());
		  assertEquals("Resource termination failed due to perform 2nd time operation!", RHHelper.OCSTACK_ERROR, m_RHHelper.resourceHostingTerminate());
	  }
	  catch(Exception ex)
	  {
		  ex.printStackTrace();
		  fail("Exception occurred : " + ex.getLocalizedMessage());
	  }		  
  }
  
  /**
   * @since 			2015-04-20
   * @see none
   * @objective 		test OICStartCoordinate and OICStopCoordinate API multiple times.
   * @target 			void OICStartCoordinate();
   * @target 			void OICStopCoordinate();
   * @test_data 		none
   * @pre_condition 	Initialize Resource
   * @procedure 		1. Call OICStartCoordinate() 
   * 					2. Call OICStopCoordinate() 
   * 					3. Repeat step 1~2 tem times.
   * @post_condition 	Perform ResourceHostingTerminate()
   * @expected 			OICStartCoordinate and OICStopCoordinate will success each time.
   */
  public void testStartCoordinateAndStopCoorditaneMultipleTimes_VLCC_P() {
    try
    {
	  assertEquals("Initialization failed!", RHHelper.OCSTACK_OK, m_RHHelper.initOICStack());
	  for (int i = RHHelper.RH_INITIALIZE_INT; i < RHHelper.RH_LOOP_COUNT_VAL; i++) 
	  {
		  assertEquals("coordinatorStart is failed.",RHHelper.OCSTACK_OK,m_RHHelper.coordinatorStart());
		  m_RHHelper.waitForResource(RHHelper.RH_MIN_WAIT_TIME);
		  assertEquals("coordinatorStop is failed.",RHHelper.OCSTACK_OK,m_RHHelper.coordinatorStop());
	  }

	  m_RHHelper.resourceHostingTerminate();
    }
    catch(Exception ex)
	{
    	ex.printStackTrace();
    	fail("Exception occurred : " + ex.getLocalizedMessage());
	}
  }
}
