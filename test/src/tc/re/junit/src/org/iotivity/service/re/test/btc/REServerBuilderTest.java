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

import org.iotivity.service.re.test.helper.REHelper;
import static org.iotivity.service.re.test.helper.ResourceProperties.*;
import org.iotivity.service.RcsException;
import org.iotivity.service.RcsResourceAttributes;
import org.iotivity.service.RcsValue;
import org.iotivity.service.server.RcsResourceObject;
import org.iotivity.service.server.RcsResourceObject.AutoNotifyPolicy;
import org.iotivity.service.server.RcsResourceObject.SetRequestHandlerPolicy;
import android.test.InstrumentationTestCase;
import android.util.Log;

public class REServerBuilderTest extends InstrumentationTestCase {
  private RcsResourceObject.Builder m_Builder;
  private RcsResourceObject         m_ResourceObject;
  private static final String       LOG_TAG = "RETest";
  REHelper                          m_REHelper;

  protected void setUp() throws Exception {
    super.setUp();

    m_REHelper = new REHelper();

    m_Builder = new RcsResourceObject.Builder(URI, TYPE, INTERFACE);

    RcsResourceAttributes attributes = new RcsResourceAttributes();
    attributes.put(ATTR_KEY_TEMPERATURE, 5);

    m_Builder.setAttributes(attributes);

    m_ResourceObject = m_Builder.build();

    Log.d(LOG_TAG, "Resource created successfully");
  }

  protected void tearDown() throws Exception {
    super.tearDown();

    if (m_Builder != null) {
      m_Builder = null;

      Log.d(LOG_TAG, "Resource stopped successfully");
    }

    if (m_REHelper != null) {
      m_REHelper = null;
    }

    if (m_ResourceObject != null) {
      m_ResourceObject.destroy();
    }

    Log.d(LOG_TAG, "tearDown() called for REServerBuilderTest");
  }

  /**
   * @since 2015-10-22
   * @see None
   * @objective Test 'Builder' constructor with positive basic way
   * @target Builder(String uri, String resourceType, String resourceInterface)
   * @test_data None
   * @pre_condition None
   * @procedure Make Builder class with valid value
   * @post_condition None
   * @expected No crash occurs
   **/
  public void testBuilder_FSV_P() {
    RcsResourceObject.Builder builder = null;

    try {
      builder = new RcsResourceObject.Builder(URI, TYPE, INTERFACE);

    } catch (NullPointerException e) {
      assertTrue(
          "Throw NullPointerException when called Builder API. "
              + e.getLocalizedMessage(), false);
    }

    if (builder != null) {
      builder = null;
    }
  }

  /**
   * @since 2016-02-26
   * @see None
   * @objective Test 'Builder' constructor with Null uri
   * @target Builder(String uri, String resourceType, String resourceInterface)
   * @test_data uri = null, resourceType = oic.r.temperature.sensor,
   *            resourceInterface = oic.if.
   * @pre_condition None
   * @procedure Make Builder class with valid values but Resource Uri is null
   * @post_condition None
   * @expected Should throw NullPointerException
   **/
  public void testBuilderForUri_NV_N() {
    RcsResourceObject.Builder builder = null;

    try {
      builder = new RcsResourceObject.Builder(null, TYPE, INTERFACE);
      fail("Successfully called Builder() API when called Builder API with Resource Uri.");
    } catch (NullPointerException e) {
    }

    if (builder != null) {
      builder = null;
    }
  }

  /**
   * @since 2016-02-26
   * @see None
   * @objective Test 'Builder' constructor with null resourceType
   * @target Builder(String uri, String resourceType, String resourceInterface)
   * @test_data uri = /a/TempSensor, resourceType = null, resourceInterface =
   *            oic.if.
   * @pre_condition None
   * @procedure Builder class with valid values but Resource Type is null
   * @post_condition None
   * @expected Should throw NullPointerException
   **/
  public void testBuilderForType_NV_N() {
    RcsResourceObject.Builder builder = null;

    try {
      builder = new RcsResourceObject.Builder(URI, null, INTERFACE);
      fail("Successfully called Builder() API when called Builder API with Resource Type.");
    } catch (NullPointerException e) {
    }

    if (builder != null) {
      builder = null;
    }
  }

  /**
   * @since 2016-02-26
   * @see None
   * @objective Test 'Builder' constructor with null interface
   * @target Builder(String uri, String resourceType, String resourceInterface)
   * @test_data uri = /a/TempSensor, resourceType = oic.r.temperature.sensor,
   *            resourceInterface = null
   * @pre_condition None
   * @procedure Builder class with valid values but Resource Interface is null
   * @post_condition None
   * @expected Should throw NullPointerException
   **/
  public void testBuilderForInterface_NV_N() {
    RcsResourceObject.Builder builder = null;

    try {
      builder = new RcsResourceObject.Builder(URI, TYPE, null);
      fail("Successfully called Builder() API when called Builder API with null Resource ");
    } catch (NullPointerException e) {
    }

    if (builder != null) {
      builder = null;
    }
  }

