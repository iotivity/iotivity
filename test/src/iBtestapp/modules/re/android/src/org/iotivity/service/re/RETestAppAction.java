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
import org.iotivity.service.client.RcsRemoteResourceObject;
import org.iotivity.service.server.RcsResourceObject;
import org.iotivity.service.server.RcsResourceObject.AutoNotifyPolicy;

import static org.iotivity.service.re.REUtility.*;

import org.iotivity.service.re.REAppHelper.LogLevel;
import org.iotivity.service.re.REAppHelper.ResourceFounderTask;
import org.iotivity.service.testapp.framework.Base;

public class RETestAppAction extends Base {
    private RcsRemoteResourceObject m_Resource = null;
    private REAppHelper m_REAppHelper = null;
    private Boolean m_IsResourceCreated = false;
    private RcsResourceObject m_TempActuatorRes = null;
    private static int m_Temperature = 0;

    public RETestAppAction() {
        m_REAppHelper = REAppHelper.getInstance();
        m_IsResourceCreated = false;
    }

    public void createResource() {

        if (!m_IsResourceCreated) {
            m_REAppHelper.showLog(LogLevel.INFO, "Creating Resource...", true);

            try {
                RcsResourceObject.Builder builderSwitch = new RcsResourceObject.Builder(
                        BINARY_SWITCH_URI, BINARY_SWITCH_TYPE,
                        DEFAULT_ACTUATOR_INTERFACE);

                builderSwitch.setDiscoverable(true);
                builderSwitch.setObservable(true);

                builderSwitch.build();

                RcsResourceObject.Builder builderTempS = new RcsResourceObject.Builder(
                        TEMPERATURE_SENSOR_URI, TEMPERATURE_RES_TYPE,
                        DEFAULT_SENSOR_INTERFACE);

                builderTempS.setDiscoverable(false);
                builderTempS.setObservable(false);

                builderTempS.build();

                RcsResourceObject.Builder builderTempA = new RcsResourceObject.Builder(
                        TEMPERATURE_ACTUATOR_URI, TEMPERATURE_RES_TYPE,
                        DEFAULT_SENSOR_INTERFACE);

                builderTempA.setDiscoverable(true);
                builderTempA.setObservable(true);

                m_TempActuatorRes = builderTempA.build();

                m_TempActuatorRes.setAttribute(TEMPERATURE_UNITS_KEY,
                        new RcsValue("C"));
                m_TempActuatorRes.setAttribute(TEMPERATURE_RANGE_KEY,
                        new RcsValue("0,30"));
                m_TempActuatorRes.setAttribute(TEMPERATURE_TEMP_KEY,
                        new RcsValue(m_Temperature));

                m_TempActuatorRes
                        .setGetRequestHandler(m_REAppHelper.m_GetRequestHandler);
                m_TempActuatorRes
                        .setSetRequestHandler(m_REAppHelper.m_SetRequestHandler);
                m_TempActuatorRes.setAutoNotifyPolicy(AutoNotifyPolicy.UPDATED);

                m_REAppHelper.waitInSecond(WAIT_TIME_DEFAULT);

                m_IsResourceCreated = true;

                m_REAppHelper.showLog(LogLevel.INFO,
                        "Resource created successfully.", true);
            } catch (Exception e) {
                m_REAppHelper.showLog(
                        LogLevel.ERROR,
                        "Resource creation failed, due to "
                                + e.getLocalizedMessage(), true);
            }
        } else {
            m_REAppHelper.showLog(LogLevel.INFO, "Resource Already Created...",
                    true);
        }
    }

    public void setAttributes() {
        m_REAppHelper.showLog(LogLevel.INFO, "Setting Attributes...", true);

        if (m_IsResourceCreated && m_TempActuatorRes != null) {
            if (++m_Temperature > 30)
                m_Temperature = 0;

            RcsValue tempValue = new RcsValue(m_Temperature);

            try {
                m_TempActuatorRes.setAttribute(TEMPERATURE_TEMP_KEY, tempValue);

                m_REAppHelper.showLog(LogLevel.INFO,
                        "Set Attributes Successfully.", true);
            } catch (Exception e) {
                m_REAppHelper.showLog(
                        LogLevel.ERROR,
                        "Set Attributes failed, due to "
                                + e.getLocalizedMessage(), true);
            }
        } else {
            m_REAppHelper.showLog(LogLevel.ERROR, "Create Resource First", true);
        }
    }

