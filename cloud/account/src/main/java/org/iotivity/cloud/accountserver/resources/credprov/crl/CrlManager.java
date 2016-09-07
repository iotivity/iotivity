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

import org.bouncycastle.operator.OperatorCreationException;
import org.bson.types.Binary;
import org.iotivity.cloud.accountserver.Constants;
import org.iotivity.cloud.accountserver.db.CRLTable;
import org.iotivity.cloud.accountserver.util.TypeCastingManager;
import org.iotivity.cloud.base.exception.ServerException;
import org.iotivity.cloud.util.Log;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.security.cert.CRLException;
import java.security.cert.X509CRL;
import java.text.ParseException;
import java.util.*;

import static org.iotivity.cloud.accountserver.resources.credprov.cert.CertificateConstants.*;
import static org.iotivity.cloud.accountserver.x509.crl.CrlIssuer.CRL_ISSUER;

/**
 * Class is used for managing CRLs(creation, revoke, update)
 */
public final class CrlManager {

    /**
     * Static manager for CRLs.
     */
    public static CrlManager CRL_MANAGER;
    /**
     * Casting manager for working with CRLTable in mongo db
     */
    private static TypeCastingManager<CRLTable> castingManager = new TypeCastingManager<>();

    static {
        try {
            CRL_MANAGER = new CrlManager();
            CRL_MANAGER.init();
        } catch (CRLException | IOException | OperatorCreationException e) {
            Log.e(e.getMessage());
        }
    }

    /**
     * X509 CRL presentation.
     */
    private X509CRL x509CRL;

    private CrlManager() {
    }

    /**
     * Revokes specified serial numbers.
     *
     * @param serialNumber specified var args serial numbers from 0.
     */
    public void revoke(String... serialNumber) throws CRLException, IOException, OperatorCreationException {
        if (x509CRL != null) {
            update(x509CRL.getThisUpdate(),
                    CRL_ISSUER.generate(x509CRL.getThisUpdate(), x509CRL.getNextUpdate(),
                            x509CRL.getRevokedCertificates(), serialNumber));
        }
    }

    /**
     * Check if last update is before CRL this update.
     * @param lastUpdate specified last update;
     * @return true if before and false - otherwise.
     */
    boolean checkLastUpdate(String lastUpdate) {
        boolean checkCondition = false;
        try {
            if (x509CRL != null) {
                checkCondition = DATE_FORMAT.parse(lastUpdate).before(x509CRL.getThisUpdate());
            }
        } catch (ParseException e) {
            Log.e(e.getMessage());
        }
        return checkCondition;
    }

    /**
     * Returns response payload, including this update, next update, and CRL in DER encoding.
     */
    Map<String, Object> getPayload() throws ServerException.PreconditionFailedException, CRLException {
        if (x509CRL != null) {
            Map<String, Object> responsePayload = new HashMap<>();
            responsePayload.put(Constants.REQ_THIS_UPDATE, DATE_FORMAT.format(x509CRL.getThisUpdate()));
            responsePayload.put(Constants.REQ_NEXT_UPDATE, DATE_FORMAT.format(x509CRL.getNextUpdate()));
            responsePayload.put(Constants.REQ_CRL, new CRL(DER, x509CRL.getEncoded()));
            return responsePayload;
        }
        return Collections.emptyMap();
    }

    /**
     * Updates CRLTable with specified this update and binary CRL data.
     */
    void update(Date thisUpdate, byte[] data) throws CRLException {
        CRLTable crlTable = castingManager.convertMaptoObject(
                ACCOUNT_DB_MANAGER.selectRecord(Constants.CRL_TABLE, new HashMap<>()).get(0), new CRLTable());
        crlTable.setThisUpdate(thisUpdate);
        crlTable.setBinaryData(new Binary(data));
        ACCOUNT_DB_MANAGER.updateRecord(Constants.CRL_TABLE, castingManager.convertObjectToMap(crlTable));
        setX509CRL(data);
    }

    /**
     * Create CRL with default options;
     */
    private void init() throws CRLException, IOException, OperatorCreationException {
        Calendar calendar = Calendar.getInstance();
        Date thisUpdate = calendar.getTime();
        calendar.add(Calendar.DAY_OF_MONTH,
                Integer.parseInt(NEXT_UPDATE_INTERVAL));
        byte[] data = CRL_ISSUER.generate(thisUpdate, calendar.getTime(), Collections.emptyList());
        ACCOUNT_DB_MANAGER.insertRecord(Constants.CRL_TABLE,
                castingManager.convertObjectToMap(new CRLTable(thisUpdate, new Binary(data))));
        setX509CRL(data);
    }

    /**
     * Updates currect x509 CRL state by parsing specified data.
     */
    private void setX509CRL(byte[] data) throws CRLException {
        x509CRL = (X509CRL) CERTIFICATE_FACTORY.generateCRL(new ByteArrayInputStream(data));
    }

    /**
     * Static inner class for CBOR Crl presentation.
     */
    private static final class CRL {

        private final String encoding;

        private final byte[] data;

        public CRL(String encoding, byte[] data) {
            this.encoding = encoding;
            this.data = data;
        }

        public String getEncoding() {
            return encoding;
        }

        public byte[] getData() {
            return data;
        }
    }

}
