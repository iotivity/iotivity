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

package oic.ctt.ui.util;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

import com.fasterxml.jackson.core.JsonFactory;
import com.fasterxml.jackson.core.JsonGenerator;
import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.core.util.DefaultPrettyPrinter;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.node.ArrayNode;
import com.fasterxml.jackson.databind.node.ObjectNode;

import org.slf4j.Logger;

import oic.ctt.ui.util.CTLogger;

/**
 * ResourceProvider is a class for parse the DUTDescriptor.json and process that
 * json file to usable format for CTT GUI
 */
public class ResourceProvider {
    private static final String        RESOURCE_NOT_EXIST_MESSAGE = "Resource doesn't exist";
    private static ResourceDescription m_ResourceDescriptor;
    private static ArrayList<String>   m_ResourceList;

    private static ObjectMapper        m_Mapper                   = new ObjectMapper();
    private static JsonNode            m_RootNode                 = null;
    private static String              m_JsonPath                 = "";
    private static String              m_SelectedResource         = "";
    private static String              m_CommonMsg                = "";
    private static final String        DUPLICATE_RESOURCE_MESSAGE = "Resource Already Exist..!!!";
    private Logger                     logger                     = CTLogger
                                                                          .getInstance();

    /**
     * ResourceProvider class constructor without parameter. It takes default
     * path as DUTDescriptor.json
     */
    public ResourceProvider() {
        m_ResourceDescriptor = new ResourceDescription();
        m_Mapper = new ObjectMapper();
        initJsonFile(DUTDescriptorUtil.DEFAULT_DUT_DESCRIPTOR_PATH);
    }

    /**
     * ResourceProvider class constructor with parameter.
     *
     * @param jsonPath
     *            Path of the DUTDescriptor Json
     * 
     */
    public ResourceProvider(String jsonPath) {
        m_ResourceDescriptor = new ResourceDescription();
        m_Mapper = new ObjectMapper();

        if (jsonPath.length() == 0) {
            m_CommonMsg = "File path is empty.";
            logger.error(m_CommonMsg);

            return;
        } else if (jsonPath.contains(".json") == false) {
            m_CommonMsg = "Make sure about valid json file path.";
            logger.error(m_CommonMsg);

            return;
        }

        initJsonFile(jsonPath);
    }

    // Json initialization method. It takes the provided json path.
    private void initJsonFile(String jsonPath) {
        logger.info("Resource provider class is initilizing with json, path is "
                + jsonPath);

        m_CommonMsg = "";

        m_JsonPath = jsonPath;

        try {
            m_RootNode = m_Mapper.readTree(new File(jsonPath));
        } catch (JsonProcessingException e) {
            m_CommonMsg = "Can't process json.";
            logger.error(m_CommonMsg);

            return;
        } catch (IOException e) {
            m_CommonMsg = "File I/O Error.";
            logger.error(m_CommonMsg);

            return;
        }

        logger.info("Initilize success with the json file.");
    }

    /**
     * Get Device ID from Json file
     *
     * Get di from the DUTDescriptor.json
     * 
     * @return If all precondition filled properly, then the di is returned as
     *         String
     *
     */
    public String getDI() {
        if (m_CommonMsg.length() == 0)
            return m_RootNode.get(DUTDescriptorUtil.DI).asText();
        else {
            logger.warn("Has previous error: " + m_CommonMsg);
            return "";
        }
    }

    /**
     * Resource list Get method from json
     *
     * @return Resource List from Json
     *
     */
    public ArrayList<String> getResourceList() {
        m_ResourceList = new ArrayList<String>();

        if (m_CommonMsg.length() == 0) {
            JsonNode resNodes = m_RootNode.get(DUTDescriptorUtil.OC);

            for (JsonNode ocNode : resNodes) {
                m_ResourceList.add(ocNode.get(DUTDescriptorUtil.URI).asText());
            }
        }

        if (m_ResourceList == null) {
            m_CommonMsg = "Resource list is null.";
        } else if (m_ResourceList.size() == 0) {
            m_CommonMsg = m_CommonMsg + DUTDescriptorUtil.RESOURCE_ERROR;
        }

        if (!m_CommonMsg.isEmpty())
            logger.warn(m_CommonMsg);

        return m_ResourceList;
    }

