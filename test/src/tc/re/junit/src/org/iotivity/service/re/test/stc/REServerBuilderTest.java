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

import org.iotivity.service.re.test.helper.REAPIHelper;

import static org.iotivity.service.re.test.helper.ResourceUtil.*;

import org.iotivity.service.RcsException;
import org.iotivity.service.RcsDestroyedObjectException;
import org.iotivity.service.RcsResourceAttributes;
import org.iotivity.service.RcsValue;
import org.iotivity.service.server.RcsGetResponse;
import org.iotivity.service.server.RcsRequest;
import org.iotivity.service.server.RcsResourceObject;
import org.iotivity.service.server.RcsResourceObject.AutoNotifyPolicy;
import org.iotivity.service.server.RcsResourceObject.GetRequestHandler;
import org.iotivity.service.server.RcsResourceObject.SetRequestHandler;
import org.iotivity.service.server.RcsResourceObject.SetRequestHandlerPolicy;
import org.iotivity.service.server.RcsSetResponse;
import android.test.InstrumentationTestCase;
import android.util.Log;

public class REServerBuilderTest extends InstrumentationTestCase {
  private RcsResourceObject.Builder m_Builder;
  private RcsResourceObject         m_ResourceObject;
  private static final String       LOG_TAG = "RETest";
  REAPIHelper                       m_REHelper;
  int                               m_count = 10;

