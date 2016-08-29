/******************************************************************
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 * <p/>
 * <p/>
 * <p/>
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * <p/>
 * http://www.apache.org/licenses/LICENSE-2.0
 * <p/>
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************/

package org.iotivity.service.ri;

import android.content.Context;

import org.iotivity.base.ModeType;
import org.iotivity.base.ObserveType;
import org.iotivity.base.OcConnectivityType;
import org.iotivity.base.OcException;
import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcPlatform.OnDeviceFoundListener;
import org.iotivity.base.OcPlatform.OnPlatformFoundListener;
import org.iotivity.base.OcPlatform.OnResourceFoundListener;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResource;
import org.iotivity.base.OcResource.OnDeleteListener;
import org.iotivity.base.OcResource.OnGetListener;
import org.iotivity.base.OcResource.OnObserveListener;
import org.iotivity.base.OcResource.OnPostListener;
import org.iotivity.base.OcResource.OnPutListener;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;
import org.iotivity.common.ResourceConstants;
import org.iotivity.common.ResourceHelper;
import org.iotivity.resource.ResourceServer;
import org.iotivity.resource.SampleResource;
import org.iotivity.service.testapp.framework.Base;

import java.util.EnumSet;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Vector;

public class RITestAppAction extends Base {

    private static final String DEFAULT_IP = "0.0.0.0";
    private static final int DEFAULT_PORT = 0;

    private Vector<OcResource> m_foundResourceList;
    private Vector<SampleResource> m_createdResourceList;
    private ResourceHelper m_resourceHelper;
    private boolean m_hasCallbackArrived = false;
    private boolean m_isSecuredServer = false;
    private boolean m_isSecuredClient = false;
    private boolean m_isAirConDeviceCreated = false;
    private boolean m_isObservingResource = false;
    private boolean m_hasUserResponded = false;
    private Context m_appContext;
    private String m_serverDBPath = "";
    private String m_clientDBPath = "";
    private String m_latestUserInput = "";

    private SampleResource m_acSwitchResource;
    private SampleResource m_acTemperatureResource;
    private SampleResource m_acAirFlowResource;
    private SampleResource m_acTimerResource;
    private SampleResource m_acChildLockResource;

    private OcConnectivityType m_connectivityType = OcConnectivityType.CT_ADAPTER_IP;
    private QualityOfService m_qos = QualityOfService.HIGH;

    public RITestAppAction(Context context) {
        m_appContext = context;
        m_createdResourceList = new Vector<SampleResource>();
        m_foundResourceList = new Vector<OcResource>();
        m_resourceHelper = ResourceHelper.getInstance();
        m_serverDBPath = m_appContext.getFilesDir().getPath() + "/" + ResourceConstants.SERVER_DATABASE_FILE_NAME;
        m_clientDBPath = m_appContext.getFilesDir().getPath() + "/" + ResourceConstants.CLIENT_DATABASE_FILE_NAME;
    }

    public void exitApplication() {
        Base.showOutPut("Quit from RITestApp...");

        m_resourceHelper.waitInSecond(ResourceConstants.CALLBACK_WAIT_MIN);

        android.os.Process.killProcess(android.os.Process.myPid());
    }

    OnResourceFoundListener onResourceFound = new OnResourceFoundListener() {

        @Override
        public void onResourceFound(OcResource resource) {
            if (resource != null) {
                m_foundResourceList.add(resource);
                Base.showOutPut("uri of the found resource is " + resource.getUri());
                m_hasCallbackArrived = true;

                Base.showOutPut("Host of found resource: " + resource.getHost());
                Base.showOutPut("di( OCResource.sid() ) of found resource is = " + resource.getServerId());
                Base.showOutPut("unique identifier of found resource is = " + resource.getUniqueIdentifier());
            } else {
                Base.showOutPut("Found resource is invalid!!");
            }

        }

        @Override
        public void onFindResourceFailed(Throwable exception, String message) {
            Base.showOutPut("Failed to discover resource, reason: " + message);

        }
    };

    OnPlatformFoundListener onPlatformInfoReceived = new OnPlatformFoundListener() {

        @Override
        public void onPlatformFound(OcRepresentation rep) {
            Base.showOutPut("\nPlatform Information received ---->");
            String value = "";
            String values[] =
                    {"pi", "Platform ID                    ", "mnmn", "Manufacturer name              ", "mnml",
                            "Manufacturer url               ", "mnmo", "Manufacturer Model No          ", "mndt",
                            "Manufactured Date              ", "mnpv", "Manufacturer Platform Version  ", "mnos",
                            "Manufacturer OS version        ", "mnhw", "Manufacturer hardware version  ", "mnfv",
                            "Manufacturer firmware version  ", "mnsl", "Manufacturer support url       ", "st",
                            "Manufacturer system time       "};

            for (int i = 0; i < values.length; i += 2) {
                if (rep.hasAttribute(values[i])) {
                    try {
                        value = rep.getValue(values[i]);
                    } catch (OcException e) {
                        e.printStackTrace();
                        Base.showOutPut("Failed to get value from Representation : " + e.getMessage());
                    }
                    Base.showOutPut(values[i + 1] + " : " + value);
                }
            }
            m_resourceHelper.printRepresentation(rep);
            m_hasCallbackArrived = true;

        }
    };

