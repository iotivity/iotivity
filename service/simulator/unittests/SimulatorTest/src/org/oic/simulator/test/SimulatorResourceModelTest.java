/*
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.oic.simulator.test;

import java.util.Map;

import org.oic.simulator.AttributeValue;
import org.oic.simulator.InvalidArgsException;
import org.oic.simulator.SimulatorResourceAttribute;
import org.oic.simulator.SimulatorResourceModel;

import junit.framework.TestCase;

/**
 * This class tests the APIs of SimulatorResourceModel class.
 */
public class SimulatorResourceModelTest extends TestCase {
    private static final String INT_KEY    = "Interger";
    private static final String DOUBLE_KEY = "Double";
    private static final String BOOL_KEY   = "Boolean";
    private static final String STRING_KEY = "String";

    @Override
    protected void setUp() throws Exception {
        super.setUp();
    }

    @Override
    protected void tearDown() throws Exception {
        super.tearDown();
    }

    public void testSimulatorResourceModel_P01() {
        SimulatorResourceModel resModel = new SimulatorResourceModel();
        assertNotNull(resModel);
    }

    public void testAddAttributeInt_P01() {
        int result = -1;

        SimulatorResourceModel resModel = new SimulatorResourceModel();
        SimulatorResourceAttribute attribute = new SimulatorResourceAttribute(
                INT_KEY, new AttributeValue(10), null);
        try {
            resModel.addAttribute(attribute);
            result = ((Integer) resModel.getAttribute(INT_KEY).value().get())
                    .intValue();
        } catch (InvalidArgsException e) {
            e.printStackTrace();
        }

        assertEquals(10, result);
    }

    public void testAddAttributeInt_P02() {
        int result = -1;

        SimulatorResourceModel resModel = new SimulatorResourceModel();
        SimulatorResourceAttribute attribute = new SimulatorResourceAttribute(
                INT_KEY, new AttributeValue(-10), null);
        try {
            resModel.addAttribute(attribute);
            result = ((Integer) resModel.getAttribute(INT_KEY).value().get())
                    .intValue();
        } catch (InvalidArgsException e) {
            e.printStackTrace();
        }

        assertEquals(-10, result);
    }

    public void testAddAttributeDouble_P01() {
        double result = 0.0;

        SimulatorResourceModel resModel = new SimulatorResourceModel();
        SimulatorResourceAttribute attribute = new SimulatorResourceAttribute(
                DOUBLE_KEY, new AttributeValue(4.0), null);
        try {
            resModel.addAttribute(attribute);
            result = ((Double) resModel.getAttribute(DOUBLE_KEY).value().get())
                    .doubleValue();
        } catch (InvalidArgsException e) {
            e.printStackTrace();
        }

        assertEquals(4.0, result);
    }

    public void testAddAttributeDouble_P02() {
        double result = 0.0;

        SimulatorResourceModel resModel = new SimulatorResourceModel();
        SimulatorResourceAttribute attribute = new SimulatorResourceAttribute(
                DOUBLE_KEY, new AttributeValue(-4.0), null);
        try {
            resModel.addAttribute(attribute);
            result = ((Double) resModel.getAttribute(DOUBLE_KEY).value().get())
                    .doubleValue();
        } catch (InvalidArgsException e) {
            e.printStackTrace();
        }

        assertEquals(-4.0, result);
    }

    public void testAddAttributeBoolean_P01() {
        boolean result = false;

        SimulatorResourceModel resModel = new SimulatorResourceModel();
        SimulatorResourceAttribute attribute = new SimulatorResourceAttribute(
                BOOL_KEY, new AttributeValue(true), null);
        try {
            resModel.addAttribute(attribute);
            result = ((Boolean) resModel.getAttribute(BOOL_KEY).value().get())
                    .booleanValue();
        } catch (InvalidArgsException e) {
            e.printStackTrace();
        }

        assertEquals(true, result);
    }

