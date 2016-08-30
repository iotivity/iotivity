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

package org.iotivity.service.ri.test.helper;

import android.content.Context;

import java.util.EnumSet;
import java.util.List;
import java.util.ArrayList;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

import org.iotivity.base.EntityHandlerResult;
import org.iotivity.base.OcException;
import org.iotivity.base.ModeType;
import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcPlatformInfo;
import org.iotivity.base.OcDeviceInfo;
import org.iotivity.base.OcResourceRequest;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ResourceProperty;
import org.iotivity.base.ServiceType;
import org.iotivity.base.OcRepresentation;
import org.iotivity.base.OcResource;
import org.iotivity.base.OcResource.OnObserveListener;
import org.iotivity.base.OcResourceHandle;

import android.util.Log;

public class RIHelper {

    private static RIHelper riHelperInstance = null;

    public static final String EMPTY_STRING = "";

    public static final String PLATFORM_ID = "436f6e66-6f72-6d61-6e63-6553696d756c";
    public static final String MANUFACTURER_NAME = "Vendor";
    public static final String MANUFACTURER_URL = "www.default-vendor.com";
    public static final String MODEL_NO = "ABCDE00004";
    public static final String MANUFACTURE_DATE = "2016-06-01";
    public static final String PLATFORM_VERSION = "0.0.1";
    public static final String OS_VERSION = "10";
    public static final String HW_VERSION = "1.1.0";
    public static final String FIRMWARE_VERSION = "1.1.1";
    public static final String SUPPORT_URL = "support.default-vendor.com";
    public static final String SYSTEM_TIME = "2016-06-20T10:10:10Z";

    public final String DEVICE_NAME = "IotivitySmartRoom";
    public final String DEVICE_TYPE_AC = "AirCondition";

    public static final String RESOURCE_URI_TEMPERATURE = "/a/temperature";
    public static final String RESOURCE_TYPE_TEMPERATURE = "oic.r.temperature";
    public static final String RESOURCE_TYPE_LIGHT = "core.light";

    public final int INT_ZERO = 0;
    public final int INT_ONE = 1;
    public final int INT_TWO = 2;
    public final int INT_MINUS_ONE = -1;

    public final int CALLBACK_WAIT_DEFAULT = 5;
    public final int CALLBACK_WAIT_MAX = 10;
    public final int CALLBACK_WAIT_MIN = 1;
    public final int SUCCESS_RESPONSE = 0;
    private final String LOG_TAG = this.getClass().getSimpleName();

    public String errorMsg = "";

    private OcRepresentation representation;

    private String logMessage = "";

    private String resourceString = "";

    private static Context s_helperContext;

    private OcResourceHandle m_resourceHandle = null;
    public EnumSet<ResourceProperty> m_resourceProperty;

    private RIHelper() {
        m_resourceProperty = EnumSet.of(ResourceProperty.ACTIVE);
        m_resourceProperty.add(ResourceProperty.DISCOVERABLE);
        m_resourceProperty.add(ResourceProperty.OBSERVABLE);
    }

    /**
     * @brief Function is for getting singleton instance of RIHelper
     * @return singleton instance of RIHelper
     */
    public static RIHelper getInstance(Context context) {
        Lock mutex = new ReentrantLock();

        if (riHelperInstance == null) {
            mutex.lock();
            if (riHelperInstance == null) {
                Log.i("RIHelper", "Inside Helper");
                riHelperInstance = new RIHelper();
            }
            mutex.unlock();
        }

        s_helperContext = context;

        return riHelperInstance;
    }

    /**
     * @brief Function is for configuring platform
     */
    public void configClientServerPlatform() {
        PlatformConfig cfg = new PlatformConfig(s_helperContext, ServiceType.IN_PROC,
                ModeType.CLIENT_SERVER, "0.0.0.0", 0, QualityOfService.HIGH);
        OcPlatform.Configure(cfg);
    }

    /**
     * @brief Function is for getting platform information
     * @return OcPlatformInfo
     */
    public OcPlatformInfo getPlatformInfo() {
        configClientServerPlatform();

        OcPlatformInfo platformInfo = new OcPlatformInfo(PLATFORM_ID, MANUFACTURER_NAME, MANUFACTURER_URL);
        platformInfo.setModelNumber(MODEL_NO);
        platformInfo.setDateOfManufacture(MANUFACTURE_DATE);
        platformInfo.setPlatformVersion(PLATFORM_VERSION);
        platformInfo.setOperatingSystemVersion(OS_VERSION);
        platformInfo.setHardwareVersion(HW_VERSION);
        platformInfo.setFirmwareVersion(FIRMWARE_VERSION);
        platformInfo.setSupportUrl(SUPPORT_URL);
        platformInfo.setSystemTime(SYSTEM_TIME);

        return platformInfo;
    }

    /**
     * @brief Function is for getting device information
     * @return OcDeviceInfo
     */
    public OcDeviceInfo getDeviceInfo() {
        configClientServerPlatform();

        List<String> deviceTypes = new ArrayList<String>();
        deviceTypes.add(DEVICE_TYPE_AC);

        OcDeviceInfo deviceInfo = new OcDeviceInfo(DEVICE_NAME, deviceTypes);
        return deviceInfo;
    }

    /**
     * Entity Handler for resource register
     */
    OcPlatform.EntityHandler entityHandler = new OcPlatform.EntityHandler() {

        @Override
        public EntityHandlerResult handleEntity(final OcResourceRequest request) {
            EntityHandlerResult result = EntityHandlerResult.OK;

            Log.i(LOG_TAG, "In Server entity handler:");

            return EntityHandlerResult.OK;
        }
    };

    /**
     * @brief Function is for registering resource
     * @return OcResourceHandle
     */
    public OcResourceHandle registerResource() {
        configClientServerPlatform();
        try {
            m_resourceHandle = OcPlatform.registerResource(RESOURCE_URI_TEMPERATURE, RESOURCE_TYPE_TEMPERATURE, OcPlatform.DEFAULT_INTERFACE,
                    entityHandler, m_resourceProperty);
        } catch (Exception e) {
            Log.i(LOG_TAG, "Exception occured inside registerResource");
        }
        return m_resourceHandle;
    }
}

