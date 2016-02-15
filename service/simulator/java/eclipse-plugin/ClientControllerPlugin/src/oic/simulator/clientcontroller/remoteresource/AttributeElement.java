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

package oic.simulator.clientcontroller.remoteresource;

import java.util.HashMap;
import java.util.Map;

import oic.simulator.clientcontroller.utils.AttributeValueStringConverter;

import org.oic.simulator.AttributeValue;
import org.oic.simulator.InvalidArgsException;
import org.oic.simulator.SimulatorResourceAttribute;
import org.oic.simulator.SimulatorResourceModel;

public class AttributeElement {
    private Object                        mParent          = null;
    private SimulatorResourceAttribute    mAttribute       = null;
    private Map<String, AttributeElement> mChildAttributes = new HashMap<String, AttributeElement>();
    private boolean                       mPostState       = false;

    public AttributeElement(Object parent, SimulatorResourceAttribute attribute) {
        mParent = parent;
        mAttribute = attribute;
        AttributeValue.TypeInfo typeInfo = attribute.value().typeInfo();
        if (typeInfo.mType == AttributeValue.ValueType.RESOURCEMODEL) {
            SimulatorResourceModel resModel = (SimulatorResourceModel) attribute
                    .value().get();
            for (Map.Entry<String, SimulatorResourceAttribute> entrySet : resModel
                    .getAttributes().entrySet())
                mChildAttributes.put(entrySet.getKey(), new AttributeElement(
                        this, entrySet.getValue()));
        } else if (typeInfo.mType == AttributeValue.ValueType.ARRAY
                && typeInfo.mBaseType == AttributeValue.ValueType.RESOURCEMODEL) {
            if (typeInfo.mDepth == 1) {
                SimulatorResourceModel[] resModelArray = (SimulatorResourceModel[]) attribute
                        .value().get();
                for (int i = 0; i < resModelArray.length; i++) {
                    SimulatorResourceAttribute indexAttribute = new SimulatorResourceAttribute(
                            "[" + Integer.toString(i) + "]",
                            new AttributeValue(resModelArray[i]), null);
                    mChildAttributes.put("[" + Integer.toString(i) + "]",
                            new AttributeElement(this, indexAttribute));
                }
            } else if (typeInfo.mDepth == 2) {
                SimulatorResourceModel[][] resModelArray = (SimulatorResourceModel[][]) attribute
                        .value().get();
                for (int i = 0; i < resModelArray.length; i++) {
                    SimulatorResourceAttribute indexAttribute = new SimulatorResourceAttribute(
                            "[" + Integer.toString(i) + "]",
                            new AttributeValue(resModelArray[i]), null);
                    mChildAttributes.put("[" + Integer.toString(i) + "]",
                            new AttributeElement(this, indexAttribute));
                }
            } else if (typeInfo.mDepth == 3) {
                SimulatorResourceModel[][][] resModelArray = (SimulatorResourceModel[][][]) attribute
                        .value().get();
                for (int i = 0; i < resModelArray.length; i++) {
                    SimulatorResourceAttribute indexAttribute = new SimulatorResourceAttribute(
                            "[" + Integer.toString(i) + "]",
                            new AttributeValue(resModelArray[i]), null);
                    mChildAttributes.put("[" + Integer.toString(i) + "]",
                            new AttributeElement(this, indexAttribute));
                }
            }
        }
    }

    public Object getParent() {
        return mParent;
    }

    public boolean hasChildren() {
        if (mChildAttributes != null && mChildAttributes.size() > 0)
            return true;
        return false;
    }

    public Map<String, AttributeElement> getChildren() {
        if (hasChildren() == true)
            return mChildAttributes;
        return null;
    }

    public SimulatorResourceAttribute getSimulatorResourceAttribute() {
        return mAttribute;
    }

    public boolean isPostSupported() {
        /* Auto update is disabled for ARRAY and RESOURCEMODEL type values */
        AttributeValue.TypeInfo typeInfo = mAttribute.value().typeInfo();
        return (!(typeInfo.mType == AttributeValue.ValueType.ARRAY && typeInfo.mBaseType == AttributeValue.ValueType.RESOURCEMODEL) && typeInfo.mType != AttributeValue.ValueType.RESOURCEMODEL);
    }

