package oic.simulator.serviceprovider.model;

import java.util.HashMap;
import java.util.Map;

import org.oic.simulator.AttributeValue;
import org.oic.simulator.InvalidArgsException;
import org.oic.simulator.SimulatorResourceAttribute;
import org.oic.simulator.SimulatorResourceModel;
import org.oic.simulator.server.SimulatorResource.AutoUpdateType;

import oic.simulator.serviceprovider.utils.AttributeValueStringConverter;
import oic.simulator.serviceprovider.utils.Constants;

public class AttributeElement {
    private Object                        mParent             = null;
    private SimulatorResourceAttribute    mAttribute          = null;
    private Map<String, AttributeElement> mChildAttributes    = new HashMap<String, AttributeElement>();
    private DataChangeListener            mListener           = null;
    private boolean                       mAutoUpdateSupport  = false;
    private int                           mAutoUpdateId       = -1;
    private boolean                       mAutoUpdateState    = false;
    private int                           mAutoUpdateInterval = Constants.DEFAULT_AUTOMATION_INTERVAL;
    private AutoUpdateType                mAutoUpdateType     = Constants.DEFAULT_AUTOMATION_TYPE;

    public AttributeElement(Object parent,
            SimulatorResourceAttribute attribute, boolean autoUpdateSupport) {
        mParent = parent;
        mAttribute = attribute;
        mAutoUpdateSupport = autoUpdateSupport;
        AttributeValue.TypeInfo typeInfo = attribute.value().typeInfo();
        if (typeInfo.mType == AttributeValue.ValueType.RESOURCEMODEL) {
            mAutoUpdateSupport = false;
            SimulatorResourceModel resModel = (SimulatorResourceModel) attribute
                    .value().get();
            for (Map.Entry<String, SimulatorResourceAttribute> entrySet : resModel
                    .getAttributes().entrySet())
                mChildAttributes.put(entrySet.getKey(), new AttributeElement(
                        this, entrySet.getValue(), false));
        } else if (typeInfo.mType == AttributeValue.ValueType.ARRAY) {
            mAutoUpdateSupport = false;
            if (typeInfo.mBaseType == AttributeValue.ValueType.RESOURCEMODEL) {
                if (typeInfo.mDepth == 1) {
                    SimulatorResourceModel[] resModelArray = (SimulatorResourceModel[]) attribute
                            .value().get();
                    for (int i = 0; i < resModelArray.length; i++) {
                        SimulatorResourceAttribute indexAttribute = new SimulatorResourceAttribute(
                                "[" + Integer.toString(i) + "]",
                                new AttributeValue(resModelArray[i]), null);
                        mChildAttributes.put("[" + Integer.toString(i) + "]",
                                new AttributeElement(this, indexAttribute,
                                        false));
                    }
                } else if (typeInfo.mDepth == 2) {
                    SimulatorResourceModel[][] resModelArray = (SimulatorResourceModel[][]) attribute
                            .value().get();
                    for (int i = 0; i < resModelArray.length; i++) {
                        SimulatorResourceAttribute indexAttribute = new SimulatorResourceAttribute(
                                "[" + Integer.toString(i) + "]",
                                new AttributeValue(resModelArray[i]), null);
                        mChildAttributes.put("[" + Integer.toString(i) + "]",
                                new AttributeElement(this, indexAttribute,
                                        false));
                    }
                } else if (typeInfo.mDepth == 3) {
                    SimulatorResourceModel[][][] resModelArray = (SimulatorResourceModel[][][]) attribute
                            .value().get();
                    for (int i = 0; i < resModelArray.length; i++) {
                        SimulatorResourceAttribute indexAttribute = new SimulatorResourceAttribute(
                                "[" + Integer.toString(i) + "]",
                                new AttributeValue(resModelArray[i]), null);
                        mChildAttributes.put("[" + Integer.toString(i) + "]",
                                new AttributeElement(this, indexAttribute,
                                        false));
                    }
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

    public int getAutoUpdateId() {
        return mAutoUpdateId;
    }

    public void setAutoUpdateId(int id) {
        mAutoUpdateId = id;
    }

    public boolean isAutoUpdateInProgress() {
        return mAutoUpdateState;
    }

    public void setAutoUpdateState(boolean state) {
        if (mAutoUpdateState != state) {
            mAutoUpdateState = state;
            if (mListener != null)
                mListener.update(this);
        }
    }

    public int getAutoUpdateInterval() {
        return mAutoUpdateInterval;
    }

    public void setAutoUpdateInterval(int interval) {
        mAutoUpdateInterval = interval;
    }

    public AutoUpdateType getAutoUpdateType() {
        return mAutoUpdateType;
    }

    public void setAutoUpdateType(AutoUpdateType type) {
        mAutoUpdateType = type;
    }

    public boolean isAutoUpdateSupport() {
        return mAutoUpdateSupport;
    }

    public boolean isReadOnly() {
        return (null == mAttribute.property());
    }

    public void setListener(DataChangeListener listener) {
        mListener = listener;
        for (Map.Entry<String, AttributeElement> entry : mChildAttributes
                .entrySet())
            entry.getValue().setListener(listener);
    }

    public DataChangeListener getListener() {
        return mListener;
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
                            entry.getValue(), false);
                    mChildAttributes.put(entry.getKey(), newAttribute);
                    if (mListener != null)
                        mListener.add(newAttribute);
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
                                this, indexAttribute, false);
                        mChildAttributes.put("[" + Integer.toString(i) + "]",
                                newAttribute);
                        if (mListener != null)
                            mListener.add(newAttribute);
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
                                this, indexAttribute, false);
                        mChildAttributes.put("[" + Integer.toString(i) + "]",
                                newAttribute);
                        if (mListener != null)
                            mListener.add(newAttribute);
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
                                this, indexAttribute, false);
                        mChildAttributes.put("[" + Integer.toString(i) + "]",
                                newAttribute);
                        if (mListener != null)
                            mListener.add(newAttribute);
                    }
                }
            }
        } else {
            String currentValue = new AttributeValueStringConverter(
                    mAttribute.value()).toString();
            String newValue = new AttributeValueStringConverter(
                    attribute.value()).toString();
            if (!currentValue.equals(newValue)) {
                mAttribute = attribute;
                if (mListener != null)
                    mListener.update(this);
            }
        }
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
