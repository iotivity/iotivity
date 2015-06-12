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

/**
 * @file   IConfigurationListener.java
 *
 * @brief  This file provides interface for receiving asynchronous response for
 *        configuration APIs.
 */

package org.iotivity.service.tm;

import java.util.Vector;

import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.OcRepresentation;

/**
 * Provides interface for receiving asynchronous response for
 * configuration APIs.
 */
public interface IConfigurationListener {
    /**
     * Asynchronous response callback for ThingsManager::doBootstrap API.
     */
    public void onBootStrapCallback(Vector<OcHeaderOption> headerOptions,
            OcRepresentation rep, int errorValue);

    /**
     * Asynchronous response callback for ThingsManager::updateConfigurations
     * API.
     */
    public void onUpdateConfigurationsCallback(
            Vector<OcHeaderOption> headerOptions, OcRepresentation rep,
            int errorValue);

    /**
     * Asynchronous response callback for ThingsManager::getConfigurations API.
     */
    public void onGetConfigurationsCallback(
            Vector<OcHeaderOption> headerOptions, OcRepresentation rep,
            int errorValue);
}
