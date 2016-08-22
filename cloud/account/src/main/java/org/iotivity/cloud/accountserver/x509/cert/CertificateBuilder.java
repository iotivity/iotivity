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

import java.math.BigInteger;
import java.security.GeneralSecurityException;
import java.security.KeyPair;
import java.security.KeyPairGenerator;
import java.security.PrivateKey;
import java.security.PublicKey;
import java.security.SecureRandom;
import java.security.Security;
import java.security.cert.X509Certificate;
import java.util.Date;

import org.bouncycastle.asn1.DEROctetString;
import org.bouncycastle.asn1.x500.RDN;
import org.bouncycastle.asn1.x500.X500Name;
import org.bouncycastle.asn1.x500.X500NameBuilder;
import org.bouncycastle.asn1.x500.style.BCStyle;
import org.bouncycastle.asn1.x500.style.IETFUtils;
import org.bouncycastle.asn1.x509.Extension;
import org.bouncycastle.cert.CertIOException;
import org.bouncycastle.cert.X509v3CertificateBuilder;
import org.bouncycastle.cert.jcajce.JcaX509CertificateConverter;
import org.bouncycastle.cert.jcajce.JcaX509v3CertificateBuilder;
import org.bouncycastle.jce.ECNamedCurveTable;
import org.bouncycastle.jce.provider.BouncyCastleProvider;
import org.bouncycastle.jce.spec.ECParameterSpec;
import org.bouncycastle.operator.ContentSigner;
import org.bouncycastle.operator.OperatorCreationException;
import org.bouncycastle.operator.jcajce.JcaContentSignerBuilder;

public class CertificateBuilder {
    private String              mIssuer;
    private String              mSubjectCN;
    private String              mSubjectC;
    private String              mSubjectO;
    private String              mSubjectOU;
    private String              mSubjectAltName;
    private Date                mNotBefore;
    private Date                mNotAfter;
    private PrivateKey          mPrivKey;
    private PublicKey           mPubKey;
    private BigInteger          mSerial;
    private static final String BC                      = BouncyCastleProvider.PROVIDER_NAME;
    private static final String SIGNATURE_ALGORITHM     = "SHA256withECDSA";
    private static final String CURVE                   = "secp256r1";
    private static final String KEY_GENERATOR_ALGORITHM = "ECDSA";

    public CertificateBuilder(String subject, Date notBefore, Date notAfter,
            BigInteger serial) {
        Security.addProvider(new BouncyCastleProvider());
        init(subject, null, notBefore, notAfter, null, null, serial);
    }

    public CertificateBuilder(String subject, PublicKey pubKey, Date notBefore,
            Date notAfter, BigInteger serial, CertificatePrivateKeyPair root) {
        X500Name x500name = new X500Name(
                root.getCertificate().getSubjectX500Principal().getName());
        RDN cn = x500name.getRDNs(BCStyle.CN)[0];
        init(subject, IETFUtils.valueToString(cn.getFirst().getValue()),
                notBefore, notAfter, root.getKey(), pubKey, serial);
    }

    public CertificateBuilder(String subject, String issuer, Date notBefore,
            Date notAfter, PrivateKey privKey, PublicKey pubKey,
            BigInteger serial) {
        Security.addProvider(new BouncyCastleProvider());
        init(subject, issuer, notBefore, notAfter, privKey, pubKey, serial);
    }

    private void init(String subject, String issuer, Date notBefore,
            Date notAfter, PrivateKey privKey, PublicKey pubKey,
            BigInteger serial) {
        this.mSubjectCN = subject;
        this.mIssuer = issuer;
        this.mNotBefore = notBefore;
        this.mNotAfter = notAfter;
        this.mPrivKey = privKey;
        this.mPubKey = pubKey;
        this.mSerial = serial;
    }

    public void setSubjectC(String subjectC) {
        this.mSubjectC = subjectC;
    }

    public void setSubjectO(String subjectO) {
        this.mSubjectO = subjectO;
    }

    public void setSubjectOU(String subjectOU) {
        this.mSubjectOU = subjectOU;
    }

    public void setSubjectAltName(String subjectAltName) {
        this.mSubjectAltName = subjectAltName;
    }

    public CertificatePrivateKeyPair build() throws GeneralSecurityException,
            OperatorCreationException, CertIOException {
        if (null == mPrivKey && null == mPubKey) {
            ECParameterSpec ecSpec = ECNamedCurveTable.getParameterSpec(CURVE);
            KeyPairGenerator g = null;

            g = KeyPairGenerator.getInstance(KEY_GENERATOR_ALGORITHM, BC);
            g.initialize(ecSpec, new SecureRandom());

            KeyPair pair = g.generateKeyPair();
            mPrivKey = pair.getPrivate();
            mPubKey = pair.getPublic();
            mIssuer = mSubjectCN;
        }

        X509Certificate cert = null;
        X500NameBuilder subjectNameBld = new X500NameBuilder(BCStyle.INSTANCE);

        subjectNameBld.addRDN(BCStyle.CN, mSubjectCN);

        if (null != mSubjectOU) {
            subjectNameBld.addRDN(BCStyle.OU, mSubjectOU);
        }

        if (null != mSubjectO) {
            subjectNameBld.addRDN(BCStyle.O, mSubjectO);
        }

        if (null != mSubjectC) {
            subjectNameBld.addRDN(BCStyle.C, mSubjectC);
        }

        X500NameBuilder issuerNameBld = new X500NameBuilder(BCStyle.INSTANCE);
        issuerNameBld.addRDN(BCStyle.CN, mIssuer);

        if (mPrivKey == null || mPubKey == null)
            throw new CertIOException("mPrivKey or mPubKey is null!");

        ContentSigner sigGen = new JcaContentSignerBuilder(SIGNATURE_ALGORITHM)
                .setProvider(BC).build(mPrivKey);

        X509v3CertificateBuilder certGen = new JcaX509v3CertificateBuilder(
                issuerNameBld.build(), mSerial, mNotBefore, mNotAfter,
                subjectNameBld.build(), mPubKey);

        if (null != mSubjectAltName) {
            certGen.addExtension(Extension.subjectAlternativeName, false,
                    new DEROctetString(mSubjectAltName.getBytes()));
        }

        cert = new JcaX509CertificateConverter().setProvider("BC")
                .getCertificate(certGen.build(sigGen));

        return new CertificatePrivateKeyPair(cert, mPrivKey);
    }

}