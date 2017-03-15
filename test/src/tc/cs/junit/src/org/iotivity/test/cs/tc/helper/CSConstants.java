/******************************************************************
 * Copyright 2017 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * LICENSE-2.0" target="_blank">http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************/

package org.iotivity.test.cs.tc.helper;

public class CSConstants {
    public static final String TAG = "CSTest";
    public static final String EMPTY_STRING = "";
    public static final String WRONG_ERROR_CODE = "Expected Error Code Mismatch";
    public static String mErrorMessage = "";
    public static final int NUMBER_ZERO = 0;
    public static final int BUFFER_SIZE = 1024;

    public static final int DELAY_SHORT = 1000;                                  // millisecond
    public static final int DISCOVERY_TIMEOUT_05 = 5;
    public static final int DISCOVERY_TIMEOUT_ZERO = 0;
    public static final int DISCOVERY_TIMEOUT_ONE = 1;
    public static final int DISCOVERY_TIMEOUT_TWO = 2;
    public static final int DISCOVERY_TIMEOUT_NEGATIVE = -1;

    // Callback Related Resources
    public static final int SUCCES_CODE = 0;
    public static final int ERROR_CODE = 1;
    public static final int CALLBACK_TIMEOUT = 60;                                    // 60
    // seconds
    public static final boolean CALLBACK_INVOKED = true;
    public static final boolean CALLBACK_NOT_INVOKED = false;
    public static final int CALLBACK_INVOKED_BUT_FAILED = 0;
    public static final String CallBackNotInvoked = "CallBack Not Invoked";
    public static final String InputPinCBCallbackInvoked = "InputPinCBCallbackInvoked";

    // PM Initialization Resources
    public static final String OIC_CLIENT_CBOR_DB_FILE = "oic_svr_db_client.dat";

    public static final String OIC_CLOUD_CLIENT = "cloud.dat";
    public static final String OIC_SQL_DB_FILE = "Pdm.db";
    public static final String SERVER_SQL_DB_FILE = "ServerPdm.db";

    
    //Cloud Resource
    public static final String CERT_SERIAL_ONE = "1";


    // Older Version... will be removed
    public static final String SUBJECT_O1 = "44446d69-6e44-6576-6963-655555400001";
    public static final String SUBJECT_O2 = "55556d69-6e44-6576-6963-655555400002";
    public static final String DEFAULT_RECURRENCES = "Daily";

    // Credential Resource
    public static final int CREDENTIAL_TYPE = 1;
    public static final int OWNER_PSK_LENGTH_128 = 128 / 8;

    // Direct Pairing Resources
    public static final String DP_PRE_CONFIGURED_PIN = "00000000";

    // Error Code
    public static final String ERROR_INVALID_PARAM = "INVALID_PARAM";
    public static final String ERROR_INVALID_CALLBACK = "INVALID_CALLBACK";
    public static final String ERROR_ERROR = "ERROR";

    // Others
    public static final int DEVICE_INDEX_ZERO = 0;
    public static final int DEVICE_INDEX_ONE = 1;
    
    public static final String DEFAULT_DEVICE_ID = "9cfbeb8e-5a1e-4d1c-9d01-2ae6fdb";
    public static final String COAP_TCP = "coap+tcp://";
    public static final String DEFAULT_COAP_DERVER_IP = "107.109.211.225";
    public static final String DEFAULT_COAP_DERVER_PORT = "5683";
    public static final String DEFAULT_AUTH_PROVIDER = "github";
    public static final String DEFAULT_AUTH_CODE = "0ae302ac2da009b10c00";

    public static final String MESSAGE = "message";
    public static final String OIC_CLOUD_SQL_DB_FILE = "Cloud.db";
    public static final String DEFAULT_CRL = "CRL";
    
    public static final String TCP_CLIENT_PACKAGE_NAME = "org.iotivity.tcpandroidclient.intent.action.Launch";
    public static final String TCP_CLIENT_BROADCAST_INTENT =  "service";
}