    OnDeviceFoundListener onDeviceInfoReceived = new OnDeviceFoundListener() {

        @Override
        public void onDeviceFound(OcRepresentation rep) {
            Base.showOutPut("\nDevice Information received ---->");
            String value = "";
            String values[] =
                    {"di", "Device ID        ", "n", "Device name      ", "lcv", "Spec version url ", "dmv",
                            "Data Model Model ",};

            for (int i = 0; i < values.length; i += 2) {
                if (rep.hasAttribute(values[i])) {
                    try {
                        value = rep.getValue(values[i]);
                    } catch (OcException e) {
                        e.printStackTrace();
                        Base.showOutPut("Failed to get value from Representation : " + e.getMessage());
                    }
                    Base.showOutPut(values[i + 1] + " : " + value);
                }
            }

            m_resourceHelper.printRepresentation(rep);
            m_hasCallbackArrived = true;

        }
    };

    OnGetListener onGet = new OnGetListener() {

        @Override
        public void onGetFailed(Throwable eCode) {

            Base.showOutPut("onGET Response error: " + eCode.getMessage());
        }

        @Override
        public void onGetCompleted(List<OcHeaderOption> headerOptions, OcRepresentation rep) {
            Base.showOutPut("Response: GET request was successful");

            m_resourceHelper.printRepresentation(rep);
            List<String> interfacelist = rep.getResourceInterfaces();

            boolean isCollection = false;
            for (String resourceInterface : interfacelist) {
                if (resourceInterface.equals(OcPlatform.GROUP_INTERFACE)
                        || resourceInterface.equals(OcPlatform.BATCH_INTERFACE)) {
                    isCollection = true;
                    break;
                }
            }

            if (isCollection) {
                List<OcRepresentation> children = rep.getChildren();

                Base.showOutPut("\nCHILD RESOURCE OF GROUP");
                for (OcRepresentation child : children) {
                    Base.showOutPut("\tURI :: " + child.getUri());
                }
            } else {
                Base.showOutPut("THe GET Response has the following representation:");
                m_resourceHelper.printRepresentation(rep);
            }
        }
    };

    OnPutListener onPut = new OnPutListener() {

        @Override
        public void onPutFailed(Throwable eCode) {
            Base.showOutPut("onPUT Response error: " + eCode.getMessage());
        }

        @Override
        public void onPutCompleted(List<OcHeaderOption> headerOptions, OcRepresentation rep) {
            Base.showOutPut("Response: PUT request was successful");
            Base.showOutPut("THe PUT response has the following representation:");
            m_resourceHelper.printRepresentation(rep);
            m_hasCallbackArrived = true;
        }
    };

    OnPostListener onPost = new OnPostListener() {

        @Override
        public void onPostFailed(Throwable eCode) {
            Base.showOutPut("onPOST Response error: " + eCode.getMessage());
        }

        @Override
        public void onPostCompleted(List<OcHeaderOption> headerOptions, OcRepresentation rep) {
            Base.showOutPut("Response: POST request was successful");
            Base.showOutPut("THe POST Response has the following representation:");
            m_resourceHelper.printRepresentation(rep);
            m_hasCallbackArrived = true;
        }
    };


    OnDeleteListener onDelete = new OnDeleteListener() {

        @Override
        public void onDeleteFailed(Throwable eCode) {
            Base.showOutPut("onDELETE Response error: " + eCode.getMessage());
        }

        @Override
        public void onDeleteCompleted(List<OcHeaderOption> headerOptions) {
            Base.showOutPut("Response: DELETE request was successful");
            m_hasCallbackArrived = true;
        }
    };

    OnObserveListener onObserve = new OnObserveListener() {

        @Override
        public void onObserveFailed(Throwable eCode) {
            Base.showOutPut("Observe Response/Notification Error: " + eCode.getMessage());
        }

        @Override
        public void onObserveCompleted(List<OcHeaderOption> arg0, OcRepresentation rep, int sequenceNumber) {
            Base.showOutPut("OBSERVE RESULT:");
            Base.showOutPut("\tSequenceNumber: " + sequenceNumber);
            m_resourceHelper.printRepresentation(rep);
            m_hasCallbackArrived = true;
        }
    };

    void waitForCallback() {
        int elapsedSecond = 0;
        while (m_hasCallbackArrived == false) {
            m_resourceHelper.waitInSecond(ResourceConstants.CALLBACK_WAIT_MIN);
            elapsedSecond++;
            if (elapsedSecond > ResourceConstants.CALLBACK_WAIT_MAX) {
                break;
            }
        }
    }

