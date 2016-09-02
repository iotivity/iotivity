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

import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

public final class CrlStore {

    private CrlStore() {
        throw new AssertionError(); // to get rid of security issue, connected
                                    // with Java Reflection API
    }

    private static final String CRL_FILE_NAME = "crl";

    public static void saveCrl(byte[] crl) {

        FileOutputStream out = null;
        try {
            out = new FileOutputStream(CRL_FILE_NAME);
            out.write(crl);
        } catch (java.io.IOException e) {
            e.printStackTrace();
        }

        try {
            if (out != null)
                out.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static byte[] loadCrl() {

        InputStream f = null;
        try {
            f = new FileInputStream(CRL_FILE_NAME);
            int size = f.available();
            byte data[] = new byte[size];

            if (f.read(data) != data.length) {
                System.err.println("couldn't read crl");
            }
            f.close();
            return data;

        } catch (java.io.IOException e) {
            e.printStackTrace();
        }

        try {
            if (f != null)
                f.close();
        } catch (IOException e) {
            e.printStackTrace();
        }

        return null;
    }
}
