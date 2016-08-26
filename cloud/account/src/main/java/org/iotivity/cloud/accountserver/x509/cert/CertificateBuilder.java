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

import org.bouncycastle.asn1.ASN1Encodable;
import org.bouncycastle.asn1.DERSequence;
import org.bouncycastle.asn1.x500.RDN;
import org.bouncycastle.asn1.x500.X500Name;
import org.bouncycastle.asn1.x500.X500NameBuilder;
import org.bouncycastle.asn1.x500.style.IETFUtils;
import org.bouncycastle.asn1.x509.Extension;
import org.bouncycastle.asn1.x509.GeneralName;
import org.bouncycastle.cert.CertIOException;
import org.bouncycastle.cert.X509v3CertificateBuilder;
import org.bouncycastle.cert.jcajce.JcaX509CertificateConverter;
import org.bouncycastle.cert.jcajce.JcaX509v3CertificateBuilder;
import org.bouncycastle.jce.ECNamedCurveTable;
import org.bouncycastle.jce.spec.ECParameterSpec;
import org.bouncycastle.operator.OperatorCreationException;
import org.bouncycastle.operator.jcajce.JcaContentSignerBuilder;
import org.iotivity.cloud.accountserver.resources.credprov.cert.CertificateStorage;

import java.math.BigInteger;
import java.security.*;
import java.security.cert.X509Certificate;
import java.util.Date;

import static org.bouncycastle.asn1.x500.style.BCStyle.*;

public class CertificateBuilder {

    private String issuerName;

    private String subjectCN;

    private String subjectC;

    private String subjectO;

    private String subjectOU;

    private String subjectAltName;

    private Date notBefore;

    private Date notAfter;

    private PrivateKey privateKey;

    private PublicKey publicKey;

    private BigInteger serial;

    public static final String SIGNATURE_ALGORITHM = CertificateStorage.PROPERTIES.getProperty("signatureAlgorithm");

    private static final String CURVE = CertificateStorage.PROPERTIES.getProperty("ellipticCurve");

    private static final String KEY_GENERATOR_ALGORITHM = CertificateStorage.PROPERTIES.getProperty("keyGeneratorAlgorithm");

    private final X500NameBuilder subjectNameBld = new X500NameBuilder(INSTANCE);

    public CertificateBuilder(String subject, Date notBefore, Date notAfter, BigInteger serial) {
        init(subject, null, notBefore, notAfter, null, null, serial);
    }

    public CertificateBuilder(String subject, PublicKey pubKey, Date notBefore, Date notAfter,
                              BigInteger serial, CertificatePrivateKeyPair root) {
        X500Name x500name = new X500Name(root.getX509Certificate().getSubjectX500Principal().getName());
        RDN cn = x500name.getRDNs(CN)[0];
        init(subject, IETFUtils.valueToString(cn.getFirst().getValue()), notBefore, notAfter, root.getPrivateKey(), pubKey, serial);
    }

    private void init(String subject, String issuer, Date notBefore, Date notAfter,
                      PrivateKey privKey, PublicKey pubKey, BigInteger serial) {
        this.subjectCN = subject;
        this.issuerName = issuer;
        this.notBefore = notBefore;
        this.notAfter = notAfter;
        this.privateKey = privKey;
        this.publicKey = pubKey;
        this.serial = serial;
        subjectNameBld.addRDN(CN, subjectCN);
    }

    public void setSubjectC(String subjectC) {
        this.subjectC = subjectC;
        subjectNameBld.addRDN(C, subjectC);
    }

    public void setSubjectO(String subjectO) {
        this.subjectO = subjectO;
        subjectNameBld.addRDN(O, subjectO);
    }

    public void setSubjectOU(String subjectOU) {
        this.subjectOU = subjectOU;
        subjectNameBld.addRDN(OU, subjectOU);
    }

    public void setSubjectAltName(String subjectAltName) {
        this.subjectAltName = subjectAltName;
    }

    public CertificatePrivateKeyPair build()
            throws GeneralSecurityException, OperatorCreationException, CertIOException {
        if (privateKey == null && publicKey == null) {
            ECParameterSpec ecSpec = ECNamedCurveTable.getParameterSpec(CURVE);
            KeyPairGenerator g = KeyPairGenerator.getInstance(KEY_GENERATOR_ALGORITHM, CertificateStorage.BC);
            g.initialize(ecSpec, new SecureRandom());
            KeyPair pair = g.generateKeyPair();
            privateKey = pair.getPrivate();
            publicKey = pair.getPublic();
            issuerName = subjectCN;
        }
        X509v3CertificateBuilder certGen = new JcaX509v3CertificateBuilder(
                new X500NameBuilder(INSTANCE).addRDN(CN, issuerName).build(),
                serial, notBefore, notAfter, subjectNameBld.build(), publicKey);

        if (subjectAltName != null) {
            ASN1Encodable[] subjectAlternativeNames = new ASN1Encodable[]
                    {
                            new GeneralName(GeneralName.dNSName, subjectAltName)
                    };
            DERSequence subjectAlternativeNamesExtension = new DERSequence(subjectAlternativeNames);
            certGen.addExtension(Extension.subjectAlternativeName, false,
                    subjectAlternativeNamesExtension);
        }
        X509Certificate cert = new JcaX509CertificateConverter().setProvider(CertificateStorage.BC).
                getCertificate(certGen.build(
                        new JcaContentSignerBuilder(SIGNATURE_ALGORITHM).setProvider(CertificateStorage.BC).build(privateKey)));

        return new CertificatePrivateKeyPair(cert, privateKey);
    }

}