    boolean initiateServer() {
        boolean result = false;
        if (m_isSecuredClient) {
            PlatformConfig cfg = new PlatformConfig(m_appContext, ServiceType.IN_PROC, ModeType.CLIENT_SERVER, DEFAULT_IP, DEFAULT_PORT, m_qos, m_clientDBPath);

            result = SampleResource.constructServer(cfg);
        } else if (m_isSecuredServer) {
            PlatformConfig cfg = new PlatformConfig(m_appContext, ServiceType.IN_PROC, ModeType.CLIENT_SERVER, DEFAULT_IP, DEFAULT_PORT, m_qos, m_serverDBPath);

            result = SampleResource.constructServer(cfg);
        } else {
            PlatformConfig cfg = new PlatformConfig(m_appContext, ServiceType.IN_PROC, ModeType.CLIENT_SERVER, DEFAULT_IP, DEFAULT_PORT, m_qos);
            result = SampleResource.constructServer(cfg);
        }

        if (result == false) {
            Base.showOutPut("Unable to start Iotivity servers");
        } else {
            Base.showOutPut("Iotivity Server started successfully");
        }

        return result;
    }

    public void createAirConDevice(boolean isSecured) {

        boolean result = false;
        if (m_isAirConDeviceCreated == false) {
            Base.showOutPut("Creating AirCon Device Resources!!");
            ResourceServer.setDeviceInfo("Vendor Smart Home AirCon Device", ResourceConstants.DEVICE_TYPE_AC);

            m_acSwitchResource = new SampleResource();
            m_acSwitchResource.setResourceProperties(ResourceConstants.AC_SWITCH_URI, ResourceConstants.SWITCH_RESOURCE_TYPE,
                    ResourceConstants.SWITCH_RESOURCE_INTERFACE);
            m_acSwitchResource.setAsDiscoverableResource();
            m_acSwitchResource.setAsObservableResource();
            if (isSecured) {
                m_acSwitchResource.setAsSecuredResource();
            }

            OcRepresentation switchRep = new OcRepresentation();
            String key = ResourceConstants.BINARY_SWITCH_KEY;
            String value = "";
            double[] rangeTemperature = {ResourceConstants.TEMPERATURE_RANGE_START_VALUE, ResourceConstants.TEMPERATURE_RANGE_END_VALUE};
            try {
                switchRep.setValue(key, ResourceConstants.BINARY_SWITCH_VALUE);
            } catch (OcException e) {
                e.printStackTrace();
                Base.showOutPut("Failed to set value to Representation : " + e.getMessage());
            }
            m_acSwitchResource.setResourceRepresentation(switchRep);

            result = m_acSwitchResource.startResource();

            if (result == true) {
                Base.showOutPut("AirConditioner Binary Switch Resource created successfully");
                m_createdResourceList.add(m_acSwitchResource);
                m_isAirConDeviceCreated = true;
            } else {
                Base.showOutPut("Unable to create AirConditioner Binary Switch resource");
            }

            m_acTemperatureResource = new SampleResource();
            m_acTemperatureResource.setResourceProperties(ResourceConstants.AC_TEMPERATURE_URI,
                    ResourceConstants.TEMPERATURE_RESOURCE_TYPE,
                    ResourceConstants.TEMPERATURE_RESOURCE_INTERFACE);
            m_acTemperatureResource.setAsDiscoverableResource();
            m_acTemperatureResource.setAsObservableResource();
            if (isSecured) {
                m_acTemperatureResource.setAsSecuredResource();
            }
            OcRepresentation temperatureRep = new OcRepresentation();

            try {
                value = ResourceConstants.TEMPERATURE_UNITS_SUPPORTED_VALUE;
                key = ResourceConstants.TEMPERATURE_UNITS_KEY;
                temperatureRep.setValue(key, value);
                m_acTemperatureResource.setAsReadOnly(key);
                key = ResourceConstants.TEMPERATURE_RANGE_KEY;
                temperatureRep.setValue(key, rangeTemperature);
                m_acTemperatureResource.setAsReadOnly(key);
                double temperature = ResourceConstants.TEMPERATURE_VALUE;
                key = ResourceConstants.TEMPERATURE_KEY;
                temperatureRep.setValue(key, temperature);
            } catch (OcException e) {
                e.printStackTrace();
                Base.showOutPut("Failed to set value to Representation : " + e.getMessage());
            }
            m_acTemperatureResource.setResourceRepresentation(temperatureRep);
            result = m_acTemperatureResource.startResource();

            if (result == true) {
                Base.showOutPut("Air Conditioner Temperature Resource created successfully");
                m_createdResourceList.add(m_acTemperatureResource);
                m_isAirConDeviceCreated = true;
            } else {
                Base.showOutPut("Unable to create Air Conditioner Temperature resource");
            }

            m_acAirFlowResource = new SampleResource();
            m_acAirFlowResource.setResourceProperties(ResourceConstants.AC_AIR_FLOW_URI, ResourceConstants.AIR_FLOW_RESOURCE_TYPE,
                    ResourceConstants.AIR_FLOW_RESOURCE_INTERFACE);
            m_acAirFlowResource.setAsDiscoverableResource();
            m_acAirFlowResource.setAsObservableResource();
            if (isSecured) {
                m_acAirFlowResource.setAsSecuredResource();
            }

            OcRepresentation airFlowRep = new OcRepresentation();

            try {
                int speed = ResourceConstants.AIRFLOW_SPEED_VALUE;
                value = ResourceConstants.AIRFLOW_DIRECTION_VALUE;
                key = ResourceConstants.AIRFLOW_DIRECTION_KEY;
                airFlowRep.setValue(key, value);
                key = ResourceConstants.AIRFLOW_SPEED_KEY;
                airFlowRep.setValue(key, speed);
                int[] range = {ResourceConstants.AIRFLOW_SPEED_RANGE_START_VALUE, ResourceConstants.AIRFLOW_SPEED_RANGE_END_VALUE};
                key = ResourceConstants.AIRFLOW_RANGE_KEY;
                airFlowRep.setValue(key, range);
            } catch (OcException e) {
                e.printStackTrace();
                Base.showOutPut("Failed to set value to Representation : " + e.getMessage());
            }
            m_acAirFlowResource.setAsReadOnly(key);
            m_acAirFlowResource.setResourceRepresentation(airFlowRep);

            result = m_acAirFlowResource.startResource();

            if (result == true) {
                Base.showOutPut("Air Conditioner AirFlow Resource created successfully");
                m_createdResourceList.add(m_acAirFlowResource);
                m_isAirConDeviceCreated = true;
            } else {
                Base.showOutPut("Unable to create Air Conditioner AirFlow resource");
            }

            m_acTimerResource = new SampleResource();
            m_acTimerResource.setResourceProperties(ResourceConstants.AC_TIMER_URI, ResourceConstants.TIMER_RESOURCE_TYPE,
                    ResourceConstants.TIMER_RESOURCE_INTERFACE);
            m_acTimerResource.setAsDiscoverableResource();
            if (isSecured) {
                m_acTimerResource.setAsSecuredResource();
            }

            OcRepresentation clockRep = new OcRepresentation();

            try {
                key = ResourceConstants.TIMER_HOUR_KEY;
                clockRep.setValue(key, ResourceConstants.TIMER_HOUR_VALUE);
                key = ResourceConstants.TIMER_MINUTE_KEY;
                clockRep.setValue(key, ResourceConstants.TIMER_MINUTE_VALUE);
                key = ResourceConstants.TIMER_SECOND_KEY;
                clockRep.setValue(key, ResourceConstants.TIMER_SECOND_VALUE);
                key = ResourceConstants.TIMER_RESET_KEY;
                clockRep.setValue(key, ResourceConstants.TIMER_RESET_VALUE);
            } catch (OcException e) {
                e.printStackTrace();
                Base.showOutPut("Failed to set value to Representation : " + e.getMessage());
            }

            m_acTimerResource.setResourceRepresentation(clockRep);

            result = m_acTimerResource.startResource();
            m_acTimerResource.setAsSlowResource();

            if (result == true) {
                Base.showOutPut("Air Conditioner Timer Resource created successfully");
                m_createdResourceList.add(m_acTimerResource);
                m_isAirConDeviceCreated = true;
            } else {
                Base.showOutPut("Unable to create Air Conditioner Timer resource");
            }

            m_acChildLockResource = new SampleResource();
            m_acChildLockResource.setResourceProperties(ResourceConstants.AC_CHILD_LOCK_URI, ResourceConstants.CHILD_LOCK_RESOURCE_TYPE,
                    ResourceConstants.CHILD_LOCK_RESOURCE_INTERFACE);
            m_acChildLockResource.setAsObservableResource();
            if (isSecured) {
                m_acChildLockResource.setAsSecuredResource();
            }

            OcRepresentation childLockRep = new OcRepresentation();
            key = ResourceConstants.CHILD_LOCK_KEY;
            try {
                childLockRep.setValue(key, ResourceConstants.CHILD_LOCK_VALUE);
            } catch (OcException e) {
                e.printStackTrace();
                Base.showOutPut("Failed to set value to Representation : " + e.getMessage());
            }
            m_acChildLockResource.setResourceRepresentation(childLockRep);

            result = m_acChildLockResource.startResource();

            if (result == true) {
                Base.showOutPut("Air Conditioner Timer Resource created successfully");
                m_createdResourceList.add(m_acChildLockResource);
                m_isAirConDeviceCreated = true;
            } else {
                Base.showOutPut("Unable to create Air Conditioner Timer resource");
            }
        } else {
            Base.showOutPut("Already Smart Home AirCon Device Resources are  created!!");
        }

    }

