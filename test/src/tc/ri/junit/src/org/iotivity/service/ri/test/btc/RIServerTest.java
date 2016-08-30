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

import java.util.EnumSet;

import org.iotivity.base.EntityHandlerResult;
import org.iotivity.base.ModeType;
import org.iotivity.base.OcDeviceInfo;
import org.iotivity.base.OcException;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcPlatformInfo;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResourceHandle;
import org.iotivity.base.OcResourceRequest;
import org.iotivity.base.OcResourceResponse;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.RequestHandlerFlag;
import org.iotivity.base.RequestType;
import org.iotivity.base.ResourceProperty;
import org.iotivity.base.ServiceType;
import org.iotivity.service.ri.test.helper.RIHelper;

import android.test.InstrumentationTestCase;
import android.util.Log;

public class RIServerTest extends InstrumentationTestCase {
    private final String LOG_TAG = this.getClass().getSimpleName();
    private RIHelper m_riHelper;
    private OcResourceHandle m_resourceHandle = null;

    protected void setUp() {
        m_riHelper = RIHelper.getInstance(getInstrumentation()
                .getTargetContext());
        Log.i(LOG_TAG, "SetUp Finished");
    }

    protected void tearDown() {
        Log.i(LOG_TAG, "tearDown Finished");
        if (m_resourceHandle != null) {
            try {
                OcPlatform.unregisterResource(m_resourceHandle);
            } catch (OcException e) {
                e.printStackTrace();
            }
        }
    }

    OcPlatform.EntityHandler entityHandler = new OcPlatform.EntityHandler() {

        @Override
        public EntityHandlerResult handleEntity(final OcResourceRequest request) {
            EntityHandlerResult result = EntityHandlerResult.OK;

            Log.i(LOG_TAG, "In Server entity handler:");

            return EntityHandlerResult.OK;
        }
    };

