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

package oic.ctt.raml.parser;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Map.Entry;
import java.util.Set;
import java.util.UUID;

import org.raml.model.ActionType;
import org.raml.model.Raml;
import org.raml.parser.visitor.RamlDocumentBuilder;

import com.fasterxml.jackson.core.JsonFactory;
import com.fasterxml.jackson.core.JsonGenerator;
import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.core.util.DefaultPrettyPrinter;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;

public class RAMLParser {

    private static final String DI = "di";
    private static final String OC = "oc";
    private static final String HREF = "href";
    private static final String RT = "rt";
    private static final String IF = "if";
    private static final String DIS = "dis";
    private static final String OBS = "obs";
    private static final String CcCsRUcUpD = "CcCsRUcUpD";
    private static final String REP = "rep";
    private static final String DEFINITIONS = "definitions";
    private static final String PROPERTIES = "properties";
    private static final String ALLOF = "allOf";
    private static final String ANYOF = "anyOf";
    private static final String ONEOF = "oneOf";
    private static final String KEY = "key";
    private static final String TYPE = "type";
    private static final String PAYLOADTYPE = "payloadType";
    private static final String TESTDATA = "testdata";
    private static final String TESTDATA2 = "testdata2";
    private static final String ACCESSMODE = "accessmode";
    private static final String DUTTYPE = "dutType";
    private static final String DUTDESCRIPTOR = "DUTDescriptor.json";


