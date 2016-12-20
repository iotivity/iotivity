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
    TLS_ECDH_anon_WITH_AES_128_CBC_SHA(0xC018),
    TLS_PSK_WITH_AES_128_CCM_8(0xC0A8),
    TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8(0xC0AE),
    TLS_RSA_WITH_AES_256_CBC_SHA(0x35);

    private int cipher;

    private OicCipher(int cipher) {
        this.cipher = cipher;
    }

    public int getValue(){
        return cipher;
    }
}