    public void deleteResource() {
        if (m_createdResourceList.size() != 0) {

            for (SampleResource resource : m_createdResourceList) {
                if (resource.stopResource()) {
                    Base.showOutPut("Successfully stopped Resource with URI: "
                            + resource.getUri());
                }
            }
        } else {
            Base.showOutPut("There is no resource available to delete!!");
        }

        m_createdResourceList.clear();
        m_isAirConDeviceCreated = false;

    }

    public void findResource(String resourceType, String host) {
        m_foundResourceList.clear();
        String requestURI;

        try {
            // Find all resources
            requestURI = OcPlatform.WELL_KNOWN_QUERY + "?rt=" + resourceType;

            OcPlatform.findResource(host, requestURI, EnumSet.of(m_connectivityType),
                    onResourceFound, m_qos);
            Base.showOutPut("Finding Resource....");

        } catch (OcException e) {
            Base.showOutPut("Unable to find resource, exception Occured: " + e.getMessage());
        }
        waitForCallback();
    }

    public void findAllResources() {
        String host = "";
        String query = "";
        findAllResources(host, query);
    }

    public void findAllResources(String host, String query) {
        m_foundResourceList.clear();
        String requestURI = "";

        try {
            // Find all resources
            if (query.isEmpty()) {
                requestURI = OcPlatform.WELL_KNOWN_QUERY;
            } else {
                requestURI = OcPlatform.WELL_KNOWN_QUERY + "?" + query;
            }

            OcPlatform.findResource(host, requestURI, EnumSet.of(m_connectivityType),
                    onResourceFound, m_qos);
            Base.showOutPut("Finding Resource....");

        } catch (OcException e) {
            Base.showOutPut("Unable to find resource, exception Occured: " + e.getMessage());
        }
        waitForCallback();
    }

