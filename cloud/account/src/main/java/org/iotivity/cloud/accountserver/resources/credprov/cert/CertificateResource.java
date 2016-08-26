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
import org.bouncycastle.util.encoders.Base64;
import org.iotivity.cloud.accountserver.Constants;
import org.iotivity.cloud.accountserver.x509.cert.CSRParser;
import org.iotivity.cloud.accountserver.x509.cert.CertificateBuilder;
import org.iotivity.cloud.accountserver.x509.cert.CertificatePrivateKeyPair;
import org.iotivity.cloud.accountserver.x509.crl.CrlIssuer;
import org.iotivity.cloud.base.device.Device;
import org.iotivity.cloud.base.exception.ServerException;
import org.iotivity.cloud.base.protocols.IRequest;
import org.iotivity.cloud.base.protocols.IResponse;
import org.iotivity.cloud.base.protocols.MessageBuilder;
import org.iotivity.cloud.base.protocols.enums.ContentFormat;
import org.iotivity.cloud.base.protocols.enums.ResponseStatus;
import org.iotivity.cloud.base.resource.Resource;
import org.iotivity.cloud.util.Cbor;
import org.iotivity.cloud.util.Log;

import java.io.IOException;
import java.math.BigInteger;
import java.security.GeneralSecurityException;
import java.security.PrivateKey;
import java.security.Security;
import java.security.cert.Certificate;
import java.security.cert.X509Certificate;
import java.util.Arrays;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;

import static org.iotivity.cloud.accountserver.Constants.*;
import static org.iotivity.cloud.accountserver.resources.credprov.cert.CertificateStorage.*;


public class CertificateResource extends Resource {

    private static final Cbor<Map<String, Object>> MAP_CBOR = new Cbor<>();

    public CertificateResource() {
        super(Arrays.asList(PREFIX_OIC, CREDPROV_URI, CERT_URI));
    }

    static {
        Security.insertProviderAt(new BouncyCastleProvider(), 0);
    }

    @Override
    public void onDefaultRequestReceived(Device srcDevice, IRequest request)
            throws ServerException {
        IResponse response;
        switch (request.getMethod()) {
            case POST:
                response = handlePostRequest(request);
                break;
            default:
                response = MessageBuilder.createResponse(request, ResponseStatus.METHOD_NOT_ALLOWED);
        }
        srcDevice.sendResponse(response);
    }

    private IResponse handlePostRequest(IRequest request)
            throws ServerException {
        Map<String, Object> payloadData = MAP_CBOR
                .parsePayloadFromCbor(request.getPayload(), HashMap.class);
        IResponse response = MessageBuilder.createResponse(request, ResponseStatus.BAD_REQUEST);
        if (payloadData.containsKey(CSR_ENCODING)) {
            String encoding = (String) payloadData.get(CSR_ENCODING);
            byte[] data = (byte[]) payloadData.get(CSR_DATA);
            if (data != null) {
                if (encoding.equals(CertificateConstants.BASE_64)) {
                    data = Base64.decode(data);
                }
                try {
                    CSRParser parser = new CSRParser(data);
                    String commonName = parser.getCommonName();
                    String[] split = commonName.split(":");
                    String deviceId = (String) payloadData.get(RESP_DEVICE_ID);
                    if (split[0].equals(KEYFIELD_UUID) && split[1].equals(deviceId) && parser.verify()) {
                        CertificatePrivateKeyPair privateKeyPair = getRootCertificatePrivateKeyPair();
                        X509Certificate x509Certificate = getCertificate(deviceId);
                        CertificateManager certificateManager = new CertificateManager();
                        BigInteger serialNumber = getNextSerialNumber();
                        if (x509Certificate != null) {
                            PrivateKey privateKey = privateKeyPair.getPrivateKey();
                            CrlIssuer.revokeCertificate(x509Certificate.getSerialNumber(), privateKey);
                            certificateManager.updateCertificate(x509Certificate.getSerialNumber().toString(), true);
                        }
                        Date notBefore = getNotBeforeDate();
                        Date notAfter = getNotAfterDate();
                        CertificateBuilder certBuilder = new CertificateBuilder(
                                commonName, parser.getPublicKey(),
                                notBefore, notAfter,
                                serialNumber, privateKeyPair);
                        checkSubjectName(parser, certBuilder);
                        try {
                            certificateManager.putDeviceId((String) payloadData.get(RESP_DEVICE_ID));
                            String uid = certificateManager.getUserID(deviceId);
                            certBuilder.setSubjectAltName(Constants.KEYFIELD_USERID + ":" + uid);
                            Certificate certificate = certBuilder.build().getX509Certificate();
                            CertificateStorage.saveCertificate(certificate, deviceId);
                            certificateManager.putCert(encoding, certificate.getEncoded());
                            certificateManager.putCACert(encoding, privateKeyPair.getX509Certificate().getEncoded());
                            certificateManager.createCertificate(serialNumber.toString(), notBefore, notAfter, deviceId, uid);
                            response = MessageBuilder.createResponse(request, ResponseStatus.CHANGED,
                                    ContentFormat.APPLICATION_CBOR,
                                    MAP_CBOR.encodingPayloadToCbor(certificateManager.getPayLoad()));
                        } catch (GeneralSecurityException | OperatorCreationException | CertIOException e) {
                            Log.e(e.getMessage());
                        }
                    }
                } catch (IOException e) {
                    Log.e(e.getMessage());
                }
            }

        }
        return response;
    }

    private void checkSubjectName(CSRParser parser, CertificateBuilder certBuilder) {
        String country = parser.getCountry();
        if (country != null) {
            certBuilder.setSubjectC(country);
        }
        String organization = parser.getOrganizational();
        if (organization != null) {
            certBuilder.setSubjectO(organization);
        }
        String organizationUnit = parser.getOrganizationalUnit();
        if (organizationUnit != null) {
            certBuilder.setSubjectOU(organizationUnit);
        }
    }
}
