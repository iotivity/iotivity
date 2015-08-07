/*
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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
 */

/**
 * This file provides interface for getting notification on resource model change.
 */
package org.oic.simulator.clientcontroller;

import org.oic.simulator.serviceprovider.SimulatorResourceModel;

/**
 * Provides interface for getting notification when resource model of an
 * observed resource gets changed. An OnObserveListener can be registered via
 * the SimulatorRemoteResource observe call. Event listeners are notified
 * asynchronously.
 */
public interface IObserveListener {
    /**
     * This callback method will be called whenever the resource model of an
     * observed resource gets changed.
     *
     * @param representation
     *            Updated Simulator Resource Model
     *
     * @param sequenceNumber
     *            Observe Sequence number
     */
    public void onObserveCompleted(SimulatorResourceModel representation,
            int sequenceNumber);

    /**
     * This callback method will be called whenever the resource model of an
     * observed resource gets changed and there is a failure in notifying the
     * updated resource model.
     *
     * @param resource
     *            resource discovered in the network
     */
    public void onObserveFailed(Throwable ex);
}
