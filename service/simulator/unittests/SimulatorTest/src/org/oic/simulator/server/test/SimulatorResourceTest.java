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

import java.util.Vector;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;

import org.oic.simulator.InvalidArgsException;
import org.oic.simulator.SimulatorException;
import org.oic.simulator.SimulatorManager;
import org.oic.simulator.client.FindResourceListener;
import org.oic.simulator.client.SimulatorRemoteResource;
import org.oic.simulator.server.SimulatorResource;
import org.oic.simulator.test.ExceptionType;
import org.oic.simulator.utils.ObjectHolder;

import junit.framework.TestCase;

/**
 * This class tests the APIs of SimulatorResource class
 */
public class SimulatorResourceTest extends TestCase {
    private static final String RES_NAME = "test-resource";
    private static final String RES_URI  = "/test/resource";
    private static final String RES_TYPE = "test.resource";
    private SimulatorResource   resource = null;

    static {
        System.loadLibrary("SimulatorManager");
        System.loadLibrary("RamlParser");
        System.loadLibrary("oc");
        System.loadLibrary("oc_logger");
        System.loadLibrary("octbstack");
    }

    protected void setUp() throws Exception {
        super.setUp();
        resource = SimulatorManager.createResource(
                SimulatorResource.Type.SINGLE, RES_NAME, RES_URI, RES_TYPE);
    }

    protected void tearDown() throws Exception {
        super.tearDown();
        resource = null;
    }

    public void testGetName_P01() {
        String name = null;

        try {
            name = resource.getName();
        } catch (SimulatorException e) {
            e.printStackTrace();
        }

        assertEquals(RES_NAME, name);
    }

    public void testGetType_P01() {
        SimulatorResource.Type type = SimulatorResource.Type.SINGLE;

        try {
            type = resource.getType();
        } catch (SimulatorException e) {
            e.printStackTrace();
        }

        assertEquals(SimulatorResource.Type.SINGLE, type);
    }

    public void testGetURI_P01() {
        String uri = null;

        try {
            uri = resource.getURI();
        } catch (SimulatorException e) {
            e.printStackTrace();
        }

        assertEquals(RES_URI, uri);
    }

    public void testGetResourceType_P01() {
        String resType = null;

        try {
            resType = resource.getResourceType();
        } catch (SimulatorException e) {
            e.printStackTrace();
        }

        assertEquals(RES_TYPE, resType);
    }

    public void testGetInterface_P01() {
        Vector<String> interfaces = null;

        try {
            interfaces = resource.getInterface();
        } catch (SimulatorException e) {
            e.printStackTrace();
        }

        assertNotNull(interfaces);
        assertTrue(interfaces.size() > 0);
    }

    public void testSetName_P01() {
        String name = "new-name";
        String newName = null;

        try {
            resource.setName(name);
            newName = resource.getName();
        } catch (InvalidArgsException e) {
            e.printStackTrace();
        } catch (SimulatorException e) {
            e.printStackTrace();
        }

        assertEquals(name, newName);
    }

    public void testSetName_N01() {
        String newName = "";
        ExceptionType exType = ExceptionType.UNKNOWN;

        try {
            resource.setName(newName);
        } catch (InvalidArgsException e) {
            exType = ExceptionType.INVALID_ARGS;
        } catch (SimulatorException e) {
            exType = ExceptionType.SIMULATOR;
        }

        assertEquals(ExceptionType.INVALID_ARGS, exType);
    }

    public void testSetName_N02() {
        String newName = null;
        ExceptionType exType = ExceptionType.UNKNOWN;

        try {
            resource.setName(newName);
        } catch (InvalidArgsException e) {
            exType = ExceptionType.INVALID_ARGS;
        } catch (SimulatorException e) {
            exType = ExceptionType.SIMULATOR;
        }

        assertEquals(ExceptionType.INVALID_ARGS, exType);
    }

    public void testSetURI_P01() {
        String newUri = "/test/newuri/1";
        String uri = null;

        try {
            resource.setURI(newUri);
            uri = resource.getURI();
        } catch (InvalidArgsException e) {
            e.printStackTrace();
        } catch (SimulatorException e) {
            e.printStackTrace();
        }

        assertEquals(newUri, uri);
    }

    public void testSetURI_N01() {
        String newUri = "";
        ExceptionType exType = ExceptionType.UNKNOWN;

        try {
            resource.setURI(newUri);
        } catch (InvalidArgsException e) {
            exType = ExceptionType.INVALID_ARGS;
        } catch (SimulatorException e) {
            exType = ExceptionType.SIMULATOR;
        }

        assertEquals(ExceptionType.INVALID_ARGS, exType);
    }

