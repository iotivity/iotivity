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

package org.iotivity.service.re.test.stc;

import org.iotivity.base.ModeType;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;
import org.iotivity.service.RcsException;
import org.iotivity.service.RcsResourceAttributes;
import org.iotivity.service.RcsValue;
import org.iotivity.service.client.RcsRemoteResourceObject;
import org.iotivity.service.client.RcsRemoteResourceObject.CacheState;
import org.iotivity.service.re.test.helper.REAPIHelper;
import static org.iotivity.service.re.test.helper.ResourceUtil.*;
import android.test.InstrumentationTestCase;
import android.util.Log;

public class REResourceCacheTest extends InstrumentationTestCase {
  private static final String            LOG_TAG    = "RETest";
  private REAPIHelper                    m_REHelper;
  private static RcsRemoteResourceObject m_Resource;
  private StringBuilder                  m_ErrorMsg = new StringBuilder();

  protected void setUp() throws Exception {
    super.setUp();

    m_REHelper = new REAPIHelper();
    m_ErrorMsg.setLength(0);

    PlatformConfig platformConfigObj = new PlatformConfig(getInstrumentation()
        .getTargetContext(), ServiceType.IN_PROC, ModeType.CLIENT_SERVER,
        "0.0.0.0", 0, QualityOfService.LOW);

    OcPlatform.Configure(platformConfigObj);
    Log.i(LOG_TAG, "Configuration done Successfully");

    if (!m_REHelper.disocverResources(m_ErrorMsg)) {
      assertTrue(
          "Precondition Failed, No Resource Found!! " + m_ErrorMsg.toString(),
          false);
    } else {
      m_Resource = m_REHelper.getFoundResourceList().get(0);
      Log.i(LOG_TAG, m_ErrorMsg.toString());
    }

    m_ErrorMsg.setLength(0);
  }

  protected void tearDown() throws Exception {
    super.tearDown();

    m_REHelper.distroyResources();

    if (m_Resource != null) {
      m_Resource.destroy();
      m_Resource = null;
    }

    Log.i(LOG_TAG, "tearDown called for REResourceCacheTest");
  }

