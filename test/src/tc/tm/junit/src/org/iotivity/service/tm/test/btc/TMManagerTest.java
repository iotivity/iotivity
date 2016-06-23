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

package org.iotivity.service.tm.test.btc;

import java.util.Vector;

import org.iotivity.base.ModeType;
import org.iotivity.base.OcException;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcResourceHandle;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.OcResource;
import org.iotivity.base.ServiceType;
import org.iotivity.service.tm.GroupManager;
import org.iotivity.service.tm.OCStackResult;
import org.iotivity.service.tm.test.helper.TMHelper;

import android.test.InstrumentationTestCase;
import android.util.Log;

public class TMManagerTest extends InstrumentationTestCase  {

  private final String   LOG_TAG         = this.getClass().getSimpleName();
  private OCStackResult  result          = OCStackResult.OC_STACK_OK;
  private TMHelper       tmHelper        = TMHelper.getInstance();
  private Vector<String> resourceTypeList = new Vector<String>(); 

  protected void setUp() {
	  
    result = OCStackResult.OC_STACK_OK;
    resourceTypeList.clear();

    PlatformConfig cfg = new PlatformConfig(getInstrumentation().getTargetContext(),ServiceType.IN_PROC,
        ModeType.CLIENT_SERVER, "0.0.0.0", 0, QualityOfService.LOW);
    OcPlatform.Configure(cfg);    
    Log.i(LOG_TAG, "SetUp Finished");
    
  }

  protected void tearDown() {
	  
    tmHelper.unregisterAllResources();    
    Log.i(LOG_TAG, "TearDown Finished");
    
  }

  /**
   * @since 2015-03-04
   * @see public synchronized static void Configure(PlatformConfig platformConfig)
   * @objective Test 'findCandidateResources' function with positive basic way
   * @target OCStackResult findCandidateResources(Vector<String> resourceTypes,
   *         	int waitTime)
   * @test_data 	1. resourceTypes resource Type list
   * 				2. waitTime = -1
   * @pre_condition	1. several types of resources(3 core.light type and 2 core.fan type) running 
   * 					in testserver app in linux
   *                2. GroupManager is instantiated 
   *                3. findCandidateResource listener is added to GroupManager
   * @procedure 	1. Create resource type list using core.light type resource
   * 				2. Perform findCandidateResources() using resource type list
   * 				3. check if OC_STACK_OK is returned
   * @post_condition Close the linux app and unregister all resources
   * @expected The target API returns OC_STACK_OK
   **/
  public void testFindCandidateResources_SRC_P() {
    
	resourceTypeList.add(tmHelper.RESOURCE_TYPE_LIGHT);

    tmHelper.resourceList.clear();
    
    int waitTime = -1;
    result = tmHelper.groupManager.findCandidateResources(resourceTypeList, waitTime);

    assertEquals("Failed to find resources", OCStackResult.OC_STACK_OK.toString(),
        result.toString());

  }

  /**
   * @since 2015-03-04
   * @see public synchronized static void Configure(PlatformConfig platformConfig)
   * @objective Test 'findCandidateResources' function with negative basic way using null resource
   * 		list
   * @target OCStackResult findCandidateResources(Vector<String> resourceTypes,
   *         	int waitTime)
   * @test_data 	1. resourceTypes resource Type list
   * 				2. waitTime = -1
   * @pre_condition	1. several types of resources(3 core.light type and 2 core.fan type) running 
   * 					in testserver app in linux 
   * 				2. GroupManager is instantiated 
   * 				3. findCandidateResource listener is added to GroupManager
   * @procedure 	1. Create resource type list using null 
   * 				2. Perform findCandidateResources() using resource type list
   * 				3. check if OC_STACK_OK is returned
   * @post_condition Close the linux app
   * @expected 		The target API returns OC_STACK_ERROR
   **/
  public void testFindCandidateResources_ECRC_N() {

    Vector<String> resourceTypeList = new Vector<String>();

    tmHelper.resourceList.clear();
    
    int waitTime = -1;
    try {
        result = tmHelper.groupManager.findCandidateResources(resourceTypeList, waitTime);
    } catch (Exception e) {
      e.printStackTrace();
      fail("Exception occured while calling findCandidateResources");
    }
    assertEquals("Failed to find resources", OCStackResult.OC_STACK_ERROR.toString(),
        result.toString());
    
  }

  /**
   * @since 2015-03-04
   * @see public synchronized static void Configure(PlatformConfig platformConfig)
   * @objective Test 'findCandidateResources' API without adding
   *            	findCandidateResource listener
   * @target OCStackResult findCandidateResources(Vector<String> resourceTypes,
   *         	int waitTime)
   * @test_data 	1. resourceTypes resource Type list
   * 				2. waitTime = 10
   * @pre_condition several types of resources(3 core.light type and 2 core.fan type) running 
   * 					in testserver app in linux   				
   * @procedure 	1. Instantiate a local copy of ThingsManager(no need to add any
   *            		listener) 
   *            	2. Create resource type list using core.light type resource
   *            	3. Perform findCandidateResources() from the local ThingsManager
   *            			using resource type list and waitsec
   *            	4. check if OC_STACK_OK is returned
   * @post_condition Close the linux app
   * @expected The target API returns OC_STACK_OK
   **/
  public void testFindCandidateResourcesWithoutListener_ECRC_N() {
    
	String resourceType = "core.light";
    resourceTypeList.add(resourceType);
    GroupManager localGroupManager = new GroupManager();

    tmHelper.resourceList.clear();
    
    int waitTime = -1;
    try {
      result = localGroupManager.findCandidateResources(resourceTypeList, waitTime);
    } catch (Exception e) {
      e.printStackTrace();
      fail("Exception occured while calling findCandidateResources");
    }

    assertEquals("Failed to find resources", OCStackResult.OC_STACK_OK.toString(),
        result.toString());
  }

}