    public void discoverDevice(boolean isMulticast) {

        String host = "";
        String deviceDiscoveryRequest = "";

        if (isMulticast) {
            deviceDiscoveryRequest = OcPlatform.WELL_KNOWN_DEVICE_QUERY;
            Base.showOutPut("Discovering Device using Multicast... ");
        } else {
            if (m_foundResourceList.isEmpty()) {
                Base.showOutPut("Finding OIC Servers to send Unicast Discovery Request");
                findAllResources();
            }

            m_hasCallbackArrived = false;
            if (m_foundResourceList.size() > 0) {
                host = m_foundResourceList.get(0).getHost();
                deviceDiscoveryRequest = OcPlatform.WELL_KNOWN_DEVICE_QUERY;
                Base.showOutPut("Discovering Device using Unicast... ");
                Base.showOutPut("Sending Unicast device discovery to the host: " + host);
            } else {
                Base.showOutPut("No server found to send Unicast device discovery");
                Base.showOutPut("Sending Multicast Device discovery request...");
                deviceDiscoveryRequest = OcPlatform.WELL_KNOWN_DEVICE_QUERY;
            }
        }

        try {
            OcPlatform.getDeviceInfo(host, deviceDiscoveryRequest,
                    EnumSet.of(m_connectivityType), onDeviceInfoReceived);

            Base.showOutPut("Device discovery done successfully");
            waitForCallback();

        } catch (OcException e) {
            Base.showOutPut("Exception occurred while discovering device, reason is: " + e.getMessage());
        }
    }

    public void discoverPlatform(boolean isMulticast) {
        String host = "";
        String platformDiscoveryRequest;
        String platformDiscoveryURI = OcPlatform.WELL_KNOWN_PLATFORM_QUERY;

        if (isMulticast) {
            platformDiscoveryRequest = platformDiscoveryURI;
            Base.showOutPut("Discovering Platform using Multicast... ");

        } else {
            if (m_foundResourceList.isEmpty()) {
                findAllResources();
                m_hasCallbackArrived = false;
            }

            host = m_foundResourceList.get(0).getHost();
            platformDiscoveryRequest = platformDiscoveryURI;
            Base.showOutPut("Discovering Platform using Unicast... ");
        }

        try {
            OcPlatform.getPlatformInfo(host, platformDiscoveryRequest,
                    EnumSet.of(m_connectivityType), onPlatformInfoReceived);

            Base.showOutPut("Platform discovery ");
            waitForCallback();

        } catch (OcException e) {
            Base.showOutPut("Failure in main thread: " + e.getMessage());
        }
    }

    public void sendGetRequest() {
        int selection = selectResource();
        if (selection != -1) {
            Map<String, String> qpMap = new HashMap<String, String>();
            OcResource targetResource = m_foundResourceList.get(selection);
            Base.showOutPut("Sending Get Request to the resource with: " + targetResource.getHost()
                    + targetResource.getUri());
            try {
                targetResource.get(qpMap, onGet, m_qos);
            } catch (OcException e) {
                Base.showOutPut("Unable to send GET request, Exception Occured!! reason: " + e.getMessage());
            }
            Base.showOutPut("GET request sent!!");
            waitForCallback();

        } else {
            Base.showOutPut("No resource to send GET!!");
        }
    }

    public void sendGetRequestWithQuery(String key, String value) {
        int selection = selectResource();
        if (selection != -1) {
            Map<String, String> qpMap = new HashMap<String, String>();
            qpMap.put(key, value);
            try {
                m_foundResourceList.get(selection).get(qpMap, onGet, m_qos);
            } catch (OcException e) {
                Base.showOutPut("Unable to send GET request, Exception Occured!! reason: " + e.getMessage());
            }
            Base.showOutPut("GET request sent!!");
            waitForCallback();

        } else {
            Base.showOutPut("No resource to send GET!!");
        }
    }