    /**
     * @since 2016-08-30
     * @see None
     * @objective Test Configure function with positive basic way with InProc ServiceType
     * @target void Configure(PlatformConfig platformConfig)
     * @test_data PlatformConfig platform configuration
     * @pre_condition configure with InProc ServiceType
     * @procedure Call Configure() API
     * @post_condition None
     * @expected No exceptions should occur
     **/
    public void testConfigureInProc_SRC_P() {
        try {
            PlatformConfig cfg = new PlatformConfig(getInstrumentation()
                    .getTargetContext(), ServiceType.IN_PROC,
                    ModeType.CLIENT_SERVER, "0.0.0.0", 0, QualityOfService.HIGH);
            OcPlatform.Configure(cfg);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @since 2016-08-30
     * @see None
     * @objective Test Configure function with positive basic way with OutProc ServiceType
     * @target void Configure(PlatformConfig platformConfig)
     * @test_data PlatformConfig platform configuration
     * @pre_condition configure with OutOfProc ServiceType
     * @procedure Call Configure() API
     * @post_condition None
     * @expected No exceptions should occur
     **/
    public void testConfigureOutOfProc_SRC_P() {
        try {
            PlatformConfig cfg = new PlatformConfig(getInstrumentation()
                    .getTargetContext(), ServiceType.OUT_OF_PROC,
                    ModeType.CLIENT_SERVER, "0.0.0.0", 0, QualityOfService.HIGH);
            OcPlatform.Configure(cfg);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @since 2016-08-30
     * @see None
     * @objective Test Configure function with positive basic way with low Quality of service
     * @target void Configure(PlatformConfig platformConfig)
     * @test_data PlatformConfig platform configuration
     * @pre_condition configure with low Quality of service
     * @procedure Call Configure() API
     * @post_condition None
     * @expected No exceptions should occur
     **/
    public void testConfigureNon_SRC_P() {
        try {
            PlatformConfig cfg = new PlatformConfig(getInstrumentation()
                    .getTargetContext(), ServiceType.IN_PROC,
                    ModeType.CLIENT_SERVER, "0.0.0.0", 0, QualityOfService.LOW);
            OcPlatform.Configure(cfg);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @since 2016-08-30
     * @see void Configure(PlatformConfig platformConfig)
     * @objective Test registerPlatformInfo function with positive basic way
     * @target void registerPlatformInfo(OcPlatformInfo ocPlatformInfo)
     * @test_data OcPlatformInfo platform information
     * @pre_condition Configure platform
     * @procedure Call registerPlatformInfo() API
     * @post_condition None
     * @expected No exceptions should occur
     **/
    public void testRegisterPlatformInfo_FSV_P() {
        try {
            OcPlatform.registerPlatformInfo(m_riHelper.getPlatformInfo());
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @since 2016-08-30
     * @see void Configure(PlatformConfig platformConfig)
     * @objective Test registerDeviceInfo function with positive basic way
     * @target void registerDeviceInfo(OcDeviceInfo ocDeviceInfo)
     * @test_data OcDeviceInfo Device information
     * @pre_condition Configure platform
     * @procedure Call registerDeviceInfo() API
     * @post_condition None
     * @expected No exceptions should occur
     **/
    public void testRegisterDeviceInfo_FSV_P() {
        try {
            OcDeviceInfo deviceInfo = m_riHelper.getDeviceInfo();
            OcPlatform.registerDeviceInfo(deviceInfo);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @since 2016-08-31
     * @see void Configure(PlatformConfig platformConfig)
     * @objective Test registerResource function with positive basic way
     * @target OcResourceHandle registerResource(
     *       String resourceUri,
     *       String resourceTypeName,
     *       String resourceInterface,
     *       EntityHandler entityHandler,
     *       EnumSet<ResourceProperty> resourcePropertySet)
     * @test_data 1. resourceUri "/a/temperature"
     * 				2. resourceTypeName "oic.r.temperature"
     * 				3. resourceInterface DEFAULT_INTERFACE
     * 				4. entityHandler entity handler
     * 				5. resourcePropertySet indicates property of the resource
     * @pre_condition Configure platform
     * @procedure Call registerResource() API
     * @post_condition Call unregisterResource() API
     * @expected No exceptions should occur and resource handle should be returned
     **/
    public void testRegisterResource_SRC_P() {
        m_riHelper.configClientServerPlatform();
        try {
            m_resourceHandle = OcPlatform.registerResource(RIHelper.RESOURCE_URI_TEMPERATURE, RIHelper.RESOURCE_TYPE_TEMPERATURE, OcPlatform.DEFAULT_INTERFACE,
                    entityHandler, m_riHelper.m_resourceProperty);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
        assertNotNull("Can not create resource", m_resourceHandle);
    }

    /**
     * @since 2016-08-31
     * @see void Configure(PlatformConfig platformConfig)
     * @objective Test registerResource function with negative basic way
     * 		using empty string as resource uri
     * @target OcResourceHandle registerResource(
     *       String resourceUri,
     *       String resourceTypeName,
     *       String resourceInterface,
     *       EntityHandler entityHandler,
     *       EnumSet<ResourceProperty> resourcePropertySet)
     * @test_data 1. resourceUri ""
     * 				2. resourceTypeName "oic.r.temperature"
     * 				3. resourceInterface DEFAULT_INTERFACE
     * 				4. entityHandler entity handler
     * 				5. resourcePropertySet indicates property of the resource
     * @pre_condition Configure platform
     * @procedure Call registerResource() API
     * @post_condition None
     * @expected OcException should occur
     **/
    public void testRegisterResourceUri_ESV_ETC_N() {
        m_riHelper.configClientServerPlatform();
        try {
            m_resourceHandle = OcPlatform.registerResource(RIHelper.EMPTY_STRING, RIHelper.RESOURCE_TYPE_TEMPERATURE, OcPlatform.DEFAULT_INTERFACE,
                    entityHandler, m_riHelper.m_resourceProperty);
            fail("Exception should occur");
        } catch (OcException e) {
            e.printStackTrace();
        }
    }

    /**
     * @since 2016-08-31
     * @see void Configure(PlatformConfig platformConfig)
     * @objective Test registerResource function with negative basic way
     * 		using empty string as resource type
     * @target OcResourceHandle registerResource(
     *       String resourceUri,
     *       String resourceTypeName,
     *       String resourceInterface,
     *       EntityHandler entityHandler,
     *       EnumSet<ResourceProperty> resourcePropertySet)
     * @test_data 1. resourceUri "/a/temperature"
     * 				2. resourceTypeName ""
     * 				3. resourceInterface DEFAULT_INTERFACE
     * 				4. entityHandler entity handler
     * 				5. resourcePropertySet indicates property of the resource
     * @pre_condition Configure platform
     * @procedure Call registerResource() API
     * @post_condition None
     * @expected OcException should occur
     **/
    public void testRegisterResourceResType_ESV_ETC_N() {
        m_riHelper.configClientServerPlatform();
        try {
            m_resourceHandle = OcPlatform.registerResource(RIHelper.RESOURCE_URI_TEMPERATURE, RIHelper.EMPTY_STRING, OcPlatform.DEFAULT_INTERFACE,
                    entityHandler, m_riHelper.m_resourceProperty);
            fail("Exception should occur");
        } catch (OcException e) {
            e.printStackTrace();
        }
    }

    /**
     * @since 2016-08-31
     * @see void Configure(PlatformConfig platformConfig)
     * @objective Test registerResource function with positive basic way
     * 		using null entity handler
     * @target OcResourceHandle registerResource(
     *       String resourceUri,
     *       String resourceTypeName,
     *       String resourceInterface,
     *       EntityHandler entityHandler,
     *       EnumSet<ResourceProperty> resourcePropertySet)
     * @test_data 1. resourceUri "/a/temperature"
     * 				2. resourceTypeName ""
     * 				3. resourceInterface DEFAULT_INTERFACE
     * 				4. entityHandler null
     * 				5. resourcePropertySet indicates property of the resource
     * @pre_condition Configure platform
     * @procedure Call registerResource() API
     * @post_condition Call unregisterResource() API
     * @expected Exceptions should not occur and resource handler should be returned
     **/
    public void testRegisterResourceEntityHandler_NV_P() {
        m_riHelper.configClientServerPlatform();
        try {
            m_resourceHandle = OcPlatform.registerResource(RIHelper.RESOURCE_URI_TEMPERATURE, RIHelper.RESOURCE_TYPE_TEMPERATURE, OcPlatform.DEFAULT_INTERFACE,
                    null, m_riHelper.m_resourceProperty);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
        assertNotNull("Can not create resource", m_resourceHandle);
    }

    /**
     * @since 2016-08-31
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResourceHandle registerResource(
     *       String resourceUri,
     *       String resourceTypeName,
     *       String resourceInterface,
     *       EntityHandler entityHandler,
     *       EnumSet<ResourceProperty> resourcePropertySet)
     * @objective Test unregisterResource function with positive basic way
     * @target void unregisterResource(
     *       OcResourceHandle ocResourceHandle)
     * @test_data OcResourceHandle Handle of the resource
     * @pre_condition 1. Configure platform
     * 					2. Call registerResource() API
     * @procedure Call unregisterResource() API
     * @post_condition None
     * @expected No exceptions should occur
     **/
    public void testUnregisterResource_SRC_P() {
        m_resourceHandle = m_riHelper.registerResource();
        assertNotNull("Can not create resource", m_resourceHandle);

        try {
            OcPlatform.unregisterResource(m_resourceHandle);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @since 2016-08-31
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResourceHandle registerResource(
     *       String resourceUri,
     *       String resourceTypeName,
     *       String resourceInterface,
     *       EntityHandler entityHandler,
     *       EnumSet<ResourceProperty> resourcePropertySet)
     * @objective Test unregisterResource function with negative basic way
     * 		using null resource handle
     * @target void unregisterResource(
     *       OcResourceHandle ocResourceHandle)
     * @test_data OcResourceHandle null
     * @pre_condition 1. Configure platform
     * 					2. Call registerResource() API
     * @procedure Call unregisterResource() API
     * @post_condition None
     * @expected OcException should occur
     **/
    public void testUnregisterResourceHandle_NV_N() {
        m_resourceHandle = m_riHelper.registerResource();
        assertNotNull("Can not create resource", m_resourceHandle);

        try {
            OcPlatform.unregisterResource(null);
            fail("Exception should occur");
        } catch (OcException e) {
            e.printStackTrace();
        }
    }

    /**
     * @since 2016-08-31
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResourceHandle registerResource(
     *       String resourceUri,
     *       String resourceTypeName,
     *       String resourceInterface,
     *       EntityHandler entityHandler,
     *       EnumSet<ResourceProperty> resourcePropertySet)
     * @objective Test bindTypeToResource function with positive basic way
     * @target void bindTypeToResource(
     *       OcResourceHandle ocResourceHandle,
     *       String resourceTypeName)
     * @test_data 1. OcResourceHandle Handle of the resource
     * 					2. resourceTypeName "core.light"
     * @pre_condition 1. Configure platform
     * 					2. Call registerResource() API
     * @procedure Call bindTypeToResource() API
     * @post_condition Call unregisterResource() API
     * @expected No exceptions should occur
     **/
    public void testBindTypeToResource_SRC_P() {
        m_resourceHandle = m_riHelper.registerResource();
        assertNotNull("Can not create resource", m_resourceHandle);

        try {
            OcPlatform.bindTypeToResource(m_resourceHandle, RIHelper.RESOURCE_TYPE_LIGHT);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @since 2016-08-31
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResourceHandle registerResource(
     *       String resourceUri,
     *       String resourceTypeName,
     *       String resourceInterface,
     *       EntityHandler entityHandler,
     *       EnumSet<ResourceProperty> resourcePropertySet)
     * @objective Test bindTypeToResource function with negative basic way
     * 		using null resource handle
     * @target void bindTypeToResource(
     *       OcResourceHandle ocResourceHandle,
     *       String resourceTypeName)
     * @test_data 1. OcResourceHandle null
     * 					2. resourceTypeName "core.light"
     * @pre_condition 1. Configure platform
     * 					2. Call registerResource() API
     * @procedure Call bindTypeToResource() API
     * @post_condition Call unregisterResource() API
     * @expected OcException should occur
     **/
    public void testBindTypeToResourceHandle_NV_ETC_N() {
        m_resourceHandle = m_riHelper.registerResource();
        assertNotNull("Can not create resource", m_resourceHandle);

        try {
            OcPlatform.bindTypeToResource(null, RIHelper.RESOURCE_TYPE_LIGHT);
            fail("Exception should occur");
        } catch (OcException e) {
            e.printStackTrace();
        }
    }
}
