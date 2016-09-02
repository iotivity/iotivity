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
package org.iotivity.cloud.accountserver.resources.certificate;

import java.security.PublicKey;
import java.util.Arrays;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;

import org.iotivity.cloud.accountserver.Constants;
import org.iotivity.cloud.accountserver.x509.cert.CSRParser;
import org.iotivity.cloud.accountserver.x509.cert.CertificateBuilder;
import org.iotivity.cloud.accountserver.x509.cert.CertificateIssuerConfig;
import org.iotivity.cloud.accountserver.x509.cert.CertificatePrivateKeyPair;
import org.iotivity.cloud.base.device.Device;
import org.iotivity.cloud.base.exception.ServerException;
import org.iotivity.cloud.base.exception.ServerException.BadRequestException;
import org.iotivity.cloud.base.protocols.IRequest;
import org.iotivity.cloud.base.protocols.IResponse;
import org.iotivity.cloud.base.protocols.MessageBuilder;
import org.iotivity.cloud.base.protocols.enums.ContentFormat;
import org.iotivity.cloud.base.protocols.enums.ResponseStatus;
import org.iotivity.cloud.base.resource.Resource;
import org.iotivity.cloud.util.Cbor;

public class CertificateResource extends Resource {
    /* resource uri for certificate issuer */
    private Cbor<HashMap<String, Object>> mCbor               = new Cbor<>();
    private CertificateManager            mCertificateManager = new CertificateManager();
    private static String                 DEVICE_OU           = "OCF Device";

    public CertificateResource() {
        super(Arrays.asList(Constants.PREFIX_OIC, Constants.CREDPROV_URI,
                Constants.CERT_URI));
    }

    @Override
    public void onDefaultRequestReceived(Device srcDevice, IRequest request)
            throws ServerException {
        IResponse response = null;

        switch (request.getMethod()) {
            case POST:
                response = handlePutRequest(request);
                break;

            default:
                throw new BadRequestException(
                        request.getMethod() + " request type is not support");
        }

        srcDevice.sendResponse(response);
    }

    private IResponse handlePutRequest(IRequest request)
            throws ServerException {

        Map<String, Object> payloadData = mCbor
                .parsePayloadFromCbor(request.getPayload(), HashMap.class);

        if (payloadData == null) {
            throw new BadRequestException("CBOR parsing failed");
        }

        Map<String, Object> responsePayload = null;

        if (payloadData.containsKey(Constants.REQ_CSR)) {

            byte[] csrDer = (byte[]) payloadData.get(Constants.REQ_CSR);
            CSRParser parser = null;
            PublicKey pubKey = null;

            try {
                parser = new CSRParser(csrDer);
                if (!parser.verify()) {
                    throw new Exception();
                }
                pubKey = parser.getPublicKey();
            } catch (Exception e) {
                throw new BadRequestException("Bad CSR");
            }

            CertificateIssuerConfig ciConfig = CertificateIssuerConfig
                    .getInstance();

            Date notBefore = ciConfig.getNotBeforeDate();
            Date notAfter = ciConfig.getNotAfterDate();

            CertificateBuilder certBuilder = new CertificateBuilder(
                    parser.getCommonName(), pubKey, notBefore, notAfter,
                    ciConfig.getNextSerialNumber(),
                    ciConfig.getRootCertificate());

            certBuilder.setSubjectC(parser.getCountry());

            certBuilder.setSubjectO(parser.getOrganizational());

            certBuilder.setSubjectOU(DEVICE_OU);

            CertificatePrivateKeyPair devCert = null;

            try {
                devCert = certBuilder.build();
                // System.out.println("============================== DEV CERT
                // ===========================");
                // System.out.println();
                // System.out.println(devCert.getCertificate().toString());
                // System.out.println("===================================================================");

                if (payloadData.containsKey(Constants.REQ_DEVICE_ID)) {
                    mCertificateManager.addDeviceId(
                            (String) payloadData.get(Constants.RESP_DEVICE_ID));
                }

                responsePayload = mCertificateManager.createPayload(
                        devCert.getCertificate().getEncoded(),
                        ciConfig.getRootCertificate().getCertificate()
                                .getEncoded());

            } catch (Exception e) {
                throw new BadRequestException("Certificate generation error");
            }

        } else {
            throw new BadRequestException("CSR is null");
        }

        return MessageBuilder.createResponse(request, ResponseStatus.CONTENT,
                ContentFormat.APPLICATION_CBOR,
                mCbor.encodingPayloadToCbor(responsePayload));
    }
}
