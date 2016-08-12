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

package org.iotivity.service.tm.test.stc;

import java.util.Vector;

import org.iotivity.base.ModeType;
import org.iotivity.base.OcException;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcResourceHandle;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;
import org.iotivity.service.tm.OCStackResult;
import org.iotivity.service.tm.test.helper.TMHelper;

import android.test.InstrumentationTestCase;
import android.util.Log;

public class TMManagerTest extends InstrumentationTestCase  {
  private final String   LOG_TAG                = this.getClass().getSimpleName();

  OCStackResult          result;
  int                    findCandidateCallCount = 0;
  int                    expectedResourceCount  = 0;
  TMHelper               tmHelper               = TMHelper.getInstance();

  protected void setUp() {
    
    PlatformConfig cfg = new PlatformConfig(getInstrumentation().getTargetContext(),ServiceType.IN_PROC,
        ModeType.CLIENT_SERVER, "0.0.0.0", 0, QualityOfService.LOW);
    OcPlatform.Configure(cfg);
    tmHelper.errorMsg = "";    
    Log.i(LOG_TAG, "SetUp Finished");

  }

  protected void tearDown() {
	  
    tmHelper.unregisterAllResources();    
    Log.i(LOG_TAG, "TearDown Finished");
    
  }

  /**
   * @since 2015-03-20
   * @see public synchronized static void Configure(PlatformConfig platformConfig)
   * @objective to test if target API can find at least one resource of a
   *            specified type
   * @target OCStackResult findCandidateResources(Vector<String> resourceTypes,
   *         	int waitTime)
   * @test_data 	1. resourceTypes resource Type list
   * 				2. waitTime = -1
   * @pre_condition 1. several types of resources(3 core.light type and 2 core.fan type) running 
   * 					in testserver app in linux
   *                2. GroupManager is instantiated 
   *                3. findCandidateResource listener is added to GroupManager
   * @procedure 	1. Create resource type list using core.light type resource
   * 				2. Perform findCandidateResources() using resource type list
   * 				3. Wait till callback is called
   * 				4. Check how many resource is found
   * @post_condition Close the linux app and unregister all resources
   * @expected 		1. API returns OC_STACK_OK 
   * 				2. At least 1 core.fan type resource is found by callback
   **/
  public void testFindOneTypeOfCandidateResources_SQV_P() {
	      
    Vector<String> resourceTypeList = new Vector<String>();
    resourceTypeList.add(tmHelper.RESOURCE_TYPE_FAN); 
    
    if (!tmHelper.findCandidateResourcesAndVerify(resourceTypeList, OCStackResult.OC_STACK_OK,
        tmHelper.RESOURCE_COUNT_FAN)) {
      fail(tmHelper.errorMsg);
    }
    
  }

  /**
   * @since 2015-03-20
   * @see public synchronized static void Configure(PlatformConfig platformConfig)
   * @objective to check stability of target API by calling it repeatedly(10 times),
   *            waiting for callback each time and check whether the API can
   *            always find resources through callback
   * @target OCStackResult findCandidateResources(Vector<String> resourceTypes,
   *         	int waitTime)
   * @test_data 	1. resourceTypes resource Type list
   * 				2. waitTime = -1
   * @pre_condition 1. several types of resources(3 core.light type and 2 core.fan type) running 
   * 					in testserver app in linux
   *                2. GroupManager is instantiated 
   *                3. findCandidateResource listener is added to GroupManager
   * @procedure 	1. Create resource type list using core.light and core.fan type resource
   * 				2. Perform findCandidateResources() using resource type list
   * 				3. Wait till callback is called
   * 				4. Check how many resource is found
   * 				5. Repeat step 2-4 nine times
   * @post_condition Close the linux app and unregister all resources
   * @expected 		1. API returns OC_STACK_OK each time 
   * 				2. At least 2 resources are found by callback each time(at step 4)
   **/
  public void testFindCandidateResourcesTenTimes_VLCC_P() {
	      
    Vector<String> resourceTypeList = new Vector<String>();
    resourceTypeList.add(tmHelper.RESOURCE_TYPE_FAN);
    resourceTypeList.add(tmHelper.RESOURCE_TYPE_LIGHT);

    tmHelper.errorMsg = "";
    expectedResourceCount = resourceTypeList.size();
    findCandidateCallCount = 0;

    for (int i = 0; i < 10; i++) {
      findCandidateCallCount++;
      tmHelper.resourceList.clear();
      tmHelper.resourceFoundCallbackCount = 0;
      
      int waitTime = -1;
      result = tmHelper.groupManager.findCandidateResources(resourceTypeList, waitTime);

      if (result != OCStackResult.OC_STACK_OK) {
        fail("TestCase Failed. Expected = OCStackResult.OC_STACK_OK, Actual =" + result);
      }

      tmHelper.waitForResourceFound(tmHelper.CALLBACK_WAIT_MAX);

      if (tmHelper.resourceList.size() < expectedResourceCount) {
        fail("TestCase Failed. Resource count mismatch at iteration = " + i + " ; Expected = "
            + expectedResourceCount + ", Actual =" + tmHelper.resourceList.size());
      }
      
      tmHelper.waitInSecond(tmHelper.CALLBACK_WAIT_MIN);
      
    }

  }