    public void getAttributes() {
        m_REAppHelper.showLog(LogLevel.INFO, "Getting Attributes...", true);

        if (m_IsResourceCreated && m_TempActuatorRes != null) {
            try {
                RcsResourceAttributes attrs = m_TempActuatorRes.getAttributes();

                m_REAppHelper.printAttributes(attrs);
            } catch (RcsException e) {
                m_REAppHelper.showLog(
                        LogLevel.ERROR,
                        "No Attribute found, because of "
                                + e.getLocalizedMessage(), true);
            }
        } else {
            m_REAppHelper.showLog(LogLevel.ERROR, "Create Resource First", true);
        }
    }

    public void discoverResource() {
        if (m_REAppHelper.g_IsDiscovering) {
            m_REAppHelper.showLog(LogLevel.INFO,
                    "Resource is already Discovering...", false);
        } else {
            m_REAppHelper
                    .showLog(LogLevel.INFO, "Discovering Resource...", true);

            m_Resource = null;

            ArrayList<String> uris = new ArrayList<String>();
            uris.add(BINARY_SWITCH_URI);
            uris.add(TEMPERATURE_ACTUATOR_URI);

            m_REAppHelper.discoverAllResource(uris);

            try {
                ResourceFounderTask resFounder = REAppHelper.getInstance().new ResourceFounderTask();
                resFounder.execute(String.valueOf(WAIT_TIME_MAX));

            } catch (Exception e) {
                m_REAppHelper.showLog(LogLevel.ERROR,
                        "Exception occurred while finding resource, exception is: "
                                + e.getLocalizedMessage(), true);
            }
        }
    }

    public void discoverResourceByType() {
        if (m_REAppHelper.g_IsDiscovering) {
            m_REAppHelper.showLog(LogLevel.INFO,
                    "Resource is already Discovering...", false);
        } else {
            m_REAppHelper
                    .showLog(LogLevel.INFO, "Discovering Resource...", true);

            m_Resource = null;

            ArrayList<String> uris = new ArrayList<String>();
            uris.add(TEMPERATURE_ACTUATOR_URI);

            m_REAppHelper.discoverResourceWithQuery(uris);

            try {
                ResourceFounderTask resFounder = REAppHelper.getInstance().new ResourceFounderTask();
                resFounder.execute(String.valueOf(WAIT_TIME_MAX));

            } catch (Exception e) {
                m_REAppHelper.showLog(LogLevel.ERROR,
                        "Exception occurred while finding resource, exception is: "
                                + e.getLocalizedMessage(), true);
            }
        }
    }

    public void discoverNonDiscoverableResource() {
        if (m_REAppHelper.g_IsDiscovering) {
            m_REAppHelper.showLog(LogLevel.INFO,
                    "Resource is already Discovering...", false);
        } else {
            m_REAppHelper
                    .showLog(LogLevel.INFO, "Discovering Resource...", true);

            m_Resource = null;

            ArrayList<String> uris = new ArrayList<String>();
            uris.add(TEMPERATURE_SENSOR_URI);

            m_REAppHelper.discoverAllResource(uris);

            try {
                ResourceFounderTask resFounder = REAppHelper.getInstance().new ResourceFounderTask();
                resFounder.execute(String.valueOf(WAIT_TIME_MAX));

            } catch (Exception e) {
                m_REAppHelper.showLog(LogLevel.ERROR,
                        "Exception occurred while finding resource, exception is: "
                                + e.getLocalizedMessage(), true);
            }
        }
    }

    public void startMonitoring() {
        m_REAppHelper.showLog(LogLevel.INFO, "Starting Resource Monitoring...",
                true);

        m_Resource = m_REAppHelper
                .getResourceFromSelection(TEMPERATURE_ACTUATOR_URI);

        if (m_REAppHelper.isResourceAvailable(m_Resource)) {
            try {
                if (m_Resource.isMonitoring()) {
                    m_REAppHelper.showLog(LogLevel.INFO,
                            "Monitoring already started.", true);
                } else {
                    m_Resource
                            .startMonitoring(m_REAppHelper.m_OnStateChangedListener);

                    m_REAppHelper.waitInSecond(WAIT_TIME_MIN);

                    if (m_Resource.isMonitoring()) {
                        m_REAppHelper.showLog(LogLevel.INFO,
                                "Monitoring started successfully.", false);
                    } else {
                        m_REAppHelper.showLog(LogLevel.INFO,
                                "Monitoring is not started yet.", false);
                    }
                }
            } catch (RcsException e) {
                m_REAppHelper.showLog(LogLevel.ERROR,
                        "Exception occured in startMonitoring. Exception is "
                                + e.getLocalizedMessage(), false);
            }
        }
    }

