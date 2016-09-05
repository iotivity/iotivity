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

package org.iotivity.service.re.test.btc;

import org.iotivity.base.ModeType;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;
import org.iotivity.service.RcsException;
import org.iotivity.service.RcsResourceAttributes;
import org.iotivity.service.RcsValue;
import org.iotivity.service.re.test.helper.REHelper;
import org.iotivity.service.re.test.helper.ResourceProperties;

import android.test.InstrumentationTestCase;
import android.util.Log;

public class REResourceCacheTest extends InstrumentationTestCase {
  private static final String LOG_TAG           = "RETest";
  private REHelper            m_REHelper;
  private StringBuilder       m_ErrorMsg        = new StringBuilder();
  private static final int    CALLBACK_WAIT_SEC = 10;

  protected void setUp() throws Exception {
    super.setUp();

    m_REHelper = new REHelper();
    m_ErrorMsg.setLength(0);

    PlatformConfig platformConfigObj = new PlatformConfig(getInstrumentation()
        .getTargetContext(), ServiceType.IN_PROC, ModeType.CLIENT_SERVER,
        "0.0.0.0", 0, QualityOfService.LOW);

    OcPlatform.Configure(platformConfigObj);
    Log.i(LOG_TAG, "Configuration done Successfully");

    if (!m_REHelper.disocverResources(m_ErrorMsg)) {
      fail("Precondition Failed, No Resource Found!! " + m_ErrorMsg.toString());
      return;
    } else {
      Log.d(LOG_TAG, m_ErrorMsg.toString());
    }

    m_ErrorMsg.setLength(0);
  }

  protected void tearDown() throws Exception {
    super.tearDown();

    m_REHelper.distroyResources();

    Log.d(LOG_TAG, "tearDown called for REResourceCacheTest");
  }

  /**
   * @since 2015-09-20
   * @see none
   * @objective Test 'startCaching' function with positive basic way
   * @target void startCaching()
   * @test_data Callback function for receiving changed state
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure 1. setCacheUpdateListener as null 2. Perform startCaching() API
   * @post_condition None
   * @expected No crash occurs
   */
  public void testStartCaching_SRC_P() {
    if (!m_REHelper.startCaching(false, m_ErrorMsg)) {
      fail(m_ErrorMsg.toString());
    }
  }

  /**
   * @since 2015-09-20
   * @see none
   * @objective Test 'startCaching' function with positive basic way
   * @target void startCaching()
   * @test_data Callback function for receiving changed state
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure 1. setCacheUpdateListener with a valid listener 2. Perform
   *            startCaching() API
   * @post_condition None
   * @expected No crash occurs
   */
  public void testStartCachingWithCallback_SRC_P() {
    if (!m_REHelper.startCaching(true, m_ErrorMsg)) {
      fail(m_ErrorMsg.toString());
    }
  }

  /**
   * @since 2016-02-23
   * @see none
   * @objective Test 'startCaching' function with positive basic way
   * @target void startCaching(OnCacheUpdatedListener listener)
   * @test_data null callback
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure Perform startCaching() API with null callback
   * @post_condition None
   * @expected No exception occurs
   */
  public void testStartCaching_NV_ETC_P() {
    try {
      m_REHelper.mResourceObj.startCaching(null);
    } catch (RcsException e) {
      fail("Exception occured in TestStartCaching_NV_ETC_P.");
    }
  }

  /**
   * @since 2015-09-20
   * @see void startCaching()
   * @objective Test 'stopCaching' function with positive basic way
   * @target void stopCaching()
   * @test_data Callback function for receiving changed state
   * @pre_condition 1. Remote Resource Object should be instantialized 2.
   *                Perform startCaching() API
   * @procedure Perform stopCaching() API
   * @post_condition None
   * @expected No crash occurs
   */
  public void testStopCaching_SRC_P() {
    if (!m_REHelper.startCaching(true, m_ErrorMsg)) {
      Log.d(LOG_TAG, m_ErrorMsg.toString());
    }

    if (!m_REHelper.stopCaching(m_ErrorMsg)) {
      fail(m_ErrorMsg.toString());
    }
  }

