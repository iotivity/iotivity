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

package oic.simulator.serviceprovider.model;

import org.oic.simulator.SimulatorResourceAttribute;
import org.oic.simulator.server.SimulatorResource.AutoUpdateType;

/**
 * This class represents an attribute in the simulated resource.
 */
public class LocalResourceAttribute {

    // Java SDK object reference
    private SimulatorResourceAttribute resourceAttributeRef;

    private int                        automationId;

    private boolean                    automationInProgress;

    private int                        automationUpdateInterval;

    private AutoUpdateType             automationType;

    public SimulatorResourceAttribute getResourceAttributeRef() {
        return resourceAttributeRef;
    }

    public void setResourceAttributeRef(
            SimulatorResourceAttribute resourceAttribute) {
        this.resourceAttributeRef = resourceAttribute;
    }

    public int getAutomationId() {
        return automationId;
    }

    public void setAutomationId(int automationId) {
        this.automationId = automationId;
    }

    public boolean isAutomationInProgress() {
        return automationInProgress;
    }

    public void setAutomationInProgress(boolean automationInProgress) {
        this.automationInProgress = automationInProgress;
    }

    public int getAutomationUpdateInterval() {
        return automationUpdateInterval;
    }

    public void setAutomationUpdateInterval(int automationUpdateInterval) {
        this.automationUpdateInterval = automationUpdateInterval;
    }

    public AutoUpdateType getAutomationType() {
        return automationType;
    }

    public void setAutomationType(AutoUpdateType automationType) {
        this.automationType = automationType;
    }
}