  /**
   * @since 2015-03-22
   * @see public synchronized static void Configure(PlatformConfig platformConfig)
   * @objective to test if target API can find all available resources if enough
   *            time is provided
   * @target OCStackResult findCandidateResources(Vector<String> resourceTypes,
   *         	int waitTime)
   * @test_data 	1. resourceTypes resource Type list
   * 				2. waitTime = 10
   * @pre_condition 1. several types of resources(3 core.light type and 2 core.fan type) running 
   * 					in testserver app in linux
   *                2. GroupManager is instantiated 
   *                3. findCandidateResource listener is added to GroupManager
   * @procedure 	1. Create resource type list using core.light and core.fan type resource
   * 				2. Perform findCandidateResources() using resource type list
   * 				3. Wait till callback is called
   * 				4. Check if all 5 resources are found
   * @post_condition Close the linux app and unregister all resources
   * @expected 		1. API returns OC_STACK_OK 
   * 				2. All 5 resources are found by callback
   **/  
  public void testFindCandidateResourcesTillAllResourcesFound_SQV_P() {
    
    Vector<String> resourceTypeList = new Vector<String>();
    resourceTypeList.add(tmHelper.RESOURCE_TYPE_FAN);
    resourceTypeList.add(tmHelper.RESOURCE_TYPE_LIGHT);

    tmHelper.errorMsg = "";
    expectedResourceCount = tmHelper.RESOURCE_COUNT_FAN + tmHelper.RESOURCE_COUNT_LIGHT;
    findCandidateCallCount = 1;

    tmHelper.resourceList.clear();
    tmHelper.resourceFoundCallbackCount = 0;

    result = tmHelper.groupManager.findCandidateResources(resourceTypeList,
        tmHelper.CALLBACK_WAIT_MAX);

    tmHelper.waitInSecond(tmHelper.CALLBACK_WAIT_MAX + tmHelper.CALLBACK_WAIT_MIN);
    if (result != OCStackResult.OC_STACK_OK) {
      fail("TestCase Failed. Expected = OCStackResult.OC_STACK_OK, Actual = " + result);
    }

    if (tmHelper.resourceList.size() != expectedResourceCount) {
      fail("TestCase Failed. Expected = " + expectedResourceCount + ", Actual ="
          + tmHelper.resourceList.size());
    }
    
  }