  /**
   * @since 2015-09-20
   * @see void startCaching()
   * @objective Test 'stopCaching' function without performing startCaching()
   *            API
   * @target void stopCaching()
   * @test_data Callback function for receiving changed state
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure Perform stopCaching() API
   * @post_condition None
   * @expected 1. No crash occurs 2. Should not stop cachihing without starting
   */
  public void testStopCaching_ECRC_N() {
    if (!m_REHelper.stopCaching(m_ErrorMsg)) {
      fail(m_ErrorMsg.toString());
    }
  }

  /**
   * @since 2016-02-23
   * @see None
   * @objective Test 'isCaching' function with positive basic way
   * @target boolean isCaching()
   * @test_data none
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure Perform isCaching() API
   * @post_condition None
   * @expected No crash occurs
   */
  public void testIsCaching_SRC_P() {
    boolean isCaching = true;

    try {
      isCaching = m_REHelper.mResourceObj.isCaching();
    } catch (RcsException e) {
      fail("Should not throw exception when calling isCaching API.");
    }

    if (isCaching) {
      fail("Cache is true without calling startCaching API.");
    }
  }

  /**
   * @since 2016-02-23
   * @see None
   * @objective Test 'isCachedAvailable' function with positive basic way
   * @target boolean isCachedAvailable()
   * @test_data none
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure Perform isCachedAvailable() API
   * @post_condition None
   * @expected No crash occurs
   */
  public void testIsCachedAvailable_SRC_P() {
    boolean isCachedAvailable = true;

    try {
      isCachedAvailable = m_REHelper.mResourceObj.isCachedAvailable();
    } catch (RcsException e) {
      fail("Should not throw exception when calling isCachedAvailable API.");
    }

    if (isCachedAvailable) {
      fail("CachedAvailable is true without calling startCaching API.");
    }
  }

  /**
   * @since 2016-02-23
   * @see None
   * @objective Test 'isObservable' function with positive basic way
   * @target boolean isObservable()
   * @test_data none
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure Perform isObservable() API
   * @post_condition None
   * @expected No crash occurs
   */
  public void testIsObservable_SRC_P() {
    try {
      m_REHelper.mResourceObj.isObservable();
    } catch (RcsException e) {
      fail("Should not throw exception when calling isObservable API.");
    }
  }

  /**
   * @since 2015-10-20
   * @see startCaching()
   * @objective Test 'getCachedAttributes' function with positive basic way
   * @target void getCachedAttributes()
   * @test_data None
   * @pre_condition 1. Remote Resource Object should be instantialized 2.
   *                Perform startCaching() API
   * @procedure Perform getCachedAttributes() API
   * @post_condition None
   * @expected Returned Resource Attributes is not null
   **/
  public void testGetCachedAttributes_SRC_P() {
    if (!m_REHelper.startCaching(false, m_ErrorMsg)) {
      fail("Precondition failed. " + m_ErrorMsg);
    }

    m_REHelper.waitInSecond(CALLBACK_WAIT_SEC);

    m_ErrorMsg.setLength(0);

    if (!m_REHelper.getCachedAttributes(m_ErrorMsg)) {
      fail(m_ErrorMsg.toString());
    } else {
      Log.d(LOG_TAG, m_ErrorMsg.toString());
    }
  }

  /**
   * @since 2015-10-20
   * @see None
   * @objective Test 'getCachedAttribute' function with positive basic way
   * @target RcsValue getCachedAttribute(String key)
   * @test_data key = "Temperature"
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure Perform getCachedAttributes(string&) API
   * @post_condition None
   * @expected returned Attribute is not empty
   **/
  public void testGetCachedAttribute_P() {
    if (!m_REHelper.startCaching(true, m_ErrorMsg)) {
      fail("Precondition failed. " + m_ErrorMsg);
    }

    m_REHelper.waitInSecond(CALLBACK_WAIT_SEC);

    m_ErrorMsg.setLength(0);

    Boolean expected = true;

    if (m_REHelper.getCachedAttribute(ResourceProperties.ATTR_KEY_POWER,
        m_ErrorMsg, expected) == !expected) {
      fail(m_ErrorMsg.toString());
    }
  }

