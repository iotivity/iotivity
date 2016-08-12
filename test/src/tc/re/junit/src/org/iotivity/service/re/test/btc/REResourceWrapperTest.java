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

package org.iotivity.service.re.test.btc;

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
import org.iotivity.service.re.test.helper.REHelper;
import static org.iotivity.service.re.test.helper.ResourceProperties.*;
import static org.iotivity.service.re.test.helper.ResourceUtil.*;
import android.test.InstrumentationTestCase;
import android.util.Log;

public class REResourceWrapperTest extends InstrumentationTestCase {
  private static final String     LOG_TAG    = "RETest";
  private StringBuilder           m_ErrorMsg = new StringBuilder();
  private REHelper                m_REHelper;
  private RcsRemoteResourceObject m_Resource;

  protected void setUp() throws Exception {
    super.setUp();

    m_REHelper = new REHelper();
    m_ErrorMsg.setLength(0);

    PlatformConfig platformConfigObj = new PlatformConfig(getInstrumentation()
        .getTargetContext(), ServiceType.IN_PROC, ModeType.CLIENT_SERVER,
        "0.0.0.0", 0, QualityOfService.LOW);

    OcPlatform.Configure(platformConfigObj);
    Log.i(LOG_TAG, "Configuration done Successfully");
  }

  private OnResourceDiscoveredListener mOnResourceDiscoveredListener = new OnResourceDiscoveredListener() {

    @Override
    public void onResourceDiscovered(
        RcsRemoteResourceObject foundResource) {
      Log.i(
          LOG_TAG,
          "onResourceDiscovered in test case");
    }
  };

  /**
   * @since 2015-10-22
   * @see None
   * @objective Test 'getUri' function with positive basic way
   * @target String getUri()
   * @test_data None
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure Perform getUri() API
   * @post_condition None
   * @expected Returned uri should not be empty
   **/
  public void testGetUri_SRC_P() {
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
        fail("Throws exception when getUri() API called.");
        return;
      }

