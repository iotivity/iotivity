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

package oic.ctt.formatter;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import oic.ctt.logger.CTLogger;

import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.json.simple.JSONValue;
import org.slf4j.Logger;

import com.fasterxml.jackson.core.JsonParser;
import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.DeserializationFeature;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.github.fge.jackson.JsonLoader;

/**
 * JsonParser for robotFramework testcase 
 */
public class JsonAnalyzer {

    private static Logger mlogger   = CTLogger.getInstance();
    private String        mJsonString;
    private final String  DELIMITER = " ";
    private final String  REP       = "rep";
    private final String  OC        = "oc";

    public JsonAnalyzer(String jsonString) {
        mJsonString = jsonString;
    }

    public JsonAnalyzer(File jsonFile) throws IOException {
        mJsonString = JsonLoader.fromFile(jsonFile).toString();
    }

    public String getJsonString() {
        return mJsonString;
    }

    public boolean setJsonString(String jsonString) {
        mJsonString = jsonString;
        return validate();
    }

    /**
     * Return value from specific href resource
     */
    public ArrayList<String> getValueWithHref(String href, String key) {

        try {
            String targetResourceJsonString = getSingleResourceJsonNode(href,
                    mJsonString);
            return getValue(key, targetResourceJsonString);
        } catch (NullPointerException e) {
            e.printStackTrace();
            mlogger.error(e.getMessage());

        }
        return null;
    }

    /**
     * Return resource information of specific href
     */
    public String getSingleResourceJsonNode(String href, String jsonString) {
        try {
            ObjectMapper mapper = new ObjectMapper();
            JsonNode jsonNodeTree = mapper.readTree(jsonString);

            for (JsonNode tempJsonNode : jsonNodeTree) {
                String tempJsonString = tempJsonNode.toString();

                if (tempJsonNode.get(IotivityKey.HREF.toString()) != null) {
                    if (tempJsonString.contains(href))
                        return tempJsonString;

                } else {
                    if (tempJsonString.contains(href)
                            && tempJsonString.contains("\""
                                    + IotivityKey.HREF.toString() + "\"")) {
                        return getSingleResourceJsonNode(href, tempJsonString);
                    }
                }
            }
        } catch (IOException e) {

            e.printStackTrace();
            mlogger.error(e.getMessage());
        }

        return null;

    }

    /**
     * Return value of specific key
     */
    public ArrayList<String> getValue(String key) {
        return getValue(key, mJsonString);
    }

    /**
     * Return value of specific key This method have recursive logic
     */
    private ArrayList<String> getValue(String key, String jsonString) {
        ArrayList<String> returnValueList = new ArrayList<String>();

        if (jsonString == null || jsonString.isEmpty()) {
            return returnValueList;

        } else if (jsonString.contains(key) == false) {
            return returnValueList;
        }

        try {
            ObjectMapper mapper = new ObjectMapper();
            JsonNode jsonNodeTree = mapper.readTree(jsonString);
            JsonNode jsonNode = jsonNodeTree.get(key);
            if (jsonNode != null) {
                returnValueList = getValueFromJsonNode(key, jsonNode);
            }

            for (JsonNode tempJsonNode : jsonNodeTree) {
                if (tempJsonNode.toString().contains("\"" + key + "\"")) {
                    return getValue(key, tempJsonNode.toString());
                }
            }

        } catch (IOException e) {
            e.printStackTrace();
            mlogger.error(e.getMessage());
        }

        return returnValueList;
    }

    /**
     * Returns value of JsonNode with handing JsonNode is array and value of
     * asText() is empty string
     */
    private ArrayList<String> getValueFromJsonNode(String key, JsonNode jsonNode) {
        ArrayList<String> returnValueList = new ArrayList<String>();

        if (jsonNode.isArray()) {
            for (JsonNode tempNode : jsonNode) {
                returnValueList.addAll(getJsonValueList(key, tempNode));
            }
        } else {
            returnValueList.addAll(getJsonValueList(key, jsonNode));
        }
        return returnValueList;
    }

    /**
     * Return value that are related key include duplication
     */
    public ArrayList<String> getAllValueFromJson(String key) {
        ArrayList<String> listHREF = new ArrayList<>();

        ObjectMapper mapper = new ObjectMapper();
        JsonNode jsonNodeArray = null;
        try {
            jsonNodeArray = mapper.readTree(mJsonString);

            for (JsonNode jsonNode : jsonNodeArray) {
                if (jsonNode.has(key)) {
                    if (jsonNode.path(key).toString().startsWith("\"")) {
                        String strContainsQuot = jsonNode.path(key).toString();
                        strContainsQuot = strContainsQuot.subSequence(1,
                                strContainsQuot.length() - 1).toString();
                        listHREF.add(strContainsQuot);
                    } else
                        listHREF.add(jsonNode.path(key).toString());

                }
            }
        } catch (JsonProcessingException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }

        return listHREF;
    }

    /**
     * Return JSonObject list is included oic value
     */
    public ArrayList<String> getResourceJson() {
        JSONObject jsnObject = (JSONObject) JSONValue.parse(mJsonString);
        ArrayList<String> resourceRepLst = new ArrayList<>();

        if (jsnObject != null) {
            JSONArray jsnArray = (JSONArray) jsnObject.get(IotivityKey.ROOT
                    .toString());

            for (int i = 0; i < jsnArray.size(); i++) {
                JSONObject obj = (JSONObject) jsnArray.get(i);
                resourceRepLst.add(obj.toString());
            }
        }
        return resourceRepLst;
    }

    /**
     * Return true if jsonString is valid, otherwise false
     */
    public boolean validate() {
        boolean retValue = false;

        try {
            ObjectMapper objectMapper = new ObjectMapper();
            objectMapper
                    .enable(DeserializationFeature.FAIL_ON_READING_DUP_TREE_KEY);
            JsonParser parser = objectMapper.getFactory().createParser(
                    mJsonString);

            while (parser.nextToken() != null) {
            }

            objectMapper.readTree(mJsonString);
            retValue = true;

            return retValue;

        } catch (IOException ioe) {
            ioe.printStackTrace();
            mlogger.error(ioe.getMessage());
            return false;
        }
    }

    /**
     * Return Json Value as String
     */
    public String getJsonValueAsString(String key) {
        ObjectMapper mapper = new ObjectMapper();
        JsonNode jsonNodeArray = null;
        try {
            return mapper.readTree(mJsonString).path(key).toString();
        } catch (JsonProcessingException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return "";
    }

    private ArrayList<String> getJsonValueList(String key, JsonNode jsonNode){
        ArrayList<String> jsonValueList = new ArrayList<>();
        String value = jsonNode.asText();
        if (value.isEmpty()) {
            value = jsonNode.toString();
        }
        // Convert space separated values into array of values
        if (key != REP && key != OC) {
            String[] valList = value.split(DELIMITER);
            for (String val : valList) {
                jsonValueList.add(val);
            }
        }
        else
            jsonValueList.add(value);
        return jsonValueList;
    }
}
