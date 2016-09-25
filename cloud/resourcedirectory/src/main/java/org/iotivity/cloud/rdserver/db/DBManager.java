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
package org.iotivity.cloud.rdserver.db;

import java.util.ArrayList;
import java.util.HashMap;

import org.iotivity.cloud.rdserver.Constants;
import org.iotivity.cloud.rdserver.resources.presence.resource.ResPresencePayload;

public class DBManager {

    private static DBManager mDBManager = new DBManager();
    private MongoDB          mMongoDB   = null;;

    private DBManager() {
        try {
            mMongoDB = new MongoDB(Constants.RD_DB_NAME);
            mMongoDB.createTable(Constants.RD_TABLE);
            mMongoDB.createTable(Constants.PRESENCE_TABLE);
        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

    public static DBManager getInstance() {
        return mDBManager;
    }

    public ArrayList<ResPresencePayload> registerResource(
            ArrayList<HashMap<Object, Object>> pubResourceList) {
        return mMongoDB.createRDResource(pubResourceList, Constants.RD_TABLE);
    }

    public ArrayList<HashMap<Object, Object>> findResourceAboutDi(String di) {
        return mMongoDB.readResourceAboutDid(di, Constants.RD_TABLE);
    }
    
    public ArrayList<HashMap<Object, Object>> findResourceAboutDiAndFilter(String di,
            String key, String value) {
        return mMongoDB.readResourceAboutDidAndFilter(di, key, value,
                Constants.RD_TABLE);
    }

    public ArrayList<ResPresencePayload> deleteResourceAboutDi(String di) {
        return mMongoDB.deleteResourceAboutDi(di, Constants.RD_TABLE);
    }

    public ArrayList<ResPresencePayload> deleteResourceAboutDiAandIns(String di,
            String ins) {
        return mMongoDB.deleteResourceAboutDiAndIns(di, ins,
                Constants.RD_TABLE);
    }

    public Object findInsAboutDi(String di, String href) {
        return mMongoDB.readInsAboutDid(di, href, Constants.RD_TABLE);
    }

    public void updateDeviceState(HashMap<Object, Object> deviceState) {
        mMongoDB.createDevicePresenceResource(deviceState,
                Constants.PRESENCE_TABLE);
    }

    public String findDeviceState(String deviceId) {
        return mMongoDB.readDeviceState(deviceId, Constants.PRESENCE_TABLE);
    }
}
