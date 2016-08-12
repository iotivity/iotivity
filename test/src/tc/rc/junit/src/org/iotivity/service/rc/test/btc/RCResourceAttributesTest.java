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
package org.iotivity.service.rc.test.btc;

import static org.iotivity.service.rc.test.helper.RCCommonUtilBTC.*;
import org.iotivity.service.rc.test.helper.RCHelperBTC;

import org.iotivity.service.resourcecontainer.RcsResourceAttributes;

import android.test.InstrumentationTestCase;

public class RCResourceAttributesTest extends InstrumentationTestCase {

	public static final String S_KEY = "test";
	public static final int N_VALUE = 30;

	/**
	 * @since 2016-03-11
	 * @see None
	 * @objective Test 'size' function test a positive way
	 * @target int size()
	 * @test_data None
	 * @pre_condition None
	 * @procedure Perform size() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAttributeSize_SRC_P() {
		RcsResourceAttributes rcsResourceAttributes = new RcsResourceAttributes();

		int attrSize = rcsResourceAttributes.size();

		assertEquals("Attribute size should 0 at initial.", attrSize, 0);
	}

	/**
	 * @since 2016-03-11
	 * @see Resource PUT
	 * @objective Test 'remove' function test with valid resource key value
	 * @target boolean remove()
	 * @test_data key = "test"
	 * @pre_condition Perform put method to add a key and value.
	 * @procedure Perform remove() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAttributeRemove_SRC_P() {
		RcsResourceAttributes rcsResourceAttributes = new RcsResourceAttributes();

		rcsResourceAttributes.put(S_KEY, N_VALUE);

		try {
			rcsResourceAttributes.remove(S_KEY);
		} catch (Exception e) {
			fail("Exception occurs when calling remove() API for RcsResourceAttributes.");
		}
	}

	/**
	 * @since 2016-03-11
	 * @see None
	 * @objective Test 'remove' function test with NULL key value
	 * @target boolean remove()
	 * @test_data key = null
	 * @pre_condition None
	 * @procedure Perform remove() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAttributeRemove_ETC_N() {
		RcsResourceAttributes rcsResourceAttributes = new RcsResourceAttributes();

		try {
			rcsResourceAttributes.remove(null);
		} catch (NullPointerException e) {
		} catch (Exception e) {
			fail("Should occur NullPointerException when calling remove() API.");
		}
	}

	/**
	 * @since 2016-03-11
	 * @see None
	 * @objective Test 'remove' function test with valid key value
	 * @target boolean remove()
	 * @test_data key = ""
	 * @pre_condition None
	 * @procedure Perform remove() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAttributeRemove_ESV_P() {
		RcsResourceAttributes rcsResourceAttributes = new RcsResourceAttributes();

		try {
			rcsResourceAttributes.remove(EMPTY_STRING);
		} catch (Exception e) {
			fail("Exception occurs when calling remove() API.");
		}
	}

	/**
	 * @since 2016-03-11
	 * @see None
	 * @objective Test 'clear' function test with positive way
	 * @target void clear()
	 * @test_data None
	 * @pre_condition None
	 * @procedure Perform clear() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAttributeClear_SRC_P() {
		RcsResourceAttributes rcsResourceAttributes = new RcsResourceAttributes();

		try {
			rcsResourceAttributes.clear();
		} catch (Exception e) {
			fail("Exception occurs when calling clear() API.");
		}
	}

	/**
	 * @since 2016-03-11
	 * @see Resource attributes PUT
	 * @objective Test 'contains' function test with valid key value
	 * @target void contains()
	 * @test_data key = "test"
	 * @pre_condition Put value with put method
	 * @procedure Perform contains() API
	 * @post_condition None
	 * @expected 1. No crash occurs 2. Should contain the key which is added
	 **/
	public void testAttributeContains_SRC_SRCC_P() {
		RcsResourceAttributes rcsResourceAttributes = new RcsResourceAttributes();

		boolean isContain = false;

		rcsResourceAttributes.put(S_KEY, N_VALUE);

		try {
			isContain = rcsResourceAttributes.contains(S_KEY);
		} catch (Exception e) {
			assertTrue(
					"Exception occurs when calling contains() API for RcsResourceAttributes.",
					false);
		}

		assertEquals("Should contain key", true, isContain);
	}

	/**
	 * @since 2016-03-12
	 * @see Resource PUT
	 * @objective Test 'contains' function test with empty key value
	 * @target void contains()
	 * @test_data key = ""
	 * @pre_condition Put value with put method
	 * @procedure Perform contains() API
	 * @post_condition None
	 * @expected 1. No crash occurs 2. Should contain the key which is added
	 **/
	public void testAttributeContains_ESV_N() {
		RcsResourceAttributes rcsResourceAttributes = new RcsResourceAttributes();

		boolean isContain = false;

		rcsResourceAttributes.put(S_KEY, N_VALUE);

		try {
			isContain = rcsResourceAttributes.contains(EMPTY_STRING);
		} catch (Exception e) {
			assertTrue(
					"Exception occurs when calling contains() API for RcsResourceAttributes.",
					false);
		}

		assertEquals("Should not contain key what is not added", false,
				isContain);
	}

	/**
	 * @since 2016-03-12
	 * @see None
	 * @objective Test 'contains' function test with NULL key value
	 * @target void contains()
	 * @test_data key = null
	 * @pre_condition None
	 * @procedure Perform contains() API
	 * @post_condition None
	 * @expected Exception should occur with "key is null" message.
	 **/
	public void testAttributeContains_NV_ETC_N() {
		RcsResourceAttributes rcsResourceAttributes = new RcsResourceAttributes();

		try {
			rcsResourceAttributes.contains(null);
			fail("Successfully called contains() API with null.");
		} catch (Exception e) {
			if (!e.getLocalizedMessage().equalsIgnoreCase(NULL_KEY_MESSAGE)) {
				fail("Didn't get proper exception message when calling contains() API with null.");
			}
		}
	}

	/**
	 * @since 2016-03-12
	 * @see None
	 * @objective Test 'put' function test with valid resource attributes
	 * @target void put(RcsResourceAttributes attrs)
	 * @test_data attrs = test attribute class reference
	 * @pre_condition Create Attributes
	 * @procedure Perform put(RcsResourceAttributes attrs) API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAttributePut_SRC_P() {
		RcsResourceAttributes rcsResourceAttributes = new RcsResourceAttributes();

		try {
			rcsResourceAttributes.put(RCHelperBTC.getResourceAttributes());
		} catch (Exception e) {
			assertTrue(
					"Exception occurs when calling put(RcsResourceAttributes attrs) API.",
					false);
		}
	}

	/**
	 * @since 2016-03-12
	 * @see None
	 * @objective Test 'put' function test with NULL resource attributes
	 * @target void put(RcsResourceAttributes attrs)
	 * @test_data attrs = null
	 * @pre_condition Create Attributes
	 * @procedure Perform put(RcsResourceAttributes attrs) API
	 * @post_condition None
	 * @expected Exception should occur
	 **/
	public void testAttributePut_NV_ETC_N() {
		RcsResourceAttributes rcsResourceAttributes = new RcsResourceAttributes();

		try {
			rcsResourceAttributes.put(null);
			fail("Successfully called void put(RcsResourceAttributes attrs) API with null attrs.");
		} catch (Exception e) {
		}
	}
}
