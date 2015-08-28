/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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
package org.iotivity.service.easysetup.mediator.common;

import org.iotivity.base.OcConnectivityType;
import org.iotivity.service.easysetup.mediator.ip.IPEnrolleeDevice;

/*
* EnrolleeDeviceFactory is a factory class which instantiates a respective transport specific
* EnrolleeDevice class.
* This class will be used during the OnBoarding Phase to create a transport specific Enrollee Device
* after the device is OnBoarded successfully
* The transport specific instantiation of Enrollee Device will be used in all subsequent operations
* after OnBoarding
*/
public class EnrolleeDeviceFactory {
    public static EnrolleeDevice getDevice(OcConnectivityType connectivityType)
    {
        if(connectivityType == OcConnectivityType.CT_IP_USE_V4)
        {
            return new IPEnrolleeDevice();
        }
        return null;
    }
}
