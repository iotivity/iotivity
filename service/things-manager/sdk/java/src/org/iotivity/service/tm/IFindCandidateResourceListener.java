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
 * @file    IFindCandidateResourceListener.java
 *
 * @brief   This file provides interface for getting notification when resources are
 *        discovered in network.
 */

package org.iotivity.service.tm;

import java.util.Vector;

import org.iotivity.base.OcResource;

/**
 * Provides interface for getting notification when resources are
 * discovered in network.
 */
public interface IFindCandidateResourceListener {
    /**
     * This callback method will be called whenever resource is discovered in
     * the network.
     *
     * @param resources
     *            List of resources discovered in the network
     */
    public void onResourceCallback(Vector<OcResource> resources);
}
