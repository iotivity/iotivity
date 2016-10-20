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

    /**
     * API to initialize group table instance
     * 
     * @param gid
     *            group ID
     * @param midlist
     *            member ID list as a form of array
     * @param dilist
     *            device ID list as a form of array
     * @param gmid
     *            group master ID
     * @param gacl
     *            group ACL
     * @param gtype
     *            group type
     */
    public GroupTable(String gid, Object midlist, Object dilist, String gmid,
            String gacl, String gtype) {
        this.gid = gid;
        this.midlist = midlist;
        this.dilist = dilist;
        this.gmid = gmid;
        this.gacl = gacl;
        this.gtype = gtype;
    }

    /**
     * API to get group ID
     * 
     * @return group ID
     */
    public String getGid() {
        return gid;
    }

    /**
     * API to set group ID
     * 
     * @param gid
     *            group ID to be set
     */
    public void setGid(Object gid) {
        this.gid = gid.toString();
    }

    /**
     * API to get member ID list of the group
     * 
     * @return member ID list
     */
    public Object getMidlist() {
        return midlist;
    }

    /**
     * API to set member ID list of the group
     * 
     * @param midlist
     *            member ID list to be set
     */
    public void setMidlist(Object midlist) {
        this.midlist = midlist;
    }

    /**
     * API to get device ID list of the group
     * 
     * @return device ID list
     */
    public Object getDilist() {
        return dilist;
    }

    /**
     * API to set device ID list of the group
     * 
     * @param dilist
     *            device ID list to be set
     */
    public void setDilist(Object dilist) {
        this.dilist = dilist;
    }

    /**
     * API to get the group master ID
     * 
     * @return group master ID
     */
    public String getGmid() {
        return gmid;
    }

    /**
     * API to set the master ID of the group
     * 
     * @param gmid
     *            group master ID to be set
     */
    public void setGmid(Object gmid) {
        this.gmid = gmid.toString();
    }

    /**
     * API to get group ACL
     * 
     * @return group ACL
     */
    public String getGacl() {
        return gacl;
    }

    /**
     * API to set group ACL of the group
     * 
     * @param gacl
     *            group ACL to be set
     */
    public void setGacl(Object gacl) {
        this.gacl = gacl.toString();
    }

    /**
     * API to get group type
     * 
     * @return group type
     */
    public String getGtype() {
        return gtype;
    }

    /**
     * API to set group type
     * 
     * @param gtype
     *            group type to be set
     */
    public void setGtype(Object gtype) {
        this.gtype = gtype.toString();
    }
}