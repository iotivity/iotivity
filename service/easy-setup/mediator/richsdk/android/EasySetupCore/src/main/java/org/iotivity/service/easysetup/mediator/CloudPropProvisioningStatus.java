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

import org.iotivity.service.easysetup.mediator.enums.ESCloudProvState;
import org.iotivity.service.easysetup.mediator.enums.ESResult;

/**
 * This interface facilitates Application to get progress & result of Cloud provisioning
 * process in easy setup
 */
public class CloudPropProvisioningStatus
{
    private ESResult m_result;
    private ESCloudProvState m_esCloudState;

    public CloudPropProvisioningStatus(int result, int state)
    {
        m_result = ESResult.fromInt(result);
        m_esCloudState = ESCloudProvState.fromInt(state);
    }

    public ESResult getESResult()
    {
        return m_result;
    }

    public ESCloudProvState getESCloudState()
    {
        return m_esCloudState;
    }
};