    /**
     * Resource description Get method from json
     *
     * @param resUri
     *            Resource URI
     * @return Resource description
     *
     */
    public ResourceDescription getResourceDescription(String resUri) {
        if (m_CommonMsg.length() == 0)
            setResDescription(resUri);

        if (resUri.length() == 0)
            m_CommonMsg = m_CommonMsg + DUTDescriptorUtil.EMPTY_STRING;

        logger.info("Get description for resource: " + resUri);

        return m_ResourceDescriptor;
    }

    // Checking method for Existing Resource and New Resource
    private Boolean checkExistingResourceWithNewResource(String oldResUri,
            String newResUri) {
        List<String> resourceList = getResourceList();

        for (String resource : resourceList) {
            if (oldResUri.compareTo(resource) == 0)
                continue;

            if (newResUri.compareTo(resource) == 0)
                return true;
        }

        return false;
    }

    // Resource Description update method. It will receive resource description
    // and update json nodes
    private Boolean updateResourceDescription(
            ResourceDescription resourceDescription) {
        logger.info("updateResourceDescription called with Selected Resource: "
                + m_SelectedResource + "\n");

        Boolean isResFound = false;

        JsonNode resNodes = m_RootNode.get(DUTDescriptorUtil.OC);

        for (JsonNode ocNode : resNodes) {
            if (ocNode.get(DUTDescriptorUtil.URI).asText()
                    .equals(m_SelectedResource)) {
                insertResourceDescriptionValue(resourceDescription, ocNode);
                isResFound = true;
            }
        }

        return isResFound;
    }

    // Insert method of json description. It put the all values to the json node
    // and wait for updating.
    private void insertResourceDescriptionValue(
            ResourceDescription resourceDescription, JsonNode ocNode) {
        ((ObjectNode) ocNode).put(DUTDescriptorUtil.URI,
                resourceDescription.getHref());
        ((ObjectNode) ocNode).put(DUTDescriptorUtil.RT,
                resourceDescription.getRt());
        ((ObjectNode) ocNode).put(DUTDescriptorUtil.DIS,
                resourceDescription.getDis());
        ((ObjectNode) ocNode).put(DUTDescriptorUtil.OBS,
                resourceDescription.getObs());

        ArrayNode arrNode = m_Mapper.valueToTree(resourceDescription
                .getIfList());
        ((ObjectNode) ocNode).putArray(DUTDescriptorUtil.IF).addAll(arrNode);

        arrNode = m_Mapper.valueToTree(resourceDescription.getCcCsRUcUpD());
        ((ObjectNode) ocNode).putArray(DUTDescriptorUtil.CCCSRUCUPD).addAll(
                arrNode);

        arrNode = m_Mapper.valueToTree(resourceDescription.getRep());

        ((ObjectNode) ocNode).putArray(DUTDescriptorUtil.REP).addAll(arrNode);

        JsonNode repNodes = ocNode.path(DUTDescriptorUtil.REP);

        for (JsonNode repNode : repNodes) {
            if (repNode.get(DUTDescriptorUtil.TYPE).asText().toLowerCase()
                    .contains("int")) {
                ((ObjectNode) repNode).put(DUTDescriptorUtil.TESTDATA, repNode
                        .get(DUTDescriptorUtil.TESTDATA).asInt());
                ((ObjectNode) repNode).put(DUTDescriptorUtil.TESTDATA_2, repNode
                        .get(DUTDescriptorUtil.TESTDATA_2).asInt());
            } else if (repNode.get(DUTDescriptorUtil.TYPE).asText()
                    .toLowerCase().contains("double")
                    || repNode.get(DUTDescriptorUtil.TYPE).asText()
                            .toLowerCase().contains("float")) {
                ((ObjectNode) repNode).put(DUTDescriptorUtil.TESTDATA, repNode
                        .get(DUTDescriptorUtil.TESTDATA).asDouble());
                ((ObjectNode) repNode).put(DUTDescriptorUtil.TESTDATA_2, repNode
                        .get(DUTDescriptorUtil.TESTDATA_2).asDouble());
            } else if (repNode.get(DUTDescriptorUtil.TYPE).asText().toLowerCase()
                    .contains("bool")) {
                ((ObjectNode) repNode).put(DUTDescriptorUtil.TESTDATA, repNode
                        .get(DUTDescriptorUtil.TESTDATA).asBoolean());
                ((ObjectNode) repNode).put(DUTDescriptorUtil.TESTDATA_2, repNode
                        .get(DUTDescriptorUtil.TESTDATA_2).asBoolean());
            }
        }

        ((ObjectNode) ocNode).put(DUTDescriptorUtil.PAYLOAD_TYPE,
                resourceDescription.getPayloadType());
        ((ObjectNode) ocNode).put(DUTDescriptorUtil.DUT_TYPE,
                resourceDescription.getDutType());
    }

