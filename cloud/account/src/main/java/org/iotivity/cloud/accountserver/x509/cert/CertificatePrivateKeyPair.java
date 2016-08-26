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
package org.iotivity.cloud.accountserver.x509.cert;

import java.security.PrivateKey;
import java.security.cert.X509Certificate;

public class CertificatePrivateKeyPair {

    private final X509Certificate x509Certificate;

    private final PrivateKey privateKey;

    /**
     * Constructs new CertificatePrivateKeyPair with help of X509Certificate and PrivateKey instances.
     * @param x509Certificate specified certificate
     * @param privateKey specified private key.
     */
    public CertificatePrivateKeyPair(X509Certificate x509Certificate, PrivateKey privateKey) {
        this.x509Certificate = x509Certificate;
        this.privateKey = privateKey;
    }

    /**
     * Returns private key.
     */
    public PrivateKey getPrivateKey() {
        return privateKey;
    }

    /**
     * Returns certificate.
     */
    public X509Certificate getX509Certificate() {
        return x509Certificate;
    }
}

