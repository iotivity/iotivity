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

import java.util.Map;
import java.util.Vector;

import org.iotivity.base.ModeType;
import org.iotivity.base.OcException;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcResourceHandle;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;
import org.iotivity.base.OcResource;
import org.iotivity.service.tm.OCStackResult;
import org.iotivity.service.tm.test.helper.TMHelper;

import android.test.InstrumentationTestCase;
import android.util.Log;

public class TMGroupSynchronizationTest extends InstrumentationTestCase  {

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
   * @since 2015-03-06
   * @see public synchronized static void Configure(PlatformConfig platformConfig)
   * @see OCStackResult findCandidateResources(Vector<String> resourceTypes,
   *      	int waitTime)
   * @objective Test 'bindResourceToGroup' function with positive basic way
   * @target OcResourceHandle bindResourceToGroup(OcResource resource,
   *            OcResourceHandle collectionHandle)
   * @test_data 	1. resource resource for register and bind to group. It has all data.
   * 				2. collectionHandle null
   * @pre_condition 1. several types of resources(3 core.light type and 2 core.fan type) running 
   * 					in testserver app in linux
   *  	            2. 3 collections(core.room-large, 
   * 					core.room-medium & core.room-small) are running in testcollection app in linux
   *  				3. GroupManager is instantiated 
   * 				4. findCandidateResource listener is added to GroupManager
   * @procedure 	1. Create resource type list using core.light type resource
   * 				2. Perform findCandidateResources() using resource type list 
   * 				3. Wait for the findResource callback to get found resource list
   * 				4. GroupManager is instantiated
   * 				5. Perform bindResourceToGroup() with found resource and null collectionHandle
   * 				6. check if OC_STACK_ERROR is returned
   * @post_condition Close the linux apps and unregister all resources
   * @expected The target API should return OC_STACK_ERROR
   **/
  public void testBindResourceToGroup_NV_N() {
	
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
    
    OcResourceHandle childHandle = null;

    try {
      childHandle = tmHelper.groupManager.bindResourceToGroup(resource,
          null);
      if (childHandle != null) {
        result = OCStackResult.OC_STACK_OK;
      } else {
        result = OCStackResult.OC_STACK_ERROR;
      }
    } catch (OcException e) {
      e.printStackTrace();
      fail("Exception occured while calling bindResourceToGroup");
    }    

    assertEquals("bindResourceToGroup failed!!", OCStackResult.OC_STACK_ERROR.toString(), result.toString());

  }

}
