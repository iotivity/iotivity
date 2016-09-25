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
package org.iotivity.cloud.accountserver.db;

public class GroupTable {

    private String gid;
    private Object midlist;
    private Object dilist;
    private String gmid;
    private String gacl;
    private String gtype;

    public GroupTable() {
    }

    public GroupTable(String gid, Object midlist, Object dilist, String gmid,
            String gacl, String gtype) {
        this.gid = gid;
        this.midlist = midlist;
        this.dilist = dilist;
        this.gmid = gmid;
        this.gacl = gacl;
        this.gtype = gtype;
    }

    public String getGid() {
        return gid;
    }

    public void setGid(Object gid) {
        this.gid = gid.toString();
    }

    public Object getMidlist() {
        return midlist;
    }

    public void setMidlist(Object midlist) {
        this.midlist = midlist;
    }

    public Object getDilist() {
        return dilist;
    }

    public void setDilist(Object dilist) {
        this.dilist = dilist;
    }

    public String getGmid() {
        return gmid;
    }

    public void setGmid(Object gmid) {
        this.gmid = gmid.toString();
    }

    public String getGacl() {
        return gacl;
    }

    public void setGacl(Object gacl) {
        this.gacl = gacl.toString();
    }

    public String getGtype() {
        return gtype;
    }

    public void setGtype(Object gtype) {
        this.gtype = gtype.toString();
    }
}