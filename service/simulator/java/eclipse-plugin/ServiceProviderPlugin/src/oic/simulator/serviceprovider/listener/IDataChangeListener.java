package oic.simulator.serviceprovider.listener;

import oic.simulator.serviceprovider.model.AttributeElement;

public interface IDataChangeListener {
    public void add(AttributeElement attribute);

    public void remove(AttributeElement attribute);

    public void update(AttributeElement attribute);
}
