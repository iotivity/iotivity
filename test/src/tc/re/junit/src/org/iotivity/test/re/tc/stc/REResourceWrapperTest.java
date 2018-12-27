//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
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

package org.iotivity.test.re.tc.stc;

import org.iotivity.base.ModeType;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;
import org.iotivity.service.RcsException;
import org.iotivity.service.client.RcsAddress;
import org.iotivity.service.client.RcsDiscoveryManager;
import org.iotivity.service.client.RcsRemoteResourceObject;
import org.iotivity.service.client.RcsDiscoveryManager.OnResourceDiscoveredListener;
import org.iotivity.test.re.tc.helper.REAPIHelper;
import static org.iotivity.test.re.tc.helper.ResourceUtil.*;
import static org.iotivity.test.re.tc.helper.ResourceProperties.*;
import android.test.InstrumentationTestCase;

import android.util.Log;
import java.util.ArrayList;

public class REResourceWrapperTest extends InstrumentationTestCase {
  private static final String     LOG_TAG    = "RETest";
  private StringBuilder           m_ErrorMsg = new StringBuilder();
  private REAPIHelper             m_REHelper;
  private RcsRemoteResourceObject m_Resource;
  private ArrayList<RcsRemoteResourceObject> m_ResourceList = new ArrayList<RcsRemoteResourceObject>();
  boolean callbackcheck = false;

  protected void setUp() throws Exception {
    super.setUp();
    callbackcheck = false;
    m_REHelper = new REAPIHelper();
    m_ErrorMsg.setLength(0);

    PlatformConfig platformConfigObj = new PlatformConfig(
        getInstrumentation().getTargetContext(), ServiceType.IN_PROC,
        ModeType.CLIENT_SERVER, "0.0.0.0", 0, QualityOfService.LOW);

    OcPlatform.Configure(platformConfigObj);
    Log.i(LOG_TAG, "Configuration done Successfully");
  }

  private OnResourceDiscoveredListener mOnResourceDiscoveredListener = new OnResourceDiscoveredListener() {

    @Override
    public void onResourceDiscovered(RcsRemoteResourceObject foundResource) {
      Log.i(LOG_TAG, "onResourceDiscovered in API test case");
      m_ResourceList.add(foundResource);
      callbackcheck = true;
    }
  };

  protected void tearDown() throws Exception {
    super.tearDown();
  }

  /**
   * @since 2015-11-05
   * @see None
   * @objective Test 'getUri' function with positive basic way
   * @target string getUri()
   * @test_data None
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure Perform getUri() API with TEMP_SENSOR_URI and LIGHT_URI
   * @post_condition None
   * @expected Returned uri should not be empty
   **/
  public void testGetUri_SQV_P() {
    if (!m_REHelper.disocverResources(m_ErrorMsg)) {
      fail("Precondition Failed, No Resource Found!! " + m_ErrorMsg.toString());
      return;
    } else {
      Log.d(LOG_TAG, m_ErrorMsg.toString());

      m_Resource = m_REHelper.getFoundResourceList().get(0);
    }

    String uri = "";
    if (m_Resource != null) {
      try {
        uri = m_Resource.getUri();
      } catch (RcsException e) {
        fail("Throws exception when getUri() API called: "
            + e.getLocalizedMessage());
        return;
      }

      if (uri.compareTo(TEMP_SENSOR_URI) != 0 && uri.compareTo(LIGHT_URI) != 0
          && uri.compareTo(FAN_URI) != 0) {
        fail("Expected uri: " + TEMP_SENSOR_URI + " Actual: " + uri);
      }
    }
  }

