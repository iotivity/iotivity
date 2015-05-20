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
 * @file   IActionListener.java
 *
 * @brief  This file provides interface for receiving the callback for the GET, PUT and POST requested
 *        actions.
 *
 */

package org.iotivity.service.tm;

import java.util.Vector;

import org.iotivity.base.OcHeaderOption;
import org.iotivity.base.OcRepresentation;

/**
 * Provides interface for receiving the callback for the GET, PUT and
 * POST requested actions.
 */
public interface IActionListener {
    /**
     * This callback method is called when a asynchronous response for the getActionSet
     * request is received.
     *
     * @param headerOptions
     *            It comprises of optionID and optionData as members.
     * @param rep
     *            Configuration parameters are carried as a pair of attribute
     *            key and value in a form of OCRepresentation instance.
     * @param errorValue
     *            error code.
     *
     */
    public void onGetResponseCallback(Vector<OcHeaderOption> headerOptions,
            OcRepresentation rep, int errorValue);

    /**
     * This callback method is called when a asynchronous response for the addActionSet
     * request is received.
     *
     * @param headerOptions
     *            It comprises of optionID and optionData as members.
     * @param rep
     *            Configuration parameters are carried as a pair of attribute
     *            key and value in a form of OCRepresentation instance.
     * @param errorValue
     *            error code.
     *
     */
    public void onPutResponseCallback(Vector<OcHeaderOption> headerOptions,
            OcRepresentation rep, int errorValue);

    /**
     * This callback method is called when a asynchronous response for the executeActionSet
     * or deleteActionSet request is received.
     *
     * @param headerOptions
     *            It comprises of optionID and optionData as members.
     * @param rep
     *            Configuration parameters are carried as a pair of attribute
     *            key and value in a form of OCRepresentation instance.
     * @param errorValue
     *            error code.
     *
     */
    public void onPostResponseCallback(Vector<OcHeaderOption> headerOptions,
            OcRepresentation rep, int errorValue);
}
