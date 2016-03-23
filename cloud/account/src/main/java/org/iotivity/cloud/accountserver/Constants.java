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
package org.iotivity.cloud.accountserver;

public class Constants {

    // MongoDB
    public static final String DEVICE_TABLE          = "USER_DEVICE";
    public static final String SESSION_TABLE         = "USER_SESSION";
    public static final String DB_NAME               = "ACCOUNTSERVER_DB";
    public static final String USER_ID               = "userId";
    public static final String DEVICE_ID             = "deviceId";
    public static final String SESSION_CODE          = "sessionCode";

    // Resource Uri
    public static final String AUTH_URI              = "oic/auth";
    public static final String ACCOUNT_URI           = "oic/account";

    // query parameter
    public static final String REQ_TYPE              = "reqtype";

    // request type
    public static final String TYPE_REGISTER         = "register";
    public static final String TYPE_LOGIN            = "login";
    public static final String TYPE_PUBLISH          = "publish";
    public static final String TYPE_FIND             = "find";

    // request payload
    public static final String REQUEST_AUTH_CODE     = "authcode";
    public static final String REQUEST_AUTH_SERVER   = "authprovider";
    public static final String REQUEST_DEVICE_ID     = "deviceid";
    public static final String REQUEST_USER_ID       = "userid";
    public static final String REQUEST_SESSION_CODE  = "session";

    // response
    public static final String RESPONSE_SESSION_CODE = "session";
    public static final String RESPONSE_USER_ID      = "userid";
    public static final String RESPONSE_STATE        = "state";
    public static final String RESPONSE_DEVICES      = "devices";

    // auth servers
    public static final String GITHUB                = "github";
    public static final String GOOGLE                = "google";
    public static final String FACEBOOK              = "facebook";
}
