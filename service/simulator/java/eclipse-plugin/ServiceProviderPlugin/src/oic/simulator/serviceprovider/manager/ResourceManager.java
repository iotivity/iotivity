package oic.simulator.serviceprovider.manager;

import java.util.ArrayList;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.Vector;

import oic.simulator.serviceprovider.listener.IResourceListChangedListener;
import oic.simulator.serviceprovider.resource.ResourceAttribute;
import oic.simulator.serviceprovider.resource.SimulatorResource;
import oic.simulator.serviceprovider.resource.StandardConfiguration;

import org.iotivity.simulator.SimulatorManager;
import org.iotivity.simulator.SimulatorResourceAttribute;
import org.iotivity.simulator.SimulatorResourceModel;
import org.iotivity.simulator.SimulatorResourceServer;

public class ResourceManager {

    private Map<String, Map<String, SimulatorResource>> resourceMap;

    private StandardConfiguration                       stdConfig;

    private IResourceListChangedListener                resourceListChangedListener;

    static {
        System.loadLibrary("SimulatorManager");
    }

    public ResourceManager() {
        resourceMap = new HashMap<String, Map<String, SimulatorResource>>();
        stdConfig = new StandardConfiguration();

        // Populate standard configuration file list
        populateStandardConfigurationList();
    }

    public void populateStandardConfigurationList() {
        // TODO: Add all the standard configuration files
        // Ex: stdConfig.addResourceConfiguration(LIGHT, LIGHT_FILE);
    }

    public synchronized List<String> getResourceConfigurationList() {
        List<String> resourceConfigurationList = new ArrayList<String>();
        Map<String, String> configMap = stdConfig
                .getStandardResourceConfigurationList();
        if (null != configMap) {
            Set<String> keySet = configMap.keySet();
            if (null != keySet) {
                Iterator<String> keyItr = keySet.iterator();
                while (keyItr.hasNext()) {
                    resourceConfigurationList.add(keyItr.next());
                }
            }
        }
        return resourceConfigurationList;
    }

    public synchronized String getConfigFilePath(String resourceType) {
        String path = null;
        if (null != resourceType) {
            path = stdConfig.getResourceConfigFilePath(resourceType);
        }
        return path;
    }

    private synchronized void addResourceToMap(
            SimulatorResource simulatorResource) {
        if (null != simulatorResource) {
            Map<String, SimulatorResource> resourceTypeMap;
            resourceTypeMap = resourceMap.get(simulatorResource
                    .getResourceType());
            if (null == resourceTypeMap) {
                resourceTypeMap = new HashMap<String, SimulatorResource>();
                resourceMap.put(simulatorResource.getResourceType(),
                        resourceTypeMap);
            }
            resourceTypeMap.put(simulatorResource.getResourceURI(),
                    simulatorResource);
        }
    }

    private synchronized void addResourceToMap(String resourceType,
            Map<String, SimulatorResource> newResourceTypeMap) {
        if (null != resourceType && null != newResourceTypeMap) {
            Map<String, SimulatorResource> resourceTypeMap = resourceMap
                    .get(resourceType);
            if (null != resourceTypeMap) {
                resourceTypeMap.putAll(newResourceTypeMap);
            } else {
                resourceMap.put(resourceType, newResourceTypeMap);
            }
        }
    }

    private synchronized void removeResourceFromMap(String resourceType,
            String resourceURI) {
        if (null != resourceURI && null != resourceType) {
            Map<String, SimulatorResource> resourceTypeMap = resourceMap
                    .get(resourceType);
            if (null != resourceTypeMap) {
                resourceTypeMap.remove(resourceURI);
                if (resourceTypeMap.size() < 1) {
                    resourceMap.remove(resourceType);
                }
            }
        }
    }

    public synchronized boolean isResourceExist(String resourceURI) {
        boolean result = false;
        if (null != resourceURI) {
            SimulatorResource resource = getSimulatorResourceByURI(resourceURI);
            if (null != resource) {
                result = true;
            }
        }
        return result;
    }