  /**
   * @since 2015-11-04
   * @see OnCacheUpdatedListener
   * @objective Test 'startCaching' function with positive way for Callback
   *            validation
   * @target void startCaching(OnCacheUpdatedListener listener)
   * @test_data OnCacheUpdatedListener function for receiving cached attribute
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure Perform startCaching() API
   * @post_condition None
   * @expected 1. Callback should be invoked. 2. No crash occurs
   **/
  public void testStartCachingWithCallback_CV_P() {
    try {
      m_Resource.startCaching(m_REHelper.mOnCacheUpdatedListener);
      m_REHelper.waitInSecond(CALLBACK_WAIT_MIN);

      if (REAPIHelper.g_IsCasheUpdated == false) {
        fail("Callback did not come");
      }
    } catch (RcsException e) {
      fail("Exception occurred inside testStartCaching_CV_P: "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2016-03-02
   * @see None
   * @objective Test 'startCaching' function with State Change Validation
   * @target void startCaching(OnCacheUpdatedListener listener)
   * @target boolean isCaching()
   * @target void stopCaching()
   * @test_data Callback function for receiving cached attribute
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure 1. Perform startCaching() API 2. Wait for 2 seconds. 3. Perform
   *            stopCaching() API.
   * @post_condition None
   * @expected 1. isCaching() should return true. 2. No exception occur.
   **/

  public void testStartCachingWithCallback_SCV_P() {
    try {
      m_Resource.startCaching(m_REHelper.mOnCacheUpdatedListener);
      boolean isCaching = m_Resource.isCaching();
      assertEquals(
          "isCaching() API returns false, where it should return true.", true,
          isCaching);

      m_Resource.stopCaching();
      isCaching = m_Resource.isCaching();
      assertEquals(
          "isCaching() API returns true, where it should return false.", false,
          isCaching);
    } catch (RcsException e) {
      fail("Exception occurred inside testStartCachingWithoutCallback_SQV_P: "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2016-03-02
   * @see None
   * @objective Test 'startCaching' function with Sequential Validation
   * @target void startCaching(OnCacheUpdatedListener listener)
   * @target void stopCaching()
   * @test_data Callback function for receiving cached attribute
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure 1. Perform startCaching() API 2. Perform stopCaching() API.
   * @post_condition None
   * @expected No exception occur.
   **/

  public void testStartCachingWithCallback_SQV_P() {
    try {
      m_Resource.startCaching(m_REHelper.mOnCacheUpdatedListener);

      m_REHelper.waitInSecond(CALLBACK_WAIT_MIN);

      m_Resource.stopCaching();
    } catch (RcsException e) {
      fail("Exception occurred inside testStartCachingWithoutCallback_SCV_P: "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2016-03-02
   * @see None
   * @objective Test 'startCaching' function with State Change Validation
   * @target void startCaching()
   * @target boolean isCaching()
   * @target void stopCaching()
   * @test_data None
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure 1. Perform startCaching() API 2. Wait for 2 seconds. 3. Perform
   *            stopCaching() API.
   * @post_condition None
   * @expected 1. isCaching() should return true. 2. No exception occur.
   **/

  public void testStartCachingWithoutCallback_SCV_P() {
    try {
      m_Resource.startCaching();
      boolean isCaching = m_Resource.isCaching();
      assertEquals(
          "isCaching() API returns false, where it should return true.", true,
          isCaching);

      m_Resource.stopCaching();
      isCaching = m_Resource.isCaching();
      assertEquals(
          "isCaching() API returns true, where it should return false.", false,
          isCaching);
    } catch (RcsException e) {
      fail("Exception occurred inside testStartCachingWithoutCallback_SQV_P: "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-11-04
   * @see None
   * @objective Test 'startCaching' function with Sequential Validation
   * @target void startCaching()
   * @target void stopCaching()
   * @test_data None
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure 1. Perform startCaching() API 2. Perform stopCaching() API.
   * @post_condition None
   * @expected No exception occur.
   **/

  public void testStartCachingWithoutCallback_SQV_P() {
    try {
      m_Resource.startCaching();

      m_REHelper.waitInSecond(CALLBACK_WAIT_MIN);

      m_Resource.stopCaching();
    } catch (RcsException e) {
      fail("Exception occurred inside testStartCachingWithoutCallback_SCV_P: "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-11-04
   * @see None
   * @objective Test 'startCaching' function without callback invoking
   * @target void startCaching()
   * @test_data None
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure 1. Perform startCaching() API 2. check for IsCasheUpdated
   * @post_condition None
   * @expected No crash occurs
   **/
  public void testStartCachingWithoutCallback_EG_N() {
    try {
      m_Resource.startCaching();

      m_REHelper.waitInSecond(CALLBACK_WAIT_MIN);

      if (REAPIHelper.g_IsCasheUpdated) {
        fail("Callback is coming, while the callback was null");
      }
    } catch (RcsException e) {
      fail("Exception occurred inside StartCachingWithoutCallback_SRC_P: "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-11-04
   * @see None
   * @objective Test 'startCaching' function for twice
   * @target void startCaching(OnCacheUpdatedListener listener)
   * @test_data None
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure Perform startCaching() API twice
   * @post_condition None
   * @expected No crash occurs
   **/
  public void testStartCachingTwice_VLCC_N() {
    try {
      m_Resource.startCaching();
      m_REHelper.waitInSecond(CALLBACK_WAIT_MIN);
      m_Resource.startCaching();

      fail("Cache is started successfully second time.");
    } catch (RcsException e) {
      if (e.getLocalizedMessage().compareTo("Caching already started.") != 0) {
        fail("Didn't get proper exception message. \"Cheching Caching already started.\" but got "
            + e.getLocalizedMessage());
      }
    }
  }

  /**
   * @since 2015-11-04
   * @see startCaching(OnCacheUpdatedListener)
   * @objective Test 'stopCaching' function with positive basic way
   * @target void stopCaching()
   * @test_data None
   * @pre_condition 1. Remote Resource Object should be instantialized 2.
   *                Perform startCaching() API
   * @procedure Perform stopCaching() API
   * @post_condition None
   * @expected The API should not generate any exception
   **/
  public void testStopCachingWithisCaching_SQV_P() {
    try {
      m_Resource.startCaching(m_REHelper.mOnCacheUpdatedListener);

      m_REHelper.waitInSecond(CALLBACK_WAIT_MIN);
      m_Resource.stopCaching();
      m_REHelper.waitInSecond(CALLBACK_WAIT_MIN);

      if (m_Resource.isCaching() == true) {
        fail("isCaching() API returns true, where it should return false.");
      }
    } catch (RcsException e) {
      fail("Exception occurred inside StopCaching_P: "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2016-03-02
   * @see None
   * @objective Test 'stopCaching' function with Initial/Terminate Loop Check
   * @target void stopCaching()
   * @test_data None
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure Perform stopCaching() API multiple times
   * @post_condition None
   * @expected No crash occurs
   **/

  public void testStopCaching_ITLC_P() {
    for (int i = 0; i < 10; i++) {
      try {
        m_Resource.stopCaching();
      } catch (RcsException e) {
        assertTrue(
            "Exception occurred inside testStopCaching_ITLC_P: "
                + e.getLocalizedMessage(), false);
      }
    }
  }

  /**
   * @since 2015-11-04
   * @see startCaching(OnCacheUpdatedListener)
   * @objective Test 'getCachedAttributes' function with positive basic way
   * @target RcsResourceAttributes getCachedAttributes()
   * @test_data None
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure 1. Perform startCaching() API 2. Perform getCachedAttributes()
   *            API
   * @post_condition None
   * @expected Returned Resource Attributes is not null
   **/
  public void testGetCachedAttributes_SQV_P() {
    try {
      m_Resource.startCaching(m_REHelper.mOnCacheUpdatedListener);
      m_REHelper.waitInSecond(CALLBACK_WAIT_MIN);

      RcsResourceAttributes attributes = m_Resource.getCachedAttributes();

      if (attributes.size() == 0) {
        fail("getCachedAttributes() API returned NULL resource attributes");
      }

      m_REHelper.waitInSecond(CALLBACK_WAIT_MIN);
      if (REAPIHelper.g_IsCasheUpdated == false) {
        fail("Callback is not coming.");
      }
    } catch (RcsException e) {
      fail("Exception occurred inside GetCachedAttributes_SRC_P: "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-11-04
   * @see void startCaching()
   * @objective Test 'getCachedAttributes' function with Sequential Validation
   * @target RcsResourceAttributes getCachedAttributes()
   * @test_data None
   * @pre_condition 1. Remote Resource Object should be instantialized 2.
   *                Perform startCaching() API.
   * @procedure Perform getCachedAttributes() API
   * @post_condition None
   * @expected Returned Resource Attributes is not null
   **/

  public void testGetCachedAttributesWithoutCallback_SQV_P() {
    try {
      m_Resource.startCaching();
      m_REHelper.waitInSecond(CALLBACK_WAIT_MAX);

      RcsResourceAttributes attributes = m_Resource.getCachedAttributes();

      if (attributes.size() == 0) {
        fail("getCachedAttributes() API returned NULL resource attributes");
      }

      m_REHelper.waitInSecond(CALLBACK_WAIT_MAX);
      if (REAPIHelper.g_IsCasheUpdated == true) {
        fail("Callback is coming after calling startCaching() without callback.");
      }
    } catch (RcsException e) {
      fail("Exception occurred inside GetCachedAttributes_P: "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-11-04
   * @see startCaching(OnCacheUpdatedListener)
   * @objective Test 'getCachedAttribute' function with Sequential Validation
   * @target RcsValue getCachedAttribute(String key)
   * @test_data key = "Temperature"
   * @pre_condition 1. Remote Resource Object should be instantialized 2.
   *                Perform startCaching() APi with callback
   * @procedure Perform getCachedAttribute(String key) API
   * @post_condition None
   * @expected returned Attribute is not empty
   **/
  public void testGetCachedAttributeWithKey_SQV_P() {
    try {
      m_Resource.startCaching(m_REHelper.mOnCacheUpdatedListener);
      m_REHelper.waitInSecond(CALLBACK_WAIT_MIN);

      RcsValue value = m_Resource.getCachedAttribute(ATTR_KEY_POWER);

      if (value == null) {
        fail("Can't getCachedAttribute for " + ATTR_KEY_POWER);
      }

      if (!(value.asString().equalsIgnoreCase(ATTR_VALUE_POWER_ON) || value
          .asString().equalsIgnoreCase(ATTR_VALUE_POWER_OFF))) {
        fail("Got " + value.asString() + " for " + ATTR_KEY_POWER
            + " it should be " + ATTR_VALUE_POWER_ON + " or "
            + ATTR_VALUE_POWER_OFF);
      }
    } catch (RcsException e) {
      fail("Exception occurred inside GetCachedAttributeWithKey_SRC_P: "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-11-04
   * @see None
   * @objective Test 'getCachedAttribute' function with invalid key
   * @target void getCachedAttribute(string key)
   * @test_data key = "invalid_key"
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure Perform getCachedAttributes(string key) API
   * @post_condition None
   * @expected returned Attribute is empty
   **/
  public void testGetCachedAttributeWithKey_ECRC_N() {
    try {
      m_Resource.getCachedAttribute(ATTR_KEY_POWER);

      fail("Successfully got attribute without starting cache.");
    } catch (RcsException e) {
      if (e.getLocalizedMessage().compareTo("Caching not started.") != 0) {
        fail("Didn't get proper exception message. \"Cheching Caching not started.\" but got "
            + e.getLocalizedMessage());
      }
    }
  }

  /**
   * @since 2015-11-04
   * @see None
   * @objective Test 'getResourceCacheState' function with Squential Validation
   * @target void getResourceCacheState()
   * @test_data None
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure 1. Perform getCacheState() API 2. Perform startCaching() API 3.
   *            Perform getCacheState() API again
   * @post_condition None
   * @expected No crash occurs
   **/

  public void testGetCacheStateWithoutCallback_SQV_P() {
    try {
      CacheState cacheState = m_Resource.getCacheState();
      if (cacheState != CacheState.NONE) {
        fail("Cache state should NONE before CachingStart. But got "
            + cacheState);
      }

      m_Resource.startCaching();

      m_REHelper.waitInSecond(CALLBACK_WAIT_MIN);

      cacheState = m_Resource.getCacheState();
      if (cacheState != CacheState.READY) {
        fail("Cache state should READY after CachingStart. But got "
            + cacheState);
      }
    } catch (RcsException e) {
      fail("Exception occurred inside GetCacheState_P: "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-11-04
   * @see OnRemoteAttributesReceivedListener
   * @objective Test 'getRemoteAttributes' function with Sequential Validation
   * @target void getRemoteAttributes(OnRemoteAttributesReceivedListener
   *         listener)
   * @test_data OnRemoteAttributesReceivedListener
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure Perform getRemoteAttributes() API
   * @post_condition None
   * @expected The API should not generate any exception
   **/
  public void testGetRemoteAttributes_SQV_P() {
    try {
      m_Resource
          .getRemoteAttributes(m_REHelper.mOnRemoteAttributesReceivedListener);
      m_REHelper.waitInSecond(CALLBACK_WAIT_MIN);

      if (REAPIHelper.g_IsAttributeReceived == false) {
        fail("Callback did not come");
      }
    } catch (RcsException e) {
      fail("Exception occurred inside GetRemoteAttributes_SRC_P: "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-11-05
   * @see None
   * @objective Test 'getRemoteAttributes' function with null Listener
   * @target void getRemoteAttributes(OnRemoteAttributesReceivedListener
   *         listener)
   * @test_data NULL listener
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure Perform getRemoteAttributes() API
   * @post_condition None
   * @expected The API should not crash
   **/
  public void testGetRemoteAttributes_ETC_N() {
    try {
      m_Resource.getRemoteAttributes(null);
      m_REHelper.waitInSecond(CALLBACK_WAIT_MIN);

      if (REAPIHelper.g_IsAttributeReceived == true) {
        fail("Callback is coming while listener was null.");
      }
    } catch (NullPointerException enull) {
      if (enull.getLocalizedMessage().compareTo("listener is null.") != 0) {
        fail("Expected exception message is \"listener is null.\" but got "
            + enull.getLocalizedMessage());
      }
    } catch (RcsException e) {
      fail("Exception occurred inside GetRemoteAttributes_N: "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-11-05
   * @see OnRemoteAttributesReceivedListener
   * @objective Test 'setRemoteAttributes' function with sequential validation
   * @target void setRemoteAttributes(RcsResourceAttributes attributes,
   *         OnRemoteAttributesReceivedListener listener)
   * @test_data Resource Attributes, listener
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure 1. Create resource Attributes 2. Perform setRemoteAttributes()
   *            API
   * @post_condition None
   * @expected The API should not generate any exception
   **/
  public void testSetRemoteAttributes_SQV_P() {
    try {
      RcsValue rcsValue = new RcsValue(ATTR_VALUE_POWER_OFF);
      RcsResourceAttributes rcsAttributes = new RcsResourceAttributes();
      rcsAttributes.put(ATTR_KEY_POWER, rcsValue);

      m_Resource.setRemoteAttributes(rcsAttributes,
          m_REHelper.mOnRemoteAttributesReceivedListener);
      m_REHelper.waitInSecond(CALLBACK_WAIT_MIN);

      if (REAPIHelper.g_IsAttributeReceived == false) {
        fail("Callback did not come");
      }

      RcsResourceAttributes attr = m_REHelper.getRcsResourceAttributes();

      if (attr == null) {
        Log.e(LOG_TAG, "RcsResourceAttributes is Null");
      }

      RcsValue receivedValue = attr.get(ATTR_KEY_POWER);

      if (receivedValue != null) {
        if (!receivedValue.asString().equals(ATTR_VALUE_POWER_OFF)) {
          fail("Received Value should be " + ATTR_VALUE_POWER_OFF + " but got "
              + receivedValue.asString());
        }
      } else {
        fail("Can't get the proper set value in callback");
      }
    } catch (RcsException e) {
      fail("Exception occurred inside SetRemoteAttributes_SRC_P: "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-11-05
   * @see OnRemoteAttributesReceivedListener
   * @objective Test 'setRemoteAttributes' function without creating Resource
   *            Attributes
   * @target void setRemoteAttributes(RcsResourceAttributes attributes,
   *         OnRemoteAttributesReceivedListener listener)
   * @test_data Null Resource Attributes, listener
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure 1. Create resource Attributes 2. Perform setRemoteAttributes()
   *            API
   * @post_condition None
   * @expected No crash occurs
   **/
  public void testSetRemoteAttributes_ECRC_N() {
    REAPIHelper.g_IsAttributeReceived = false;

    try {
      RcsResourceAttributes rcsAttributes = new RcsResourceAttributes();

      m_Resource.setRemoteAttributes(rcsAttributes,
          m_REHelper.mOnRemoteAttributesReceivedListener);
      m_REHelper.waitInSecond(CALLBACK_WAIT_MIN);

      if (REAPIHelper.g_IsAttributeReceived == false) {
        fail("Callback is not coming for setRemoteAttributes");
      }
    } catch (RcsException e) {
      fail("Exception occurred inside SetRemoteAttributes_ECRC_N: "
          + e.getLocalizedMessage());
    }
  }
}