  protected void setUp() throws Exception {
    super.setUp();

    if (getName().equalsIgnoreCase("testSetDiscoverable_SQV_P")
        || getName().equalsIgnoreCase("testSetObservable_SQV_P")) {
      Log.i(LOG_TAG, "Found selected testcase");
    } else {
      m_REHelper = new REAPIHelper();

      m_Builder = new RcsResourceObject.Builder(TEMP_SENSOR_URI,
          TEMP_SENSOR_TYPE, DEFAULT_INTERFACE);

      RcsResourceAttributes attributes = new RcsResourceAttributes();
      attributes.put(ATTR_KEY_POWER, ATTR_VALUE_POWER_ON);

      m_Builder.setAttributes(attributes);

      m_ResourceObject = m_Builder.build();

      Log.i(LOG_TAG, "Resource created successfully");
    }

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

  private GetRequestHandler mGetRequestHandler = new GetRequestHandler() {
    @Override
    public RcsGetResponse onGetRequested(RcsRequest request,
        RcsResourceAttributes attrs) {
      Log.d(LOG_TAG, "Got a Get request from client, send default response "
          + "URI : " + request.getResourceUri());

      return RcsGetResponse.defaultAction();
    }
  };

  private SetRequestHandler mSetRequestHandler = new SetRequestHandler() {
    @Override
    public RcsSetResponse onSetRequested(RcsRequest request,
        RcsResourceAttributes attrs) {
      Log.d(LOG_TAG,
          "Got a Set request from client, send default response URI: "
              + request.getResourceUri());

      return RcsSetResponse.defaultAction();
    }
  };

  /**
   * @since 2015-11-06
   * @see None
   * @objective Test 'Builder' constructor with positive basic way
   * @target Builder(String uri, String resourceType, String resourceInterface)
   * @test_data None
   * @pre_condition None
   * @procedure Make Builder class with valid value
   * @post_condition None
   * @expected No crash occurs
   **/
  public void testBuilder_SQV_P() {
    boolean isBuilderNull = true;
    try {
      RcsResourceObject.Builder builder;
      builder = new RcsResourceObject.Builder(TEMP_SENSOR_URI, TEMP_SENSOR_TYPE,
          DEFAULT_INTERFACE);

      m_REHelper.waitInSecond(2);

      if (builder != null) {
        isBuilderNull = false;
        Log.i(LOG_TAG, "Builder is not null");
        builder = null;
      }
    } catch (NullPointerException e) {
      fail("Throw NullPointerException when called Builder API. "
          + e.getLocalizedMessage());
    }

    if (isBuilderNull) {
      fail("RcsResourceObject is not constructed.");
    }
  }

  /**
   * @since 2015-11-06
   * @see None
   * @objective Test 'setDiscoverable' function with positive basic way
   * @target Builder setDiscoverable(boolean isDiscoverable)
   * @test_data discoverable = true
   * @pre_condition Builder class should be initialized
   * @procedure 1. Perform setDiscoverable() API 2. Perform isDiscoverable() API
   * @post_condition None
   * @expected Returned builder should not be null
   **/
  public void testSetDiscoverable_SQV_P() {
    boolean discoverable = true;

    try {
      m_Builder.setDiscoverable(discoverable);

      boolean isDiscoverable = m_ResourceObject.isDiscoverable();

      if (discoverable != isDiscoverable) {
        fail("Discoverable is not setted using setDiscoverable API.");
      }
    } catch (Exception e) {
      fail("Unable to set discoverable to true, exception is "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-11-06
   * @see None
   * @objective Test 'setObservable' function with positive basic way
   * @target Builder setObservable(boolean isObservable)
   * @test_data observable = true
   * @pre_condition Builder class should be initialized
   * @procedure 1. Perform setObservable() API 2. Perform isObservable()
   * @post_condition None
   * @expected Returned builder should not be null
   **/
  public void testSetObservable_SQV_P() {
    Boolean observable = true;

    try {
      m_Builder.setObservable(observable);

      boolean isObservable = m_ResourceObject.isObservable();

      if (observable != isObservable) {
        fail("Observable is not setted using setObservable API.");
      }
    } catch (Exception e) {
      fail("Unable to set observable to true, exception is "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-11-06
   * @see None
   * @objective Test 'setAttributes' function with positive basic way
   * @target Builder setAttributes(RcsResourceAttributes attributes)
   * @test_data Resource Attributes, "Power" "On"
   * @pre_condition Builder class should be initialized
   * @procedure 1. Perform setAttributes API 2. Perform getAttributes()
   * @post_condition None
   * @expected Should not throw any exception
   **/
  public void testSetAttributes_SQV_P() {
    RcsResourceAttributes attrs = new RcsResourceAttributes();
    attrs.put(ATTR_KEY_POWER, ATTR_VALUE_POWER_ON);

    try {
      m_Builder.setAttributes(attrs);

      RcsResourceAttributes rAttributes = m_ResourceObject.getAttributes();

      if (attrs.get(ATTR_KEY_POWER) == rAttributes.get(ATTR_KEY_POWER)) {
        fail("Atrribute is not same as set value.");
      }
    } catch (Exception e) {
      fail("Unable to set Resource Attributes, exception is : "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-11-06
   * @see None
   * @objective Test 'setAttributes' function with empty attribute value
   * @target Builder setAttributes(RcsResourceAttributes attributes)
   * @test_data empty Resource Attribute Value
   * @pre_condition Builder class should be initialized
   * @procedure 1. Perform setAttributes API 2. Perform getAttributes()
   * @post_condition None
   * @expected Should throw exception
   **/
  public void testSetAttributes_EG_N() {
    RcsResourceAttributes attrs = new RcsResourceAttributes();
    attrs.put(ATTR_KEY_POWER, "");

    try {
      m_Builder.setAttributes(attrs);

      RcsResourceAttributes rAttributes = m_ResourceObject.getAttributes();

      if (attrs.get(ATTR_KEY_POWER) == rAttributes.get(ATTR_KEY_POWER)) {
        fail("Atrribute is not same as set value.");
      }
    } catch (Exception e) {
      fail("Unable to set Resource Attributes, exception is : "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-11-06
   * @see None
   * @objective Test 'setAttribute' function with positive basic way
   * @target void setAttribute(String key, RcsValue value)
   * @test_data 1. key = "Power" 2. value = "On"
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure 1. Perform getAttributeValue() API 2. Perform setAttribute() API
   * @post_condition None
   * @expected The API should not generate exception
   **/
  public void testSetAttribute_SQV_P() {
    RcsValue value = new RcsValue(ATTR_VALUE_POWER_ON);

    try {
      m_ResourceObject.setAttribute(ATTR_KEY_POWER, value);

      RcsValue rValue = m_ResourceObject.getAttributeValue(ATTR_KEY_POWER);

      if (value.asString().compareTo(rValue.asString()) != 0) {
        fail("Set value is not same as received value.");
      }
    } catch (RcsException e) {
      fail("Unable to set attribute, exception occurred: "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-11-06
   * @see None
   * @objective Test 'setAttribute' function with empty key
   * @target void setAttribute(String key, RcsValue value)
   * @test_data 1. key = "" 2. value = "On"
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure 1. Perform setAttribute() API 2. Perform getAttributeValue() API
   * @post_condition None
   * @expected The API should generate NullPointerException exception
   **/
  public void testSetAttribute_EG_N() {
    try {
      RcsValue rcsValue = new RcsValue(ATTR_VALUE_POWER_ON);
      m_ResourceObject.setAttribute("", rcsValue);

      RcsValue rValue = m_ResourceObject.getAttributeValue("");

      Log.d(LOG_TAG, "value " + rValue);

      if (rcsValue.asString().compareTo(rValue.asString()) != 0) {
        fail("Set value is not same as received value.");
      }
    } catch (RcsException e) {
      fail("Got RcsException at SetAttribute_N. Exception: "
          + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2015-11-06
   * @see None
   * @objective Test 'containsAttribute' function with positive basic way
   * @target boolean containsAttribute(String key)
   * @test_data key = "Temperature"
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure 1. Perform setAttribute() API 2. Perform containsAttribute() API
   *            using key
   * @post_condition None
   * @expected The returned value should be true
   **/
  public void testHasAttribute_SQV_P() {
    try {
      RcsValue value = new RcsValue(ATTR_VALUE_POWER_ON);
      m_ResourceObject.setAttribute(ATTR_KEY_POWER, value);

      boolean hasAttribute = m_ResourceObject.containsAttribute(ATTR_KEY_POWER);

      if (hasAttribute == false) {
        fail("\"" + ATTR_KEY_POWER + "\"" + " attribute is not available.");
      }
    } catch (RcsException e) {
      fail("Got RcsException while calling containsAttribute API with "
          + ATTR_KEY_POWER + "key. Exception is: " + e.getLocalizedMessage());
    }
  }

  /**
   * @since 2016-07-25
   * @see None
   * @objective Test 'containsAttribute' function with empty key
   * @target boolean containsAttribute(String key)
   * @test_data key = ""
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure 1. Perform setAttribute() API with empty string value 2. Perform
   *            containsAttribute() API using empty key
   * @post_condition None
   * @expected The API should throw exception
   **/
  public void testContainsAttribute_ESV_N() {
    boolean isException = false;

    try {
      RcsValue value = new RcsValue(ATTR_VALUE_POWER_ON);
      m_ResourceObject.setAttribute(DEFAULT_EMPTY_STRING_VALUE, value);

      boolean hasAttribute = m_ResourceObject
          .containsAttribute(DEFAULT_EMPTY_STRING_VALUE);

      if (hasAttribute == true) {
        fail("\"" + DEFAULT_EMPTY_STRING_VALUE + "\""
            + " attribute is not available.");
      }

    } catch (Exception e) {
      isException = true;

    } finally {
      if (!isException)
        fail("No exception found!");
    }
  }

  /**
   * @since 2016-07-25
   * @see None
   * @objective Test 'setAutoNotifyPolicy' & 'getAutoNotifyPolicy' functions
   *            with sequential validation
   * @target 1. void setAutoNotifyPolicy(AutoNotifyPolicy policy) 2.
   *         AutoNotifyPolicy getAutoNotifyPolicy()
   * @test_data policy = NEVER, ALWAYS and UPDATED
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure 1. Perform setAutoNotifyPolicy() and getAutoNotifyPolicy() API
   *            with multiple policies
   * @post_condition None
   * @expected Should not occur any exception.
   **/
  public void testSetGetAutoNotifyPolicy_SQV_P() {
    try {
      m_ResourceObject.setAutoNotifyPolicy(AutoNotifyPolicy.NEVER);
      m_ResourceObject.getAutoNotifyPolicy();

    } catch (Exception e) {
      fail("Exception occurs when calling setAutoNotifyPolicy API with NEVER.");
    }

    try {
      m_ResourceObject.setAutoNotifyPolicy(AutoNotifyPolicy.ALWAYS);
      m_ResourceObject.getAutoNotifyPolicy();

    } catch (Exception e) {
      fail(
          "Exception occurs when calling setAutoNotifyPolicy API with ALWAYS.");
    }

    try {
      m_ResourceObject.setAutoNotifyPolicy(AutoNotifyPolicy.UPDATED);
      m_ResourceObject.getAutoNotifyPolicy();

    } catch (Exception e) {
      fail(
          "Exception occurs when calling setAutoNotifyPolicy API with UPDATED.");
    }
  }

  /**
   * @since 2016-07-25
   * @see None
   * @objective Test 'setSetRequestHandlerPolicy' and
   *            'getSetRequestHandlerPolicy' functions with sequential
   *            validation
   * @target 1. void setSetRequestHandlerPolicy(SetRequestHandlerPolicy policy)
   *         2. SetRequestHandlerPolicy getSetRequestHandlerPolicy()
   * @test_data policy = NEVER, ACCEPT
   * @pre_condition 1. Builder class should be initialized 2. Resource object
   *                should be built
   * @procedure Perform setSetRequestHandlerPolicy() and
   *            getSetRequestHandlerPolicy() API with multiple policies
   * @post_condition None
   * @expected Should not occur exception.
   **/
  public void testSetSetGetRequestHandlerPolicy_SQV_P() {
    try {
      m_ResourceObject
      .setSetRequestHandlerPolicy(SetRequestHandlerPolicy.NEVER);
      m_ResourceObject.getSetRequestHandlerPolicy();

    } catch (Exception e) {
      fail(
          "Exception occurs when calling setSetRequestHandlerPolicy API with NEVER.");
    }

    try {
      m_ResourceObject
      .setSetRequestHandlerPolicy(SetRequestHandlerPolicy.ACCEPT);
      m_ResourceObject.getSetRequestHandlerPolicy();

    } catch (Exception e) {
      fail(
          "Exception occurs when calling setSetRequestHandlerPolicy API with ACCEPT.");
    }
  }

}
