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



#ifndef CMErrorCODE_H_
#define CMErrorCODE_H_

namespace OC
{

    namespace Cm
    {
        /**
        *  @addtogroup ControlManager
        *  @{
        */

        /**
         * @enum        shp_err_codes
         * @brief       Error codes defined by Control Manager framework
         */
        typedef enum shp_err_codes
        {
            CM_SUCCESS = 0,                             /** Represents Success */
            CM_FAILURE = 1,                             /** Represents Failure */

            CM_ALREADY_INITIALIZED = 100,                   /** Represents Framework is already initialized */
            CM_FC_UNINITIALIZED = 101,                      /** Represents Framework is currently uninitialized */

            CM_NET_ERROR_TIMED_OUT = 102,                   /** Represents Network Connection Timed Out Error */
            CM_INVALID_INPUT = 103,                     /** Represents Input in invalid */
            CM_MEMORY_ALLOCATION_FAILED = 104,              /** Represents Memory allocation failed */

            CM_NET_INVALID_INTERFACE = 105,             /** Represents Invalid network interface */
            CM_NET_INVALID_IPADDRESS = 106,             /** Represents Invalid IP address */
            CM_NET_INVALID_RESPONSE = 107,                  /** Represents Invalid Response */

            CM_NET_LISTEN_FAILED = 108,                 /** Represents Socket listen failed */
            CM_NET_ACCEPT_FAILED = 109,                 /** Represents Socket accept failed */
            CM_NET_CONNECT_FAILED = 110,                    /** Represents Socket connect failed */
            CM_NET_WRITE_FAILED = 111,                      /** Represents Socket write failed */
            CM_NET_BIND_FAILED = 112,                       /** Represents Socket bind failed */
            CM_NET_SOCKET_ERROR = 113,                      /** Represents Error occurred on socket */
            CM_NET_UDP_SOCKET_ERROR = 114,                  /** Represents Error occurred on UDP socket */
            CM_NET_EOF = 115,                               /** Represents Socket is closed */

            CM_HTTPS_HANDSHAKE_FAILED = 116,                /** Represents Handshake Failed, Secure connection not established */

            CM_HTTP_NEXT_PACKET_REQUIRED = 117,         /** Represents HTTP Parser received incomplete HTTP buffer */
            CM_HTTP_INVALID_FIRST_LINE = 118,               /** Represents HTTP Message contains invalid first line */
            CM_HTTP_INVALID_HEADER_FIELD = 119,         /** Represents HTTP Message contains invalid header field */
            CM_HTTP_INVALID_CONTENT_LENGTH = 120,           /** Represents HTTP Message contains invalid content length */
            CM_HTTP_NOT_SUPPORTED_TRANSFER_ENCODING = 121, /** Represents unsupported transfer encoding */
            CM_HTTP_CHUNCK_SIZE_MISMATCH = 122,         /** Represents current chunk size and size specified in chunk header are not equal */

            CM_HTTP_PARSING_FAILED = 123,                   /** Represents HTTP Parsing failed */
            CM_HTTP_ENCODING_FAILED = 124,                  /** Represents HTTP Encoding failed */
            CM_HTTP_INVALID_DATA_RECEIVED = 125,            /** Represents HTTP Parser receive invalid payload */
            CM_DESERIALIZE_FACTORY_NOT_FOUND = 126,     /** Represents Deserializer factory is not available */
            CM_DESERIALIZE_FAILED = 127,                    /** Represents Deserialization failed */
            CM_SERIALIZATION_FAILED = 128,                  /** Represents Serialization failed */

            CM_SUBSCRIPTION_DB_RECORD_NOT_FOUND = 129,      /** Represents Subscription database does not contain entry for current query */
            CM_SUBSCRIPTION_INVALID_RESPONSE = 130,     /** Represents Invalid response is received */
            CM_NO_PEERDEVICE_TOKEN = 131,                   /** Represents There is no Peer Device Token */
            CM_DEVICE_OFFLINE = 132,                        /** Represents Device online status is offline */
            CM_SERVER_ERROR = 133,                          /** Represents Internal Server Error */
            CM_REMOTE_CHANNEL_INITIALIZED_FAILED = 134, /** Represents Some error while initializing remote access channel */
            CM_IN_STOPPING = 135,                           /** Represents Control Manager is in stopping mode */

            CIPHER_KEY_GENERATION_FAILURE = 141,            /** Represents Failed to generate Key with given UUID */
            DECRYPT_INCORRECT_PADDING = 142,                /** Represents CipherText input has incorrect padding after decryption */
            DECRYPT_INCORRECT_LENGTH = 143                  /** Represents CipherText input has incorrect length */

        } ErrorCode; /**< Represents  enum shp_err_codes */

        /** @} */
    } /* namespace Cm */

} /* namespace OC */

#endif /* CMErrorCODE_H_ */
