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

package org.iotivity.service.rc;

import org.iotivity.service.resourcecontainer.BundleResource;
import org.iotivity.service.resourcecontainer.RcsResourceAttributes;
import org.iotivity.service.rc.RCServer;
import static org.iotivity.service.rc.RCTestAppStaticUtil.*;
import static org.iotivity.service.testapp.framework.Base.*;
import org.iotivity.service.rc.RCClient;

import java.util.*;

public class Action {

    private static int                ADDED_BUNDLE_COUNT   = 1;
    private static int                STARTED_BUNDLE_COUNT = 1;
    private static final int          BUNDLE_MAX_SIZE      = 2;
    private RCServer                  rcServer;
    private RCClient                  rcClient;
    private Vector<BundleInformation> vecBundleInfoList;

    Action() {
        rcServer = new RCServer();
        rcClient = new RCClient();
        vecBundleInfoList = new Vector<BundleInformation>();
        initializeBundleInfo();
    }

    public void startContainer() {
        if (RCServer.isContainerStarted) {
            showOutPut("Resource container has been started already...");
            return;
        }

        rcServer.rcStartContainer();
        initializeBundleInfo();
        ADDED_BUNDLE_COUNT = 1;
        STARTED_BUNDLE_COUNT = 1;
    }

    public void stopContainer() {
        if (!RCServer.isContainerStarted) {
            showOutPut("Please start resource container before perform this action...");
            return;
        }

        rcServer.rcStopContainer();
        initializeBundleInfo();
        ADDED_BUNDLE_COUNT = 1;
        STARTED_BUNDLE_COUNT = 1;
    }

    public void addBundle() {
        if (ADDED_BUNDLE_COUNT > 2) {
            showOutPut("Sorry.You can add maximum 2 bundle id for this test application...");
            return;
        }

        int flag = getARandomAvailableBundleID();
        if (flag == BUNDLE_MAX_SIZE + 1) {
            showOutPut("Existing bundle id is already added...");
            return;
        }

        rcServer.rcAddBundle(vecBundleInfoList.elementAt(flag).mBundleId,
                vecBundleInfoList.elementAt(flag).mBundleUri,
                vecBundleInfoList.elementAt(flag).mBundlePath,
                vecBundleInfoList.elementAt(flag).mActivator,
                vecBundleInfoList.elementAt(flag).mBundleParams);
        vecBundleInfoList.elementAt(flag).isBundleIDAdded = true;
        ADDED_BUNDLE_COUNT++;
    }

    public void removeBundle() {
        if (ADDED_BUNDLE_COUNT <= 1) {
            showOutPut("Sorry.There is no added bundle id to remove...");
            return;
        }

        int flag = getARandomActiveBundleID();
        if (flag == BUNDLE_MAX_SIZE) {
            showOutPut("Existing bundle id is already removed...");
            return;
        }

        // Note:Before remove bundle Hue(light type) registered resource should
        // be unregistered
        if (vecBundleInfoList.elementAt(flag).mBundleId
                .compareTo(HUE_BUNDLE_ID) == 0) {
            BundleResource bundleResource = getBundleResourceInstance(flag);
            rcServer.rcUnregisterResource(bundleResource,
                    vecBundleInfoList.elementAt(flag).mBundleId, false);
            vecBundleInfoList.elementAt(flag).isBundleResourceRegistered = false;
        }

        rcServer.rcRemoveBundle(vecBundleInfoList.elementAt(flag).mBundleId);

        vecBundleInfoList.elementAt(flag).isBundleIDAdded = false;
        vecBundleInfoList.elementAt(flag).isBundleStarted = false;
        vecBundleInfoList.elementAt(flag).isBundleResourceAdded = false;
        vecBundleInfoList.elementAt(flag).isBundleResourceRegistered = false;
        STARTED_BUNDLE_COUNT--;
        ADDED_BUNDLE_COUNT--;
    }