  /**
   * @since 2015-03-22
   * @see public synchronized static void Configure(PlatformConfig platformConfig)
   * @objective to test if target API doesn't call the callback when a
   *            non-existing type resource is asked to find
   * @target OCStackResult findCandidateResources(Vector<String> resourceTypes,
   *         	int waitTime)
   * @test_data 	1. resourceTypes resource Type list
   * 				2. waitTime = 10
   * @pre_condition 1. several types of resources(3 core.light type and 2 core.fan type) running 
   * 					in testserver app in linux
   *                2. GroupManager is instantiated 
   *                3. findCandidateResource listener is added to GroupManager
   * @procedure 	1. Create resource type list using invalid resource
   *            		types(core.dummy-1, core.dummy-2, core.dummy-3) 
   *            	2. Perform findCandidateResources() using resource type list
   * 				3. Wait to test if the callback never gets called
   * @post_condition Close the linux app and unregister all resources
   * @expected 		1. API returns OC_STACK_OK 
   * 				2. No resources are found after waiting 10 seconds
   **/  
  public void testFindNonExistingResources_DSCC_N() {
    
    Vector<String> resourceTypeList = new Vector<String>();
    resourceTypeList.add(tmHelper.RESOURCE_TYPE_DUMMY_1);
    resourceTypeList.add(tmHelper.RESOURCE_TYPE_DUMMY_2);
    resourceTypeList.add(tmHelper.RESOURCE_TYPE_DUMMY_3);

    tmHelper.errorMsg = "";
    expectedResourceCount = 0;
    findCandidateCallCount = 1;

    tmHelper.resourceList.clear();
    tmHelper.resourceFoundCallbackCount = 0;

    result = tmHelper.groupManager.findCandidateResources(resourceTypeList,
        tmHelper.CALLBACK_WAIT_MAX);

    tmHelper.waitInSecond(tmHelper.CALLBACK_WAIT_MAX + tmHelper.CALLBACK_WAIT_MIN);
    if (result != OCStackResult.OC_STACK_OK) {
      fail("TestCase Failed. Expected = OCStackResult.OC_STACK_OK, Actual =" + result);
    }

    if (tmHelper.resourceList.size() != expectedResourceCount) {
      fail("TestCase Failed. Expected = " + expectedResourceCount + ", Actual ="
          + tmHelper.resourceList.size());
    }

  }

  /**
   * @since 2015-03-22
   * @see public synchronized static void Configure(PlatformConfig platformConfig)
   * @objective to test if target API can find any resource when waitsec is 0
   * @target OCStackResult findCandidateResources(Vector<String> resourceTypes,
   *         	int waitTime)
   * @test_data 	1. resourceTypes resource Type list
   * 				2. waitTime = 0
   * @pre_condition 1. several types of resources(3 core.light type and 2 core.fan type) running 
   * 					in testserver app in linux
   *                2. GroupManager is instantiated 
   *                3. findCandidateResource listener is added to GroupManager
   * @procedure		1. Create resource type list using invalid resource
   *            		types(core.dummy-1, core.dummy-2, core.dummy-3) 
   *            	2. Perform findCandidateResources() using resource type list and 0 waitsec
   *            	3. check if callback is called 
   *            	4. Wait 1 second and again check if callback is called
   * @post_condition Close the linux app and unregister all resources
   * @expected 		1. API returns OC_STACK_OK 
   * 				2. Callback is not called and No resources are found
   **/
  public void testFindCandidateResourcesImmediately_SQV_P() {
    
    Vector<String> resourceTypeList = new Vector<String>();
    resourceTypeList.add(tmHelper.RESOURCE_TYPE_FAN);
    resourceTypeList.add(tmHelper.RESOURCE_TYPE_LIGHT);

    tmHelper.errorMsg = "";
    expectedResourceCount = 0;
    findCandidateCallCount = 1;

    tmHelper.resourceList.clear();
    tmHelper.resourceFoundCallbackCount = 0;

    result = tmHelper.groupManager.findCandidateResources(resourceTypeList,
        tmHelper.CALLBACK_WAIT_NONE);

    if (result != OCStackResult.OC_STACK_OK) {
      fail("TestCase Failed. Expected = OCStackResult.OC_STACK_OK, Actual =" + result);
    }

    if ((tmHelper.resourceFoundCallbackCount == tmHelper.INT_ONE)
        && (tmHelper.resourceList.size() != expectedResourceCount)) {
      fail("TestCase Failed. Expected = " + expectedResourceCount + ", Actual ="
          + tmHelper.resourceList.size());
      return;
    }

    tmHelper.waitInSecond(tmHelper.CALLBACK_WAIT_MIN);
    if ((tmHelper.resourceFoundCallbackCount == tmHelper.INT_ONE)
        && (tmHelper.resourceList.size() != expectedResourceCount)) {
      fail("TestCase Failed. Expected = " + expectedResourceCount + ", Actual ="
          + tmHelper.resourceList.size());
    }

  }