  /**
   * @since 2016-02-26
   * @see None
   * @objective Test 'Builder' constructor with null uri to error throw check
   * @target Builder(String uri, String resourceType, String resourceInterface)
   * @test_data uri = null, resourceType = oic.r.temperature.sensor,
   *            resourceInterface = oic.if.
   * @pre_condition None
   * @procedure Make Builder class with valid values but Resource Uri is null
   * @post_condition None
   * @expected Should throw NullPointerException with message "uri is null."
   **/
  public void testBuilderForUri_ETC_N() {
    RcsResourceObject.Builder builder = null;

    try {
      builder = new RcsResourceObject.Builder(null, TYPE, INTERFACE);
    } catch (NullPointerException e) {
      assertEquals("uri is null.", e.getLocalizedMessage());
    }

    if (builder != null) {
      builder = null;
    }
  }

  /**
   * @since 2016-02-26
   * @see None
   * @objective Test 'Builder' constructor with null resource Type to error
   *            throw check
   * @target Builder(String uri, String resourceType, String resourceInterface)
   * @test_data uri = /a/TempSensor, resourceType = null, resourceInterface =
   *            oic.if.
   * @pre_condition None
   * @procedure Builder class with valid values but Resource Type is null
   * @post_condition None
   * @expected Should throw NullPointerException with message
   *           "resourceType is null."
   **/
  public void testBuilderForType_ETC_N() {
    RcsResourceObject.Builder builder = null;

    try {
      builder = new RcsResourceObject.Builder(URI, null, INTERFACE);
    } catch (NullPointerException e) {
      assertEquals("resourceType is null.", e.getLocalizedMessage());
    }

    if (builder != null) {
      builder = null;
    }
  }

  /**
   * @since 2016-02-26
   * @see None
   * @objective Test 'Builder' constructor with null interface to error throw
   *            check
   * @target Builder(String uri, String resourceType, String resourceInterface)
   * @test_data uri = /a/TempSensor, resourceType = oic.r.temperature.sensor,
   *            resourceInterface = null
   * @pre_condition None
   * @procedure Builder class with valid values but Resource Interface is null
   * @post_condition None
   * @expected No crash occurs
   **/
  public void testBuilderForInterface_ETC_N() {
    RcsResourceObject.Builder builder = null;

    try {
      builder = new RcsResourceObject.Builder(URI, TYPE, null);
      fail("Successfully called Builder() API when called Builder API with null Resource ");
    } catch (NullPointerException e) {
      assertEquals("resourceInterface is null.", e.getLocalizedMessage());
    }

    if (builder != null) {
      builder = null;
    }
  }

