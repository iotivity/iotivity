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

import org.iotivity.cloud.base.OCFConstants;

public class Constants extends OCFConstants {

    // MongoDB

    public static final String DB_NAME             = "ACCOUNTSERVER_DB";

    public static final String DEVICE_TABLE        = "USER_DEVICE";

    public static final String TOKEN_TABLE         = "USER_TOKEN";

    public static final String KEY_USER_ID         = "userId";

    public static final String KEY_DEVICE_ID       = "deviceId";

    public static final String KEY_ACCESS_TOKEN    = "accesstoken";

    public static final String KEY_REFRESH_TOKEN   = "refreshtoken";

    public static final String KEY_ISSUED_TIME     = "issuedTime";

    // request payload key

    public static final String REQ_DEVICE_ID       = "di";

    public static final String REQ_AUTH_CODE       = "authcode";

    public static final String REQ_AUTH_PROVIDER   = "authprovider";

    public static final String REQ_ACCESS_TOKEN    = "accesstoken";

    public static final String REQ_STATUS          = "status";

    public static final String REQ_REFRESH_TOKEN   = "refreshtoken";

    public static final String REQ_AUTH_OPTIONS    = "options";

    public static final String REQ_AUTH_SERVER_URL = "auth_server_url";

    public static final String REQ_API_SERVER_URL  = "api_server_url";

    // response payload key

    public static final String RESP_ACCESS_TOKEN   = "accesstoken";

    public static final String RESP_REFRESH_TOKEN  = "refreshtoken";

    public static final String RESP_TOKEN_TYPE     = "tokentype";

    public static final String RESP_EXPIRES_IN     = "expiresin";

    public static final String RESP_DEVICES        = "devices";

    public static final String RESP_USER_ID        = "uid";

    // auth servers

    public static final String GITHUB              = "Github";

    public static final String SAMSUNG             = "Samsung";

    public static final String KEY_ACCESS_TOKEN_GH = "access_token";

    // etc

    public static final String ERROR_MESSAGE       = "error_message";
}
