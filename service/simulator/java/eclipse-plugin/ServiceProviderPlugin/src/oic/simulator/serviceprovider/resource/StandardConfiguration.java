package oic.simulator.serviceprovider.resource;

import java.util.HashMap;
import java.util.Map;

public class StandardConfiguration {

    private String      configuration_directory_path;

    // A map of resourceType of standard resources as the key and the complete
    // location of the file as the value
    Map<String, String> standardResourceConfigurationMap;

    public StandardConfiguration() {
        standardResourceConfigurationMap = new HashMap<String, String>();
    }

    public String getConfiguration_directory_path() {
        return configuration_directory_path;
    }

    public void setConfiguration_directory_path(
            String configuration_directory_path) {
        this.configuration_directory_path = configuration_directory_path;
    }

    public Map<String, String> getStandardResourceConfigurationList() {
        return standardResourceConfigurationMap;
    }

    public void setStandardResourceConfigurationList(
            Map<String, String> standardResourceConfigurationList) {
        this.standardResourceConfigurationMap = standardResourceConfigurationList;
    }

    public void addResourceConfiguration(String resourceType,
            String absolutePath) {
        if (null != resourceType && resourceType.length() > 0
                && null != absolutePath && absolutePath.length() > 0) {
            standardResourceConfigurationMap.put(resourceType, absolutePath);
        }
    }

    public String getResourceConfigFilePath(String resourceType) {
        String path = null;
        if (null != resourceType && resourceType.length() > 0) {
            path = standardResourceConfigurationMap.get(resourceType);
        }
        return path;
    }

    public void removeResourceFromConfiguration(String resourceType) {
        if (null != resourceType && resourceType.length() > 0) {
            standardResourceConfigurationMap.remove(resourceType);
        }
    }
}