    public void update(SimulatorResourceAttribute attribute) {
        if (attribute == null)
            return;

        AttributeValue.TypeInfo typeInfo = attribute.value().typeInfo();
        if (typeInfo.mType == AttributeValue.ValueType.RESOURCEMODEL) {
            SimulatorResourceModel resModel = (SimulatorResourceModel) attribute
                    .value().get();
            for (Map.Entry<String, SimulatorResourceAttribute> entry : resModel
                    .getAttributes().entrySet()) {
                AttributeElement attributeElement = mChildAttributes.get(entry
                        .getKey());
                if (attributeElement != null) {
                    attributeElement.update(entry.getValue());
                } else // Display new attribute in UI
                {
                    AttributeElement newAttribute = new AttributeElement(this,
                            entry.getValue());
                    mChildAttributes.put(entry.getKey(), newAttribute);
                }
            }
        } else if (typeInfo.mType == AttributeValue.ValueType.ARRAY
                && typeInfo.mBaseType == AttributeValue.ValueType.RESOURCEMODEL) {
            if (typeInfo.mDepth == 1) {
                SimulatorResourceModel[] resModelArray = (SimulatorResourceModel[]) attribute
                        .value().get();
                for (int i = 0; i < resModelArray.length; i++) {
                    SimulatorResourceAttribute indexAttribute = new SimulatorResourceAttribute(
                            "[" + Integer.toString(i) + "]",
                            new AttributeValue(resModelArray[i]), null);
                    AttributeElement attributeElement = mChildAttributes
                            .get("[" + Integer.toString(i) + "]");
                    if (attributeElement != null) {
                        attributeElement.update(indexAttribute);
                    } else // Display new attribute in UI
                    {
                        AttributeElement newAttribute = new AttributeElement(
                                this, indexAttribute);
                        mChildAttributes.put("[" + Integer.toString(i) + "]",
                                newAttribute);
                    }
                }
            }
            if (typeInfo.mDepth == 2) {
                SimulatorResourceModel[][] resModelArray = (SimulatorResourceModel[][]) attribute
                        .value().get();
                for (int i = 0; i < resModelArray.length; i++) {
                    SimulatorResourceAttribute indexAttribute = new SimulatorResourceAttribute(
                            "[" + Integer.toString(i) + "]",
                            new AttributeValue(resModelArray[i]), null);
                    AttributeElement attributeElement = mChildAttributes
                            .get("[" + Integer.toString(i) + "]");
                    if (attributeElement != null) {
                        attributeElement.update(indexAttribute);
                    } else // Display new attribute in UI
                    {
                        AttributeElement newAttribute = new AttributeElement(
                                this, indexAttribute);
                        mChildAttributes.put("[" + Integer.toString(i) + "]",
                                newAttribute);
                    }
                }
            }
            if (typeInfo.mDepth == 3) {
                SimulatorResourceModel[][][] resModelArray = (SimulatorResourceModel[][][]) attribute
                        .value().get();
                for (int i = 0; i < resModelArray.length; i++) {
                    SimulatorResourceAttribute indexAttribute = new SimulatorResourceAttribute(
                            "[" + Integer.toString(i) + "]",
                            new AttributeValue(resModelArray[i]), null);
                    AttributeElement attributeElement = mChildAttributes
                            .get("[" + Integer.toString(i) + "]");
                    if (attributeElement != null) {
                        attributeElement.update(indexAttribute);
                    } else // Display new attribute in UI
                    {
                        AttributeElement newAttribute = new AttributeElement(
                                this, indexAttribute);
                        mChildAttributes.put("[" + Integer.toString(i) + "]",
                                newAttribute);
                    }
                }
            }
        } else {
            String currentValue = new AttributeValueStringConverter(
                    mAttribute.value()).toString();
            String newValue = new AttributeValueStringConverter(
                    attribute.value()).toString();
            if (currentValue != newValue) {
                mAttribute.setValue(attribute.value());
            }
        }
    }

