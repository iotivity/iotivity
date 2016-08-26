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
import org.iotivity.cloud.accountserver.resources.credprov.cert.CertificateConstants;
import org.iotivity.cloud.accountserver.resources.credprov.cert.CertificateStorage;
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

import java.text.ParseException;
import java.util.*;

import static org.iotivity.cloud.accountserver.resources.credprov.cert.CertificateStorage.PROPERTIES;
import static org.iotivity.cloud.accountserver.resources.credprov.cert.CertificateStorage.getRootCertificatePrivateKeyPair;
import static org.iotivity.cloud.accountserver.x509.crl.CrlParser.DATE_FORMAT;

public class CrlResource extends Resource {

    private static final Cbor<HashMap<String, Object>> MAP_CBOR = new Cbor<>();

    static {
        Calendar calendar = Calendar.getInstance();
        Date thisUpdate = calendar.getTime();
        calendar.add(Calendar.DAY_OF_MONTH,
                Integer.parseInt(PROPERTIES.getProperty("nextUpdateInterval")));
        Date nextUpdate = calendar.getTime();
        CrlIssuer.generate(PROPERTIES.getProperty("crlCN"), thisUpdate,
                nextUpdate, getRootCertificatePrivateKeyPair().getPrivateKey());
        CrlManager.createCRL(thisUpdate);
    }

    public CrlResource() {
        super(Arrays.asList(Constants.PREFIX_OIC,
                Constants.CREDPROV_URI, Constants.REQ_CRL));

    }

    @Override
    public void onDefaultRequestReceived(Device srcDevice, IRequest request)
            throws ServerException {
        IResponse response;
        switch (request.getMethod()) {
            case POST:
                response = handlePostRequest(request);
                break;
            case GET:
                response = handleGetRequest(request);
                break;
            default:
                response = MessageBuilder.createResponse(request, ResponseStatus.BAD_REQUEST);
        }
        srcDevice.sendResponse(response);
    }

    private IResponse handleGetRequest(IRequest request)
            throws ServerException {
        HashMap<String, List<String>> queryData = request.getUriQueryMap();
        IResponse iResponse = MessageBuilder.createResponse(request, ResponseStatus.NOT_FOUND);
        if (queryData != null) {
            List<String> lastUpdateList = queryData.get(Constants.REQ_LAST_UPDATE);

            if (lastUpdateList != null && !lastUpdateList.isEmpty() && CrlManager.checkLastUpdate(lastUpdateList.get(0))) {
                System.out.println("LAST DATE = " + lastUpdateList.get(0));
                iResponse = MessageBuilder.createResponse(request, ResponseStatus.CONTENT,
                        ContentFormat.APPLICATION_CBOR, MAP_CBOR.encodingPayloadToCbor(CrlManager.getCrlInfo()));
            }
        }
        return iResponse;
    }

    private IResponse handlePostRequest(IRequest request)
            throws ServerException {
        HashMap<String, Object> payloadData = MAP_CBOR
                .parsePayloadFromCbor(request.getPayload(), HashMap.class);
        Object thisUpdate = payloadData.get(Constants.REQ_THIS_UPDATE);
        Object nextUpdate = payloadData.get(Constants.REQ_NEXT_UPDATE);
        IResponse response = MessageBuilder.createResponse(request, ResponseStatus.PRECONDITION_FAILED);
        if (thisUpdate != null || nextUpdate != null) {
            String encoding = (String) payloadData.get(Constants.CRL_ENCODING);
            byte[] crl = (byte[]) payloadData.get(Constants.CRL_DATA);
            if(encoding != null && crl != null && encoding.equals(CertificateConstants.BASE_64)) {
                if (encoding.equals(CertificateConstants.BASE_64)) {
                    crl = org.bouncycastle.util.encoders.Base64.decode(crl);
                }
                CrlIssuer.update(crl);
                response = MessageBuilder.createResponse(request, ResponseStatus.CHANGED);
            }
            Object reqSerialNumber = payloadData.get(Constants.REQ_SERIAL_NUMBER);
            if (reqSerialNumber != null) {
                Date thisUpdateDate = null;
                Date nextUpdateDate = null;
                boolean parseable = true;
                try {
                    thisUpdateDate = DATE_FORMAT.parse(thisUpdate.toString());
                    nextUpdateDate = DATE_FORMAT.parse(nextUpdate.toString());
                } catch (ParseException e) {
                    Log.e(e.getMessage());
                    parseable = false;
                }
                if(parseable) {
                    CrlIssuer.update(thisUpdateDate, nextUpdateDate, reqSerialNumber.toString(), CertificateStorage.getRootCertificatePrivateKeyPair().getPrivateKey());
                    response = MessageBuilder.createResponse(request, ResponseStatus.CHANGED);
                }
            }
        }
        return response;
    }
}
