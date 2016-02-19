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

package org.oic.simulator.server.test;

import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;

import org.oic.simulator.AttributeProperty;
import org.oic.simulator.AttributeValue;
import org.oic.simulator.InvalidArgsException;
import org.oic.simulator.SimulatorException;
import org.oic.simulator.SimulatorManager;
import org.oic.simulator.SimulatorResourceAttribute;
import org.oic.simulator.SimulatorResourceModel;
import org.oic.simulator.server.SimulatorResource;
import org.oic.simulator.server.SimulatorResource.AutoUpdateListener;
import org.oic.simulator.server.SimulatorSingleResource;
import org.oic.simulator.test.ExceptionType;
import org.oic.simulator.utils.ObjectHolder;

import junit.framework.TestCase;

public class SimulatorSingleResourceTest extends TestCase {
    static {
        System.loadLibrary("SimulatorManager");
        System.loadLibrary("RamlParser");
        System.loadLibrary("oc");
        System.loadLibrary("oc_logger");
        System.loadLibrary("octbstack");
    }

    private static final String     SINGLE_RES_RAML = "./ramls/oic.r.light.raml";
    private static final String     INT_KEY         = "Interger";
    private static final String     DOUBLE_KEY      = "Double";
    private static final String     BOOL_KEY        = "Boolean";
    private static final String     STRING_KEY      = "String";
    private SimulatorSingleResource singleResource  = null;

    protected void setUp() throws Exception {
        super.setUp();
        singleResource = (SimulatorSingleResource) SimulatorManager
                .createResource(SimulatorResource.Type.SINGLE, "test-resource",
                        "/test/resource", "test.resource");
    }

    protected void tearDown() throws Exception {
        super.tearDown();
        singleResource = null;
    }

    SimulatorSingleResource createResourceFromRAML() {
        try {
            return (SimulatorSingleResource) SimulatorManager
                    .createResource(SINGLE_RES_RAML);
        } catch (InvalidArgsException e) {
            e.printStackTrace();
        } catch (SimulatorException e) {
            e.printStackTrace();
        }

        return null;
    }

    public void testGetResourceModel_P01() {
        SimulatorResourceModel resModel = null;

        try {
            SimulatorSingleResource resource = createResourceFromRAML();
            resModel = resource.getResourceModel();
        } catch (SimulatorException e) {
            e.printStackTrace();
        }

        assertNotNull(resModel);
        assertTrue(resModel.size() > 0);
    }

    public void testAddAttributeInteger_P01() {
        int result = -1;

        try {
            SimulatorResourceAttribute attribute = new SimulatorResourceAttribute(
                    INT_KEY, new AttributeValue(2), null);
            singleResource.addAttribute(attribute);
            result = ((Integer) singleResource.getAttribute(INT_KEY).value()
                    .get()).intValue();
        } catch (InvalidArgsException e) {
            e.printStackTrace();
        } catch (SimulatorException e) {
            e.printStackTrace();
        }

        assertEquals(2, result);
    }

    public void testAddAttributeDouble_P01() {
        double result = 0.0;

        try {
            SimulatorResourceAttribute attribute = new SimulatorResourceAttribute(
                    DOUBLE_KEY, new AttributeValue(4.0), null);
            singleResource.addAttribute(attribute);
            result = ((Double) singleResource.getAttribute(DOUBLE_KEY).value()
                    .get()).doubleValue();
        } catch (InvalidArgsException e) {
            e.printStackTrace();
        } catch (SimulatorException e) {
            e.printStackTrace();
        }

        assertEquals(4.0, result);
    }

    public void testAddAttributeBoolean_P01() {
        boolean result = false;

        try {
            SimulatorResourceAttribute attribute = new SimulatorResourceAttribute(
                    BOOL_KEY, new AttributeValue(true), null);
            singleResource.addAttribute(attribute);
            result = ((Boolean) singleResource.getAttribute(BOOL_KEY).value()
                    .get()).booleanValue();
        } catch (InvalidArgsException e) {
            e.printStackTrace();
        } catch (SimulatorException e) {
            e.printStackTrace();
        }

        assertEquals(true, result);
    }

