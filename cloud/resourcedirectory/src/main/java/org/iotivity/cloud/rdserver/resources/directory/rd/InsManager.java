/*
 * //******************************************************************
 * //
 * // Copyright 2016 Samsung Electronics All Rights Reserved.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 * //
 * // Licensed under the Apache License, Version 2.0 (the "License");
 * // you may not use this file except in compliance with the License.
 * // You may obtain a copy of the License at
 * //
 * //      http://www.apache.org/licenses/LICENSE-2.0
 * //
 * // Unless required by applicable law or agreed to in writing, software
 * // distributed under the License is distributed on an "AS IS" BASIS,
 * // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * // See the License for the specific language governing permissions and
 * // limitations under the License.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */
package org.iotivity.cloud.rdserver.resources.directory.rd;

import java.util.HashMap;

import org.iotivity.cloud.rdserver.db.DBManager;

public class InsManager {

    private HashMap<String, Integer> mNextIns      = new HashMap<>();
    private int                      mInitialValue = 1;

    public int getIns(String di, String href) {

        Object objectIns = DBManager.getInstance().findInsAboutDi(di, href);
        if (objectIns == null) {
            return 0;
        } else {
            return (int) objectIns;

        }
    }

    public int createIns(String di) {
        Object objectIns;

        objectIns = mNextIns.get(di);
        if (objectIns == null) {
            mNextIns.put(di, mInitialValue + 1);
            return mInitialValue;
        } else {
            int ins = (int) objectIns;
            mNextIns.put(di, ins + 1);
            return ins;
        }
    }
}