    public void sendPutRequestUpdate() {
        int selection = selectResource();
        if (selection != -1) {
            OcRepresentation rep = new OcRepresentation();

            Base.showOutPut("Sending Complete Update Message(PUT)...");

            try {
                String key = ResourceConstants.REGION_KEY;
                String value = ResourceConstants.DEFAULT_REGION;
                rep.setValue(key, value);
                key = ResourceConstants.POWER_KEY;
                value = ResourceConstants.DEFAULT_POWER_STATE;
                rep.setValue(key, value);
                key = ResourceConstants.INTENSITY_KEY;
                int intensity = ResourceConstants.DEFAULT_INTENSITY;
                rep.setValue(key, intensity);
                key = ResourceConstants.MANUFACTURER_KEY;
                value = ResourceConstants.DEFAULT_MANUFACTURER;
                rep.setValue(key, value);
            } catch (OcException e) {
                Base.showOutPut("Unable to set representation, Exception Occured!! reason: " + e.getMessage());
            }

            // Invoke resource's put API with rep, query map and the callback parameter
            Map<String, String> qpMap = new HashMap<String, String>();
            try {
                m_foundResourceList.get(selection).put(rep, qpMap, onPut, m_qos);
            } catch (OcException e) {
                Base.showOutPut("Unable to send PUT request, Exception Occured!! reason: " + e.getMessage());
            }
            Base.showOutPut("PUT request sent!!");
            waitForCallback();

        } else {
            Base.showOutPut("No resource to send PUT!!");
        }
    }

    public void sendPutRequestCreate() {
        int selection = selectResource();
        if (selection != -1) {
            OcRepresentation rep = new OcRepresentation();

            Base.showOutPut("Sending Create Resource Message(PUT)...");


            try {
                Vector<String> resourceTypes = new Vector<String>();
                String key = ResourceConstants.URI_KEY;
                String value = ResourceConstants.NEW_RESOURCE_URI;
                rep.setUri(value);
                value = ResourceConstants.SWITCH_RESOURCE_TYPE;
                resourceTypes.add(value);
                rep.setResourceTypes(resourceTypes);
                key = ResourceConstants.BINARY_SWITCH_KEY;
                rep.setValue(key, ResourceConstants.BINARY_SWITCH_VALUE);
            } catch (OcException e) {
                Base.showOutPut("Unable to set representation, Exception Occured!! reason: " + e.getMessage());
            }

            // Invoke resource's post API with rep, query map and the callback parameter
            Map<String, String> qpMap = new HashMap<String, String>();
            try {
                m_foundResourceList.get(selection).put(rep, qpMap, onPut, m_qos);
            } catch (OcException e) {
                Base.showOutPut("Unable to send PUT request, Exception Occured!! reason: " + e.getMessage());
            }
            Base.showOutPut("PUT request sent!!");
            waitForCallback();

        } else {
            Base.showOutPut("No resource to send PUT!!");
        }
    }

    public OcRepresentation setAttributeValueFromUser(OcRepresentation rep, String key) {
        //unblock later
        String valueString = "";
        boolean valueBool = false;
        int valueInt = 0;
        float valueFloat = (float) 0.0;
        double valueDouble = 0.0;
        int choice = 0;
        boolean validChoice = false;

        do {
            Base.showOutPut("Please select attribute data type and press Enter: ");
            Base.showOutPut("\t\t 1. Integer");
            Base.showOutPut("\t\t 2. Floating Point - Single Precision");
            Base.showOutPut("\t\t 3. Floating Point - Double Precision");
            Base.showOutPut("\t\t 4. Boolean");
            Base.showOutPut("\t\t 5. String");
            choice = (int) Integer.valueOf(waitAndGetInputFromUser());

            if (choice > 0 && choice < 5) {
                validChoice = true;
            } else {
                validChoice = false;
                Base.showOutPut("Invalid input for attribute data type. Please select between 1 and 5");
            }

        } while (!validChoice);

        Base.showOutPut("Please input Attribute Value: ");

        try {
            switch (choice) {
                case 1:
                    valueInt = (int) Integer.valueOf(waitAndGetInputFromUser());
                    rep.setValue(key, valueInt);
                    break;
                case 2:
                    valueFloat = (float) Float.valueOf(waitAndGetInputFromUser());
                    rep.setValue(key, valueFloat);
                    break;
                case 3:
                    valueDouble = (double) Double.valueOf(waitAndGetInputFromUser());
                    rep.setValue(key, valueDouble);
                    break;
                case 4:
                    Base.showOutPut("Please provide boolean value(O for False, 1 for True) : ");
                    valueBool = (boolean) Boolean.valueOf(waitAndGetInputFromUser());
                    rep.setValue(key, valueBool);
                    break;
                case 5:
                    valueString = waitAndGetInputFromUser();
                    rep.setValue(key, valueString);
                    break;
            }
        } catch (OcException e) {
            Base.showOutPut("Unable to set representation, exception occurred: " + e.getMessage());
        }

        return rep;
    }

    public void updateLocalResource() {
        int selection = selectLocalResource();
        if (selection != -1) {
            String key = "";

            Base.showOutPut("Please input Attribute Key: ");
            key = waitAndGetInputFromUser();

            OcRepresentation rep = m_createdResourceList.get(selection).getRepresentation();

            if (rep.hasAttribute(key)) {
                rep = setAttributeValueFromUser(rep, key);
                m_createdResourceList.get(selection).setResourceRepresentation(rep);
                Base.showOutPut("Successfully updated resource attribute!!");
                m_resourceHelper.printRepresentation(rep);
                m_createdResourceList.get(selection).notifyObservers(
                        m_createdResourceList.get(selection));
            } else {
                Base.showOutPut("The resource does not have the mentioned attribute");
            }

        } else {
            Base.showOutPut("No resource to Update!!");
        }
    }

