/* ****************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

package org.iotivity.ca;

public enum OicCipher {
    TLS_RSA_WITH_AES_256_CBC_SHA256(0x3D),
    TLS_RSA_WITH_AES_128_GCM_SHA256(0x009C),
    TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256(0xC02B),
    TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8(0xC0AE),
    TLS_ECDHE_ECDSA_WITH_AES_128_CCM(0xC0AC),
    TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256(0xC023),
    TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384(0xC024),
    TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384(0xC02C),
    TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256(0xC037),
    TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256(0xC027),
    TLS_ECDH_anon_WITH_AES_128_CBC_SHA(0xC018);

    private int cipher;

    private OicCipher(int cipher) {
        this.cipher = cipher;
    }

    public int getValue(){
        return cipher;
    }
}