  /**
   * @since 2015-03-22
   * @see public synchronized static void Configure(PlatformConfig platformConfig)
   * @objective to test if target API can find at least one resource of each
   *            	type mentioned in the resource type list
   * @target OCStackResult findCandidateResources(Vector<String> resourceTypes,
   *         	int waitTime)
   * @test_data 	1. resourceTypes resource Type list
   * 				2. waitTime = -1
   * @pre_condition 1. several types of resources(3 core.light type and 2 core.fan type) running 
   * 					in testserver app in linux
   *                2. GroupManager is instantiated 
   *                3. findCandidateResource listener is added to GroupManager
   * @procedure 	1. Create resource type list using core.light and core.fan type resource
   * 				2. Perform findCandidateResources() using resource type list
   * 				3. Wait till callback is called
   *            	4. check whether the found resource list contains both core.light & core.fan type resource
   * @post_condition Close the linux app and unregister all resources
   * @expected 		1. API returns OC_STACK_OK 
   * 				2. Both core.light & core.fan type resources are found
   **/
  public void testFindCandidateResourcesAllTypes_SQV_P() {
    
    Vector<String> resourceTypeList = new Vector<String>();
    resourceTypeList.add(tmHelper.RESOURCE_TYPE_FAN);
    resourceTypeList.add(tmHelper.RESOURCE_TYPE_LIGHT);

    tmHelper.errorMsg = "";
    findCandidateCallCount = 1;

    tmHelper.resourceList.clear();
    tmHelper.resourceFoundCallbackCount = 0;
    expectedResourceCount = resourceTypeList.size();

    int waitTime = -1;
    result = tmHelper.groupManager.findCandidateResources(resourceTypeList, waitTime);

    tmHelper.waitInSecond(tmHelper.CALLBACK_WAIT_MAX);
    if (result != OCStackResult.OC_STACK_OK) {
      fail("TestCase Failed. Expected = OCStackResult.OC_STACK_OK, Actual =" + result);
    }

    if (!tmHelper.containsResource(tmHelper.resourceList, resourceTypeList)) {
      fail(tmHelper.errorMsg);
    }
  }

  /**
   * @since 2015-03-22
   * @see public synchronized static void Configure(PlatformConfig platformConfig)
   * @objective to test if target API finds resources within the the time limit
   *            	specified(i.e. whether the waitsec argument works fine and the
   *            		API does not consume extra time)
   * @target OCStackResult findCandidateResources(Vector<String> resourceTypes,
   *         	int waitTime)
   * @test_data 	1. resourceTypes resource Type list
   * 				2. waitTime = 10
   * @pre_condition 1. several types of resources(3 core.light type and 2 core.fan type) running 
   * 					in testserver app in linux
   *                2. GroupManager is instantiated 
   *                3. findCandidateResource listener is added to GroupManager
   * @procedure 	1. Create a resource type list with core.light and core.fan type
   *            	2. Perform findCandidateResources with resource type list and
   *           			 waitsec = 10 
   *           		3. Wait to check if callback is called 
   *           		4. Check whether the callback is called within 10 seconds
   * @post_condition Close the linux app and unregister all resources
   * @expected 		1. API returns OC_STACK_OK 
   * 				2. The callback is called within 10 seconds
   **/
  public void testFindCandidateResourcesCheckWaitsec_SQV_P() {
    
    Vector<String> resourceTypeList = new Vector<String>();
    String resourceType = tmHelper.RESOURCE_TYPE_FAN;
    resourceTypeList.add(resourceType);
    resourceType = tmHelper.RESOURCE_TYPE_LIGHT;
    resourceTypeList.add(resourceType);

    tmHelper.errorMsg = "";
    findCandidateCallCount = 1;

    tmHelper.resourceList.clear();
    tmHelper.resourceFoundCallbackCount = 0;
    expectedResourceCount = resourceTypeList.size();

    result = tmHelper.groupManager.findCandidateResources(resourceTypeList,
        tmHelper.CALLBACK_WAIT_MAX);
    if (result != OCStackResult.OC_STACK_OK) {
      fail("TestCase Failed. Expected = OCStackResult.OC_STACK_OK, Actual =" + result);
    }

    int elapsedTime = 0;
    do {
      tmHelper.waitInSecond(tmHelper.CALLBACK_WAIT_MIN);
      elapsedTime += tmHelper.CALLBACK_WAIT_MIN;

      if ((tmHelper.resourceFoundCallbackCount == tmHelper.INT_ONE) && (elapsedTime < tmHelper.CALLBACK_WAIT_MAX)) {
        fail("TestCase Failed. Callback was called at " + elapsedTime);
      }

    } while (tmHelper.resourceFoundCallbackCount == tmHelper.INT_ZERO);
    
  }