  /**
   * @since 2015-10-20
   * @see None
   * @objective Test 'getCachedAttribute' function with positive basic way
   * @target RcsValue getCachedAttribute(String key)
   * @test_data invalid key = "invalid_key"
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure Perform getCachedAttributes(string&) API
   * @post_condition None
   * @expected returned Attribute is not empty
   **/
  public void testGetCachedAttribute_N() {
    if (!m_REHelper.startCaching(false, m_ErrorMsg)) {
      assertTrue("Precondition fail. " + m_ErrorMsg, false);
    }

    m_REHelper.waitInSecond(CALLBACK_WAIT_SEC);

    m_ErrorMsg.setLength(0);

    String key = "invalid_key";
    Boolean expected = false;

    if (m_REHelper.getCachedAttribute(key, m_ErrorMsg, expected) == !expected) {
      fail(m_ErrorMsg.toString());
    }
  }

  /**
   * @since 2016-02-25
   * @see None
   * @objective Test 'getCachedAttribute' function with empty key
   * @target RcsValue getCachedAttribute(String key)
   * @test_data Empty key = ""
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure Perform getCachedAttributes(string&) API
   * @post_condition None
   * @expected Returned Attribute is empty
   **/
  public void testGetCachedAttribute_ESV_N() {
    if (!m_REHelper.startCaching(false, m_ErrorMsg)) {
      assertTrue("Precondition fail. " + m_ErrorMsg, false);
    }

    m_REHelper.waitInSecond(CALLBACK_WAIT_SEC);

    m_ErrorMsg.setLength(0);

    String key = "";
    Boolean expected = false;

    if (m_REHelper.getCachedAttribute(key, m_ErrorMsg, expected) == !expected) {
      fail(m_ErrorMsg.toString());
    }
  }

  /**
   * @since 2016-02-25
   * @see None
   * @objective Test 'getCachedAttribute' function with null key
   * @target RcsValue getCachedAttribute(String key)
   * @test_data key = null
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure Perform getCachedAttribute() API
   * @post_condition None
   * @expected Should throw exception.
   **/
  public void testGetCachedAttribute_NV_N() {
    if (!m_REHelper.startCaching(false, m_ErrorMsg)) {
      assertTrue("Precondition fail. " + m_ErrorMsg, false);
    }

    m_REHelper.waitInSecond(CALLBACK_WAIT_SEC);

    try {
      m_REHelper.mResourceObj.getCachedAttribute(null);
      assertTrue("Called getCachedAttribute API successfully with null key.",
          false);
    } catch (NullPointerException e) {
    } catch (RcsException e) {
      fail("Should throw NullPointerException");
    }
  }

  /**
   * @since 2016-02-25
   * @see None
   * @objective Test 'getCachedAttribute' function with null key to error throw
   *            check
   * @target RcsValue getCachedAttribute(String key)
   * @test_data key = null
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure Perform getCachedAttributes(string&) API
   * @post_condition None
   * @expected Returned Attribute is empty
   **/
  public void testGetCachedAttribute_ETC_N() {
    if (!m_REHelper.startCaching(false, m_ErrorMsg)) {
      assertTrue("Precondition fail. " + m_ErrorMsg, false);
    }

    m_REHelper.waitInSecond(CALLBACK_WAIT_SEC);

    try {
      m_REHelper.mResourceObj.getCachedAttribute(null);
    } catch (NullPointerException e) {
      assertEquals("key is null.", e.getLocalizedMessage());
    } catch (RcsException e) {
      fail("Should throw NullPointerException");
    }
  }

  /**
   * @since 2015-10-21
   * @see None
   * @objective Test 'getRemoteAttributes' function with positive basic way
   * @target void getRemoteAttributes(AttributeRecievedCallback)
   * @test_data AttributeRecievedCallback
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure Perform getRemoteAttributes() API
   * @post_condition None
   * @expected The API should not crash
   **/
  public void testGetRemoteAttributes_P() {
    if (!m_REHelper.getRemoteAttributes(true, m_ErrorMsg)) {
      fail(m_ErrorMsg.toString());
    }
  }

