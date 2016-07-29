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
import org.bouncycastle.asn1.x500.X500Name;
import org.bouncycastle.cert.X509CRLHolder;
import org.bouncycastle.cert.X509v2CRLBuilder;
import org.bouncycastle.jce.provider.BouncyCastleProvider;
import org.bouncycastle.operator.ContentSigner;
import org.bouncycastle.operator.jcajce.JcaContentSignerBuilder;

import java.math.BigInteger;
import java.security.*;
import java.security.spec.PKCS8EncodedKeySpec;
import java.util.Arrays;
import java.util.Date;

import java.security.spec.ECGenParameterSpec;

public final class CrlIssuer {

    private static final String BC = org.bouncycastle.jce.provider.BouncyCastleProvider.PROVIDER_NAME;
    private static final String SIGNING_ALGORITHM = "SHA256withECDSA";

    private CrlIssuer() {
        throw new AssertionError();//to get rid of security issue, connected with Java Reflection API
    }

    static {
        Security.insertProviderAt(new BouncyCastleProvider(), 1);
    }

    public static byte[] generateCrl(String issuerName,
                              Date thisUpdate,
                              CrlInfo[] items,
                              byte[] issuerPrivateKey) throws Exception {

        X500Name issuerDN = new X500Name(issuerName);
        X509v2CRLBuilder crlBuilder = new X509v2CRLBuilder(issuerDN, thisUpdate);

        for (CrlInfo item: items) {
            crlBuilder.addCRLEntry(item.getSerialNumber(), item.getRevocationDate(), 0);
        }

        KeyFactory kf = KeyFactory.getInstance("ECDSA");
        PrivateKey privateKey = kf.generatePrivate(new PKCS8EncodedKeySpec(issuerPrivateKey));

        // build and sign CRL with CA private key
        ContentSigner signer = new JcaContentSignerBuilder(SIGNING_ALGORITHM).setProvider(BC).build(privateKey);
        X509CRLHolder crl = crlBuilder.build(signer);

        CrlStore.saveCrl(crl);

        return crl.getEncoded();
    }

    public static byte[] getCrl() throws Exception {
        X509CRLHolder crl = CrlStore.loadCrl();
        return crl.getEncoded();
    }

    public static void main(String[] args) {
        System.out.println("Start!");

        /* generate dummy crl items */
        CrlInfo[] items = new CrlInfo[2];
        int val = 1024;
        for (int i = 0; i < items.length; i++) {
            items[i] = new CrlInfo();
            items[i].setSerialNumber(BigInteger.valueOf(val++));
            items[i].setRevocationDate(new Date());
        }

        ECGenParameterSpec ecGenSpec = new ECGenParameterSpec("prime192v1");
        try {
            KeyPairGenerator g = KeyPairGenerator.getInstance("ECDSA", "BC");
            g.initialize(ecGenSpec, new SecureRandom());
            KeyPair pair = g.generateKeyPair();

            PrivateKey key = pair.getPrivate();
            byte[] crl = generateCrl("C=DE,O=Samsung", new Date(), items, key.getEncoded());

            System.out.println("Success!");
            System.out.println("Stored CRL = " + getHex(crl));
        }
        catch (java.lang.Exception e)
        {
            e.printStackTrace();
        }
        System.out.println("End!");
    }

    static final String HEXES = "0123456789ABCDEF";
    public static String getHex( byte [] raw ) {
        if ( raw == null ) {
            return null;
        }
        final StringBuilder hex = new StringBuilder( 2 * raw.length );
        for ( final byte b : raw ) {
            hex.append(HEXES.charAt((b & 0xF0) >> 4))
                    .append(HEXES.charAt((b & 0x0F)));
        }
        return hex.toString();
    }
}