  /**
   * @since 2015-04-10
   * @see public synchronized static void Configure(PlatformConfig platformConfig)
   * @objective to test if target API can find collections
   * @target OCStackResult findCandidateResources(Vector<String> resourceTypes,
   *         	int waitTime)
   * @test_data 	1. resourceTypes resource Type list
   * 				2. waitTime = 10
   * @pre_condition 1. several types of resources(3 core.light type and 2 core.fan type) running 
   * 					in testserver app in linux
   * 				2. 3 collections(core.room-large, 
   * 					core.room-medium & core.room-small) are running in testcollection app in linux
   *                3. GroupManager is instantiated 
   *                4. findCandidateResource listener is added to GroupManager
   * @procedure 	1. Create resource type list using core.room-large,
   *            		core.room-medium, core.room-small 
   *            	2. Perform findCandidateResources with resource type list and maximum time (10 sec) 
   *            	3. Wait to test if it finds any collection
   * @post_condition Close the linux apps and unregister all resources
   * @expected 		1. API returns OC_STACK_OK 
   * 				2. All 3 collections should be found
   **/
  public void testFindCollenctionsUsingFindCandidateResources_SQV_P() {
    
    Vector<String> resourceTypeList = new Vector<String>();
    resourceTypeList.add(tmHelper.COLLECTION_TYPE_ROOM_LARGE);
    resourceTypeList.add(tmHelper.COLLECTION_TYPE_ROOM_MEDIUM);
    resourceTypeList.add(tmHelper.COLLECTION_TYPE_ROOM_SMALL);

    tmHelper.errorMsg = "";
    expectedResourceCount = 3;
    findCandidateCallCount = 1;

    tmHelper.resourceList.clear();
    tmHelper.resourceFoundCallbackCount = 0;

    result = tmHelper.groupManager.findCandidateResources(resourceTypeList,
        tmHelper.CALLBACK_WAIT_MAX);
    if (result != OCStackResult.OC_STACK_OK) {
      fail("TestCase Failed. Expected = OCStackResult.OC_STACK_OK, Actual =" + result);
    }

    tmHelper.waitForResourceFound(tmHelper.CALLBACK_WAIT_MAX + tmHelper.CALLBACK_WAIT_MIN);
    if (tmHelper.resourceList.size() != expectedResourceCount) {
      fail("TestCase Failed. Expected Collection count = " + expectedResourceCount
          + ", Actual Collection count =" + tmHelper.resourceList.size());
    }
    
  }

  /**
   * @since 2015-03-20
   * @see public synchronized static void Configure(PlatformConfig platformConfig)
   * @objective to test the stability of target API when it is called repeatedly
   *            	without waiting for callback
   * @target OCStackResult findCandidateResources(Vector<String> resourceTypes,
   *         	int waitTime)
   * @test_data 	1. resourceTypes resource Type list
   * 				2. waitTime = -1
   * @pre_condition 1. several types of resources(3 core.light type and 2 core.fan type) running 
   * 					in testserver app in linux
   *                2. GroupManager is instantiated 
   *                3. findCandidateResource listener is added to GroupManager
   * @procedure 	1. Create a resource type list with core.light and core.fan type resource
   *            	2. Perform findCandidateResources with resource type list 
   *            	3. Repeat step 1-2 nine more times without waiting for callback 
   *            	4. Wait for callback to get called 
   *            	5. Check no. of resource found
   *            	6. Check how many times the callback got called
   * @post_condition Close the linux app and unregister all resources
   * @expected 		1. API should return OC_STACK_OK at step 2(10
   *           			times) 
   *           		2. At least 2 resource will be found at step 5 
   *           		3. The callback should get called only once
   **/  
  public void testFindCandidateResourcesTenTimesWithoutCallback_DSCC_N() {
    
    Vector<String> resourceTypeList = new Vector<String>();
    resourceTypeList.add(tmHelper.RESOURCE_TYPE_FAN);
    resourceTypeList.add(tmHelper.RESOURCE_TYPE_LIGHT);

    tmHelper.errorMsg = "";
    expectedResourceCount = resourceTypeList.size();
    findCandidateCallCount = 0;

    int totalApiCall = 10;

    for (int i = 0; i < totalApiCall; i++) {
      findCandidateCallCount++;
      tmHelper.resourceList.clear();
      tmHelper.resourceFoundCallbackCount = 0;

      int waitTime = -1;
      result = tmHelper.groupManager.findCandidateResources(resourceTypeList, waitTime);

      if (result != OCStackResult.OC_STACK_OK) {
        fail("TestCase Failed. Expected = OCStackResult.OC_STACK_OK, Actual =" + result);
      }

    }

    tmHelper.waitInSecond(tmHelper.CALLBACK_WAIT_DEFAULT);
    if (tmHelper.resourceList.size() == tmHelper.INT_ZERO) {
      fail("TestCase Failed. Expected Resource Count = " + expectedResourceCount + ", Actual ="
          + tmHelper.resourceList.size());
    }

    if (tmHelper.resourceFoundCallbackCount != tmHelper.INT_ONE) {
      fail("TestCase Failed. Callback count mismatches. Expected = " + totalApiCall + "; Actual = "
          + tmHelper.resourceFoundCallbackCount);
    }
  }

