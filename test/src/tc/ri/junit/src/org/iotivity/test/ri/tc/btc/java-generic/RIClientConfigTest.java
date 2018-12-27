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

import org.iotivity.base.ModeType;
import org.iotivity.base.OcConnectivityType;
import org.iotivity.base.OcException;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ResourceProperty;
import org.iotivity.base.ServiceType;
import org.iotivity.test.ri.tc.helper.RIHelper;

import org.iotivity.testcase.IoTivityLog;
import org.iotivity.testcase.IoTivityTc;

public class RIClientConfigTest extends IoTivityTc {
    private final String     LOG_TAG = this.getClass().getSimpleName();

    protected void setUp() {
        IoTivityLog.i(LOG_TAG, "SetUp Finished");
    }

    protected void tearDown() {
        IoTivityLog.i(LOG_TAG, "tearDown Finished");
    }

    /**
     * @objective Test Configure function with positive basic way with InProc
     *            ServiceType
     * @target void Configure(PlatformConfig platformConfig)
     * @test_data PlatformConfig platform configuration
     * @pre_condition configure with InProc ServiceType
     * @procedure Call Configure() API
     * @post_condition None
     * @expected No exceptions should occur
     * @see None
     * @since 2018-01-10
     **/
    public void testConfigureClient_SRC_P() {
        try {
            PlatformConfig cfg = new PlatformConfig(ServiceType.IN_PROC,
                    ModeType.CLIENT, "0.0.0.0", 0, QualityOfService.HIGH);
            OcPlatform.Configure(cfg);
        } catch (Exception e) {
            e.printStackTrace();
            fail("Exception occured");
        }
    }

}