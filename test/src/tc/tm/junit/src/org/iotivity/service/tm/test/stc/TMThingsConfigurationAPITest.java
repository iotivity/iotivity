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
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcResource;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;
import org.iotivity.service.tm.OCStackResult;
import org.iotivity.service.tm.test.helper.TMHelper;

import android.test.InstrumentationTestCase;
import android.util.Log;

public class TMThingsConfigurationAPITest extends InstrumentationTestCase {
  private final String   LOG_TAG         = this.getClass().getSimpleName();

  OCStackResult          result          = OCStackResult.OC_STACK_OK;  
  TMHelper               tmHelper        = TMHelper.getInstance();

  protected void setUp() {

    PlatformConfig cfg = new PlatformConfig(getInstrumentation().getTargetContext(),ServiceType.IN_PROC,
        ModeType.CLIENT_SERVER, "0.0.0.0", 0, QualityOfService.LOW);
    OcPlatform.Configure(cfg);

    tmHelper.errorMsg = "";    
    Log.i(LOG_TAG, "SetUp Finished");

  }

  protected void tearDown() {

    tmHelper.unregisterAllResources();
    Log.i(LOG_TAG, "tearDown Finished");
    
  }

  /**
   * @since 2015-03-25
   * @see public synchronized static void Configure(PlatformConfig platformConfig)
   * @see OCStackResult findCandidateResources(Vector<String> resourceTypes,
   *         	int waitTime)
   * @objective to test if target API can successfully return the configuration of single resource
   * @target OCStackResult getConfigurations(OcResource resource, Vector<String> configurations)
   * @test_data 	1. resource reference to resource to be used
   * 				2. configurations List of configurations key 
   * @pre_condition 1. several types of resources(3 core.light type and 2 core.fan type) running 
   * 					in testserver app in linux
   *  				2. GroupManager is instantiated 
   * 				3. findCandidateResource listener is added to GroupManager
   * @procedure 	1. Create resource type list using core.light type resource
   * 				2. Perform findCandidateResources() using resource type list 
   * 				3. Wait for the findResource callback to get found resource list
   * 				4. ThingsConfiguration is instantiated
   * 				5. configuration Listener is added to ThingsConfiguration
   * 				6. Create configuration name list 
   * 				7. Perform getConfigurations on the found resource, using the configuration name list 
   * 				8. Check whether callback was called 
   * 				9. Check whether callback returns desired error code 
   * 				10. Check the returned representation whether the configuration is actually valid value or null
   * @post_condition Close the linux app and unregister all resources
   * @expected 		1. The target API returns OC_STACK_OK 
   * 				2. The configuration found from callback has valid values
   **/  
  public void testGetConfigurationsOfSingleResource_SQV_P() {
    
    Vector<String> resourceTypeList = new Vector<String>();
    resourceTypeList.add(tmHelper.RESOURCE_TYPE_LIGHT);

    boolean result = tmHelper.findCandidateResourcesAndVerify(resourceTypeList,
        OCStackResult.OC_STACK_OK, tmHelper.RESOURCE_COUNT_LIGHT);

    if (!result) {
      fail(tmHelper.errorMsg);
      return;
    }

    String expectedValue = "off";
    result = tmHelper.getConfigurationAndVerify(tmHelper.getResourceList().get(tmHelper.INT_ZERO),
        OCStackResult.OC_STACK_OK, true, tmHelper.INT_ZERO, expectedValue);

    if (!result) {
      fail(tmHelper.errorMsg);
    }

  }
  
