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

import org.bouncycastle.asn1.ASN1ObjectIdentifier;
import org.bouncycastle.asn1.x500.RDN;
import org.bouncycastle.asn1.x500.X500Name;
import org.bouncycastle.asn1.x509.SubjectPublicKeyInfo;
import org.bouncycastle.openssl.PEMException;
import org.bouncycastle.openssl.jcajce.JcaPEMKeyConverter;
import org.bouncycastle.operator.OperatorCreationException;
import org.bouncycastle.operator.jcajce.JcaContentVerifierProviderBuilder;
import org.bouncycastle.pkcs.PKCS10CertificationRequest;
import org.bouncycastle.pkcs.PKCSException;
import org.iotivity.cloud.accountserver.resources.credprov.cert.CertificateStorage;
import org.iotivity.cloud.util.Log;

import java.io.IOException;
import java.security.PublicKey;


public class CSRParser {

    private static final String COUNTRY = "2.5.4.6";

    private static final String ORGANIZATION = "2.5.4.10";

    private static final String ORGANIZATION_UNIT = "2.5.4.11";

    private static final String COMMON_NAME = "2.5.4.3";

    private final PKCS10CertificationRequest mCsr;

    /**
     * Creates CSRParser instance with specified csrDer byte array.
     *
     * @param csrDer specified byte array.
     */
    public CSRParser(byte[] csrDer) throws IOException {
        mCsr = new PKCS10CertificationRequest(csrDer);
    }

    /**
     * Returns public key
     */
    public PublicKey getPublicKey() throws PEMException {
        return new JcaPEMKeyConverter().setProvider(CertificateStorage.BC).getPublicKey(getPublicKeyInfo());
    }

    /**
     * Verifies signature from this CSR request with help of
     *
     * @return true if signature is correct and false otherwise.
     */
    public boolean verify() {
        boolean condition = false;
        try {
            condition = mCsr.isSignatureValid(new JcaContentVerifierProviderBuilder()
                    .setProvider(CertificateStorage.BC).build(getPublicKeyInfo()));
        } catch (OperatorCreationException | PKCSException e) {
            Log.e(e.getMessage());
        }
        return condition;
    }

    /**
     * Returns Organization Unit from csr subject.
     */
    public String getOrganizationalUnit() {
        return getX500Field(ORGANIZATION_UNIT, mCsr.getSubject());
    }

    /**
     * Returns organization from csr subject.
     */
    public String getOrganizational() {
        return getX500Field(ORGANIZATION, mCsr.getSubject());
    }

    /**
     * Returns country from csr subject.
     */
    public String getCountry() {
        return getX500Field(COUNTRY, mCsr.getSubject());
    }

    /**
     * Returns common name from csr subject.
     */
    public String getCommonName() {
        return getX500Field(COMMON_NAME, mCsr.getSubject());
    }

    /**
     * Returns public key information for this csr.
     */
    public SubjectPublicKeyInfo getPublicKeyInfo() {
        return mCsr.getSubjectPublicKeyInfo();
    }

    /**
     * Converts asn1.0 identifier ti string DATE_FORMAT.
     *
     * @param asn1ObjectIdentifier
     * @param x500Name
     * @return
     */
    private String getX500Field(String asn1ObjectIdentifier, X500Name x500Name) {
        String result = null;
        RDN[] arRdns = x500Name.getRDNs(new ASN1ObjectIdentifier(asn1ObjectIdentifier));
        if (arRdns.length != 0) {
            result = arRdns[0].getFirst().getValue().toString();
        }
        return result;
    }
}