    /**
     * New Resource Add method. It add new resource to the existing json file.
     *
     * @param resourceDescription
     *            Resource Description for new resource
     *
     * @return Status for saving as String
     *
     */
    public String addNewResource(ResourceDescription resourceDescription) {
        if (ResourceDescription.errorMessage.length() != 0) {
            return ResourceDescription.errorMessage.toString();
        }

        if (checkExistingResourceWithNewResource("",
                resourceDescription.getHref())) {
            return DUPLICATE_RESOURCE_MESSAGE;
        }

        ObjectNode newNode = null;

        try {
            newNode = m_Mapper.valueToTree(resourceDescription);
        } catch (Exception e) {
            m_CommonMsg = "Can't Add New Resource because of Json File Error.\n";

            logger.error(m_CommonMsg + "Error: " + e.getLocalizedMessage());

            return m_CommonMsg;
        }

        Iterator<String> fieldNames = newNode.fieldNames();

        while (fieldNames.hasNext()) {
            String fieldName = fieldNames.next();

            if (fieldName.equals("ifList")) {
                newNode.set(DUTDescriptorUtil.IF, newNode.get("ifList"));
                newNode.remove("ifList");

                fieldNames = newNode.fieldNames();
            } else if (fieldName.equals("ccCsRUcUpD")) {
                newNode.set(DUTDescriptorUtil.CCCSRUCUPD,
                        newNode.get("ccCsRUcUpD"));
                newNode.remove("ccCsRUcUpD");

                break;
            }
        }

        ObjectNode objectNode = m_Mapper.createObjectNode();
        ArrayNode arrayNode = objectNode.putArray(DUTDescriptorUtil.OC);

        arrayNode.addObject().setAll(newNode);

        JsonNode ocNode = m_RootNode.get(DUTDescriptorUtil.OC);

        for (JsonNode jsonNode : ocNode) {
            arrayNode.add(jsonNode);
        }

        ((ObjectNode) m_RootNode).putArray(DUTDescriptorUtil.OC).addAll(
                arrayNode);

        return saveJsonWithStyle();
    }

    /**
     * Resource description update method. It update the existing resource.
     *
     * @param resourceDescription
     *            Resource Description for updated resource
     * @param resourceUri
     *            Resource URI of that resource
     *
     * @return Status for saving as String
     *
     */
    public String saveResourceDescription(
            ResourceDescription resourceDescription, String resourceUri) {

        if (ResourceDescription.errorMessage.length() != 0) {
            return ResourceDescription.errorMessage.toString();
        }

        if (checkExistingResourceWithNewResource(resourceUri,
                resourceDescription.getHref())) {
            return DUPLICATE_RESOURCE_MESSAGE;
        }

        m_CommonMsg = "";
        m_SelectedResource = resourceUri;

        if (updateResourceDescription(resourceDescription) == false) {
            return RESOURCE_NOT_EXIST_MESSAGE;
        }

        return saveJsonWithStyle();
    }

