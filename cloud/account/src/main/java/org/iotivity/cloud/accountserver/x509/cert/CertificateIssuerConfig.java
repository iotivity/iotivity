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
import java.text.DateFormat;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

public class CertificateIssuerConfig {
    private BigInteger mSerial;
    private CertificatePrivateKeyPair mRootCertificate;
    private static volatile CertificateIssuerConfig mInstance;

    private CertificateIssuerConfig(){
        readConfig();
    }

    public static CertificateIssuerConfig getInstance() {
        if(mInstance == null) {
            synchronized (CertificateIssuerConfig.class) {
                if(mInstance == null) {
                    mInstance = new CertificateIssuerConfig();
                }
            }
        }
        return mInstance;
    }

    private void readConfig() {
        //TODO read from config db
        mSerial =  BigInteger.valueOf(0);
        String issuer = new String("uuid:31313131-3131-3131-3131-313131313131");
        CertificateBuilder rootBuilder = new CertificateBuilder(issuer, getNotBeforeDate(),
                getNotAfterDate(), getNextSerialNumber());
        rootBuilder.setSubjectC("Uk");
        rootBuilder.setSubjectO("Samsung");
        rootBuilder.setSubjectOU("OCF Root CA");

        try {
            mRootCertificate =  rootBuilder.build();
        } catch (Exception e) {
        }
    }

    public BigInteger getNextSerialNumber() {
        mSerial = mSerial.add(BigInteger.ONE);
        return mSerial;
    }

    public Date getNotBeforeDate() {
        return new Date(System.currentTimeMillis() - 50000);
    }

    public Date getNotAfterDate() {
        String string = "January 2, 2017";
        DateFormat format = new SimpleDateFormat("MMMM d, yyyy", Locale.ENGLISH);
        Date notAfter = null;
        try {
            notAfter = format.parse(string);
        } catch (ParseException e) {
            e.printStackTrace();
        }
        return notAfter;
    }

    public CertificatePrivateKeyPair getRootCertificate() {
        return mRootCertificate;
    }
}
