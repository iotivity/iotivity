package oic.simulator.serviceprovider.resource;

import java.util.Map;

import org.iotivity.simulator.SimulatorResourceModel;
import org.iotivity.simulator.SimulatorResourceServer;

public class SimulatorResource {
    private String                         resourceURI;
    private String                         resourceName;
    private String                         resourceType;
    private String                         resourceInterface;

    // Reference to objects created in native layer
    // Native methods can be invoked using these object references
    private SimulatorResourceServer        resourceServer;
    private SimulatorResourceModel         resourceModel;

    private Map<String, ResourceAttribute> resourceAttributesMap;

    public String getResourceURI() {
        return resourceURI;
    }

    public void setResourceURI(String resourceURI) {
        this.resourceURI = resourceURI;
    }

    public String getResourceName() {
        return resourceName;
    }

    public void setResourceName(String resourceName) {
        this.resourceName = resourceName;
    }

    public String getResourceType() {
        return resourceType;
    }

    public void setResourceType(String resourceType) {
        this.resourceType = resourceType;
    }

    public String getResourceInterface() {
        return resourceInterface;
    }

    public void setResourceInterface(String resourceInterface) {
        this.resourceInterface = resourceInterface;
    }

    public SimulatorResourceServer getResourceServer() {
        return resourceServer;
    }

    public void setResourceServer(SimulatorResourceServer resourceServer) {
        this.resourceServer = resourceServer;
    }

    public SimulatorResourceModel getResourceModel() {
        return resourceModel;
    }

    public void setResourceModel(SimulatorResourceModel resourceModel) {
        this.resourceModel = resourceModel;
    }

    public Map<String, ResourceAttribute> getResourceAttributesMap() {
        return resourceAttributesMap;
    }

    public void setResourceAttributesMap(
            Map<String, ResourceAttribute> resourceAttributesMap) {
        this.resourceAttributesMap = resourceAttributesMap;
    }

}
