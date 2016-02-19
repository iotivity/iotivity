/*
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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
 */

package oic.simulator.clientcontroller.remoteresource;

import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;
import java.util.List;

public class SerializedClientController implements Serializable {

    public List<String> getResourceTypes() {
        return m_resourceTypes;
    }

    public void setResourceTypes(List<String> resourceTypes) {
        this.m_resourceTypes = resourceTypes;
    }

    public List<String> getFavorites() {
        return m_favorites;
    }

    public void setFavorites(List<String> favorites) {
        this.m_favorites = favorites;
    }

    public void serialize(String filePath) throws Exception {
        try {
            FileOutputStream fileOut = new FileOutputStream(filePath);
            ObjectOutputStream out = new ObjectOutputStream(fileOut);
            out.writeObject(this);
            out.close();
            fileOut.close();
        } catch (IOException i) {
            throw new Exception("Failed to Serialize data : IOException");
        }
    }

    public static SerializedClientController deSerialize(String filePath)
            throws Exception {
        SerializedClientController r = null;
        try {
            FileInputStream fileIn = new FileInputStream(filePath);
            ObjectInputStream in = new ObjectInputStream(fileIn);
            r = (SerializedClientController) in.readObject();
            in.close();
            fileIn.close();
        } catch (IOException i) {
            throw new Exception("Failed to DeSerialize data : IOException");
        } catch (ClassNotFoundException c) {
            throw new Exception("Failed to DeSerialize data : ClassNotFound");
        }
        return r;
    }

    private List<String> m_resourceTypes;
    private List<String> m_favorites;

}