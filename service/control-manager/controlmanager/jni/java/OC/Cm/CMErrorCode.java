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

package OC.Cm;

public enum CMErrorCode {

    SHP_SUCCESS(0), SHP_FAILURE(1), SHP_ALREADY_INITIALIZED(100), SHP_FC_UNINITIALIZED(
            101),

    SHP_NET_ERROR_TIMED_OUT(102), SHP_INVALID_INPUT(103), SHP_MEMORY_ALLOCATION_FAILED(
            104),

    SHP_NET_INVALID_INTERFACE(105), SHP_NET_INVALID_IPADDRESS(106), SHP_NET_INVALID_RESPONSE(
            107),

    SHP_NET_LISTEN_FAILED(108), SHP_NET_ACCEPT_FAILED(109), SHP_NET_CONNECT_FAILED(
            110), SHP_NET_WRITE_FAILED(111), SHP_NET_BIND_FAILED(112), SHP_NET_SOCKET_ERROR(
            113), SHP_NET_UDP_SOCKET_ERROR(114), SHP_NET_EOF(115),

    SHP_HTTPS_HANDSHAKE_FAILED(116),

    SHP_HTTP_NEXT_PACKET_REQUIRED(117), SHP_HTTP_INVALID_FIRST_LINE(118), SHP_HTTP_INVALID_HEADER_FIELD(
            119), SHP_HTTP_INVALID_CONTENT_LENGTH(120), SHP_HTTP_NOT_SUPPORTED_TRANSFER_ENCODING(
            121), SHP_HTTP_CHUNCK_SIZE_MISMATCH(122),

    SHP_HTTP_PARSING_FAILED(123), SHP_HTTP_ENCODING_FAILED(124), SHP_HTTP_INVALID_DATA_RECEIVED(
            125), SHP_DESERIALIZE_FACTORY_NOT_FOUND(126), SHP_DESERIALIZE_FAILED(
            127), SHP_SERIALIZATION_FAILED(128),

    SHP_SUBSCRIPTION_DB_RECORD_NOT_FOUND(129), SHP_SUBSCRIPTION_INVALID_RESPONSE(
            130), SHP_NO_PEERDEVICE_TOKEN(131), SHP_DEVICE_OFFLINE(132), SHP_SERVER_ERROR(
            133), SHP_REMOTE_CHANNEL_INITIALIZED_FAILED(134), SHP_IN_STOPPING(
            135);

    private int value;

    private CMErrorCode(int value) {
        this.value = value;
    }

    private CMErrorCode() {
    }

    public int getValue() {
        return value;
    }

    public static CMErrorCode getErrorCode(int value) {
        for (CMErrorCode error : CMErrorCode.values()) {
            if (error.getValue() == value) {
                return error;
            }
        }

        return SHP_FAILURE;
    }
}
