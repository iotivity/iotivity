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

package org.iotivity.service.re;

import java.util.ArrayList;

import org.iotivity.service.RcsException;
import org.iotivity.service.RcsResourceAttributes;
import org.iotivity.service.RcsValue;
import org.iotivity.service.client.RcsAddress;
import org.iotivity.service.client.RcsDiscoveryManager;
import org.iotivity.service.client.RcsRemoteResourceObject;
import org.iotivity.service.client.RcsDiscoveryManager.OnResourceDiscoveredListener;
import org.iotivity.service.client.RcsRemoteResourceObject.CacheState;
import org.iotivity.service.client.RcsRemoteResourceObject.OnCacheUpdatedListener;
import org.iotivity.service.client.RcsRemoteResourceObject.OnRemoteAttributesReceivedListener;
import org.iotivity.service.client.RcsRemoteResourceObject.OnStateChangedListener;
import org.iotivity.service.client.RcsRemoteResourceObject.ResourceState;
import org.iotivity.service.server.RcsGetResponse;
import org.iotivity.service.server.RcsRequest;
import org.iotivity.service.server.RcsSetResponse;
import org.iotivity.service.server.RcsResourceObject.GetRequestHandler;
import org.iotivity.service.server.RcsResourceObject.OnAttributeUpdatedListener;
import org.iotivity.service.server.RcsResourceObject.SetRequestHandler;

import static org.iotivity.service.re.REUtility.*;

import android.os.AsyncTask;
import android.util.Log;

import org.iotivity.service.testapp.framework.Base;

public class REAppHelper extends Base {
    public Boolean m_IsDiscoverLogging = false;
    public Boolean g_IsDiscovering = false;
    private static REAppHelper s_REHelperInstance = null;
    private RcsRemoteResourceObject m_Resource = null;
    private RcsDiscoveryManager.DiscoveryTask m_DiscoveryTask = null;
    private ArrayList<RcsRemoteResourceObject> m_ResourceList = null;
    private ArrayList<String> m_UriList = null;

    private REAppHelper() {
    }

    public static REAppHelper getInstance() {
        if (s_REHelperInstance == null) {
            s_REHelperInstance = new REAppHelper();
        }

        return s_REHelperInstance;
    }

    public OnResourceDiscoveredListener m_OnResourceDiscoveredListener = new OnResourceDiscoveredListener() {
        @Override
        public void onResourceDiscovered(
                RcsRemoteResourceObject foundResource) {
            m_Resource = foundResource;

            m_ResourceList
                    .add(foundResource);

            showLog(LogLevel.INFO,
                    "***onResourceDiscovered***",
                    false);

            printResourceDetail(foundResource);
        }
    };

    public OnStateChangedListener m_OnStateChangedListener = new OnStateChangedListener() {
        @Override
        public void onStateChanged(
                ResourceState resourceState) {
            showLog(LogLevel.INFO,
                    "State changed to : "
                            + getStringResourceState(resourceState),
                    false);
        }
    };

    public OnRemoteAttributesReceivedListener m_OnRemoteAttributesReceivedListener = new OnRemoteAttributesReceivedListener() {
        @Override
        public void onAttributesReceived(
                RcsResourceAttributes attrs,
                int eCode) {
            showLog(LogLevel.INFO,
                    "***onAttributesReceived***",
                    false);
            printAttributes(attrs);
        }
    };

    public OnCacheUpdatedListener m_OnCacheUpdatedListener = new OnCacheUpdatedListener() {
        @Override
        public void onCacheUpdated(
                RcsResourceAttributes attrs) {
            showLog(LogLevel.INFO,
                    "***onCacheUpdated***",
                    false);
            printAttributes(attrs);
        }
    };

    public GetRequestHandler m_GetRequestHandler = new GetRequestHandler() {
        @Override
        public RcsGetResponse onGetRequested(
                RcsRequest request,
                RcsResourceAttributes attrs) {
            showLog(LogLevel.INFO,
                    "Received a GET request from CLIENT. \nSend default response for URI: "
                            + request
                            .getResourceUri(),
                    false);

            return RcsGetResponse
                    .defaultAction();
        }
    };