    public void testAddAttributeString_P01() {
        String result = null;

        SimulatorResourceModel resModel = new SimulatorResourceModel();
        SimulatorResourceAttribute attribute = new SimulatorResourceAttribute(
                STRING_KEY, new AttributeValue("string-value"), null);
        try {
            resModel.addAttribute(attribute);
            result = (String) resModel.getAttribute(STRING_KEY).value().get();
        } catch (InvalidArgsException e) {
            e.printStackTrace();
        }

        assertEquals("string-value", result);
    }

    public void testAddAttributeString_P02() {
        String result = null;

        SimulatorResourceModel resModel = new SimulatorResourceModel();
        SimulatorResourceAttribute attribute = new SimulatorResourceAttribute(
                STRING_KEY, new AttributeValue(""), null);
        try {
            resModel.addAttribute(attribute);
            result = (String) resModel.getAttribute(STRING_KEY).value().get();
        } catch (InvalidArgsException e) {
            e.printStackTrace();
        }

        assertEquals("", result);
    }

    public void testAddAttributeString_P03() {
        String result = null;

        SimulatorResourceModel resModel = new SimulatorResourceModel();
        SimulatorResourceAttribute attribute = new SimulatorResourceAttribute(
                STRING_KEY, new AttributeValue("@#$$&^*^(*^&"), null);
        try {
            resModel.addAttribute(attribute);
            result = (String) resModel.getAttribute(STRING_KEY).value().get();
        } catch (InvalidArgsException e) {
            e.printStackTrace();
        }

        assertEquals("@#$$&^*^(*^&", result);
    }

    public void testSize_P01() {
        int result = -1;

        SimulatorResourceModel resModel = new SimulatorResourceModel();
        result = resModel.size();

        assertEquals(0, result);
    }

    public void testSize_P02() {
        int result = -1;

        SimulatorResourceModel resModel = new SimulatorResourceModel();
        try {
            resModel.addAttribute(new SimulatorResourceAttribute(INT_KEY,
                    new AttributeValue(1234), null));
            resModel.addAttribute(new SimulatorResourceAttribute(DOUBLE_KEY,
                    new AttributeValue(1.234), null));
            resModel.addAttribute(new SimulatorResourceAttribute(BOOL_KEY,
                    new AttributeValue(true), null));
            resModel.addAttribute(new SimulatorResourceAttribute(STRING_KEY,
                    new AttributeValue("string-value"), null));
            result = resModel.size();
        } catch (InvalidArgsException e) {
            e.printStackTrace();
        }

        assertEquals(4, result);
    }

    public void testGetAttribute_P01() {
        SimulatorResourceAttribute result = null;

        SimulatorResourceModel resModel = new SimulatorResourceModel();
        try {
            resModel.addAttribute(new SimulatorResourceAttribute(INT_KEY,
                    new AttributeValue(10), null));
            result = resModel.getAttribute(INT_KEY);
        } catch (InvalidArgsException e) {
            e.printStackTrace();
        }

        assertNotNull(result);
    }

    public void testGetAttributes_P01() {
        Map<String, SimulatorResourceAttribute> result = null;

        SimulatorResourceModel resModel = new SimulatorResourceModel();
        try {
            resModel.addAttribute(new SimulatorResourceAttribute(INT_KEY,
                    new AttributeValue(1234), null));
            resModel.addAttribute(new SimulatorResourceAttribute(DOUBLE_KEY,
                    new AttributeValue(1.234), null));
            resModel.addAttribute(new SimulatorResourceAttribute(BOOL_KEY,
                    new AttributeValue(true), null));
            resModel.addAttribute(new SimulatorResourceAttribute(STRING_KEY,
                    new AttributeValue("string-value"), null));
            result = resModel.getAttributes();
        } catch (InvalidArgsException e) {
            e.printStackTrace();
        }

        assertNotNull(result);
        assertEquals(4, result.size());
    }
}
