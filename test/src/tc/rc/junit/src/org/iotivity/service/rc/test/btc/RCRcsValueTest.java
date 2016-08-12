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

import org.iotivity.service.resourcecontainer.RcsResourceAttributes;
import org.iotivity.service.resourcecontainer.RcsValue;

import android.test.InstrumentationTestCase;

public class RCRcsValueTest extends InstrumentationTestCase {
	private static final int MIN_ARRAY_SIZE = 1;

	private RcsValue m_RcsValueInt = new RcsValue(MIN_ARRAY_SIZE);
	private RcsValue m_RcsValueBool = new RcsValue(true);

	protected void setUp() throws Exception {
		super.setUp();
	}

	protected void tearDown() throws Exception {
		super.tearDown();
	}

	/**
	 * @since 2016-03-12
	 * @see None
	 * @objective Test 'RcsValue()' constructor with positive way
	 * @target RcsValue()
	 * @test_data None
	 * @pre_condition None
	 * @procedure Perform RcsValue() constructor
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testRcsValue_SRC_P() {
		try {
			new RcsValue();
		} catch (Exception e) {
			fail("Exception occurs when creating object reference using RcsValue().");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see None
	 * @objective Test 'RcsValue()' constructor with boolean value
	 * @target RcsValue(boolean value)
	 * @test_data None
	 * @pre_condition None
	 * @procedure Perform RcsValue(boolean value) constructor
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testRcsValueBool_SRC_P() {
		boolean value = true;

		try {
			new RcsValue(value);
		} catch (Exception e) {
			fail("Exception occurs when creating object reference using RcsValue(boolean value).");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see None
	 * @objective Test 'RcsValue()' constructor with boolean array
	 * @target RcsValue(boolean[] value)
	 * @test_data None
	 * @pre_condition None
	 * @procedure Perform RcsValue(boolean[] value) constructor
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testRcsValueBoolArray_SRC_P() {
		boolean[] value = new boolean[MIN_ARRAY_SIZE];

		try {
			assertTrue(new RcsValue(value) instanceof Object);
		} catch (Exception e) {
			fail("Exception occurs when creating object reference using RcsValue(boolean[] value)");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see None
	 * @objective Test 'RcsValue()' constructor with boolean NULL
	 * @target RcsValue(boolean[] value)
	 * @test_data value = null
	 * @pre_condition None
	 * @procedure Perform RcsValue(boolean[] value) constructor
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testRcsValueBoolArray_NV_ETC_N() {
		boolean[] value = null;

		try {
			assertEquals(new RcsValue(value), value);
			fail("Successfully called RcsValue(boolean[] value) with null value");
		} catch (NullPointerException e) {
		} catch (Exception e) {
			fail("Should throw NullPointerException when calling RcsValue(boolean[] value) with null value");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see None
	 * @objective Test 'RcsValue()' constructor with boolean 2D array
	 * @target RcsValue(boolean[][] value)
	 * @test_data None
	 * @pre_condition None
	 * @procedure Perform RcsValue(boolean[][] value) constructor
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testRcsValueBoolArray2D_SRC_P() {
		boolean[][] value = new boolean[MIN_ARRAY_SIZE][MIN_ARRAY_SIZE];

		try {
			assertTrue(new RcsValue(value) instanceof Object);
		} catch (Exception e) {
			fail("Exception occurs when creating object reference using RcsValue(boolean[][] value)");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see None
	 * @objective Test 'RcsValue()' constructor with boolean NULL 2D array
	 * @target RcsValue(boolean[] value)
	 * @test_data value = null
	 * @pre_condition None
	 * @procedure Perform RcsValue(boolean[] value) constructor
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testRcsValueBoolArray2D_NV_ETC_N() {
		boolean[][] value = null;

		try {
			assertEquals(new RcsValue(value), value);
			fail("Successfully called RcsValue(boolean[][] value) with null value");
		} catch (NullPointerException e) {
		} catch (Exception e) {
			fail("Should throw NullPointerException when calling RcsValue(boolean[][] value) with null value");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see None
	 * @objective Test 'RcsValue()' constructor with boolean 3D array
	 * @target RcsValue(boolean[][][] value)
	 * @test_data None
	 * @pre_condition None
	 * @procedure Perform RcsValue(boolean[][][] value) constructor
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testRcsValueBoolArray3D_SRC_P() {
		boolean[][][] value = new boolean[MIN_ARRAY_SIZE][MIN_ARRAY_SIZE][MIN_ARRAY_SIZE];

		try {
			assertTrue(new RcsValue(value) instanceof Object);
		} catch (Exception e) {
			fail("Exception occurs when creating object reference using RcsValue(boolean[][][] value)");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see None
	 * @objective Test 'RcsValue()' constructor with NULL boolean 3D array
	 * @target RcsValue(boolean[][][] value)
	 * @test_data value = null
	 * @pre_condition None
	 * @procedure Perform RcsValue(boolean[][][] value) constructor
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testRcsValueBoolArray3D_NV_ETC_N() {
		boolean[][][] value = null;

		try {
			assertEquals(new RcsValue(value), value);
			fail("Successfully called RcsValue(boolean[][][] value) with null value");
		} catch (NullPointerException e) {
		} catch (Exception e) {
			fail("Should throw NullPointerException when calling RcsValue(boolean[][][] value) with null value");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see None
	 * @objective Test 'RcsValue()' constructor with integer
	 * @target RcsValue(int value)
	 * @test_data None
	 * @pre_condition None
	 * @procedure Perform RcsValue(int value) constructor
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testRcsValueInt_RV_P() {
		int value = 0;

		try {
			assertTrue(new RcsValue(value) instanceof Object);
		} catch (Exception e) {
			fail("Exception occurs when creating object reference using RcsValue(int value).");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see None
	 * @objective Test 'RcsValue()' constructor with integer array
	 * @target RcsValue(int[] value)
	 * @test_data None
	 * @pre_condition None
	 * @procedure Perform RcsValue(int[] value) constructor
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testRcsValueIntArray_SRC_P() {
		int[] value = new int[MIN_ARRAY_SIZE];

		try {
			assertTrue(new RcsValue(value) instanceof Object);
		} catch (Exception e) {
			fail("Exception occurs when creating object reference using RcsValue(int[] value)");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see None
	 * @objective Test 'RcsValue()' constructor with integer NULL array
	 * @target RcsValue(int[] value)
	 * @test_data value = null
	 * @pre_condition None
	 * @procedure Perform RcsValue(int[] value) constructor
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testRcsValueIntArray_NV_ETC_N() {
		int[] value = null;

		try {
			assertEquals(new RcsValue(value), value);
			fail("Successfully called RcsValue(int[] value) with null value");
		} catch (NullPointerException e) {
		} catch (Exception e) {
			fail("Should throw NullPointerException when calling RcsValue(int[] value) with null value");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see None
	 * @objective Test 'RcsValue()' constructor with integer 2D array
	 * @target RcsValue(int[][] value)
	 * @test_data None
	 * @pre_condition None
	 * @procedure Perform RcsValue(int[][] value) constructor
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testRcsValueIntArray2D_SRC_P() {
		int[][] value = new int[MIN_ARRAY_SIZE][MIN_ARRAY_SIZE];

		try {
			assertTrue(new RcsValue(value) instanceof Object);
		} catch (Exception e) {
			fail("Exception occurs when creating object reference using RcsValue(boolean[][] value)");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see None
	 * @objective Test 'RcsValue()' constructor with integer NULL 2D array
	 * @target RcsValue(int[][] value)
	 * @test_data value = null
	 * @pre_condition None
	 * @procedure Perform RcsValue(int[][] value) constructor
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testRcsValueIntArray2D_NV_ETC_N() {
		int[][] value = null;

		try {
			assertEquals(new RcsValue(value), value);
			fail("Successfully called RcsValue(int[][] value) with null value");
		} catch (NullPointerException e) {
		} catch (Exception e) {
			fail("Should throw NullPointerException when calling RcsValue(int[][] value) with null value");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see None
	 * @objective Test 'RcsValue()' constructor with integer 3D array
	 * @target RcsValue(int[][][] value)
	 * @test_data None
	 * @pre_condition None
	 * @procedure Perform RcsValue(int[][][] value) constructor
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testRcsValueIntArray3D_SRC_P() {
		int[][][] value = new int[MIN_ARRAY_SIZE][MIN_ARRAY_SIZE][MIN_ARRAY_SIZE];

		try {
			assertTrue(new RcsValue(value) instanceof Object);
		} catch (Exception e) {
			fail("Exception occurs when creating object reference using RcsValue(int[][][] value)");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see None
	 * @objective Test 'RcsValue()' constructor with integer NULL 3D array
	 * @target RcsValue(int[][][] value)
	 * @test_data value = null
	 * @pre_condition None
	 * @procedure Perform RcsValue(int[][][] value) constructor
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testRcsValueIntArray3D_NV_ETC_N() {
		int[][][] value = null;

		try {
			assertEquals(new RcsValue(value), value);
			fail("Successfully called RcsValue(int[][][] value) with null value");
		} catch (NullPointerException e) {
		} catch (Exception e) {
			fail("Should throw NullPointerException when calling RcsValue(int[][][] value) with null value");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see None
	 * @objective Test 'RcsValue()' constructor with Double
	 * @target RcsValue(double value)
	 * @test_data None
	 * @pre_condition None
	 * @procedure Perform RcsValue(double value) constructor
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testRcsValueDouble_SRC_P() {
		double value = 0;

		try {
			assertTrue(new RcsValue(value) instanceof Object);
		} catch (Exception e) {
			fail("Exception occurs when creating object reference using RcsValue(double value).");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see None
	 * @objective Test 'RcsValue()' constructor with Double array
	 * @target RcsValue(int[] value)
	 * @test_data None
	 * @pre_condition None
	 * @procedure Perform RcsValue(int[] value) constructor
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testRcsValueDoubleArray_SRC_P() {
		double[] value = new double[MIN_ARRAY_SIZE];

		try {
			assertTrue(new RcsValue(value) instanceof Object);
		} catch (Exception e) {
			fail("Exception occurs when creating object reference using RcsValue(double[] value)");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see None
	 * @objective Test 'RcsValue()' constructor with Double NULL array
	 * @target RcsValue(double[] value)
	 * @test_data value = null
	 * @pre_condition None
	 * @procedure Perform RcsValue(double[] value) constructor
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testRcsValueDoubleArray_NV_ETC_N() {
		double[] value = null;

		try {
			assertEquals(new RcsValue(value), value);
			fail("Successfully called RcsValue(double[] value) with null value");
		} catch (NullPointerException e) {
		} catch (Exception e) {
			fail("Should throw NullPointerException when calling RcsValue(double[] value) with null value");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see None
	 * @objective Test 'RcsValue()' constructor with Double 2D array
	 * @target RcsValue(double[][] value)
	 * @test_data None
	 * @pre_condition None
	 * @procedure Perform RcsValue(double[][] value) constructor
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testRcsValueDoubleArray2D_SRC_P() {
		double[][] value = new double[MIN_ARRAY_SIZE][MIN_ARRAY_SIZE];

		try {
			assertTrue(new RcsValue(value) instanceof Object);
		} catch (Exception e) {
			fail("Exception occurs when creating object reference using RcsValue(double[][] value)");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see None
	 * @objective Test 'RcsValue()' constructor with Double NULL 2D array
	 * @target RcsValue(double[][] value)
	 * @test_data value = null
	 * @pre_condition None
	 * @procedure Perform RcsValue(double[][] value) constructor
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testRcsValueDoubleArray2D_NV_ETC_N() {
		double[][] value = null;

		try {
			assertEquals(new RcsValue(value), value);
			fail("Successfully called RcsValue(double[][] value) with null value");
		} catch (NullPointerException e) {
		} catch (Exception e) {
			fail("Should throw NullPointerException when calling RcsValue(double[][] value) with null value");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see None
	 * @objective Test 'RcsValue()' constructor with Double 3D array
	 * @target RcsValue(double[][][] value)
	 * @test_data None
	 * @pre_condition None
	 * @procedure Perform RcsValue(double[][][] value) constructor
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testRcsValueDoubleArray3D_SRC_P() {
		double[][][] value = new double[MIN_ARRAY_SIZE][MIN_ARRAY_SIZE][MIN_ARRAY_SIZE];

		try {
			assertTrue(new RcsValue(value) instanceof Object);
		} catch (Exception e) {
			fail("Exception occurs when creating object reference using RcsValue(double[][][] value)");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see None
	 * @objective Test 'RcsValue()' constructor with Double NULL 3D array
	 * @target RcsValue(double[][][] value)
	 * @test_data value = null
	 * @pre_condition None
	 * @procedure Perform RcsValue(double[][][] value) constructor
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testRcsValueDoubleArray3D_NV_ETC_N() {
		double[][][] value = null;

		try {
			assertEquals(new RcsValue(value), value);
			fail("Successfully called RcsValue(double[][][] value) with null value");
		} catch (NullPointerException e) {
		} catch (Exception e) {
			fail("Should throw NullPointerException when calling RcsValue(double[][][] value) with null value");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see None
	 * @objective Test 'RcsValue()' constructor with String
	 * @target RcsValue(String value)
	 * @test_data None
	 * @pre_condition None
	 * @procedure Perform RcsValue(String value) constructor
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testRcsValueString_SRC_P() {
		String value = "value";

		try {
			assertTrue(new RcsValue(value) instanceof Object);
		} catch (Exception e) {
			fail("Exception occurs when creating object reference using RcsValue(String value).");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see None
	 * @objective Test 'RcsValue()' constructor with String array
	 * @target RcsValue(String[] value)
	 * @test_data None
	 * @pre_condition None
	 * @procedure Perform RcsValue(String[] value) constructor
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testRcsValueStringArray_SRC_P() {
		String[] value = new String[MIN_ARRAY_SIZE];

		try {
			assertTrue(new RcsValue(value) instanceof Object);
		} catch (Exception e) {
			fail("Exception occurs when creating object reference using RcsValue(String[] value)");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see None
	 * @objective Test 'RcsValue()' constructor with NULL String array
	 * @target RcsValue(double[] value)
	 * @test_data value = null
	 * @pre_condition None
	 * @procedure Perform RcsValue(double[] value) constructor
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testRcsValueStringArray_NV_ETC_N() {
		double[] value = null;

		try {
			assertEquals(new RcsValue(value), value);
			fail("Successfully called RcsValue(String[] value) with null value");
		} catch (NullPointerException e) {
		} catch (Exception e) {
			fail("Should throw NullPointerException when calling RcsValue(String[] value) with null value");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see None
	 * @objective Test 'RcsValue()' constructor with String 2D array
	 * @target RcsValue(String[][] value)
	 * @test_data None
	 * @pre_condition None
	 * @procedure Perform RcsValue(double[][] value) constructor
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testRcsValueStringArray2D_SRC_P() {
		String[][] value = new String[MIN_ARRAY_SIZE][MIN_ARRAY_SIZE];

		try {
			assertTrue(new RcsValue(value) instanceof Object);
		} catch (Exception e) {
			fail("Exception occurs when creating object reference using RcsValue(double[][] value)");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see None
	 * @objective Test 'RcsValue()' constructor with NULL String 2D array
	 * @target RcsValue(String[][] value)
	 * @test_data value = null
	 * @pre_condition None
	 * @procedure Perform RcsValue(String[][] value) constructor
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testRcsValueStringArray2D_NV_ETC_N() {
		String[][] value = null;

		try {
			assertEquals(new RcsValue(value), value);
			fail("Successfully called RcsValue(String[][] value) with null value");
		} catch (NullPointerException e) {
		} catch (Exception e) {
			fail("Should throw NullPointerException when calling RcsValue(String[][] value) with null value");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see None
	 * @objective Test 'RcsValue()' constructor with String 3D array
	 * @target RcsValue(String[][][] value)
	 * @test_data None
	 * @pre_condition None
	 * @procedure Perform RcsValue(String[][][] value) constructor
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testRcsValueStringArray3D_SRC_P() {
		String[][][] value = new String[MIN_ARRAY_SIZE][MIN_ARRAY_SIZE][MIN_ARRAY_SIZE];

		try {
			assertTrue(new RcsValue(value) instanceof Object);
		} catch (Exception e) {
			fail("Exception occurs when creating object reference using RcsValue(String[][][] value)");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see None
	 * @objective Test 'RcsValue()' constructor with NULL String 3D array
	 * @target RcsValue(String[][][] value)
	 * @test_data value = null
	 * @pre_condition None
	 * @procedure Perform RcsValue(String[][][] value) constructor
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testRcsValueStringArray3D_NV_ETC_N() {
		String[][][] value = null;

		try {
			assertEquals(new RcsValue(value), value);
			fail("Successfully called RcsValue(String[][][] value) with null value");
		} catch (NullPointerException e) {
		} catch (Exception e) {
			fail("Should throw NullPointerException when calling RcsValue(String[][][] value) with null value");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsResourceAttributes
	 * @objective Test 'RcsValue()' constructor with RcsResourceAttributes
	 * @target RcsValue(RcsResourceAttributes value)
	 * @test_data None
	 * @pre_condition None
	 * @procedure Perform RcsValue(RcsResourceAttributes value) constructor
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testRcsValueAtrributes_SRC_P() {
		RcsResourceAttributes value = new RcsResourceAttributes();

		try {
			assertTrue(new RcsValue(value) instanceof Object);
		} catch (Exception e) {
			fail("Exception occurs when creating object reference using RcsValue(RcsResourceAttributes value).");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsResourceAttributes
	 * @objective Test 'RcsValue()' constructor with RcsResourceAttributes array
	 * @target RcsValue(RcsResourceAttributes[] value)
	 * @test_data None
	 * @pre_condition None
	 * @procedure Perform RcsValue(RcsResourceAttributes[] value) constructor
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testRcsValueAtrrsArray_SRC_P() {
		RcsResourceAttributes[] value = new RcsResourceAttributes[MIN_ARRAY_SIZE];

		try {
			assertTrue(new RcsValue(value) instanceof Object);
		} catch (Exception e) {
			fail("Exception occurs when creating object reference using RcsValue(RcsResourceAttributes[] value).");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsResourceAttributes
	 * @objective Test 'RcsValue()' constructor with NULL RcsResourceAttributes
	 *            array
	 * @target RcsValue(RcsResourceAttributes[] value)
	 * @test_data value = null
	 * @pre_condition None
	 * @procedure Perform RcsValue(RcsResourceAttributes[] value) constructor
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testRcsValueAtrrsArray_NV_ETC_N() {
		RcsResourceAttributes[] value = null;

		try {
			assertEquals(new RcsValue(value), value);
		} catch (NullPointerException e) {
		} catch (Exception e) {
			fail("Should throw NullPointerException when calling RcsValue(RcsResourceAttributes[] value) with null value");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsResourceAttributes
	 * @objective Test 'RcsValue()' constructor with RcsResourceAttributes 2D
	 *            array
	 * @target RcsValue(RcsResourceAttributes[][] value)
	 * @test_data None
	 * @pre_condition None
	 * @procedure Perform RcsValue(RcsResourceAttributes[][] value) constructor
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testRcsValueAtrrsArray2D_SRC_P() {
		RcsResourceAttributes[][] value = new RcsResourceAttributes[MIN_ARRAY_SIZE][MIN_ARRAY_SIZE];

		try {
			assertTrue(new RcsValue(value) instanceof Object);
		} catch (Exception e) {
			fail("Exception occurs when creating object reference using RcsValue(RcsResourceAttributes[][] value).");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsResourceAttributes
	 * @objective Test 'RcsValue()' constructor with NULL RcsResourceAttributes
	 *            2D array
	 * @target RcsValue(RcsResourceAttributes[][] value)
	 * @test_data value = null
	 * @pre_condition None
	 * @procedure Perform RcsValue(RcsResourceAttributes[][] value) constructor
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testRcsValueAtrrsArray2D_NV_ETC_N() {
		RcsResourceAttributes[][] value = null;

		try {
			assertEquals(new RcsValue(value), value);
		} catch (NullPointerException e) {
		} catch (Exception e) {
			fail("Should throw NullPointerException when calling RcsValue(RcsResourceAttributes[][] value) with null value");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsResourceAttributes
	 * @objective Test 'RcsValue()' constructor with RcsResourceAttributes 3D
	 *            array
	 * @target RcsValue(RcsResourceAttributes[][][] value)
	 * @test_data None
	 * @pre_condition None
	 * @procedure Perform RcsValue(RcsResourceAttributes[][][] value)
	 *            constructor
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testRcsValueAtrrsArray3D_SRC_P() {
		RcsResourceAttributes[][][] value = new RcsResourceAttributes[MIN_ARRAY_SIZE][MIN_ARRAY_SIZE][MIN_ARRAY_SIZE];

		try {
			assertTrue(new RcsValue(value) instanceof Object);
		} catch (Exception e) {
			fail("Exception occurs when creating object reference using RcsValue(RcsResourceAttributes[][][] value).");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsResourceAttributes
	 * @objective Test 'RcsValue()' constructor with NULL RcsResourceAttributes
	 *            3D array
	 * @target RcsValue(RcsResourceAttributes[][][] value)
	 * @test_data value = null
	 * @pre_condition None
	 * @procedure Perform RcsValue(RcsResourceAttributes[][][] value)
	 *            constructor
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testRcsValueAtrrsArray3D_NV_ETC_N() {
		RcsResourceAttributes[][][] value = null;

		try {
			assertEquals(new RcsValue(value), value);
		} catch (NullPointerException e) {
		} catch (Exception e) {
			fail("Should throw NullPointerException when calling RcsValue(RcsResourceAttributes[][][] value) with null value");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asBoolean()' function with boolean
	 * @target boolean asBoolean()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with boolean
	 * @procedure Perform boolean asBoolean() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsBoolean_SRC_SRCC_P() {
		boolean value = true;

		RcsValue rcsValue = new RcsValue(value);

		try {
			boolean retValue = rcsValue.asBoolean();

			assertEquals("Didn't get same value with asBoolean()", value,
					retValue);
		} catch (Exception e) {
			fail("Exception occurs when calling boolean asBoolean() API.");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asBooleanArray()' function with boolean array
	 * @target boolean[] asBooleanArray()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with boolean array
	 * @procedure Perform boolean[] asBooleanArray() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsBooleanArray_SRC_P() {
		boolean[] value = new boolean[MIN_ARRAY_SIZE];

		RcsValue rcsValue = new RcsValue(value);

		try {
			assertEquals(rcsValue.asBooleanArray(), value);
		} catch (Exception e) {
			fail("Exception occurs when calling boolean asBooleanArray() API.");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asBooleanArray()' function with int array instead of
	 *            boolean array
	 * @target boolean[] asBooleanArray()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with boolean array
	 * @procedure Perform boolean[] asBooleanArray() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsBooleanArray_ETC_N() {
		int[] value = new int[MIN_ARRAY_SIZE];

		RcsValue rcsValue = new RcsValue(value);

		try {
			assertEquals(rcsValue.asBooleanArray(), value);
			fail("Successfully called asBooleanArray() API where the RcsValue is integer array.");
		} catch (Exception e) {
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asBoolean2DArray()' function with boolean 2D array
	 * @target boolean[][] asBoolean2DArray()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with boolean array
	 * @procedure Perform boolean[][] asBoolean2DArray() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsBooleanArray2D_SRC_P() {
		boolean[][] value = new boolean[MIN_ARRAY_SIZE][MIN_ARRAY_SIZE];

		RcsValue rcsValue = new RcsValue(value);

		try {
			assertEquals(rcsValue.asBoolean2DArray(), value);
		} catch (Exception e) {
			fail("Exception occurs when calling boolean asBoolean2DArray() API.");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asBoolean2DArray()' function with int 2D array instead
	 *            of boolean 2D array
	 * @target boolean[][] asBoolean2DArray()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with boolean array
	 * @procedure Perform boolean[][] asBoolean2DArray() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsBooleanArray2D_ETC_N() {
		int[][] value = new int[MIN_ARRAY_SIZE][MIN_ARRAY_SIZE];

		RcsValue rcsValue = new RcsValue(value);

		try {
			assertEquals(rcsValue.asBoolean2DArray(), value);
			fail("Successfully called asBoolean2DArray() API where the RcsValue is integer array.");
		} catch (Exception e) {
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asBoolean3DArray()' function with boolean 3D array
	 * @target boolean[][][] asBoolean3DArray()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with boolean array
	 * @procedure Perform boolean[][][] asBoolean3DArray() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsBooleanArray3D_SRC_P() {
		boolean[][][] value = new boolean[MIN_ARRAY_SIZE][MIN_ARRAY_SIZE][MIN_ARRAY_SIZE];

		RcsValue rcsValue = new RcsValue(value);

		try {
			assertEquals(rcsValue.asBoolean3DArray(), value);
		} catch (Exception e) {
			fail("Exception occurs when calling boolean asBoolean3DArray() API.");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asBoolean3DArray()' function with int 3D array instead
	 *            of boolean 3D array
	 * @target boolean[][][] asBoolean3DArray()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with boolean array
	 * @procedure Perform boolean[][][] asBoolean3DArray() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsBooleanArray3D_ETC_N() {
		int[][][] value = new int[MIN_ARRAY_SIZE][MIN_ARRAY_SIZE][MIN_ARRAY_SIZE];

		RcsValue rcsValue = new RcsValue(value);

		try {
			assertEquals(rcsValue.asBoolean3DArray(), value);
			fail("Successfully called asBoolean3DArray() API where the RcsValue is integer array.");
		} catch (Exception e) {
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asBoolean()' function with boolean false
	 * @target boolean asBoolean()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with integer
	 * @procedure Perform boolean asBoolean() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsBoolean_UFSV_N() {
		boolean defaultValue = false;

		try {
			boolean retValue = m_RcsValueInt.asBoolean();

			assertEquals(
					"Didn't get default value with asBoolean() when calling int as initialize",
					defaultValue, retValue);
		} catch (Exception e) {
			fail("Exception occurs when calling boolean asBoolean() API.");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asString()' function with string with null
	 * @target String asString(String defaultValue)
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with integer value
	 * @procedure Perform String asString(String defaultValue) API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsStringDefault_SRC_SRCC_P() {
		String defaultValue = null;

		try {
			String retValue = m_RcsValueInt.asString(defaultValue);

			assertEquals(
					"Didn't get same value with asString(String defaultValue)",
					defaultValue, retValue);
		} catch (Exception e) {
			fail("Exception occurs when calling boolean asString(String defaultValue) API.");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asBoolean()' function with fixed integer value
	 * @target boolean asBoolean()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with boolean
	 * @procedure Perform boolean asBoolean() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsInt_SRC_SRCC_P() {
		int value = 5;

		RcsValue rcsValue = new RcsValue(value);

		try {
			int retValue = rcsValue.asInt();

			assertEquals("Didn't get same value with asInt()", value, retValue);
		} catch (Exception e) {
			fail("Exception occurs when calling boolean asInt() API.");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asIntArray()' function with RcsValue that have int array
	 * @target int[] asIntArray()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with boolean array
	 * @procedure Perform int[] asIntArray() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsIntArray_SRC_P() {
		int[] value = new int[MIN_ARRAY_SIZE];

		RcsValue rcsValue = new RcsValue(value);

		try {
			assertEquals(rcsValue.asIntArray(), value);
		} catch (Exception e) {
			fail("Exception occurs when calling boolean asIntArray() API.");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asIntArray()' function with RcsValue that have boolean
	 *            array
	 * @target int[] asIntArray()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with boolean array
	 * @procedure Perform int[] asIntArray() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsIntArray_ETC_N() {
		boolean[] value = new boolean[MIN_ARRAY_SIZE];

		RcsValue rcsValue = new RcsValue(value);

		try {
			assertEquals(rcsValue.asIntArray(), value);
			fail("Successfully called asIntArray() API where the RcsValue is integer array.");
		} catch (Exception e) {
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asInt2DArray()' function with RcsValue that have int 2D
	 *            array
	 * @target int[][] asInt2DArray()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with boolean array
	 * @procedure Perform int[][] asInt2DArray() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsIntArray2D_SRC_P() {
		int[][] value = new int[MIN_ARRAY_SIZE][MIN_ARRAY_SIZE];

		RcsValue rcsValue = new RcsValue(value);

		try {
			assertEquals(rcsValue.asInt2DArray(), value);
		} catch (Exception e) {
			fail("Exception occurs when calling boolean asInt2DArray() API.");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asInt2DArray()' function with RcsValue that have boolean
	 *            2D array
	 * @target int[][] asInt2DArray()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with boolean array
	 * @procedure Perform int[][] asInt2DArray() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsIntArray2D_ETC_N() {
		boolean[][] value = new boolean[MIN_ARRAY_SIZE][MIN_ARRAY_SIZE];

		RcsValue rcsValue = new RcsValue(value);

		try {
			assertEquals(rcsValue.asInt2DArray(), value);
			fail("Successfully called asInt2DArray() API where the RcsValue is boolean array.");
		} catch (Exception e) {
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asInt3DArray()' function with RcsValue that have int 3D
	 *            array
	 * @target int[][][] asInt3DArray()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with boolean array
	 * @procedure Perform int[][][] asInt3DArray() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsIntArray3D_SRC_P() {
		int[][][] value = new int[MIN_ARRAY_SIZE][MIN_ARRAY_SIZE][MIN_ARRAY_SIZE];

		RcsValue rcsValue = new RcsValue(value);

		try {
			assertEquals(rcsValue.asInt3DArray(), value);
		} catch (Exception e) {
			fail("Exception occurs when calling boolean asInt3DArray() API.");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asInt3DArray()' function with RcsValue that have boolean
	 *            3D array
	 * @target int[][][] asInt3DArray()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with boolean array
	 * @procedure Perform int[][][] asInt3DArray() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsIntArray3D_ETC_N() {
		boolean[][][] value = new boolean[MIN_ARRAY_SIZE][MIN_ARRAY_SIZE][MIN_ARRAY_SIZE];

		RcsValue rcsValue = new RcsValue(value);

		try {
			assertEquals(rcsValue.asInt3DArray(), value);
			fail("Successfully called asInt3DArray() API where the RcsValue is boolean array.");
		} catch (Exception e) {
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asInt()' function with Zero value
	 * @target int asInt()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with integer
	 * @procedure Perform int asInt() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsInt_UFSV_N() {
		int defaultValue = 0;

		try {
			int retValue = m_RcsValueBool.asInt();

			assertEquals(
					"Didn't get default value with asInt() when calling int as initialize",
					defaultValue, retValue);
		} catch (Exception e) {
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asInt()' function with pass Zero value
	 * @target int asInt(int defaultValue)
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with integer value
	 * @procedure Perform int asInt(int defaultValue) API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsIntDefault_SRC_SRCC_P() {
		int defaultValue = 0;

		try {
			int retValue = m_RcsValueBool.asInt(defaultValue);

			assertEquals("Didn't get same value with asInt(int defaultValue)",
					defaultValue, retValue);
		} catch (Exception e) {
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asDouble()' function with double fixed value
	 * @target double asDouble()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with boolean
	 * @procedure Perform asDouble() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsDouble_SRC_SRCC_P() {
		double value = 5.5;

		RcsValue rcsValue = new RcsValue(value);

		try {
			double retValue = rcsValue.asDouble();

			assertEquals("Didn't get same value with asDouble()", value,
					retValue);
		} catch (Exception e) {
			fail("Exception occurs when calling asDouble() API.");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asDoubleArray()' function with double array
	 * @target double[] asDoubleArray()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with boolean array
	 * @procedure Perform double[] asDoubleArray() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsDoubleArray_SRC_P() {
		double[] value = new double[MIN_ARRAY_SIZE];

		RcsValue rcsValue = new RcsValue(value);

		try {
			assertEquals(rcsValue.asDoubleArray(), value);
		} catch (Exception e) {
			fail("Exception occurs when calling boolean asDoubleArray() API.");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asDoubleArray()' function with boolean array
	 * @target double[] asDoubleArray()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with boolean array
	 * @procedure Perform double[] asDoubleArray() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsDoubleArray_ETC_N() {
		boolean[] value = new boolean[MIN_ARRAY_SIZE];

		RcsValue rcsValue = new RcsValue(value);

		try {
			assertEquals(rcsValue.asDoubleArray(), value);
			fail("Successfully called asIntArray() API where the RcsValue is boolean array.");
		} catch (Exception e) {
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asDouble2DArray()' function with double 2D array
	 * @target double[][] asDouble2DArray()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with double array
	 * @procedure Perform asDouble2DArray() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsDoubleArray2D_SRC_P() {
		double[][] value = new double[MIN_ARRAY_SIZE][MIN_ARRAY_SIZE];

		RcsValue rcsValue = new RcsValue(value);

		try {
			assertEquals(rcsValue.asDouble2DArray(), value);
		} catch (Exception e) {
			fail("Exception occurs when calling boolean asDouble2DArray() API.");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asDouble2DArray()' function with boolean 2D array
	 * @target double[][] asDouble2DArray()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with boolean array
	 * @procedure Perform asDouble2DArray() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsDoubleArray2D_ETC_N() {
		boolean[][] value = new boolean[MIN_ARRAY_SIZE][MIN_ARRAY_SIZE];

		RcsValue rcsValue = new RcsValue(value);

		try {
			assertEquals(rcsValue.asInt2DArray(), value);
			fail("Successfully called asDouble2DArray() API where the RcsValue is boolean array.");
		} catch (Exception e) {
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asDouble3DArray()' function with double 3D array
	 * @target double[][][] asDouble3DArray()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with boolean array
	 * @procedure Perform asDouble3DArray() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsDoubleArray3D_SRC_P() {
		double[][][] value = new double[MIN_ARRAY_SIZE][MIN_ARRAY_SIZE][MIN_ARRAY_SIZE];

		RcsValue rcsValue = new RcsValue(value);

		try {
			assertEquals(rcsValue.asDouble3DArray(), value);
		} catch (Exception e) {
			fail("Exception occurs when calling asDouble3DArray() API.");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asDouble3DArray()' function with boolean 3D array
	 * @target double[][][] asDouble3DArray()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with boolean array
	 * @procedure Perform asDouble3DArray() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsDoubleArray3D_ETC_N() {
		boolean[][][] value = new boolean[MIN_ARRAY_SIZE][MIN_ARRAY_SIZE][MIN_ARRAY_SIZE];

		RcsValue rcsValue = new RcsValue(value);

		try {
			assertEquals(rcsValue.asDouble3DArray(), value);
			fail("Successfully called asDouble3DArray() API where the RcsValue is boolean array.");
		} catch (Exception e) {
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asDouble()' function with Zero value
	 * @target double asDouble()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with integer
	 * @procedure Perform int asInt() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsDouble_UFSV_N() {
		double defaultValue = 0;

		try {
			double retValue = m_RcsValueBool.asDouble();

			assertEquals(
					"Didn't get default value with asDouble() when calling boolean as initialize",
					defaultValue, retValue);
		} catch (Exception e) {
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asInt()' function with pass Zero value
	 * @target int asInt(int defaultValue)
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with integer value
	 * @procedure Perform int asInt(int defaultValue) API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsDoubleDefault_SRC_SRCC_P() {
		double defaultValue = 0;

		try {
			double retValue = m_RcsValueBool.asDouble(defaultValue);

			assertEquals(
					"Didn't get same value with asDouble(double defaultValue)",
					defaultValue, retValue);
		} catch (Exception e) {
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asString()' function with fixed String
	 * @target String asString()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with boolean
	 * @procedure Perform boolean asString() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsString_SRC_SRCC_P() {
		String value = "test";

		RcsValue rcsValue = new RcsValue(value);

		try {
			String retValue = rcsValue.asString();

			assertEquals("Didn't get same value with asBoolean()", value,
					retValue);
		} catch (Exception e) {
			fail("Exception occurs when calling boolean asBoolean() API.");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asStringArray()' function with String array
	 * @target String[] asStringArray()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with boolean array
	 * @procedure Perform String[] asStringArray() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsStringArray_SRC_P() {
		String[] value = new String[MIN_ARRAY_SIZE];

		RcsValue rcsValue = new RcsValue(value);

		try {
			assertEquals(rcsValue.asStringArray(), value);
		} catch (Exception e) {
			fail("Exception occurs when calling asStringArray() API.");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asStringArray()' function with int array instead of
	 *            String array
	 * @target String[] asStringArray()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with boolean array
	 * @procedure Perform String[] asStringArray() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsStringArray_ETC_N() {
		int[] value = new int[MIN_ARRAY_SIZE];

		RcsValue rcsValue = new RcsValue(value);

		try {
			assertEquals(rcsValue.asStringArray(), value);
			fail("Successfully called asStringArray() API where the RcsValue is integer array.");
		} catch (Exception e) {
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asString2DArray()' function String 2D array
	 * @target String[][] asString2DArray()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with boolean array
	 * @procedure Perform String[][] asString2DArray() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsStringArray2D_SRC_P() {
		String[][] value = new String[MIN_ARRAY_SIZE][MIN_ARRAY_SIZE];

		RcsValue rcsValue = new RcsValue(value);

		try {
			assertEquals(rcsValue.asString2DArray(), value);
		} catch (Exception e) {
			fail("Exception occurs when calling asString2DArray() API.");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asString2DArray()' function with int 2D array instead of
	 *            String 2Darray
	 * @target String[][] asString2DArray()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with boolean array
	 * @procedure Perform String[][] asString2DArray() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsStringArray2D_ETC_N() {
		int[][] value = new int[MIN_ARRAY_SIZE][MIN_ARRAY_SIZE];

		RcsValue rcsValue = new RcsValue(value);

		try {
			assertEquals(rcsValue.asString2DArray(), value);
			fail("Successfully called asString2DArray() API where the RcsValue is integer array.");
		} catch (Exception e) {
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asString3DArray()' function with String 3D array
	 * @target String[][][] asString3DArray()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with boolean array
	 * @procedure Perform String[][][] asString3DArray() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsStringArray3D_SRC_P() {
		String[][][] value = new String[MIN_ARRAY_SIZE][MIN_ARRAY_SIZE][MIN_ARRAY_SIZE];

		RcsValue rcsValue = new RcsValue(value);

		try {
			assertEquals(rcsValue.asString3DArray(), value);
		} catch (Exception e) {
			fail("Exception occurs when calling asString3DArray() API.");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asString3DArray()' function with int 3D array instead of
	 *            String 3D array
	 * @target boolean[][][] asString3DArray()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with boolean array
	 * @procedure Perform String[][][] asString3DArray() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsStringArray3D_ETC_N() {
		int[][][] value = new int[MIN_ARRAY_SIZE][MIN_ARRAY_SIZE][MIN_ARRAY_SIZE];

		RcsValue rcsValue = new RcsValue(value);

		try {
			assertEquals(rcsValue.asString3DArray(), value);
			fail("Successfully called asString3DArray() API where the RcsValue is integer array.");
		} catch (Exception e) {
		}
	}

	/**
	 * @since 2016-03-12
	 * @see None
	 * @objective Test 'asString()' function with NULL value
	 * @target String asString()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with integer
	 * @procedure Perform String asString() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsString_UFSV_N() {
		String defaultValue = null;

		try {
			String retValue = m_RcsValueInt.asString();

			assertEquals(
					"Didn't get default value with asString() when calling int as initialize",
					defaultValue, retValue);
		} catch (Exception e) {
			fail("Exception occurs when calling String asString() API.");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asBoolean()' function with boolean value
	 * @target boolean asBoolean(boolean defaultValue)
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with integer value
	 * @procedure Perform boolean asBoolean(boolean defaultValue) API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsBooleanDefault_SRC_SRCC_P() {
		boolean defaultValue = true;

		try {
			boolean retValue = m_RcsValueInt.asBoolean(defaultValue);

			assertEquals(
					"Didn't get same value with asBoolean(boolean defaultValue)",
					defaultValue, retValue);
		} catch (Exception e) {
			fail("Exception occurs when calling boolean asBoolean(boolean defaultValue) API.");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asAttributes()' function with Rcs Resource attributes
	 * @target RcsResourceAttributes asAttributes()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with boolean
	 * @procedure Perform RcsResourceAttributes asAttributes() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsAttributes_SRC_P() {
		RcsResourceAttributes rcsAttrs = new RcsResourceAttributes();
		RcsValue rcsValue = new RcsValue(rcsAttrs);

		try {
			rcsValue.asAttributes();
		} catch (Exception e) {
			fail("Exception occurs when calling asAttributes() API.");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asAttributes()' function without Rcs Resource attributes
	 * @target RcsResourceAttributes asAttributes()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with integer
	 * @procedure Perform RcsResourceAttributes asAttributes() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsAttributes_ETC_N() {
		try {
			m_RcsValueInt.asAttributes();
			fail("Successfully called asAttributes() where the RcsValue is initialize as int");
		} catch (Exception e) {
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asAttributes()' function with RcsResource attributes
	 *            array
	 * @target RcsResourceAttributes asAttributes()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with boolean
	 * @procedure Perform RcsResourceAttributes asAttributes() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsAttributesArray_SRC_P() {
		RcsResourceAttributes[] rcsAttrs = new RcsResourceAttributes[MIN_ARRAY_SIZE];
		RcsValue rcsValue = new RcsValue(rcsAttrs);

		try {
			rcsValue.asAttributesArray();
		} catch (Exception e) {
			fail("Exception occurs when calling asAttributesArray() API.");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asAttributes()' function with int array instead of
	 *            RcsResource attributes array
	 * @target RcsResourceAttributes asAttributes()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with integer
	 * @procedure Perform RcsResourceAttributes asAttributes() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsAttributesArray_ETC_N() {
		int[] value = new int[MIN_ARRAY_SIZE];
		RcsValue rcsValue = new RcsValue(value);

		try {
			rcsValue.asAttributesArray();

			fail("Successfully called asAttributes() where the RcsValue is initialize as int array");
		} catch (Exception e) {
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asAttributes2DArray()' function with RcsResource
	 *            attributes 2D array
	 * @target RcsResourceAttributes[] asAttributes2DArray()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with boolean
	 * @procedure Perform asAttributes2DArray() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsAttributesArray2D_SRC_P() {
		RcsResourceAttributes[][] rcsAttrs = new RcsResourceAttributes[MIN_ARRAY_SIZE][MIN_ARRAY_SIZE];
		RcsValue rcsValue = new RcsValue(rcsAttrs);

		try {
			rcsValue.asAttributes2DArray();
		} catch (Exception e) {
			fail("Exception occurs when calling asAttributes2DArray() API.");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asAttributes()' function with int 2D array instead of
	 *            RcsResource attributes 2D array
	 * @target RcsResourceAttributes asAttributes()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with integer
	 * @procedure Perform RcsResourceAttributes asAttributes() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsAttributesArray2D_ETC_N() {
		int[][] value = new int[MIN_ARRAY_SIZE][MIN_ARRAY_SIZE];
		RcsValue rcsValue = new RcsValue(value);

		try {
			rcsValue.asAttributes2DArray();

			fail("Successfully called asAttributes2DArray() where the RcsValue is initialize as int array");
		} catch (Exception e) {
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asAttributes3DArray()' function with RcsResource
	 *            attributes 3D array
	 * @target RcsResourceAttributes[][][] asAttributes3DArray()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with boolean
	 * @procedure Perform RcsResourceAttributes[][][] asAttributes3DArray() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsAttributesArray3D_SRC_P() {
		RcsResourceAttributes[][][] rcsAttrs = new RcsResourceAttributes[MIN_ARRAY_SIZE][MIN_ARRAY_SIZE][MIN_ARRAY_SIZE];
		RcsValue rcsValue = new RcsValue(rcsAttrs);

		try {
			rcsValue.asAttributes3DArray();
		} catch (Exception e) {
			fail("Exception occurs when calling asAttributes3DArray() API.");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'asAttributes3DArray()' function with int 3D array
	 *            instead of RcsResource attributes 2D array
	 * @target RcsResourceAttributes[][][] asAttributes3DArray()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class with integer
	 * @procedure Perform RcsResourceAttributes[][][] asAttributes3DArray() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testAsAttributesArray3D_ETC_N() {
		int[] value = new int[MIN_ARRAY_SIZE];
		RcsValue rcsValue = new RcsValue(value);

		try {
			rcsValue.asAttributes3DArray();

			fail("Successfully called asAttributes3DArray() where the RcsValue is initialize as int array");
		} catch (Exception e) {
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'getType()' function with RcsValue
	 * @target RcsValue.Type getType()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class
	 * @procedure Perform getType() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testRcsValueType_SRC_P() {
		RcsValue rcsValue = new RcsValue();

		try {
			rcsValue.getType();
		} catch (Exception e) {
			fail("Exception occurs when calling getType() API.");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'isNullObject()' function with NULL object
	 * @target boolean isNullObject()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class
	 * @procedure Perform isNullObject() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testIsNullObject_SRC_P() {
		Object object = new Object();

		try {
			RcsValue.isNullObject(object);
		} catch (Exception e) {
			fail("Exception occurs when calling isNullObject() API.");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'getId()' function with RcsValue Type
	 * @target RcsValue.TypeId getId()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class
	 * @procedure Perform getId() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testTypeGetId_SRC_P() {
		RcsValue rcsValue = new RcsValue();
		RcsValue.Type rcsValueType = rcsValue.getType();

		try {
			rcsValueType.getId();
		} catch (Exception e) {
			fail("Exception occurs when calling getId() API.");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'getBaseTypeId()' function with RcsValue
	 * @target RcsValue.TypeId getBaseTypeId(Type t)
	 * @test_data None
	 * @pre_condition Initialize RcsValue class
	 * @procedure Perform getBaseTypeId() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testTypeGetBaseTypeId_SRC_P() {
		RcsValue rcsValue = new RcsValue();
		RcsValue.Type rcsValueType = rcsValue.getType();

		try {
			RcsValue.Type.getBaseTypeId(rcsValueType);
		} catch (Exception e) {
			fail("Exception occurs when calling getBaseTypeId() API.");
		}
	}

	/**
	 * @since 2016-03-12
	 * @see RcsValue
	 * @objective Test 'getId()' function with RcsValue
	 * @target RcsValue.TypeId getId()
	 * @test_data None
	 * @pre_condition Initialize RcsValue class
	 * @procedure Perform getId() API
	 * @post_condition None
	 * @expected No crash occurs
	 **/
	public void testTypeGetDepth_SRC_P() {
		RcsValue rcsValue = new RcsValue();
		RcsValue.Type rcsValueType = rcsValue.getType();

		try {
			RcsValue.Type.getDepth(rcsValueType);
		} catch (Exception e) {
			fail("Exception occurs when calling getDepth() API.");
		}
	}
}
