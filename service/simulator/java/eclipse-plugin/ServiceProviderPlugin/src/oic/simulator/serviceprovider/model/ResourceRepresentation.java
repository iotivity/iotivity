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

import java.util.HashMap;
import java.util.Map;

import oic.simulator.serviceprovider.manager.UiListenerHandler;

import org.oic.simulator.SimulatorResourceAttribute;
import org.oic.simulator.SimulatorResourceModel;

public class ResourceRepresentation {
    private Map<String, AttributeElement> mAttributes = new HashMap<String, AttributeElement>();

    public ResourceRepresentation(SimulatorResourceModel resourceModel)
            throws NullPointerException {
        if (resourceModel != null && resourceModel.size() > 0) {
            for (Map.Entry<String, SimulatorResourceAttribute> entry : resourceModel
                    .getAttributes().entrySet()) {
                mAttributes.put(entry.getKey(), new AttributeElement(this,
                        entry.getValue(), true));
            }
        }
    }

    public Map<String, AttributeElement> getAttributes() {
        return mAttributes;
    }

    public void update(SimulatorResourceModel resourceModel) {
        if (null == resourceModel)
            return;

        for (Map.Entry<String, SimulatorResourceAttribute> entry : resourceModel
                .getAttributes().entrySet()) {
            AttributeElement attributeElement = mAttributes.get(entry.getKey());
            if (null != attributeElement) {
                if (!attributeElement.getEditLock())
                    attributeElement.update(entry.getValue());
            } else // Display new attribute in UI
            {
                AttributeElement newAttribute = new AttributeElement(this,
                        entry.getValue(), true);
                mAttributes.put(entry.getKey(), newAttribute);

                UiListenerHandler.getInstance().attributeAddedUINotification(
                        newAttribute);
            }
        }
    }
}
