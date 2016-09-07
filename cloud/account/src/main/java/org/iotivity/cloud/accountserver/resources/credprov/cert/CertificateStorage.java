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
package org.iotivity.cloud.accountserver.resources.credprov.cert;

import org.bouncycastle.asn1.x509.BasicConstraints;
import org.bouncycastle.asn1.x509.Extension;
import org.bouncycastle.cert.CertIOException;
import org.bouncycastle.jce.ECNamedCurveTable;
import org.bouncycastle.jce.provider.BouncyCastleProvider;
import org.bouncycastle.operator.OperatorCreationException;
import org.iotivity.cloud.accountserver.x509.cert.CertificateBuilder;
import org.iotivity.cloud.accountserver.x509.cert.CertificateExtension;

import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.security.*;
import java.security.cert.Certificate;
import java.security.cert.X509Certificate;

import static org.iotivity.cloud.accountserver.resources.credprov.cert.CertificateConstants.*;

public final class CertificateStorage {

    /**
     * This attribute is used to get password to kestore, that stores CA certificate info.
     * Private key and certificate.
     */
    private static final String PASSWORD = PROPERTIES.getProperty("password");

    /**
     * Keystore object for save, get data from keystore.
     */
    private static KeyStore keyStore;

    public static PrivateKey ROOT_PRIVATE_KEY;

    public static X509Certificate ROOT_CERTIFICATE;

    private CertificateStorage() {
        throw new AssertionError();
    }

    /**
     * Init KeyStore. If it does not exists, create it and push to KEYSTORE_FILE.
     */
    static void init() throws GeneralSecurityException, IOException, OperatorCreationException {
        Files.createDirectories(Paths.get("keystore"));
        keyStore = load(null, null);
        store();
        CertificateStorage.generateCACertificate();
        CertificateStorage.saveCertificatePrivateKey();
    }

    /**
     * Load KeyStore with default keystore file and password.
     *
     * @return KeyStore instance.
     */
    public static void load() throws GeneralSecurityException, IOException {
        keyStore = load(new FileInputStream(KEYSTORE_FILE), PASSWORD.toCharArray());
        initRoot();
    }

    /**
     * Loads KeyStore with defined inputStream object and password.
     *
     * @param is       specified inputStream which contains keystore bytes.
     * @param password specified password for opening keystore.
     * @return KeyStore instance.
     */
    private static KeyStore load(InputStream is, char[] password) throws IOException, GeneralSecurityException {
        KeyStore keyStore = KeyStore.getInstance(KEYSTORE_TYPE, BouncyCastleProvider.PROVIDER_NAME);
        keyStore.load(is, password);
        return keyStore;
    }


    /**
     * Stores keyStore to default file KEYSTORE_FILE with default password.
     */
    static void store() throws IOException, GeneralSecurityException {
        store(keyStore, new FileOutputStream(KEYSTORE_FILE), PASSWORD.toCharArray());
    }

    /**
     * Stores KeyStore to file outputstream with specifie password.
     *
     * @param keyStore
     */
    private static void store(KeyStore keyStore, FileOutputStream out, char[] password) throws GeneralSecurityException,
            IOException {
        keyStore.store(out, password);
        out.close();
    }

    /**
     * Generates X509Certificate  with PublicKey and PrivateKey
     *
     * @return certificate and private key
     */
    private static void generateCACertificate() throws GeneralSecurityException,
            OperatorCreationException, CertIOException {
        if (ROOT_PRIVATE_KEY == null) {
            KeyPairGenerator g = KeyPairGenerator.getInstance(KEY_GENERATOR_ALGORITHM, SECURITY_PROVIDER);
            g.initialize(ECNamedCurveTable.getParameterSpec(CURVE), new SecureRandom());
            KeyPair pair = g.generateKeyPair();
            ROOT_PRIVATE_KEY = pair.getPrivate();
            ROOT_CERTIFICATE = new CertificateBuilder(CA_ISSUER, pair.getPublic(),
                    new CertificateExtension(Extension.basicConstraints, false,
                            new BasicConstraints(true))).build();
        }
    }

    /**
     * Stores certificate and private key to keystore.
     */
    private static void saveCertificatePrivateKey() throws GeneralSecurityException, IOException {
        keyStore.setCertificateEntry(CA_ALIAS, ROOT_CERTIFICATE);
        keyStore.setKeyEntry(CA_ALIAS, ROOT_PRIVATE_KEY, PASSWORD.toCharArray(),
                new Certificate[]{ROOT_CERTIFICATE});
        store();
    }

    private static void initRoot() throws GeneralSecurityException {
        ROOT_PRIVATE_KEY = (PrivateKey) keyStore.getKey(CA_ALIAS, PASSWORD.toCharArray());
        ROOT_CERTIFICATE = (X509Certificate) keyStore.getCertificate(CA_ALIAS);
    }
}
