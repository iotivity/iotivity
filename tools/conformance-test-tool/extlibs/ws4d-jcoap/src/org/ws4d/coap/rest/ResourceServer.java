
package org.ws4d.coap.rest;

import java.net.URI;

/**
 * A ResourceServer provides network access to resources via a network protocol such as HTTP or CoAP.
 * 
 * @author Nico Laum <nico.laum@uni-rostock.de>
 * @author Christian Lerche <christian.lerche@uni-rostock.de>
 */
public interface ResourceServer {
    /**
     * 
     * @param resource The resource to be handled.
     */
	
	/* creates a resource. resource must not exist. if resource exists, false is returned */
    public boolean createResource(Resource resource);
    /* returns the resource at the given path, null if no resource exists*/ 
    public Resource readResource(String path);
    /* updates a resource. resource must exist. if does not resource exist, false is returned. Resource is NOT created. */
    public boolean updateResource(Resource resource);
    /* deletes resource, returns false is resource does not exist */
    public boolean deleteResource(String path);

    /**
     * Start the ResourceServer. This usually opens network ports and makes the
     * resources available through a certain network protocol.
     */
    public void start() throws Exception;

    /**
     * Stops the ResourceServer.
     */
    public void stop();
    
    /**
     * Returns the Host Uri
     */    
    public URI getHostUri();
    
    public void resourceChanged(Resource resource);
}
