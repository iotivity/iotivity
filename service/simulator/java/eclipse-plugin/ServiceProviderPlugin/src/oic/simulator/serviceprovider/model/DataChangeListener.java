package oic.simulator.serviceprovider.model;

public interface DataChangeListener {
    public void add(AttributeElement attribute);

    public void remove(AttributeElement attribute);

    public void update(AttributeElement attribute);
}