  /**
   * @since 2015-10-21
   * @see OnRemoteAttributesReceivedListener
   * @objective Test 'getRemoteAttributes' function with null listener
   * @target void getRemoteAttributes(OnRemoteAttributesReceivedListener
   *         listener)
   * @test_data OnRemoteAttributesReceivedListener is null
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure Perform getRemoteAttributes() API
   * @post_condition None
   * @expected 1. The API should not crash. 2. Should throw an exception.
   **/
  public void testGetRemoteAttributes_NV_N() {
    if (!m_REHelper.getRemoteAttributes(false, m_ErrorMsg)) {
      fail(m_ErrorMsg.toString());
    }
  }

  /**
   * @since 2016-02-25
   * @see OnRemoteAttributesReceivedListener
   * @objective Test 'getRemoteAttributes' function with null listener to
   *            exception throw check
   * @target void getRemoteAttributes(OnRemoteAttributesReceivedListener
   *         listener)
   * @test_data OnRemoteAttributesReceivedListener is null
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure Perform getRemoteAttributes() API
   * @post_condition None
   * @expected 1. The API should not crash. 2. Should throw an exception.
   **/
  public void testGetRemoteAttributes_ETC_N() {
    try {
      m_REHelper.mResourceObj.getRemoteAttributes(null);
    } catch (NullPointerException e) {
      assertEquals("listener is null.", e.getLocalizedMessage());
    } catch (RcsException e) {
      fail("Should throw NullPointerException");
    }
  }

  /**
   * @since 2015-10-21
   * @see OnRemoteAttributesReceivedListener
   * @objective Test 'setRemoteAttributes' function with positive basic way
   * @target void setRemoteAttributes(RcsResourceAttributes attributes,
   *         OnRemoteAttributesReceivedListener listener)
   * @test_data Resource Attributes
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure 1. Create resource Attributes 2. Perform setRemoteAttributes()
   *            API
   * @post_condition None
   * @expected The API should not generate any exception
   **/
  public void testSetRemoteAttributes_SRC_P() {
    int value = 20;
    Boolean expected = true;
    RcsResourceAttributes attribute;

    RcsValue rcsValue = new RcsValue(Integer.valueOf(value).toString());

    attribute = new RcsResourceAttributes();
    attribute.put(ResourceProperties.ATTR_KEY_POWER, rcsValue);

    if (m_REHelper.setRemoteAttributes(attribute, m_ErrorMsg, expected, true) != expected) {
      fail(m_ErrorMsg.toString());
    }
  }

  /**
   * @since 2015-10-21
   * @see OnRemoteAttributesReceivedListener
   * @objective Test 'setRemoteAttributes' function with Null Resource
   *            Attributes
   * @target void setRemoteAttributes(RcsResourceAttributes attributes,
   *         OnRemoteAttributesReceivedListener listener)
   * @test_data Null Resource Attributes
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure Perform setRemoteAttributes() API with Null Resource Attributes
   * @post_condition None
   * @expected The API should not crash
   **/
  public void testSetRemoteAttributesResourceAttributes_NV_N() {
    Boolean expected = false;

    if (m_REHelper.setRemoteAttributes(null, m_ErrorMsg, expected, true) != expected) {
      fail(m_ErrorMsg.toString());
    }
  }

  /**
   * @since 2015-10-21
   * @see OnRemoteAttributesReceivedListener
   * @objective Test 'setRemoteAttributes' function with Null Callback
   * @target void setRemoteAttributes(RcsResourceAttributes attributes,
   *         OnRemoteAttributesReceivedListener listener)
   * @test_data Null Callback
   * @pre_condition Remote Resource Object should be instantialized
   * @procedure 1. Create resource Attributes 2. Perform setRemoteAttributes()
   *            API
   * @post_condition None
   * @expected The API should not crash
   **/
  public void testSetRemoteAttributesWithoutCallback_NV_N() {
    int value = 20;
    Boolean expected = false;
    RcsResourceAttributes attribute;

    RcsValue rcsValue = new RcsValue(Integer.valueOf(value).toString());

    attribute = new RcsResourceAttributes();
    attribute.put(ResourceProperties.ATTR_KEY_POWER, rcsValue);

    if (m_REHelper.setRemoteAttributes(attribute, m_ErrorMsg, expected, false) != expected) {
      fail(m_ErrorMsg.toString());
    }
  }

}
