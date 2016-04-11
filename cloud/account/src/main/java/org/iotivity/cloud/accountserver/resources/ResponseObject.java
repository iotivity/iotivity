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
package org.iotivity.cloud.accountserver.resources;

import java.util.ArrayList;

/**
 * 
 * This class provides a set of APIs to manage data for response according to
 * request.
 * 
 */
public class ResponseObject {
    // private String channel = null;
    private String            sessionCode = null;
    private String            userId      = null;
    private ArrayList<String> deviceList  = null;

    ResponseObject() {
        deviceList = new ArrayList<String>();
    }

    public String getUserId() {
        return this.userId;
    }

    public void setUserId(String id) {
        this.userId = id;
    }

    public String getSessionCode() {
        return this.sessionCode;
    }

    public void setSessionCode(String code) {
        this.sessionCode = code;
    }

    public void setDeviceList(ArrayList<String> list) {
        this.deviceList = list;
    }

    public ArrayList<String> getDeviceList() {
        return this.deviceList;
    }
}