    public static void main(String[] args) throws JsonProcessingException, IOException {
        JsonGenerator jsonGen = new JsonFactory().createGenerator(new FileOutputStream(DUTDESCRIPTOR));
        jsonGen.setPrettyPrinter(new DefaultPrettyPrinter());   // for beautification

        jsonGen.writeStartObject();
        jsonGen.writeStringField(DI, UUID.randomUUID().toString());
        jsonGen.writeArrayFieldStart(OC);

        // for all raml files provided in argument
        for (String resourceLocation : args) {
            String path = new File(resourceLocation).getCanonicalPath();
            path = path.substring(0, path.lastIndexOf('/')+1);
            System.out.println("Parsing file: " + resourceLocation);
            Raml raml = new RamlDocumentBuilder().build(resourceLocation);

            // for each resource in the RAML file
            Set resKey = raml.getResources().keySet();
            for (Object resURI : resKey) {
                jsonGen.writeStartObject();
                jsonGen.writeStringField(HREF, resURI.toString());
                // get 'rt' from GET method 'response' example
                ObjectMapper mapper = new ObjectMapper();
                JsonNode getMethodExample = mapper.readTree(raml.getResource(resURI.toString()).getAction(ActionType.GET).getResponses().get("200").getBody().get("application/json").getExample());
                jsonGen.writeStringField(RT, getMethodExample.get(RT).textValue());

                // 'dis' and 'obs' field not available in raml file
                jsonGen.writeNumberField(DIS, 1);
                jsonGen.writeNumberField(OBS, 1);

                // get 'if' from GET method QueryParameter (inherited from trait)
                List<String> inf = raml.getResource(resURI.toString()).getAction(ActionType.GET).getQueryParameters().get(IF).getEnumeration();
                jsonGen.writeArrayFieldStart(IF);
                for (String string : inf) {
                    jsonGen.writeString(string);
                }
                jsonGen.writeEndArray();

                // 'CcCsRUcUpD' constant string. getting the values for the methods supported by the resource
                ArrayList<Boolean> supportedMethodList = new ArrayList<Boolean>(6);
                for (int i = 0 ; i < 6; i++) {
                    supportedMethodList.add(false);
                }
                for (ActionType action : raml.getResource(resURI.toString()).getActions().keySet()) {
                    if (action.compareTo(ActionType.GET) == 0){
                        supportedMethodList.set(2, true);
                    }
                    if (action.compareTo(ActionType.PUT) == 0){
                        supportedMethodList.set(0, true);
                        supportedMethodList.set(3, true);
                    }
                    if (action.compareTo(ActionType.POST) == 0){
                        supportedMethodList.set(1, true);
                        supportedMethodList.set(4, true);
                    }
                    if (action.compareTo(ActionType.DELETE) == 0){
                        supportedMethodList.set(5, true);
                    }
                }
                jsonGen.writeArrayFieldStart(CcCsRUcUpD);
                for (Boolean boolean1 : supportedMethodList) {
                    jsonGen.writeBoolean(boolean1);
                }
                jsonGen.writeEndArray();

                //Getting the schema for the 'rep' properties
                String schemaName = (String) raml.getSchemas().get(0).keySet().toArray()[0];
                JsonNode schema = mapper.readTree(raml.getSchemas().get(0).get(schemaName));
                JsonNode resDef = schema.get(DEFINITIONS).fields().next().getValue();

                // When properties are in nested referenced files
                Iterator<String> fieldItr = resDef.fieldNames();
                while (fieldItr.hasNext()){
                    String field = fieldItr.next();
                    if (field.equals(ALLOF) || field.equals(ONEOF) || field.equals(ANYOF)){
                        JsonNode allOfkey = resDef.get(field);
                        for (JsonNode refs : allOfkey) {
                            Iterator<String> refKeys = refs.fieldNames();
                            while (refKeys.hasNext()){
                                String refKey = refKeys.next();
                                String refFile = refs.get(refKey).asText();
                                String refRT = refFile.split("#")[1].split("/")[2];
                                refFile = refFile.split("#")[0];
                                JsonNode refJson = mapper.readTree(new File(path+refFile));
                                resDef = refJson.get(DEFINITIONS).get(refRT);
                                break;
                            }
                        }
                    }
                }

                JsonNode properties = resDef.get(PROPERTIES);
                jsonGen.writeArrayFieldStart(REP);

                // When there is no field 'properties' but there is 'media'
                if (properties == null && resDef.get("media") != null){
                    properties = resDef.get("media").get("items").get(PROPERTIES);
                }
                Iterator<Entry<String, JsonNode>> propertyKeysIterator = properties.fields();

                //Getting value for each property from GET response(200) example
                while (propertyKeysIterator.hasNext()){
                    Entry<String, JsonNode> property = propertyKeysIterator.next();
                    String propertyKey = property.getKey();

                    JsonNode typeNode = property.getValue().get(TYPE);
                    JsonNode exampleValue = getMethodExample.get(propertyKey);
                    if (exampleValue != null){
                        jsonGen.writeStartObject();
                        jsonGen.writeStringField(KEY, propertyKey);
                        if (typeNode != null){
                            // If type value is an array
                            String typeValue = typeNode.textValue();
                            if (typeNode.isArray()){
                                jsonGen.writeArrayFieldStart(TYPE);
                                for (JsonNode jsonNode : typeNode) {
                                    mapper.writeTree(jsonGen, jsonNode);
                                }
                                jsonGen.writeEndArray();
                                typeValue = typeNode.get(0).asText();
                            }
                            else
                                jsonGen.writeStringField(TYPE, typeValue);

                            // getting values from GET response(200) example
                            if (typeValue.equals("array")){
                                jsonGen.writeArrayFieldStart(TESTDATA);
                                for (JsonNode jsonNode : exampleValue) {
                                    mapper.writeTree(jsonGen, jsonNode);
                                }
                                jsonGen.writeEndArray();

                                jsonGen.writeArrayFieldStart(TESTDATA2);
                                for (JsonNode jsonNode : exampleValue) {
                                    mapper.writeTree(jsonGen, jsonNode);
                                }
                                jsonGen.writeEndArray();
                            }
                            if (typeValue.equals("object")){
                                jsonGen.writeStringField(TESTDATA, exampleValue.asText());
                                jsonGen.writeStringField(TESTDATA2, exampleValue.asText());
                            }
                            if (typeValue.equals("string")){
                                jsonGen.writeStringField(TESTDATA, exampleValue.asText());
                                jsonGen.writeStringField(TESTDATA2, exampleValue.asText());
                            }
                            if (typeValue.equals("integer")){
                                jsonGen.writeNumberField(TESTDATA, exampleValue.asInt());
                                jsonGen.writeNumberField(TESTDATA2, exampleValue.asInt());
                            }
                            if (typeValue.equals("boolean")){
                                jsonGen.writeBooleanField(TESTDATA, exampleValue.asBoolean());
                                jsonGen.writeBooleanField(TESTDATA2, exampleValue.asBoolean());
                            }
                            if (typeValue.equals("number")){
                                jsonGen.writeNumberField(TESTDATA, exampleValue.asInt());
                                jsonGen.writeNumberField(TESTDATA2, exampleValue.asInt());
                            }
                        }
                        else if (property.getValue().get("enum") != null){
                            jsonGen.writeStringField(TYPE, "enum");
                            if (getMethodExample.get(propertyKey) != null)
                            {
                                jsonGen.writeStringField(TESTDATA, getMethodExample.get(propertyKey).asText());
                                jsonGen.writeStringField(TESTDATA2, getMethodExample.get(propertyKey).asText());
                            }
                            else{
                                jsonGen.writeStringField(TESTDATA, property.getValue().get("enum").get(0).asText());
                                jsonGen.writeStringField(TESTDATA2, property.getValue().get("enum").get(0).asText());
                            }
                        }
                        // accessmode is not available from raml file
                        jsonGen.writeStringField(ACCESSMODE, "R");
                        jsonGen.writeEndObject();
                    }
                    else {
                        // When property value is not in GET response(200) example
                        //TODO: Check if property is required
                    }
                }

                jsonGen.writeEndArray();
                jsonGen.writeStringField(PAYLOADTYPE, schema.get(TYPE).asText());
                jsonGen.writeStringField(DUTTYPE, "SERVER"); // not available in raml file
                jsonGen.writeEndObject();
            }
        }
        jsonGen.writeEndArray();
        jsonGen.writeEndObject();
        jsonGen.flush();
        jsonGen.close();
        System.out.println("Completed");
    }

}
