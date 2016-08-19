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

import java.io.BufferedReader;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.Reader;
import java.security.PublicKey;
import java.security.Security;

import org.bouncycastle.asn1.ASN1ObjectIdentifier;
import org.bouncycastle.asn1.x500.RDN;
import org.bouncycastle.asn1.x500.X500Name;
import org.bouncycastle.asn1.x509.SubjectPublicKeyInfo;
import org.bouncycastle.jce.provider.BouncyCastleProvider;
import org.bouncycastle.openssl.PEMException;
import org.bouncycastle.openssl.PEMParser;
import org.bouncycastle.openssl.jcajce.JcaPEMKeyConverter;
import org.bouncycastle.operator.OperatorCreationException;
import org.bouncycastle.operator.jcajce.JcaContentVerifierProviderBuilder;
import org.bouncycastle.pkcs.PKCS10CertificationRequest;
import org.bouncycastle.pkcs.PKCSException;


public class CSRParser {
    private PKCS10CertificationRequest mCsr;
    private static final String BC = BouncyCastleProvider.PROVIDER_NAME;
    private static final String COUNTRY = "2.5.4.6";
    private static final String STATE = "2.5.4.8";
    private static final String LOCALE = "2.5.4.7";
    private static final String ORGANIZATION = "2.5.4.10";
    private static final String ORGANIZATION_UNIT = "2.5.4.11";
    private static final String COMMON_NAME = "2.5.4.3";

    public CSRParser(String csrPem) throws IOException {
        Security.addProvider(new BouncyCastleProvider());
        mCsr = convertPemToPKCS10CertificationRequest(csrPem);
    }

    public CSRParser(byte[] csrDer) throws IOException {
        Security.addProvider(new BouncyCastleProvider());
        mCsr = new PKCS10CertificationRequest(csrDer);
    }

    public String getOrganizationalUnit() {
        return getX500Field(ORGANIZATION_UNIT, mCsr.getSubject());
    }

    public String getOrganizational() {
        return getX500Field(ORGANIZATION, mCsr.getSubject());
    }

    public String getCountry() {
        return getX500Field(COUNTRY, mCsr.getSubject());
    }

    public String getState() {
        return getX500Field(STATE, mCsr.getSubject());
    }

    public String getLocale() {
        return getX500Field(LOCALE, mCsr.getSubject());
    }

    public String getCommonName() {
        return getX500Field(COMMON_NAME, mCsr.getSubject());
    }

    public SubjectPublicKeyInfo getPublicKeyInfo() {
        return mCsr.getSubjectPublicKeyInfo();
    }

    public PublicKey getPublicKey() throws PEMException {
        JcaPEMKeyConverter converter = new JcaPEMKeyConverter().setProvider(BC);
        return converter.getPublicKey(getPublicKeyInfo());
    }

    public boolean verify() {
        SubjectPublicKeyInfo publicKeyInfo = mCsr.getSubjectPublicKeyInfo();
        try {
            return mCsr.isSignatureValid(new JcaContentVerifierProviderBuilder()
                    .setProvider(BC).build(publicKeyInfo));
        } catch (OperatorCreationException | PKCSException e) {
            return false;
        }
    }

    public boolean authenticate(String uuid) {
        return uuid.equals(this.getCommonName());
    }

    private PKCS10CertificationRequest convertPemToPKCS10CertificationRequest(String pem)
            throws IOException {
        Security.addProvider(new BouncyCastleProvider());
        ByteArrayInputStream pemStream = null;

        pemStream = new ByteArrayInputStream(pem.getBytes("UTF-8"));

        Reader pemReader = new BufferedReader(new InputStreamReader(pemStream));
        PEMParser pemParser = new PEMParser(pemReader);

        Object parsedObj = pemParser.readObject();

        if (parsedObj instanceof PKCS10CertificationRequest) {
            mCsr = (PKCS10CertificationRequest) parsedObj;
        }

        pemParser.close();
        mCsr.getEncoded();

        return mCsr;
    }

    private String getX500Field(String asn1ObjectIdentifier, X500Name x500Name) {
        RDN[] rdnArray = x500Name.getRDNs(new ASN1ObjectIdentifier(asn1ObjectIdentifier));
        String retVal = null;
        for (RDN item : rdnArray) {
            retVal = item.getFirst().getValue().toString();
        }
        return retVal;
    }

}
