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

import java.util.HashMap;
import java.util.Map;
import java.util.Vector;

import org.iotivity.base.ModeType;
import org.iotivity.base.OcException;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;
import org.iotivity.base.OcResource;
import org.iotivity.service.tm.OCStackResult;
import org.iotivity.service.tm.test.helper.TMHelper;

import android.test.InstrumentationTestCase;
import android.util.Log;

public class TMThingsConfigurationTest extends InstrumentationTestCase  {
  private final String   LOG_TAG         = this.getClass().getSimpleName();
  private OCStackResult  result          = OCStackResult.OC_STACK_OK;
  private TMHelper       tmHelper        = TMHelper.getInstance();

  protected void setUp() {
	  
    result = OCStackResult.OC_STACK_OK;

    PlatformConfig cfg = new PlatformConfig(getInstrumentation().getTargetContext(),ServiceType.IN_PROC,
        ModeType.CLIENT_SERVER, "0.0.0.0", 0, QualityOfService.LOW);
    OcPlatform.Configure(cfg);    
    Log.i(LOG_TAG, "SetUp Finished");
    
  }

  protected void tearDown() {
	  
    tmHelper.unregisterAllResources();    
    Log.i(LOG_TAG, "tearDown Finished");
    
  }

  /**
   * @since 2015-03-05
   * @see public synchronized static void Configure(PlatformConfig platformConfig)
   * @objective Test 'getListOfSupportedConfigurationUnits' function with positive basic way
   * @target OCStackResult getListOfSupportedConfigurationUnits ()
   * @test_data None
   * @pre_condition 1. several types of resources(3 core.light type and 2 core.fan type) running 
   * 					in testserver app in linux
   * 				2. ThingsConfiguration is instantiated 
   * @procedure 	1. Perform getListOfSupportedConfigurationUnits
   *            	2. Check whether the returned string is not null
   * @post_condition Close the app in linux and unregister all resources
   * @expected A string consisting the supporting units will be returned
   **/
  public void testGetListOfSupportedConfigurationUnits_SRC_P() {

    String configurationUnitsList;
    configurationUnitsList = tmHelper.thingsConfiguration.getListOfSupportedConfigurationUnits();
    Log.i(LOG_TAG, "Configuration Units:" + configurationUnitsList);    

    assertNotNull(configurationUnitsList);
  }

  /**
   * @since 2015-03-05
   * @see public synchronized static void Configure(PlatformConfig platformConfig)
   * @see OCStackResult findCandidateResources(Vector<String> resourceTypes,
   *         	int waitTime)
   * @objective Test 'getConfigurations' function with positive basic way
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
   * 				6. Create a configuration list using 'power' 
   * 				7. Perform getConfigurations()
   *            			using the configuration and a resource from the found resource list
   *            	8. check if OC_STACK_OK is returned
   * @post_condition Close the app in linux and unregister all resources
   * @expected The target API should return OC_STACK_OK
   **/
  public void testGetConfiguration_SRC_P() {

    Vector<String> resourceTypeList = new Vector<String>();
    resourceTypeList.add("core.light");

    tmHelper.resourceList.clear();
    
    int waitTime = -1;
    result = tmHelper.groupManager.findCandidateResources(resourceTypeList, waitTime);
    assertEquals("FindCandidateResources failed!!", OCStackResult.OC_STACK_OK.toString(),
        result.toString());

    tmHelper.waitForResourceFound(tmHelper.CALLBACK_WAIT_MAX);

    Vector<String> configurations = new Vector<String>();
    configurations.add("power");

    OcResource resource=null;
    try{resource = tmHelper.resourceList.get(tmHelper.INT_ZERO);    
    } catch (ArrayIndexOutOfBoundsException e) {
		e.printStackTrace();
	    fail("resourceList is empty");	    
	  }
    
    assertNotNull("resource is NULL",resource);
    
    try {
      result = tmHelper.thingsConfiguration.getConfigurations(resource,
          configurations);
    } catch (OcException e) {
      result = OCStackResult.OC_STACK_ERROR;
      e.printStackTrace();
    }    
    
    assertEquals("getConfiguration failed!!", OCStackResult.OC_STACK_OK.toString(),
        result.toString());

  }

