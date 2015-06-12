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
 * @file    ISubscribePresenceListener.java
 *
 * @brief   This file provides interface for getting child resource presence status.
 *
 */

package org.iotivity.service.tm;

/**
 * Provides interface for getting child resource presence status.
 */
public interface ISubscribePresenceListener {
    /**
     * This callback method will be called for child resource presence status.
     *
     * @param resource
     *            URI of resource.
     * @param result
     *            error code.
     */
    public void onPresenceCallback(String resource, int result);
}