  /**
   * @since 2015-03-27
   * @see public synchronized static void Configure(PlatformConfig platformConfig)
   * @see OCStackResult findCandidateResources(Vector<String> resourceTypes,
   *         	int waitTime)
   * @objective to test if target API can successfully update the configuration of a resource
   * @target OCStackResult updateConfigurations(OcResource resource,
   *          		Map<String, String> configurations)
   * @test_data 	1. resource reference to resource to be used
   *            	2. configurations map of configurations key and value
   * @pre_condition 1. several types of resources(3 core.light type and 2 core.fan type) running 
   * 					in testserver app in linux
   *  				2. GroupManager is instantiated 
   * 				3. findCandidateResource listener is added to GroupManager
   * @procedure 	1. Create resource type list using core.light type resource
   * 				2. Perform findCandidateResources() using resource type list 
   * 				3. Wait for the findResource callback to get found resource list
   * 				4. ThingsConfiguration is instantiated
   * 				5. configuration Listener is added to ThingsConfiguration 
   * 				6  Create a configuration using 'power' = 'on'
   * 				7. Perform updateConfigurations on the found resource, using update value "on" 
   * 				8. Check whether callback was called 
   * 				9. Check whether callback returns desired error code 
   * 				10. Check the returned representation whether the configuration is actually updated (using getValueString())
   * @post_condition Close the linux app and unregister all resources
   * @expected 		1. The target API should return OC_STACK_OK 
   * 				2. The callback should return with error code 0 
   * 				3. The configuration should be updated as 'power' = 'on'
   **/  
  public void testUpdateConfigurationsOfSingleResource_SQV_P() {
    
    Vector<String> resourceTypeList = new Vector<String>();
    resourceTypeList.add(tmHelper.RESOURCE_TYPE_LIGHT);

    boolean result = tmHelper.findCandidateResourcesAndVerify(resourceTypeList,
        OCStackResult.OC_STACK_OK, tmHelper.RESOURCE_COUNT_LIGHT);
    if (!result) {
      fail(tmHelper.errorMsg);
      return;
    }

    String toUpdateValue = "on";
    result = tmHelper.updateConfigurationAndVerify(tmHelper.getResourceList().get(tmHelper.INT_ZERO),
        OCStackResult.OC_STACK_OK, true, tmHelper.INT_ZERO, toUpdateValue);
    if (!result) {
      fail(tmHelper.errorMsg);
    }

  }

  /**
   * @since 2015-03-27
   * @see public synchronized static void Configure(PlatformConfig platformConfig)
   * @objective to test if target API can successfully do bootstrapping of bootstrap server
   * @target OCStackResult doBootstrap()
   * @test_data None
   * @pre_condition Start the bootstrap server app in linux 
   * @procedure 		1. ThingsConfiguration is instantiated
   * 					2. Perform doBootstrap() 
   * 					3. Wait for bootstrapping to be finished 
   * 					4. Verify from callback function
   * @post_condition Close the linux app and unregister all resources
   * @expected 			1. The target API should return OC_STACK_OK 
   * 					2. The callback should be called with error code = 0
   **/
  public void testDoBootstrap_SQV_P() {

    OCStackResult expectedResult = OCStackResult.OC_STACK_OK;
    int expectedErrrorCode = tmHelper.SUCCESS_RESPONSE;
    boolean result = tmHelper.doBootstrapAndVerify(expectedResult, expectedErrrorCode);

    if (!result) {
      fail(tmHelper.errorMsg);
      return;
    }
    
  }