    public void testaddAttributeString_P01() {
        String result = null;

        try {
            SimulatorResourceAttribute attribute = new SimulatorResourceAttribute(
                    STRING_KEY, new AttributeValue("string-value"), null);
            singleResource.addAttribute(attribute);
            result = (String) singleResource.getAttribute(STRING_KEY).value()
                    .get();
        } catch (InvalidArgsException e) {
            e.printStackTrace();
        } catch (SimulatorException e) {
            e.printStackTrace();
        }

        assertEquals("string-value", result);
    }

    public void testUpdateAttributeInteger_P01() {
        int result = -1;

        try {
            SimulatorResourceAttribute attribute = new SimulatorResourceAttribute(
                    INT_KEY, new AttributeValue(10), null);
            singleResource.addAttribute(attribute);
            singleResource.updateAttribute(INT_KEY, new AttributeValue(12));
            result = ((Integer) singleResource.getAttribute(INT_KEY).value()
                    .get()).intValue();
        } catch (InvalidArgsException e) {
            e.printStackTrace();
        } catch (SimulatorException e) {
            e.printStackTrace();
        }

        assertEquals(12, result);
    }

    public void testUpdateAttributeDouble_P01() {
        double result = 0.0;

        try {
            SimulatorResourceAttribute attribute = new SimulatorResourceAttribute(
                    DOUBLE_KEY, new AttributeValue(22.0), null);
            singleResource.addAttribute(attribute);
            singleResource
                    .updateAttribute(DOUBLE_KEY, new AttributeValue(25.3));
            result = ((Double) singleResource.getAttribute(DOUBLE_KEY).value()
                    .get()).doubleValue();
        } catch (InvalidArgsException e) {
            e.printStackTrace();
        } catch (SimulatorException e) {
            e.printStackTrace();
        }

        assertEquals(25.3, result);
    }

    public void testUpdateAttributeBoolean_P01() {
        boolean result = true;

        try {
            SimulatorResourceAttribute attribute = new SimulatorResourceAttribute(
                    BOOL_KEY, new AttributeValue(true), null);
            singleResource.addAttribute(attribute);
            singleResource.updateAttribute(BOOL_KEY, new AttributeValue(false));
            result = ((Boolean) singleResource.getAttribute(BOOL_KEY).value()
                    .get()).booleanValue();
        } catch (InvalidArgsException e) {
            e.printStackTrace();
        } catch (SimulatorException e) {
            e.printStackTrace();
        }

        assertEquals(false, result);
    }

    public void testupdateAttributeString_P01() {
        String result = null;

        try {
            SimulatorResourceAttribute attribute = new SimulatorResourceAttribute(
                    STRING_KEY, new AttributeValue("old-value"), null);
            singleResource.addAttribute(attribute);
            singleResource.updateAttribute(STRING_KEY, new AttributeValue(
                    "new-value"));
            result = (String) singleResource.getAttribute(STRING_KEY).value()
                    .get();
        } catch (InvalidArgsException e) {
            e.printStackTrace();
        } catch (SimulatorException e) {
            e.printStackTrace();
        }

        assertEquals("new-value", result);
    }

    public void testSetRange_P01() {
        int result = -1;

        try {
            SimulatorResourceAttribute attribute = new SimulatorResourceAttribute(
                    INT_KEY, new AttributeValue(10), new AttributeProperty(1,
                            12));
            singleResource.addAttribute(attribute);
            singleResource.updateAttribute(INT_KEY, new AttributeValue(3));
            result = ((Integer) singleResource.getAttribute(INT_KEY).value()
                    .get()).intValue();
        } catch (InvalidArgsException e) {
            e.printStackTrace();
        } catch (SimulatorException e) {
            e.printStackTrace();
        }

        assertEquals(3, result);
    }