    public void startBundle() {

        if (STARTED_BUNDLE_COUNT > 2) {
            showOutPut("Sorry.There is no bundle id is remaining to start..");
            return;
        }

        int flag = getARandomActiveAndStoppedBundleID();
        if (flag == BUNDLE_MAX_SIZE) {
            showOutPut("Existing bundle id is already started...");
            return;
        }

        rcServer.rcStartBundle(vecBundleInfoList.elementAt(flag).mBundleId);
        vecBundleInfoList.elementAt(flag).isBundleStarted = true;
        STARTED_BUNDLE_COUNT++;
        if (vecBundleInfoList.elementAt(flag).isBundleResourceAdded) {
            HashMap<String, String> resourceParams = new HashMap<String, String>();

            resourceParams.put("resourceType",
                    vecBundleInfoList.elementAt(flag).mResourceType);
            resourceParams.put("address",
                    vecBundleInfoList.elementAt(flag).mResourceAddress);

            rcServer.rcAddResourceConfig(
                    vecBundleInfoList.elementAt(flag).mBundleId,
                    vecBundleInfoList.elementAt(flag).mResourceURI,
                    resourceParams, false);
            vecBundleInfoList.elementAt(flag).isBundleResourceAdded = true;

            if (vecBundleInfoList.elementAt(flag).mBundleId
                    .compareTo(HUE_BUNDLE_ID) == 0) {
                BundleResource bundleResource = getBundleResourceInstance(flag);
                rcServer.rcRegisterResource(
                        vecBundleInfoList.elementAt(flag).mBundleId,
                        bundleResource, false);
                vecBundleInfoList.elementAt(flag).isBundleResourceRegistered = true;
            }
        }
    }

    public void stopBundle() {
        if (STARTED_BUNDLE_COUNT <= 1) {
            showOutPut("Sorry.There is no bundle id is remaining to stop..");
            return;
        }

        int flag = getARandomAvailableStartedBundleID();
        if (flag == BUNDLE_MAX_SIZE) {
            showOutPut("Existing bundle id is already stopped...");
            return;
        }

        rcServer.rcStopBundle(vecBundleInfoList.elementAt(flag).mBundleId);
        vecBundleInfoList.elementAt(flag).isBundleStarted = false;
        STARTED_BUNDLE_COUNT--;

        if (vecBundleInfoList.elementAt(flag).mBundleId
                .compareTo(HUE_BUNDLE_ID) == 0) {
            BundleResource bundleResource = getBundleResourceInstance(flag);
            rcServer.rcUnregisterResource(bundleResource,
                    vecBundleInfoList.elementAt(flag).mBundleId, false);
            vecBundleInfoList.elementAt(flag).isBundleResourceRegistered = false;
        }
    }

    public void addResources() {
        if (ADDED_BUNDLE_COUNT <= 1) {
            showOutPut("Please add bundle first...");
            return;
        }

        int flag = getActiveBundleIDAvailableForAddResource();
        if (flag == BUNDLE_MAX_SIZE) {
            showOutPut("Existing bundle id is already used.Please add bundle first...");
            return;
        }

        HashMap<String, String> resourceParams = new HashMap<String, String>();

        resourceParams.put("resourceType",
                vecBundleInfoList.elementAt(flag).mResourceType);
        resourceParams.put("address",
                vecBundleInfoList.elementAt(flag).mResourceAddress);

        rcServer.rcAddResourceConfig(
                vecBundleInfoList.elementAt(flag).mBundleId,
                vecBundleInfoList.elementAt(flag).mResourceURI, resourceParams,
                true);
        vecBundleInfoList.elementAt(flag).isBundleResourceAdded = true;
    }

    public void registerResources() {
        int flag = getActiveBundleIDThatUsedForAddResourceAndNotRegistered();
        if (flag == BUNDLE_MAX_SIZE) {
            showOutPut("Sorry.There is no active bundle id having unregistered resource...");
            return;
        }

        BundleResource bundleResource = getBundleResourceInstance(flag);

        rcServer.rcRegisterResource(
                vecBundleInfoList.elementAt(flag).mBundleId, bundleResource,
                true);
        vecBundleInfoList.elementAt(flag).isBundleResourceRegistered = true;
    }

    public void unRegisterResources() {
        int flag = getActiveBundleIDThatUsedForAddResourceAndRegistered();
        if (flag == BUNDLE_MAX_SIZE) {
            showOutPut("Sorry.There is no active bundle having registered resource...");
            return;
        }

        BundleResource bundleResource = getBundleResourceInstance(flag);

        rcServer.rcUnregisterResource(bundleResource,
                vecBundleInfoList.elementAt(flag).mBundleId, true);
        vecBundleInfoList.elementAt(flag).isBundleResourceRegistered = false;
    }

