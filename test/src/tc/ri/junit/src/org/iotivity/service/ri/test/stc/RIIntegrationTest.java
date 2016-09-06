/******************************************************************
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 * <p/>
 * <p/>
 * <p/>
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * <p/>
 * // * http://www.apache.org/licenses/LICENSE-2.0
 * <p/>
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************/

package org.iotivity.service.ri.test.stc;

import java.util.ArrayList;
import java.util.EnumSet;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.iotivity.base.EntityHandlerResult;
import org.iotivity.base.ModeType;
import org.iotivity.base.OcConnectivityType;
import org.iotivity.base.OcDeviceInfo;
import org.iotivity.base.OcException;
import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcPlatformInfo;
import org.iotivity.base.OcPlatformStatus;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResource;
import org.iotivity.base.OcResourceHandle;
import org.iotivity.base.OcResourceRequest;
import org.iotivity.base.OcResourceResponse;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.RequestHandlerFlag;
import org.iotivity.base.RequestType;
import org.iotivity.base.ResourceProperty;
import org.iotivity.base.ServiceType;
import org.iotivity.base.OcPlatform.OnDeviceFoundListener;
import org.iotivity.base.OcPlatform.OnPlatformFoundListener;
import org.iotivity.base.OcPlatform.OnResourceFoundListener;
import org.iotivity.base.OcResource.OnDeleteListener;
import org.iotivity.base.OcResource.OnGetListener;
import org.iotivity.base.OcResource.OnPostListener;
import org.iotivity.base.OcResource.OnPutListener;
import org.iotivity.service.ri.test.helper.RIHelper;

import android.test.InstrumentationTestCase;
import android.util.Log;

public class RIIntegrationTest extends InstrumentationTestCase {
    private final String LOG_TAG = this.getClass().getSimpleName();

    private RIHelper m_riHelper;
    private OcResourceHandle m_resourceHandle;
    private String m_host = "";
    private boolean m_isCallback;
    private OcResource m_temperatureResource;
    private boolean m_foundAllDeviceInfo;
    private boolean m_foundAllPlatformInfo;
    private Map<String, String> m_qpMap;
    private int m_receivedTemperature;
    private int m_receivedHour;
    private boolean m_isGetCorrect;
    private boolean m_isPutCorrect;
    private boolean m_isPostCorrect;
    private boolean m_isDeleteCorrect;
    private final int m_setTemp = 20;
    private final int m_setHour = 20;

    protected void setUp() {
        m_riHelper = RIHelper.getInstance(getInstrumentation()
                .getTargetContext());
        m_resourceHandle = null;
        m_isCallback = false;
        m_temperatureResource = null;
        m_foundAllDeviceInfo = true;
        m_foundAllPlatformInfo = true;
        m_qpMap = new HashMap<String, String>();
        m_isGetCorrect = false;
        m_isPutCorrect = false;
        m_isPostCorrect = false;
        m_isDeleteCorrect = false;

        RIHelper.s_isServerOk = false;
        RIHelper.s_errorMsg = "";

        Log.i(LOG_TAG, "SetUp Finished");
    }

    protected void tearDown() {
        if (m_resourceHandle != null) {
            try {
                OcPlatform.unregisterResource(m_resourceHandle);
            } catch (OcException e) {
                e.printStackTrace();
            }
        }
        Log.i(LOG_TAG, "tearDown Finished");
    }

    OnResourceFoundListener onResourceFound = new OnResourceFoundListener() {
        @Override
        public void onResourceFound(OcResource resource) {
            Log.i(LOG_TAG, "Inside onResourceFound");

            if (resource != null) {
                m_isCallback = true;
                Log.i(LOG_TAG, "\tHost of found resource: " + resource.getHost());
                Log.i(LOG_TAG, "\tdi( OCResource.sid() ) of found resource is = " + resource.getServerId());
                Log.i(LOG_TAG, "\tunique identifier of found resource is = " + resource.getUniqueIdentifier());
                Log.i(LOG_TAG, "\turi of the found resource is: " + resource.getUri());
                for (String resourceType : resource.getResourceTypes()) {
                    Log.i(LOG_TAG, "\t\tResource Type: " + resourceType);
                    if (resourceType.equals(RIHelper.RESOURCE_TYPE_TEMPERATURE)) {
                        Log.i(LOG_TAG, "\t\tFound temperature resource");
                        m_temperatureResource = resource;
                    }
                }
            } else {
                Log.i(LOG_TAG, "Found Resource is invalid");
            }
        }

        @Override
        public void onFindResourceFailed(Throwable exception, String message) {
            Log.i(LOG_TAG, "onFindResourceFailed, reason: " + message);
        }
    };

    OnDeviceFoundListener onDeviceInfoReceived = new OnDeviceFoundListener() {

        @Override
        public void onDeviceFound(OcRepresentation rep) {
            Log.i(LOG_TAG, "Inside onDeviceFound");
            Log.i(LOG_TAG, "Device Information received ---->");
            m_isCallback = true;
            String value = "";
            String values[] =
                    {"di", "Device ID        ", "n", "Device name      ", "lcv", "Spec version url ", "dmv",
                            "Data Model Model ",};

            for (int i = 0; i < values.length; i += 2) {
                if (rep.hasAttribute(values[i])) {
                    try {
                        value = rep.getValue(values[i]);
                        if (value.equals(RIHelper.KEY_DEVICE_NAME)) {
                            if (value.equals(RIHelper.DEVICE_NAME) == false) {
                                m_foundAllDeviceInfo = false;
                            }
                        }
                    } catch (OcException e) {
                        e.printStackTrace();
                        Log.i(LOG_TAG, "Failed to get value from Representation : " + e.getMessage());
                    }
                    Log.i(LOG_TAG, values[i + 1] + " : " + value);
                }
            }
        }
    };

