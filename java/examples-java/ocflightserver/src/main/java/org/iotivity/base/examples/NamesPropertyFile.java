/*
 *******************************************************************
 *
 * Copyright 2018 Intel Corporation.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

package org.iotivity.base.examples;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Properties;

/**
 * This class encapsulates the names property file. The names property file is
 * used for obtaining the uuid for previously used light names.
 */
public class NamesPropertyFile {

    static private Properties namesProperties;
    static private File namesPropertyFile;

    static private NamesPropertyFile instance;

    static {
        try {
            namesProperties = new Properties();
            namesPropertyFile = new File("names.prop");
            instance = new NamesPropertyFile();

        } catch (Exception e) {
            OcfLightServer.msgError("Error creating names property file instance.");
        }
    }

    private NamesPropertyFile() {
        // read the names property file
        FileInputStream inStream = null;
        try {
            if (namesPropertyFile.exists() && namesPropertyFile.length() > 0) {
                inStream = new FileInputStream(namesPropertyFile);
                namesProperties.loadFromXML(inStream);
                namesProperties.list(System.out);
            }

        } catch (IOException e) {
            OcfLightServer.msgError("Error loading name properties: " + e.toString());
            e.printStackTrace();

        } finally {
            if (inStream != null) {
                try {
                    inStream.close();
                } catch (IOException e) {
                    OcfLightServer.msgError("Error closing name properties file: " + e.toString());
                    e.printStackTrace();
                }
            }
        }
    }

    public static NamesPropertyFile getInstance() {
        return instance;
    }

    public boolean hasName(String name) {
        return namesProperties.containsKey(name);
    }

    public String getUuidForName(String name) {
        return (String) namesProperties.get(name);
    }

    public void updateNamesProperty(String name, String uuid) {
        // update names property file
        namesProperties.put(name, uuid);
        FileOutputStream outStream = null;
        try {
            outStream = new FileOutputStream(namesPropertyFile);
            namesProperties.storeToXML(outStream, null);

        } catch (IOException e) {
            OcfLightServer.msgError("Error storing name properties: " + e.toString());
            e.printStackTrace();
        }

        finally {
            if (outStream != null) {
                try {
                    outStream.close();
                } catch (IOException e) {
                    OcfLightServer.msgError("Error closing name properties file: " + e.toString());
                    e.printStackTrace();
                }
            }
        }
    }
}
