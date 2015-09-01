/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 *
 *
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
 *
 ******************************************************************/
package com.re.sampleclient;

import java.util.Vector;

import org.iotivity.ResourceEncapsulation.client.RCSAddress;
import org.iotivity.ResourceEncapsulation.client.RCSDiscoveryManager;
import org.iotivity.ResourceEncapsulation.client.RCSDiscoveryManager.*;
import org.iotivity.ResourceEncapsulation.client.RCSRemoteResourceObject;
import org.iotivity.ResourceEncapsulation.client.RCSRemoteResourceObject.*;
import org.iotivity.ResourceEncapsulation.common.RCSException;
import org.iotivity.ResourceEncapsulation.common.RCSResourceAttributes;

import android.os.Message;
import android.util.Log;

/**
 * For calling the Resource Client APIs as per user selection on UI and for
 * updating the UI
 *
 * It contains all the Resource Client APIs.
 */
public class ResourceClient {

    private static final String            LOG_TAG                  = "[RESample] Client";
    private static Message                 msg;
    public String                          logMessage;

    private final RCSDiscoveryManager      discoveyManagerObj;
    public RCSRemoteResourceObject         remoteResourceObj;

    private static ResourceClientActivity  resourcClientActivityObj = null;
    private final DiscoverResourceListener discoverResourceListener;
    private final StateChangedListener     stateChangedListener;
    private final CacheUpdateListener      cacheUpdateListener;
    private final RemoteAttributeListener  remoteAttributeListener;
    static boolean                         monitoringState          = false;
    static boolean                         cachingState             = false;

    // constructor
    public ResourceClient() {
        discoveyManagerObj = RCSDiscoveryManager.getInstance();
        discoverResourceListener = new DiscoverResourceListener();
        stateChangedListener = new StateChangedListener();
        cacheUpdateListener = new CacheUpdateListener();
        remoteAttributeListener = new RemoteAttributeListener();

        discoveyManagerObj
                .setDiscoverResourceListener(discoverResourceListener);

        resourcClientActivityObj = ResourceClientActivity
                .getResourceClientActivityObj();
    }

    /**
     * Listener for receiving Resource discovered in network.
     */
    private class DiscoverResourceListener implements IDiscoverResourceListener{

        @Override
        public void onResourceDiscovered(RCSRemoteResourceObject foundResource) {
            Log.i(LOG_TAG, "onResourceDiscovered invoked");

            String resourceURI = foundResource.getUri();
            String hostAddress = foundResource.getAddress();
            boolean isObservableflag = foundResource.isObservable();

            Vector<String> resourceTypes = foundResource.getTypes();
            Vector<String> resourceInterfaces = foundResource.getInterFaces();
            remoteResourceObj = foundResource;

            msg = Message.obtain();
            msg.what = 0;
            resourcClientActivityObj.getHandler().sendMessage(msg);

            logMessage = "URI : " + resourceURI + "\n";
            logMessage = logMessage + "Host : " + hostAddress + "\n";
            for (int i = 0; i < resourceTypes.size(); i++) {
                logMessage = logMessage + "resourceType : "
                        + resourceTypes.get(i) + "\n";
            }

            for (int i = 0; i < resourceInterfaces.size(); i++) {
                logMessage = logMessage + "resourceInterfaces : "
                        + resourceInterfaces.get(i) + "\n";
            }
            logMessage = logMessage + "isObservable : " + isObservableflag
                    + "\n";

            resourcClientActivityObj.setMessageLog(logMessage);
            msg = Message.obtain();
            msg.what = 1;
            resourcClientActivityObj.getHandler().sendMessage(msg);
        }
    };

    /**
     * Listener for receiving Changed resource state as a result of
     * startMonitoring API
     */
    private class StateChangedListener implements IStateChangedCallbackListener {

        @Override
        public void onStateChanged(ResourceState resourceState) {
            Log.i(LOG_TAG, "onStateChangedCallback invoked");

            switch (resourceState) {
                case NONE:
                    Log.i(LOG_TAG, "Current Resource State : NOT_MONITORING");
                    logMessage = "Current Resource State : NOT_MONITORING\n";
                    break;

                case ALIVE:
                    Log.i(LOG_TAG, "Current Resource State : ALIVE");
                    logMessage = "Current Resource State : ALIVE\n";
                    break;

                case REQUESTED:
                    Log.i(LOG_TAG, "Current Resource State : REQUESTED");
                    logMessage = "Current Resource State : REQUESTED\n";
                    break;

                case LOST_SIGNAL:
                    Log.i(LOG_TAG, "Current Resource State : LOST_SIGNAL");
                    logMessage = "Current Resource State : LOST_SIGNAL\n";
                    logMessage = logMessage
                            + "Start the server and press discover resource button"
                            + "\n";
                    break;

                case DESTROYED:
                    Log.i(LOG_TAG, "Current Resource State : DESTROYED");
                    logMessage = "Current Resource State : DESTROYED\n";
                    break;
            }

            ResourceClientActivity.setMessageLog(logMessage);
            msg = Message.obtain();
            msg.what = 1;
            resourcClientActivityObj.getHandler().sendMessage(msg);
        }

    }