  /**
   * @since 2015-03-05
   * @see public synchronized static void Configure(PlatformConfig platformConfig)
   * @see OCStackResult findCandidateResources(Vector<String> resourceTypes,
   *         	int waitTime)
   * @objective Test 'getConfigurations' function with negative basic way using null resource
   * @target OCStackResult getConfigurations(OcResource resource, Vector<String> configurations)
   * @test_data 	1. resource null
   * 				2. configurations List of configurations key 
   * @pre_condition None
   * @procedure 	1. ThingsConfiguration is instantiated
   * 				2. configuration Listener is added to ThingsConfiguration
   * 				3. Create a configuration list using 'power' 
   * 				4. Perform getConfigurations()
   *            			using the configuration and a resource from the found resource list
   *            	5. check if OC_STACK_INVALID_PARAM is returned
   * @post_condition None
   * @expected The target API should return OC_STACK_INVALID_PARAM
   **/
  public void testGetConfiguration_NV_N() {

    tmHelper.waitInSecond(tmHelper.CALLBACK_WAIT_DEFAULT);

    Vector<String> configurations = new Vector<String>();
    configurations.add("power");

    try {
      result = tmHelper.thingsConfiguration.getConfigurations(null, configurations);
    } catch (OcException e) {
      result = OCStackResult.OC_STACK_ERROR;
      e.printStackTrace();
    }
    assertEquals("getConfiguration failed!!", OCStackResult.OC_STACK_INVALID_PARAM.toString(),
        result.toString());

  }

  /**
   * @since 2015-03-05
   * @see public synchronized static void Configure(PlatformConfig platformConfig)
   * @see OCStackResult findCandidateResources(Vector<String> resourceTypes,
   *         	int waitTime)
   * @objective Test 'updateConfigurations' function with positive basic way
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
   * 				6. Create a configuration map using 'power' and 'on' 
   * 				7. Perform updateConfigurations()
   *            			using the configuration map and a resource from the found resource list
   *            	8. check if OC_STACK_OK is returned
   * @post_condition Close the linux app and unregister all resources
   * @expected The target API should return OC_STACK_OK
   **/  
  public void testUpdateConfiguration_SRC_P() {

	Vector<String> resourceTypeList = new Vector<String>();
    resourceTypeList.add("core.light");

    tmHelper.resourceList.clear();
    
    int waitTime = -1;
    result = tmHelper.groupManager.findCandidateResources(resourceTypeList, waitTime);
    assertEquals("FindCandidateResources failed!!", OCStackResult.OC_STACK_OK.toString(),
        result.toString());

    tmHelper.waitForResourceFound(tmHelper.CALLBACK_WAIT_MAX);

    Map<String, String> configurations = new HashMap<String, String>();
    configurations.clear();
    configurations.put("power", "on");

    OcResource resource=null;
    try{resource = tmHelper.resourceList.get(tmHelper.INT_ZERO);    
    } catch (ArrayIndexOutOfBoundsException e) {
		e.printStackTrace();
	    fail("resourceList is empty");	    
	  }
    
    assertNotNull("resource is NULL",resource);
    
    try {
      result = tmHelper.thingsConfiguration.updateConfigurations(resource,
          configurations);
    } catch (OcException e) {
      result = OCStackResult.OC_STACK_ERROR;
      e.printStackTrace();
    }    
    
    assertEquals("updateConfigurations failed!!", OCStackResult.OC_STACK_OK.toString(),
        result.toString());

  }

