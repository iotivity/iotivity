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

import org.bouncycastle.cert.CertIOException;
import org.bouncycastle.jce.provider.BouncyCastleProvider;
import org.bouncycastle.operator.OperatorCreationException;
import org.iotivity.cloud.accountserver.Constants;
import org.iotivity.cloud.accountserver.x509.cert.CertificateBuilder;
import org.iotivity.cloud.accountserver.x509.cert.CertificatePrivateKeyPair;
import org.iotivity.cloud.util.Log;

import java.io.*;
import java.math.BigInteger;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.security.*;
import java.security.cert.X509Certificate;
import java.text.MessageFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.Properties;

public class CertificateStorage {

    public static final Properties PROPERTIES = new Properties();

    static {
        try {
            PROPERTIES.load(new FileInputStream(Constants.PROPERTIES_FILE_NAME));
        } catch (IOException e) {
            Log.e(e.getMessage());
        }
    }

    public static final String BC = PROPERTIES.getProperty("securityProvider");

    private static final String PASSWORD = PROPERTIES.getProperty("password");

    public static final String DEVICE_OU = PROPERTIES.getProperty("deviceOU");

    public static final String DEVICE_O = PROPERTIES.getProperty("deviceO");

    public static final String DEVICE_C = PROPERTIES.getProperty("deviceC");

    public static final String KEYSTORE_TYPE = PROPERTIES.getProperty("keystoreType");

    public static final String CA_ALIAS = PROPERTIES.getProperty("caAlias");

    public static final String SUBJECT_NAME = PROPERTIES.getProperty("subjectName");

    private static final File KEYSTORE_FILE = new File(MessageFormat.format(PROPERTIES.getProperty("keyStoreLocation"), File.separator));

    private static BigInteger SERIAL_NUMBER = new BigInteger(PROPERTIES.getProperty("serialNumber"));

    private static KeyStore keyStore;

    /**
     * Insert BouncyCastleProvider into 0 position in security provider list.
     * Init KeyStore, Generate CA certificate and save it to keyStore.
     */
    static {
        if (!KEYSTORE_FILE.exists()) {
            try {
                Files.createDirectories(Paths.get("keystore"));
            } catch (IOException e) {
                Log.e(e.getMessage());
            }
        }
        CertificateStorage.init();
        CertificateStorage.load();
        CertificatePrivateKeyPair certificatePrivateKeyPair =
                CertificateStorage.generateCACertificate(DEVICE_C, DEVICE_O, SUBJECT_NAME, DEVICE_OU);
        CertificateStorage.saveCertificatePrivateKey(certificatePrivateKeyPair);
    }

    /**
     * Init KeyStore. If it does not exists, create it and push to KEYSTORE_FILE.
     */
    private static void init() {
        keyStore = load(null, null);
        store();
    }

    /**
     * Load KeyStore with default keystore file and password.
     *
     * @return KeyStore instance.
     */
    public static void load() {
        try {
            keyStore = load(new FileInputStream(KEYSTORE_FILE), PASSWORD.toCharArray());
        } catch (FileNotFoundException e) {
            Log.e(e.getMessage());
        }
    }

    /**
     * Loads KeyStore with defined inputStream object and password.
     *
     * @param is       specified inputStream which contains keystore bytes.
     * @param password specified password for opening keystore.
     * @return KeyStore instance.
     */
    private static KeyStore load(InputStream is, char[] password) {
        KeyStore keyStore = null;
        try {
            keyStore = KeyStore.getInstance(KEYSTORE_TYPE, BouncyCastleProvider.PROVIDER_NAME);
            keyStore.load(is, password);
        } catch (GeneralSecurityException | IOException e) {
            Log.e(e.getMessage());
        }
        return keyStore;
    }


    /**
     * Stores keyStore to default file KEYSTORE_FILE with default password.
     */
    public static void store() {
        try {
            store(keyStore, new FileOutputStream(KEYSTORE_FILE), PASSWORD.toCharArray());
        } catch (IOException e) {
            Log.e(e.getMessage());
        }
    }

    /**
     * Stores KeyStore to s
     *
     * @param keyStore
     */
    private static void store(KeyStore keyStore, FileOutputStream out, char[] password) {
        try {
            keyStore.store(out, password);
            out.close();
        } catch (GeneralSecurityException | IOException e) {
            Log.e(e.getMessage());
        }
    }

