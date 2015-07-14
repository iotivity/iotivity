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

/*
* EnrolleeDevice is a super class for handling Enrollees on different transports.
* All the transport specific Enrollee Devices inherit from this base class and provide the
* transport specific implementation in the transport specific subclasses.
*/
public class EnrolleeDevice {
    private OcConnectivityType connectivityType;
    private boolean isReachable;

    public boolean isReachable() {
        return isReachable;
    }

    public void setReachable(boolean isReachable) {
        this.isReachable = isReachable;
    }

    public OcConnectivityType getConnectivityType() {
        return connectivityType;
    }

    public void setConnectivityType(OcConnectivityType connectivityType) {
        this.connectivityType = connectivityType;
    }
}
