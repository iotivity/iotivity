/******************************************************************
*
* Copyright 2016 Samsung Electronics All Rights Reserved.
*
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

package org.iotivity.service.ca.test.helper;

public class TestInfo {

    public static final int NETWORK = 1;
    public static final int TRY_COUNT = 3;

    public static final int MESSAGE_PAYLOAD = 0;
    public static final int MESSAGE_URI = 1;
    public static final int MESSAGE_HEADER = 2;

    public static final int MESSAGE_UNICAST = 0;
    public static final int MESSAGE_MULTICAST = 1;

    public static final int MESSAGE_INCOMING = 0;
    public static final int MESSAGE_OUTGOING = 1;

    public static final int CA_GET = 1;
    public static final int CA_POST = 2;
    public static final int CA_PUT = 3;
    public static final int CA_DELETE = 4;

    public static final int CA_MSG_CONFIRM = 0;
    public static final int CA_MSG_NONCONFIRM = 1;
    public static final int CA_MSG_ACK = 2;
    public static final int CA_MSG_RESET = 3;

    public static final int URI_TYPE_NULL_ENDPOINT = -3;
    public static final int URI_TYPE_ARBITRARY_ENDPOINT = -2;
    public static final int URI_TYPE_NULL = -1;
    public static final int URI_TYPE_EMPTY = 0;
    public static final int URI_TYPE_NORMAL = 1;

    public static final int PAYLOAD_TYPE_NULL = -1;
    public static final int PAYLOAD_TYPE_EMPTY = 0;
    public static final int PAYLOAD_TYPE_NORMAL = 1;
    public static final int PAYLOAD_TYPE_LARGE = 2;

    public static final int HEADER_NONE = 0;
    public static final int HEADER_ONE = 1;
    public static final int HEADER_TWO = 2;

    public static final int CA_VALID = 203;
    public static final int CA_INVALID = -100;

    public static final int SEND_MESSAGE = 0;
    public static final int MESSAGE_RESPONSE = 1;
    public static final int MESSAGE_REQUEST = 2;

    public static final int HANDLER_NORMAL = 1;
    public static final int HANDLER_NULL = 0;
    public static final int HANDLER_SECOND = 2;

    public static final int INVALID_NETWORK = 0;
    public static final int TOTAL_MESSAGE = 1;

    public static final int CA_STATUS_OK = 0;
    public static final int CA_STATUS_INVALID_PARAM = 1;
    public static final int CA_ADAPTER_NOT_ENABLED = 2;
    public static final int CA_SERVER_STARTED_ALREADY = 3;
    public static final int CA_SERVER_NOT_STARTED = 4;
    public static final int CA_DESTINATION_NOT_REACHABLE = 5;
    public static final int CA_SOCKET_OPERATION_FAILED = 6;
    public static final int CA_SEND_FAILED = 7;
    public static final int CA_RECEVIE_FAILED = 8;
    public static final int CA_MEMORY_ALLOC_FAILED = 9;
    public static final int CA_REQUEST_TIMEOUT = 10;
    public static final int CA_DESTINATION_DISCONNECTED = 11;
    public static final int CA_NOT_SUPPORTED = 12;
    public static final int CA_STATUS_NOT_INITIALIZED = 13;
    public static final int CA_DTLS_AUTHENTICATION_FAILURE = 14;
    public static final int CA_STATUS_FAILED = 255;

    public static final int TOKEN_TYPE_INVALID_REFERENCE = 0;
    public static final int TOKEN_TYPE_NULL = 1;
    public static final int TOKEN_TYPE_VALID = 2;

    public static final int TOKEN_LENGTH_LBV_VALID = 1;
    public static final int TOKEN_LENGTH_NORMAL = 8;
    public static final int TOKEN_LENGTH_LOBV_INVALID = -1;
    public static final int TOKEN_LENGTH_UOBV_INVALID = 9;

    public static final int CA_INVALID_LOBV_ADAPTER = -1;
    public static final int CA_DEFAULT_ADAPTER = 0;
    public static final int CA_ALL_ADAPTERS = 0xffffffff;
    public static final long CA_INVALID_UOBV_ADAPTER = 0x100000000L;

    public static final int NETWORKINFO_ENDPOINT_VALID = 0;
    public static final int NETWORKINFO_ENDPOINT_UFV = 1;
    public static final int NETWORKINFO_ENDPOINT_NULL = 2;

    public static final int NETWORKINFO_SIZE_VALID = 0;
    public static final int NETWORKINFO_SIZE_USV = 1;
    public static final int NETWORKINFO_SIZE_NULL = 2;

    public static final int ENDPOINT_VALID = 0;
    public static final int ENDPOINT_INVALID_FLAG = 1;
    public static final int ENDPOINT_INVALID_ADAPTER = 2;
    public static final int ENDPOINT_INVALID_ADDRESS = 3;
    public static final int ENDPOINT_INVALID_PORT = 4;
    public static final int ENDPOINT_INVALID_ENDPOINT = 5;
    public static final int ENDPOINT_ESV_ADDRESS = 6;
    public static final int ENDPOINT_URV_ENDPOINT = 7;
    public static final int ENDPOINT_NULL = 0;

    public static String serverIp;
    public static int serverPort;
    public static int serverSecuredPort;
}