  /**
   * @since 2015-03-27
   * @see public synchronized static void Configure(PlatformConfig platformConfig)
   * @see OCStackResult findCandidateResources(Vector<String> resourceTypes,
   *         	int waitTime)
   * @objective to test if target API can successfully reboot a resource
   * @target OCStackResult reboot(OcResource resource)
   * @test_data resource reference to resource to be used
   * @pre_condition 1. several types of resources(3 core.light type and 2 core.fan type) running 
   * 					in testserver app in linux
   *  				2. GroupManager is instantiated 
   * 				3. findCandidateResource listener is added to GroupManager
   * @procedure 	1. Create resource type list using core.light type resource
   * 				2. Perform findCandidateResources() using resource type list 
   * 				3. Wait for the findResource callback to get found resource list
   * 				4. ThingsMaintenance is instantiated
   * 				5. Perform reboot() a resource form the found resource list
   * 				6. Check whether callback is called 
   * 				7. Check the error code from the callback 
   * 				8. check representation for reboot state
   * @post_condition Close the linux app and unregister all resources
   * @expected 		1. The target API should return OC_STACK_OK 
   * 				2. From the callback, representation reboot state should be true
   **/  
  public void testRebootResource_SQV_P() {
    
    Vector<String> resourceTypeList = new Vector<String>();
    resourceTypeList.add(tmHelper.RESOURCE_TYPE_LIGHT);

    boolean result = tmHelper.findCandidateResourcesAndVerify(resourceTypeList,
        OCStackResult.OC_STACK_OK, tmHelper.RESOURCE_COUNT_LIGHT);

    if (!result) {
      fail(tmHelper.errorMsg);
      return;
    }

    int expectedErrorCode = tmHelper.SUCCESS_RESPONSE;
    String expectedValue = "true";
    result = tmHelper.rebootAndVerify(tmHelper.getResourceList().get(tmHelper.INT_ZERO), OCStackResult.OC_STACK_OK,
        expectedErrorCode, expectedValue);

    if (!result) {
      fail(tmHelper.errorMsg);
    }
    
  }

  /**
   * @since 2015-03-26
   * @see public synchronized static void Configure(PlatformConfig platformConfig)
   * @see OCStackResult findCandidateResources(Vector<String> resourceTypes,
   *         	int waitTime)
   * @objective to test if target API can successfully reboot with unsupported resource
   * @target OCStackResult reboot(OcResource resource)
   * @test_data resource reference to resource to be used
   * @pre_condition 1. GroupManager is instantiated 
   * 				2. findCandidateResource listener is added to GroupManager
   * @procedure 	1. Create a resource type list using core.dummy 
   * 				2. Start a server of unsupported type(core.dummy) in linux
   * 				3. Perform findCandidateResources() API using the list
   * 				4. ThingsMaintenance is instantiated 
   * 				5. Perform reboot() API on one of the found resources 
   * 				6. Check whether callback is called 
   * 				7. Check the error code from the callback 
   * 				8. check representation for reboot state
   * @post_condition Close the linux app and unregister all resources
   * @expected 		1. The target API should return OC_STACK_OK 
   * 				2. From the callback, the reboot state should be empty
   **/  
  public void testRebootUnsupportedResource_DSCC_N() {
    
    Vector<String> resourceTypeList = new Vector<String>();
    resourceTypeList.add(tmHelper.RESOURCE_TYPE_DUMMY);

    boolean result = tmHelper.findCandidateResourcesAndVerify(resourceTypeList,
        OCStackResult.OC_STACK_OK, tmHelper.RESOURCE_COUNT_LIGHT);

    if (!result) {
      fail(tmHelper.errorMsg);
      return;
    }

    int expectedErrorCode = tmHelper.SUCCESS_RESPONSE;
    String expectedValue = "";
    result = tmHelper.rebootAndVerify(tmHelper.getResourceList().get(tmHelper.INT_ZERO), OCStackResult.OC_STACK_OK,
        expectedErrorCode, expectedValue);

    if (!result) {
      fail(tmHelper.errorMsg);
    }

  }

