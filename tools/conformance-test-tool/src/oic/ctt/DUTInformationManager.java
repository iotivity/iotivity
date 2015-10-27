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

package oic.ctt;

import java.util.HashMap;

import oic.ctt.logger.CTLogger;

import org.slf4j.Logger;

/**
 * This Class works as a Manager for DUT Information. Basically this is a
 * singleton class. It Creates the DUT Information instance based on the JSON
 * file provided.
 *
 */
public class DUTInformationManager {

    private Logger                          mlogger                        = CTLogger
                                                                                   .getInstance();
    private HashMap<String, DUTInformation> mDUTMap                        = new HashMap<String, DUTInformation>();
    private static DUTInformationManager    sDUTInformationManagerInstance = new DUTInformationManager();

    private DUTInformationManager() {
    }

    /**
     * This is the Get Instance Method of This Singleton class
     * 
     * @return DUTInformationManager : Singleton Instance fo the
     *         DUTInformationManager
     */
    public static DUTInformationManager getInstance() {
        return sDUTInformationManagerInstance;
    }

    /**
     * This Method Creates a new DUT Information Instance with specific ID from
     * the JSON file provided.
     * 
     * @param dutID
     *            : Specific and Unique DUT ID
     * @param dutPath
     *            : Absolute path of the JSON file
     * @return boolean : true if the creation is successful else false
     */
    public boolean createDUTInformation(String dutID, String dutPath) {
        DUTInformation dutInformation = null;

        try {
            IDutLoader loader = new DutJsonLoader(dutPath);
            dutInformation = new DUTInformation(loader);

        } catch (Exception e) {
            e.printStackTrace();
            mlogger.error(e.getMessage());
        }

        if (dutInformation != null) {
            mDUTMap.put(dutID, dutInformation);
            return true;
        }
        return false;
    }

    /**
     * This Method returns the DUT Information Instance that matches with the
     * specific DUT ID
     * 
     * @param dutID
     *            : specific DUT ID to get the corresponding DUT Information
     * @return DUTInformation : DUTInformation instance that matches with the
     *         dutID
     */
    public DUTInformation getDUTInformation(String dutID) {
        return mDUTMap.get(dutID);
    }

    /**
     * This Method removes the DUT Information Instance that matches with the
     * dutID from Map
     * 
     * @param dutID
     *            : DUT Id to delete from the Map
     */
    public void removeDUTInformation(String dutID) {
        mDUTMap.remove(dutID);
    }
}
