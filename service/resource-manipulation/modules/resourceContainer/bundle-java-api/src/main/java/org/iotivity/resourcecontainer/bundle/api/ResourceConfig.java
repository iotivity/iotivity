package org.iotivity.resourcecontainer.bundle.api;

public class ResourceConfig {
    private String m_name, m_uri, m_resourceType, m_address;

    public ResourceConfig() {

    }

    public ResourceConfig(String[] params) {
        m_name = params[0];
        m_uri = params[1];
        m_resourceType = params[2];
        m_address = params[3];
    }

    public String getM_name() {
        return m_name;
    }

    public void setM_name(String m_name) {
        this.m_name = m_name;
    }

    public String getM_uri() {
        return m_uri;
    }

    public void setM_uri(String m_uri) {
        this.m_uri = m_uri;
    }

    public String getM_resourceType() {
        return m_resourceType;
    }

    public void setM_resourceType(String m_resourceType) {
        this.m_resourceType = m_resourceType;
    }

    public String getM_address() {
        return m_address;
    }

    public void setM_address(String m_address) {
        this.m_address = m_address;
    }

    @Override
    public String toString() {
        return "ResourceConfig [m_name=" + m_name + ", m_uri=" + m_uri
                + ", m_resourceType=" + m_resourceType + ", m_address="
                + m_address + "]";
    }

}