  /**
   * @since 2015-03-27
   * @see public synchronized static void Configure(PlatformConfig platformConfig)
   * @see OCStackResult findCandidateResources(Vector<String> resourceTypes,
   *         	int waitTime)
   * @see OCStackResult updateConfigurations(OcResource resource,
   *          		Map<String, String> configurations)
   * @objective to test if target API can factoryReset successfully
   * @target OCStackResult factoryReset(OcResource resource)
   * @test_data resource reference to resource to be used
   * @pre_condition 1. several types of resources(3 core.light type and 2 core.fan type) running 
   * 					in testserver app in linux
   *  				2. GroupManager is instantiated 
   * 				3. findCandidateResource listener is added to GroupManager
   * @procedure 	1. Create resource type list using core.light type resource
   * 				2. Perform findCandidateResources() using resource type list 
   * 				3. Wait for the findResource callback to get found resource list
   * 				4. Create a configuration using 'power' = 'on' 
   * 				5. Perform updateConfiguration() on one of the found resources 
   * 				6. Check whether callback is called 
   * 				7. Check the error code from the callback 
   * 				8. check representation for all attribute state has changed 
   * 				9. Perform factoryReset() API on the same resource 
   * 				10. Check whether callback is called 
   * 				11. Check the error code from the callback 
   * 				12. check representation for all attribute state has reverted to default value
   * @post_condition Close the linux app and unregister all resources
   * @expected 		1. The target API should return OC_STACK_OK 
   * 				2. From the callback, all attribute value will be equal to default value
   **/  
  public void testFactoryReset_SQV_P() {
    
    Vector<String> resourceTypeList = new Vector<String>();
    resourceTypeList.add(tmHelper.RESOURCE_TYPE_LIGHT);

    boolean result = tmHelper.findCandidateResourcesAndVerify(resourceTypeList,
        OCStackResult.OC_STACK_OK, tmHelper.RESOURCE_COUNT_LIGHT);

    if (!result) {
      fail(tmHelper.errorMsg);
      return;
    }

    String toUpdateValue = "on";
    result = tmHelper.updateConfigurationAndVerify(tmHelper.getResourceList().get(tmHelper.INT_ZERO),
        OCStackResult.OC_STACK_OK, true, tmHelper.INT_ZERO, toUpdateValue);

    if (!result) {
      fail(tmHelper.errorMsg);
    }

    result = tmHelper.factoryResetAndVerify(tmHelper.getResourceList().get(tmHelper.INT_ZERO),
        OCStackResult.OC_STACK_OK, tmHelper.INT_ZERO, true);

    if (!result) {
      fail(tmHelper.errorMsg);
    }
    
  }

  /**
   * @since 2015-03-28
   * @see public synchronized static void Configure(PlatformConfig platformConfig)
   * @see OCStackResult findCandidateResources(Vector<String> resourceTypes,
   *         	int waitTime)
   * @objective to test if target API can successfully factory reset on unsupported resource
   * @target OCStackResult factoryReset(OcResource resource)
   * @test_data resource reference to resource to be used
   * @pre_condition 1. GroupManager is instantiated 
   * 				2. findCandidateResource listener is added to GroupManager
   * @procedure 	1. Create a resource type list using core.dummy 
   * 				2. Start a server of unsupported type(core.dummy) in linux
   * 				3. Perform findCandidateResources() API using the list
   * 				4. ThingsMaintenance is instantiated    
   * 				5. Perform factoryReset() API on one of the found resources 
   * 				6. Check whether callback is called
   *            	7. Check the error code from the callback 
   *            	8. check representation for all attribute
   * @post_condition Close the linux app and unregister all resources
   * @expected 		1. The target API should return OC_STACK_OK 
   * 				2. From the callback, all attribute value will be equal to default value
   **/  
  public void testFactoryResetToUnsupportedResource_DSCC_N() {
    
    Vector<String> resourceTypeList = new Vector<String>();
    resourceTypeList.add(tmHelper.RESOURCE_TYPE_DUMMY);

    boolean result = tmHelper.findCandidateResourcesAndVerify(resourceTypeList,
        OCStackResult.OC_STACK_OK, tmHelper.INT_ONE);

    if (!result) {
      fail(tmHelper.errorMsg);
      return;
    }

    result = tmHelper.factoryResetAndVerify(tmHelper.getResourceList().get(tmHelper.INT_ZERO),
        OCStackResult.OC_STACK_OK, tmHelper.INT_ZERO, false);

    if (!result) {
      fail(tmHelper.errorMsg);
    }

  }
  
}