    public void removeResources() {
        int flag = getActiveBundleIDThatUsedForAddResource();
        if (flag == BUNDLE_MAX_SIZE) {
            showOutPut("Sorry.There is no active bundle having this resource...");
            return;
        }

        if (vecBundleInfoList.elementAt(flag).mBundleId
                .compareTo(HUE_BUNDLE_ID) == 0) {
            BundleResource bundleResource = getBundleResourceInstance(flag);
            rcServer.rcUnregisterResource(bundleResource,
                    vecBundleInfoList.elementAt(flag).mBundleId, false);
            vecBundleInfoList.elementAt(flag).isBundleResourceRegistered = false;
        }

        rcServer.rcRemoveResourceConfig(
                vecBundleInfoList.elementAt(flag).mBundleId,
                vecBundleInfoList.elementAt(flag).mResourceURI);
        vecBundleInfoList.elementAt(flag).isBundleResourceAdded = false;

    }

    public void displayCurrentBundleList() {
        rcServer.rcDisplayCurrentBundleList();
    }

    public void displayCurrentResourceList() {
        showOutPut("Existing bundle resource's are : ");

        for (int index = 0; index < vecBundleInfoList.size(); index++) {
            if (vecBundleInfoList.elementAt(index).isBundleIDAdded
                    && vecBundleInfoList.elementAt(index).isBundleResourceAdded) {
                rcServer.rcDisplayCurrentResourceList(vecBundleInfoList
                        .elementAt(index).mBundleId);
            }
        }

        // Note:Search for default configured resource
        rcServer.rcDisplayCurrentResourceList(DI_BUNDLE_ID);
    }

    public void clearLog() {
        clearOutPut();
    }

    public void exitTestApp() {
        initializeBundleInfo();
        rcServer.rcExitTestApp();
    }

    public void observeResourceContainer() {
        rcClient.rcObserveResourceContainer();
    }

    // ************************************Local function *******************//
    class BundleInformation {
        public String              mBundleId;
        public String              mBundleName;
        public String              mBundleUri;
        public String              mBundlePath;
        public String              mActivator;
        public Map<String, String> mBundleParams;
        public String              mResourceType;
        public String              mResourceAddress;
        public String              mResourceURI;

        public Boolean             isBundleStarted;
        public Boolean             isBundleIDAdded;
        public Boolean             isBundleResourceAdded;
        public Boolean             isBundleResourceRegistered;

        public BundleInformation(String id, String name, String uri,
                String path, String activator, Map<String, String> params,
                String types, String address, String resourceURI) {
            mBundleId = id;
            mBundleName = name;
            mBundleUri = uri;
            mBundlePath = path;
            mActivator = activator;
            mBundleParams = params;
            mResourceType = types;
            mResourceURI = resourceURI;
            mResourceAddress = address;
            isBundleStarted = false;
            isBundleIDAdded = false;
            isBundleResourceAdded = false;
            isBundleResourceRegistered = false;
        }
    }

    private void initializeBundleInfo() {
        BundleInformation BMIBundle = new BundleInformation(BMI_BUNDLE_ID,
                BMI_NAME, BMI_BUNDLE_URI, BMI_SO_PATH, BMI_BUNDLE_ACTIVATOR,
                new HashMap<String, String>(), BMI_BUNDLE_RESOURCE_TYPE,
                BMI_BUNDLE_RESOURCE_ADDRESS, BMI_BUNDLE_RESOURCE_URI);

        BundleInformation HueBundle = new BundleInformation(HUE_BUNDLE_ID,
                HUE_BUNDLE_NAME, HUE_BUNDLE_URI, HUE_BUNDLE_JAR,
                HUE_BUNDLE_ACTIVATOR, new HashMap<String, String>(),
                HUE_BUNDLE_RESOURCE_TYPE, HUE_BUNDLE_RESOURCE_ADDRESS,
                HUE_BUNDLE_RESOURCE_URI);

        vecBundleInfoList.add(BMIBundle);
        vecBundleInfoList.add(HueBundle);
    }