    public void sendPostRequestUpdateUserInput() {
        int selection = selectResource();
        if (selection != -1) {
            OcRepresentation rep = new OcRepresentation();
            String key = "";

            Base.showOutPut("Please input Attribute Key: ");
            key = waitAndGetInputFromUser();
            rep = setAttributeValueFromUser(rep, key);

            // Invoke resource's put API with rep, query map and the callback parameter
            Base.showOutPut("Sending Partial Update Message(POST)...");
            Map<String, String> qpMap = new HashMap<String, String>();
            try {
                m_foundResourceList.get(selection).post(rep, qpMap, onPost, m_qos);
            } catch (OcException e) {
                Base.showOutPut("Unbable tosend POST request, Exception occurred: " + e.getMessage());
            }
            Base.showOutPut("POST request sent!!");
            waitForCallback();

        } else {
            Base.showOutPut("No resource to send POST!!");
        }
    }

    public void sendPostRequestCreate() {
        int selection = selectResource();
        if (selection != -1) {
            OcRepresentation rep = new OcRepresentation();

            Base.showOutPut("Sending Subordinate Resource Create Message(POST)...");

            Vector<String> resourceTypes = new Vector<String>();
            String key = ResourceConstants.URI_KEY;
            String value = ResourceConstants.NEW_RESOURCE_URI;
            rep.setUri(value);
            value = ResourceConstants.SWITCH_RESOURCE_TYPE;
            resourceTypes.add(value);
            rep.setResourceTypes(resourceTypes);
            key = ResourceConstants.BINARY_SWITCH_KEY;
            boolean attrValue = ResourceConstants.BINARY_SWITCH_VALUE;
            try {
                rep.setValue(key, attrValue);
            } catch (OcException e) {
                Base.showOutPut("Unbable to set representation, Exception occurred: " + e.getMessage());
            }

            // Invoke resource's post API with rep, query map and the callback parameter
            try {
                m_foundResourceList.get(selection).post(rep, new HashMap<String, String>(), onPost, m_qos);
            } catch (OcException e) {
                Base.showOutPut("Unbable to send POST request, Exception occurred: " + e.getMessage());
            }
            Base.showOutPut("POST request sent!!");
            waitForCallback();

        } else {
            Base.showOutPut("No resource to send POST!!");
        }
    }

    public void sendDeleteRequest() {
        int selection = selectResource();
        if (selection != -1) {
            Base.showOutPut("Sending Delete Request...");

            // Invoke resource's delete API with the callback parameter
            OcResource resource = m_foundResourceList.get(selection);
            try {
                resource.deleteResource(onDelete);
            } catch (OcException e) {
                Base.showOutPut("Unbable to send DELETE request, Exception occurred: " + e.getMessage());
            }
            Base.showOutPut("DELETE request sent!!");
            waitForCallback();
        } else {
            Base.showOutPut("No resource to send DELETE!!");
        }
    }

    public void observeResource() {
        int selection = selectResource();
        if (selection != -1) {
            Base.showOutPut("Observing resource...");

            OcResource resource = m_foundResourceList.get(selection);
            try {
                resource.observe(ObserveType.OBSERVE, new HashMap<String, String>(), onObserve, m_qos);
            } catch (OcException e) {
                Base.showOutPut("Unbable to Observe resource, Exception occurred: " + e.getMessage());
            }
            Base.showOutPut("Observe request sent!!");
            m_isObservingResource = true;
            waitForCallback();

        } else {
            Base.showOutPut("No resource to Observe!!");
        }
    }

    public void cancelObserveResource() {
        int selection = selectResource();
        if (selection != -1) {
            if (m_isObservingResource) {
                Base.showOutPut("Canceling Observe resource...");

                OcResource resource = m_foundResourceList.get(selection);
                try {
                    resource.cancelObserve(m_qos);
                } catch (OcException e) {
                    Base.showOutPut("Unbable to Cancel Observe resource, Exception occurred: " + e.getMessage());
                }
                Base.showOutPut("Cancel Observe request sent!!");
                m_isObservingResource = false;
            } else {
                Base.showOutPut("No resource is being Observed currently!!");
            }
        } else {
            Base.showOutPut("No resource to cancel Observe!!");
        }
    }

    public void cancelObservePassively() {
        int selection = selectResource();
        if (selection != -1) {
            if (m_isObservingResource) {
                Base.showOutPut("Canceling Observe passively...");

                // Currently, there is no api to cancel observe passively
                OcResource resource = m_foundResourceList.get(selection);
                Base.showOutPut("Cancel Observe request not sent!! Currently there is no API!!");
            } else {
                Base.showOutPut("No resource is being Observed currently!!");
            }
        } else {
            Base.showOutPut("No resource to cancel Observe!!");
        }
    }


