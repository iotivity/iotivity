// ******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
//
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

package oic.ctt;

import java.util.ArrayList;
import java.util.HashMap;

import org.slf4j.Logger;

import oic.ctt.DutKey;

import oic.ctt.formatter.JsonAnalyzer;

/**
 * This Class contains Information of each DUT in key value pairs
 */

public class DUTResource {

    /**
     * Enumeration for CRUDN operations
     */
    public enum CRUDNType {
        COMPLETE_CREATE("Cc"), SUBORDINATE_CREATE("Cs"), RETRIEVE(
        "R"), COMPLETE_UPDATE("Uc"), PARTIAL_UPDATE("Up"), DELETE("D");

        private String key;

        private CRUDNType(String key) {
            this.key = key;
        }

        @Override
        public String toString() {
            return key;
        }
    }

    public enum PayloadType {
        OBJECT("object"), ARRAY("array");

        private String key;

        private PayloadType(String key) {
            this.key = key;
        }

        @Override
        public String toString() {
            return key;
        }

        public static PayloadType getPayloadTypeByValue(String type) {
            for (int i = 0; i < PayloadType.values().length; i++) {
                if (type.equals(PayloadType.values()[i].key))
                    return PayloadType.valueOf(PayloadType.values()[i].name());
            }
            return null;
        }
    }

    private HashMap<String, ArrayList<String>> mDutResourceValue;
    private HashMap<String, Boolean>           mCRUDNState = new HashMap<String, Boolean>();

    /**
     * Constructor for DUTResource. It creates DUT Resource Instance according
     * to the JSON string provided to it
     *
     * @param jsonString
     *            : JSON String to create DUTResource instance
     */
    public DUTResource(String jsonString) {

        if (mDutResourceValue == null) {
            mDutResourceValue = new HashMap<String, ArrayList<String>>();
        }
        JsonAnalyzer jsonAnaylyzer = new JsonAnalyzer(jsonString);
        DutKey[] dutKeys = DutKey.values();

        for (int i = 0; i < dutKeys.length; i++) {
            String key = dutKeys[i].toString();
            ArrayList<String> valueList = new ArrayList<String>();

            if (jsonAnaylyzer.getValue(key) != null) {
                for (String value : jsonAnaylyzer.getValue(key)) {
                    valueList.add(value);
                }
            }
            this.setDUTResourceValue(key, valueList);
        }
    }

    /**
     * This Method sets the Key and Value to the DUT Resource Instance
     *
     * @param key
     *            : Key for the Value
     * @param value
     *            : Value for the corresponding key
     */
    public void setDUTResourceValue(String key, ArrayList<String> value) {
        mDutResourceValue.put(key, value);
    }

    /**
     * This Method returns corresponding Value list of the Key provided
     *
     * @param key
     *            : Corresponding Key for the desired Value list
     * @return ArrayList<String> : Corresponding Value list of the key
     */
    public ArrayList<String> getDUTResourceValue(String key) {
        return mDutResourceValue.get(key);
    }

    /**
     * This method generate the CRUDN states supported by the resource
     */
    public void generateCRUDNState() {
        String[] crudn = DutKey.CRUDN.toString().split("(?=[A-Z])");

        ArrayList<String> crudnState = getDUTResourceValue(
                DutKey.CRUDN.toString());
        int count = 0;
        for (String string : crudnState) {
            Boolean bool = Boolean.parseBoolean(string);
            mCRUDNState.put(crudn[count++], bool);
        }
    }

    /**
     * This method checks if a CRUDN operation is supported by the resource
     *
     * @param key : the CRUDN operation type to check
     * @return true : if the key is supported false otherwise
     */
    public boolean getCRUDNState(CRUDNType key) {
        return mCRUDNState.get(key.toString());
    }

    /**
     * This method returns the object type of the whole payload of a resource
     *
     * @return PayloadType : Desired object type of the response payload
     */
    public PayloadType getResourcePayloadType() {
        return PayloadType.getPayloadTypeByValue(
                mDutResourceValue.get(DutKey.OBJ_TYPE.toString()).get(0));
    }
}
