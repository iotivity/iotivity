/******************************************************************
 *
 * Copyright 2018 Samsung Electronics All Rights Reserved.
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
 *
 ******************************************************************/

package org.iotivity.test.rfw.common.devicecontroller.datamodel;

import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;

import org.iotivity.test.rfw.common.devicecontroller.datamodel.JigFunction;
import org.iotivity.test.rfw.common.devicecontroller.datamodel.JigMode;
import org.iotivity.test.rfw.common.devicecontroller.datamodel.JigSolution;

public class JigStatus {
    private JigSolution               solution;
    private boolean                   isVbattOn;
    private JigMode                   mode;
    private Map<JigFunction, Boolean> functions;

    public JigStatus() {
    }

    public JigStatus(JigStatus status) {
        solution = status.solution;

        setFunctions(new HashMap<JigFunction, Boolean>());
        for (Entry<JigFunction, Boolean> entry : status.getFunctions()
                .entrySet()) {
            getFunctions().put(entry.getKey(), entry.getValue());
        }

        isVbattOn = status.isVbattOn();
        mode = status.getMode();
    }

    public JigSolution getSolution() {
        return solution;
    }

    public void setSolution(JigSolution solution) {
        this.solution = solution;
    }

    public boolean isVbattOn() {
        return isVbattOn;
    }

    public void setVbattOn(boolean isVbattOn) {
        this.isVbattOn = isVbattOn;
    }

    public JigMode getMode() {
        return mode;
    }

    public void setMode(JigMode mode) {
        this.mode = mode;
    }

    public Map<JigFunction, Boolean> getFunctions() {
        return functions;
    }

    public void setFunctions(Map<JigFunction, Boolean> functions) {
        this.functions = functions;
    }
}
