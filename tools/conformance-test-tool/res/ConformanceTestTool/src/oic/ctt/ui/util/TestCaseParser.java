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

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.Reader;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.LinkedHashMap;
import java.util.StringTokenizer;

import static oic.ctt.ui.util.PopUpUtil.*;

public class TestCaseParser {
    private static final String                                   PARSING_ERROR_MSG       = "Fail to parsing Test Case.\nTest suite : ";
    private static final String                                   TEST_CASE_PARSING_ERROR = "Test Case Parsing Error";
    private static final String                                   OTHER_MATCH_STR         = "^\\s{0,}(\\.\\.\\.)(?!\\.).{0,}";
    private static final String                                   SPEC_MATCH_STR          = "^\\s{0,}(\\.\\.\\.)(?!\\.).*";
    private static final String                                   TAGS_MATCH_STR          = "(^\\t|^\\s{2,})(\\[Tags\\]).*";
    private static final String                                   ENCODING_TYPE           = "UTF-8";
    private static final String                                   STR_TC_FULL_NAME        = "TC FULL NAME";
    private static final String                                   STR_TC_ID               = "TC ID";
    private static final String                                   STR_TC_NAME             = "TC NAME";
    private static final String                                   STR_TEST_SPEC           = "test spec";
    private static final String                                   PARSE_MATCH_STR         = "^(?!\\s*\\#) {0,1}\\S.*";
    private static final String                                   DOC_MATCH_STR           = "(^\\t|^\\s{2,})(\\[Documentation\\]).*";
    private static final String                                   MAIN_NOT_MATCH_STR      = "^ {0,1}(\\*){1,}.*";
    private static final String                                   MAIN_MATCH_STR          = "(?i)^ {0,1}(\\*){1,} {0,1}t {0,1}e {0,1}s {0,1}t {0,1}c {0,1}a {0,1}s {0,1}e {0,1}s{0,1}\\W+";
    private LinkedHashMap<Integer, LinkedHashMap<String, String>> hashMapParent;
    private LinkedHashMap<String, String>                         hashMapChild;
    private String                                                str;
    private int                                                   parentIndex;

    public TestCaseParser() {
    }

    public LinkedHashMap<Integer, LinkedHashMap<String, String>> getDocumentHashMap(
            String filePathString) {
        try {
            return setDocumentHashMap(Paths.get(filePathString));
        } catch (IOException e) {
            e.printStackTrace();
            return null;
        }
    }

    public LinkedHashMap<Integer, LinkedHashMap<String, String>> getDocumentHashMap(
            Path filePath) {
        try {
            return setDocumentHashMap(filePath);
        } catch (IOException e) {
            e.printStackTrace();
            return null;
        }
    }

    private LinkedHashMap<Integer, LinkedHashMap<String, String>> setDocumentHashMap(
            Path filePath) throws IOException {
        str = null;
        boolean parseFlag = false;
        boolean documentationFlag = false;
        parentIndex = 0;
        hashMapChild = new LinkedHashMap<>();
        hashMapParent = new LinkedHashMap<Integer, LinkedHashMap<String, String>>();
        Reader fileReader = null;
        try {
            fileReader = new InputStreamReader(new FileInputStream(
                    filePath.toFile()), ENCODING_TYPE);
            BufferedReader bufferedReader = new BufferedReader(fileReader);

            while ((str = bufferedReader.readLine()) != null) {
                if (str.matches(MAIN_MATCH_STR)) {
                    parseFlag = true;
                    continue;
                } else if (str.matches(MAIN_NOT_MATCH_STR)) {
                    parseFlag = false;
                }

                if (parseFlag) {
                    if (str.matches(PARSE_MATCH_STR) && !documentationFlag) {
                        parseDocument();
                    } else if (str.matches(DOC_MATCH_STR)) {
                        documentationFlag = true;
                    } else if ((documentationFlag == true)
                            && (str.matches(SPEC_MATCH_STR))
                            && !(str.contains("==") && str.toLowerCase()
                                    .contains(STR_TEST_SPEC))) {
                        putTokenizeValueToChildMap();
                    } else if (str.matches(TAGS_MATCH_STR)) {
                        splitMainString();
                    } else if ((documentationFlag == true)
                            && !(str.matches(OTHER_MATCH_STR))) {
                        documentationFlag = false;
                    }
                }
            }
            bufferedReader.close();
        } catch (IOException e) {
            displayErrorDialog(e, TEST_CASE_PARSING_ERROR, PARSING_ERROR_MSG
                    + filePath.getFileName() + "\nTest case : " + str);
        }
        if (fileReader != null) {
            fileReader.close();
        }

        return hashMapParent;
    }

    /**
     * This method is to Parse the Document
     */
    private void parseDocument() {
        str = str.split("(\\t|\\s{2,})#.*$")[0];
        str = str.trim();
        hashMapChild = new LinkedHashMap<>();
        hashMapParent.put(parentIndex++, hashMapChild);
        hashMapChild.put(STR_TC_FULL_NAME, str);
        if (str.contains("]")) {
            StringTokenizer strTokenizer = new StringTokenizer(str.trim(), "]");
            hashMapChild.put(STR_TC_ID, strTokenizer.nextToken().trim()
                    .substring(1));
            hashMapChild.put(STR_TC_NAME, strTokenizer.nextToken().trim());
        } else {
            hashMapChild.put(STR_TC_NAME, str);
        }
    }

    /**
     * This method is for split and put the value into the Child hash map.
     */
    private void splitMainString() {
        String[] strArray = str.split("]");
        if (strArray.length < 2) {
            hashMapChild.put(strArray[0].split("\\[")[1].toUpperCase(), null);
        } else {
            hashMapChild.put(strArray[0].split("\\[")[1].toUpperCase(),
                    strArray[1].substring(1));
        }
    }

    /**
     * This method is for Putting token value to the Child Hash Map.
     */
    private void putTokenizeValueToChildMap() {
        StringTokenizer strTokenizer = new StringTokenizer(str, "|");
        int index = 0;
        String[] strArray = new String[strTokenizer.countTokens()];
        while (strTokenizer.hasMoreTokens()) {
            strArray[index++] = strTokenizer.nextToken().trim();
        }

        if (strArray.length < 3) {
            if ((strArray.length > 1)
                    && (!hashMapChild.containsKey(strArray[1].toUpperCase()))) {
                hashMapChild.put(strArray[1].toUpperCase(), null);
            }
        } else {
            if (hashMapChild.containsKey(strArray[1].toUpperCase())) {
                hashMapChild.put(strArray[1].toUpperCase(),
                        hashMapChild.get(strArray[1].toUpperCase()) + "\n"
                                + strArray[2]);
            } else {
                hashMapChild.put(strArray[1].toUpperCase(), strArray[2]);
            }
        }
    }
}
