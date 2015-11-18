package oic.simulator.serviceprovider.model;

import java.util.HashMap;
import java.util.Map;

import org.oic.simulator.SimulatorResourceAttribute;
import org.oic.simulator.SimulatorResourceModel;

public class ResourceRepresentation {
    private DataChangeListener            mListener   = null;
    private Map<String, AttributeElement> mAttributes = new HashMap<String, AttributeElement>();

    public ResourceRepresentation(SimulatorResourceModel resourceModel) {
        if (resourceModel != null && resourceModel.size() > 0) {
            for (Map.Entry<String, SimulatorResourceAttribute> entry : resourceModel
                    .getAttributes().entrySet())
                mAttributes.put(entry.getKey(), new AttributeElement(this,
                        entry.getValue(), true));
        }
    }

    public Map<String, AttributeElement> getAttributes() {
        return mAttributes;
    }

    public void setListener(DataChangeListener listener) {
        mListener = listener;
        for (Map.Entry<String, AttributeElement> entry : mAttributes.entrySet())
            entry.getValue().setListener(listener);
    }

    public DataChangeListener getListener() {
        return mListener;
    }

    public void update(SimulatorResourceModel resourceModel) {
        if (null == resourceModel)
            return;

        for (Map.Entry<String, SimulatorResourceAttribute> entry : resourceModel
                .getAttributes().entrySet()) {
            AttributeElement attributeElement = mAttributes.get(entry.getKey());
            if (attributeElement != null) {
                attributeElement.update(entry.getValue());
            } else // Display new attribute in UI
            {
                AttributeElement newAttribute = new AttributeElement(this,
                        entry.getValue(), true);
                mAttributes.put(entry.getKey(), newAttribute);
                if (mListener != null)
                    mListener.add(newAttribute);
            }
        }
    }
}
