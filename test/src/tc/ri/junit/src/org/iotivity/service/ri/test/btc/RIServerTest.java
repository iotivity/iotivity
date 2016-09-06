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
import org.iotivity.base.OcDeviceInfo;
import org.iotivity.base.OcException;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcPlatformInfo;
import org.iotivity.base.OcPlatformStatus;
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

    public static final int DEFAULT_PRESENCE_TIME = 30;
    public static final int MIN_PRESENCE_TIME = 0;
    public static final int MAX_PRESENCE_TIME = (60 * 60 * 24);
    private RIHelper m_riHelper;
    private OcResourceHandle m_resourceHandle;

    protected void setUp() {
        m_riHelper = RIHelper.getInstance(getInstrumentation()
                .getTargetContext());
        m_resourceHandle = null;
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

    OcPlatform.EntityHandler entityHandler = new OcPlatform.EntityHandler() {

        @Override
        public EntityHandlerResult handleEntity(final OcResourceRequest request) {
            EntityHandlerResult result = EntityHandlerResult.OK;

            Log.i(LOG_TAG, "In Server entity handler:");

            return EntityHandlerResult.OK;
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
    public void testConfigureServerInProc_SRC_P() {
        try {
            PlatformConfig cfg = new PlatformConfig(getInstrumentation()
                    .getTargetContext(), ServiceType.IN_PROC, ModeType.SERVER,
                    "0.0.0.0", 0, QualityOfService.HIGH);
            OcPlatform.Configure(cfg);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @objective Test Configure function with positive basic way with OutProc
     * ServiceType
     * @target void Configure(PlatformConfig platformConfig)
     * @test_data PlatformConfig platform configuration
     * @pre_condition configure with OutOfProc ServiceType
     * @procedure Call Configure() API
     * @post_condition None
     * @expected No exceptions should occur
     * @see None
     * @since 2016-08-30
     **/
    public void testConfigureServerOutOfProc_SRC_P() {
        try {
            PlatformConfig cfg = new PlatformConfig(getInstrumentation()
                    .getTargetContext(), ServiceType.OUT_OF_PROC,
                    ModeType.SERVER, "0.0.0.0", 0, QualityOfService.HIGH);
            OcPlatform.Configure(cfg);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @objective Test Configure function with positive basic way with low
     * Quality of service
     * @target void Configure(PlatformConfig platformConfig)
     * @test_data PlatformConfig platform configuration
     * @pre_condition configure with low Quality of service
     * @procedure Call Configure() API
     * @post_condition None
     * @expected No exceptions should occur
     * @see None
     * @since 2016-08-30
     **/
    public void testConfigureServerNon_SRC_P() {
        try {
            PlatformConfig cfg = new PlatformConfig(getInstrumentation()
                    .getTargetContext(), ServiceType.IN_PROC, ModeType.SERVER,
                    "0.0.0.0", 0, QualityOfService.LOW);
            OcPlatform.Configure(cfg);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @objective Test registerPlatformInfo function with positive basic way
     * @target void registerPlatformInfo(OcPlatformInfo ocPlatformInfo)
     * @test_data OcPlatformInfo platform information
     * @pre_condition Configure platform
     * @procedure Call registerPlatformInfo() API
     * @post_condition None
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @since 2016-08-30
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
     * @objective Test registerDeviceInfo function with positive basic way
     * @target void registerDeviceInfo(OcDeviceInfo ocDeviceInfo)
     * @test_data OcDeviceInfo Device information
     * @pre_condition Configure platform
     * @procedure Call registerDeviceInfo() API
     * @post_condition None
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @since 2016-08-30
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
     * @objective Test registerResource function with positive basic way
     * @target OcResourceHandle registerResource( String resourceUri, String
     * resourceTypeName, String resourceInterface, EntityHandler
     * entityHandler, EnumSet<ResourceProperty> resourcePropertySet)
     * @test_data 1. resourceUri "/a/temperature"
     * 2. resourceTypeName "oic.r.temperature"
     * 3. resourceInterface DEFAULT_INTERFACE
     * 4. entityHandler entity handler
     * 5. resourcePropertySet indicates property of the resource
     * @pre_condition Configure platform
     * @procedure Call registerResource() API
     * @post_condition Call unregisterResource() API
     * @expected No exceptions should occur and resource handle should be
     * returned
     * @see void Configure(PlatformConfig platformConfig)
     * @see void unregisterResource( OcResourceHandle ocResourceHandle)
     * @since 2016-08-31
     **/
    public void testRegisterResource_SRC_P() {
        m_riHelper.configClientServerPlatform();
        try {
            m_resourceHandle = OcPlatform.registerResource(
                    RIHelper.RESOURCE_URI_TEMPERATURE,
                    RIHelper.RESOURCE_TYPE_TEMPERATURE,
                    OcPlatform.DEFAULT_INTERFACE, entityHandler,
                    m_riHelper.m_resourceProperty);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
        assertNotNull("Can not create resource", m_resourceHandle);
    }

    /**
     * @objective Test registerResource function with negative basic way using
     * empty string as resource uri
     * @target OcResourceHandle registerResource( String resourceUri, String
     * resourceTypeName, String resourceInterface, EntityHandler
     * entityHandler, EnumSet<ResourceProperty> resourcePropertySet)
     * @test_data 1. resourceUri ""
     * 2. resourceTypeName "oic.r.temperature"
     * 3. resourceInterface DEFAULT_INTERFACE
     * 4. entityHandler entity handler
     * 5. resourcePropertySet indicates property of the
     * resource
     * @pre_condition Configure platform
     * @procedure Call registerResource() API
     * @post_condition None
     * @expected OcException should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @since 2016-08-31
     **/
    public void testRegisterResourceUri_ESV_ETC_N() {
        m_riHelper.configClientServerPlatform();
        try {
            m_resourceHandle = OcPlatform.registerResource(
                    RIHelper.EMPTY_STRING, RIHelper.RESOURCE_TYPE_TEMPERATURE,
                    OcPlatform.DEFAULT_INTERFACE, entityHandler,
                    m_riHelper.m_resourceProperty);
            fail("OcException should occur");
        } catch (OcException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
            fail("Unexpected exception occured");
        }
    }

    /**
     * @objective Test registerResource function with negative basic way using
     * empty string as resource type
     * @target OcResourceHandle registerResource( String resourceUri, String
     * resourceTypeName, String resourceInterface, EntityHandler
     * entityHandler, EnumSet<ResourceProperty> resourcePropertySet)
     * @test_data 1. resourceUri "/a/temperature"
     * 2. resourceTypeName ""
     * 3. resourceInterface DEFAULT_INTERFACE
     * 4. entityHandler entity handler
     * 5. resourcePropertySet indicates property of the resource
     * @pre_condition Configure platform
     * @procedure Call registerResource() API
     * @post_condition None
     * @expected OcException should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @since 2016-08-31
     **/
    public void testRegisterResourceResType_ESV_ETC_N() {
        m_riHelper.configClientServerPlatform();
        try {
            m_resourceHandle = OcPlatform.registerResource(
                    RIHelper.RESOURCE_URI_TEMPERATURE, RIHelper.EMPTY_STRING,
                    OcPlatform.DEFAULT_INTERFACE, entityHandler,
                    m_riHelper.m_resourceProperty);
            fail("OcException should occur");
        } catch (OcException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
            fail("Unexpected exception occured");
        }
    }

    /**
     * @objective Test registerResource function with positive basic way using
     * null entity handler
     * @target OcResourceHandle registerResource( String resourceUri, String
     * resourceTypeName, String resourceInterface, EntityHandler
     * entityHandler, EnumSet<ResourceProperty> resourcePropertySet)
     * @test_data 1. resourceUri "/a/temperature"
     * 2. resourceTypeName ""
     * 3. resourceInterface DEFAULT_INTERFACE
     * 4. entityHandler null
     * 5.resourcePropertySet indicates property of the resource
     * @pre_condition Configure platform
     * @procedure Call registerResource() API
     * @post_condition Call unregisterResource() API
     * @expected Exceptions should not occur and resource handler should be
     * returned
     * @see void Configure(PlatformConfig platformConfig)
     * @see void unregisterResource( OcResourceHandle ocResourceHandle)
     * @since 2016-08-31
     **/
    public void testRegisterResourceEntityHandler_NV_P() {
        m_riHelper.configClientServerPlatform();
        try {
            m_resourceHandle = OcPlatform.registerResource(
                    RIHelper.RESOURCE_URI_TEMPERATURE,
                    RIHelper.RESOURCE_TYPE_TEMPERATURE,
                    OcPlatform.DEFAULT_INTERFACE, null,
                    m_riHelper.m_resourceProperty);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
        assertNotNull("Can not create resource", m_resourceHandle);
    }

    /**
     * @objective Test unregisterResource function with positive basic way
     * @target void unregisterResource( OcResourceHandle ocResourceHandle)
     * @test_data OcResourceHandle Handle of the resource
     * @pre_condition 1. Configure platform
     * 2. Call registerResource() API
     * @procedure Call unregisterResource() API
     * @post_condition None
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResourceHandle registerResource( String resourceUri, String
     * resourceTypeName, String resourceInterface, EntityHandler
     * entityHandler, EnumSet<ResourceProperty> resourcePropertySet)
     * @see void unregisterResource( OcResourceHandle ocResourceHandle)
     * @since 2016-08-31
     **/
    public void testUnregisterResource_SRC_P() {
        m_resourceHandle = m_riHelper.registerResource(
                RIHelper.RESOURCE_URI_TEMPERATURE,
                RIHelper.RESOURCE_TYPE_TEMPERATURE);
        assertNotNull("Can not create resource", m_resourceHandle);

        try {
            OcPlatform.unregisterResource(m_resourceHandle);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @objective Test unregisterResource function with negative basic way using
     * null resource handle
     * @target void unregisterResource(OcResourceHandle ocResourceHandle)
     * @test_data OcResourceHandle null
     * @pre_condition 1. Configure platform
     * 2. Call registerResource() API
     * @procedure Call unregisterResource() API
     * @post_condition None
     * @expected OcException should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResourceHandle registerResource( String resourceUri, String
     * resourceTypeName, String resourceInterface, EntityHandler
     * entityHandler, EnumSet<ResourceProperty> resourcePropertySet)
     * @see void unregisterResource( OcResourceHandle ocResourceHandle)
     * @since 2016-08-31
     **/
    public void testUnregisterResourceHandle_NV_N() {
        m_resourceHandle = m_riHelper.registerResource(
                RIHelper.RESOURCE_URI_TEMPERATURE,
                RIHelper.RESOURCE_TYPE_TEMPERATURE);
        assertNotNull("Can not create resource", m_resourceHandle);

        try {
            OcPlatform.unregisterResource(null);
            fail("OcException should occur");
        } catch (OcException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
            fail("Unexpected exception occured");
        }
    }

    /**
     * @objective Test bindTypeToResource function with positive basic way
     * @target void bindTypeToResource( OcResourceHandle ocResourceHandle,
     * String resourceTypeName)
     * @test_data 1. OcResourceHandle Handle of the resource
     * 2. resourceTypeName "core.light"
     * @pre_condition 1. Configure platform
     * 2. Call registerResource() API
     * @procedure Call bindTypeToResource() API
     * @post_condition Call unregisterResource() API
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResourceHandle registerResource( String resourceUri, String
     * resourceTypeName, String resourceInterface, EntityHandler
     * entityHandler, EnumSet<ResourceProperty> resourcePropertySet)
     * @see void unregisterResource( OcResourceHandle ocResourceHandle)
     * @since 2016-08-31
     **/
    public void testBindTypeToResource_SRC_P() {
        m_resourceHandle = m_riHelper.registerResource(
                RIHelper.RESOURCE_URI_TEMPERATURE,
                RIHelper.RESOURCE_TYPE_TEMPERATURE);
        assertNotNull("Can not create resource", m_resourceHandle);

        try {
            OcPlatform.bindTypeToResource(m_resourceHandle,
                    RIHelper.RESOURCE_TYPE_LIGHT);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @objective Test bindTypeToResource function with negative basic way using
     * null resource handle
     * @target void bindTypeToResource( OcResourceHandle ocResourceHandle,
     * String resourceTypeName)
     * @test_data 1. OcResourceHandle null
     * 2. resourceTypeName "core.light"
     * @pre_condition 1. Configure platform
     * 2. Call registerResource() API
     * @procedure Call bindTypeToResource() API
     * @post_condition Call unregisterResource() API
     * @expected OcException should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResourceHandle registerResource( String resourceUri, String
     * resourceTypeName, String resourceInterface, EntityHandler
     * entityHandler, EnumSet<ResourceProperty> resourcePropertySet)
     * @see void unregisterResource( OcResourceHandle ocResourceHandle)
     * @since 2016-08-31
     **/
    public void testBindTypeToResourceHandle_NV_ETC_N() {
        m_resourceHandle = m_riHelper.registerResource(
                RIHelper.RESOURCE_URI_TEMPERATURE,
                RIHelper.RESOURCE_TYPE_TEMPERATURE);
        assertNotNull("Can not create resource", m_resourceHandle);

        try {
            OcPlatform.bindTypeToResource(null, RIHelper.RESOURCE_TYPE_LIGHT);
            fail("OcException should occur");
        } catch (OcException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
            fail("Unexpected exception occured");
        }
    }

    /**
     * @objective Test bindInterfaceToResource function with positive basic way
     * @target void bindInterfaceToResource( OcResourceHandle ocResourceHandle,
     * String resourceInterfaceName)
     * @test_data 1. OcResourceHandle Handle of the resource
     * 2. resourceInterfaceName "core.light"
     * @pre_condition 1. Configure platform
     * 2. Call registerResource() API
     * @procedure Call bindInterfaceToResource() API
     * @post_condition Call unregisterResource() API
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResourceHandle registerResource( String resourceUri, String
     * resourceTypeName, String resourceInterface, EntityHandler
     * entityHandler, EnumSet<ResourceProperty> resourcePropertySet)
     * @see void unregisterResource( OcResourceHandle ocResourceHandle)
     * @since 2016-08-31
     **/
    public void testBindInterfaceToResource_SRC_P() {
        m_resourceHandle = m_riHelper.registerResource(
                RIHelper.RESOURCE_URI_TEMPERATURE,
                RIHelper.RESOURCE_TYPE_TEMPERATURE);
        assertNotNull("Can not create resource", m_resourceHandle);

        try {
            OcPlatform.bindInterfaceToResource(m_resourceHandle,
                    OcPlatform.BATCH_INTERFACE);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @objective Test bindInterfaceToResource function with negative basic way
     * using null resource handle
     * @target void bindInterfaceToResource( OcResourceHandle ocResourceHandle,
     * String resourceInterfaceName)
     * @test_data 1. OcResourceHandle null
     * 2. resourceTypeName "core.light"
     * @pre_condition 1. Configure platform
     * 2. Call registerResource() API
     * @procedure Call bindInterfaceToResource() API
     * @post_condition Call unregisterResource() API
     * @expected OcException should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResourceHandle registerResource( String resourceUri, String
     * resourceTypeName, String resourceInterface, EntityHandler
     * entityHandler, EnumSet<ResourceProperty> resourcePropertySet)
     * @see void unregisterResource( OcResourceHandle ocResourceHandle)
     * @since 2016-08-31
     **/
    public void testBindIntefaceToResourceHandle_NV_ETC_N() {
        m_resourceHandle = m_riHelper.registerResource(
                RIHelper.RESOURCE_URI_TEMPERATURE,
                RIHelper.RESOURCE_TYPE_TEMPERATURE);
        assertNotNull("Can not create resource", m_resourceHandle);

        try {
            OcPlatform
                    .bindInterfaceToResource(null, OcPlatform.BATCH_INTERFACE);
            fail("OcException should occur");
        } catch (OcException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
            fail("Unexpected exception occured");
        }

    }

    /**
     * @objective Test bindResource function with positive basic way
     * @target bindResource(OcResourceHandle ocResourceCollectionHandle,
     *OcResourceHandle ocResourceHandle)
     * @test_data 1. ocResourceCollectionHandle Handle of the collection resource
     * 2. ocResourceHandle Handle of the resource to be bound
     * @pre_condition 1. Configure platform
     * 2. Call registerResource() API for two resources
     * @procedure Call bindResource() API
     * @post_condition Call unregisterResource() API for two resources
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResourceHandle registerResource( String resourceUri, String
     * resourceTypeName, String resourceInterface, EntityHandler
     * entityHandler, EnumSet<ResourceProperty> resourcePropertySet)
     * @see void unregisterResource( OcResourceHandle ocResourceHandle)
     * @since 2016-08-31
     **/
    public void testBindResource_SRC_P() {
        OcResourceHandle lightHandle = m_riHelper.registerResource(
                RIHelper.RESOURCE_URI_LIGHT, RIHelper.RESOURCE_TYPE_LIGHT);
        assertNotNull("Can not create resource", lightHandle);
        OcResourceHandle fanHandle = m_riHelper.registerResource(
                RIHelper.RESOURCE_URI_FAN, RIHelper.RESOURCE_TYPE_FAN);
        assertNotNull("Can not create resource", fanHandle);

        try {
            OcPlatform.bindResource(fanHandle, lightHandle);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
        try {
            OcPlatform.unregisterResource(lightHandle);
            OcPlatform.unregisterResource(fanHandle);
        } catch (OcException e) {
            e.printStackTrace();
        }
    }

    /**
     * @objective Test bindResource function with negative basic way using null
     * resource handle
     * @target bindResource(OcResourceHandle ocResourceCollectionHandle,
     *OcResourceHandle ocResourceHandle)
     * @test_data 1. ocResourceCollectionHandle null
     * 2. ocResourceHandle null
     * @pre_condition Configure platform *
     * @procedure Call bindResource() API
     * @post_condition Call unregisterResource() API for two resources
     * @expected OcException should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResourceHandle registerResource( String resourceUri, String
     * resourceTypeName, String resourceInterface, EntityHandler
     * entityHandler, EnumSet<ResourceProperty> resourcePropertySet)
     * @see void unregisterResource( OcResourceHandle ocResourceHandle)
     * @since 2016-08-31
     **/
    public void testBindResourceHandle_NV_N() {
        m_riHelper.configClientServerPlatform();
        try {
            OcPlatform.bindResource(null, null);
            fail("OcException should occur");
        } catch (OcException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
            fail("Unexpected exception occured");
        }
    }

    /**
     * @objective Test unbindResource function with positive basic way
     * @target unbindResource(OcResourceHandle ocResourceCollectionHandle,
     *OcResourceHandle ocResourceHandle)
     * @test_data 1. ocResourceCollectionHandle Handle of the collection resource
     * 2. ocResourceHandle Handle of the bound resource
     * @pre_condition 1. Configure platform
     * 2. Call registerResource() API for two resources
     * 3. Call bindResource() API
     * @procedure Call unbindResource() API
     * @post_condition Call unregisterResource() API for two resources
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResourceHandle registerResource( String resourceUri, String
     * resourceTypeName, String resourceInterface, EntityHandler
     * entityHandler, EnumSet<ResourceProperty> resourcePropertySet)
     * @see void unregisterResource( OcResourceHandle ocResourceHandle)
     * @see bindResource( OcResourceHandle ocResourceCollectionHandle,
     * OcResourceHandle ocResourceHandle)
     * @since 2016-08-31
     **/
    public void testUnbindResource_SRC_P() {
        OcResourceHandle lightHandle = m_riHelper.registerResource(
                RIHelper.RESOURCE_URI_LIGHT, RIHelper.RESOURCE_TYPE_LIGHT);
        assertNotNull("Can not create resource", lightHandle);
        OcResourceHandle fanHandle = m_riHelper.registerResource(
                RIHelper.RESOURCE_URI_FAN, RIHelper.RESOURCE_TYPE_FAN);
        assertNotNull("Can not create resource", fanHandle);

        try {
            OcPlatform.bindResource(fanHandle, lightHandle);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured in bindResource");
        }

        try {
            OcPlatform.unbindResource(fanHandle, lightHandle);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }

        try {
            OcPlatform.unregisterResource(lightHandle);
            OcPlatform.unregisterResource(fanHandle);
        } catch (OcException e) {
            e.printStackTrace();
        }
    }

    /**
     * @objective Test unbindResource function with negative basic way using
     * null resource handle
     * @target unbindResource(OcResourceHandle ocResourceCollectionHandle,
     *OcResourceHandle ocResourceHandle)
     * @test_data 1. ocResourceCollectionHandle null
     * 2. ocResourceHandle null
     * @pre_condition 1. Configure platform
     * 2. Call registerResource() API for	two resources
     * 3. Call bindResource() API
     * @procedure Call unbindResource() API
     * @post_condition Call unregisterResource() API for two resources
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResourceHandle registerResource( String resourceUri, String
     * resourceTypeName, String resourceInterface, EntityHandler
     * entityHandler, EnumSet<ResourceProperty> resourcePropertySet)
     * @see void unregisterResource( OcResourceHandle ocResourceHandle)
     * @see bindResource( OcResourceHandle ocResourceCollectionHandle,
     * OcResourceHandle ocResourceHandle)
     * @since 2016-08-31
     **/
    public void testUnbindResourceHandle_NV_N() {
        OcResourceHandle lightHandle = m_riHelper.registerResource(
                RIHelper.RESOURCE_URI_LIGHT, RIHelper.RESOURCE_TYPE_LIGHT);
        assertNotNull("Can not create resource", lightHandle);
        OcResourceHandle fanHandle = m_riHelper.registerResource(
                RIHelper.RESOURCE_URI_FAN, RIHelper.RESOURCE_TYPE_FAN);
        assertNotNull("Can not create resource", fanHandle);

        try {
            OcPlatform.bindResource(fanHandle, lightHandle);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured in bindResource");
        }

        try {
            OcPlatform.unbindResource(null, null);
            fail("OcException should occur");
        } catch (OcException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
            fail("Unexpected exception occured");
        }

        try {
            OcPlatform.unregisterResource(lightHandle);
            OcPlatform.unregisterResource(fanHandle);
        } catch (OcException e) {
            e.printStackTrace();
        }
    }

    /**
     * @objective Test bindResources function with positive basic way
     * @target void bindResources( OcResourceHandle ocResourceCollectionHandle,
     * List<OcResourceHandle> ocResourceHandleList)
     * @test_data 1. ocResourceCollectionHandle Handle of the collection resource
     * 2. ocResourceHandleList list of handles to be bound
     * @pre_condition 1. Configure platform
     * 2. Call registerResource() API for three resources
     * @procedure Call bindResources() API
     * @post_condition Call unregisterResource() API for three resources
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResourceHandle registerResource( String resourceUri, String
     * resourceTypeName, String resourceInterface, EntityHandler
     * entityHandler, EnumSet<ResourceProperty> resourcePropertySet)
     * @see void unregisterResource( OcResourceHandle ocResourceHandle)
     * @since 2016-08-31
     **/
    public void testBindResources_SRC_P() {
        m_resourceHandle = m_riHelper.registerResource(
                RIHelper.RESOURCE_URI_TEMPERATURE,
                RIHelper.RESOURCE_TYPE_TEMPERATURE);

        OcResourceHandle lightHandle = m_riHelper.registerResource(
                RIHelper.RESOURCE_URI_LIGHT, RIHelper.RESOURCE_TYPE_LIGHT);
        assertNotNull("Can not create resource", lightHandle);
        OcResourceHandle fanHandle = m_riHelper.registerResource(
                RIHelper.RESOURCE_URI_FAN, RIHelper.RESOURCE_TYPE_FAN);
        assertNotNull("Can not create resource", fanHandle);

        List<OcResourceHandle> resourceHandleList = new ArrayList<OcResourceHandle>();
        resourceHandleList.add(lightHandle);
        resourceHandleList.add(fanHandle);

        try {
            OcPlatform.bindResources(m_resourceHandle, resourceHandleList);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
        try {
            OcPlatform.unregisterResource(lightHandle);
            OcPlatform.unregisterResource(fanHandle);
        } catch (OcException e) {
            e.printStackTrace();
        }
    }

    /**
     * @objective Test bindResources function with negative basic way using null
     * parameter
     * @target void bindResources( OcResourceHandle ocResourceCollectionHandle,
     * List<OcResourceHandle> ocResourceHandleList)
     * @test_data 1. ocResourceCollectionHandle null
     * 2. ocResourceHandleList null
     * @pre_condition Configure platform
     * @procedure Call bindResources() API
     * @post_condition None
     * @expected Exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResourceHandle registerResource( String resourceUri, String
     * resourceTypeName, String resourceInterface, EntityHandler
     * entityHandler, EnumSet<ResourceProperty> resourcePropertySet)
     * @see void unregisterResource( OcResourceHandle ocResourceHandle)
     * @since 2016-08-31
     **/
    public void testBindResources_NV_N() {
        m_riHelper.configClientServerPlatform();
        try {
            OcPlatform.bindResources(null, null);
            fail("OcException should occur");
        } catch (OcException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
            fail("Unexpected exception occured");
        }
    }

    /**
     * @objective Test unbindResources function with positive basic way
     * @target void unbindResources( OcResourceHandle
     * ocResourceCollectionHandle, List<OcResourceHandle>
     * ocResourceHandleList)
     * @test_data 1. ocResourceCollectionHandle Handle of the collection resource
     * 2. ocResourceHandle Handle of the bound resource
     * @pre_condition 1. Configure platform
     * 2. Call registerResource() API for three resources
     * 3. Call bindResources() API
     * @procedure Call unbindResources() API
     * @post_condition Call unregisterResource() API for three resources
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResourceHandle registerResource( String resourceUri, String
     * resourceTypeName, String resourceInterface, EntityHandler
     * entityHandler, EnumSet<ResourceProperty> resourcePropertySet)
     * @see void unregisterResource( OcResourceHandle ocResourceHandle)
     * @see void bindResources( OcResourceHandle ocResourceCollectionHandle,
     * List<OcResourceHandle> ocResourceHandleList)
     * @since 2016-08-31
     **/
    public void testUnbindResources_SRC_P() {
        m_resourceHandle = m_riHelper.registerResource(
                RIHelper.RESOURCE_URI_TEMPERATURE,
                RIHelper.RESOURCE_TYPE_TEMPERATURE);

        OcResourceHandle lightHandle = m_riHelper.registerResource(
                RIHelper.RESOURCE_URI_LIGHT, RIHelper.RESOURCE_TYPE_LIGHT);
        assertNotNull("Can not create resource", lightHandle);
        OcResourceHandle fanHandle = m_riHelper.registerResource(
                RIHelper.RESOURCE_URI_FAN, RIHelper.RESOURCE_TYPE_FAN);
        assertNotNull("Can not create resource", fanHandle);

        List<OcResourceHandle> resourceHandleList = new ArrayList<OcResourceHandle>();
        resourceHandleList.add(lightHandle);
        resourceHandleList.add(fanHandle);

        try {
            OcPlatform.bindResources(m_resourceHandle, resourceHandleList);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured in bindResources");
        }

        try {
            OcPlatform.unbindResources(m_resourceHandle, resourceHandleList);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }

        try {
            OcPlatform.unregisterResource(lightHandle);
            OcPlatform.unregisterResource(fanHandle);
        } catch (OcException e) {
            e.printStackTrace();
        }
    }

    /**
     * @objective Test unbindResources function with negative basic way using
     * null parameters
     * @target void unbindResources( OcResourceHandle
     * ocResourceCollectionHandle, List<OcResourceHandle>
     * ocResourceHandleList)
     * @test_data 1. ocResourceCollectionHandle null
     * 2. ocResourceHandle null
     * @pre_condition Configure platform
     * @procedure Call unbindResources() API
     * @post_condition None
     * @expected OcException should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResourceHandle registerResource( String resourceUri, String
     * resourceTypeName, String resourceInterface, EntityHandler
     * entityHandler, EnumSet<ResourceProperty> resourcePropertySet)
     * @see void unregisterResource( OcResourceHandle ocResourceHandle)
     * @see void bindResources( OcResourceHandle ocResourceCollectionHandle,
     * List<OcResourceHandle> ocResourceHandleList)
     * @since 2016-08-31
     **/
    public void testUnbindResources_NV_N() {
        m_riHelper.configClientServerPlatform();
        try {
            OcPlatform.unbindResources(null, null);
            fail("OcException should occur");
        } catch (OcException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
            fail("Unexpected exception occured");
        }
    }

    /**
     * @objective Test startPresence() with positive way
     * @target void startPresence(int ttl)
     * @test_data ttl = 30
     * @pre_condition Configure platform
     * @procedure Call startPresence() API
     * @post_condition None
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @since 2016-08-31
     */
    public void testStartPresence_SRC_P() {
        m_riHelper.configClientServerPlatform();
        try {
            OcPlatform.startPresence(DEFAULT_PRESENCE_TIME);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @objective Test startPresence() with lower boundary value
     * @target void startPresence(int ttl)
     * @test_data ttl = 0
     * @pre_condition Configure platform
     * @procedure Call startPresence() API
     * @post_condition None
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @since 2016-06-29
     */
    public void testStartPresence_LBV_P() {
        m_riHelper.configClientServerPlatform();
        try {
            OcPlatform.startPresence(MIN_PRESENCE_TIME);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @objective Test startPresence() with upper boundary value
     * @target void startPresence(int ttl)
     * @test_data ttl = 60 * 60 * 24
     * @pre_condition Configure platform
     * @procedure Call startPresence() API
     * @post_condition None
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @since 2016-06-29
     */
    public void testStartPresence_UBV_P() {
        m_riHelper.configClientServerPlatform();
        try {
            OcPlatform.startPresence(MAX_PRESENCE_TIME);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

    /**
     * @objective Test stopPresence() with positive way
     * @target void stopPresence()
     * @test_data None
     * @pre_condition 1. Configure platform 2. Perform startPresence() API
     * @procedure Call stopPresence() API
     * @post_condition None
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see void startPresence(int ttl)
     * @since 2016-06-29
     */
    public void testStopPresence_SRC_P() {
        m_riHelper.configClientServerPlatform();

        try {
            OcPlatform.startPresence(DEFAULT_PRESENCE_TIME);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured in startPresence");
        }

        try {
            OcPlatform.stopPresence();
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured ");
        }
    }

    /**
     * @objective Test notifyAllObservers function with negative basic way using
     * null resource handle
     * @target void notifyAllObservers( OcResourceHandle ocResourceHandle)
     * @test_data ocResourceHandle null
     * @pre_condition 1. Configure platform
     * 2. Call registerResource() API
     * @procedure Call notifyAllObservers() API
     * @post_condition Call unregisterResource() API
     * @expected OcException should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResourceHandle registerResource( String resourceUri, String
     * resourceTypeName, String resourceInterface, EntityHandler
     * entityHandler, EnumSet<ResourceProperty> resourcePropertySet)
     * @see void unregisterResource( OcResourceHandle ocResourceHandle)
     * @since 2016-08-31
     **/
    public void testNotifyAllObservers_NV_N() {
        m_resourceHandle = m_riHelper.registerResource(
                RIHelper.RESOURCE_URI_TEMPERATURE,
                RIHelper.RESOURCE_TYPE_TEMPERATURE);
        assertNotNull("Can not create resource", m_resourceHandle);
        try {
            OcPlatform.notifyAllObservers(null);
            fail("OcException should occur");
        } catch (OcException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
            fail("Unexpected exception occured");
        }
    }

    /**
     * @objective Test notifyAllObservers function(With Qos) with negative basic
     * way using null resource handle
     * @target void notifyAllObservers( OcResourceHandle ocResourceHandle,
     * QualityOfService qualityOfService)
     * @test_data 1. ocResourceHandle null
     * 2. qualityOfService High
     * @pre_condition 1. Configure platform
     * 2. Call registerResource() API
     * @procedure Call notifyAllObservers() API with Qos High
     * @post_condition Call unregisterResource() API
     * @expected OcException should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResourceHandle registerResource( String resourceUri, String
     * resourceTypeName, String resourceInterface, EntityHandler
     * entityHandler, EnumSet<ResourceProperty> resourcePropertySet)
     * @see void unregisterResource( OcResourceHandle ocResourceHandle)
     * @since 2016-08-31
     **/
    public void testNotifyAllObserversWithQos_NV_N() {
        m_resourceHandle = m_riHelper.registerResource(
                RIHelper.RESOURCE_URI_TEMPERATURE,
                RIHelper.RESOURCE_TYPE_TEMPERATURE);
        assertNotNull("Can not create resource", m_resourceHandle);
        try {
            OcPlatform.notifyAllObservers(null, QualityOfService.HIGH);
            fail("OcException should occur");
        } catch (OcException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
            fail("Unexpected exception occured");
        }
    }

    /**
     * @objective Test notifyAllObservers function with negative basic way using
     * null parameters
     * @target void notifyListOfObservers( OcResourceHandle ocResourceHandle,
     * List<Byte> ocObservationIdList, OcResourceResponse
     * ocResourceResponse)
     * @test_data 1. ocResourceHandle null
     * 2. ocObservationIdList null
     * 3. ocResourceResponse null
     * @pre_condition 1. Configure platform 2. Call registerResource() API
     * @procedure Call notifyAllObservers() API
     * @post_condition Call unregisterResource() API
     * @expected OcException should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResourceHandle registerResource( String resourceUri, String
     * resourceTypeName, String resourceInterface, EntityHandler
     * entityHandler, EnumSet<ResourceProperty> resourcePropertySet)
     * @see void unregisterResource( OcResourceHandle ocResourceHandle)
     * @since 2016-08-31
     **/
    public void testNotifyListOfObservers_NV_N() {
        m_resourceHandle = m_riHelper.registerResource(
                RIHelper.RESOURCE_URI_TEMPERATURE,
                RIHelper.RESOURCE_TYPE_TEMPERATURE);
        assertNotNull("Can not create resource", m_resourceHandle);

        try {
            OcPlatform.notifyListOfObservers(m_resourceHandle, null, null);
            fail("OcException should occur");
        } catch (OcException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
            fail("Unexpected exception occured");
        }
    }

    /**
     * @objective Test notifyAllObservers function(With Qos) with negative basic
     * way using null parameters
     * @target void notifyListOfObservers( OcResourceHandle ocResourceHandle,
     * List<Byte> ocObservationIdList, OcResourceResponse
     * ocResourceResponse, QualityOfService qualityOfService)
     * @test_data 1. ocResourceHandle Handle of the resource
     * 2. ocObservationIdList null
     * 3. ocResourceResponse null
     * 4. qualityOfService High
     * @pre_condition 1. Configure platform
     * 2. Call registerResource() API
     * @procedure Call notifyAllObservers() API with Qos High
     * @post_condition Call unregisterResource() API
     * @expected OcException should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @see OcResourceHandle registerResource( String resourceUri, String
     * resourceTypeName, String resourceInterface, EntityHandler
     * entityHandler, EnumSet<ResourceProperty> resourcePropertySet)
     * @see void unregisterResource( OcResourceHandle ocResourceHandle)
     * @since 2016-08-31
     **/
    public void testNotifyListOfObserversWithQos_NV_N() {
        m_resourceHandle = m_riHelper.registerResource(
                RIHelper.RESOURCE_URI_TEMPERATURE,
                RIHelper.RESOURCE_TYPE_TEMPERATURE);
        assertNotNull("Can not create resource", m_resourceHandle);

        try {
            OcPlatform.notifyListOfObservers(m_resourceHandle, null, null,
                    QualityOfService.HIGH);
            fail("OcException should occur");
        } catch (OcException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
            fail("Unexpected exception occured");
        }
    }

    /**
     * @objective Test startPresence() with positive way
     * @target void sendResponse(OcResourceResponse ocResourceResponse)
     * @test_data OcResourceResponse null
     * @pre_condition Configure platform
     * @procedure Call sendResponse() API
     * @post_condition None
     * @expected No exceptions should occur
     * @see void Configure(PlatformConfig platformConfig)
     * @since 2016-08-31
     */
    public void testSendResponse_NV_N() {
        m_riHelper.configClientServerPlatform();
        try {
            OcPlatform.sendResponse(null);
            fail("OcException should occur");
        } catch (OcException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
            fail("Unexpected Exception occured");
        }
    }
}