    // Saving the resource content with json formatter
    private String saveJsonWithStyle() {
        JsonGenerator jsonGen = null;

        try {
            jsonGen = new JsonFactory().createGenerator(new FileOutputStream(
                    m_JsonPath));
        } catch (IOException e) {
            m_CommonMsg = m_CommonMsg + "Json File Path Error.\n";
        }

        if (!m_CommonMsg.isEmpty()) {
            logger.warn(m_CommonMsg);
            return m_CommonMsg;
        }

        jsonGen.setPrettyPrinter(new DefaultPrettyPrinter());

        try {
            m_Mapper.writeValue(jsonGen, m_RootNode);
        } catch (Exception e) {
            m_CommonMsg = m_CommonMsg + "Json Processing Error.\n";
        }

        if (!m_CommonMsg.isEmpty()) {
            return m_CommonMsg;
        }

        logger.info(m_CommonMsg);

        return DUTDescriptorUtil.SUCCESS;
    }

    // Resource description element Set method. It prepare the resource
    // description from json.
    private Boolean setResDescription(String resourceUri) {
        logger.info("Description class is set with corresponding values for URI: "
                + resourceUri);

        Boolean isResFound = false;

        JsonNode resNodes = m_RootNode.get(DUTDescriptorUtil.OC);

        for (JsonNode jsonNode : resNodes) {
            if (jsonNode.get(DUTDescriptorUtil.URI).asText()
                    .equals(resourceUri)) {
                isResFound = true;

                m_SelectedResource = resourceUri;

                // Set Resource
                m_ResourceDescriptor.setHref(resourceUri);

                // Set Resource Type
                m_ResourceDescriptor.setRt(jsonNode.get(DUTDescriptorUtil.RT)
                        .asText());

                // Set Discoverable
                m_ResourceDescriptor.setDis(Integer.parseInt(jsonNode.get(
                        DUTDescriptorUtil.DIS).asText()));

                // Set Observable
                m_ResourceDescriptor.setObs(Integer.parseInt(jsonNode.get(
                        DUTDescriptorUtil.OBS).asText()));

                // Setting Resource Interface
                JsonNode ifNodes = jsonNode.path(DUTDescriptorUtil.IF);
                ArrayList<String> list = new ArrayList<String>();
                for (JsonNode ifNode : ifNodes) {
                    list.add(ifNode.asText());
                }

                m_ResourceDescriptor.setIf(list);

                // Set Resource Actions
                JsonNode ccNodes = jsonNode.path(DUTDescriptorUtil.CCCSRUCUPD);
                ArrayList<Boolean> ccList = new ArrayList<Boolean>();
                for (JsonNode ccNode : ccNodes) {
                    ccList.add(ccNode.asBoolean());
                }

                m_ResourceDescriptor.setCcCsRUcUpD(ccList);

                // Set Resource Representation
                JsonNode repNodes = jsonNode.path(DUTDescriptorUtil.REP);
                ArrayList<Map<String, String>> repMapList = new ArrayList<Map<String, String>>();

                for (JsonNode repNode : repNodes) {
                    Map<String, String> jsonMap = new LinkedHashMap<String, String>();

                    jsonMap.put(DUTDescriptorUtil.KEY,
                            repNode.get(DUTDescriptorUtil.KEY).asText());
                    jsonMap.put(DUTDescriptorUtil.TYPE,
                            repNode.get(DUTDescriptorUtil.TYPE).asText());
                    jsonMap.put(DUTDescriptorUtil.TESTDATA,
                            repNode.get(DUTDescriptorUtil.TESTDATA).asText());
                    jsonMap.put(DUTDescriptorUtil.TESTDATA_2,
                            repNode.get(DUTDescriptorUtil.TESTDATA_2).asText());
                    jsonMap.put(DUTDescriptorUtil.ACCESSMODE,
                            repNode.get(DUTDescriptorUtil.ACCESSMODE).asText());

                    repMapList.add(jsonMap);
                }

                m_ResourceDescriptor.setRep(repMapList);

                repMapList.clear();

                // Set Payload Type
                m_ResourceDescriptor.setPayloadType(jsonNode.get(
                        DUTDescriptorUtil.PAYLOAD_TYPE).asText());

                // Set DUT Type
                m_ResourceDescriptor.setDutType(jsonNode.get(
                        DUTDescriptorUtil.DUT_TYPE).asText());
            }
        }

        return isResFound;
    }
}
