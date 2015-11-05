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

import oic.ctt.ui.UIConst;

import org.eclipse.core.runtime.MultiStatus;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.swt.widgets.Display;

public class TestCaseParser {

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
        String str = null;
        boolean parseFlag = false;
        boolean documentationFlag = false;
        int parentIndex = 0;
        LinkedHashMap<String, String> hashMapChild = new LinkedHashMap<>();
        LinkedHashMap<Integer, LinkedHashMap<String, String>> hashMapParent = new LinkedHashMap<Integer, LinkedHashMap<String, String>>();
        Reader fileReader = null;
        try {
            fileReader = new InputStreamReader(new FileInputStream(
                    filePath.toFile()), "UTF-8");
            BufferedReader bufferedReader = new BufferedReader(fileReader);

            while ((str = bufferedReader.readLine()) != null) {
                if (str.matches("(?i)^ {0,1}(\\*){1,} {0,1}t {0,1}e {0,1}s {0,1}t {0,1}c {0,1}a {0,1}s {0,1}e {0,1}s{0,1}\\W+")) {
                    parseFlag = true;
                    continue;
                } else if (str.matches("^ {0,1}(\\*){1,}.*")) {
                    parseFlag = false;
                }

                if (parseFlag) {
                    if (str.matches("^(?!\\s*\\#) {0,1}\\S.*")
                            && !documentationFlag) {
                        str = str.split("(\\t|\\s{2,})#.*$")[0]; 
                        str = str.trim();
                        hashMapChild = new LinkedHashMap<>();
                        hashMapParent.put(parentIndex++, hashMapChild);
                        hashMapChild.put("TC FULL NAME", str);
                        if (str.contains("]")) {
                            StringTokenizer strTokenizer = new StringTokenizer(
                                    str.trim(), "]");
                            hashMapChild.put("TC ID", strTokenizer.nextToken()
                                    .trim().substring(1));
                            hashMapChild.put("TC NAME", strTokenizer
                                    .nextToken().trim());
                        } else {
                            hashMapChild.put("TC NAME", str);
                        }

                    } else if (str
                            .matches("(^\\t|^\\s{2,})(\\[Documentation\\]).*")) {
                        documentationFlag = true;
                    } else if ((documentationFlag == true)
                            && (str.matches("^\\s{0,}(\\.\\.\\.)(?!\\.).*"))
                            && !(str.contains("==") && str.toLowerCase()
                                    .contains("test spec"))) {
                        StringTokenizer strTokenizer = new StringTokenizer(str,
                                "|");
                        int index = 0;
                        String[] strArray = new String[strTokenizer
                                .countTokens()];
                        while (strTokenizer.hasMoreTokens()) {
                            strArray[index++] = strTokenizer.nextToken().trim();
                        }

                        if (strArray.length < 3) {
                            if (strArray.length > 1) {
                                if (!hashMapChild.containsKey(strArray[1]
                                        .toUpperCase())) {
                                    hashMapChild.put(strArray[1].toUpperCase(),
                                            null);
                                }
                            }
                        } else {
                            if (hashMapChild.containsKey(strArray[1]
                                    .toUpperCase())) {
                                hashMapChild.put(
                                        strArray[1].toUpperCase(),
                                        hashMapChild.get(strArray[1]
                                                .toUpperCase())
                                                + "\n"
                                                + strArray[2]);
                            } else {
                                hashMapChild.put(strArray[1].toUpperCase(),
                                        strArray[2]);
                            }
                        }
                    } else if (str.matches("(^\\t|^\\s{2,})(\\[Tags\\]).*")) {
                        String[] strArray = str.split("]");
                        if (strArray.length < 2) {
                            hashMapChild.put(
                                    strArray[0].split("\\[")[1].toUpperCase(),
                                    null);
                        } else {
                            hashMapChild.put(
                                    strArray[0].split("\\[")[1].toUpperCase(),
                                    strArray[1].substring(1));
                        }
                    } else if ((documentationFlag == true)
                            && !(str.matches("^\\s{0,}(\\.\\.\\.)(?!\\.).{0,}"))) {
                        documentationFlag = false;
                    }
                }
            }
            bufferedReader.close();
        } catch (IOException e) {
            MultiStatus status = UIConst.createMultiStatus(
                    e.getLocalizedMessage(), e);
            ErrorDialog.openError(
                    Display.getDefault().getActiveShell(),
                    "Test Case Parsing Error",
                    "Fail to parsing Test Case.\nTest suite : "
                            + filePath.getFileName() + "\nTest case : " + str,
                    status);
        }
        if (fileReader != null) {
            fileReader.close();
        }

        return hashMapParent;
    }
}
