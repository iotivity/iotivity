/******************************************************************
 * Copyright 2017 Samsung Electronics All Rights Reserved.
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
 ******************************************************************/

package org.iotivity.test.ri.app;

import java.util.EnumSet;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Vector;

import android.content.Context;

import org.iotivity.base.OcPlatform.OnDeviceFoundListener;
import org.iotivity.base.OcPlatform.OnPlatformFoundListener;
import org.iotivity.base.OcPlatform.OnResourceFoundListener;
import org.iotivity.base.ModeType;
import org.iotivity.base.ObserveType;
import org.iotivity.base.OcConnectivityType;
import org.iotivity.base.OcException;
import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.OcPlatform;
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
import org.iotivity.ca.CaInterface;
import org.iotivity.common.ResourceConstants;
import org.iotivity.common.ResourceHelper;
import org.iotivity.resource.ResourceServer;
import org.iotivity.resource.SampleResource;
import org.iotivity.service.testapp.framework.Base;

public class RITestAppAction extends Base {

    private static final String DEFAULT_IP = "0.0.0.0";
    private static final int DEFAULT_PORT = 0;
    private static double s_targetTemp = 31.234;
    private static double s_tempDeviation = 3.33333;
    private static String s_directionValue = "left";
    private static int s_hourValue = 1;
    private static boolean s_swingerValue = true;

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

    private String m_commonSmartHomeUri = "URI";
    private String m_commonVendorUri = "Vendor";

    private SampleResource m_acSwitchResource;
    private SampleResource m_acTemperatureResource;
    private SampleResource m_acAirFlowResource;
    private SampleResource m_acTimerResource;
    private SampleResource m_acSwingerResource;

    private OcConnectivityType m_connectivityType = OcConnectivityType.CT_ADAPTER_IP;
    private QualityOfService m_qos = QualityOfService.HIGH;

    private enum ResourceSelectionType {
        VERTICAL_RESOURCE, SYSTEM_RESOURCE, VERTICAL_OBSERVABLE_RESOURCE, VERTICAL_NON_OBSERVABLE_RESOURCE
    };

    public RITestAppAction(Context context) {
        m_appContext = context;
        m_createdResourceList = new Vector<SampleResource>();
        m_foundResourceList = new Vector<OcResource>();
        m_resourceHelper = ResourceHelper.getInstance();
        m_serverDBPath = m_appContext.getFilesDir().getPath() + "/" + ResourceConstants.SERVER_DATABASE_FILE_NAME;
        m_clientDBPath = m_appContext.getFilesDir().getPath() + "/" + ResourceConstants.CLIENT_DATABASE_FILE_NAME;
    }