    public void testSetURI_N02() {
        String newUri = null;
        ExceptionType exType = ExceptionType.UNKNOWN;

        try {
            resource.setURI(newUri);
        } catch (InvalidArgsException e) {
            exType = ExceptionType.INVALID_ARGS;
        } catch (SimulatorException e) {
            exType = ExceptionType.SIMULATOR;
        }

        assertEquals(ExceptionType.INVALID_ARGS, exType);
    }

    public void testSetResourceType_P01() {
        String newResType = "test.newresource";
        String resType = null;

        try {
            resource.setResourceType(newResType);
            resType = resource.getResourceType();
        } catch (InvalidArgsException e) {
            e.printStackTrace();
        } catch (SimulatorException e) {
            e.printStackTrace();
        }

        assertEquals(newResType, resType);
    }

    public void testSetResourceType_N01() {
        String newResType = "";
        ExceptionType exType = ExceptionType.UNKNOWN;

        try {
            resource.setResourceType(newResType);
        } catch (InvalidArgsException e) {
            exType = ExceptionType.INVALID_ARGS;
        } catch (SimulatorException e) {
            exType = ExceptionType.SIMULATOR;
        }

        assertEquals(ExceptionType.INVALID_ARGS, exType);
    }

    public void testSetResourceType_N02() {
        String newResType = null;
        ExceptionType exType = ExceptionType.UNKNOWN;

        try {
            resource.setResourceType(newResType);
        } catch (InvalidArgsException e) {
            exType = ExceptionType.INVALID_ARGS;
        } catch (SimulatorException e) {
            exType = ExceptionType.SIMULATOR;
        }

        assertEquals(ExceptionType.INVALID_ARGS, exType);
    }

    public void testSetObservable_P01() {
        boolean newState = true;
        boolean state = false;
        try {
            resource.setObservable(newState);
            state = resource.isObservable();
        } catch (SimulatorException e) {
            e.printStackTrace();
        }

        assertEquals(newState, state);
    }

    public void testSetObservable_P02() {
        boolean newState = false;
        boolean state = true;
        try {
            resource.setObservable(newState);
            state = resource.isObservable();
        } catch (SimulatorException e) {
            e.printStackTrace();
        }

        assertEquals(newState, state);
    }

    public void testIsObservable_P01() {
        boolean state = false;

        try {
            state = resource.isObservable();
        } catch (SimulatorException e) {
            e.printStackTrace();
        }

        assertEquals(true, state);
    }

    public void testIsStarted_P01() {
        boolean state = true;

        try {
            state = resource.isStarted();
        } catch (SimulatorException e) {
            e.printStackTrace();
        }

        assertEquals(false, state);
    }

    public void testIsStarted_P02() {
        boolean state = false;

        try {
            resource.start();
            state = resource.isStarted();
            resource.stop();
        } catch (SimulatorException e) {
            e.printStackTrace();
        }

        assertEquals(true, state);
    }

    public void testIsStarted_P03() {
        boolean state = true;

        try {
            resource.start();
            resource.stop();
            state = resource.isStarted();
        } catch (SimulatorException e) {
            e.printStackTrace();
        }

        assertEquals(false, state);
    }

    public void testStart_P01() {
        CountDownLatch lockObject = new CountDownLatch(1);
        ObjectHolder<SimulatorRemoteResource> resourceHolder = new ObjectHolder<>();
        FindResourceCallbackListener listener = new FindResourceCallbackListener(
                lockObject, resourceHolder);

        try {
            resource.start();
            SimulatorManager.findResource(resource.getResourceType(), listener);

            // Wait for the resource to found
            try {
                lockObject.await(10, TimeUnit.SECONDS);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
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

        assertNotNull(resourceHolder.get());
    }

    public void testStop_P01() {
        CountDownLatch lockObject = new CountDownLatch(1);
        ObjectHolder<SimulatorRemoteResource> resourceHolder = new ObjectHolder<>();
        FindResourceCallbackListener listener = new FindResourceCallbackListener(
                lockObject, resourceHolder);

        try {
            resource.start();
            resource.stop();
            SimulatorManager.findResource(resource.getResourceType(), listener);

            // Wait for the resource to found
            try {
                lockObject.await(10, TimeUnit.SECONDS);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
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

        assertNull(resourceHolder.get());
    }
}

class FindResourceCallbackListener implements FindResourceListener {

    private CountDownLatch                        mLockObject;
    private ObjectHolder<SimulatorRemoteResource> mResourceHolder;

    public FindResourceCallbackListener(CountDownLatch lockObject,
            ObjectHolder<SimulatorRemoteResource> resourceHolder) {
        mLockObject = lockObject;
        mResourceHolder = resourceHolder;
    }

    @Override
    public void onResourceFound(SimulatorRemoteResource resource) {
        mResourceHolder.set(resource);
        mLockObject.countDown();
    }
}