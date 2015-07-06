package org.iotivity.resourcecontainer.bundle.api;


import java.util.HashMap;
import java.util.Set;

public abstract class BundleResource 
{	
	protected String m_name, m_uri, m_resourceType, m_address;
	
	protected HashMap<String, String> m_attributes;	    	
	
	protected abstract void initAttributes();
	public abstract void setAttribute(String key, String value);
	public abstract String getAttribute(String key);
	public String[] getAttributeKeys(){
		Set<String> keys = m_attributes.keySet();
		return keys.toArray(new String[keys.size()]);		
	}
	
	public void setURI(String uri){
		this.m_uri = uri;
	}
	
	public String getURI(){
		return m_uri;
	}
	
	public void setResourceType(String resourceType){
		this.m_resourceType = resourceType;
	}
	
	public String getResourceType(){
		return m_resourceType;
	}
	
	public void setAddress(String address){
		this.m_address = address;
	}
	
	public String getAddress(){
		return m_address;
	}
	
	public void setName(String name){
		this.m_name = name;
	}
	
	public String getName(){
		return m_name;
	}

}
