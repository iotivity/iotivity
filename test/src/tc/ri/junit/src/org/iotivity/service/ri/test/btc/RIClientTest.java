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

package org.iotivity.service.ri.test.btc;

import java.util.ArrayList;
import java.util.EnumSet;
import java.util.List;

import org.iotivity.base.EntityHandlerResult;
import org.iotivity.base.ModeType;
import org.iotivity.base.OcConnectivityType;
import org.iotivity.base.OcDeviceInfo;
import org.iotivity.base.OcException;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcPlatform.OnResourceFoundListener;
import org.iotivity.base.OcPlatform.OnDeviceFoundListener;
import org.iotivity.base.OcPlatform.OnPlatformFoundListener;
import org.iotivity.base.OcPlatform.OnPresenceListener;
import org.iotivity.base.OcPlatformInfo;
import org.iotivity.base.OcPlatformStatus;
import org.iotivity.base.OcPresenceStatus;
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
import org.iotivity.base.OcPresenceHandle;
import org.iotivity.service.ri.test.helper.RIHelper;

import android.test.InstrumentationTestCase;
import android.util.Log;

public class RIClientTest extends InstrumentationTestCase {
    private final String LOG_TAG = this.getClass().getSimpleName();

    private RIHelper m_riHelper;
    private String m_host = "";
    private OcPresenceHandle m_presenceHandle;

    protected void setUp() {
        m_riHelper = RIHelper.getInstance(getInstrumentation()
                .getTargetContext());
        m_presenceHandle = null;
        Log.i(LOG_TAG, "SetUp Finished");
    }

    protected void tearDown() {
        Log.i(LOG_TAG, "tearDown Finished");
    }

    OnResourceFoundListener onResourceFound = new OnResourceFoundListener() {
        @Override
        public void onResourceFound(OcResource arg0) {
            Log.i(LOG_TAG, "Inside onResourceFound");
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
        }
    };

    OnPlatformFoundListener onPlatformInfoReceived = new OnPlatformFoundListener() {

        @Override
        public void onPlatformFound(OcRepresentation rep) {
            Log.i(LOG_TAG, "Inside onPlatformFound");
        }
    };

    OnPresenceListener onPresenceReceived = new OnPresenceListener() {

        @Override
        public void onPresence(OcPresenceStatus arg0, int arg1, String arg2) {
            Log.i(LOG_TAG, "Inside onPresenceReceived");
        }
    };

