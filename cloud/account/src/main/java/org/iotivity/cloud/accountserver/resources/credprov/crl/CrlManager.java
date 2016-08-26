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
package org.iotivity.cloud.accountserver.resources.credprov.crl;

import org.iotivity.cloud.accountserver.Constants;
import org.iotivity.cloud.accountserver.db.AccountDBManager;
import org.iotivity.cloud.accountserver.db.CRLTable;
import org.iotivity.cloud.accountserver.resources.credprov.cert.CertificateConstants;
import org.iotivity.cloud.accountserver.util.TypeCastingManager;
import org.iotivity.cloud.accountserver.x509.crl.CrlParser;
import org.iotivity.cloud.accountserver.x509.crl.CrlStore;
import org.iotivity.cloud.util.Log;

import java.text.ParseException;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;

import static java.util.Collections.EMPTY_MAP;
import static org.iotivity.cloud.accountserver.x509.crl.CrlParser.DATE_FORMAT;

public final class CrlManager {

    private static TypeCastingManager<CRLTable> castingManager = new TypeCastingManager<>();

    private CrlManager() {
        throw new AssertionError();
    }

    public static boolean checkLastUpdate(String lastUpdate) {
        boolean checkCondition = false;
        try {
            checkCondition = CrlStore.checkLastUpdate(DATE_FORMAT.parse(lastUpdate));
        } catch (ParseException e) {
            Log.e(e.getMessage());
        }
        return checkCondition;
    }

    public static Map<String, Object> getCrlInfo() {
        byte[] crl = CrlStore.load();
        if (crl == null) {
            return EMPTY_MAP;
        }
        CrlParser parser = new CrlParser(crl);
        Map<String, Object> responsePayload = new HashMap<>();
        responsePayload.put(Constants.REQ_THIS_UPDATE, DATE_FORMAT.format(parser.getThisUpdate()));
        responsePayload.put(Constants.REQ_NEXT_UPDATE, DATE_FORMAT.format(parser.getNextUpdate()));
        responsePayload.put(Constants.CRL_ENCODING, CertificateConstants.DER);
        responsePayload.put(Constants.CRL_DATA, crl);
        return responsePayload;
    }

    public static void createCRL(Date thisUpdate) {
        CRLTable crlTable = new CRLTable(thisUpdate);
        AccountDBManager.getInstance().insertRecord(Constants.CRL_TABLE,
                castingManager.convertObjectToMap(crlTable));
    }

}