  /**
   * @since 2015-10-22
   * @see None
   * @objective Test 'setDiscoverable' function with discoverable as true
   * @target Builder setDiscoverable(boolean isDiscoverable)
   * @test_data discoverable = true
   * @pre_condition Builder class should be initialized
   * @procedure Perform setDiscoverable() API
   * @post_condition None
   * @expected Returned builder should not be null
   **/
  public void testSetDiscoverableasTrue_SRC_P() {
    Boolean discoverable = true;

    try {
      m_Builder.setDiscoverable(discoverable);
    } catch (Exception e) {
      fail("Unable to set discoverable to true, exception is "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-10-22
   * @see None
   * @objective Test 'setDiscoverable' function with discoverable as false
   * @target Builder setDiscoverable(boolean isDiscoverable)
   * @test_data discoverable = false
   * @pre_condition Builder class should be initialized
   * @procedure Perform setDiscoverable() API
   * @post_condition None
   * @expected Returned builder should not be null
   **/
  public void testSetDiscoverableasFalse_SRC_P() {
    Boolean discoverable = false;

    try {
      m_Builder.setDiscoverable(discoverable);
    } catch (Exception e) {
      fail("Unable to set discoverable to true, exception is "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-10-22
   * @see None
   * @objective Test 'setObservable' function with Observable as true
   * @target Builder setObservable(boolean isObservable)
   * @test_data observable = true
   * @pre_condition Builder class should be initialized
   * @procedure Perform setObservable() API
   * @post_condition None
   * @expected Returned builder should not be null
   **/
  public void testSetObservableasTrue_SRC_P() {
    Boolean observable = true;

    try {
      m_Builder.setObservable(observable);
    } catch (Exception e) {
      fail("Unable to set observable to true, exception is "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-10-22
   * @see None
   * @objective Test 'setObservable' function with with Observable as false
   * @target Builder setObservable(boolean isObservable)
   * @test_data observable = false
   * @pre_condition Builder class should be initialized
   * @procedure Perform setObservable() API
   * @post_condition None
   * @expected Returned builder should not be null
   **/
  public void testSetObservableasFalse_P() {
    Boolean observable = false;

    try {
      m_Builder.setObservable(observable);
    } catch (Exception e) {
      fail("Unable to set discoverable to true, exception is "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2016-02-26
   * @see None
   * @objective Test 'isObservable' function with positive basic way
   * @target boolean isObservable()
   * @test_data None
   * @pre_condition RcsResourceObject class should be initialized
   * @procedure Perform isObservable() API
   * @post_condition None
   * @expected Should return true
   **/
  public void testIsObservable_SRC_P() {
    boolean isObservable = false;

    try {
      isObservable = m_ResourceObject.isObservable();
    } catch (RcsException e) {
      assertTrue(
          "Exception occurs at TestIsObservable_P. Exception is "
              + e.getLocalizedMessage(), false);
    }

    assertEquals("isObservable should return true.", true, isObservable);
  }

  /**
   * @since 2016-02-26
   * @see None
   * @objective Test 'isDiscoverable' function with positive basic way
   * @target boolean isDiscoverable()
   * @test_data None
   * @pre_condition RcsResourceObject class should be initialized
   * @procedure Perform isDiscoverable() API
   * @post_condition None
   * @expected Should return true
   **/
  public void testIsDiscoverable_SRC_P() {
    boolean isDiscoverable = false;

    try {
      isDiscoverable = m_ResourceObject.isDiscoverable();
    } catch (RcsException e) {
      assertTrue(
          "Exception occurs at TestIsDiscoverable_P. Exception is "
              + e.getLocalizedMessage(), false);
    }

    assertEquals("isDiscoverable should return true.", true, isDiscoverable);
  }

  /**
   * @since 2015-10-22
   * @see None
   * @objective Test 'setAttributes' function with positive basic way
   * @target Builder setAttributes(RcsResourceAttributes attributes)
   * @test_data Resource Attributes
   * @pre_condition Builder class should be initialized
   * @procedure Perform build() API
   * @post_condition None
   * @expected returned Builder should not be null
   **/
  public void testSetAttributes_SRC_P() {
    RcsResourceAttributes attrs = new RcsResourceAttributes();
    attrs.put(ATTR_KEY_TEMPERATURE, 10);

    try {
      m_Builder.setAttributes(attrs);
    } catch (Exception e) {
      fail("Unable to set Resource Attributes, exception is : "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-10-22
   * @see None
   * @objective Test 'setAttributes' function with negative basic way
   * @target Builder setAttributes(RcsResourceAttributes attributes)
   * @test_data Resource Attributes
   * @pre_condition Builder class should be initialized
   * @procedure Perform build() API
   * @post_condition None
   * @expected Should throw exception
   **/
  public void testSetAttributes_ECRC_N() {
    RcsResourceAttributes attrs = new RcsResourceAttributes();

    try {
      m_Builder.setAttributes(attrs);
    } catch (Exception e) {
      fail("Unable to set Resource Attributes, exception is : "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-10-22
   * @see None
   * @objective Test 'setAttribute' function with positive basic way
   * @target void setAttribute(String key, RcsValue value)
   * @test_data 1. key = "Temperature" 2. value = "30"
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform setAttribute() API
   * @post_condition None
   * @expected The API should not generate exception
   **/
  public void testSetAttributeWithIntValue_SRC_P() {
    RcsValue value = new RcsValue(Integer.valueOf(30));

    try {
      m_ResourceObject.setAttribute(ATTR_KEY_TEMPERATURE, value);
    } catch (RcsException e) {
      fail("Unable to set attribute, exception occurred: "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-10-22
   * @see None
   * @objective Test 'setAttribute' function with null key
   * @target void setAttribute(String key, RcsValue value)
   * @test_data 1. key = null 2. value = 30
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform setAttribute() API
   * @post_condition None
   * @expected The API should generate NullPointerException exception
   **/
  public void testSetAttributeForKey_NV_N() {
    RcsValue value = new RcsValue(Integer.valueOf(30));

    try {
      m_ResourceObject.setAttribute(null, value);

      fail("Successfully setAttribute for null key, but it should return RcsException");
    } catch (Exception e) {
    }
  }

  /**
   * @since 2016-02-26
   * @see None
   * @objective Test 'setAttribute' function with null value
   * @target void setAttribute(String key, RcsValue value)
   * @test_data 1. key = null 2. value = null
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform setAttribute() API
   * @post_condition None
   * @expected The API should generate NullPointerException exception
   **/
  public void testSetAttributeForValue_NV_N() {
    RcsValue value = null;

    try {
      m_ResourceObject.setAttribute(ATTR_KEY_TEMPERATURE, value);

      fail("Successfully setAttribute for null value, but it should return RcsException");
    } catch (Exception e) {
    }
  }

  /**
   * @since 2015-10-22
   * @see None
   * @objective Test 'setAttribute' function with null key for error throw
   *            checking
   * @target void setAttribute(String key, RcsValue value)
   * @test_data 1. key = null 2. value = 30
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform setAttribute() API
   * @post_condition None
   * @expected The API should generate NullPointerException exception with
   *           message "key is null"
   **/
  public void testSetAttributeForKey_ETC_N() {
    RcsValue value = new RcsValue(Integer.valueOf(30));

    try {
      m_ResourceObject.setAttribute(null, value);
    } catch (NullPointerException e) {
      assertEquals("Can't find proper exception.", "key is null",
          e.getLocalizedMessage());
    } catch (RcsException e) {
      fail("Should throw NullPointerException");
    }
  }

  /**
   * @since 2016-02-26
   * @see None
   * @objective Test 'setAttribute' function with null value for error throw
   *            checking
   * @target void setAttribute(String key, RcsValue value)
   * @test_data 1. key = null 2. value = null
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform setAttribute() API
   * @post_condition None
   * @expected The API should generate NullPointerException exception with
   *           message "value is null"
   **/
  public void testSetAttributeForValue_ETC_N() {
    RcsValue value = null;

    try {
      m_ResourceObject.setAttribute(ATTR_KEY_TEMPERATURE, value);
    } catch (NullPointerException e) {
      assertEquals("Can't find proper exception.", "value is null",
          e.getLocalizedMessage());
    } catch (RcsException e) {
      fail("Should throw NullPointerException");
    }
  }

  /**
   * @since 2015-10-23
   * @see None
   * @objective Test 'getAttributeValue' function with positive basic way
   * @target RcsValue getAttributeValue(String key)
   * @test_data key = "Temperature"
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform getAttributeValue() API using key
   * @post_condition None
   * @expected The returned value should not be empty
   **/
  public void testGetAttribute_FSV_P() {
    try {
      RcsValue value = m_ResourceObject.getAttributeValue(ATTR_KEY_TEMPERATURE);

      if (value.isNull()) {
        fail("Attribute Value is empty!!");
      } else {
        Log.d(LOG_TAG, "Attribute value is " + value);
      }
    } catch (RcsException e) {
      fail("Throw RcsException for getAttributeValue with key: "
          + ATTR_KEY_TEMPERATURE);
    }
  }

  /**
   * @since 2015-10-23
   * @see None
   * @objective Test 'getAttributeValue' function with null key
   * @target RcsValue getAttributeValue(String key)
   * @test_data key = null
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform getAttributeValue() API using key
   * @post_condition None
   * @expected Should generate RcsException
   **/
  public void testGetAttribute_NV_N() {
    try {
      m_ResourceObject.getAttributeValue(null);
      fail("Successfully called getAttributeValue with null key, but it should throw RcsException.");
    } catch (NullPointerException e) {
    } catch (RcsException e) {
      fail("Should throw NullPointerException");
    }
  }

  /**
   * @since 2016-02-26
   * @see None
   * @objective Test 'getAttributeValue' function with null key for error throw
   *            checking
   * @target RcsValue getAttributeValue(String key)
   * @test_data key = null
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform getAttributeValue() API using key
   * @post_condition None
   * @expected Should generate RcsException with message "key is null"
   **/
  public void testGetAttribute_ETC_N() {
    try {
      m_ResourceObject.getAttributeValue(null);
      fail("Successfully called getAttributeValue with null key, but it should throw RcsException.");
    } catch (NullPointerException e) {
      assertEquals(
          "Can't get proper exception for getAttributeValue with null key.",
          "key is null", e.getLocalizedMessage());
    } catch (RcsException e) {
      fail("Should throw NullPointerException");
    }
  }

  /**
   * @since 2016-02-26
   * @see None
   * @objective Test 'getAttributeValue' function with empty key
   * @target RcsValue getAttributeValue(String key)
   * @test_data key = ""
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform getAttributeValue() API using key
   * @post_condition None
   * @expected Should not generate RcsException
   **/
  public void testGetAttribute_ESV_P() {
    try {
      m_ResourceObject.getAttributeValue(EMPTY_STRING_VALUE);
    } catch (RcsException e) {
      assertTrue(
          "Get exception when calling getAttributeValue with empty key.", false);
    }
  }

  /**
   * @since 2015-10-23
   * @see None
   * @objective Test 'getAttributes' function with positive basic way
   * @target RcsResourceAttributes getAttributes()
   * @test_data None
   * @pre_condition None
   * @procedure Perform getAttributes() API using key
   * @post_condition None
   * @expected The returned resource Attributes should not be empty
   **/
  public void testGetAttributes_SRC_P() {
    try {
      RcsResourceAttributes resAttributes = m_ResourceObject.getAttributes();

      if (resAttributes.size() <= 0) {
        fail("Unable to get Resource Attributes");
      }
    } catch (RcsException e) {
      fail("Throws exception while calling getAttributes API. Exception is: "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-10-23
   * @see None
   * @objective Test 'containsAttribute' function with positive basic way
   * @target boolean containsAttribute(String key)
   * @test_data key = "Temperature"
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform containsAttribute() API using key
   * @post_condition None
   * @expected The returned value should be true
   **/
  public void testContainsAttribute_SRC_P() {
    try {
      boolean hasAttribute = m_ResourceObject
          .containsAttribute(ATTR_KEY_TEMPERATURE);

      if (hasAttribute == false) {
        fail(ATTR_KEY_TEMPERATURE + " attribute is not available.");
      }
    } catch (RcsException e) {
      fail("Got RcsException while calling containsAttribute API with "
          + ATTR_KEY_TEMPERATURE + "key. Exception is: "
          + e.getLocalizedMessage());
    } catch (NullPointerException enull) {
      fail("Got NullPointerException while calling containsAttribute API with "
          + ATTR_KEY_TEMPERATURE + "key. Exception is: "
          + enull.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-10-23
   * @see None
   * @objective Test 'containsAttribute' function with null key
   * @target boolean containsAttribute(String key)
   * @test_data key = null
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform containsAttribute() API using key
   * @post_condition None
   * @expected Should throw exception.
   **/
  public void testContainsAttribute_NV_N() {
    boolean hasAttribute = false;

    try {
      hasAttribute = m_ResourceObject.containsAttribute(null);

      assertTrue(
          "Successfully called when key is null for containsAttribute() API.",
          false);
    } catch (NullPointerException e) {
    } catch (RcsException e) {
      fail("Should throw NullPointerException");
    }

    if (hasAttribute == true) {
      fail("Got attibute for null key.");
    }
  }

  /**
   * @since 2015-10-23
   * @see None
   * @objective Test 'containsAttribute' function with empty key
   * @target boolean containsAttribute(String key)
   * @test_data key = ""
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform containsAttribute() API using key
   * @post_condition None
   * @expected The returned value should be false
   **/
  public void testContainsAttribute_ESV_N() {
    try {
      boolean hasAttribute = m_ResourceObject
          .containsAttribute(EMPTY_STRING_VALUE);

      assertEquals("Got attibute for empty key.", false, hasAttribute);
    } catch (RcsException e) {
      fail("Got RcsException while calling containsAttribute API with empty key. Exception is: "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2016-02-29
   * @see None
   * @objective Test 'containsAttribute' function with null key for error throw
   *            checking
   * @target boolean containsAttribute(String key)
   * @test_data key = null
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform containsAttribute() API using key
   * @post_condition None
   * @expected Should throw exception with message "key is null".
   **/
  public void testContainsAttribute_ETC_N() {
    try {
      m_ResourceObject.containsAttribute(null);
    } catch (NullPointerException e) {
      assertEquals(
          "Can't find proper exception message for containsAttribute() with null key.",
          "key is null", e.getLocalizedMessage());
    } catch (RcsException e) {
      fail("Should throw NullPointerException");
    }
  }

  /**
   * @since 2015-10-23
   * @see None
   * @objective Test 'notifyObservers' function with positive basic way
   * @target void notifyObservers()
   * @test_data None
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform notifyObservers() API using key
   * @post_condition None
   * @expected Should not generate exception
   **/
  public void testNotifyObservers_SRC_P() {
    try {
      m_ResourceObject.notifyObservers();
    } catch (RcsException e) {
      fail("Unable to notifyObservers, exception occurred: "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-10-23
   * @see None
   * @objective Test 'setGetRequestHandler' function with positive basic way
   * @target void setGetRequestHandler(GetRequestHandler handler)
   * @test_data GetRequestHandler
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform setGetRequestHandler() API using GetRequestHandler
   * @post_condition None
   * @expected No exception should be generated
   **/
  public void testSetGetRequestHandler_SRC_P() {
    try {
      m_ResourceObject.setGetRequestHandler(m_REHelper.mGetRequestHandler);
    } catch (RcsException e) {
      fail("Unable to set GetRequestHandler, exception occurred: "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-10-23
   * @see None
   * @objective Test 'setGetRequestHandler' function with null handler
   * @target void setGetRequestHandler(GetRequestHandler handler)
   * @test_data None
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform setGetRequestHandler() API using key
   * @post_condition None
   * @expected RcsException should be generated
   **/
  public void testSetGetRequestHandler_NV_N() {
    try {
      m_ResourceObject.setGetRequestHandler(null);
    } catch (RcsException e) {
      assertTrue("Can't set null to the handler for setGetRequestHandler().",
          false);
    }
  }

  /**
   * @since 2015-10-23
   * @see None
   * @objective Test 'setGetRequestHandler' function with positive basic way
   * @target setSetRequestHandler(SetRequestHandler handler)
   * @test_data SetRequestHandler
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform setSetRequestHandler() API using SetRequestHandler
   * @post_condition None
   * @expected No exception should be generated
   **/
  public void testSetSetAttributesRequestHandler_SRC_P() {
    try {
      m_ResourceObject.setSetRequestHandler(m_REHelper.mSetRequestHandler);
    } catch (RcsException e) {
      fail("Unable to set SetRequestHandler, exception occurred: "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-10-23
   * @see None
   * @objective Test 'setSetRequestHandler' function with null handler
   * @target setSetRequestHandler(SetRequestHandler handler)
   * @test_data Null handler
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform setSetRequestHandler() API
   * @post_condition None
   * @expected should throw exception
   **/
  public void testSetSetAttributesRequestHandler_NV_N() {
    try {
      m_ResourceObject.setSetRequestHandler(null);
    } catch (RcsException e) {
      fail("Unable to set SetRequestHandler, exception occurred: "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-10-23
   * @see None
   * @objective Test 'removeAttribute' function with positive basic way
   * @target boolean removeAttribute(String key)
   * @test_data key = "Temperature"
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform removeAttribute() API using key
   * @post_condition None
   * @expected The returned value should not be empty
   **/
  public void testRemoveAttribute_FSV_P() {
    try {
      boolean isRemoved = m_ResourceObject
          .removeAttribute(ATTR_KEY_TEMPERATURE);

      assertEquals("Can't remove attribute.", true, isRemoved);
    } catch (RcsException e) {
      fail("Throw RcsException for removeAttribute with key: "
          + ATTR_KEY_TEMPERATURE);
    }
  }

  /**
   * @since 2015-10-23
   * @see None
   * @objective Test 'removeAttribute' function with empty key
   * @target boolean removeAttribute(String key)
   * @test_data key = ""
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform removeAttribute() API using key
   * @post_condition None
   * @expected Should do nothing for Empty key
   **/
  public void testRemoveAttribute_ESV_N() {
    try {
      boolean isRemoved = m_ResourceObject.removeAttribute(EMPTY_STRING_VALUE);

      assertEquals("Attribute removed successfully for empty key.", false,
          isRemoved);
    } catch (RcsException e) {
      fail("Throw RcsException for removeAttribute with empty key: "
          + ATTR_KEY_TEMPERATURE);
    }
  }

  /**
   * @since 2016-02-29
   * @see None
   * @objective Test 'removeAttribute' function with null key
   * @target boolean removeAttribute(String key)
   * @test_data key = null
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform removeAttribute() API using key
   * @post_condition None
   * @expected Should throw exception.
   **/
  public void testRemoveAttribute_NV_N() {
    try {
      m_ResourceObject.removeAttribute(null);
      assertTrue("Successfully called removeAttribute() API with null key.",
          false);
    } catch (NullPointerException e) {
    } catch (RcsException e) {
      fail("Should throw NullPointerException");
    }
  }

  /**
   * @since 2016-02-29
   * @see None
   * @objective Test 'removeAttribute' function with null key to error throw
   *            check
   * @target boolean removeAttribute(String key)
   * @test_data key = null
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform removeAttribute() API using key
   * @post_condition None
   * @expected Should throw exception.
   **/
  public void testRemoveAttribute_ETC_N() {
    try {
      m_ResourceObject.removeAttribute(null);
    } catch (NullPointerException e) {
      assertEquals(
          "Can't get proper exception message for removeAttribute() API with null key.",
          "key is null", e.getLocalizedMessage());
    } catch (RcsException e) {
      fail("Should throw NullPointerException");
    }
  }

  /**
   * @since 2016-02-29
   * @see None
   * @objective Test 'addAttributeUpdatedListener' function with negative basic
   *            way
   * @target void addAttributeUpdatedListener(String key,
   *         OnAttributeUpdatedListener listener)
   * @test_data key = Temperature, listener = mAttributeUpdatedListener
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform addAttributeUpdatedListener() API using key and listener
   * @post_condition None
   * @expected Should perform successfully.
   **/
  public void testAddAttributeUpdatedListener_SRC_P() {
    try {
      m_ResourceObject.addAttributeUpdatedListener(ATTR_KEY_TEMPERATURE,
          m_REHelper.mAttributeUpdatedListener);
    } catch (RcsException e) {
      fail("Exception occurs at TestAddAttributeUpdatedListener_SRC_P. Exception is "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2016-02-29
   * @see None
   * @objective Test 'addAttributeUpdatedListener' function with Null Key
   * @target void addAttributeUpdatedListener(String key,
   *         OnAttributeUpdatedListener listener)
   * @test_data key = null, listener = mAttributeUpdatedListener
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform addAttributeUpdatedListener() API using key and listener
   * @post_condition None
   * @expected Should throw exception.
   **/
  public void testAddAttributeUpdatedListenerWithNullKey_NV_N() {
    try {
      m_ResourceObject.addAttributeUpdatedListener(null,
          m_REHelper.mAttributeUpdatedListener);
      fail("addAttributeUpdatedListener() API called successfully with null key.");
    } catch (NullPointerException e) {
    } catch (RcsException e) {
      fail("Should throw NullPointerException");
    }
  }

  /**
   * @since 2016-02-29
   * @see None
   * @objective Test 'addAttributeUpdatedListener' function with Null Listener
   * @target void addAttributeUpdatedListener(String key,
   *         OnAttributeUpdatedListener listener)
   * @test_data key = Temperature, listener = null
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform addAttributeUpdatedListener() API using key and listener
   * @post_condition None
   * @expected Should throw exception.
   **/
  public void testAddAttributeUpdatedListenerWithNullListener_NV_N() {
    try {
      m_ResourceObject.addAttributeUpdatedListener(null,
          m_REHelper.mAttributeUpdatedListener);
      fail("addAttributeUpdatedListener() API called successfully with null listener.");
    } catch (NullPointerException e) {
    } catch (RcsException e) {
      fail("Should throw NullPointerException");
    }
  }

  /**
   * @since 2016-02-29
   * @see None
   * @objective Test 'addAttributeUpdatedListener' function with Null Key to
   *            error throw check
   * @target void addAttributeUpdatedListener(String key,
   *         OnAttributeUpdatedListener listener)
   * @test_data key = null, listener = mAttributeUpdatedListener
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform addAttributeUpdatedListener() API using key and listener
   * @post_condition None
   * @expected Should throw exception.
   **/
  public void testAddAttributeUpdatedListenerWithNullKey_ETC_N() {
    try {
      m_ResourceObject.addAttributeUpdatedListener(null,
          m_REHelper.mAttributeUpdatedListener);
    } catch (NullPointerException e) {
      assertEquals("Can't find proper exception message.", "key is null.",
          e.getLocalizedMessage());
    } catch (RcsException e) {
      fail("Should throw NullPointerException");
    }
  }

  /**
   * @since 2016-02-29
   * @see None
   * @objective Test 'addAttributeUpdatedListener' function with Null Listener
   *            to error throw check
   * @target void addAttributeUpdatedListener(String key,
   *         OnAttributeUpdatedListener listener)
   * @test_data key = Temperature, listener = null
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform addAttributeUpdatedListener() API using key and listener
   * @post_condition None
   * @expected Should throw exception.
   **/
  public void testAddAttributeUpdatedListenerWithNullListener_ETC_N() {
    try {
      m_ResourceObject.addAttributeUpdatedListener(ATTR_KEY_TEMPERATURE, null);
    } catch (NullPointerException e) {
      assertEquals("Can't find proper exception message.", "listener is null.",
          e.getLocalizedMessage());
    } catch (RcsException e) {
      fail("Should throw NullPointerException");
    }
  }

  /**
   * @since 2016-02-29
   * @see None
   * @objective Test 'addAttributeUpdatedListener' function with empty key
   * @target void addAttributeUpdatedListener(String key,
   *         OnAttributeUpdatedListener listener)
   * @test_data key = "", listener = mAttributeUpdatedListener
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform addAttributeUpdatedListener() API using key and listener
   * @post_condition None
   * @expected Should not throw exception.
   **/
  public void testAddAttributeUpdatedListener_ESV_P() {
    try {
      m_ResourceObject.addAttributeUpdatedListener(EMPTY_STRING_VALUE,
          m_REHelper.mAttributeUpdatedListener);
    } catch (RcsException e) {
      fail("Exception occurs at when addAttributeUpdatedListener() API called with empty string.");
    }
  }

  /**
   * @since 2016-02-29
   * @see None
   * @objective Test 'removeAttributeUpdatedListener' function with positive
   *            basic way
   * @target booleam removeAttributeUpdatedListener(String key)
   * @test_data key = "Temperature"
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform removeAttributeUpdatedListener() API using key
   * @post_condition None
   * @expected Should not throw exception.
   **/
  public void testRemoveAttributeUpdatedListener_SRC_P() {
    try {
      m_ResourceObject.removeAttributeUpdatedListener(ATTR_KEY_TEMPERATURE);
    } catch (RcsException e) {
      fail("Exception occurs at when removeAttributeUpdatedListener() API called.");
    }
  }

  /**
   * @since 2016-02-29
   * @see None
   * @objective Test 'removeAttributeUpdatedListener' function with empty key
   * @target booleam removeAttributeUpdatedListener(String key)
   * @test_data key = ""
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform removeAttributeUpdatedListener() API
   * @post_condition None
   * @expected Should not throw exception.
   **/
  public void testRemoveAttributeUpdatedListener_ESV_P() {
    try {
      m_ResourceObject.removeAttributeUpdatedListener(EMPTY_STRING_VALUE);
    } catch (RcsException e) {
      fail("Exception occurs at when removeAttributeUpdatedListener() API called with empty key.");
    }
  }

  /**
   * @since 2016-02-29
   * @see None
   * @objective Test 'removeAttributeUpdatedListener' function with null key
   * @target booleam removeAttributeUpdatedListener(String key)
   * @test_data key = null
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform removeAttributeUpdatedListener() API
   * @post_condition None
   * @expected Should throw exception.
   **/
  public void testRemoveAttributeUpdatedListener_NV_N() {
    try {
      m_ResourceObject.removeAttributeUpdatedListener(null);

      fail("removeAttributeUpdatedListener API callled successfully with null key.");
    } catch (NullPointerException e) {
    } catch (RcsException e) {
      fail("Should throw NullPointerException");
    }
  }

  /**
   * @since 2016-02-29
   * @see None
   * @objective Test 'removeAttributeUpdatedListener' function with null key to
   *            error throw check
   * @target booleam removeAttributeUpdatedListener(String key)
   * @test_data key = null
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform removeAttributeUpdatedListener() API
   * @post_condition None
   * @expected Should throw exception with message "key is null"
   **/
  public void testRemoveAttributeUpdatedListener_ETC_N() {
    try {
      m_ResourceObject.removeAttributeUpdatedListener(null);
    } catch (NullPointerException e) {
      assertEquals(
          "Can't find proper exception message when calling removeAttributeUpdatedListener with null key.",
          "key is null", e.getLocalizedMessage());
    } catch (RcsException e) {
      fail("Should throw NullPointerException");
    }
  }

  /**
   * @since 2016-02-29
   * @see None
   * @objective Test 'setAutoNotifyPolicy' function with positive basic way
   * @target void setAutoNotifyPolicy(AutoNotifyPolicy policy)
   * @test_data policy = NEVER, ALWAYS and UPDATED
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform setAutoNotifyPolicy() API
   * @post_condition None
   * @expected Should not occur exception.
   **/
  public void testSetAutoNotifyPolicy_SRC_P() {
    try {
      m_ResourceObject.setAutoNotifyPolicy(AutoNotifyPolicy.NEVER);
    } catch (RcsException e) {
      fail("Exception occurs when calling setAutoNotifyPolicy API with NEVER.");
    }

    try {
      m_ResourceObject.setAutoNotifyPolicy(AutoNotifyPolicy.ALWAYS);
    } catch (RcsException e) {
      fail("Exception occurs when calling setAutoNotifyPolicy API with ALWAYS.");
    }

    try {
      m_ResourceObject.setAutoNotifyPolicy(AutoNotifyPolicy.UPDATED);
    } catch (RcsException e) {
      fail("Exception occurs when calling setAutoNotifyPolicy API with UPDATED.");
    }
  }

  /**
   * @since 2016-02-29
   * @see None
   * @objective Test 'setAutoNotifyPolicy' function with null policy
   * @target void setAutoNotifyPolicy(AutoNotifyPolicy policy)
   * @test_data policy = null
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform setAutoNotifyPolicy() API
   * @post_condition None
   * @expected Should not occur exception.
   **/
  public void testSetAutoNotifyPolicy_NV_N() {
    try {
      m_ResourceObject.setAutoNotifyPolicy(null);
      fail("Successfullly called setAutoNotifyPolicy() API with null policy.");
    } catch (NullPointerException e) {
    } catch (RcsException e) {
      fail("Should throw NullPointerException");
    }
  }

  /**
   * @since 2016-02-29
   * @see None
   * @objective Test 'setAutoNotifyPolicy' function with null policy to error
   *            throw check
   * @target void setAutoNotifyPolicy(AutoNotifyPolicy policy)
   * @test_data policy = null
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform setAutoNotifyPolicy() API
   * @post_condition None
   * @expected Should not occur exception.
   **/
  public void testSetAutoNotifyPolicy_ETC_N() {
    try {
      m_ResourceObject.setAutoNotifyPolicy(null);
    } catch (NullPointerException e) {
      assertEquals(
          "Can't find proper exception message when calling setAutoNotifyPolicy() with null policy.",
          "policy is null", e.getLocalizedMessage());
    } catch (RcsException e) {
      fail("Should throw NullPointerException");
    }
  }

  /**
   * @since 2016-02-29
   * @see None
   * @objective Test 'getAutoNotifyPolicy' function with positive basic way
   * @target AutoNotifyPolicy getAutoNotifyPolicy()
   * @test_data None
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform getAutoNotifyPolicy() API
   * @post_condition None
   * @expected Should not occur exception.
   **/
  public void testGetAutoNotifyPolicy_SRC_P() {
    try {
      m_ResourceObject.getAutoNotifyPolicy();
    } catch (RcsException e) {
      fail("Exception occurs when calling getAutoNotifyPolicy API.");
    }
  }

  /**
   * @since 2016-02-29
   * @see None
   * @objective Test 'setSetRequestHandlerPolicy' function with positive basic
   *            way
   * @target void setSetRequestHandlerPolicy(SetRequestHandlerPolicy policy)
   * @test_data policy = NEVER, ACCEPT
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform setSetRequestHandlerPolicy() API
   * @post_condition None
   * @expected Should not occur exception.
   **/
  public void testSetSetRequestHandlerPolicy_SRC_P() {
    try {
      m_ResourceObject
          .setSetRequestHandlerPolicy(SetRequestHandlerPolicy.NEVER);
    } catch (RcsException e) {
      fail("Exception occurs when calling setSetRequestHandlerPolicy API with NEVER.");
    }

    try {
      m_ResourceObject
          .setSetRequestHandlerPolicy(SetRequestHandlerPolicy.ACCEPT);
    } catch (RcsException e) {
      fail("Exception occurs when calling setSetRequestHandlerPolicy API with ACCEPT.");
    }
  }

  /**
   * @since 2016-02-29
   * @see None
   * @objective Test 'setSetRequestHandlerPolicy' function with null policy
   * @target void setSetRequestHandlerPolicy(SetRequestHandlerPolicy policy)
   * @test_data policy = NEVER, ACCEPT
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform setSetRequestHandlerPolicy() API
   * @post_condition None
   * @expected Should occur exception.
   **/
  public void testSetSetRequestHandlerPolicy_NV_N() {
    try {
      m_ResourceObject.setSetRequestHandlerPolicy(null);
      fail("Successfully called the setSetRequestHandlerPolicy API with null SetRequestHandlerPolicy.");
    } catch (NullPointerException e) {
    } catch (RcsException e) {
      fail("Should throw NullPointerException");
    }
  }

  /**
   * @since 2016-02-29
   * @see None
   * @objective Test 'setSetRequestHandlerPolicy' function with null policy to
   *            error throw check
   * @target void setSetRequestHandlerPolicy(SetRequestHandlerPolicy policy)
   * @test_data policy = NEVER, ACCEPT
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform setSetRequestHandlerPolicy() API
   * @post_condition None
   * @expected Should occur exception with message "policy is null".
   **/
  public void testSetSetRequestHandlerPolicy_ETC_N() {
    try {
      m_ResourceObject.setSetRequestHandlerPolicy(null);
    } catch (NullPointerException e) {
      assertEquals(
          "Can't find proper exception message when calling setSetRequestHandlerPolicy API with null.",
          "policy is null", e.getLocalizedMessage());
    } catch (RcsException e) {
      fail("Should throw NullPointerException");
    }
  }

  /**
   * @since 2016-02-29
   * @see None
   * @objective Test 'getSetRequestHandlerPolicy' function with positive basic
   *            way
   * @target SetRequestHandlerPolicy getSetRequestHandlerPolicy()
   * @test_data None
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform getSetRequestHandlerPolicy() API
   * @post_condition None
   * @expected Should not occur exception.
   **/
  public void testGetSetRequestHandlerPolicy_SRC_P() {
    try {
      m_ResourceObject.getSetRequestHandlerPolicy();
    } catch (RcsException e) {
      fail("Exception occurs when calling getSetRequestHandlerPolicy API.");
    }
  }

  /**
   * @since 2016-03-01
   * @see None
   * @objective Test 'getAttributesLock' function with positive basic way
   * @target AttributesLock getAttributesLock()
   * @test_data None
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform getAttributesLock() API
   * @post_condition None
   * @expected Should not occur exception.
   **/
  public void testGetAttributesLock_SRC_P() {
    try {
      m_ResourceObject.getAttributesLock();
    } catch (RcsException e) {
      fail("Exception occurs when calling getAttributesLock API.");
    }
  }
  
   /**
   * @since 2015-11-06
   * @see None
   * @objective Test 'setGetRequestHandler' function with positive basic way
   * @target void setSetRequestHandler()
   * @test_data SetRequestHandler
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform setSetRequestHandler() API using SetRequestHandler
   * @post_condition None
   * @expected No exception should be generated
   **/
  public void testSetSetAttributesRequestHandler_P() {
    try {
      m_ResourceObject.setSetRequestHandler(m_REHelper.mSetRequestHandler);
    } catch (RcsException e) {
      fail("Unable to set SetRequestHandler, exception occurred: "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-11-06
   * @see None
   * @objective Test 'setSetRequestHandler' function with negative basic way
   * @target void setSetRequestHandler()
   * @test_data None
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform setSetRequestHandler() API using key
   * @post_condition None
   * @expected No exception should be generated
   **/
  public void testSetSetAttributesRequestHandler_N() {
    try {
      m_ResourceObject.setSetRequestHandler(null);
    } catch (RcsException e) {
      fail("Unable to set SetRequestHandler, exception occurred: "
          + e.getLocalizedMessage());
    }
  }
}
