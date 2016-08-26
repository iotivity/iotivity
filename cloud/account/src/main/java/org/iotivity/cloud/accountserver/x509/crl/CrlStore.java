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

import org.iotivity.cloud.accountserver.resources.credprov.cert.CertificateStorage;
import org.iotivity.cloud.util.Log;

import java.io.*;
import java.text.MessageFormat;
import java.util.Date;

public final class CrlStore {

    private CrlStore() {
        throw new AssertionError();
    }

    private static final File CRL_FILE = new File(MessageFormat.format(CertificateStorage.PROPERTIES.getProperty("crlLocation"), File.separator));

    public static void save(byte[] crl) {
        try {
            FileOutputStream out = new FileOutputStream(CRL_FILE);
            out.write(crl);
            out.close();
        } catch (IOException e) {
            Log.e(e.getMessage());
        }
    }

    public static byte[] load() {
        try {
            InputStream f = new FileInputStream(CRL_FILE);
            int size = f.available();
            byte data[] = new byte[size];
            if (f.read(data) != data.length) {
                System.err.println("couldn't read crl");
            }
            f.close();
            return data;

        } catch (java.io.IOException e) {
            Log.e(e.getMessage());
        }
        return null;
    }

    public static boolean checkLastUpdate(Date lastUpdate) {
        boolean result = false;
        try {
            if (CRL_FILE.isFile()) {
                Date current = new Date(CRL_FILE.lastModified());
                result = current.after(lastUpdate);
            }
        } catch (Exception e) {
            Log.e(e.getMessage());
        }
        return result;
    }
}