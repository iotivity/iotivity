package oic.simulator.serviceprovider.resource;

import java.util.Map;

import org.oic.simulator.AutomationType;
import org.oic.simulator.serviceprovider.SimulatorResourceModel;
import org.oic.simulator.serviceprovider.SimulatorResourceServer;

public class SimulatorResource {
    private String                         resourceURI;
    private String                         resourceName;
    private String                         resourceType;
    private String                         resourceInterface;

    // Native Object references
    private SimulatorResourceServer        resourceServer;
    private SimulatorResourceModel         resourceModel;

    private int                            automationId;

    private boolean                        resourceAutomationInProgress;

    private boolean                        attributeAutomationInProgress;

    private int                            automationUpdateInterval;

    private AutomationType                 automationType;

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

    public int getAutomationUpdateInterval() {
        return automationUpdateInterval;
    }

    public void setAutomationUpdateInterval(int automationUpdateInterval) {
        this.automationUpdateInterval = automationUpdateInterval;
    }

    public AutomationType getAutomationType() {
        return automationType;
    }

    public void setAutomationType(AutomationType automationType) {
        this.automationType = automationType;
    }

    public int getAutomationId() {
        return automationId;
    }

    public void setAutomationId(int automationId) {
        this.automationId = automationId;
    }

    public boolean isResourceAutomationInProgress() {
        return resourceAutomationInProgress;
    }

    public void setResourceAutomationInProgress(
            boolean resourceAutomationInProgress) {
        this.resourceAutomationInProgress = resourceAutomationInProgress;
    }

    public boolean isAttributeAutomationInProgress() {
        return attributeAutomationInProgress;
    }

    public void setAttributeAutomationInProgress(
            boolean attributeAutomationInProgress) {
        this.attributeAutomationInProgress = attributeAutomationInProgress;
    }
}