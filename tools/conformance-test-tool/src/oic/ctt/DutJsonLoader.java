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

import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.HashMap;

import oic.ctt.formatter.JsonAnalyzer;
import oic.ctt.logger.CTLogger;

import org.slf4j.Logger;

/**
 * This Class is for Parsing DUT Information JSON file
 */
public class DutJsonLoader implements IDutLoader {
    private JsonAnalyzer mDutAnalyzer;
    private Logger       mlogger = CTLogger.getInstance();

    /**
     * Constructor to Get json string from json file
     *
     * @param dutJsonFile
     *            : Absolute Path to JSON file
     */
    public DutJsonLoader(String dutJsonFilePath) {
        try (InputStream in = this.getClass().getResourceAsStream(
                "/" + dutJsonFilePath)) {
            try (java.util.Scanner sc = new java.util.Scanner(in)) {
                mlogger.debug("DUT JSON String : " + dutJsonFilePath);

                StringBuffer dutJson = new StringBuffer();
                sc.useDelimiter("\\n");
                while (sc.hasNext()) {
                    dutJson.append(sc.next().trim());
                }
                mDutAnalyzer = new JsonAnalyzer(dutJson.toString());
                mlogger.debug("DUT JSON String : "
                        + mDutAnalyzer.getJsonString());
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /**
     * Return HashMap have dut resource information
     */
    @Override
    public HashMap<String, DUTResource> loadDutResources() {
        HashMap<String, DUTResource> dutResourcesMap = new HashMap<String, DUTResource>();
        ArrayList<String> resourcesInfo;
        resourcesInfo = mDutAnalyzer.getValue(DutKey.ROOT.toString());

        if (resourcesInfo == null) {
            mlogger.debug("DutResuorcesMap is Empty");
            return dutResourcesMap;
        }

        for (String resourceInfo : resourcesInfo) {
            JsonAnalyzer resourceAnalyzer = new JsonAnalyzer(resourceInfo);
            ArrayList<String> resourceHref = resourceAnalyzer
                    .getValue(DutKey.HREF.toString());

            if (resourceHref.size() > 0) {
                dutResourcesMap.put(resourceHref.get(0), new DUTResource(
                        resourceInfo));
            }

        }
        return dutResourcesMap;
    }

    /**
     * Returns the string value is di of json information loaded
     */
    @Override
    public String loadDUTId() {
        ArrayList<String> valueList = mDutAnalyzer.getValue(DutKey.DI
                .toString());

        if (valueList == null) {
            mlogger.debug("get value returns null for Key : "
                    + DutKey.DI.toString());
            return "";
        } else {
            return valueList.get(0);
        }
    }
}
