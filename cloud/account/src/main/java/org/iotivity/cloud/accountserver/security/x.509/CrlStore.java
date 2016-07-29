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
import org.bouncycastle.cert.X509CRLHolder;

public final class CrlStore {

    private CrlStore() {
        throw new AssertionError(); //to get rid of security issue, connected with Java Reflection API
    }

    private static final String CRLFILENAME = "crl.txt";

    public static void saveCrl(X509CRLHolder crl) {
        //TODO: implement Java KeyStore
    }

    public static X509CRLHolder loadCrl() {
        //TODO: implement Java KeyStore
        return null;
    }
}