    public SetRequestHandler m_SetRequestHandler = new SetRequestHandler() {
        @Override
        public RcsSetResponse onSetRequested(
                RcsRequest request,
                RcsResourceAttributes attrs) {
            showLog(LogLevel.INFO,
                    "Received a SET request from CLIENT. \nSend default response for URI: "
                            + request
                            .getResourceUri(),
                    false);

            return RcsSetResponse
                    .defaultAction();
        }
    };

    public OnAttributeUpdatedListener m_AttributeUpdatedListener = new OnAttributeUpdatedListener() {
        @Override
        public void onAttributeUpdated(
                RcsValue oldRcsValue,
                RcsValue newRcsValue) {
            showLog(LogLevel.INFO,
                    "Old RcsValue: "
                            + oldRcsValue
                            + " New RcsValue: "
                            + newRcsValue,
                    false);
        }
    };

    private void printResourceDetail(RcsRemoteResourceObject resource) {
        try {
            String resLog = "Resource URI: " + resource.getUri() + " Address: "
                    + resource.getAddress();
            showLog(LogLevel.INFO, resLog, false);

            showLog(LogLevel.INFO, "Resource Types: ", false);
            resLog = "";
            for (String type : resource.getTypes()) {
                resLog += " " + type;
            }

            showLog(LogLevel.INFO, resLog, false);

            showLog(LogLevel.INFO, "Resource Interfaces: ", false);
            resLog = "";
            for (String ifs : resource.getInterfaces()) {
                resLog += " " + ifs;
            }

            showLog(LogLevel.INFO, resLog, false);
        } catch (Exception e) {
            showLog(LogLevel.INFO, "Resource Info Error...", false);
        }
    }

    public void waitInSecond(int time) {
        int timeInSec = time * 1000;

        try {
            Thread.sleep(timeInSec);
        } catch (InterruptedException ex) {
            Thread.currentThread().interrupt();
        }
    }

    public RcsRemoteResourceObject getResourceFromSelection(String uri) {
        m_IsDiscoverLogging = false;

        if (m_ResourceList == null)
            return null;

        showLog(LogLevel.INFO, "Uri: " + uri, false);

        try {
            for (RcsRemoteResourceObject rcsRemoteResourceObject : m_ResourceList) {
                showLog(LogLevel.INFO, "ResUri: " + rcsRemoteResourceObject.getUri(), false);

                if (rcsRemoteResourceObject.getUri().contains(uri)) {
                    return rcsRemoteResourceObject;
                }
            }
        } catch (Exception e) {
            return null;
        }

        return null;
    }

    public ArrayList<RcsRemoteResourceObject> getResourceList() {
        m_IsDiscoverLogging = false;

        return m_ResourceList;
    }

    public Boolean checkResourceWithUri(String uri) {
        try {
            for (RcsRemoteResourceObject resource : m_ResourceList) {
                if (resource.getUri().contains(uri)) {
                    return true;
                }
            }
        } catch (Exception e) {
            return false;
        }

        return false;
    }

    public void printAttributes(RcsResourceAttributes attrs) {
        showLog(LogLevel.INFO, "*** [ATTRIBUTES] ***", false);

        for (String key : attrs.keySet()) {
            showLog(LogLevel.INFO,
                    "Attribute : " + key + " = " + attrs.get(key), false);
        }
    }

    public String getStringResourceState(ResourceState resourceState) {
        switch (resourceState) {
            case NONE:
                return "NONE";

            case ALIVE:
                return "ALIVE";

            case REQUESTED:
                return "REQUESTED";

            case LOST_SIGNAL:
                return "LOST_SIGNAL";

            case DESTROYED:
                return "DESTROYED";

            default:
                return "NOT_DEFINED";
        }
    }