    public synchronized void createResource(String configFilePath) {
        SimulatorResourceServer resourceServerN;
        resourceServerN = SimulatorManager.createResource(configFilePath);

        SimulatorResource simulatorResource;
        simulatorResource = fetchResourceData(resourceServerN);
        if (null != simulatorResource) {
            addResourceToMap(simulatorResource);

            String resourceType = simulatorResource.getResourceType();
            String resourceURI = simulatorResource.getResourceURI();
            List<String> resourceURIList = new ArrayList<String>();
            resourceURIList.add(resourceURI);

            resourceCreatedNotification(resourceType, resourceURIList);
        }
    }

    public synchronized void createResource(String configFilePath,
            int noOfInstances) {
        Map<String, SimulatorResource> resourceTypeMap;
        List<String> resourceUriList;
        Vector<SimulatorResourceServer> simulatorResourceServerVectorN = null;
        simulatorResourceServerVectorN = SimulatorManager.createResource(
                configFilePath, noOfInstances);
        if (null != simulatorResourceServerVectorN) {
            resourceTypeMap = new HashMap<String, SimulatorResource>();
            resourceUriList = new ArrayList<String>();
            Enumeration<SimulatorResourceServer> uriItr = simulatorResourceServerVectorN
                    .elements();
            SimulatorResourceServer resourceServerN;
            SimulatorResource resource;
            String uri;
            while (uriItr.hasMoreElements()) {
                resourceServerN = uriItr.nextElement();
                resource = fetchResourceData(resourceServerN);
                if (null != resource) {
                    uri = resource.getResourceURI();
                    resourceUriList.add(uri);
                    resourceTypeMap.put(uri, resource);
                }
            }

            // Find the resourceType and add it to the local data structure and
            // notify UI Listeners
            if (resourceTypeMap.size() > 0) {
                String resourceType;
                Set<String> uriSet = resourceTypeMap.keySet();
                if (null != uriSet) {
                    Iterator<String> itr = uriSet.iterator();
                    if (itr.hasNext()) {
                        SimulatorResource simResource = resourceTypeMap.get(itr
                                .next());
                        if (null != simResource) {
                            resourceType = simResource.getResourceType();

                            addResourceToMap(resourceType, resourceTypeMap);

                            resourceCreatedNotification(resourceType,
                                    resourceUriList);
                        }
                    }
                }
            }
        }

    }

    private synchronized SimulatorResource fetchResourceData(
            SimulatorResourceServer resourceServerN) {
        SimulatorResource simulatorResource = null;
        if (null != resourceServerN) {
            Map<String, ResourceAttribute> resourceAttributeMap = null;

            simulatorResource = new SimulatorResource();
            simulatorResource.setResourceServer(resourceServerN);
            simulatorResource.setResourceURI(resourceServerN.getURI());
            simulatorResource
                    .setResourceType(resourceServerN.getResourceType());
            simulatorResource.setResourceName(resourceServerN.getName());
            simulatorResource.setResourceInterface(resourceServerN
                    .getInterfaceType());

            SimulatorResourceModel resourceModelN = resourceServerN.getModel();
            if (null != resourceModelN) {
                simulatorResource.setResourceModel(resourceModelN);

                Map<String, SimulatorResourceAttribute> attributeMapN;
                attributeMapN = resourceModelN.getAttributes();
                if (null != attributeMapN) {
                    resourceAttributeMap = new HashMap<String, ResourceAttribute>();

                    Set<String> attNameSet = attributeMapN.keySet();
                    if (null != attNameSet) {
                        String attName;
                        SimulatorResourceAttribute attributeN;
                        ResourceAttribute attribute;
                        Iterator<String> attNameItr = attNameSet.iterator();
                        while (attNameItr.hasNext()) {
                            attName = attNameItr.next();
                            attributeN = attributeMapN.get(attName);
                            if (null != attributeN) {
                                attribute = new ResourceAttribute();
                                attribute.setAttributeName(attName);

                                // Other attribute details such as value,
                                // allowed values and range are based on type
                                // Type implementation is yet to be done
                                resourceAttributeMap.put(attName, attribute);
                            }
                        }
                        simulatorResource
                                .setResourceAttributesMap(resourceAttributeMap);
                    }
                }
            }
        }
        return simulatorResource;
    }

    private synchronized void resourceCreatedNotification(String resourceType,
            List<String> resourceURIList) {
        if (null != resourceType && null != resourceURIList
                && null != resourceListChangedListener) {
            resourceListChangedListener.onResourceCreation(resourceType,
                    resourceURIList);
        }
    }

