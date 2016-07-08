/**
 * ***************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
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
 *
 * ****************************************************************
 */

package org.iotivity.service.easysetup.mediator;

import org.iotivity.service.easysetup.mediator.enums.WIFI_FREQ;
import org.iotivity.service.easysetup.mediator.enums.WIFI_MODE;

import java.util.ArrayList;

public class PropertyData
{
    private final DeviceConfig mDevConf;
    private final NetworkInfo mNetInfo;
    private final boolean mCloudable;

    public PropertyData(DeviceConfig devConf, NetworkInfo netInfo, boolean cloudable)
    {
        mDevConf = devConf;
        mNetInfo = netInfo;
        mCloudable = cloudable;
    }

    public DeviceConfig getDeviceConfig()
    {
        return mDevConf;
    }

    public NetworkInfo getNetworkInfo()
    {
        return mNetInfo;
    }

    public boolean isCloudable()
    {
        return mCloudable;
    }
}