  /**
   * @since 2015-11-05
   * @see None
   * @objective Test 'getInterfaces' function with BASELINE_INTERFACE
   * @target String[] getInterfaces()
   * @test_data None
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure Perform getInterfaces() API
   * @post_condition None
   * @expected 1. Returned interface list should not be empty 2. At least one
   *           interface should not be an empty string
   **/
  public void testGetInterfaces_SQV_P() {
    if (!m_REHelper.disocverResources(m_ErrorMsg)) {
      fail("Precondition Failed, No Resource Found!! " + m_ErrorMsg.toString());
      return;
    } else {
      Log.d(LOG_TAG, m_ErrorMsg.toString());

      m_Resource = m_REHelper.getFoundResourceList().get(0);
    }

    try {
      String[] interfaces = m_Resource.getInterfaces();

      if (interfaces.length > 0) {
        boolean foundInterface = false;
        int i = 0;

        for (i = 0; i < interfaces.length; i++) {
          if (interfaces[i].compareTo(DEFAULT_INTERFACE) == 0
              || interfaces[i].compareTo(BASELINE_INTERFACE) == 0) {
            foundInterface = true;
            break;
          }
        }

        if (foundInterface == false) {
          fail("Could not found expected interface: " + DEFAULT_INTERFACE
              + " or " + BASELINE_INTERFACE);
        }
      } else {
        fail("Unable to get interfaces");
      }
    } catch (RcsException e) {
      fail("Throws exception when getInterfaces API called. Exception: "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-11-06
   * @see None
   * @objective Test 'getTypes' function with LIGHT_TYPE, FAN_TYPE,
   *            TEMP_SENSOR_TYPE
   * @target std::vector< std::string > getTypes()
   * @test_data None
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure Perform getTypes() API
   * @post_condition None
   * @expected 1. Returned type list should not be empty 2. At least one type
   *           should not be an empty string
   **/
  public void testGetTypes_SQV_P() {
    if (!m_REHelper.disocverResources(m_ErrorMsg)) {
      fail("Precondition Failed, No Resource Found!! " + m_ErrorMsg.toString());
      return;
    } else {
      Log.d(LOG_TAG, m_ErrorMsg.toString());
      m_Resource = m_REHelper.getFoundResourceList().get(0);
    }

    try {
      String[] types = m_Resource.getTypes();

      if (types.length > 0) {
        boolean foundType = false;
        int i = 0;

        for (i = 0; i < types.length; i++) {
          if (types[i].compareTo(LIGHT_TYPE) == 0
              || types[i].compareTo(FAN_TYPE) == 0
              || types[i].compareTo(TEMP_SENSOR_TYPE) == 0) {
            foundType = true;
            break;
          }
        }

        if (foundType == false) {
          fail("Could not found expected type: " + LIGHT_TYPE + " or "
              + FAN_TYPE + " or " + TEMP_SENSOR_TYPE);
        }
      } else {
        fail("Unable to get types");
      }
    } catch (RcsException e) {
      fail("Throws exception when getInterfaces API called. Exception: "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2016-07-25
   * @see None
   * @objective Test 'discoverResource' function with null callback
   * @target public DiscoveryTask discoverResource(RcsAddress address, String
   *         uri, OnResourceDiscoveredListener listener)
   * @test_data 1. Resource Address 2. Resource uri 3. null Callback
   * @pre_condition None
   * @procedure Perform discoverResource() API with null callback
   * @post_condition None
   * @expected API should throw NullPointerException
   **/
  public void testDiscoverResourceWithURI_ETC_N() {
    boolean isNullException = false;
    try {

      RcsDiscoveryManager.DiscoveryTask discoveryTask = RcsDiscoveryManager
          .getInstance().discoverResource(RcsAddress.multicast(),
              OC_RSRVD_WELL_KNOWN_URI, null);

      m_REHelper.waitInSecond(CALLBACK_WAIT_MAX);

      discoveryTask.cancel();

    } catch (NullPointerException e) {
      Log.e(LOG_TAG,
          "Exception occurs at testDiscoverResourceWithURI_ETC_N as expected. Exception: "
              + e.getLocalizedMessage());
      isNullException = true;
    } catch (RcsException e) {
      fail("Throws exception when getInterfaces API called. Exception: "
          + e.getLocalizedMessage());
    } finally {
      if (!isNullException)
        fail("Should throw NullPointerException");
    }
  }

  /**
   * @since 2016-07-25
   * @see None
   * @objective Test 'discoverResource' function with Null Address
   * @target public DiscoveryTask discoverResource(RcsAddress address, String
   *         uri, OnResourceDiscoveredListener listener)
   * @test_data 1. Null Resource Address 2. Resource URI 3.
   *            OnResourceDiscoveredListener Callback
   * @pre_condition None
   * @procedure Perform discoverResource() API with Null Address
   * @post_condition None
   * @expected Should throw NullPointerException if address is null
   **/
  public void testDiscoverResourceWithURIAddress_ETC_N() {
    boolean isNullException = false;
    try {

      RcsDiscoveryManager.DiscoveryTask discoveryTask = RcsDiscoveryManager
          .getInstance().discoverResource(null, OC_RSRVD_WELL_KNOWN_URI,
              mOnResourceDiscoveredListener);

      m_REHelper.waitInSecond(CALLBACK_WAIT_MAX);
      discoveryTask.cancel();

    } catch (NullPointerException e) {
      Log.e(LOG_TAG,
          "Exception occurs at testDiscoverResourceWithURIAddress_ETC_N as expected. Exception: "
              + e.getLocalizedMessage());
      isNullException = true;
    } catch (RcsException e) {
      fail("Throws exception when getInterfaces API called. Exception: "
          + e.getLocalizedMessage());
    } finally {
      if (!isNullException)
        fail("Should throw NullPointerException");
    }
  }

  /**
   * @since 2016-07-25
   * @see None
   * @objective Test 'discoverResourceByType' function with null callback
   * @target public DiscoveryTask discoverResourceByType(RcsAddress address,
   *         String resourceType, OnResourceDiscoveredListener listener)
   * @test_data 1. Resource Address 2. Resource Type 3. Null Callback
   * @pre_condition None
   * @procedure Perform discoverResourceByType() API with null callback
   * @post_condition None
   * @expected Should throw NullPointerException if listener is null.
   **/
  public void testDiscoverResourceByTypeForMulticast_ETC_N() {
    boolean isNullException = false;
    try {

      RcsDiscoveryManager.DiscoveryTask discoveryTask = RcsDiscoveryManager
          .getInstance().discoverResourceByType(RcsAddress.multicast(),
              RESOURCE_TYPE_TEMPERATURE, null);

      m_REHelper.waitInSecond(CALLBACK_WAIT_MAX);
      discoveryTask.cancel();

    } catch (NullPointerException e) {
      Log.e(LOG_TAG,
          "Exception occurs at testDiscoverResourceByTypeForMulticast_ETC_N as expected. Exception: "
              + e.getLocalizedMessage());
      isNullException = true;
    } catch (RcsException e) {
      fail("Throws exception when getInterfaces API called. Exception: "
          + e.getLocalizedMessage());
    } finally {
      if (!isNullException)
        fail("Should throw NullPointerException");
    }
  }

  /**
   * @since 2017-03-07
   * @see None
   * @objective Test 'discoverResource' function multiple times
   * @target public DiscoveryTask discoverResource(RcsAddress address, String
   *         uri, OnResourceDiscoveredListener listener)
   * @test_data None
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure Perform discoverResource() API multiple times
   * @post_condition None
   * @expected returned discovery manager should not be null
   **/
  public void testDiscoverResourceMultipleTimes_VLCC_P() {
    for (int i = 1; i <= 10; i++) {
      if (!m_REHelper.disocverResources(m_ErrorMsg)) {
        fail("Failed, No Resource Found!! " + m_ErrorMsg.toString());
      } else {
        Log.d(LOG_TAG, m_ErrorMsg.toString());
        if(m_REHelper.getFoundResourceList().size() == 0 ){
          fail("testDiscoverResourceMultipleTimes_VLCC_P Failed for for iteration no. "+i);
        }
      }
    }
  }

  /**
   * @since 2017-03-07
   * @see None
   * @objective Test 'discoverResource' function with state transition loop check
   * @target public DiscoveryTask discoverResource(RcsAddress address, String
   *         uri, OnResourceDiscoveredListener listener)
   * @test_data 1. host
   *            2. uri
   *            3. Callback
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure Perform discoverResource() API multiple times Without Waiting
   * @post_condition Cancel Discovery Task
   * @expected The API should not crash
   **/
  public void testDiscoverResourceMultipleTimesWithoutWaiting_STLC_N() {
    int i = 0;
    try {
      
      for (i = 1; i <= 10; i++) {
        RcsDiscoveryManager.DiscoveryTask discoveryTask = RcsDiscoveryManager
            .getInstance().discoverResource(RcsAddress.multicast(), OC_RSRVD_WELL_KNOWN_URI,
                mOnResourceDiscoveredListener);
        discoveryTask.cancel();
      }
    } catch (Exception e) {
      fail("Iteration:"+i+" Throws exception when discoverResource API called. Exception: "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2017-03-07
   * @see None
   * @objective Test 'discoverResource' function with sequential validation
   * @target public DiscoveryTask discoverResource(RcsAddress address, String
   *         uri, OnResourceDiscoveredListener listener)
   * @test_data 1. host
   *            2. uri
   *            3. Callback
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure Perform discoverResource() API
   * @post_condition Cancel Discovery Task
   * @expected returned discovery manager should not be null
   **/
  public void testDiscoverAllResources_SQV_P() {
    try {
        m_ResourceList.clear();
        RcsDiscoveryManager.DiscoveryTask discoveryTask = RcsDiscoveryManager
            .getInstance().discoverResource(RcsAddress.multicast(), OC_RSRVD_WELL_KNOWN_URI,
                mOnResourceDiscoveredListener);
        m_REHelper.waitInSecond(CALLBACK_WAIT_MAX);
        discoveryTask.cancel();
        if( m_ResourceList.size() < 2 ){
          fail("All Resources are not Found!!");
        }
    } catch (Exception e) {
      fail("Throws exception when discoverResource API called. Exception: "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2017-03-07
   * @see None
   * @objective Test 'discoverResourceByType' function with callback condition checking
   * @target public DiscoveryTask discoverResourceByType(RcsAddress address,
   *         String uri, String resourceType, OnResourceDiscoveredListener
   *         listener)
   * @test_data 1. Resource Address 2. Resource Type 3. Callback
   * @pre_condition None
   * @procedure Perform discoverResourceByType() API
   * @post_condition None
   * @expected returned discovery manager should not be null
   **/
  public void testDiscoverResourceByTypeWithoutURI_CCC_P() {
    try {
      RcsDiscoveryManager.DiscoveryTask discoveryTask = RcsDiscoveryManager
          .getInstance().discoverResourceByType(RcsAddress.multicast()
              , LIGHT_TYPE, mOnResourceDiscoveredListener);

      m_REHelper.waitInSecond(CALLBACK_WAIT_MAX);

      discoveryTask.cancel();
      if( m_ResourceList.size() < 1 ){
        fail("Resources are not Found!!");
      }
    } catch (RcsException e) {
      fail("Throws exception when called discoverResourceByType API with uri. "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2017-03-07
   * @see None
   * @objective Test 'discoverResourceByType' function twice
   * @target public DiscoveryTask discoverResourceByType(RcsAddress address,
   *         String uri, String resourceType, OnResourceDiscoveredListener
   *         listener)
   * @test_data 1. Resource Address 2. Resource Type 3. Callback
   * @pre_condition None
   * @procedure Perform discoverResourceByType() API twice
   * @post_condition None
   * @expected returned discovery manager should not be null
   **/
  public void testDiscoverResourceByTypeWithoutURI_CACC_P() {
    try {
      RcsDiscoveryManager.DiscoveryTask discoveryTask = RcsDiscoveryManager
          .getInstance().discoverResourceByType(RcsAddress.multicast()
              , LIGHT_TYPE, mOnResourceDiscoveredListener);
      m_REHelper.waitInSecond(CALLBACK_WAIT_MAX);
      discoveryTask.cancel();

      RcsDiscoveryManager.DiscoveryTask discoveryTask2 = RcsDiscoveryManager
          .getInstance().discoverResourceByType(RcsAddress.multicast()
              , LIGHT_TYPE, mOnResourceDiscoveredListener);
      m_REHelper.waitInSecond(CALLBACK_WAIT_MAX);
      discoveryTask2.cancel();

      if( m_ResourceList.size() < 1 ){
        fail("Resources are not Found!!");
      }
    } catch (RcsException e) {
      fail("Throws exception when called discoverResourceByType API with uri. "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2017-03-07
   * @see None
   * @objective Test 'discoverResourceByType' function multiple times
   * @target public DiscoveryTask discoverResourceByType(RcsAddress address,
   *         String uri, String resourceType, OnResourceDiscoveredListener
   *         listener)
   * @test_data 1. Resource Address 2. Resource Type 3. Callback
   * @pre_condition None
   * @procedure Perform discoverResourceByType() API repeatedly
   * @post_condition None
   * @expected returned discovery manager should not be null
   **/
  public void testDiscoverResourceByTypeWithoutURI_ALVC_P() {
    try {
      for (int i = 1; i <= 10; i++) {
        RcsDiscoveryManager.DiscoveryTask discoveryTask = RcsDiscoveryManager
            .getInstance().discoverResourceByType(RcsAddress.multicast()
                , RESOURCE_TYPE_TEMPERATURE, mOnResourceDiscoveredListener);
        discoveryTask.cancel();
      }
    } catch (RcsException e) {
      fail("Throws exception when called discoverResourceByType API with uri. "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2017-03-07
   * @see None
   * @objective Test 'discoverResourceByType' function twice with URI
   * @target public DiscoveryTask discoverResourceByType(RcsAddress address,
   *         String uri, String resourceType, OnResourceDiscoveredListener
   *         listener)
   * @test_data 1. Resource Address 2. Resource Type 3. Callback
   * @pre_condition None
   * @procedure Perform discoverResourceByType() API twice
   * @post_condition None
   * @expected returned discovery manager should not be null
   **/
  public void testDiscoverResourceByType_CACC_P() {
    try {
      RcsDiscoveryManager.DiscoveryTask discoveryTask = RcsDiscoveryManager
          .getInstance().discoverResourceByType(RcsAddress.multicast(),
              OC_RSRVD_WELL_KNOWN_URI, LIGHT_TYPE, mOnResourceDiscoveredListener);
      m_REHelper.waitInSecond(CALLBACK_WAIT_MAX);
      discoveryTask.cancel();

      RcsDiscoveryManager.DiscoveryTask discoveryTask2 = RcsDiscoveryManager
          .getInstance().discoverResourceByType(RcsAddress.multicast(),
              OC_RSRVD_WELL_KNOWN_URI, LIGHT_TYPE, mOnResourceDiscoveredListener);
      m_REHelper.waitInSecond(CALLBACK_WAIT_MAX);
      discoveryTask2.cancel();

      if( m_ResourceList.size() < 1 ){
        fail("Resources are not Found!!");
      }
    } catch (RcsException e) {
      fail("Throws exception when called discoverResourceByType API with uri. "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2017-03-07
   * @see None
   * @objective Test 'discoverResourceByType' function twice with URI
   * @target public DiscoveryTask discoverResourceByType(RcsAddress address,
   *         String uri, String resourceType, OnResourceDiscoveredListener
   *         listener)
   * @test_data 1. Resource Address 2. Resource Type 3. Callback
   * @pre_condition None
   * @procedure Perform discoverResourceByType() API twice
   * @post_condition None
   * @expected returned discovery manager should not be null
   **/
  public void testDiscoverResourceByTypeWithInvalidResType_EG_N() {
    try {
      callbackcheck = false;
      String resType="Hello";
      RcsDiscoveryManager.DiscoveryTask discoveryTask = RcsDiscoveryManager
          .getInstance().discoverResourceByType(RcsAddress.multicast()
              , OC_RSRVD_WELL_KNOWN_URI, resType, mOnResourceDiscoveredListener);
      m_REHelper.waitInSecond(CALLBACK_WAIT_MAX);
      discoveryTask.cancel();

      if( callbackcheck ){
        fail("Callback Invoked !");
      }
    } catch (RcsException e) {
      fail("Throws exception when called discoverResourceByType API with uri. "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2017-03-07
   * @see None
   * @objective Test 'discoverResourceByType' function multiple times
   * @target public DiscoveryTask discoverResourceByType(RcsAddress address,
   *         String uri, String resourceType, OnResourceDiscoveredListener
   *         listener)
   * @test_data 1. Resource Address 2. Resource Type 3. Callback
   * @pre_condition None
   * @procedure Perform discoverResourceByType() API twice
   * @post_condition None
   * @expected returned discovery manager should not be null
   **/
  public void testDiscoverResourceByType_ALVC_P() {
    try {
      for (int i = 1; i <= 10; i++) {
        RcsDiscoveryManager.DiscoveryTask discoveryTask = RcsDiscoveryManager
            .getInstance().discoverResourceByType(RcsAddress.multicast()
                , OC_RSRVD_WELL_KNOWN_URI, RESOURCE_TYPE_TEMPERATURE, mOnResourceDiscoveredListener);
        discoveryTask.cancel();
      }
    } catch (RcsException e) {
      fail("Throws exception when called discoverResourceByType API with uri. "
          + e.getLocalizedMessage());
    }
  }

}
