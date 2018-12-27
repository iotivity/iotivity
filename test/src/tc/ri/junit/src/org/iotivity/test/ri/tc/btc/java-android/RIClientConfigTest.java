/******************************************************************
 *
 * Copyright 2018 Samsung Electronics All Rights Reserved.
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

package org.iotivity.test.ri.tc.btc;

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
import org.iotivity.test.ri.tc.helper.RIHelper;

import org.iotivity.testcase.IoTivityLog;
import org.iotivity.testcase.IoTivityTc;

public class RIClientConfigTest extends IoTivityTc {
    private final String LOG_TAG = this.getClass().getSimpleName();

    private RIHelper m_riHelper;
    private String m_host = "";
    private OcPresenceHandle m_presenceHandle;

    protected void setUp() {
        m_riHelper = RIHelper.getInstance(this);
        m_presenceHandle = null;
        IoTivityLog.i(LOG_TAG, "SetUp Finished");
    }

    protected void tearDown() {
        IoTivityLog.i(LOG_TAG, "tearDown Finished");
    }

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
}