    OnPlatformFoundListener onPlatformInfoReceived = new OnPlatformFoundListener() {

        @Override
        public void onPlatformFound(OcRepresentation rep) {
            Log.i(LOG_TAG, "Inside onPlatformFound");
            Log.i(LOG_TAG, "Platform Information received ---->");
            m_isCallback = true;
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
                        if (value.equals(RIHelper.KEY_PLATFORM_ID)) {
                            if (value.equals(RIHelper.PLATFORM_ID) == false) {
                                m_foundAllDeviceInfo = false;
                            }
                        } else if (value.equals(RIHelper.KEY_MANUFACTURER_NAME)) {
                            if (value.equals(RIHelper.MANUFACTURER_NAME) == false) {
                                m_foundAllDeviceInfo = false;
                            }
                        } else if (value.equals(RIHelper.KEY_MANUFACTURER_URL)) {
                            if (value.equals(RIHelper.MANUFACTURER_URL) == false) {
                                m_foundAllDeviceInfo = false;
                            }
                        } else if (value.equals(RIHelper.KEY_MODEL_NO)) {
                            if (value.equals(RIHelper.MODEL_NO) == false) {
                                m_foundAllDeviceInfo = false;
                            }
                        } else if (value.equals(RIHelper.KEY_MANUFACTURE_DATE)) {
                            if (value.equals(RIHelper.MANUFACTURE_DATE) == false) {
                                m_foundAllDeviceInfo = false;
                            }
                        } else if (value.equals(RIHelper.KEY_PLATFORM_VERSION)) {
                            if (value.equals(RIHelper.PLATFORM_VERSION) == false) {
                                m_foundAllDeviceInfo = false;
                            }
                        } else if (value.equals(RIHelper.KEY_OS_VERSION)) {
                            if (value.equals(RIHelper.OS_VERSION) == false) {
                                m_foundAllDeviceInfo = false;
                            }
                        } else if (value.equals(RIHelper.KEY_HW_VERSION)) {
                            if (value.equals(RIHelper.HW_VERSION) == false) {
                                m_foundAllDeviceInfo = false;
                            }
                        } else if (value.equals(RIHelper.KEY_FIRMWARE_VERSION)) {
                            if (value.equals(RIHelper.FIRMWARE_VERSION) == false) {
                                m_foundAllDeviceInfo = false;
                            }
                        } else if (value.equals(RIHelper.KEY_SUPPORT_URL)) {
                            if (value.equals(RIHelper.SUPPORT_URL) == false) {
                                m_foundAllDeviceInfo = false;
                            }
                        }
                    } catch (OcException e) {
                        e.printStackTrace();
                        Log.i(LOG_TAG, "Failed to get value from Representation : " + e.getMessage());
                    }
                    Log.i(LOG_TAG, values[i + 1] + " : " + value);
                }
            }
        }
    };

    OnGetListener onGet = new OnGetListener() {

        @Override
        public void onGetCompleted(List<OcHeaderOption> arg0, OcRepresentation rep) {
            Log.i(LOG_TAG, "Inside onGet");
            try {
                m_receivedTemperature = rep.getValue(RIHelper.KEY_TEMPERATURE);
                m_receivedHour = rep.getValue(RIHelper.KEY_HOUR);
            } catch (OcException e) {
                Log.i(LOG_TAG, "Exception occured inside getValue");
                e.printStackTrace();
            }
            Log.i(LOG_TAG, "\t" + RIHelper.KEY_TEMPERATURE + ": " + m_receivedTemperature);
            Log.i(LOG_TAG, "\t" + RIHelper.KEY_HOUR + ": " + m_receivedHour);
            if (m_receivedTemperature == m_riHelper.m_temp && m_receivedHour == m_riHelper.m_hour) {
                m_isGetCorrect = true;
            }
        }

        @Override
        public void onGetFailed(Throwable message) {
            Log.i(LOG_TAG, "onGet failed, reason: " + message);
            m_isGetCorrect = false;
        }
    };

    OnPutListener onPut = new OnPutListener() {

        @Override
        public void onPutFailed(Throwable message) {
            Log.i(LOG_TAG, "onPut failed, reason: " + message);
        }

        @Override
        public void onPutCompleted(List<OcHeaderOption> arg0, OcRepresentation rep) {
            Log.i(LOG_TAG, "Inside onPut");
            try {
                m_receivedTemperature = rep.getValue(RIHelper.KEY_TEMPERATURE);
                m_receivedHour = rep.getValue(RIHelper.KEY_HOUR);
            } catch (OcException e) {
                Log.i(LOG_TAG, "Exception occured inside getValue");
                e.printStackTrace();
            }
            Log.i(LOG_TAG, "\t" + RIHelper.KEY_TEMPERATURE + ": " + m_receivedTemperature);
            Log.i(LOG_TAG, "\t" + RIHelper.KEY_HOUR + ": " + m_receivedHour);
            if (m_receivedTemperature == m_riHelper.m_temp && m_receivedHour == m_riHelper.m_hour) {
                m_isPutCorrect = true;
            }
        }
    };

    OnPostListener onPost = new OnPostListener() {

        @Override
        public void onPostFailed(Throwable message) {
            Log.i(LOG_TAG, "onPost failed, reason: " + message);
        }

        @Override
        public void onPostCompleted(List<OcHeaderOption> arg0, OcRepresentation rep) {
            Log.i(LOG_TAG, "Inside onPost");
            try {
                m_receivedTemperature = rep.getValue(RIHelper.KEY_TEMPERATURE);
                m_receivedHour = rep.getValue(RIHelper.KEY_HOUR);
            } catch (OcException e) {
                Log.i(LOG_TAG, "Exception occured inside getValue");
                e.printStackTrace();
            }
            Log.i(LOG_TAG, "\t" + RIHelper.KEY_TEMPERATURE + ": " + m_receivedTemperature);
            Log.i(LOG_TAG, "\t" + RIHelper.KEY_HOUR + ": " + m_receivedHour);
            if (m_receivedTemperature == m_riHelper.m_temp && m_receivedHour == m_riHelper.m_hour) {
                m_isPostCorrect = true;
            }
        }
    };

    OnDeleteListener onDelete = new OnDeleteListener() {

        @Override
        public void onDeleteFailed(Throwable message) {
            Log.i(LOG_TAG, "onDelete failed, reason: " + message);
        }

        @Override
        public void onDeleteCompleted(List<OcHeaderOption> arg0) {
            Log.i(LOG_TAG, "Inside onDelete");
            m_isDeleteCorrect = true;
        }
    };

    /**
     * @objective Test  registerResource() API and findResource() API positively to check
     * if registered resource is found and foundResource callback is called
     * @target OcResourceHandle registerResource( String resourceUri, String
     * resourceTypeName, String resourceInterface, EntityHandler
     * entityHandler, EnumSet<ResourceProperty> resourcePropertySet)
     * @target void findResource(
     * String host,
     * String resourceUri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * OnResourceFoundListener onResourceFoundListener)
     * @test_data 1. resourceUri "/a/temperature"
     * 2. resourceTypeName "oic.r.temperature"
     * 3. resourceInterface DEFAULT_INTERFACE
     * 4. entityHandler entity handler
     * 5. resourcePropertySet indicates property of the resource
     * 6. host = ""
     * 7. resourceUri = "/oic/res"
     * 8. connectivityTypeSet Set of OcConnectivityType
     * 9. onResourceFoundListener event handler
     * @pre_condition Configure platform for client server mode
     * @procedure 1. Perform registerResource() API
     * 2. Perform findResource() API
     * 3. Check if entity handler is called
     * 4. Check if temperature resource is found
     * @post_condition None
     * @expected Resource should be found successfully and entity handler should be called
     * @see void Configure(PlatformConfig platformConfig)
     * @since 2016-09-08
     */
    public void testCreateAndFindResource_SQV_CV_P() {
        m_resourceHandle = m_riHelper.registerResource(RIHelper.RESOURCE_URI_TEMPERATURE, RIHelper.RESOURCE_TYPE_TEMPERATURE);
        assertNotNull("Can not create resource", m_resourceHandle);

        try {
            OcPlatform.findResource(m_host, OcPlatform.WELL_KNOWN_QUERY, EnumSet.of(OcConnectivityType.CT_DEFAULT), onResourceFound);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }

        m_riHelper.waitInSecond(m_riHelper.CALLBACK_WAIT_DEFAULT);

        assertEquals("OnResourceFoundListener is not called", true, m_isCallback);
        assertNotNull("Temperature Resource is not found", m_temperatureResource);
    }

    /**
     * @objective Test  registerResource() API and findResource() API(with resource type) positively to check
     * if registered resource is found and foundResource callback is called
     * @target OcResourceHandle registerResource( String resourceUri, String
     * resourceTypeName, String resourceInterface, EntityHandler
     * entityHandler, EnumSet<ResourceProperty> resourcePropertySet)
     * @target void findResource(
     * String host,
     * String resourceUri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * OnResourceFoundListener onResourceFoundListener)
     * @test_data 1. resourceUri "/a/temperature"
     * 2. resourceTypeName "oic.r.temperature"
     * 3. resourceInterface DEFAULT_INTERFACE
     * 4. entityHandler entity handler
     * 5. resourcePropertySet indicates property of the resource
     * 6. host = ""
     * 7. resourceUri = "/oic/res/?rt=oic.r.temperature"
     * 8. connectivityTypeSet Set of OcConnectivityType
     * 9. onResourceFoundListener event handler
     * @pre_condition Configure platform for client server mode
     * @procedure 1. Perform registerResource() API
     * 2. Perform findResource() API
     * 3. Check if entity handler is called
     * 4. Check if temperature resource is found
     * @post_condition None
     * @expected Resource should be found successfully and entity handler should be called
     * @see void Configure(PlatformConfig platformConfig)
     * @since 2016-09-08
     */
    public void testCreateAndFindResourceUsingResourceType_SQV_CV_P() {
        m_resourceHandle = m_riHelper.registerResource(RIHelper.RESOURCE_URI_TEMPERATURE, RIHelper.RESOURCE_TYPE_TEMPERATURE);
        assertNotNull("Can not create resource", m_resourceHandle);

        try {
            OcPlatform.findResource(m_host, RIHelper.TEMPERATURE_RESOURCE_QUERY, EnumSet.of(OcConnectivityType.CT_DEFAULT), onResourceFound);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }

        m_riHelper.waitInSecond(m_riHelper.CALLBACK_WAIT_DEFAULT);

        assertEquals("OnResourceFoundListener is not called", true, m_isCallback);
        assertNotNull("Temperature Resource is not found", m_temperatureResource);
    }

    /**
     * @objective Test  registerResource() API and findResource() API(with resource type) positively for two times to check
     * if registered resource is found and foundResource callback is called
     * @target OcResourceHandle registerResource( String resourceUri, String
     * resourceTypeName, String resourceInterface, EntityHandler
     * entityHandler, EnumSet<ResourceProperty> resourcePropertySet)
     * @target void findResource(
     * String host,
     * String resourceUri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * OnResourceFoundListener onResourceFoundListener)
     * @test_data 1. resourceUri "/a/temperature"
     * 2. resourceTypeName "oic.r.temperature"
     * 3. resourceInterface DEFAULT_INTERFACE
     * 4. entityHandler entity handler
     * 5. resourcePropertySet indicates property of the resource
     * 6. host = ""
     * 7. resourceUri = "/oic/res/?rt=oic.r.temperature"
     * 8. connectivityTypeSet Set of OcConnectivityType
     * 9. onResourceFoundListener event handler
     * @pre_condition Configure platform for client server mode
     * @procedure 1. Perform registerResource() API
     * 2. Perform findResource() API
     * 3. Check if entity handler is called
     * 4. Check if temperature resource is found
     * 5. Perform findResource() API again with resource type in query
     * 6. Check if entity handler is called
     * 7. Check if temperature resource is found
     * @post_condition None
     * @expected For both times, Resource should be found successfully and entity handler should be called
     * @see void Configure(PlatformConfig platformConfig)
     * @since 2016-09-08
     */
    public void testCreateAndFindResourceTwice_VLCC_P() {
        m_resourceHandle = m_riHelper.registerResource(RIHelper.RESOURCE_URI_TEMPERATURE, RIHelper.RESOURCE_TYPE_TEMPERATURE);
        assertNotNull("Can not create resource", m_resourceHandle);

        Log.i(LOG_TAG, "Finding Resource for the first time");
        try {
            OcPlatform.findResource(m_host, RIHelper.TEMPERATURE_RESOURCE_QUERY, EnumSet.of(OcConnectivityType.CT_DEFAULT), onResourceFound);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }

        m_riHelper.waitInSecond(m_riHelper.CALLBACK_WAIT_DEFAULT);

        assertEquals("OnResourceFoundListener is not called in the first call", true, m_isCallback);
        assertNotNull("Temperature Resource is not found in the first call", m_temperatureResource);

        m_isCallback = false;
        m_temperatureResource = null;
        Log.i(LOG_TAG, "Finding Resource for the second time");

        try {
            OcPlatform.findResource(m_host, RIHelper.TEMPERATURE_RESOURCE_QUERY, EnumSet.of(OcConnectivityType.CT_DEFAULT), onResourceFound);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }

        m_riHelper.waitInSecond(m_riHelper.CALLBACK_WAIT_DEFAULT);

        assertEquals("OnResourceFoundListener is not called in the second call", true, m_isCallback);
        assertNotNull("Temperature Resource is not found in the second call", m_temperatureResource);
    }

    /**
     * @objective Test  registerResource() API and findResource() API(with qos) positively to check
     * if registered resource is found and foundResource callback is called
     * @target OcResourceHandle registerResource( String resourceUri, String
     * resourceTypeName, String resourceInterface, EntityHandler
     * entityHandler, EnumSet<ResourceProperty> resourcePropertySet)
     * @target void findResource(
     * String host,
     * String resourceUri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * OnResourceFoundListener onResourceFoundListener,
     * QualityOfService qualityOfService)
     * @test_data 1. resourceUri "/a/temperature"
     * 2. resourceTypeName "oic.r.temperature"
     * 3. resourceInterface DEFAULT_INTERFACE
     * 4. entityHandler entity handler
     * 5. resourcePropertySet indicates property of the resource
     * 6. host = ""
     * 7. resourceUri = "/oic/res/?rt=oic.r.temperature"
     * 8. connectivityTypeSet Set of OcConnectivityType
     * 9. onResourceFoundListener event handler
     * 10. qualityOfService High
     * @pre_condition Configure platform for client server mode
     * @procedure 1. Perform registerResource() API
     * 2. Perform findResource() API (with qos)
     * 3. Check if entity handler is called
     * 4. Check if temperature resource is found
     * @post_condition None
     * @expected Resource should be found successfully and entity handler should be called
     * @see void Configure(PlatformConfig platformConfig)
     * @since 2016-09-08
     */
    public void testCreateAndFindResourceWithQos_SQV_CV_P() {
        m_resourceHandle = m_riHelper.registerResource(RIHelper.RESOURCE_URI_TEMPERATURE, RIHelper.RESOURCE_TYPE_TEMPERATURE);
        assertNotNull("Can not create resource", m_resourceHandle);

        try {
            OcPlatform.findResource(m_host, RIHelper.TEMPERATURE_RESOURCE_QUERY, EnumSet.of(OcConnectivityType.CT_DEFAULT), onResourceFound,
                    QualityOfService.HIGH);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }

        m_riHelper.waitInSecond(m_riHelper.CALLBACK_WAIT_DEFAULT);

        assertEquals("OnResourceFoundListener is not called", true, m_isCallback);
        assertNotNull("Temperature Resource is not found", m_temperatureResource);
    }

    /**
     * @objective Test  registerResource() API and findResource() API (with resource type & with qos) positively for two times to check
     * if registered resource is found and foundResource callback is called
     * @target OcResourceHandle registerResource( String resourceUri, String
     * resourceTypeName, String resourceInterface, EntityHandler
     * entityHandler, EnumSet<ResourceProperty> resourcePropertySet)
     * @target void findResource(
     * String host,
     * String resourceUri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * OnResourceFoundListener onResourceFoundListener,
     * QualityOfService qualityOfService)
     * @test_data 1. resourceUri "/a/temperature"
     * 2. resourceTypeName "oic.r.temperature"
     * 3. resourceInterface DEFAULT_INTERFACE
     * 4. entityHandler entity handler
     * 5. resourcePropertySet indicates property of the resource
     * 6. host = ""
     * 7. resourceUri = "/oic/res/?rt=oic.r.temperature"
     * 8. connectivityTypeSet Set of OcConnectivityType
     * 9. onResourceFoundListener event handler
     * 10. qualityOfService High
     * @pre_condition Configure platform for client server mode
     * @procedure 1. Perform registerResource() API
     * 2. Perform findResource() API(with qos)
     * 3. Check if entity handler is called
     * 4. Check if temperature resource is found
     * 5. Perform findResource() API(with qos) again with resource type in query
     * 6. Check if entity handler is called
     * 7. Check if temperature resource is found
     * @post_condition None
     * @expected For both times, Resource should be found successfully and entity handler should be called
     * @see void Configure(PlatformConfig platformConfig)
     * @since 2016-09-08
     */
    public void testCreateAndFindResourceWithQosTwice_VLCC_P() {
        m_resourceHandle = m_riHelper.registerResource(RIHelper.RESOURCE_URI_TEMPERATURE, RIHelper.RESOURCE_TYPE_TEMPERATURE);
        assertNotNull("Can not create resource", m_resourceHandle);

        Log.i(LOG_TAG, "Finding Resource for the first time");
        try {
            OcPlatform.findResource(m_host, RIHelper.TEMPERATURE_RESOURCE_QUERY, EnumSet.of(OcConnectivityType.CT_DEFAULT), onResourceFound,
                    QualityOfService.HIGH);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }

        m_riHelper.waitInSecond(m_riHelper.CALLBACK_WAIT_DEFAULT);

        assertEquals("OnResourceFoundListener is not called in the first call", true, m_isCallback);
        assertNotNull("Temperature Resource is not found in the first call", m_temperatureResource);

        m_isCallback = false;
        m_temperatureResource = null;
        Log.i(LOG_TAG, "Finding Resource for the second time");

        try {
            OcPlatform.findResource(m_host, RIHelper.TEMPERATURE_RESOURCE_QUERY, EnumSet.of(OcConnectivityType.CT_DEFAULT), onResourceFound,
                    QualityOfService.HIGH);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }

        m_riHelper.waitInSecond(m_riHelper.CALLBACK_WAIT_DEFAULT);

        assertEquals("OnResourceFoundListener is not called in the second call", true, m_isCallback);
        assertNotNull("Temperature Resource is not found in the second call", m_temperatureResource);
    }

    /**
     * @objective Test registerDeviceInfo() API and getDeviceInfo() API to check if
     * registered device information is get
     * @target void registerDeviceInfo(OcDeviceInfo ocDeviceInfo)
     * @target void getDeviceInfo(
     * String host,
     * String deviceUri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * OnDeviceFoundListener onDeviceFoundListener)
     * @test_data 1. OcDeviceInfo Device information
     * 2. host = ""
     * 3. deviceUri = "/oic/d"
     * 4. connectivityTypeSet Set of OcConnectivityType
     * 5. OnDeviceFoundListener event handler
     * @pre_condition Configure platform for client server mode
     * @procedure 1. Perform registerDeviceInfo() API
     * 2. Perform getDeviceInfo() API
     * 3. Check if entity handler is called
     * 4. Check if registered information is received
     * @post_condition None
     * @expected Received device info should be matched with Set device info
     * @see void Configure(PlatformConfig platformConfig)
     * @since 2016-09-08
     */
    public void testRegisterAndGetDeviceInfo_GSRV_CV_P() {
        try {
            OcDeviceInfo deviceInfo = m_riHelper.getDeviceInfo();
            OcPlatform.registerDeviceInfo(deviceInfo);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }

        try {
            OcPlatform.getDeviceInfo(m_host, OcPlatform.WELL_KNOWN_DEVICE_QUERY, EnumSet.of(OcConnectivityType.CT_DEFAULT), onDeviceInfoReceived);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }

        m_riHelper.waitInSecond(m_riHelper.CALLBACK_WAIT_DEFAULT);

        assertEquals("onDeviceInfoReceived is not called", true, m_isCallback);
        assertEquals("Did not find all device information", true, m_foundAllDeviceInfo);
    }

    /**
     * @objective Test registerDeviceInfo() API and getDeviceInfo() API(with qos) to check if
     * registered device information is get
     * @target void registerDeviceInfo(OcDeviceInfo ocDeviceInfo)
     * @target void getDeviceInfo(
     * String host,
     * String deviceUri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * OnDeviceFoundListener onDeviceFoundListener,
     * QualityOfService qualityOfService)
     * @test_data 1. OcDeviceInfo Device information
     * 2. host = ""
     * 3. deviceUri = "/oic/d"
     * 4. connectivityTypeSet Set of OcConnectivityType
     * 5. OnDeviceFoundListener event handler
     * 6. QualityOfService High
     * @pre_condition Configure platform for client server mode
     * @procedure 1. Perform registerDeviceInfo() API
     * 2. Perform getDeviceInfo() API(with qos)
     * 3. Check if entity handler is called
     * 4. Check if registered information is received
     * @post_condition None
     * @expected Received device info should be matched with Set device info
     * @see void Configure(PlatformConfig platformConfig)
     * @since 2016-09-08
     */
    public void testRegisterAndGetDeviceInfoWithQos_GSRV_CV_P() {
        try {
            OcDeviceInfo deviceInfo = m_riHelper.getDeviceInfo();
            OcPlatform.registerDeviceInfo(deviceInfo);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }

        try {
            OcPlatform.getDeviceInfo(m_host, OcPlatform.WELL_KNOWN_DEVICE_QUERY, EnumSet.of(OcConnectivityType.CT_DEFAULT), onDeviceInfoReceived,
                    QualityOfService.HIGH);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }

        m_riHelper.waitInSecond(m_riHelper.CALLBACK_WAIT_DEFAULT);

        assertEquals("onDeviceInfoReceived is not called", true, m_isCallback);
        assertEquals("Did not find all device information", true, m_foundAllDeviceInfo);
    }

    /**
     * @objective Test registerPlatformInfo() API and getPlatformInfo() API to check if
     * registered platform information is get
     * @target void registerPlatformInfo(OcPlatformInfo ocPlatformInfo)
     * @target void getPlatformInfo(
     * String host,
     * String platformUri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * OnPlatformFoundListener onPlatformFoundListener)
     * @test_data 1. OcPlatformInfo Platform information
     * 2. host = ""
     * 3. platformUri = "/oic/p"
     * 4. connectivityTypeSet Set of OcConnectivityType
     * 5. onPlatformFoundListener event handler
     * @pre_condition Configure platform for client server mode
     * @procedure 1. Perform registerPlatformInfo() API
     * 2. Perform getPlatformInfo() API
     * 3. Check if entity handler is called
     * 4. Check if registered information is received
     * @post_condition None
     * @expected Received platform info should be matched with Set platform info
     * @see void Configure(PlatformConfig platformConfig)
     * @since 2016-09-08
     */
    public void testRegisterAndGetPlatformInfo_GSRV_CV_P() {
        try {
            OcPlatform.registerPlatformInfo(m_riHelper.getPlatformInfo());
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }

        try {
            OcPlatform.getPlatformInfo(m_host, OcPlatform.WELL_KNOWN_PLATFORM_QUERY, EnumSet.of(OcConnectivityType.CT_DEFAULT), onPlatformInfoReceived);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }

        m_riHelper.waitInSecond(m_riHelper.CALLBACK_WAIT_DEFAULT);

        assertEquals("onPlatformInfoReceived is not called", true, m_isCallback);
        assertEquals("Did not find all platform information", true, m_foundAllPlatformInfo);
    }

    /**
     * @objective Test registerPlatformInfo() API and getPlatformInfo() API to check if
     * registered platform information is get
     * @target void registerPlatformInfo(OcPlatformInfo ocPlatformInfo)
     * @target void getPlatformInfo(
     * String host,
     * String platformUri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * OnPlatformFoundListener onPlatformFoundListener,
     * QualityOfService qualityOfService)
     * @test_data 1. OcPlatformInfo Platform information
     * 2. host = ""
     * 3. platformUri = "/oic/p"
     * 4. connectivityTypeSet Set of OcConnectivityType
     * 5. onPlatformFoundListener event handler
     * 6. QualityOfService High
     * @pre_condition Configure platform for client server mode
     * @procedure 1. Perform registerPlatformInfo() API
     * 2. Perform getPlatformInfo() API
     * 3. Check if entity handler is called
     * 4. Check if registered information is received
     * @post_condition None
     * @expected Received platform info should be matched with Set platform info
     * @see void Configure(PlatformConfig platformConfig)
     * @since 2016-09-08
     */
    public void testRegisterAndGetPlatformInfoWithQos_GSRV_CV_P() {
        try {
            OcPlatform.registerPlatformInfo(m_riHelper.getPlatformInfo());
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }

        try {
            OcPlatform.getPlatformInfo(m_host, OcPlatform.WELL_KNOWN_PLATFORM_QUERY, EnumSet.of(OcConnectivityType.CT_DEFAULT), onPlatformInfoReceived,
                    QualityOfService.HIGH);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }

        m_riHelper.waitInSecond(m_riHelper.CALLBACK_WAIT_DEFAULT);

        assertEquals("onPlatformInfoReceived is not called", true, m_isCallback);
        assertEquals("Did not find all platform information", true, m_foundAllPlatformInfo);
    }

    /**
     * @objective Test  registerResource() API and get() API positively to check
     * if get request is done successfully
     * @target OcResourceHandle registerResource( String resourceUri, String
     * resourceTypeName, String resourceInterface, EntityHandler
     * entityHandler, EnumSet<ResourceProperty> resourcePropertySet)
     * @target void get(Map<String, String> queryParamsMap,
     * OnGetListener onGetListener)
     * @test_data 1. resourceUri "/a/temperature"
     * 2. resourceTypeName "oic.r.temperature"
     * 3. resourceInterface DEFAULT_INTERFACE
     * 4. entityHandler entity handler
     * 5. resourcePropertySet indicates property of the resource
     * 6. queryParamsMap map with query paramter and value
     * `				7. onGetListener event handler
     * @pre_condition Configure platform for client server mode
     * @procedure 1. Perform registerResource() API
     * 2. Perform findResource() API with resource type in query
     * 3. Check if callback is called
     * 4. Check if temperature resource is found
     * 5. Perform get() API on the found temperature resource
     * 6. Check if server can get the get request and send response correctly
     * 7. Check if client's onGet handler is called and resource attributes are found correctly in response
     * @post_condition None
     * @expected onGet callback is called and response is found correctly
     * @see void Configure(PlatformConfig platformConfig)
     * @see void findResource(
     * String host,
     * String resourceUri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * OnResourceFoundListener onResourceFoundListener)
     * @since 2016-09-08
     */
    public void testCreateResourceAndSendGetRequest_SQV_CV_P() {
        m_resourceHandle = m_riHelper.registerResource(RIHelper.RESOURCE_URI_TEMPERATURE, RIHelper.RESOURCE_TYPE_TEMPERATURE);
        assertNotNull("Can not create resource", m_resourceHandle);

        try {
            OcPlatform.findResource(m_host, RIHelper.TEMPERATURE_RESOURCE_QUERY, EnumSet.of(OcConnectivityType.CT_DEFAULT), onResourceFound);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }

        m_riHelper.waitInSecond(m_riHelper.CALLBACK_WAIT_DEFAULT);

        assertEquals("OnResourceFoundListener is not called", true, m_isCallback);
        assertNotNull("Temperature Resource is not found", m_temperatureResource);

        try {
            m_temperatureResource.get(m_qpMap, onGet);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }

        m_riHelper.waitInSecond(m_riHelper.CALLBACK_WAIT_DEFAULT);

        if (m_riHelper.s_isServerOk == false) {
            fail(m_riHelper.s_errorMsg);
        }

        if (m_isGetCorrect == false) {
            fail("onGet Callback values are not correct");
        }
    }

    /**
     * @objective Test  registerResource() API and get() API(with qos) positively to check
     * if get request is done successfully
     * @target OcResourceHandle registerResource( String resourceUri, String
     * resourceTypeName, String resourceInterface, EntityHandler
     * entityHandler, EnumSet<ResourceProperty> resourcePropertySet)
     * @target void get(Map<String, String> queryParamsMap,
     * OnGetListener onGetListener, QualityOfService qualityOfService)
     * @test_data 1. resourceUri "/a/temperature"
     * 2. resourceTypeName "oic.r.temperature"
     * 3. resourceInterface DEFAULT_INTERFACE
     * 4. entityHandler entity handler
     * 5. resourcePropertySet indicates property of the resource
     * 6. queryParamsMap map with query paramter and value
     * `				7. onGetListener event handler
     * 8. qualityOfService High
     * @pre_condition Configure platform for client server mode
     * @procedure 1. Perform registerResource() API
     * 2. Perform findResource() API with resource type in query
     * 3. Check if callback is called
     * 4. Check if temperature resource is found
     * 5. Perform get() API(with qos) on the found temperature resource
     * 6. Check if server can get the get request and send response correctly
     * 7. Check if client's onGet handler is called and resource attributes are found correctly in response
     * @post_condition None
     * @expected onGet callback is called and response is found correctly
     * @see void Configure(PlatformConfig platformConfig)
     * @see void findResource(
     * String host,
     * String resourceUri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * OnResourceFoundListener onResourceFoundListener)
     * @since 2016-09-08
     */
    public void testCreateResourceAndSendGetRequestWithQos_SQV_CV_P() {
        m_resourceHandle = m_riHelper.registerResource(RIHelper.RESOURCE_URI_TEMPERATURE, RIHelper.RESOURCE_TYPE_TEMPERATURE);
        assertNotNull("Can not create resource", m_resourceHandle);

        try {
            OcPlatform.findResource(m_host, RIHelper.TEMPERATURE_RESOURCE_QUERY, EnumSet.of(OcConnectivityType.CT_DEFAULT), onResourceFound);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }

        m_riHelper.waitInSecond(m_riHelper.CALLBACK_WAIT_DEFAULT);

        assertEquals("OnResourceFoundListener is not called", true, m_isCallback);
        assertNotNull("Temperature Resource is not found", m_temperatureResource);

        try {
            m_temperatureResource.get(m_qpMap, onGet, QualityOfService.HIGH);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }

        m_riHelper.waitInSecond(m_riHelper.CALLBACK_WAIT_DEFAULT);

        if (m_riHelper.s_isServerOk == false) {
            fail(m_riHelper.s_errorMsg);
        }

        if (m_isGetCorrect == false) {
            fail("onGet Callback values are not correct");
        }
    }

    /**
     * @objective Test  registerResource() API and put() API positively to check
     * if put request is done successfully
     * @target OcResourceHandle registerResource( String resourceUri, String
     * resourceTypeName, String resourceInterface, EntityHandler
     * entityHandler, EnumSet<ResourceProperty> resourcePropertySet)
     * @target void put(OcRepresentation representation,
     * Map<String, String> queryParamsMap,
     * OnPutListener onPutListener)
     * @test_data 1. resourceUri "/a/temperature"
     * 2. resourceTypeName "oic.r.temperature"
     * 3. resourceInterface DEFAULT_INTERFACE
     * 4. entityHandler entity handler
     * 5. resourcePropertySet indicates property of the resource
     * 6. representation representation to set
     * 7. queryParamsMap map with query paramter and value
     * `				8. onPutListener event handler
     * @pre_condition Configure platform for client server mode
     * @procedure 1. Perform registerResource() API
     * 2. Perform findResource() API with resource type in query
     * 3. Check if callback is called
     * 4. Check if temperature resource is found
     * 5. Perform put() API on the found temperature resource
     * 6. Check if server can get the put request and send response correctly
     * 7. Check if client's onPut handler is called and resource attributes are found correctly in response
     * @post_condition None
     * @expected onGet callback is called and response is found correctly
     * @see void Configure(PlatformConfig platformConfig)
     * @see void findResource(
     * String host,
     * String resourceUri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * OnResourceFoundListener onResourceFoundListener)
     * @since 2016-09-08
     */
    public void testCreateResourceAndSendPutRequest_SQV_CV_P() {
        m_resourceHandle = m_riHelper.registerResource(RIHelper.RESOURCE_URI_TEMPERATURE, RIHelper.RESOURCE_TYPE_TEMPERATURE);
        assertNotNull("Can not create resource", m_resourceHandle);

        try {
            OcPlatform.findResource(m_host, RIHelper.TEMPERATURE_RESOURCE_QUERY, EnumSet.of(OcConnectivityType.CT_DEFAULT), onResourceFound);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }

        m_riHelper.waitInSecond(m_riHelper.CALLBACK_WAIT_DEFAULT);

        assertEquals("OnResourceFoundListener is not called", true, m_isCallback);
        assertNotNull("Temperature Resource is not found", m_temperatureResource);


        OcRepresentation rep = new OcRepresentation();
        try {
            rep.setValue(RIHelper.KEY_TEMPERATURE, m_setTemp);
            rep.setValue(RIHelper.KEY_HOUR, m_setHour);
        } catch (OcException e1) {
            fail("Exception occured inside setValue for OcRepresentation");
            e1.printStackTrace();
        }

        try {
            m_temperatureResource.put(rep, m_qpMap, onPut);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }

        m_riHelper.waitInSecond(m_riHelper.CALLBACK_WAIT_DEFAULT);

        if (m_riHelper.s_isServerOk == false) {
            fail(m_riHelper.s_errorMsg);
        }

        if (m_isPutCorrect == false) {
            fail("onPut Callback values are not correct");
        }
    }

    /**
     * @objective Test  registerResource() API and put() API(with qos) positively to check
     * if put request is done successfully
     * @target OcResourceHandle registerResource( String resourceUri, String
     * resourceTypeName, String resourceInterface, EntityHandler
     * entityHandler, EnumSet<ResourceProperty> resourcePropertySet)
     * @target void put(OcRepresentation representation,
     * Map<String, String> queryParamsMap,
     * OnPutListener onPutListener,
     * QualityOfService qualityOfService)
     * @test_data 1. resourceUri "/a/temperature"
     * 2. resourceTypeName "oic.r.temperature"
     * 3. resourceInterface DEFAULT_INTERFACE
     * 4. entityHandler entity handler
     * 5. resourcePropertySet indicates property of the resource
     * 6. representation representation to set
     * 7. queryParamsMap map with query paramter and value
     * `				8. onPutListener event handler
     * 9. qualityOfService High
     * @pre_condition Configure platform for client server mode
     * @procedure 1. Perform registerResource() API
     * 2. Perform findResource() API with resource type in query
     * 3. Check if callback is called
     * 4. Check if temperature resource is found
     * 5. Perform put() API(with qos) on the found temperature resource
     * 6. Check if server can get the put request and send response correctly
     * 7. Check if client's onPut handler is called and resource attributes are found correctly in response
     * @post_condition None
     * @expected onGet callback is called and response is found correctly
     * @see void Configure(PlatformConfig platformConfig)
     * @see void findResource(
     * String host,
     * String resourceUri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * OnResourceFoundListener onResourceFoundListener)
     * @since 2016-09-08
     */
    public void testCreateResourceAndSendPutRequestWithQos_SQV_CV_P() {
        m_resourceHandle = m_riHelper.registerResource(RIHelper.RESOURCE_URI_TEMPERATURE, RIHelper.RESOURCE_TYPE_TEMPERATURE);
        assertNotNull("Can not create resource", m_resourceHandle);

        try {
            OcPlatform.findResource(m_host, RIHelper.TEMPERATURE_RESOURCE_QUERY, EnumSet.of(OcConnectivityType.CT_DEFAULT), onResourceFound);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }

        m_riHelper.waitInSecond(m_riHelper.CALLBACK_WAIT_DEFAULT);

        assertEquals("OnResourceFoundListener is not called", true, m_isCallback);
        assertNotNull("Temperature Resource is not found", m_temperatureResource);


        OcRepresentation rep = new OcRepresentation();
        try {
            rep.setValue(RIHelper.KEY_TEMPERATURE, m_setTemp);
            rep.setValue(RIHelper.KEY_HOUR, m_setHour);
        } catch (OcException e1) {
            fail("Exception occured inside setValue for OcRepresentation");
            e1.printStackTrace();
        }

        try {
            m_temperatureResource.put(rep, m_qpMap, onPut);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }

        m_riHelper.waitInSecond(m_riHelper.CALLBACK_WAIT_DEFAULT);

        if (m_riHelper.s_isServerOk == false) {
            fail(m_riHelper.s_errorMsg);
        }

        if (m_isPutCorrect == false) {
            fail("onPut Callback values are not correct");
        }
    }

    /**
     * @objective Test  registerResource() API and post() API positively to check
     * if post request is done successfully
     * @target OcResourceHandle registerResource( String resourceUri, String
     * resourceTypeName, String resourceInterface, EntityHandler
     * entityHandler, EnumSet<ResourceProperty> resourcePropertySet)
     * @target void post(OcRepresentation ocRepresentation,
     * Map<String, String> queryParamsMap,
     * OnPostListener onPostListener)
     * @test_data 1. resourceUri "/a/temperature"
     * 2. resourceTypeName "oic.r.temperature"
     * 3. resourceInterface DEFAULT_INTERFACE
     * 4. entityHandler entity handler
     * 5. resourcePropertySet indicates property of the resource
     * 6. representation representation to set
     * 7. queryParamsMap map with query paramter and value
     * `				8. onPostListener event handler
     * @pre_condition Configure platform for client server mode
     * @procedure 1. Perform registerResource() API
     * 2. Perform findResource() API with resource type in query
     * 3. Check if callback is called
     * 4. Check if temperature resource is found
     * 5. Perform post() API on the found temperature resource
     * 6. Check if server can get the post request and send response correctly
     * 7. Check if client's onPost handler is called and resource attributes are found correctly in response
     * @post_condition None
     * @expected onGet callback is called and response is found correctly
     * @see void Configure(PlatformConfig platformConfig)
     * @see void findResource(
     * String host,
     * String resourceUri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * OnResourceFoundListener onResourceFoundListener)
     * @since 2016-09-08
     */
    public void testCreateResourceAndSendPostRequest_SQV_CV_P() {
        m_resourceHandle = m_riHelper.registerResource(RIHelper.RESOURCE_URI_TEMPERATURE, RIHelper.RESOURCE_TYPE_TEMPERATURE);
        assertNotNull("Can not create resource", m_resourceHandle);

        try {
            OcPlatform.findResource(m_host, RIHelper.TEMPERATURE_RESOURCE_QUERY, EnumSet.of(OcConnectivityType.CT_DEFAULT), onResourceFound);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }

        m_riHelper.waitInSecond(m_riHelper.CALLBACK_WAIT_DEFAULT);

        assertEquals("OnResourceFoundListener is not called", true, m_isCallback);
        assertNotNull("Temperature Resource is not found", m_temperatureResource);


        OcRepresentation rep = new OcRepresentation();
        try {
            rep.setValue(RIHelper.KEY_TEMPERATURE, m_setTemp);
            rep.setValue(RIHelper.KEY_HOUR, m_setHour);
        } catch (OcException e1) {
            fail("Exception occured inside setValue for OcRepresentation");
            e1.printStackTrace();
        }

        try {
            m_temperatureResource.post(rep, m_qpMap, onPost);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }

        m_riHelper.waitInSecond(m_riHelper.CALLBACK_WAIT_DEFAULT);

        if (m_riHelper.s_isServerOk == false) {
            fail(m_riHelper.s_errorMsg);
        }

        if (m_isPostCorrect == false) {
            fail("onPost Callback values are not correct");
        }
    }

    /**
     * @objective Test  registerResource() API and post() API(with qos) positively to check
     * if post request is done successfully
     * @target OcResourceHandle registerResource( String resourceUri, String
     * resourceTypeName, String resourceInterface, EntityHandler
     * entityHandler, EnumSet<ResourceProperty> resourcePropertySet)
     * @target void post(OcRepresentation ocRepresentation,
     * Map<String, String> queryParamsMap,
     * OnPostListener onPostListener,
     * QualityOfService qualityOfService)
     * @test_data 1. resourceUri "/a/temperature"
     * 2. resourceTypeName "oic.r.temperature"
     * 3. resourceInterface DEFAULT_INTERFACE
     * 4. entityHandler entity handler
     * 5. resourcePropertySet indicates property of the resource
     * 6. representation representation to set
     * 7. queryParamsMap map with query paramter and value
     * `				8. onPostListener event handler
     * 9. qualityOfService High
     * @pre_condition Configure platform for client server mode
     * @procedure 1. Perform registerResource() API
     * 2. Perform findResource() API with resource type in query
     * 3. Check if callback is called
     * 4. Check if temperature resource is found
     * 5. Perform post() API(with qos) on the found temperature resource
     * 6. Check if server can get the post request and send response correctly
     * 7. Check if client's onPost handler is called and resource attributes are found correctly in response
     * @post_condition None
     * @expected onGet callback is called and response is found correctly
     * @see void Configure(PlatformConfig platformConfig)
     * @see void findResource(
     * String host,
     * String resourceUri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * OnResourceFoundListener onResourceFoundListener)
     * @since 2016-09-08
     */
    public void testCreateResourceAndSendPostRequestWithQos_SQV_CV_P() {
        m_resourceHandle = m_riHelper.registerResource(RIHelper.RESOURCE_URI_TEMPERATURE, RIHelper.RESOURCE_TYPE_TEMPERATURE);
        assertNotNull("Can not create resource", m_resourceHandle);

        try {
            OcPlatform.findResource(m_host, RIHelper.TEMPERATURE_RESOURCE_QUERY, EnumSet.of(OcConnectivityType.CT_DEFAULT), onResourceFound);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }

        m_riHelper.waitInSecond(m_riHelper.CALLBACK_WAIT_DEFAULT);

        assertEquals("OnResourceFoundListener is not called", true, m_isCallback);
        assertNotNull("Temperature Resource is not found", m_temperatureResource);


        OcRepresentation rep = new OcRepresentation();
        try {
            rep.setValue(RIHelper.KEY_TEMPERATURE, m_setTemp);
            rep.setValue(RIHelper.KEY_HOUR, m_setHour);
        } catch (OcException e1) {
            fail("Exception occured inside setValue for OcRepresentation");
            e1.printStackTrace();
        }

        try {
            m_temperatureResource.post(rep, m_qpMap, onPost, QualityOfService.HIGH);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }

        m_riHelper.waitInSecond(m_riHelper.CALLBACK_WAIT_DEFAULT);

        if (m_riHelper.s_isServerOk == false) {
            fail(m_riHelper.s_errorMsg);
        }

        if (m_isPostCorrect == false) {
            fail("onPost Callback values are not correct");
        }
    }

    /**
     * @objective Test  registerResource() API and deleteResource() API positively to check
     * if delete request is done successfully
     * @target OcResourceHandle registerResource( String resourceUri, String
     * resourceTypeName, String resourceInterface, EntityHandler
     * entityHandler, EnumSet<ResourceProperty> resourcePropertySet)
     * @target void deleteResource(OnDeleteListener onDeleteListener)
     * @test_data 1. resourceUri "/a/temperature"
     * 2. resourceTypeName "oic.r.temperature"
     * 3. resourceInterface DEFAULT_INTERFACE
     * 4. entityHandler entity handler
     * 5. resourcePropertySet indicates property of the resource
     * 6. onDeleteListener event handler
     * @pre_condition Configure platform for client server mode
     * @procedure 1. Perform registerResource() API
     * 2. Perform findResource() API with resource type in query
     * 3. Check if callback is called
     * 4. Check if temperature resource is found
     * 5. Perform deleteResource() API on the found temperature resource
     * 6. Check if server can get the delete request and send response correctly
     * 7. Call unregisterResource() API inside server's entity handler
     * 8. Check if client's onDelete callback is called
     * 9. Call findResource() API again with resource type in query
     * 10. Check if resource is not found now(after delete operation)
     * @post_condition None
     * @expected onDelete callback is called and response is found correctly and resource is not found after delete operation
     * @see void Configure(PlatformConfig platformConfig)
     * @see void findResource(
     * String host,
     * String resourceUri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * OnResourceFoundListener onResourceFoundListener)
     * @since 2016-09-08
     */
    public void testCreateResourceAndSendDeleteRequest_SQV_CV_P() {
        m_resourceHandle = m_riHelper.registerResource(RIHelper.RESOURCE_URI_TEMPERATURE, RIHelper.RESOURCE_TYPE_TEMPERATURE);
        assertNotNull("Can not create resource", m_resourceHandle);

        try {
            OcPlatform.findResource(m_host, RIHelper.TEMPERATURE_RESOURCE_QUERY, EnumSet.of(OcConnectivityType.CT_DEFAULT), onResourceFound);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }

        m_riHelper.waitInSecond(m_riHelper.CALLBACK_WAIT_DEFAULT);

        assertEquals("OnResourceFoundListener is not called", true, m_isCallback);
        assertNotNull("Temperature Resource is not found", m_temperatureResource);

        try {
            m_temperatureResource.deleteResource(onDelete);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }

        m_riHelper.waitInSecond(m_riHelper.CALLBACK_WAIT_DEFAULT);

        if (m_riHelper.s_isServerOk == false) {
            fail(m_riHelper.s_errorMsg);
        }

        if (m_isDeleteCorrect == false) {
            fail("onDelete Callback was not called");
        }

        m_temperatureResource = null;

        try {
            OcPlatform.findResource(m_host, RIHelper.TEMPERATURE_RESOURCE_QUERY, EnumSet.of(OcConnectivityType.CT_DEFAULT), onResourceFound);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }

        m_riHelper.waitInSecond(m_riHelper.CALLBACK_WAIT_DEFAULT);

        assertNull("Temperature Resource should not be found after delete operation", m_temperatureResource);
    }

    /**
     * @objective Test  registerResource() API and deleteResource() API(with qos) positively to check
     * if delete request is done successfully
     * @target OcResourceHandle registerResource( String resourceUri, String
     * resourceTypeName, String resourceInterface, EntityHandler
     * entityHandler, EnumSet<ResourceProperty> resourcePropertySet)
     * @target void deleteResource(OnDeleteListener onDeleteListener,QualityOfService qualityOfService)
     * @test_data 1. resourceUri "/a/temperature"
     * 2. resourceTypeName "oic.r.temperature"
     * 3. resourceInterface DEFAULT_INTERFACE
     * 4. entityHandler entity handler
     * 5. resourcePropertySet indicates property of the resource
     * 6. onDeleteListener event handler
     * 7. qualityOfService High
     * @pre_condition Configure platform for client server mode
     * @procedure 1. Perform registerResource() API
     * 2. Perform findResource() API with resource type in query
     * 3. Check if callback is called
     * 4. Check if temperature resource is found
     * 5. Perform deleteResource() API(with qos) on the found temperature resource
     * 6. Check if server can get the delete request and send response correctly
     * 7. Call unregisterResource() API inside server's entity handler
     * 8. Check if client's onDelete callback is called
     * 9. Call findResource() API again with resource type in query
     * 10. Check if resource is not found now(after delete operation)
     * @post_condition None
     * @expected onDelete callback is called and response is found correctly and resource is not found after delete operation
     * @see void Configure(PlatformConfig platformConfig)
     * @see void findResource(
     * String host,
     * String resourceUri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * OnResourceFoundListener onResourceFoundListener)
     * @since 2016-09-08
     */
    public void testCreateResourceAndSendDeleteRequestWithQos_SQV_CV_P() {
        m_resourceHandle = m_riHelper.registerResource(RIHelper.RESOURCE_URI_TEMPERATURE, RIHelper.RESOURCE_TYPE_TEMPERATURE);
        assertNotNull("Can not create resource", m_resourceHandle);

        try {
            OcPlatform.findResource(m_host, RIHelper.TEMPERATURE_RESOURCE_QUERY, EnumSet.of(OcConnectivityType.CT_DEFAULT), onResourceFound);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }

        m_riHelper.waitInSecond(m_riHelper.CALLBACK_WAIT_DEFAULT);

        assertEquals("OnResourceFoundListener is not called", true, m_isCallback);
        assertNotNull("Temperature Resource is not found", m_temperatureResource);

        try {
            m_temperatureResource.deleteResource(onDelete, QualityOfService.HIGH);
            ;
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }

        m_riHelper.waitInSecond(m_riHelper.CALLBACK_WAIT_DEFAULT);

        if (m_riHelper.s_isServerOk == false) {
            fail(m_riHelper.s_errorMsg);
        }

        if (m_isDeleteCorrect == false) {
            fail("onDelete Callback was not called");
        }

        m_temperatureResource = null;

        try {
            OcPlatform.findResource(m_host, RIHelper.TEMPERATURE_RESOURCE_QUERY, EnumSet.of(OcConnectivityType.CT_DEFAULT), onResourceFound);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }

        m_riHelper.waitInSecond(m_riHelper.CALLBACK_WAIT_DEFAULT);

        assertNull("Temperature Resource should not be found after delete operation", m_temperatureResource);
    }
}