    private BundleResource getBundleResourceInstance(int flag) {
        BundleResource bundleResource = new BundleResource() {

            @Override
            protected void initAttributes() {
                // TODO Auto-generated method stub
            }

            @Override
            public void handleSetAttributesRequest(RcsResourceAttributes value) {
                // TODO Auto-generated method stub
            }

            @Override
            public RcsResourceAttributes handleGetAttributesRequest() {
                // TODO Auto-generated method stub
                return null;
            }

            @Override
            public void deactivateResource() {
                // TODO Auto-generated method stub
            }
        };

        bundleResource.setName(vecBundleInfoList.elementAt(flag).mBundleName);
        bundleResource
                .setAddress(vecBundleInfoList.elementAt(flag).mResourceAddress);
        bundleResource
                .setResourceType(vecBundleInfoList.elementAt(flag).mResourceType);
        bundleResource.setURI(vecBundleInfoList.elementAt(flag).mResourceURI);

        return bundleResource;
    }

    private int getARandomAvailableBundleID() {
        int flag = BUNDLE_MAX_SIZE;

        for (int index = 0; index < vecBundleInfoList.size(); index++) {
            if (!vecBundleInfoList.elementAt(index).isBundleIDAdded) {
                flag = index;
                break;
            }
        }

        return flag;
    }

    int getARandomActiveAndStoppedBundleID() {
        int flag = BUNDLE_MAX_SIZE;

        for (int index = 0; index < vecBundleInfoList.size(); index++) {
            if ((vecBundleInfoList.elementAt(index).isBundleIDAdded)
                    && !(vecBundleInfoList.elementAt(index).isBundleStarted)) {
                flag = index;
                break;
            }
        }

        return flag;
    }

    int getARandomAvailableStartedBundleID() {
        int flag = BUNDLE_MAX_SIZE;

        for (int index = 0; index < vecBundleInfoList.size(); index++) {
            if (vecBundleInfoList.elementAt(index).isBundleIDAdded
                    && vecBundleInfoList.elementAt(index).isBundleStarted) {
                flag = index;
                break;
            }
        }

        return flag;
    }

    int getARandomActiveBundleID() {
        int flag = BUNDLE_MAX_SIZE;

        for (int index = 0; index < vecBundleInfoList.size(); index++) {
            if (vecBundleInfoList.elementAt(index).isBundleIDAdded) {
                flag = index;
                break;
            }
        }

        return flag;
    }

    int getActiveBundleIDAvailableForAddResource() {
        int flag = BUNDLE_MAX_SIZE;

        for (int index = 0; index < vecBundleInfoList.size(); index++) {
            if (vecBundleInfoList.elementAt(index).isBundleIDAdded
                    && !vecBundleInfoList.elementAt(index).isBundleResourceAdded) {
                flag = index;
                break;
            }
        }

        return flag;
    }

    int getActiveBundleIDThatUsedForAddResource() {
        int flag = BUNDLE_MAX_SIZE;

        for (int index = 0; index < vecBundleInfoList.size(); index++) {
            if (vecBundleInfoList.elementAt(index).isBundleIDAdded
                    && vecBundleInfoList.elementAt(index).isBundleResourceAdded) {
                flag = index;
                break;
            }
        }

        return flag;
    }

    int getActiveBundleIDThatUsedForAddResourceAndNotRegistered() {
        int flag = BUNDLE_MAX_SIZE;

        for (int index = 0; index < vecBundleInfoList.size(); index++) {
            if (vecBundleInfoList.elementAt(index).isBundleIDAdded
                    && vecBundleInfoList.elementAt(index).isBundleResourceAdded
                    && !vecBundleInfoList.elementAt(index).isBundleResourceRegistered) {
                flag = index;
                break;
            }
        }

        return flag;
    }

    int getActiveBundleIDThatUsedForAddResourceAndRegistered() {
        int flag = BUNDLE_MAX_SIZE;

        for (int index = 0; index < vecBundleInfoList.size(); index++) {
            if (vecBundleInfoList.elementAt(index).isBundleIDAdded
                    && vecBundleInfoList.elementAt(index).isBundleResourceAdded
                    && vecBundleInfoList.elementAt(index).isBundleResourceRegistered) {
                flag = index;
                break;
            }
        }

        return flag;
    }
}