    /**
     * Listener for receiving updated resource attributes as a result of
     * startCaching() API with callback
     */
    private class CacheUpdateListener implements ICacheUpdateListener {
        @Override
        public void onCacheUpdatedCallback(RCSResourceAttributes attributes) {
            Log.i(LOG_TAG, "onCacheUpdatedCallback invoked");

            printAttributes(attributes);
        }
    };

    /**
     * Listener for receiving the resource attributes as a result of
     * getRemoteAttributes() and setRemoteAttributes() API
     */
    private class RemoteAttributeListener implements IRemoteAttributeListener {
        @Override
        public void onRemoteAttributesGetCallback(
                RCSResourceAttributes attributes) {

            Log.i(LOG_TAG, "onRemoteAttributesGetCallback invoked");
            printAttributes(attributes);
        }
    };

    public void disocverResources() {
        Log.d(LOG_TAG, "discover resources entry");
        RCSAddress address = RCSAddress.multicast();
        try {
            discoveyManagerObj.discoverResource(address);
        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        Log.d(LOG_TAG, "discover resources exit");
    }

    public void startMonitoring() {
        Log.d(LOG_TAG, "startMonitoring entry");

        if (null == remoteResourceObj) {
            Log.e(LOG_TAG, "Remote Resource object NULL");
            return;
        }

        if (true == monitoringState) {
            logMessage = "Monitoring already started\n";
            Log.e(LOG_TAG, "Monitoring already started");
            ResourceClientActivity.setMessageLog(logMessage);
            msg = Message.obtain();
            msg.what = 1;
            resourcClientActivityObj.getHandler().sendMessage(msg);
            return;
        }

        remoteResourceObj.setStateChangedListener(stateChangedListener);

        if (null != stateChangedListener) {
            try {
                remoteResourceObj.startMonitoring();
                monitoringState = true;
            } catch (RCSException e) {
                e.printStackTrace();
                return;
            }
        }
        Log.d(LOG_TAG, "startMonitoring exit");
    }

    public void stopMonitoring() {
        Log.d(LOG_TAG, "stopMonitoring entry");

        if (monitoringState) {
            if (null == remoteResourceObj) {
                Log.e(LOG_TAG, "Remote Resource object NULL");
                return;
            }

            remoteResourceObj.stopMonitoring();
            monitoringState = false;
            logMessage = "Stopped Resource Monitoring\n";
        } else {
            Log.i(LOG_TAG, "Monitoring not started");
            logMessage = "Monitoring not started\n";
        }

        Log.d(LOG_TAG, "stopMonitoring exit");

        ResourceClientActivity.setMessageLog(logMessage);
        msg = Message.obtain();
        msg.what = 1;
        resourcClientActivityObj.getHandler().sendMessage(msg);
    }

    public void getRemoteAttributes() {
        Log.d(LOG_TAG, "getRemoteAttributes entry");

        if (null == remoteResourceObj) {
            Log.e(LOG_TAG, "Remote Resource object NULL");
            return;
        }

        try {
            remoteResourceObj
                    .setRemoteAttributesListener(remoteAttributeListener);

            if (null != remoteAttributeListener) {
                remoteResourceObj.getRemoteAttributes();
            }
        } catch (Exception e) {
            e.printStackTrace();
            return;
        }

        Log.d(LOG_TAG, "getRemoteAttributes exit");
    }

    public void setRemoteAttributes(int value) {
        Log.d(LOG_TAG, "setRemoteAttributes entry");

        if (null == remoteResourceObj) {
            Log.e(LOG_TAG, "Remote Resource object NULL");
            return;
        }

        try {
            remoteResourceObj
                    .setRemoteAttributesListener(remoteAttributeListener);

            if (null != remoteAttributeListener) {
                RCSResourceAttributes attr = new RCSResourceAttributes();
                attr.setValueInt("Temperature", value);
                remoteResourceObj.setRemoteAttributes(attr);
            }
        } catch (Exception e) {
            e.printStackTrace();
            return;
        }

        Log.d(LOG_TAG, "setRemoteAttributes exit");
    }

    public void startCaching(int cachingType) {
        Log.d(LOG_TAG, "startCaching entry");

        if (null == remoteResourceObj) {
            Log.e(LOG_TAG, "Remote Resource object NULL");
            return;
        }

        if (true == cachingState) {
            Log.d(LOG_TAG, "Caching already started");
            logMessage = "Caching already started\n";
            ResourceClientActivity.setMessageLog(logMessage);
            msg = Message.obtain();
            msg.what = 1;
            resourcClientActivityObj.getHandler().sendMessage(msg);
            return;
        }

        if (0 == cachingType) {
            Log.d(LOG_TAG, "startCaching without callback");
            remoteResourceObj.setCacheUpdateListener(null);
            logMessage = "Caching started without callback\n";
            ResourceClientActivity.setMessageLog(logMessage);
            msg = Message.obtain();
            msg.what = 1;
            resourcClientActivityObj.getHandler().sendMessage(msg);
        } else {
            Log.d(LOG_TAG, "startCaching with callback");
            remoteResourceObj.setCacheUpdateListener(cacheUpdateListener);
            logMessage = "Caching started\n";
            ResourceClientActivity.setMessageLog(logMessage);
            msg = Message.obtain();
            msg.what = 1;
            resourcClientActivityObj.getHandler().sendMessage(msg);
        }

        try {
            remoteResourceObj.startCaching();
            cachingState = true;
        } catch (RCSException e) {
            e.printStackTrace();
            return;
        }

        Log.d(LOG_TAG, "startCaching exit");
    }

    public void getCacheState() {
        Log.d(LOG_TAG, "getCacheState entry");

        if (null == remoteResourceObj) {
            Log.e(LOG_TAG, "Remote Resource object NULL");
            return;
        }

        CacheState cState = remoteResourceObj.getCacheState();
        Log.i(LOG_TAG, "Cache State : " + cState.name());

        logMessage = "Cache State : " + cState.toString() + "\n";
        ResourceClientActivity.setMessageLog(logMessage);
        msg = Message.obtain();
        msg.what = 1;
        resourcClientActivityObj.getHandler().sendMessage(msg);

        Log.d(LOG_TAG, "getCacheState exit");
    }

    public void getCachedAttributes() {
        Log.d(LOG_TAG, "getCachedAttributes entry");

        if (null == remoteResourceObj) {
            Log.e(LOG_TAG, "Remote Resource object NULL");
            return;
        }
        if (true == cachingState) {
            RCSResourceAttributes attributes = null;
            try {
                attributes = remoteResourceObj.getCachedAttributes();
            } catch (RCSException e) {
                e.printStackTrace();
                return;
            }
            printAttributes(attributes);
        } else {
            logMessage = "Caching not started" + "\n";
            ResourceClientActivity.setMessageLog(logMessage);
            msg = Message.obtain();
            msg.what = 1;
            resourcClientActivityObj.getHandler().sendMessage(msg);
        }
        Log.d(LOG_TAG, "getCachedAttributes exit");
    }

    public void getCachedAttribute() {
        Log.d(LOG_TAG, "getCachedAttribute entry");

        if (null == remoteResourceObj) {
            Log.e(LOG_TAG, "Remote Resource object NULL");
            return;
        }

        if (true == cachingState) {
            String key = "Temperature";
            String attribute;
            try {
                attribute = remoteResourceObj.getCachedAttribute(key);
            } catch (RCSException e) {
                e.printStackTrace();
                return;
            }
            logMessage = "Key : " + key + "\n";
            logMessage = logMessage + "Value : " + attribute + "\n";
            ResourceClientActivity.setMessageLog(logMessage);
            msg = Message.obtain();
            msg.what = 1;
            resourcClientActivityObj.getHandler().sendMessage(msg);
        } else {
            logMessage = "Caching not started" + "\n";
            ResourceClientActivity.setMessageLog(logMessage);
            msg = Message.obtain();
            msg.what = 1;
            resourcClientActivityObj.getHandler().sendMessage(msg);
        }
        Log.d(LOG_TAG, "getCachedAttribute exit");
    }

    public void stopCaching() {
        Log.d(LOG_TAG, "stopCaching entry");

        if (null == remoteResourceObj) {
            Log.e(LOG_TAG, "Remote Resource object NULL");
            return;
        }

        if (false == cachingState) {
            Log.e(LOG_TAG, "Caching not started");
            logMessage = "Caching not started\n";
        } else {
            remoteResourceObj.stopCaching();
            cachingState = false;
            logMessage = "Stopped Caching\n";
        }

        ResourceClientActivity.setMessageLog(logMessage);
        msg = Message.obtain();
        msg.what = 1;
        resourcClientActivityObj.getHandler().sendMessage(msg);
        Log.d(LOG_TAG, "stopCaching exit");
    }

    private void printAttributes(RCSResourceAttributes attributes) {
        Log.d(LOG_TAG, "Printing Attributes");

        try {
            int temperature = attributes.getValueInt("Temperature");
            Log.i(LOG_TAG, "Temperature : " + temperature);
            logMessage = "Attributes : \n\nTemperature : " + temperature + "\n";
        } catch (Exception e) {
            Log.i(LOG_TAG, "Temperature attribute not found");
            logMessage = "Attributes Temperature not found \n";
            e.printStackTrace();
            return;
        }

        ResourceClientActivity.setMessageLog(logMessage);
        msg = Message.obtain();
        msg.what = 1;
        resourcClientActivityObj.getHandler().sendMessage(msg);
    }
}