    public String getStringCacheState(CacheState cacheState) {
        switch (cacheState) {
            case NONE:
                return "NONE";

            case READY:
                return "READY";

            case UNREADY:
                return "UNREADY";

            case LOST_SIGNAL:
                return "LOST_SIGNAL";

            default:
                return "NOT_DEFINED";
        }
    }

    public Boolean isResourceAvailable(RcsRemoteResourceObject resource) {
        if (resource == null) {
            showLog(LogLevel.ERROR, "Discover Resource first.", true);
            return false;
        }

        return true;
    }

    public void showLog(LogLevel level, String log, Boolean isLogClear) {
        switch (level) {
            case INFO:
                Log.i(TAG, log);
                break;

            case DEBUG:
                Log.d(TAG, log);
                break;

            case WARNING:
                Log.w(TAG, log);
                break;

            case ERROR:
                Log.e(TAG, log);
                break;

            default:
                Log.v(TAG, log);
                break;
        }

        if (isLogClear) {
            clearOutPut();
        }

        showOutPut(log);
    }

    enum LogLevel {
        INFO, DEBUG, WARNING, VERBOSE, ERROR
    }

    public void discoverAllResource(ArrayList<String> uriList) {
        m_UriList = uriList;

        try {
            m_DiscoveryTask = RcsDiscoveryManager.getInstance()
                    .discoverResource(RcsAddress.multicast(),
                            m_OnResourceDiscoveredListener);
        } catch (RcsException e1) {
            showLog(LogLevel.ERROR,
                    "Exception occurs... " + e1.getLocalizedMessage(), false);
        }
    }

    public void discoverResourceWithQuery(ArrayList<String> uriList) {
        m_UriList = uriList;

        try {
            m_DiscoveryTask = RcsDiscoveryManager
                    .getInstance()
                    .discoverResourceByType(RcsAddress.multicast(),
                            TEMPERATURE_RES_TYPE, m_OnResourceDiscoveredListener);
        } catch (RcsException e1) {
            showLog(LogLevel.ERROR,
                    "Exception occurs... " + e1.getLocalizedMessage(), false);
        }
    }

    public class ResourceFounderTask extends
            AsyncTask<String, String, RcsRemoteResourceObject> {

        @Override
        protected RcsRemoteResourceObject doInBackground(String... params) {
            m_IsDiscoverLogging = true;
            g_IsDiscovering = true;
            m_ResourceList = new ArrayList<RcsRemoteResourceObject>();

            int timeCounter = 0;
            int waitTimeout = Integer.valueOf(params[0]);

            while (++timeCounter < waitTimeout) {
              waitInSecond(WAIT_TIME_MIN);
            }

            return m_Resource;
        }

        @Override
        protected void onProgressUpdate(String... values) {
            super.onProgressUpdate(values);
        }

        @Override
        protected void onPostExecute(RcsRemoteResourceObject resource) {
            super.onPostExecute(resource);
            g_IsDiscovering = false;
            m_IsDiscoverLogging = false;

            m_DiscoveryTask.cancel();

            Boolean foundResource = false;

            try {
                for (String uri : m_UriList) {
                    foundResource = checkResourceWithUri(uri);

                    if (!foundResource)
                        break;
                }
            } catch (Exception e) {
                showLog(LogLevel.ERROR,
                        "Exception occurred while finding resource, exception is: "
                                + e.getLocalizedMessage(), false);
            }

            if (foundResource) {
                if (m_UriList.size() > 1)
                    showLog(LogLevel.INFO, "All resource found successfully.",
                            false);
                else
                    showLog(LogLevel.INFO, "Resource found successfully.",
                            false);
            } else
                showLog(LogLevel.INFO, "Can't find proper resource", false);
        }

        Boolean checkResourceWithUri(String uri) {
            try {
                for (RcsRemoteResourceObject res : m_ResourceList) {
                    if (res.getUri().contains(uri)) {
                        return true;
                    }
                }
            } catch (Exception e) {
                return false;
            }

            return false;
        }
    }
}