  /**
   * @since 2015-03-05
   * @see public synchronized static void Configure(PlatformConfig platformConfig)
   * @see OCStackResult findCandidateResources(Vector<String> resourceTypes,
   *         	int waitTime)
   * @objective Test 'updateConfigurations' function with negative basic way using null resource
   * @target OCStackResult updateConfigurations(OcResource resource,
   *         	Map<String, String> configurations)
   * @test_data 	1. resource null
   *            	2. configurations map of configurations key and value
   * @pre_condition None
   * @procedure 	1. ThingsConfiguration is instantiated
   * 				2. configuration Listener is added to ThingsConfiguration
   * 				3. Create a configuration map using 'power' and 'on' 
   * 				4. Perform updateConfigurations()
   *            			using the configuration and a null resource
   *            	5. check if OC_STACK_INVALID_PARAM is returned
   * @post_condition None
   * @expected The target API should return OC_STACK_INVALID_PARAM
   **/  
  public void testUpdateConfiguration_NV_N() {

    Map<String, String> configurations = new HashMap<String, String>();
    configurations.clear();
    configurations.put("power", "on");

    try {
      result = tmHelper.thingsConfiguration.updateConfigurations(null,
          configurations);
    } catch (OcException e) {
      result = OCStackResult.OC_STACK_ERROR;
      e.printStackTrace();
    }
    assertEquals("updateConfigurations failed!!", OCStackResult.OC_STACK_INVALID_PARAM.toString(),
        result.toString());

  }

  /**
   * @since 2015-03-05
   * @see public synchronized static void Configure(PlatformConfig platformConfig)
   * @see OCStackResult findCandidateResources(Vector<String> resourceTypes,
   *         	int waitTime)
   * @objective Test 'doBootstrap' function with positive basic way
   * @target OCStackResult doBootstrap()
   * @test_data None
   * @pre_condition BootstrapServer app is running on linux
   * @procedure 	1. ThingsConfiguration is instantiated
   * 				2. configuration Listener is added to ThingsConfiguration
   * 				3. Perform doBootstrap()
   * 				4. check if OC_STACK_OK is returned
   * @post_condition stop BootstrapServer app and unregister all resources
   * @expected The target API should return OC_STACK_OK
   **/
  public void testDoBootstrap_SRC_P() {

    result = tmHelper.thingsConfiguration.doBootstrap();
    assertEquals("doBootstrap failed!!", OCStackResult.OC_STACK_OK.toString(), result.toString());
    
  }

  /**
   * @since 2015-03-05
   * @see public synchronized static void Configure(PlatformConfig platformConfig)
   * @see OCStackResult findCandidateResources(Vector<String> resourceTypes,
   *         	int waitTime)
   * @objective Test 'reboot' function with positive basic way
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
   * 				6. check if OC_STACK_OK is returned
   * @post_condition Close the linux app and unregister all resources
   * @expected The target API should return OC_STACK_OK
   **/
  public void testReboot_SRC_P() {
	
	Vector<String> resourceTypeList = new Vector<String>();
	resourceTypeList.add("core.light");

	tmHelper.resourceList.clear();

	int waitTime = -1;
	result = tmHelper.groupManager.findCandidateResources(resourceTypeList, waitTime);
	assertEquals("FindCandidateResources failed!!", OCStackResult.OC_STACK_OK.toString(),
			result.toString());

    tmHelper.waitForResourceFound(tmHelper.CALLBACK_WAIT_MAX);

    OcResource resource=null;
    try{resource = tmHelper.resourceList.get(tmHelper.INT_ZERO);    
    } catch (ArrayIndexOutOfBoundsException e) {
		e.printStackTrace();
	    fail("resourceList is empty");	    
	  }
    
    assertNotNull("resource is NULL",resource);
    
    try {
      result = tmHelper.thingsMaintenance.reboot(resource);
    } catch (OcException e) {
      result = OCStackResult.OC_STACK_ERROR;
      e.printStackTrace();
    }    
    assertEquals("reboot failed!!", OCStackResult.OC_STACK_OK.toString(), result.toString());
    
  }

