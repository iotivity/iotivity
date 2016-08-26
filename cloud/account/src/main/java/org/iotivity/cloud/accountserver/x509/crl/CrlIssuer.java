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
package org.iotivity.cloud.accountserver.x509.crl;

import org.bouncycastle.asn1.x500.X500Name;
import org.bouncycastle.cert.X509CRLEntryHolder;
import org.bouncycastle.cert.X509v2CRLBuilder;
import org.bouncycastle.operator.ContentSigner;
import org.bouncycastle.operator.OperatorCreationException;
import org.bouncycastle.operator.jcajce.JcaContentSignerBuilder;
import org.iotivity.cloud.accountserver.resources.credprov.cert.CertificateStorage;
import org.iotivity.cloud.accountserver.x509.cert.CertificateBuilder;
import org.iotivity.cloud.util.Log;

import java.io.IOException;
import java.math.BigInteger;
import java.security.PrivateKey;
import java.text.ParseException;
import java.util.Collection;
import java.util.Collections;
import java.util.Date;

import static org.iotivity.cloud.accountserver.x509.crl.CrlParser.DATE_FORMAT;

public final class CrlIssuer {

    private CrlIssuer() {
        throw new AssertionError();
    }

    public static byte[] generate(Date thisUpdate, Date nextUpdate,
                                  PrivateKey key, BigInteger[] serialNumbers, Collection<X509CRLEntryHolder> certs, String issuer) {
        X500Name issuerDN = new X500Name(issuer);
        X509v2CRLBuilder crlBuilder = new X509v2CRLBuilder(issuerDN,
                thisUpdate);
        if (nextUpdate != null) {
            crlBuilder.setNextUpdate(nextUpdate);
        }
        for (X509CRLEntryHolder crlItem : certs) {
            crlBuilder.addCRLEntry(crlItem.getSerialNumber(), crlItem.getRevocationDate(), 0);
        }
        for (BigInteger serialNumber : serialNumbers) {
            crlBuilder.addCRLEntry(serialNumber, new Date(), 0);
        }
        byte[] data = null;
        try {
            ContentSigner signer = new JcaContentSignerBuilder(CertificateBuilder.SIGNATURE_ALGORITHM)
                    .setProvider(CertificateStorage.BC).build(key);
            data = crlBuilder.build(signer).getEncoded();
            CrlStore.save(data);
        } catch (OperatorCreationException | IOException e) {
            Log.e(e.getMessage());
        }
        return data;
    }

    public static byte[] generate(String issuerName, Date thisUpdate, Date nextUpdate,
                                  PrivateKey key) {
        return generate(thisUpdate, nextUpdate, key, new BigInteger[]{}, Collections.EMPTY_LIST, issuerName);

    }

    public static void update(byte[] crl) {
        CrlStore.save(crl);
    }

    public static void revokeCertificate(BigInteger serialNumber, PrivateKey key) {
        byte[] data = CrlStore.load();
        CrlParser parser = new CrlParser(data);
        Collection<X509CRLEntryHolder> certificates = parser.getCerts();
        X509v2CRLBuilder crlBuilder = new X509v2CRLBuilder(new X500Name(parser.getIssuer()),
                parser.getThisUpdate());
        crlBuilder.setNextUpdate(parser.getNextUpdate());
        for (X509CRLEntryHolder entryHolder : certificates) {
            crlBuilder.addCRLEntry(entryHolder.getSerialNumber(), entryHolder.getRevocationDate(), 0);
        }
        crlBuilder.addCRLEntry(serialNumber, new Date(), 0);
        try {
            ContentSigner signer = new JcaContentSignerBuilder(CertificateBuilder.SIGNATURE_ALGORITHM)
                    .setProvider(CertificateStorage.BC).build(key);
            CrlStore.save(crlBuilder.build(signer).getEncoded());
        } catch (OperatorCreationException | IOException e) {
            Log.e(e.getMessage());
        }
    }

    public static void update(Date thisUpdate, Date nextUpdate,
                              String serialNumber, PrivateKey key) {
        String[] serials = serialNumber.split(",");
        BigInteger[] serialNumbers = new BigInteger[serials.length];
        for (int i = 0; i < serials.length; i++) {
            serialNumbers[i] = new BigInteger(serials[i]);
        }
        CrlParser crlParser = new CrlParser(CrlStore.load());
        generate(thisUpdate, nextUpdate, key, serialNumbers, crlParser.getCerts(), crlParser.getIssuer());
    }
}

