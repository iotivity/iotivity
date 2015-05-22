//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

/// @file Plugin.java

/// @brief This class provides information of each plugin installed for JAVA.

package org.iotivity.service.ppm;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Vector;

public class Plugin {

    private Map<String, String> m_attributeMap;
    private Vector<String>      m_supportedType;

    /**
     * Constructor for Plugin.
     *
     */
    public Plugin() {
        m_attributeMap = new HashMap<String, String>();
        m_supportedType = new Vector<String>();
    }

    /**
     * Constructor for Plugin(Overlading).
     *
     * @param Plugin's name and id Strings.
     */
    public Plugin(String name, String id) {
        this();
        setValue("Name", name);
        setValue("Id", id);
    }

    /**
     * Get unique ID of the plugin.
     *
     * @return unique id String of the plugin. if ID is not exist, it return "".
     */
    public String getID() {
        Iterator<Entry<String, String>> m_iterator = m_attributeMap.entrySet()
                .iterator();

        while (m_iterator.hasNext()) {
            Entry<String, String> entryMap = m_iterator.next();
            if (entryMap.getKey().equals("Id"))
                return entryMap.getValue();
        }
        return "";
    }

    /**
     * Get version of the plugin.
     *
     * @return version information String of the plugin. if Version is not
     *         exist, it return "".
     */
    public String getVersion() {
        Iterator<Entry<String, String>> m_iterator = m_attributeMap.entrySet()
                .iterator();

        while (m_iterator.hasNext()) {
            Entry<String, String> entryMap = m_iterator.next();
            if (entryMap.getKey().equals("Version"))
                return entryMap.getValue();
        }
        return "";
    }

    /**
     * Get name of the plugin.
     *
     * @return name String of the plugin. if Name is not exist, it return "".
     */
    public String getName() {
        Iterator<Entry<String, String>> m_iterator = m_attributeMap.entrySet()
                .iterator();

        while (m_iterator.hasNext()) {
            Entry<String, String> entryMap = m_iterator.next();
            if (entryMap.getKey().equals("Name"))
                return entryMap.getValue();
        }
        return "";
    }

    /**
     * Get provider name of the plugin.
     *
     * @return provider name of the plugin. if ProviderName is not exist, it
     *         return "".
     */
    public String getProviderName() {
        Iterator<Entry<String, String>> m_iterator = m_attributeMap.entrySet()
                .iterator();

        while (m_iterator.hasNext()) {
            Entry<String, String> entryMap = m_iterator.next();
            if (entryMap.getKey().equals("Provider"))
                return entryMap.getValue();
        }
        return "";
    }

    /**
     * Get value for the attributes.
     *
     * @param attriubute
     *            String to get value.
     * @return value String of the attribute. if "attribute" is not exist, it
     *         return "".
     */
    public String getValueByAttribute(String attribute) {
        Iterator<Entry<String, String>> m_iterator = m_attributeMap.entrySet()
                .iterator();

        while (m_iterator.hasNext()) {
            Entry<String, String> entryMap = m_iterator.next();
            if (entryMap.getKey().equals(attribute))
                return entryMap.getValue();
        }
        return "";
    }

    /**
     * Set key, value. key is attribute name.
     *
     * @param key
     *            is atrribute name.
     * @param value
     *            for the attribute.
     * @return void.
     */
    public void setValue(String key, String value) {
        m_attributeMap.put(key, value);
    }
}