    /**
     * Generates X509Certificate  with PublicKey and PrivateKey
     *
     * @param country          specified country name
     * @param organization     specified organization name
     * @param subjectName      specified subject name.
     * @param organizationUnit specified organization unit.
     * @return certificate and private key as fields of CertificatePrivateKeyPair instance.
     */
    private static CertificatePrivateKeyPair generateCACertificate(String country, String organization,
                                                                   String subjectName, String organizationUnit) {
        CertificateBuilder rootBuilder = new CertificateBuilder(subjectName, getNotBeforeDate(),
                getNotAfterDate(), getNextSerialNumber());
        rootBuilder.setSubjectC(country);
        rootBuilder.setSubjectO(organization);
        rootBuilder.setSubjectOU(organizationUnit);
        CertificatePrivateKeyPair certificatePrivateKeyPair = null;
        try {
            certificatePrivateKeyPair = rootBuilder.build();
        } catch (GeneralSecurityException | OperatorCreationException | CertIOException e) {
            Log.e(e.getMessage());
        }
        return certificatePrivateKeyPair;
    }

    /**
     * Stores certificate and private key to keystore.
     *
     * @param certificatePrivateKeyPair specified certificate private key pair.
     */
    public static void saveCertificatePrivateKey(CertificatePrivateKeyPair certificatePrivateKeyPair) {
        try {
            X509Certificate certificate = certificatePrivateKeyPair.getX509Certificate();
            keyStore.setCertificateEntry(CA_ALIAS, certificate);
            keyStore.setKeyEntry(CA_ALIAS, certificatePrivateKeyPair.getPrivateKey(), PASSWORD.toCharArray(),
                    new java.security.cert.Certificate[]{certificate});
            store();
        } catch (GeneralSecurityException e) {
            Log.e(e.getMessage());
        }
    }

    public static void saveCertificate(java.security.cert.Certificate certificate, String subjectName) {
        try {
            keyStore.setCertificateEntry(subjectName, certificate);
            store();
        } catch (KeyStoreException e) {
            Log.e(e.getMessage());
        }
    }


    /**
     * Returns next serial number.
     */
    public static BigInteger getNextSerialNumber() {
        SERIAL_NUMBER = SERIAL_NUMBER.add(BigInteger.ONE);
        PROPERTIES.setProperty("serialNumber", SERIAL_NUMBER.toString());
        try {
            PROPERTIES.store(new FileOutputStream(Constants.PROPERTIES_FILE_NAME), "New Serial number");
        } catch (IOException e) {
            Log.e(e.getMessage());
        }
        return SERIAL_NUMBER;
    }

    /**
     * Returns date not before.
     */
    public static Date getNotBeforeDate() {
        return new Date(System.currentTimeMillis());
    }

    /**
     * Returns date not after.
     */
    public static Date getNotAfterDate() {
        Calendar calendar = Calendar.getInstance();
        calendar.set(Calendar.YEAR, calendar.get(Calendar.YEAR) +
                Integer.parseInt(PROPERTIES.getProperty("notAfterInterval")));
        return calendar.getTime();
    }

    /**
     * Returns CertificatePrivateKeyPair for signing user personal certificates.
     */
    public static CertificatePrivateKeyPair getRootCertificatePrivateKeyPair() {

        X509Certificate certificate = null;
        PrivateKey privateKey = null;
        try {
            certificate = (X509Certificate) keyStore.getCertificate(CA_ALIAS);
            privateKey = (PrivateKey) keyStore.getKey(CA_ALIAS, PASSWORD.toCharArray());
        } catch (KeyStoreException | NoSuchAlgorithmException | UnrecoverableKeyException e) {
            Log.e(e.getMessage());
        }
        return new CertificatePrivateKeyPair(certificate, privateKey);
    }

    /**
     * Returns CertificatePrivateKeyPair for signing user personal certificates.
     */
    public static X509Certificate getCertificate(String alias) {
        X509Certificate certificate = null;
        try {
            certificate = (X509Certificate) keyStore.getCertificate(alias);
        } catch (KeyStoreException e) {
            Log.e(e.getMessage());
        }
        return certificate;
    }
}
