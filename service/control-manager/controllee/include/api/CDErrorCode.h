//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef CDErrorCODE_H_
#define CDErrorCODE_H_

namespace OC
{
    namespace Cm
    {
        /**
         * @enum        shp_err_codes
         * @brief       Error codes defined by SHP framework
         */
        typedef enum shp_err_codes
        {
            SHP_SUCCESS = 0, /** Represents Success */
            SHP_FAILURE = 1, /** Represents Failure */

            SHP_ALREADY_INITIALIZED = 100, /** Represents Framework is already initialized */
            SHP_FC_UNINITIALIZED = 101, /** Represents Framework is currently uninitialized */

            SHP_NET_ERROR_TIMED_OUT = 102, /** Represents Network Connection Timed Out Error */
            SHP_INVALID_INPUT = 103, /** Represents Input in invalid */
            SHP_MEMORY_ALLOCATION_FAILED = 104, /** Represents Memory allocation failed */

            SHP_NET_INVALID_INTERFACE = 105, /** Represents Invalid network interface */
            SHP_NET_INVALID_IPADDRESS = 106, /** Represents Invalid IP address */
            SHP_NET_INVALID_RESPONSE = 107, /** Represents Invalid Response */

            SHP_NET_LISTEN_FAILED = 108, /** Represents Socket listen failed */
            SHP_NET_ACCEPT_FAILED = 109, /** Represents Socket accept failed */
            SHP_NET_CONNECT_FAILED = 110, /** Represents Socket connect failed */
            SHP_NET_WRITE_FAILED = 111, /** Represents Socket write failed */
            SHP_NET_BIND_FAILED = 112, /** Represents Socket bind failed */
            SHP_NET_SOCKET_ERROR = 113, /** Represents Error occurred on socket */
            SHP_NET_UDP_SOCKET_ERROR = 114, /** Represents Error occurred on UDP socket */
            SHP_NET_EOF = 115, /** Represents Socket is closed */

            SHP_HTTPS_HANDSHAKE_FAILED = 116, /** Represents Handshake Failed, Secure connection not established */

            SHP_HTTP_NEXT_PACKET_REQUIRED = 117, /** Represents HTTP Parser received incomplete HTTP buffer */
            SHP_HTTP_INVALID_FIRST_LINE = 118, /** Represents HTTP Message contains invalid first line */
            SHP_HTTP_INVALID_HEADER_FIELD = 119, /** Represents HTTP Message contains invalid header field */
            SHP_HTTP_INVALID_CONTENT_LENGTH = 120, /** Represents HTTP Message contains invalid content length */
            SHP_HTTP_NOT_SUPPORTED_TRANSFER_ENCODING = 121, /** Represents unsupported transfer encoding */
            SHP_HTTP_CHUNCK_SIZE_MISMATCH = 122, /** Represents current chunk size and size specified in chunk header are not equal */

            SHP_HTTP_PARSING_FAILED = 123, /** Represents HTTP Parsing failed */
            SHP_HTTP_ENCODING_FAILED = 124, /** Represents HTTP Encoding failed */
            SHP_HTTP_INVALID_DATA_RECEIVED = 125, /** Represents HTTP Parser receive invalid payload */
            SHP_DESERIALIZE_FACTORY_NOT_FOUND = 126, /** Represents Deserializer factory is not available */
            SHP_DESERIALIZE_FAILED = 127, /** Represents Deserialization failed */
            SHP_SERIALIZATION_FAILED = 128, /** Represents Serialization failed */

            SHP_SUBSCRIPTION_DB_RECORD_NOT_FOUND = 129, /** Represents Subscription database does not contain entry for current query */
            SHP_SUBSCRIPTION_INVALID_RESPONSE = 130, /** Represents Invalid response is received */
            SHP_NO_PEERDEVICE_TOKEN = 131, /** Represents There is no Peer Device Token */
            SHP_DEVICE_OFFLINE = 132, /** Represents Device online status is offline */
            SHP_SERVER_ERROR = 133, /** Represents Internal Server Error */
            SHP_REMOTE_CHANNEL_INITIALIZED_FAILED = 134, /** Represents Some error while initializing remote access channel */
            SHP_IN_STOPPING = 135, /** Represents SHP is in stopping mode */

            CIPHER_KEY_GENERATION_FAILURE = 141, /** Represents Failed to generate Key with given UUID */
            DECRYPT_INCORRECT_PADDING = 142, /** Represents CipherText input has incorrect padding after decryption */
            DECRYPT_INCORRECT_LENGTH = 143 /** Represents CipherText input has incorrect length */

        } ErrorCode; /**< Represents  enum shp_err_codes */

    } /* namespace Cm */
} /* namespace OC */

#endif /* CDErrorCODE_H_ */