    public void updateForRAMLUpload(SimulatorResourceAttribute attribute) {
        if (attribute == null)
            return;

        AttributeValue.TypeInfo typeInfo = attribute.value().typeInfo();
        if (typeInfo.mType == AttributeValue.ValueType.RESOURCEMODEL) {
            SimulatorResourceModel resModel = (SimulatorResourceModel) attribute
                    .value().get();
            for (Map.Entry<String, SimulatorResourceAttribute> entry : resModel
                    .getAttributes().entrySet()) {
                AttributeElement attributeElement = mChildAttributes.get(entry
                        .getKey());
                if (attributeElement != null) {
                    attributeElement.update(entry.getValue());
                } else // Display new attribute in UI
                {
                    AttributeElement newAttribute = new AttributeElement(this,
                            entry.getValue());
                    mChildAttributes.put(entry.getKey(), newAttribute);
                }
            }
        } else if (typeInfo.mType == AttributeValue.ValueType.ARRAY
                && typeInfo.mBaseType == AttributeValue.ValueType.RESOURCEMODEL) {
            if (typeInfo.mDepth == 1) {
                SimulatorResourceModel[] resModelArray = (SimulatorResourceModel[]) attribute
                        .value().get();
                for (int i = 0; i < resModelArray.length; i++) {
                    SimulatorResourceAttribute indexAttribute = new SimulatorResourceAttribute(
                            "[" + Integer.toString(i) + "]",
                            new AttributeValue(resModelArray[i]), null);
                    AttributeElement attributeElement = mChildAttributes
                            .get("[" + Integer.toString(i) + "]");
                    if (attributeElement != null) {
                        attributeElement.update(indexAttribute);
                    } else // Display new attribute in UI
                    {
                        AttributeElement newAttribute = new AttributeElement(
                                this, indexAttribute);
                        mChildAttributes.put("[" + Integer.toString(i) + "]",
                                newAttribute);
                    }
                }
            }
            if (typeInfo.mDepth == 2) {
                SimulatorResourceModel[][] resModelArray = (SimulatorResourceModel[][]) attribute
                        .value().get();
                for (int i = 0; i < resModelArray.length; i++) {
                    SimulatorResourceAttribute indexAttribute = new SimulatorResourceAttribute(
                            "[" + Integer.toString(i) + "]",
                            new AttributeValue(resModelArray[i]), null);
                    AttributeElement attributeElement = mChildAttributes
                            .get("[" + Integer.toString(i) + "]");
                    if (attributeElement != null) {
                        attributeElement.update(indexAttribute);
                    } else // Display new attribute in UI
                    {
                        AttributeElement newAttribute = new AttributeElement(
                                this, indexAttribute);
                        mChildAttributes.put("[" + Integer.toString(i) + "]",
                                newAttribute);
                    }
                }
            }
            if (typeInfo.mDepth == 3) {
                SimulatorResourceModel[][][] resModelArray = (SimulatorResourceModel[][][]) attribute
                        .value().get();
                for (int i = 0; i < resModelArray.length; i++) {
                    SimulatorResourceAttribute indexAttribute = new SimulatorResourceAttribute(
                            "[" + Integer.toString(i) + "]",
                            new AttributeValue(resModelArray[i]), null);
                    AttributeElement attributeElement = mChildAttributes
                            .get("[" + Integer.toString(i) + "]");
                    if (attributeElement != null) {
                        attributeElement.update(indexAttribute);
                    } else // Display new attribute in UI
                    {
                        AttributeElement newAttribute = new AttributeElement(
                                this, indexAttribute);
                        mChildAttributes.put("[" + Integer.toString(i) + "]",
                                newAttribute);
                    }
                }
            }
        } else {
            mAttribute.setProperty(attribute.property());
        }
    }

    public boolean getPostState() {
        return mPostState;
    }

    public void setPostState(boolean mPostState) {
        this.mPostState = mPostState;
    }

    public void deepSetChildValue(SimulatorResourceAttribute attribute)
            throws InvalidArgsException {
        if (null == attribute || null == attribute.name())
            return;

        AttributeValue.TypeInfo myValuetypeInfo = mAttribute.value().typeInfo();
        if (myValuetypeInfo.mType == AttributeValue.ValueType.RESOURCEMODEL) {
            SimulatorResourceModel resModel = (SimulatorResourceModel) mAttribute
                    .value().get();
            if (resModel.containsAttribute(attribute.name()))
                resModel.setAttributeValue(attribute.name(), attribute.value());
            else
                return;
        }

        if (mParent instanceof AttributeElement)
            ((AttributeElement) mParent).deepSetChildValue(mAttribute);
    }
}