    /**
     * @objective Test Configure function with positive basic way with InProc
     * ServiceType
     * @target void Configure(PlatformConfig platformConfig)
     * @test_data PlatformConfig platform configuration
     * @pre_condition configure with InProc ServiceType
     * @procedure Call Configure() API
     * @post_condition None
     * @expected No exceptions should occur
     * @see None
     * @since 2016-08-30
     **/
    public void testConfigureClient_SRC_P() {
        try {
            PlatformConfig cfg = new PlatformConfig(getInstrumentation()
                    .getTargetContext(), ServiceType.IN_PROC,
                    ModeType.CLIENT, "0.0.0.0", 0, QualityOfService.HIGH);
            OcPlatform.Configure(cfg);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @objective Test findResource function with positive basic way
     * @target void findResource(
     * String host,
     * String resourceUri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * OnResourceFoundListener onResourceFoundListener)
     * @test_data 1. host = ""
     * 2. resourceUri = "oic.r.temperature"
     * 3. connectivityTypeSet Set of OcConnectivityType
     * 4. onResourceFoundListener event handler
     * @pre_condition Configure platform
     * @procedure Call findResource() API
     * @post_condition None
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @since 2016-08-30
     **/
    public void testFindResource_SRC_P() {
        m_riHelper.configClientServerPlatform();

        try {
            OcPlatform.findResource(m_host, OcPlatform.WELL_KNOWN_QUERY, EnumSet.of(OcConnectivityType.CT_DEFAULT), onResourceFound);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @objective Test findResource function with negative basic way
     * using null resourceuri
     * @target void findResource(
     * String host,
     * String resourceUri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * OnResourceFoundListener onResourceFoundListener)
     * @test_data 1. host = ""
     * 2. resourceUri = null
     * 3. connectivityTypeSet Set of OcConnectivityType
     * 4. onResourceFoundListener event handler
     * @pre_condition Configure platform
     * @procedure Call findResource() API
     * @post_condition None
     * @expected OcException should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @since 2016-08-30
     **/
    public void testFindResourceResourceUri_NV_N() {
        m_riHelper.configClientServerPlatform();

        try {
            OcPlatform.findResource(m_host, null, EnumSet.of(OcConnectivityType.CT_DEFAULT), onResourceFound);
            fail("OcException should occur");
        } catch (OcException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
            fail("Unexpected exception occured");
        }
    }

    /**
     * @objective Test findResource function with negative basic way
     * using null event handler
     * @target void findResource(
     * String host,
     * String resourceUri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * OnResourceFoundListener onResourceFoundListener)
     * @test_data 1. host = ""
     * 2. resourceUri = "oic.r.temperature"
     * 3. connectivityTypeSet Set of OcConnectivityType
     * 4. onResourceFoundListener null
     * @pre_condition Configure platform
     * @procedure Call findResource() API
     * @post_condition None
     * @expected OcException should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @since 2016-08-30
     **/
    public void testFindResourceListener_NV_N() {
        m_riHelper.configClientServerPlatform();

        try {
            OcPlatform.findResource(m_host, OcPlatform.WELL_KNOWN_QUERY, EnumSet.of(OcConnectivityType.CT_DEFAULT), null);
            fail("OcException should occur");
        } catch (OcException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
            fail("Unexpected exception occured");
        }
    }

    /**
     * @objective Test findResource() function (with qos high) with positive basic way
     * @target void findResource(
     * String host,
     * String resourceUri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * OnResourceFoundListener onResourceFoundListener,
     * QualityOfService qualityOfService)
     * @test_data 1. host = ""
     * 2. resourceUri = "oic.r.temperature"
     * 3. connectivityTypeSet Set of OcConnectivityType
     * 4. onResourceFoundListener event handler
     * 5. qualityOfService High
     * @pre_condition Configure platform
     * @procedure Call findResource() API(with high qos)
     * @post_condition None
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @since 2016-08-30
     **/
    public void testFindResourceWithHighQos_SRC_P() {
        m_riHelper.configClientServerPlatform();
        try {
            OcPlatform.findResource(m_host, OcPlatform.WELL_KNOWN_QUERY, EnumSet.of(OcConnectivityType.CT_DEFAULT), onResourceFound,
                    QualityOfService.HIGH);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @objective Test findResource() function (with qos low) with positive basic way
     * @target void findResource(
     * String host,
     * String resourceUri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * OnResourceFoundListener onResourceFoundListener,
     * QualityOfService qualityOfService)
     * @test_data 1. host = ""
     * 2. resourceUri = "oic.r.temperature"
     * 3. connectivityTypeSet Set of OcConnectivityType
     * 4. onResourceFoundListener event handler
     * 5. qualityOfService Low
     * @pre_condition Configure platform
     * @procedure Call findResource() API(with low qos)
     * @post_condition None
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @since 2016-08-30
     **/
    public void testFindResourceWithLowQos_SRC_P() {
        m_riHelper.configClientServerPlatform();
        try {
            OcPlatform.findResource(m_host, OcPlatform.WELL_KNOWN_QUERY, EnumSet.of(OcConnectivityType.CT_DEFAULT), onResourceFound,
                    QualityOfService.HIGH);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @objective Test getDeviceInfo function with positive basic way
     * @target void getDeviceInfo(
     * String host,
     * String deviceUri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * OnDeviceFoundListener onDeviceFoundListener)
     * @test_data 1. host = ""
     * 2. deviceUri = "/oic/d"
     * 3. connectivityTypeSet Set of OcConnectivityType
     * 4. OnDeviceFoundListener event handler
     * @pre_condition Configure platform
     * @procedure Call getDeviceInfo() API
     * @post_condition None
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @since 2016-08-30
     **/
    public void testGetDeviceInfo_SRC_P() {
        m_riHelper.configClientServerPlatform();

        try {
            OcPlatform.getDeviceInfo(m_host, OcPlatform.WELL_KNOWN_DEVICE_QUERY, EnumSet.of(OcConnectivityType.CT_DEFAULT), onDeviceInfoReceived);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @objective Test getDeviceInfo function with negative basic way
     * using null deviceuri
     * @target void getDeviceInfo(
     * String host,
     * String deviceUri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * OnDeviceFoundListener onDeviceFoundListener)
     * @test_data 1. host = ""
     * 2. deviceUri = null
     * 3. connectivityTypeSet Set of OcConnectivityType
     * 4. OnDeviceFoundListener event handler
     * @pre_condition Configure platform
     * @procedure Call getDeviceInfo() API
     * @post_condition None
     * @expected OcException should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @since 2016-08-30
     **/
    public void testGetDeviceInfoDeviceUri_NV_N() {
        m_riHelper.configClientServerPlatform();

        try {
            OcPlatform.getDeviceInfo(m_host, null, EnumSet.of(OcConnectivityType.CT_DEFAULT), onDeviceInfoReceived);
            fail("OcException should occur");
        } catch (OcException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
            fail("Unexpected exception occured");
        }
    }

    /**
     * @objective Test getDeviceInfo function with negative basic way
     * using null event handler
     * @target void getDeviceInfo(
     * String host,
     * String deviceUri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * OnDeviceFoundListener onDeviceFoundListener)
     * @test_data 1. host = ""
     * 2. resourceUri = "/oic/d"
     * 3. connectivityTypeSet Set of OcConnectivityType
     * 4. onDeviceFoundListener null
     * @pre_condition Configure platform
     * @procedure Call getDeviceInfo() API
     * @post_condition None
     * @expected OcException should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @since 2016-08-30
     **/
    public void testGetDeviceInfoListener_NV_N() {
        m_riHelper.configClientServerPlatform();

        try {
            OcPlatform.getDeviceInfo(m_host, OcPlatform.WELL_KNOWN_DEVICE_QUERY, EnumSet.of(OcConnectivityType.CT_DEFAULT), null);
            fail("OcException should occur");
        } catch (OcException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
            fail("Unexpected exception occured");
        }
    }

    /**
     * @objective Test getDeviceInfo() function (with qos high) with positive basic way
     * @target void getDeviceInfo(
     * String host,
     * String deviceUri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * OnDeviceFoundListener onDeviceFoundListener,
     * QualityOfService qualityOfService)
     * @test_data 1. host = ""
     * 2. deviceUri = "/oic/d"
     * 3. connectivityTypeSet Set of OcConnectivityType
     * 4. onResourceFoundListener event handler
     * 5. qualityOfService High
     * @pre_condition Configure platform
     * @procedure Call getDeviceInfo() API(with high qos)
     * @post_condition None
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @since 2016-08-30
     **/
    public void testGetDeviceInfoWithHighQos_SRC_P() {
        m_riHelper.configClientServerPlatform();
        try {
            OcPlatform.getDeviceInfo(m_host, OcPlatform.WELL_KNOWN_DEVICE_QUERY, EnumSet.of(OcConnectivityType.CT_DEFAULT), onDeviceInfoReceived,
                    QualityOfService.HIGH);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @objective Test getDeviceInfo() function (with qos low) with positive basic way
     * @target void getDeviceInfo(
     * String host,
     * String deviceUri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * OnDeviceFoundListener onDeviceFoundListener,
     * QualityOfService qualityOfService)
     * @test_data 1. host = ""
     * 2. deviceUri = "/oic/d"
     * 3. connectivityTypeSet Set of OcConnectivityType
     * 4. onDeviceFoundListener event handler
     * 5. qualityOfService Low
     * @pre_condition Configure platform
     * @procedure Call getDeviceInfo() API(with low qos)
     * @post_condition None
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @since 2016-08-30
     **/
    public void testGetDeviceInfoWithLowQos_SRC_P() {
        m_riHelper.configClientServerPlatform();
        try {
            OcPlatform.getDeviceInfo(m_host, OcPlatform.WELL_KNOWN_DEVICE_QUERY, EnumSet.of(OcConnectivityType.CT_DEFAULT), onDeviceInfoReceived,
                    QualityOfService.HIGH);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @objective Test getPlatformInfo function with positive basic way
     * @target void getPlatformInfo(
     * String host,
     * String platformUri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * OnPlatformFoundListener onPlatformFoundListener)
     * @test_data 1. host = ""
     * 2. platformUri = "/oic/p"
     * 3. connectivityTypeSet Set of OcConnectivityType
     * 4. onPlatformFoundListener event handler
     * @pre_condition Configure platform
     * @procedure Call getPlatformInfo() API
     * @post_condition None
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @since 2016-08-30
     **/
    public void testGetPlatfromInfo_SRC_P() {
        m_riHelper.configClientServerPlatform();

        try {
            OcPlatform.getPlatformInfo(m_host, OcPlatform.WELL_KNOWN_PLATFORM_QUERY, EnumSet.of(OcConnectivityType.CT_DEFAULT), onPlatformInfoReceived);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @objective Test getPlatformInfo function with negative basic way
     * using null platformUri
     * @target void getPlatformInfo(
     * String host,
     * String platformUri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * OnPlatformFoundListener onPlatformFoundListener)
     * @test_data 1. host = ""
     * 2. platformUri = null
     * 3. connectivityTypeSet Set of OcConnectivityType
     * 4. onPlatformFoundListener event handler
     * @pre_condition Configure platform
     * @procedure Call getPlatformInfo() API
     * @post_condition None
     * @expected OcException should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @since 2016-08-30
     **/
    public void testGetPlatformInfoDeviceUri_NV_N() {
        m_riHelper.configClientServerPlatform();

        try {
            OcPlatform.getPlatformInfo(m_host, null, EnumSet.of(OcConnectivityType.CT_DEFAULT), onPlatformInfoReceived);
            fail("OcException should occur");
        } catch (OcException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
            fail("Unexpected exception occured");
        }
    }

    /**
     * @objective Test getPlatformInfo function with negative basic way
     * using null event handler
     * @target void getPlatformInfo(
     * String host,
     * String platformUri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * OnPlatformFoundListener onPlatformFoundListener)
     * @test_data 1. host = ""
     * 2. platformUri = "/oic/p"
     * 3. connectivityTypeSet Set of OcConnectivityType
     * 4. onPlatformFoundListener null
     * @pre_condition Configure platform
     * @procedure Call getPlatformInfo() API
     * @post_condition None
     * @expected OcException should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @since 2016-08-30
     **/
    public void testGetPlatformInfoListener_NV_N() {
        m_riHelper.configClientServerPlatform();

        try {
            OcPlatform.getPlatformInfo(m_host, OcPlatform.WELL_KNOWN_PLATFORM_QUERY, EnumSet.of(OcConnectivityType.CT_DEFAULT), null);
            fail("OcException should occur");
        } catch (OcException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
            fail("Unexpected exception occured");
        }
    }

    /**
     * @objective Test getPlatformInfo() function (with qos high) with positive basic way
     * @target void getPlatformInfo(
     * String host,
     * String platformUri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * OnPlatformFoundListener onPlatformFoundListener,
     * QualityOfService qualityOfService)
     * @test_data 1. host = ""
     * 2. platformUri = "/oic/p"
     * 3. connectivityTypeSet Set of OcConnectivityType
     * 4. onPlatformFoundListener event handler
     * 5. qualityOfService High
     * @pre_condition Configure platform
     * @procedure Call getPlatformInfo() API(with high qos)
     * @post_condition None
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @since 2016-08-30
     **/
    public void testGetPlatformInfoWithHighQos_SRC_P() {
        m_riHelper.configClientServerPlatform();
        try {
            OcPlatform.getPlatformInfo(m_host, OcPlatform.WELL_KNOWN_PLATFORM_QUERY, EnumSet.of(OcConnectivityType.CT_DEFAULT), onPlatformInfoReceived,
                    QualityOfService.HIGH);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @objective Test getPlatformInfo() function (with qos low) with positive basic way
     * @target void getPlatformInfo(
     * String host,
     * String platformUri,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * OnPlatformFoundListener onPlatformFoundListener,
     * QualityOfService qualityOfService)
     * @test_data 1. host = ""
     * 2. platformUri = "/oic/p"
     * 3. connectivityTypeSet Set of OcConnectivityType
     * 4. onPlatformFoundListener event handler
     * 5. qualityOfService Low
     * @pre_condition Configure platform
     * @procedure Call getPlatformInfo() API(with low qos)
     * @post_condition None
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @since 2016-08-30
     **/
    public void testGetPlatformInfoWithLowQos_SRC_P() {
        m_riHelper.configClientServerPlatform();
        try {
            OcPlatform.getPlatformInfo(m_host, OcPlatform.WELL_KNOWN_PLATFORM_QUERY, EnumSet.of(OcConnectivityType.CT_DEFAULT), onPlatformInfoReceived,
                    QualityOfService.HIGH);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @objective Test subscribePresence function with positive basic way
     * @target OcPresenceHandle subscribePresence(
     * String host,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * OnPresenceListener onPresenceListener)
     * @test_data 1. host = ""
     * 2. connectivityTypeSet Set of OcConnectivityType
     * 3. onPresenceListener event handler
     * @pre_condition Configure platform
     * @procedure Call subscribePresence() API
     * @post_condition None
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @since 2016-08-30
     **/
    public void testSubscribePresence_SRC_P() {
        m_riHelper.configClientServerPlatform();

        try {
            m_presenceHandle = OcPlatform.subscribePresence(m_host, EnumSet.of(OcConnectivityType.CT_DEFAULT), onPresenceReceived);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
        assertNotNull("subscribePresence could not create presence handle", m_presenceHandle);
    }

    /**
     * @objective Test subscribePresence function with negative basic way
     * using null event handler
     * @target OcPresenceHandle subscribePresence(
     * String host,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * OnPresenceListener onPresenceListener)
     * @test_data 1. host = ""
     * 2. connectivityTypeSet Set of OcConnectivityType
     * 3. onPresenceListener null
     * @pre_condition Configure platform
     * @procedure Call subscribePresence() API
     * @post_condition None
     * @expected OcException should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @since 2016-08-30
     **/
    public void testSubscribePresenceHandle_NV_N() {
        m_riHelper.configClientServerPlatform();

        try {
            m_presenceHandle = OcPlatform.subscribePresence(m_host, EnumSet.of(OcConnectivityType.CT_DEFAULT), null);
            fail("OcException should occur");
        } catch (OcException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
            fail("Unexpected exception occured");
        }
    }

    /**
     * @objective Test subscribePresence function(with resource type) with positive basic way
     * @target OcPresenceHandle subscribePresence(
     * String host,
     * String resourceType,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * OnPresenceListener onPresenceListener)
     * @test_data 1. host = ""
     * 2. resourceType = "oic.r.temperature"
     * 3. connectivityTypeSet Set of OcConnectivityType
     * 4. onPresenceListener event handler
     * @pre_condition Configure platform
     * @procedure Call subscribePresence() API
     * @post_condition None
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @since 2016-08-30
     **/
    public void testSubscribePresenceWithResourceType_SRC_P() {
        m_riHelper.configClientServerPlatform();

        try {
            m_presenceHandle = OcPlatform.subscribePresence(m_host, RIHelper.RESOURCE_TYPE_TEMPERATURE,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT), onPresenceReceived);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
        assertNotNull("subscribePresence could not create presence handle", m_presenceHandle);
    }

    /**
     * @objective Test subscribePresence function(with resource type) with negative basic way
     * using null event handler
     * @target OcPresenceHandle subscribePresence(
     * String host,
     * String resourceType,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * OnPresenceListener onPresenceListener)
     * @test_data 1. host = ""
     * 2. resourceType = "oic.r.temperature"
     * 3. connectivityTypeSet Set of OcConnectivityType
     * 4. onPresenceListener null
     * @pre_condition Configure platform
     * @procedure Call subscribePresence() API
     * @post_condition None
     * @expected OcException should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @since 2016-08-30
     **/
    public void testSubscribePresenceWithResourceTypeHandler_NV_N() {
        m_riHelper.configClientServerPlatform();

        try {
            m_presenceHandle = OcPlatform.subscribePresence(m_host, RIHelper.RESOURCE_TYPE_TEMPERATURE,
                    EnumSet.of(OcConnectivityType.CT_DEFAULT), null);
            fail("OcException should occur");
        } catch (OcException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
            fail("Unexpected exception occured");
        }
    }

    /**
     * @objective Test unsubscribePresence function with positive basic way
     * @target void unsubscribePresence(
     * OcPresenceHandle ocPresenceHandle)
     * @test_data ocPresenceHandle the handle object provided by the subscribePresence call
     * @pre_condition 1. Configure platform
     * 2. Call subscribePresence() API
     * @procedure Call unsubscribePresence() API
     * @post_condition None
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcPresenceHandle subscribePresence(
     * String host,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * OnPresenceListener onPresenceListener)
     * @since 2016-08-30
     **/
    public void testUnsubscribePresence_SRC_P() {
        m_riHelper.configClientServerPlatform();

        try {
            m_presenceHandle = OcPlatform.subscribePresence(m_host, EnumSet.of(OcConnectivityType.CT_DEFAULT), onPresenceReceived);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured in subscribePresence");
        }
        assertNotNull("subscribePresence could not create presence handle", m_presenceHandle);

        try {
            OcPlatform.unsubscribePresence(m_presenceHandle);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @objective Test unsubscribePresence function with negative basic way
     * using null presence handler
     * @target void unsubscribePresence(
     * OcPresenceHandle ocPresenceHandle)
     * @test_data ocPresenceHandle null
     * @pre_condition 1. Configure platform
     * 2. Call subscribePresence() API
     * @procedure Call unsubscribePresence() API
     * @post_condition None
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcPresenceHandle subscribePresence(
     * String host,
     * EnumSet<OcConnectivityType> connectivityTypeSet,
     * OnPresenceListener onPresenceListener)
     * @since 2016-08-30
     **/
    public void testUnsubscribePresence_NV_N() {
        m_riHelper.configClientServerPlatform();

        try {
            m_presenceHandle = OcPlatform.subscribePresence(m_host, EnumSet.of(OcConnectivityType.CT_DEFAULT), onPresenceReceived);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured in subscribePresence");
        }
        assertNotNull("subscribePresence could not create presence handle", m_presenceHandle);

        try {
            OcPlatform.unsubscribePresence(null);
            fail("OcException should occur");
        } catch (OcException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
            fail("Unexpected exception occured");
        }
    }
}