    public synchronized void deleteResourceByURI(String resourceURI) {
        if (null != resourceURI) {
            SimulatorResource resource = getSimulatorResourceByURI(resourceURI);
            if (null != resource) {
                String resourceType = resource.getResourceType();

                // Unregister the resource from the platform
                deleteResource(resource);

                // Delete from the local data structure
                deleteLocalResourceDetails(resourceType, resourceURI);

                // Notify the UI listener for removing this resource from UI
                resourceDeletedNotification(resourceType, resourceURI);
            }
        }
    }

    private synchronized SimulatorResource getSimulatorResourceByURI(
            String resourceURI) {
        SimulatorResource resource = null;
        if (null != resourceURI) {
            Set<String> typeSet = resourceMap.keySet();
            if (null != typeSet) {
                Iterator<String> typeItr = typeSet.iterator();
                String resourceType;
                Map<String, SimulatorResource> resourceTypeMap;
                while (typeItr.hasNext()) {
                    resourceType = typeItr.next();
                    resourceTypeMap = resourceMap.get(resourceType);
                    if (null != resourceTypeMap) {
                        resource = resourceTypeMap.get(resourceURI);
                        if (null != resource) {
                            break;
                        }
                    }
                }
            }
        }
        return resource;
    }

    private synchronized void deleteResource(SimulatorResource resource) {
        if (null != resource) {
            SimulatorResourceServer resourceServerN = resource
                    .getResourceServer();
            if (null != resourceServerN) {
                SimulatorManager.deleteResource(resourceServerN);
            }
        }
    }

    public synchronized void deleteResourceByType(String resourceType) {
        if (null != resourceType) {
            // Unregister the resources from the platform
            deleteResource(resourceType);

            // Delete from the local data structure
            deleteLocalResourceDetails(resourceType, null);

            // Notify the UI listener for removing this resource from UI
            resourceTypeDeletedNotification(resourceType);

        }
    }

    private synchronized void deleteResource(String resourceType) {
        if (null != resourceType) {
            SimulatorManager.deleteResources(resourceType);
        }
    }

    public synchronized void deleteAllResources() {
        // Unregister the resources from the platform
        deleteResource();

        // Delete from the local data structure
        deleteLocalResourceDetails(null, null);

        // Notify the UI listener for removing this resource from UI
        allResourcesDeletedNotification();
    }

    private synchronized void deleteResource() {
        // SimulatorManager.deleteResources();
    }

    private synchronized void deleteLocalResourceDetails(String resourceType,
            String resourceURI) {
        if (null != resourceType && null != resourceURI) {
            removeResourceFromMap(resourceType, resourceURI);
        } else if (null != resourceType) {
            resourceMap.remove(resourceType);
        } else {
            resourceMap.clear();
        }
    }

    private synchronized void resourceDeletedNotification(String resourceType,
            String resourceURI) {
        if (null != resourceListChangedListener) {
            resourceListChangedListener.onResourceDeletion(resourceType,
                    resourceURI);
        }
    }

    private synchronized void resourceTypeDeletedNotification(
            String resourceType) {
        if (null != resourceListChangedListener) {
            resourceListChangedListener.onResourceDeletionByType(resourceType);
        }
    }

    private synchronized void allResourcesDeletedNotification() {
        if (null != resourceListChangedListener) {
            resourceListChangedListener.onAllResourceDeletion();
        }
    }

    public synchronized IResourceListChangedListener getResourceListChangedListener() {
        return resourceListChangedListener;
    }

    public synchronized void setResourceListChangedListener(
            IResourceListChangedListener resourceListChangedListener) {
        this.resourceListChangedListener = resourceListChangedListener;
    }

    public synchronized List<String> getResourceTypeList() {
        List<String> typeList = null;
        if (null != resourceMap && resourceMap.size() > 0) {
            typeList = new ArrayList<String>();
            Set<String> typeSet = resourceMap.keySet();
            Iterator<String> typeItr = typeSet.iterator();
            while (typeItr.hasNext()) {
                typeList.add(typeItr.next());
            }
        }
        return typeList;
    }
}
