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

import org.iotivity.cloud.accountserver.Constants;

/**
 *
 * This class provides a set of APIs storing session information of authorized
 * user.
 *
 */

public class TokenTable {

    private String uuid         = null;
    private String did          = null;
    private String accesstoken  = null;
    private String refreshtoken = null;
    private String provider     = null;
    private long   expiredtime  = Constants.TOKEN_INFINITE;
    private String issuedtime   = null;

    public String getUuid() {
        return uuid;
    }

    public void setUuid(Object uuid) {
        this.uuid = uuid.toString();
    }

    public String getDid() {
        return did;
    }

    public void setDid(String did) {
        this.did = did;
    }

    public String getAccesstoken() {
        return accesstoken;
    }

    public void setAccesstoken(Object accesstoken) {
        if (accesstoken != null)
            this.accesstoken = accesstoken.toString();
    }

    public String getRefreshtoken() {
        return refreshtoken;
    }

    public void setRefreshtoken(Object refreshtoken) {
        if (refreshtoken != null)
            this.refreshtoken = refreshtoken.toString();
    }

    public String getProvider() {
        return provider;
    }

    public void setProvider(Object provider) {
        if (provider != null)
            this.provider = provider.toString();
    }

    public long getExpiredtime() {
        return expiredtime;
    }

    public void setExpiredtime(Object expiredtime) {
        if (expiredtime != null)
            this.expiredtime = Long.valueOf(expiredtime.toString());
    }

    public String getIssuedtime() {
        return issuedtime;
    }

    public void setIssuedtime(Object issuedtime) {
        if (issuedtime != null)
            this.issuedtime = issuedtime.toString();
    }
}