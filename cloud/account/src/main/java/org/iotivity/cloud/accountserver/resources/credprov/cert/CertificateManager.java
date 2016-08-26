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

import org.iotivity.cloud.accountserver.Constants;
import org.iotivity.cloud.accountserver.db.AccountDBManager;
import org.iotivity.cloud.accountserver.db.CertificateTable;
import org.iotivity.cloud.accountserver.util.TypeCastingManager;

import java.util.*;

public final class CertificateManager {
    private final Map<String, Object> payLoad;

    private TypeCastingManager<CertificateTable> castingManager = new TypeCastingManager<>();

    public Map<String, Object> getPayLoad() {
        return payLoad;
    }

    public void putCert(String encoding, byte[] cert) {
        if (encoding.equals(CertificateConstants.BASE_64)){
            cert = org.bouncycastle.util.encoders.Base64.encode(cert);
        }
        payLoad.put(Constants.CERT_ENCODING, encoding);
        payLoad.put(Constants.CERT_DATA, cert);

    }

    public void putCACert(String encoding, byte[] cacert) {
        if (encoding.equals(CertificateConstants.BASE_64)){
            cacert = org.bouncycastle.util.encoders.Base64.encode(cacert);
        }
        payLoad.put(Constants.CERTCHAIN_ENCODING, encoding);
        payLoad.put(Constants.CERTCHAIN_DATA, cacert);
    }

    public void putDeviceId(String di) {
        payLoad.put(Constants.RESP_DEVICE_ID, di);
    }

    public CertificateManager() {
        payLoad = new HashMap<>();
    }

    public void createCertificate(String serialNumber, Date notAfter, Date notBefore,
                                  String deviceId, String uid) {
        CertificateTable certificateTable = new CertificateTable(serialNumber,
                notAfter, notBefore, deviceId, uid, false);
        AccountDBManager.getInstance().insertRecord(Constants.CERTIFICATE_TABLE,
                castingManager.convertObjectToMap(certificateTable));

    }

    public void updateCertificate(String serialNumber, boolean revoked) {
        CertificateTable certificateTable = getCertificateTable(serialNumber);
        certificateTable.setRevoked(revoked);
        AccountDBManager.getInstance().updateRecord(Constants.CERTIFICATE_TABLE,
                castingManager.convertObjectToMap(certificateTable));

    }


    private CertificateTable getCertificateTable(String serialNumber) {
        CertificateTable certificateTable = new CertificateTable();
        HashMap<String, Object> condition = new HashMap<>();
        condition.put(Constants.KEYFIELD_SN, serialNumber);
        certificateTable = castingManager
                .convertMaptoObject(
                        AccountDBManager.getInstance().selectRecord(
                                Constants.CERTIFICATE_TABLE, condition).get(0),
                        certificateTable);
        return certificateTable;
    }


    public String getUserID(String deviceId) {
        HashMap<String, Object> condition = new HashMap<>();
        condition.put(Constants.KEYFIELD_DID, deviceId);
        ArrayList<HashMap<String, Object>> recordList = AccountDBManager
                .getInstance().selectRecord(Constants.TOKEN_TABLE, condition);
        Iterator<HashMap<String, Object>> iterator = recordList.iterator();
        String result = null;
        if (iterator.hasNext()) {
            result = iterator.next().get(Constants.KEYFIELD_UUID).toString();
        }
        return result;
    }
}