    public void stopMonitoring() {
        m_REAppHelper.showLog(LogLevel.INFO, "Stopping Resource Monitoring...",
                true);

        m_Resource = m_REAppHelper
                .getResourceFromSelection(TEMPERATURE_ACTUATOR_URI);

        if (m_REAppHelper.isResourceAvailable(m_Resource)) {
            try {
                if (m_Resource.isMonitoring()) {
                    m_Resource.stopMonitoring();

                    m_REAppHelper.waitInSecond(WAIT_TIME_MIN);

                    if (!m_Resource.isMonitoring()) {
                        m_REAppHelper.showLog(LogLevel.INFO,
                                "Monitoring stopped successfully.", false);
                    } else {
                        m_REAppHelper.showLog(LogLevel.INFO,
                                "Monitoring is not stopped yet.", false);
                    }
                } else {
                    m_REAppHelper.showLog(LogLevel.INFO,
                            "Monitoring is not started yet.", false);
                }
            } catch (RcsException e) {
                m_REAppHelper.showLog(LogLevel.ERROR,
                        "Exception occured in stopMonitoring. Exception is "
                                + e.getLocalizedMessage(), false);
            }
        }
    }

    public void getState() {
        m_REAppHelper.showLog(LogLevel.INFO, "Getting Resource State...", true);

        m_Resource = m_REAppHelper
                .getResourceFromSelection(TEMPERATURE_ACTUATOR_URI);

        if (m_REAppHelper.isResourceAvailable(m_Resource)) {
            try {
                m_REAppHelper.showLog(
                        LogLevel.INFO,
                        "Remote Resource State : "
                                + m_REAppHelper.getStringResourceState(m_Resource
                                .getState()), false);
            } catch (RcsException e) {
                m_REAppHelper.showLog(
                        LogLevel.ERROR,
                        "Exception occured in getState. Exception is "
                                + e.getLocalizedMessage(), false);
            }
        }
    }

    public void setRemoteAttributes() {
        m_REAppHelper.showLog(LogLevel.INFO, "Setting Remote Attributes...",
                true);

        m_Resource = m_REAppHelper
                .getResourceFromSelection(TEMPERATURE_ACTUATOR_URI);

        if (m_REAppHelper.isResourceAvailable(m_Resource)) {
            try {
                if (++m_Temperature > 30)
                    m_Temperature = 0;

                RcsValue tempValue = new RcsValue(m_Temperature);

                RcsResourceAttributes attrs = new RcsResourceAttributes();
                attrs.put(TEMPERATURE_TEMP_KEY, tempValue);

                m_Resource.setRemoteAttributes(attrs,
                        m_REAppHelper.m_OnRemoteAttributesReceivedListener);

                m_REAppHelper.waitInSecond(WAIT_TIME_DEFAULT);
            } catch (RcsException e) {
                m_REAppHelper.showLog(LogLevel.ERROR,
                        "Exception occured in setRemoteAttributes. Exception is "
                                + e.getLocalizedMessage(), false);
            }
        }
    }

    public void getRemoteAttributes() {
        m_REAppHelper.showLog(LogLevel.INFO, "Getting Remote Attributes", true);

        m_Resource = m_REAppHelper
                .getResourceFromSelection(TEMPERATURE_ACTUATOR_URI);

        if (m_REAppHelper.isResourceAvailable(m_Resource)) {
            try {
                m_Resource
                        .getRemoteAttributes(m_REAppHelper.m_OnRemoteAttributesReceivedListener);

                m_REAppHelper.waitInSecond(WAIT_TIME_DEFAULT);
            } catch (RcsException e) {
                m_REAppHelper.showLog(LogLevel.ERROR,
                        "Exception occured in getRemoteAttributes. Exception is "
                                + e.getLocalizedMessage(), false);
            }
        }
    }