    public void testSetRange_N01() {
        int result = -1;

        try {
            SimulatorResourceAttribute attribute = new SimulatorResourceAttribute(
                    INT_KEY, new AttributeValue(10), new AttributeProperty(1,
                            12));
            singleResource.addAttribute(attribute);
            singleResource.updateAttribute(INT_KEY, new AttributeValue(13));
            result = ((Integer) singleResource.getAttribute(INT_KEY).value()
                    .get()).intValue();
        } catch (InvalidArgsException e) {
            e.printStackTrace();
        } catch (SimulatorException e) {
            e.printStackTrace();
        }

        assertEquals(10, result);
    }

    public void testSetAllowedValuesInteger_P01() {
        int result = -1;

        try {
            int[] values = { 1, 10, 20, 50 };
            SimulatorResourceAttribute attribute = new SimulatorResourceAttribute(
                    INT_KEY, new AttributeValue(10), new AttributeProperty(
                            values));
            singleResource.addAttribute(attribute);
            singleResource.updateAttribute(INT_KEY, new AttributeValue(20));
            result = ((Integer) singleResource.getAttribute(INT_KEY).value()
                    .get()).intValue();
        } catch (InvalidArgsException e) {
            e.printStackTrace();
        } catch (SimulatorException e) {
            e.printStackTrace();
        }

        assertEquals(20, result);
    }

    public void testSetAllowedValuesInteger_N01() {
        int result = -1;

        try {
            int[] values = { 1, 10, 20, 50 };
            SimulatorResourceAttribute attribute = new SimulatorResourceAttribute(
                    INT_KEY, new AttributeValue(10), new AttributeProperty(
                            values));
            singleResource.addAttribute(attribute);
            singleResource.updateAttribute(INT_KEY, new AttributeValue(15));
            result = ((Integer) singleResource.getAttribute(INT_KEY).value()
                    .get()).intValue();
        } catch (InvalidArgsException e) {
            e.printStackTrace();
        } catch (SimulatorException e) {
            e.printStackTrace();
        }

        assertEquals(10, result);
    }

    public void testSetAllowedValuesDouble_P01() {
        double result = 0.0;

        try {
            double[] values = { 11.5, 10.5, 20.5, 50.5 };
            SimulatorResourceAttribute attribute = new SimulatorResourceAttribute(
                    DOUBLE_KEY, new AttributeValue(11.5),
                    new AttributeProperty(values));
            singleResource.addAttribute(attribute);
            singleResource
                    .updateAttribute(DOUBLE_KEY, new AttributeValue(10.5));
            result = ((Double) singleResource.getAttribute(DOUBLE_KEY).value()
                    .get()).doubleValue();
        } catch (InvalidArgsException e) {
            e.printStackTrace();
        } catch (SimulatorException e) {
            e.printStackTrace();
        }

        assertEquals(10.5, result);
    }

    public void testSetAllowedValuesDouble_N01() {
        double result = 0.0;

        try {
            double[] values = { 11.5, 10.5, 20.5, 50.5 };
            SimulatorResourceAttribute attribute = new SimulatorResourceAttribute(
                    DOUBLE_KEY, new AttributeValue(11.5),
                    new AttributeProperty(values));
            singleResource.addAttribute(attribute);
            singleResource.updateAttribute(DOUBLE_KEY, new AttributeValue(2.2));
            result = ((Double) singleResource.getAttribute(DOUBLE_KEY).value()
                    .get()).doubleValue();
        } catch (InvalidArgsException e) {
            e.printStackTrace();
        } catch (SimulatorException e) {
            e.printStackTrace();
        }

        assertEquals(11.5, result);
    }

    public void testsetAllowedValuesString_P01() {
        String result = null;

        try {
            String[] values = { "monday", "tuesday", "wednesday" };
            SimulatorResourceAttribute attribute = new SimulatorResourceAttribute(
                    STRING_KEY, new AttributeValue("monday"),
                    new AttributeProperty(values));
            singleResource.addAttribute(attribute);
            singleResource.updateAttribute(STRING_KEY, new AttributeValue(
                    "tuesday"));
            result = (String) singleResource.getAttribute(STRING_KEY).value()
                    .get();
        } catch (InvalidArgsException e) {
            e.printStackTrace();
        } catch (SimulatorException e) {
            e.printStackTrace();
        }

        assertEquals("tuesday", result);
    }