    public void printOutput(final String toPrint) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Base.showOutPut(toPrint);
            }
        });
    }

    public void exitApplication() {
        printOutput("Quit from InterOpAppRI...");
        m_resourceHelper.waitInSecond(ResourceConstants.CALLBACK_WAIT_MIN);
        android.os.Process.killProcess(android.os.Process.myPid());
    }

    OnResourceFoundListener onResourceFound = new OnResourceFoundListener() {

        @Override
        public synchronized void onResourceFound(OcResource resource) {
            if (resource != null) {
                if (m_connectivityType == OcConnectivityType.CT_ADAPTER_TCP) {
                    String tcpHost = m_resourceHelper.getTCPHost(resource.getAllHosts());
                    resource.setHost(tcpHost);
                }

                if (resource.getHost() != "") {
                    m_foundResourceList.add(resource);
                }

                printOutput("Found a resource!!");
                printOutput("uri of the found resource is " + resource.getUri());
                printOutput("Host of found resource: " + resource.getHost());
                printOutput("di( OCResource.sid() ) of found resource is = " + resource.getServerId());

                m_hasCallbackArrived = true;
            } else {
                printOutput("Found resource is invalid!!");
            }

        }

        @Override
        public void onFindResourceFailed(Throwable exception, String message) {
            printOutput("Failed to discover resource, reason: " + message);

        }
    };

    OnPlatformFoundListener onPlatformInfoReceived = new OnPlatformFoundListener() {

        @Override
        public void onPlatformFound(OcRepresentation rep) {
            printOutput("\nPlatform Information received ---->");
            String value = "";
            String values[] = { "pi", "Platform ID                    ", "mnmn", "Manufacturer name              ",
                    "mnml", "Manufacturer url               ", "mnmo", "Manufacturer Model No          ", "mndt",
                    "Manufactured Date              ", "mnpv", "Manufacturer Platform Version  ", "mnos",
                    "Manufacturer OS version        ", "mnhw", "Manufacturer hardware version  ", "mnfv",
                    "Manufacturer firmware version  ", "mnsl", "Manufacturer support url       ", "st",
                    "Manufacturer system time       " };

            for (int i = 0; i < values.length; i += 2) {
                if (rep.hasAttribute(values[i])) {
                    try {
                        value = rep.getValue(values[i]);
                    } catch (OcException e) {
                        e.printStackTrace();
                        printOutput("Failed to get value from Representation : " + e.getMessage());
                    }
                    printOutput(values[i + 1] + " : " + value);
                }
            }
            m_resourceHelper.printRepresentation(rep);
            m_hasCallbackArrived = true;

        }
    };

    OnDeviceFoundListener onDeviceInfoReceived = new OnDeviceFoundListener() {

        @Override
        public void onDeviceFound(OcRepresentation rep) {
            printOutput("\nDevice Information received ---->");
            String value = "";
            String values[] = { "di", "Device ID        ", "n", "Device name      ", "lcv", "Spec version url ", "dmv",
                    "Data Model Model ", };

            for (int i = 0; i < values.length; i += 2) {
                if (rep.hasAttribute(values[i])) {
                    try {
                        value = rep.getValue(values[i]);
                    } catch (OcException e) {
                        e.printStackTrace();
                        printOutput("Failed to get value from Representation : " + e.getMessage());
                    }
                    printOutput(values[i + 1] + " : " + value);
                }
            }

            m_resourceHelper.printRepresentation(rep);
            m_hasCallbackArrived = true;
        }
    };

    OnGetListener onGet = new OnGetListener() {

        @Override
        public void onGetFailed(Throwable eCode) {

            printOutput("onGET Response error: " + eCode.getMessage());
        }

        @Override
        public void onGetCompleted(List<OcHeaderOption> headerOptions, OcRepresentation rep) {
            printOutput("Response: GET request was successful");

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

                printOutput("\nCHILD RESOURCE OF GROUP");
                for (OcRepresentation child : children) {
                    printOutput("\tURI :: " + child.getUri());
                }
            } else {
                printOutput("THe GET Response has the following representation:");
                printOutput(m_resourceHelper.printRepresentation(rep));
            }
        }
    };

    OnPutListener onPut = new OnPutListener() {

        @Override
        public void onPutFailed(Throwable eCode) {
            printOutput("onPUT Response error: " + eCode.getMessage());

        }

        @Override
        public void onPutCompleted(List<OcHeaderOption> headerOptions, OcRepresentation rep) {
            printOutput("Response: PUT request was successful");
            printOutput("THe PUT response has the following representation:");
            printOutput(m_resourceHelper.printRepresentation(rep));
            m_hasCallbackArrived = true;
        }
    };

    OnPostListener onPost = new OnPostListener() {

        @Override
        public void onPostFailed(Throwable eCode) {
            printOutput("onPOST Response error: " + eCode.getMessage());

        }

        @Override
        public void onPostCompleted(List<OcHeaderOption> headerOptions, OcRepresentation rep) {
            printOutput("Response: POST request was successful");
            printOutput("THe POST Response has the following representation:");
            printOutput(m_resourceHelper.printRepresentation(rep));
            m_hasCallbackArrived = true;

        }
    };

    OnDeleteListener onDelete = new OnDeleteListener() {

        @Override
        public void onDeleteFailed(Throwable eCode) {
            printOutput("onDELETE Response error: " + eCode.getMessage());

        }

        @Override
        public void onDeleteCompleted(List<OcHeaderOption> headerOptions) {
            printOutput("Response: DELETE request was successful");
            m_hasCallbackArrived = true;
        }
    };

    OnObserveListener onObserve = new OnObserveListener() {

        @Override
        public void onObserveFailed(Throwable eCode) {
            printOutput("Observe Response/Notification Error: " + eCode.getMessage());

        }

        @Override
        public void onObserveCompleted(List<OcHeaderOption> arg0, OcRepresentation rep, int sequenceNumber) {
            printOutput("OBSERVE RESULT:");
            printOutput("\tSequenceNumber: " + sequenceNumber);
            printOutput(m_resourceHelper.printRepresentation(rep));
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

        PlatformConfig cfg = new PlatformConfig(m_appContext, ServiceType.IN_PROC, ModeType.CLIENT_SERVER, DEFAULT_IP,
                DEFAULT_PORT, m_qos);
        result = SampleResource.constructServer(cfg);

        if (result == false) {
            printOutput("Unable to start Iotivity servers");
        } else {
            printOutput("Iotivity Server started successfully");
            if (m_connectivityType == OcConnectivityType.CT_ADAPTER_GATT_BTLE) {
                CaInterface.setBTConfigure(2);
            }
        }

        return result;
    }

    public void createAirConDevice(boolean isSecured) {

        boolean result = false;
        if (m_isAirConDeviceCreated == false) {
            printOutput("Creating AirCon Device Resources!!");
            ResourceServer.setDeviceInfo("Vendor Smart Home AirCon Device", ResourceConstants.DEVICE_TYPE_AC);

            m_acSwitchResource = new SampleResource();
            m_acSwitchResource.setResourceProperties(ResourceConstants.AC_SWITCH_URI,
                    ResourceConstants.SWITCH_RESOURCE_TYPE, ResourceConstants.SWITCH_RESOURCE_INTERFACE);
            m_acSwitchResource.setAsDiscoverableResource();
            m_acSwitchResource.setAsObservableResource();
            if (isSecured) {
                m_acSwitchResource.setAsSecuredResource();
            }

            OcRepresentation switchRep = new OcRepresentation();
            String key = ResourceConstants.BINARY_SWITCH_KEY;
            String value = "";
            double[] rangeTemperature = { ResourceConstants.TEMPERATURE_RANGE_START_VALUE,
                    ResourceConstants.TEMPERATURE_RANGE_END_VALUE };
            try {
                switchRep.setValue(key, ResourceConstants.BINARY_SWITCH_VALUE);
            } catch (OcException e) {
                e.printStackTrace();
                printOutput("Failed to set value to Representation : " + e.getMessage());
            }
            m_acSwitchResource.setResourceRepresentation(switchRep);

            result = m_acSwitchResource.startResource();

            if (result == true) {
                printOutput("AirConditioner Binary Switch Resource created successfully");
                m_createdResourceList.add(m_acSwitchResource);
                m_isAirConDeviceCreated = true;
            } else {
                printOutput("Unable to create AirConditioner Binary Switch resource");
            }

            m_acTemperatureResource = new SampleResource();
            m_acTemperatureResource.setResourceProperties(ResourceConstants.AC_TEMPERATURE_URI,
                    ResourceConstants.TEMPERATURE_RESOURCE_TYPE, ResourceConstants.TEMPERATURE_RESOURCE_INTERFACE);
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
                printOutput("Failed to set value to Representation : " + e.getMessage());
            }
            m_acTemperatureResource.setResourceRepresentation(temperatureRep);
            result = m_acTemperatureResource.startResource();

            if (result == true) {
                printOutput("Air Conditioner Temperature Resource created successfully");
                m_createdResourceList.add(m_acTemperatureResource);
                m_isAirConDeviceCreated = true;
            } else {
                printOutput("Unable to create Air Conditioner Temperature resource");
            }

            m_acAirFlowResource = new SampleResource();
            m_acAirFlowResource.setResourceProperties(ResourceConstants.AC_AIR_FLOW_URI,
                    ResourceConstants.AIR_FLOW_RESOURCE_TYPE, ResourceConstants.AIR_FLOW_RESOURCE_INTERFACE);
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
                int[] range = { ResourceConstants.AIRFLOW_SPEED_RANGE_START_VALUE,
                        ResourceConstants.AIRFLOW_SPEED_RANGE_END_VALUE };
                key = ResourceConstants.AIRFLOW_RANGE_KEY;
                airFlowRep.setValue(key, range);
            } catch (OcException e) {
                e.printStackTrace();
                printOutput("Failed to set value to Representation : " + e.getMessage());
            }
            m_acAirFlowResource.setAsReadOnly(key);
            m_acAirFlowResource.setResourceRepresentation(airFlowRep);

            result = m_acAirFlowResource.startResource();

            if (result == true) {
                printOutput("Air Conditioner AirFlow Resource created successfully");
                m_createdResourceList.add(m_acAirFlowResource);
                m_isAirConDeviceCreated = true;
            } else {
                printOutput("Unable to create Air Conditioner AirFlow resource");
            }

            m_acTimerResource = new SampleResource();
            m_acTimerResource.setResourceProperties(ResourceConstants.AC_TIMER_URI,
                    ResourceConstants.TIMER_RESOURCE_TYPE, ResourceConstants.TIMER_RESOURCE_INTERFACE);
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
                printOutput("Failed to set value to Representation : " + e.getMessage());
            }

            m_acTimerResource.setResourceRepresentation(clockRep);

            result = m_acTimerResource.startResource();
            m_acTimerResource.setAsSlowResource();

            if (result == true) {
                printOutput("Air Conditioner Timer Resource created successfully");
                m_createdResourceList.add(m_acTimerResource);
                m_isAirConDeviceCreated = true;
            } else {
                printOutput("Unable to create Air Conditioner Timer resource");
            }

            m_acSwingerResource = new SampleResource();
            m_acSwingerResource.setResourceProperties(ResourceConstants.AC_CHILD_LOCK_URI,
                    ResourceConstants.CHILD_LOCK_RESOURCE_TYPE, ResourceConstants.CHILD_LOCK_RESOURCE_INTERFACE);
            m_acSwingerResource.setAsObservableResource();
            if (isSecured) {
                m_acSwingerResource.setAsSecuredResource();
            }

            OcRepresentation swingerRep = new OcRepresentation();
            try {
                swingerRep.setValue(ResourceConstants.SWINGER_SWING_KEY, ResourceConstants.SWINGER_SWING_VALUE);
                swingerRep.setValue(ResourceConstants.SWINGER_BLADE_KEY, ResourceConstants.SWINGER_BLADE_VALUE);
                swingerRep.setValue(ResourceConstants.SWINGER_DIRECTION_KEY, ResourceConstants.SWINGER_DIRECTION_VALUE);
            } catch (OcException e) {
                e.printStackTrace();
                printOutput("Failed to set value to Swinger Representation : " + e.getMessage());
            }
            m_acSwingerResource.setResourceRepresentation(swingerRep);

            result = m_acSwingerResource.startResource();

            if (result == true) {
                printOutput("Air Conditioner Swinger Resource created successfully");
                m_createdResourceList.add(m_acSwingerResource);
                m_isAirConDeviceCreated = true;
            } else {
                printOutput("Unable to create Air Conditioner Swinger resource");
            }
        } else {
            printOutput("Already Smart Home AirCon Device Resources are  created!!");
        }

    }

    public void deleteResource() {
        if (m_createdResourceList.size() != 0) {

            for (SampleResource resource : m_createdResourceList) {
                if (resource.stopResource()) {
                    printOutput("Successfully stopped Resource with URI: " + resource.getUri());
                }
            }
        } else {
            printOutput("There is no resource available to delete!!");
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

            if (m_connectivityType == OcConnectivityType.CT_ADAPTER_TCP) {
                OcPlatform.findResource(host, requestURI,
                        EnumSet.of(OcConnectivityType.CT_ADAPTER_IP, m_connectivityType), onResourceFound, m_qos);
            } else {
                OcPlatform.findResource(host, requestURI, EnumSet.of(m_connectivityType), onResourceFound, m_qos);
            }

            printOutput("Finding Resource....");

        } catch (OcException e) {
            printOutput("Unable to find resource, exception Occured: " + e.getMessage());
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

            if (m_connectivityType == OcConnectivityType.CT_ADAPTER_TCP) {
                OcPlatform.findResource(host, requestURI,
                        EnumSet.of(OcConnectivityType.CT_ADAPTER_IP, m_connectivityType), onResourceFound, m_qos);
            } else {
                OcPlatform.findResource(host, requestURI, EnumSet.of(m_connectivityType), onResourceFound, m_qos);
            }
            printOutput("Finding Resource....");

        } catch (OcException e) {
            printOutput("Unable to find resource, exception Occured: " + e.getMessage());
        }
        waitForCallback();
    }

    public void discoverDevice(boolean isMulticast) {

        String host = "";
        String deviceDiscoveryRequest = "";

        if (isMulticast) {
            deviceDiscoveryRequest = OcPlatform.WELL_KNOWN_DEVICE_QUERY;
            printOutput("Discovering Device using Multicast... ");
        } else {
            if (m_foundResourceList.isEmpty()) {
                printOutput("Finding OIC Servers to send Unicast Discovery Request");
                findAllResources();
            }

            m_hasCallbackArrived = false;
            if (m_foundResourceList.size() > 0) {
                host = m_foundResourceList.get(0).getHost();
                deviceDiscoveryRequest = OcPlatform.WELL_KNOWN_DEVICE_QUERY;
                printOutput("Discovering Device using Unicast... ");
                printOutput("Sending Unicast device discovery to the host: " + host);
            } else {
                printOutput("No server found to send Unicast device discovery");
                printOutput("Sending Multicast Device discovery request...");
                deviceDiscoveryRequest = OcPlatform.WELL_KNOWN_DEVICE_QUERY;
            }
        }

        try {
            OcPlatform.getDeviceInfo(host, deviceDiscoveryRequest, EnumSet.of(m_connectivityType),
                    onDeviceInfoReceived);

            printOutput("Device discovery done successfully");
            waitForCallback();
        } catch (OcException e) {
            printOutput("Exception occurred while discovering device, reason is: " + e.getMessage());
        }
    }

    public void discoverPlatform(boolean isMulticast) {
        String host = "";
        String platformDiscoveryRequest;
        String platformDiscoveryURI = OcPlatform.WELL_KNOWN_PLATFORM_QUERY;

        if (isMulticast) {
            platformDiscoveryRequest = platformDiscoveryURI;
            printOutput("Discovering Platform using Multicast... ");

        } else {
            if (m_foundResourceList.isEmpty()) {
                findAllResources();
                m_hasCallbackArrived = false;
            }

            host = m_foundResourceList.get(0).getHost();
            platformDiscoveryRequest = platformDiscoveryURI;
            printOutput("Discovering Platform using Unicast... ");
        }

        try {
            OcPlatform.getPlatformInfo(host, platformDiscoveryRequest, EnumSet.of(m_connectivityType),
                    onPlatformInfoReceived);

            printOutput("Platform discovery ");
            waitForCallback();

        } catch (OcException e) {
            printOutput("Failure in main thread: " + e.getMessage());
        }
    }

    public void discoverPlatformMulticast() {
        discoverPlatform(true);
    }

    public void sendGetRequestVertical() {
        sendGetRequest(ResourceSelectionType.VERTICAL_RESOURCE);
    }

    public void sendGetRequestCore() {
        sendGetRequest(ResourceSelectionType.SYSTEM_RESOURCE);
    }

    void sendGetRequest(ResourceSelectionType type) {
        Vector<OcResource> remoteResourceList = getRemoteResourceList(type);
        if (remoteResourceList.size() > 0) {
            Map<String, String> qpMap = new HashMap<String, String>();
            for (OcResource resource : remoteResourceList) {
                m_hasCallbackArrived = false;
                printOutput("Sending Get Request to the resource: " + resource.getHost() + resource.getUri());
                try {
                    resource.get(qpMap, onGet, m_qos);
                } catch (OcException e) {
                    printOutput("Unable to send GET request, Exception Occured!! reason: " + e.getMessage());
                }
                printOutput("GET request sent!!");
            }

            waitForCallback();
        } else {
            printOutput("No resource to send GET!!");
        }
    }

    public void sendPutRequestVertical() {
        sendPutRequest(ResourceSelectionType.VERTICAL_RESOURCE);
    }

    public void sendPutRequestCore() {
        sendPutRequest(ResourceSelectionType.SYSTEM_RESOURCE);
    }

    public void sendPutRequest(ResourceSelectionType type) {
        Vector<OcResource> remoteResourceList = getRemoteResourceList(type);
        if (remoteResourceList.size() > 0) {
            Map<String, String> qpMap = new HashMap<String, String>();
            OcRepresentation rep = new OcRepresentation();

            String key = ResourceConstants.REGION_KEY;
            String value = ResourceConstants.DEFAULT_REGION;
            int intensity = ResourceConstants.DEFAULT_INTENSITY;
            try {
                rep.setValue(key, value);
                key = ResourceConstants.POWER_KEY;
                value = ResourceConstants.DEFAULT_POWER_STATE;
                rep.setValue(key, value);
                key = ResourceConstants.INTENSITY_KEY;
                rep.setValue(key, intensity);
                key = ResourceConstants.MANUFACTURER_KEY;
                value = ResourceConstants.DEFAULT_MANUFACTURER;
                rep.setValue(key, value);
            } catch (OcException e) {
                printOutput("Unable to set representation, Exception Occured!! reason: " + e.getMessage());
            }

            for (OcResource resource : remoteResourceList) {
                m_hasCallbackArrived = false;
                printOutput("Sending PUT Request to the resource: " + resource.getHost() + resource.getUri());
                try {
                    resource.put(rep, qpMap, onPut, m_qos);
                } catch (OcException e) {
                    printOutput("Unable to send PUT request, Exception Occured!! reason: " + e.getMessage());
                }
                printOutput("PUT request sent!!");
                waitForCallback();
            }

        } else {
            printOutput("No resource to send PUT!!");
        }
    }

    public void updateLocalResource() {
        if (m_createdResourceList.size() > 0) {
            for (SampleResource resource : m_createdResourceList) {
                String uri = resource.getUri();
                OcRepresentation rep = getupdatedAttributes(uri, resource.getRepresentation());
                resource.setResourceRepresentation(rep);
                printOutput("Successfully updated resource attribute!!");
                m_resourceHelper.printRepresentation(rep);
                if (!uri.equals(ResourceConstants.AC_SWINGER_URI) && !uri.equals(ResourceConstants.AC_TIMER_URI)) {
                    resource.notifyObservers(resource);
                }
            }

        } else {
            printOutput("No resource to Update!!");
        }
    }

    public void sendPostRequestVertical() {
        sendPostRequest(ResourceSelectionType.VERTICAL_RESOURCE);
    }

    public void sendPostRequestCore() {
        sendPostRequest(ResourceSelectionType.SYSTEM_RESOURCE);
    }

    public void sendPostRequestBlockwise() {
        sendPostRequest(ResourceSelectionType.VERTICAL_NON_OBSERVABLE_RESOURCE);
    }

    public void sendPostRequest(ResourceSelectionType type) {
        Vector<OcResource> remoteResourceList = getRemoteResourceList(type);
        Map<String, String> qpMap = new HashMap<String, String>();
        OcRepresentation rep = new OcRepresentation();
        if (type == ResourceSelectionType.SYSTEM_RESOURCE) {

            String key = ResourceConstants.REGION_KEY;
            String value = ResourceConstants.DEFAULT_REGION;
            try {
                rep.setValue(key, value);
                key = ResourceConstants.MANUFACTURER_KEY;
                value = ResourceConstants.DEFAULT_MANUFACTURER;
                rep.setValue(key, value);
            } catch (OcException e1) {
                printOutput("Failed to set representation!!");
            }

            for (OcResource resource : remoteResourceList) {
                try {
                    rep.setValue(key, value);
                    resource.post(rep, qpMap, onPost, m_qos);
                } catch (OcException e) {
                    printOutput("Unbable to send POST request to Core System Resource, Exception occurred: "
                            + e.getMessage());
                }
                printOutput("POST request sent!!");
                waitForCallback();
            }

        } else if (type == ResourceSelectionType.VERTICAL_RESOURCE) {
            for (OcResource resource : remoteResourceList) {
                rep = getupdatedAttributes(resource.getUri(), null);
                try {
                    resource.post(rep, qpMap, onPost, m_qos);
                } catch (OcException e) {
                    printOutput(
                            "Unbable to send POST request to Vertical Resource, Exception occurred: " + e.getMessage());
                }
                printOutput("POST request sent!!");
                waitForCallback();
            }
        } else if (type == ResourceSelectionType.VERTICAL_NON_OBSERVABLE_RESOURCE) {
            for (OcResource resource : remoteResourceList) {
                try {
                    rep.setValue(ResourceConstants.TIMER_LOCATION_KEY,
                            ResourceConstants.DEFAULT_REGION + ResourceConstants.VERY_BIG_VALUE);
                    resource.post(rep, qpMap, onPost, m_qos);
                } catch (OcException e) {
                    printOutput("Unbable to send Blockwise POST request to Vertical Resource, Exception occurred: "
                            + e.getMessage());
                }
                printOutput("POST request sent!!");
                waitForCallback();
            }
        } else {
            printOutput("No resource to send POST!!");
        }
    }

    public void sendDeleteRequestVertical() {
        sendDeleteRequest(ResourceSelectionType.VERTICAL_RESOURCE);
    }

    public void sendDeleteRequestCore() {
        sendDeleteRequest(ResourceSelectionType.SYSTEM_RESOURCE);
    }

    void sendDeleteRequest(ResourceSelectionType type) {
        Vector<OcResource> remoteResourceList = getRemoteResourceList(type);
        if (remoteResourceList.size() > 0) {

            for (OcResource resource : remoteResourceList) {
                printOutput("Sending Delete Request to resource: " + resource.getHost() + resource.getUri());
                m_hasCallbackArrived = false;
                try {
                    resource.deleteResource(onDelete);
                } catch (OcException e) {
                    printOutput("Unbable to send DELETE request, Exception occurred: " + e.getMessage());
                }
                printOutput("DELETE request sent!!");
                waitForCallback();
            }
        } else {
            printOutput("No resource to send DELETE!!");
        }
    }

    public void sendObserveRequestNonObservable() {
        sendObserveRequest(ResourceSelectionType.VERTICAL_NON_OBSERVABLE_RESOURCE);
    }

    public void sendObserveRequestVertical() {
        sendObserveRequest(ResourceSelectionType.VERTICAL_OBSERVABLE_RESOURCE);
    }

    public void sendObserveRequestCore() {
        sendObserveRequest(ResourceSelectionType.SYSTEM_RESOURCE);
    }

    void sendObserveRequest(ResourceSelectionType type) {
        Vector<OcResource> remoteResourceList = getRemoteResourceList(type);
        if (remoteResourceList.size() > 0) {

            for (OcResource resource : remoteResourceList) {
                printOutput("Sending Observe Request to resource: " + resource.getHost() + resource.getUri());
                m_hasCallbackArrived = false;
                try {
                    resource.observe(ObserveType.OBSERVE, new HashMap<String, String>(), onObserve, m_qos);
                } catch (OcException e) {
                    printOutput("Unbable to Observe resource, Exception occurred: " + e.getMessage());
                }
                printOutput("Observe request sent!!");
                m_isObservingResource = true;
                waitForCallback();
            }
        } else {
            printOutput("No resource to Observe!!");
        }
    }

    public void cancelObserveResource() {
        Vector<OcResource> remoteResourceList = getRemoteResourceList(
                ResourceSelectionType.VERTICAL_OBSERVABLE_RESOURCE);
        if (remoteResourceList.size() > 0) {
            if (m_isObservingResource) {
                printOutput("Canceling Observe resource...");

                for (OcResource resource : remoteResourceList) {
                    try {
                        resource.cancelObserve(m_qos);
                    } catch (OcException e) {
                        printOutput("Unbable to Cancel Observe resource, Exception occurred: " + e.getMessage());
                    }
                    if (m_qos == QualityOfService.HIGH) {
                        printOutput("Observe Cancel request sent!!");
                    } else {
                        printOutput("Observe Cancel request will be sent when a CON Notification is received!!");
                    }

                    m_isObservingResource = false;
                }
            } else {
                printOutput("No resource is being Observed currently!!");
            }
        } else {
            printOutput("No resource to cancel Observe!!");
        }
    }

    String getHost() {
        String host = "";

        for (OcResource resource : m_foundResourceList) {
            if (resource.getUri().equals(ResourceConstants.AC_TEMPERATURE_URI)) {
                host = resource.getHost();
                break;
            }
        }
        if (host.equals("")) {
            printOutput("No remote resource found to send Unicast");
        }
        return host;
    }

    Vector<OcResource> getRemoteResourceList(ResourceSelectionType type) {
        Vector<OcResource> desiredResourceList = new Vector<OcResource>();
        desiredResourceList.clear();
        String remoteHost = getHost();

        for (OcResource resource : m_foundResourceList) {
            if (ResourceSelectionType.VERTICAL_NON_OBSERVABLE_RESOURCE == type) {
                if (resource.getUri().equals(ResourceConstants.AC_TIMER_URI)) {
                    desiredResourceList.clear();
                    desiredResourceList.add(resource);
                    break;
                }
            } else if (ResourceSelectionType.VERTICAL_OBSERVABLE_RESOURCE == type) {
                if (resource.getUri().indexOf(m_commonSmartHomeUri) != -1) {
                    desiredResourceList.add(resource);
                }
            } else if (ResourceSelectionType.VERTICAL_RESOURCE == type) {
                if ((resource.getUri().indexOf(m_commonVendorUri) != -1)
                        || (resource.getUri().indexOf(m_commonSmartHomeUri) != -1)) {
                    desiredResourceList.add(resource);
                }
            } else {
                if ((remoteHost.equals(resource.getHost()))
                        && ((resource.getUri().equals(OcPlatform.WELL_KNOWN_DEVICE_QUERY))
                                || (resource.getUri().equals(OcPlatform.WELL_KNOWN_PLATFORM_QUERY)))) {
                    desiredResourceList.add(resource);
                }
            }
        }
        return desiredResourceList;
    }

    public void getUserInput(String userArgument) {
        m_latestUserInput = userArgument;
        m_hasUserResponded = true;
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

    public void createNonSecuredAirCon() {
        boolean isSecured = false;
        createAirConDevice(isSecured);
    }

    public void findResourceWithType() {
        String resourceType = ResourceConstants.TEMPERATURE_RESOURCE_TYPE;
        String host = "";
        findResource(resourceType, host);
    }

    OcRepresentation getupdatedAttributes(String uri, OcRepresentation rep) {
        double temperatureValue = 25;
        boolean binaryValue = true;
        String key = "";

        if (rep == null) {
            rep = new OcRepresentation();
        }

        try {
            if (uri.equals(ResourceConstants.AC_SWITCH_URI)) {
                key = ResourceConstants.BINARY_SWITCH_KEY;
                printOutput("binaryValue is : " + binaryValue);
                if (rep.hasAttribute(key)) {
                    binaryValue = ((Boolean) rep.getValue(key)).booleanValue();
                    printOutput("binaryValue is : " + binaryValue);
                }
                binaryValue = !binaryValue;
                rep.setValue(key, binaryValue);

            } else if (uri.equals(ResourceConstants.AC_TEMPERATURE_URI)) {
                key = ResourceConstants.TEMPERATURE_KEY;
                if (rep.hasAttribute(key)) {
                    temperatureValue = ((Double) rep.getValue(key)).doubleValue();
                }
                temperatureValue = temperatureValue > 0 ? (temperatureValue - 26) : (temperatureValue + 26);
                rep.setValue(key, temperatureValue);
            } else if (uri.equals(ResourceConstants.AC_AIR_FLOW_URI)) {
                key = ResourceConstants.AIRFLOW_DIRECTION_KEY;
                if (rep.hasAttribute(key)) {
                    s_directionValue = rep.getValue(key);
                }
                ;
                s_directionValue = s_directionValue.equals("left") ? "right" : "left";
                rep.setValue(key, s_directionValue);
            } else if (uri.equals(ResourceConstants.AC_TIMER_URI)) {
                key = ResourceConstants.TIMER_HOUR_KEY;
                if (rep.hasAttribute(key)) {
                    s_hourValue = ((Integer) rep.getValue(key)).intValue();
                }
                s_hourValue = (s_hourValue % 2) > 0 ? (s_hourValue * 2) : (s_hourValue / 2);
                rep.setValue(key, s_hourValue);
            } else if (uri.equals(ResourceConstants.AC_SWINGER_URI)) {
                key = ResourceConstants.SWINGER_SWING_KEY;
                if (rep.hasAttribute(key)) {
                    s_swingerValue = ((Boolean) rep.getValue(key)).booleanValue();
                }
                s_swingerValue = !s_swingerValue;
                rep.setValue(key, s_swingerValue);
            }
        } catch (OcException e) {
            printOutput("Failed to update Attributes, reason" + e.getLocalizedMessage());
        }

        return rep;
    }

    public void findAllResourceUnicast() {
        findAllResources();
        String host = getHost();
        String query = "";
        findAllResources(host, query);
    }

    public void discoverDeviceMulticast() {
        boolean isMulticast = true;
        discoverDevice(isMulticast);
    }
}
