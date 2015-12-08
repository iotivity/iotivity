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
package oic.ctt.testhelper;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.List;
import java.util.Map;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;

import org.apache.commons.io.FileUtils;
import org.w3c.dom.Attr;
import org.w3c.dom.Document;
import org.w3c.dom.Element;

public class RMReportManager {

    private static boolean            isFirstTime    = true;
    private String                    testsuiteName;
    private String                    status;
    private String                    time;
    private String                    failure;
    private String                    spec;
    private Map<String, List<String>> testcase;
    private final static String[]     attributeName  = { "name", "status",
            "time", "failure", "spec"               };
    private String[]                  attributeValue = { this.testsuiteName,
            this.status, this.time, this.failure, this.spec };

    public RMReportManager() {

    }

    public void createTestReport(Map<String, List<String>> testdata,
            String testsuiteName, String packageName) {
        this.testsuiteName = testsuiteName;
        this.testcase = testdata;
        String dirName = "TestReport";
        try {

            DocumentBuilderFactory docFactory = DocumentBuilderFactory
                    .newInstance();
            DocumentBuilder docBuilder = docFactory.newDocumentBuilder();

            // testsuite element
            Document doc = docBuilder.newDocument();
            Element testsuite = doc.createElement("testsuite");
            doc.appendChild(testsuite);

            // set attribute to testsuite element
            Attr suiteName = doc.createAttribute("name");
            suiteName.setValue(testsuiteName);
            testsuite.setAttributeNode(suiteName);

            for (String key : testcase.keySet()) {
                List<String> values = testcase.get(key);
                Element testCase = doc.createElement("testcase");
                testsuite.appendChild(testCase);
                for (int i = 0; i < values.size(); i++) {
                    this.attributeValue[i] = (values.get(i));
                    if (RMReportManager.attributeName[i] != "failure") {
                        Attr testCaseAttribute = doc
                                .createAttribute(RMReportManager.attributeName[i]);
                        testCaseAttribute.setValue(values.get(i));
                        testCase.setAttributeNode(testCaseAttribute);
                    }
                }
                if (this.attributeValue[3] != null) {
                    Element failure = doc.createElement("failure");
                    failure.setTextContent(this.attributeValue[3]);
                    testCase.appendChild(failure);
                    // Attr message = doc.createAttribute("message");
                    // message.setValue(this.attributeValue[3]);
                    // failure.setAttributeNode(message);
                }
            }
            TransformerFactory transformerFactory = TransformerFactory
                    .newInstance();
            Transformer transformer = transformerFactory.newTransformer();
            DOMSource source = new DOMSource(doc);
            try {
                createDirectory(dirName);
            } catch (IOException ioex) {
                ioex.printStackTrace();
            }
            StreamResult result = new StreamResult(new File(dirName + "/"
                    + attributeValue[0] + ".xml"));

            // Output to console for testing
            // StreamResult result = new StreamResult(System.out);

            transformer.transform(source, result);

            System.out.println("XML report of " + attributeValue[0]
                    + " created successfully");

        } catch (ParserConfigurationException pce) {
            pce.printStackTrace();
        } catch (TransformerException tfe) {
            tfe.printStackTrace();
        }
    }

    public void writeLog(String text, String packageName) {
        File logFile = new File("/data/data/" + packageName + "/testlog.log");
        if (!logFile.exists()) {
            try {
                logFile.createNewFile();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        try (BufferedWriter buf = new BufferedWriter(new FileWriter(logFile,
                true))) {
            buf.append(text);
            buf.newLine();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private boolean createDirectory(final String dirName) throws IOException {

        File resultDirectory = new File(dirName);
        if (!resultDirectory.exists()) {
            try {
                if (resultDirectory.mkdir()) {
                    System.out.println("Result directory: " + dirName
                            + " created");
                    return true;
                }
            } catch (SecurityException se) {
                se.printStackTrace();
            }

        } else if (isFirstTime) {
            FileUtils.cleanDirectory(resultDirectory);
            isFirstTime = false;
        }
        return false;
    }
}