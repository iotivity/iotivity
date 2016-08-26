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

import org.bouncycastle.cert.X509CRLEntryHolder;
import org.bouncycastle.cert.X509CRLHolder;
import org.iotivity.cloud.util.Log;

import java.io.IOException;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Collection;
import java.util.Date;

public class CrlParser {

    private X509CRLHolder mCrl;

    public static final DateFormat DATE_FORMAT = new SimpleDateFormat("yyyyMMddHHmmss");


    public CrlParser(byte[] crl) {
        try {
            mCrl = new X509CRLHolder(crl);
        } catch (IOException e) {
            Log.e(e.getMessage());
        }
    }

    public Date getThisUpdate() {
        return mCrl.toASN1Structure().getThisUpdate().getDate();
    }

    public Date getNextUpdate() {
        return mCrl.toASN1Structure().getNextUpdate().getDate();
    }

    public String getIssuer() {
        return mCrl.getIssuer().toString();
    }

    @SuppressWarnings("unchecked")
    public Collection<X509CRLEntryHolder> getCerts() {
        return mCrl.getRevokedCertificates();
    }
}