    int selectResource() {
        int selection = -1;
        int totalResource = m_foundResourceList.size();
        if (totalResource > 0) {
            Base.showOutPut("\tPlease select your desired resource no. to send request and press Enter:");

            for (int i = 1; i <= totalResource; i++) {
                Base.showOutPut("\t\t" + i + ". " + m_foundResourceList.get(i - 1).getUniqueIdentifier());
            }

            selection = (int) Integer.valueOf(waitAndGetInputFromUser());

            while (selection < 1 || selection > totalResource) {
                Base.showOutPut("Invalid selection of resource. Please select a resource no. between 1 & "
                        + totalResource);
                selection = (int) Integer.valueOf(waitAndGetInputFromUser());
            }
            selection--;
        }

        return selection;
    }

    int selectLocalResource() {
        int selection = -1;
        int totalResource = m_createdResourceList.size();
        if (totalResource > 0) {
            Base.showOutPut("\tPlease select your desired resource no. to update attribute:");

            int resourceCount = 1;
            for (SampleResource localResource : m_createdResourceList) {
                Base.showOutPut("\t\t" + resourceCount++ + ". " + localResource.getUri());
            }

            selection = (int) Integer.valueOf(waitAndGetInputFromUser());

            while (selection < 1 || selection > totalResource) {
                Base.showOutPut("Invalid selection of resource. Please select a resource no. between 1 & "
                        + totalResource);
                selection = (int) Integer.valueOf(waitAndGetInputFromUser());
            }
            selection--;
        }

        return selection;
    }

    public void getUserInput(String userArgument) {
        m_latestUserInput = userArgument;
        m_hasUserResponded = true;
    }

    String waitAndGetInputFromUser() {
        while (m_hasUserResponded == false) {
            m_resourceHelper.waitInSecond(ResourceConstants.CALLBACK_WAIT_MIN);
        }

        m_hasUserResponded = false;
        return m_latestUserInput;
    }

    public void setLowQoS() {
        m_qos = QualityOfService.LOW;
    }

    public void setHighQoS() {
        m_qos = QualityOfService.HIGH;
    }

    public void setIPv4() {
        m_connectivityType = OcConnectivityType.CT_IP_USE_V4;
        initiateServer();
    }

    public void setIPv6() {
        m_connectivityType = OcConnectivityType.CT_IP_USE_V6;
        initiateServer();
    }

    public void setIPBoth() {
        m_connectivityType = OcConnectivityType.CT_ADAPTER_IP;
        initiateServer();
    }

    public void setBLE() {
        m_connectivityType = OcConnectivityType.CT_ADAPTER_GATT_BTLE;
        initiateServer();
    }

    public void setEDR() {
        m_connectivityType = OcConnectivityType.CT_ADAPTER_RFCOMM_BTEDR;
        initiateServer();
    }

    public void setNFC() {
        m_connectivityType = OcConnectivityType.CT_ADAPTER_NFC;
        initiateServer();
    }

    public void setTCP() {
        m_connectivityType = OcConnectivityType.CT_ADAPTER_TCP;
        initiateServer();
    }

    public void createSecuredAirCon() {
        boolean isSecured = true;
        createAirConDevice(isSecured);
    }

    public void createNonsScuredAirCon() {
        boolean isSecured = false;
        createAirConDevice(isSecured);
    }

    public void findResourceWithType() {
        Base.showOutPut("\tPlease type the Resource Type to find, then press Enter: ");
        String resourceType = waitAndGetInputFromUser();
        String host = "";
        findResource(resourceType, host);
    }

    public void findResourceWithQuery() {
        Base.showOutPut("\tPlease type the Query to find resource, then press Enter: ");
        String query = waitAndGetInputFromUser();
        String host = "";
        findAllResources(host, query);
    }

    String getResourceHost() {
        String host = "";
        String ip = "";
        String port = "";

        int selection = selectResource();
        if (selection != -1) {
            host = m_foundResourceList.get(selection).getHost();
        } else {
            Base.showOutPut("Please enter the IP of the Resource host, then press Enter: ");
            ip = waitAndGetInputFromUser();
            Base.showOutPut("Please enter the port of the Resource host, then press Enter: ");
            port = waitAndGetInputFromUser();

            host = ip + ":" + port;
        }

        return host;
    }

    public void findAllResourceUnicast() {
        String host = getResourceHost();
        String query = "";
        findAllResources(host, query);
    }

    public void findAllResourceUnicastWithQuery() {
        String host = getResourceHost();
        Base.showOutPut("\tPlease type the Query to find resource, then press Enter: ");
        String query = waitAndGetInputFromUser();
        findAllResources(host, query);
    }

    public void findResourceUnicast() {
        String host = getResourceHost();
        Base.showOutPut("\tPlease type the resource type to find, then press Enter: ");
        String resourceType = waitAndGetInputFromUser();
        findResource(resourceType, host);
    }

    public void sendGetWithQuery() {
        String queryKey = "";
        String queryValue = "";
        Base.showOutPut("Please type query key, then press Enter: ");
        queryKey = waitAndGetInputFromUser();
        Base.showOutPut("Please type query value, then press Enter: ");
        queryValue = waitAndGetInputFromUser();
        sendGetRequestWithQuery(queryKey, queryValue);
    }

    public void discoverDeviceMulticast() {
        boolean isMulticast = true;
        discoverDevice(isMulticast);
    }
}