    public void testsetAllowedValuesString_N01() {
        String result = null;

        try {
            String[] values = { "monday", "tuesday", "wednesday" };
            SimulatorResourceAttribute attribute = new SimulatorResourceAttribute(
                    STRING_KEY, new AttributeValue("monday"),
                    new AttributeProperty(values));
            singleResource.addAttribute(attribute);
            singleResource.updateAttribute(STRING_KEY, new AttributeValue(
                    "friday"));
            result = (String) singleResource.getAttribute(STRING_KEY).value()
                    .get();
        } catch (InvalidArgsException e) {
            e.printStackTrace();
        } catch (SimulatorException e) {
            e.printStackTrace();
        }

        assertEquals("monday", result);
    }

    public void testRemoveAttribute_P01() {
        SimulatorResourceAttribute result = null;

        try {
            SimulatorResourceAttribute attribute = new SimulatorResourceAttribute(
                    STRING_KEY, new AttributeValue("friday"), null);
            singleResource.addAttribute(attribute);
            singleResource.removeAttribute(STRING_KEY);
            result = singleResource.getAttribute(STRING_KEY);
        } catch (InvalidArgsException e) {
            e.printStackTrace();
        } catch (SimulatorException e) {
            e.printStackTrace();
        }

        assertNull(result);
    }

    public void testRemoveAttribute_N02() {
        ExceptionType exType = ExceptionType.UNKNOWN;

        try {
            SimulatorResourceAttribute attribute = new SimulatorResourceAttribute(
                    STRING_KEY, new AttributeValue("friday"), null);
            singleResource.addAttribute(attribute);
            singleResource.removeAttribute(null);
        } catch (InvalidArgsException e) {
            exType = ExceptionType.INVALID_ARGS;
        } catch (SimulatorException e) {
            exType = ExceptionType.SIMULATOR;
        }

        assertEquals(ExceptionType.INVALID_ARGS, exType);
    }

    public void testRemoveAttribute_N03() {
        ExceptionType exType = ExceptionType.UNKNOWN;

        try {
            singleResource.removeAttribute("");
        } catch (InvalidArgsException e) {
            exType = ExceptionType.INVALID_ARGS;
        } catch (SimulatorException e) {
            exType = ExceptionType.SIMULATOR;
        }

        assertEquals(ExceptionType.INVALID_ARGS, exType);
    }