      if (uri.compareTo("") == 0) {
        fail("Unable to get uri");
      } else {
        Log.d(LOG_TAG, "Got uri: " + uri);
      }
    }
  }

  /**
   * @since 2015-10-22
   * @see None
   * @objective Test 'getAddress' function with positive basic way
   * @target String getAddress()
   * @test_data None
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure Perform getAddress() API
   * @post_condition None
   * @expected Returned address should not be empty
   **/
  public void testGetAddress_SRC_P() {
    if (!m_REHelper.disocverResources(m_ErrorMsg)) {
      fail("Precondition Failed, No Resource Found!! " + m_ErrorMsg.toString());
      return;
    } else {
      Log.d(LOG_TAG, m_ErrorMsg.toString());

      m_Resource = m_REHelper.getFoundResourceList().get(0);
    }

    String address = "";

    if (m_Resource != null) {
      try {
        address = m_Resource.getAddress();
      } catch (RcsException e) {
        fail("Throws exception when getAddress() API called.");
        return;
      }

      if (address.compareTo("") == 0) {
        fail("Unable to get address");
      } else {
        Log.d(LOG_TAG, "Got address: " + address);
      }
    }
  }

  /**
   * @since 2015-10-22
   * @see None
   * @objective Test 'getTypes' function with positive basic way
   * @target String[] getTypes()
   * @test_data None
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure Perform getTypes() API
   * @post_condition None
   * @expected 1. Returned type list should not be empty 2. At least one type
   *           should not be an empty string
   **/
  public void testGetTypes_SRC_P() {
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
        if (types[0].compareTo("") == 0) {
          fail("Unable to get any type");
        } else {
          for (int i = 0; i < types.length; i++) {
            Log.d(LOG_TAG, "Got type: " + types[i]);
          }
        }
      } else {
        fail("Unable to get types");
      }
    } catch (RcsException e) {
      fail("Throws exception when getTypes API called. Exception: "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-10-22
   * @see None
   * @objective Test 'getInterfaces' function with positive basic way
   * @target String[] getInterfaces()
   * @test_data None
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure Perform getInterfaces() API
   * @post_condition None
   * @expected 1. Returned interface list should not be empty 2. At least one
   *           interface should not be an empty string
   **/
  public void testGetInterfaces_SRC_P() {
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
        if (interfaces[0].compareTo("") == 0) {
          fail("Unable to get any interface");
        } else {
          for (int i = 0; i < interfaces.length; i++) {
            Log.d(LOG_TAG, "Got interface: " + interfaces[i]);
          }
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
   * @since 2015-10-22
   * @see None
   * @objective Test 'getInstance' function with positive basic way
   * @target DiscoveryManager* getInstance()
   * @test_data None
   * @pre_condition None
   * @procedure Perform getInstance() API
   * @post_condition None
   * @expected returned discovery manager should not be null
   **/
  public void testGetInstance_SRC_P() {
    RcsDiscoveryManager discoveryManager = RcsDiscoveryManager.getInstance();

    if (discoveryManager == null) {
      fail("Unable to get instance of discovery manager");
    }
  }

  /**
   * @since 2015-10-22
   * @see None
   * @objective Test 'discoverResource' function with positive basic way
   * @target public DiscoveryTask discoverResource(RcsAddress address,
   *         OnResourceDiscoveredListener listener)
   * @test_data 1. Resource Address 2. Callback
   * @pre_condition None
   * @procedure Perform discoverResource() API
   * @post_condition None
   * @expected returned discovery manager should not be null
   **/
  public void testDiscoverResource_SRC_P() {
    try {
      RcsDiscoveryManager.DiscoveryTask discoveryTask = RcsDiscoveryManager
          .getInstance().discoverResource(RcsAddress.multicast(),
              mOnResourceDiscoveredListener);

      m_REHelper.waitInSecond(CALLBACK_WAIT_MAX);

      discoveryTask.cancel();
    } catch (RcsException e) {
      fail("Throws exception when called discoverResource API. "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-10-22
   * @see None
   * @objective Test 'discoverResource' function with Null Callback
   * @target public DiscoveryTask discoverResource(RcsAddress address,
   *         OnResourceDiscoveredListener listener)
   * @test_data 1. Resource Address 2. Null Callback
   * @pre_condition None
   * @procedure Perform discoverResource() API
   * @post_condition None
   * @expected Should throw RcsException.
   **/
  public void testDiscoverResource_NV_N() {
    try {
      RcsDiscoveryManager.DiscoveryTask discoveryTask = RcsDiscoveryManager
          .getInstance().discoverResource(RcsAddress.multicast(), null);

      m_REHelper.waitInSecond(CALLBACK_WAIT_MIN);

      fail("discoverResource API called successfully when it called without callback function.");

      discoveryTask.cancel();
    } catch (NullPointerException e) {
      Log.d(
          LOG_TAG,
          "Throws exception when called discoverResource API without callback. Exception: "
              + e.getLocalizedMessage());
    } catch (RcsException e) {
      fail("Should throw NullPointerException exception but got RcsException when called discoverResource API without callback. "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2016-02-24
   * @see None
   * @objective Test 'discoverResource' function with Null Resource Address
   * @target DiscoveryTask discoverResource(RcsAddress address,
   *         OnResourceDiscoveredListener listener)
   * @test_data 1. Null Resource Address 2. Callback
   * @pre_condition None
   * @procedure Perform discoverResource() API
   * @post_condition DiscoveryTask should be destroyed if called successfully.
   * @expected Should throw RcsException.
   **/
  public void testDiscoverResourceAddress_NV_N() {
    try {
      RcsDiscoveryManager.DiscoveryTask discoveryTask = RcsDiscoveryManager
          .getInstance().discoverResource(null, mOnResourceDiscoveredListener);

      m_REHelper.waitInSecond(CALLBACK_WAIT_MIN);

      fail("discoverResource API called successfully when it called with null Address.");

      discoveryTask.cancel();
    } catch (NullPointerException e) {
    } catch (RcsException e) {
      fail("Should throw NullPointerException");
    }
  }

  /**
   * @since 2016-02-24
   * @see None
   * @objective Test 'discoverResource' function with Null Resource Address and
   *            Null Callback
   * @target DiscoveryTask discoverResource(RcsAddress address,
   *         OnResourceDiscoveredListener listener)
   * @test_data 1. Null Resource Address 2. Null Callback
   * @pre_condition None
   * @procedure Perform discoverResource() API
   * @post_condition DiscoveryTask should be destroyed if called successfully.
   * @expected Should throw RcsException.
   **/
  public void testDiscoverResourceAddressAndCallback_NV_N() {
    try {
      RcsDiscoveryManager.DiscoveryTask discoveryTask = RcsDiscoveryManager
          .getInstance().discoverResource(null, null);

      m_REHelper.waitInSecond(CALLBACK_WAIT_MIN);

      fail("discoverResource API called successfully when it called with null Address and null callback.");

      discoveryTask.cancel();
    } catch (NullPointerException e) {
    } catch (RcsException e) {
      fail("Should throw NullPointerException");
    }
  }

  /**
   * @since 2016-02-24
   * @see None
   * @objective Test 'discoverResource' function with multicast Resource Address
   * @target DiscoveryTask discoverResource(RcsAddress address,
   *         OnResourceDiscoveredListener listener)
   * @test_data 1. Resource Address 2. Callback
   * @pre_condition Prepare RcsAddress for Multicast
   * @procedure Perform discoverResource() API
   * @post_condition DiscoveryTask should be destroyed if called successfully.
   * @expected Should not throw RcsException.
   **/
  public void testDiscoverResourceMulticast_FSV_P() {
    try {
      RcsAddress rcsAddress = RcsAddress.multicast();

      RcsDiscoveryManager.DiscoveryTask discoveryTask = RcsDiscoveryManager
          .getInstance().discoverResource(rcsAddress,
              mOnResourceDiscoveredListener);

      m_REHelper.waitInSecond(CALLBACK_WAIT_MIN);

      discoveryTask.cancel();
    } catch (RcsException e) {
      fail("Exception occurs at TestDiscoverResourceMulticast_FSV_P. Exception: "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-10-22
   * @see None
   * @objective Test 'discoverResource' function with positive basic way
   * @target public DiscoveryTask discoverResource(RcsAddress address, String
   *         uri, OnResourceDiscoveredListener listener)
   * @test_data 1. Resource Address 2. Resource uri 3. Callback
   * @pre_condition None
   * @procedure Perform discoverResource() API
   * @post_condition None
   * @expected returned discovery manager should not be null
   **/
  public void testDiscoverResourceWithURI_SRC_P() {
    try {
      RcsDiscoveryManager.DiscoveryTask discoveryTask = RcsDiscoveryManager
          .getInstance().discoverResource(RcsAddress.multicast(),
              OC_RSRVD_WELL_KNOWN_URI, mOnResourceDiscoveredListener);

      m_REHelper.waitInSecond(CALLBACK_WAIT_MAX);

      discoveryTask.cancel();
    } catch (RcsException e) {
      fail("Throws exception when called discoverResource API. "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2016-02-24
   * @see None
   * @objective Test 'discoverResource' function with empty Resource URI
   * @target public DiscoveryTask discoverResourceByType(RcsAddress address,
   *         String uri, OnResourceDiscoveredListener listener)
   * @test_data 1. Resource Address 2. Resource URI = EMPTY_STRING_VALUE 3.
   *            Callback
   * @pre_condition None
   * @procedure Perform discoverResourceByType() API
   * @post_condition None
   * @expected Should throw RcsException.
   **/
  public void testDiscoverResourceWithURI_ESV_N() {
    try {
      RcsDiscoveryManager.DiscoveryTask discoveryTask = RcsDiscoveryManager
          .getInstance().discoverResource(RcsAddress.multicast(),
              EMPTY_STRING_VALUE, mOnResourceDiscoveredListener);

      m_REHelper.waitInSecond(CALLBACK_WAIT_MAX);

      discoveryTask.cancel();
    } catch (RcsException e) {
      fail("Throws exception when called discoverResourceByType API with empty type. "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-10-22
   * @see None
   * @objective Test 'discoverResource' function with Null Resource uri
   * @target public DiscoveryTask discoverResource(RcsAddress address, String
   *         uri, OnResourceDiscoveredListener listener)
   * @test_data 1. Resource Address 2. Null Resource uri 3. Callback
   * @pre_condition None
   * @procedure Perform discoverResource() API
   * @post_condition None
   * @expected Should throw NullPointerException
   **/
  public void testDiscoverResourceWithURI_NV_N() {
    try {
      RcsDiscoveryManager.DiscoveryTask discoveryTask = RcsDiscoveryManager
          .getInstance().discoverResource(RcsAddress.multicast(), null,
              mOnResourceDiscoveredListener);

      m_REHelper.waitInSecond(CALLBACK_WAIT_MAX);
      discoveryTask.cancel();

    } catch (NullPointerException enull) {
      Log.d(LOG_TAG, "Throws exception when called discoverResource API. "
          + enull.getLocalizedMessage());
      fail("Throws exception when discoverResource API called with null uri."
          + enull.getLocalizedMessage());
    } catch (RcsException e) {
      fail("Should throw NullPointerException exception when when called discoverResource API with null uri. "
          + e.getLocalizedMessage());
      fail("Throws exception when discoverResource API called with null uri."
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2016-02-24
   * @see None
   * @objective Test 'discoverResource' function with Unicast Resource Address
   *            and Uri
   * @target public DiscoveryTask discoverResource(RcsAddress address, String
   *         uri, OnResourceDiscoveredListener listener)
   * @test_data 1. Resource Address 2. Resource URI 3. Callback
   * @pre_condition Prepare RcsAddress for unicast
   * @procedure Perform discoverResource() API
   * @post_condition None
   * @expected RcsException should not throw.
   **/
  public void testDiscoverResourceWithURIUnicast_FSV_P() {
    try {
      RcsDiscoveryManager.DiscoveryTask discoveryTask = RcsDiscoveryManager
          .getInstance().discoverResource(
              REHelper.getDefaultRcsAddressForUnicast(),
              OC_RSRVD_WELL_KNOWN_URI, mOnResourceDiscoveredListener);

      m_REHelper.waitInSecond(CALLBACK_WAIT_MAX);

      discoveryTask.cancel();
    } catch (RcsException e) {
      fail("Exception occurs at discoverResource API called successfully with uri for unicast.");
    } catch (Exception e) {
      fail("Exception occurs at discoverResource API called successfully with uri for unicast.");
    }
  }

  /**
   * @since 2016-02-24
   * @see None
   * @objective Test 'discoverResource' function with Null Resource Address and
   *            Uri
   * @target public DiscoveryTask discoverResource(RcsAddress address, String
   *         uri, OnResourceDiscoveredListener listener)
   * @test_data 1. Null Resource Address 2. Resource URI 3. Callback
   * @pre_condition None
   * @procedure Perform discoverResource() API
   * @post_condition None
   * @expected RcsException should throw.
   **/
  public void testDiscoverResourceWithURIAddress_NV_N() {
    try {
      RcsDiscoveryManager.DiscoveryTask discoveryTask = RcsDiscoveryManager
          .getInstance().discoverResource(null, OC_RSRVD_WELL_KNOWN_URI,
              mOnResourceDiscoveredListener);

      m_REHelper.waitInSecond(CALLBACK_WAIT_MAX);

      fail("discoverResource API called successfully when called with null address.");

      discoveryTask.cancel();
    } catch (NullPointerException e) {
      Log.e(
          LOG_TAG,
          "Exception occurs at TestDiscoverResourceWithURIAddress_NV_N as expected. Exception: "
              + e.getLocalizedMessage());
    } catch (RcsException e) {
      fail("Should throw NullPointerException");
    }
  }

  /**
   * @since 2016-02-24
   * @see None
   * @objective Test 'discoverResource' function with positive basic way
   * @target public DiscoveryTask discoverResourceByType(RcsAddress address,
   *         String resourceType, OnResourceDiscoveredListener listener)
   * @test_data 1. Resource Address 2. Resource Type 3. Callback
   * @pre_condition None
   * @procedure Perform discoverResourceByType() API
   * @post_condition None
   * @expected returned discovery manager should not be null
   **/
  public void testDiscoverResourceByTypeForUnicast_FSV_P() {
    try {
      RcsDiscoveryManager.DiscoveryTask discoveryTask = RcsDiscoveryManager
          .getInstance().discoverResourceByType(
              REHelper.getDefaultRcsAddressForUnicast(),
              RESOURCE_TYPE_TEMPERATURE, mOnResourceDiscoveredListener);

      m_REHelper.waitInSecond(CALLBACK_WAIT_MAX);

      discoveryTask.cancel();
    } catch (RcsException e) {
      fail("Throws exception when called discoverResourceByType API. "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-10-22
   * @see None
   * @objective Test 'discoverResource' function with positive basic way
   * @target public DiscoveryTask discoverResourceByType(RcsAddress address,
   *         String resourceType, OnResourceDiscoveredListener listener)
   * @test_data 1. Resource Address 2. Resource Type 3. Callback
   * @pre_condition None
   * @procedure Perform discoverResourceByType() API
   * @post_condition None
   * @expected returned discovery manager should not be null
   **/
  public void testDiscoverResourceByTypeForMulticast_FSV_P() {
    try {
      RcsDiscoveryManager.DiscoveryTask discoveryTask = RcsDiscoveryManager
          .getInstance().discoverResourceByType(RcsAddress.multicast(),
              RESOURCE_TYPE_TEMPERATURE, mOnResourceDiscoveredListener);

      m_REHelper.waitInSecond(CALLBACK_WAIT_MAX);

      discoveryTask.cancel();
    } catch (RcsException e) {
      fail("Throws exception when called discoverResourceByType API. "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-10-22
   * @see None
   * @objective Test 'discoverResource' function with Multicast Resource Address
   *            and Null Resource Type
   * @target public DiscoveryTask discoverResourceByType(RcsAddress address,
   *         String resourceType, OnResourceDiscoveredListener listener)
   * @test_data 1. Resource Address 2. Null Resource Type 3. Callback
   * @pre_condition None
   * @procedure Perform discoverResourceByType() API
   * @post_condition None
   * @expected Should throw NullPointerException
   **/
  public void testDiscoverResourceByTypeForMulticast_NV_N() {
    try {
      RcsDiscoveryManager.DiscoveryTask discoveryTask = RcsDiscoveryManager
          .getInstance().discoverResourceByType(RcsAddress.multicast(), null,
              mOnResourceDiscoveredListener);

      m_REHelper.waitInSecond(CALLBACK_WAIT_MAX);

      discoveryTask.cancel();
    } catch (NullPointerException enull) {
      Log.d(LOG_TAG, "Throws exception when called discoverResource API. "
          + enull.getLocalizedMessage());
      fail("Throws exception when called discoverResource API with null resource type. "
          + enull.getLocalizedMessage());
    } catch (RcsException e) {
      fail("Should throw NullPointerException exception when when called discoverResource API with null uri. "
          + e.getLocalizedMessage());
      fail("Throws exception when called discoverResource API with null resource type. "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2016-02-24
   * @see None
   * @objective Test 'discoverResource' function with Multicast Resource Address
   *            and Empty Resource Type
   * @target public DiscoveryTask discoverResourceByType(RcsAddress address,
   *         String resourceType, OnResourceDiscoveredListener listener)
   * @test_data 1. Resource Address 2. Resource Type = EMPTY_STRING_VALUE 3.
   *            Callback
   * @pre_condition None
   * @procedure Perform discoverResourceByType() API
   * @post_condition None
   * @expected Should throw RcsException
   **/
  public void testDiscoverResourceByTypeForMulticast_ESV_N() {
    try {
      RcsDiscoveryManager.DiscoveryTask discoveryTask = RcsDiscoveryManager
          .getInstance().discoverResourceByType(RcsAddress.multicast(),
              EMPTY_STRING_VALUE, mOnResourceDiscoveredListener);

      m_REHelper.waitInSecond(CALLBACK_WAIT_MAX);

      discoveryTask.cancel();
    } catch (RcsException e) {
      fail("Throws exception when called discoverResourceByType API with empty type. "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-10-22
   * @see None
   * @objective Test 'discoverResource' function with Multicast Resource
   *            Address, Resource Uri, Resource Type and Callback
   * @target public DiscoveryTask discoverResourceByType(RcsAddress address,
   *         String uri, String resourceType, OnResourceDiscoveredListener
   *         listener)
   * @test_data 1. Resource Address 2. Resource Uri 3. Resource Type 4. Callback
   * @pre_condition None
   * @procedure Perform discoverResourceByType() API
   * @post_condition None
   * @expected returned discovery manager should not be null
   **/
  public void testDiscoverResourceByTypeWithURIForMulticast_FSV_P() {
    try {
      RcsDiscoveryManager.DiscoveryTask discoveryTask = RcsDiscoveryManager
          .getInstance().discoverResourceByType(RcsAddress.multicast(),
              OC_RSRVD_WELL_KNOWN_URI, RESOURCE_TYPE_TEMPERATURE,
              mOnResourceDiscoveredListener);

      m_REHelper.waitInSecond(CALLBACK_WAIT_MAX);

      discoveryTask.cancel();
    } catch (RcsException e) {
      fail("Throws exception when called discoverResourceByType API with uri. "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-10-22
   * @see None
   * @objective Test 'discoverResource' function with Unicast Resource Address,
   *            Resource Uri, Resource Type and Callback
   * @target public DiscoveryTask discoverResourceByType(RcsAddress address,
   *         String uri, String resourceType, OnResourceDiscoveredListener
   *         listener)
   * @test_data 1. Resource Address 2. Resource Uri 3. Resource Type 4. Callback
   * @pre_condition None
   * @procedure Perform discoverResourceByType() API
   * @post_condition None
   * @expected returned discovery manager should not be null
   **/
  public void testDiscoverResourceByTypeWithURIForUnicast_FSV_P() {
    try {
      RcsDiscoveryManager.DiscoveryTask discoveryTask = RcsDiscoveryManager
          .getInstance().discoverResourceByType(
              REHelper.getDefaultRcsAddressForUnicast(),
              OC_RSRVD_WELL_KNOWN_URI, RESOURCE_TYPE_TEMPERATURE,
              mOnResourceDiscoveredListener);

      m_REHelper.waitInSecond(CALLBACK_WAIT_MAX);

      discoveryTask.cancel();
    } catch (RcsException e) {
      fail("Throws exception when called discoverResourceByType API with uri for Unicast."
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2016-02-24
   * @see None
   * @objective Test 'discoverResource' function with Multicast Resource
   *            Address, empty Resource Uri, empty Resource Type and Callback
   * @target public DiscoveryTask discoverResourceByType(RcsAddress address,
   *         String uri, String resourceType, OnResourceDiscoveredListener
   *         listener)
   * @test_data 1. Resource Address 2. Resource Type = EMPTY_STRING_VALUE 3.
   *            Resource URI = EMPTY_STRING_VALUE 4. Callback
   * @pre_condition None
   * @procedure Perform discoverResourceByType() API
   * @post_condition None
   * @expected should throw RcsException
   **/
  public void testDiscoverResourceByTypeWithURI_ESV_N() {
    try {
      RcsDiscoveryManager.DiscoveryTask discoveryTask = RcsDiscoveryManager
          .getInstance().discoverResourceByType(RcsAddress.multicast(),
              EMPTY_STRING_VALUE, EMPTY_STRING_VALUE,
              mOnResourceDiscoveredListener);

      m_REHelper.waitInSecond(CALLBACK_WAIT_MAX);

      discoveryTask.cancel();
    } catch (RcsException e) {
      fail("Throws exception when called discoverResourceByType API with empty uri & type. "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2016-02-24
   * @see None
   * @objective Test 'discoverResource' function with null parameters
   * @target public DiscoveryTask discoverResourceByType(RcsAddress address,
   *         String uri, String resourceType, OnResourceDiscoveredListener
   *         listener)
   * @test_data 1. null Resource Address 2. Resource Type = NULL_STRING 3.
   *            Resource URI = NULL_STRING 4. null Callback
   * @pre_condition None
   * @procedure Perform discoverResourceByType() API
   * @post_condition None
   * @expected should throw RcsException
   **/
  public void testDiscoverResourceByTypeWithURI_NV_N() {
    try {
      RcsDiscoveryManager.DiscoveryTask discoveryTask = RcsDiscoveryManager
          .getInstance().discoverResourceByType(null, NULL_STRING, NULL_STRING,
              null);

      m_REHelper.waitInSecond(2);

      fail("Successfully called discoverResourceByType API for all null parameters.");

      discoveryTask.cancel();
    } catch (NullPointerException e) {
      Log.e(LOG_TAG,
          "Exception occurs at TestDiscoverResourceByTypeWithURI_NV_N. Exception: "
              + e.getLocalizedMessage());
    } catch (RcsException e) {
      fail("Should throw NullPointerException");
    }
  }

  /**
   * @since 2016-02-24
   * @see None
   * @objective Test 'discoverResource' function with Null Resource Address
   * @target DiscoveryTask discoverResourceByType(RcsAddress address, String
   *         uri, String resourceType, OnResourceDiscoveredListener listener)
   * @test_data 1. Null Resource Address 2. Resource Type = EMPTY_STRING_VALUE
   *            3. Resource URI = EMPTY_STRING_VALUE 4. Callback
   * @pre_condition None
   * @procedure Perform discoverResourceByType() API
   * @post_condition None
   * @expected should throw RcsException
   **/
  public void testDiscoverResourceByTypeWithURIForRcsAddess_ECRC_N() {
    try {
      RcsDiscoveryManager.DiscoveryTask discoveryTask = RcsDiscoveryManager
          .getInstance().discoverResourceByType(null, OC_RSRVD_WELL_KNOWN_URI,
              RESOURCE_TYPE_TEMPERATURE, mOnResourceDiscoveredListener);

      m_REHelper.waitInSecond(CALLBACK_WAIT_MIN);

      fail("Successfully called discoverResourceByType API for all null parameters.");

      discoveryTask.cancel();
    } catch (NullPointerException e) {
    } catch (RcsException e) {
      fail("Should throw NullPointerException");
    }
  }
}