  /**
   * @since 2015-03-05
   * @see public synchronized static void Configure(PlatformConfig platformConfig)
   * @see OCStackResult findCandidateResources(Vector<String> resourceTypes,
   *         	int waitTime)
   * @objective Test 'reboot' function with negative basic way using null resource
   * @target OCStackResult reboot(OcResource resource)
   * @test_data resource null
   * @pre_condition None
   * @procedure 	1. ThingsMaintenance is instantiated
   * 				2. Perform reboot() with null resource
   * 				3. check if OC_STACK_INVALID_PARAM is returned
   * @post_condition None
   * @expected The target API should return OC_STACK_INVALID_PARAM
   **/  
  public void testReboot_NV_N() {

    try {
      result = tmHelper.thingsMaintenance.reboot(null);
    } catch (OcException e) {
      result = OCStackResult.OC_STACK_ERROR;
      e.printStackTrace();
    }    
    assertEquals("reboot failed!!", OCStackResult.OC_STACK_INVALID_PARAM.toString(), result.toString());
    
  }

  /**
   * @since 2015-03-05
   * @see public synchronized static void Configure(PlatformConfig platformConfig)
   * @see OCStackResult findCandidateResources(Vector<String> resourceTypes,
   *         	int waitTime)
   * @objective Test 'factoryReset' function with positive basic way
   * @target OCStackResult factoryReset(OcResource resource)
   * @test_data resource reference to resource to be used
   * @pre_condition 1. several types of resources(3 core.light type and 2 core.fan type) running 
   * 					in testserver app in linux
   *  				2. GroupManager is instantiated 
   * 				3. findCandidateResource listener is added to GroupManager
   * @procedure 	1. Create resource type list using core.light type resource
   * 				2. Perform findCandidateResources() using resource type list 
   * 				3. Wait for the findResource callback to get found resource list
   * 				4. ThingsMaintenance is instantiated
   *            	5. Perform factoryReset() using a resource form the found resource list
   *            	6. check if OC_STACK_OK is returned
   * @post_condition Close the linux app and unregister all resources
   * @expected The target API should return OC_STACK_OK
   **/
  public void testFactoryReset_SRC_P() {
	
	Vector<String> resourceTypeList = new Vector<String>();
	resourceTypeList.add("core.light");

	tmHelper.resourceList.clear();

	int waitTime = -1;
	result = tmHelper.groupManager.findCandidateResources(resourceTypeList, waitTime);
	assertEquals("FindCandidateResources failed!!", OCStackResult.OC_STACK_OK.toString(),
		result.toString());

    tmHelper.waitForResourceFound(tmHelper.CALLBACK_WAIT_MAX);

    OcResource resource=null;
    try{resource = tmHelper.resourceList.get(tmHelper.INT_ZERO);    
    } catch (ArrayIndexOutOfBoundsException e) {
    	e.printStackTrace();
	    fail("resourceList is empty");	    
	}
    
    assertNotNull("resource is NULL",resource);
    
    try {
      result = tmHelper.thingsMaintenance.factoryReset(resource);
    } catch (OcException e) {
      result = OCStackResult.OC_STACK_ERROR;
      e.printStackTrace();
    }    
    assertEquals("factoryReset failed!!", OCStackResult.OC_STACK_OK.toString(), result.toString());
    
  }

  /**
   * @since 2015-03-05
   * @see public synchronized static void Configure(PlatformConfig platformConfig)
   * @see OCStackResult findCandidateResources(Vector<String> resourceTypes)
   * @objective Test 'factoryReset' function with negative basic way using null resource
   * @target OCStackResult factoryReset(OcResource resource)
   * @test_data resource null
   * @pre_condition None
   * @procedure 	1. ThingsMaintenance is instantiated
   * 				2. Perform reboot() with null resource
   * 				3. check if OC_STACK_INVALID_PARAM is returned
   * @post_condition None
   * @expected The target API should return OC_STACK_INVALID_PARAM
   **/
  public void testFactoryReset_NV_N() {
    
    try {
      result = tmHelper.thingsMaintenance.factoryReset(null);
    } catch (OcException e) {
      result = OCStackResult.OC_STACK_ERROR;
      e.printStackTrace();
    }    
    assertEquals("factoryReset failed!!", OCStackResult.OC_STACK_INVALID_PARAM.toString(), result.toString());
  }

}
