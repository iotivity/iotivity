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
* ProvisioningInfo is a super class for handling different types of transport provisioning
* informations.
* Different Connectivities have different types of provisioning information. This base class holds
* the common information that is related to all the subclasses.
* Transport specific provisioning information will be available with the subclasses of
* ProvisioningInfo
*/
public class ProvisioningInfo {
    private OcConnectivityType connectivityType;

    public OcConnectivityType getConnectivityType() {
        return connectivityType;
    }

    public void setConnectivityType(OcConnectivityType connectivityType) {
        this.connectivityType = connectivityType;
    }
}