    public void testStartResourceAutomation_P01() {
        SimulatorSingleResource resource = createResourceFromRAML();
        if (null == resource)
            return;

        CountDownLatch lockObject = new CountDownLatch(1);
        ObjectHolder<AutoUpdateInfo> autoUpdateHolder = new ObjectHolder<>();
        AutoUpdateCompleteListener automationListener = new AutoUpdateCompleteListener(
                lockObject, autoUpdateHolder);
        int id = -1;

        try {
            resource.start();
            id = resource.startResourceUpdation(
                    SimulatorResource.AutoUpdateType.REPEAT, 1000,
                    automationListener);

            try {
                lockObject.await(10, TimeUnit.SECONDS);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

            resource.stopUpdation(id);
        } catch (InvalidArgsException e) {
            e.printStackTrace();
        } catch (SimulatorException e) {
            e.printStackTrace();
        }

        try {
            resource.stop();
            resource.stopUpdation(id);
        } catch (SimulatorException e) {
            e.printStackTrace();
        }

        assertNotNull(autoUpdateHolder.get());
        assertEquals(id, autoUpdateHolder.get().getId());
    }

    public void testStartResourceAutomation_N01() {
        ExceptionType exType = ExceptionType.UNKNOWN;

        try {
            singleResource.startResourceUpdation(
                    SimulatorResource.AutoUpdateType.ONE_TIME, 500, null);
        } catch (InvalidArgsException e) {
            exType = ExceptionType.INVALID_ARGS;
        } catch (SimulatorException e) {
            exType = ExceptionType.SIMULATOR;
        }

        assertEquals(ExceptionType.INVALID_ARGS, exType);
    }

    public void testStartAttributeAutomation_P01() {
        SimulatorSingleResource resource = createResourceFromRAML();
        if (null == resource)
            return;

        String attributeName = null;
        try {
            for (SimulatorResourceAttribute resAttribute : resource
                    .getResourceModel().getAttributes().values())
                attributeName = resAttribute.name();
        } catch (SimulatorException e1) {
            e1.printStackTrace();
        }

        if (null == attributeName)
            return;

        CountDownLatch lockObject = new CountDownLatch(1);
        ObjectHolder<AutoUpdateInfo> autoUpdateHolder = new ObjectHolder<>();
        AutoUpdateCompleteListener automationListener = new AutoUpdateCompleteListener(
                lockObject, autoUpdateHolder);
        int id = -1;

        try {
            resource.start();
            id = resource.startAttributeUpdation(attributeName,
                    SimulatorResource.AutoUpdateType.REPEAT, 100,
                    automationListener);

            try {
                lockObject.await(10, TimeUnit.SECONDS);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

            resource.stopUpdation(id);
        } catch (InvalidArgsException e) {
            e.printStackTrace();
        } catch (SimulatorException e) {
            e.printStackTrace();
        }

        try {
            resource.stop();
        } catch (SimulatorException e) {
            e.printStackTrace();
        }

        assertNotNull(autoUpdateHolder.get());
        assertEquals(id, autoUpdateHolder.get().getId());
    }

    public void testStartAttributeAutomation_N01() {
        ExceptionType exType = ExceptionType.UNKNOWN;

        try {
            singleResource.startAttributeUpdation("intensity",
                    SimulatorResource.AutoUpdateType.ONE_TIME, 1000, null);
        } catch (InvalidArgsException e) {
            exType = ExceptionType.INVALID_ARGS;
        } catch (SimulatorException e) {
            exType = ExceptionType.SIMULATOR;
        }

        assertEquals(ExceptionType.INVALID_ARGS, exType);
    }

    public void testStopUpdation_P01() {
        SimulatorSingleResource resource = createResourceFromRAML();
        if (null == resource)
            return;

        CountDownLatch lockObject = new CountDownLatch(1);
        ObjectHolder<AutoUpdateInfo> autoUpdateHolder = new ObjectHolder<>();
        AutoUpdateCompleteListener automationListener = new AutoUpdateCompleteListener(
                lockObject, autoUpdateHolder);
        boolean result = false;
        try {
            resource.start();
            int id = resource.startResourceUpdation(
                    SimulatorResource.AutoUpdateType.REPEAT, 1000,
                    automationListener);
            resource.stopUpdation(id);
            result = true;
        } catch (InvalidArgsException e) {
            e.printStackTrace();
        } catch (SimulatorException e) {
            e.printStackTrace();
        }

        try {
            resource.stop();
        } catch (SimulatorException e) {
            e.printStackTrace();
        }

        assertTrue(result);
    }
}

class AutoUpdateInfo {
    private String uri = null;
    private int    id  = -1;

    AutoUpdateInfo(String uri, int id) {
        this.uri = uri;
        this.id = id;
    }

    public String getUri() {
        return uri;
    }

    public int getId() {
        return id;
    }
}

class AutoUpdateCompleteListener implements AutoUpdateListener {
    private CountDownLatch               lock;
    private ObjectHolder<AutoUpdateInfo> autoUpdateHolder;

    public AutoUpdateCompleteListener(CountDownLatch lock,
            ObjectHolder<AutoUpdateInfo> autoUpdateHolder) {
        this.lock = lock;
        this.autoUpdateHolder = autoUpdateHolder;
    }

    @Override
    public void onUpdateComplete(String uri, int id) {
        autoUpdateHolder.set(new AutoUpdateInfo(uri, id));
        lock.countDown();
    }
}