    public void startCaching() {
        m_REAppHelper.showLog(LogLevel.INFO, "Starting Resource Caching...",
                true);

        m_Resource = m_REAppHelper
                .getResourceFromSelection(TEMPERATURE_ACTUATOR_URI);

        if (m_REAppHelper.isResourceAvailable(m_Resource)) {
            try {
                if (m_Resource.isCaching()) {
                    m_REAppHelper.showLog(LogLevel.INFO,
                            "Caching already started.", true);
                } else {
                    m_Resource
                            .startCaching(m_REAppHelper.m_OnCacheUpdatedListener);

                    m_REAppHelper.waitInSecond(WAIT_TIME_MIN);

                    if (m_Resource.isCaching()) {
                        m_REAppHelper.showLog(LogLevel.INFO,
                                "Caching started successfully.", false);
                    } else {
                        m_REAppHelper.showLog(LogLevel.INFO,
                                "Caching is not started yet.", false);
                    }
                }
            } catch (RcsException e) {
                m_REAppHelper.showLog(
                        LogLevel.ERROR,
                        "Exception occured in startCaching. Exception is "
                                + e.getLocalizedMessage(), true);
            }
        }
    }

    public void stopCaching() {
        m_REAppHelper.showLog(LogLevel.INFO, "Stopping Resource Caching...",
                true);

        m_Resource = m_REAppHelper
                .getResourceFromSelection(TEMPERATURE_ACTUATOR_URI);

        if (m_REAppHelper.isResourceAvailable(m_Resource)) {
            try {
                if (m_Resource.isCaching()) {
                    m_Resource.stopCaching();

                    m_REAppHelper.waitInSecond(WAIT_TIME_MIN);

                    if (!m_Resource.isCaching()) {
                        m_REAppHelper.showLog(LogLevel.INFO,
                                "Caching stopped successfully.", false);
                    } else {
                        m_REAppHelper.showLog(LogLevel.INFO,
                                "Caching is not stopped yet.", false);
                    }
                } else {
                    m_REAppHelper.showLog(LogLevel.INFO,
                            "Caching is not started yet.", false);
                }
            } catch (RcsException e) {
                m_REAppHelper.showLog(
                        LogLevel.ERROR,
                        "Exception occured in stopCaching. Exception is "
                                + e.getLocalizedMessage(), false);
            }
        }
    }

    public void getCachedAttributes() {
        m_REAppHelper.showLog(LogLevel.INFO, "Getting Cached Attributes", true);

        m_Resource = m_REAppHelper
                .getResourceFromSelection(TEMPERATURE_ACTUATOR_URI);

        if (m_REAppHelper.isResourceAvailable(m_Resource)) {
            try {
                if (m_Resource.isCaching()) {
                    RcsResourceAttributes attrs = m_Resource
                            .getCachedAttributes();

                    m_REAppHelper.waitInSecond(WAIT_TIME_MIN);

                    m_REAppHelper.printAttributes(attrs);
                } else {
                    m_REAppHelper.showLog(LogLevel.INFO,
                            "Caching is not started yet.", false);
                }
            } catch (RcsException e) {
                m_REAppHelper.showLog(LogLevel.ERROR,
                        "Exception occured in getCachedAttributes. Exception is "
                                + e.getLocalizedMessage(), false);
            }
        }
    }

    public void getCachedState() {
        m_REAppHelper.showLog(LogLevel.INFO, "Getting Cached State", true);

        m_Resource = m_REAppHelper
                .getResourceFromSelection(TEMPERATURE_ACTUATOR_URI);

        if (m_REAppHelper.isResourceAvailable(m_Resource)) {
            try {
                m_REAppHelper.showLog(
                        LogLevel.INFO,
                        "Cached State : "
                                + m_REAppHelper.getStringCacheState(m_Resource
                                .getCacheState()), false);
            } catch (RcsException e) {
                m_REAppHelper.showLog(LogLevel.ERROR,
                        "Exception occured in getCachedState. Exception is "
                                + e.getLocalizedMessage(), false);
            }
        }
    }

    public void exitApplication() {
        m_REAppHelper.showLog(LogLevel.INFO, "Quit from RETestApp...", true);

        m_REAppHelper.waitInSecond(WAIT_TIME_MIN);

        m_TempActuatorRes = null;

        m_Resource = null;
        m_REAppHelper = null;
        m_IsResourceCreated = false;

        android.os.Process.killProcess(android.os.Process.myPid());
    }
}