  /**
   * @since 2015-03-20
   * @see public synchronized static void Configure(PlatformConfig platformConfig)
   * @see OCStackResult findCandidateResources(Vector<String> resourceTypes,
   *         	int waitTime)
   * @objective to test if subscribeCollectionPresence() API returns error when
   *            a resource(not collection) is used to subscribe for collection
   *            presence
   * @target OCStackResult subscribeCollectionPresence(OcResource resource)
   * @test_data resource reference to resource to be used
   * @pre_condition 1. several types of resources(3 core.light type and 2 core.fan type) running 
   * 					in testserver app in linux
   *                2. GroupManager is instantiated 
   *                3. findCandidateResource listener is added to GroupManager
   * @procedure 	1. Create a resource type list using core.light type 
   * 				2. Perform findCandidateResources using the resource type list created at step
   *            			one 
   *            	3. Wait for the callback to get called 
   *            	4. Perform subscribeCollectionPresence on the found resource 
   *            	5. Check presence callback
   * @post_condition Close the linux app and unregister all resources
   * @expected The target API should return OC_STACK_ERROR (at
   *           				step 5)
   **/  
  public void testSubscribeCollectionPresenceForResource_DSCC_N() {
    
    Vector<String> resourceTypeList = new Vector<String>();
    resourceTypeList.add(tmHelper.RESOURCE_TYPE_LIGHT);

    boolean result = tmHelper.findCandidateResourcesAndVerify(resourceTypeList,
        OCStackResult.OC_STACK_OK, tmHelper.INT_ONE);

    if (!result) {
      fail(tmHelper.errorMsg);
      return;
    }

    result = tmHelper.subscribeCollectionPresenceAndVerify(tmHelper.getResourceList().get(tmHelper.INT_ONE),
        OCStackResult.OC_STACK_ERROR, tmHelper.INT_MINUS_ONE);

    if (!result) {
      fail(tmHelper.errorMsg);
    }
    
  }

  /**
   * @since 2015-03-20
   * @see public synchronized static void Configure(PlatformConfig platformConfig)
   * @see OCStackResult findCandidateResources(Vector<String> resourceTypes,
   *         		int waitTime)
   * @see public static OcResourceHandle registerResource(
   *         OcResource ocResource)        
   * @objective to test if target API can successfully bind the resource to a
   *            	group
   * @target OcResourceHandle bindResourceToGroup(OcResource resource,
   *         	OcResourceHandle collectionHandle)
   * @test_data 	1. Reference to the resource that will be child 
   * 				2. collectionHandle collection handle
   * @pre_condition 1. several types of resources(3 core.light type and 2 core.fan type) running 
   * 					in testserver app in linux
   *                2. GroupManager is instantiated 
   *                3. findCandidateResource listener is added to GroupManager
   * @procedure 	1. Create a resource type list using core.light and core.fan type resource 
   * 				2. Perform findcandidateResources using the resource type list 
   * 				3. Wait for the the callback to get the resource pointer of two resources 
   * 				4. Perform registerResource to the fan resource to get collection handle 
   * 				5. Perform bindResourceToGroup() using the light resource reference and
   *            		collection handle
   * @post_condition Close the linux app and unregister all resources
   * @expected The target API should return OC_STACK_OK
   **/  
  public void testBindResourceToGroup_SQV_P() {
    
    OcResourceHandle childHandle = null;

    boolean result = tmHelper.bindResourceToGroupAndVerify(childHandle,
        OCStackResult.OC_STACK_OK);
    
    if (!result) {
      fail(tmHelper.errorMsg);
    }
    
  }
}
