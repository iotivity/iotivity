package org.iotivity.cloud.accountserver.resources.certificate;

import java.util.HashMap;
import java.util.Map;

import org.iotivity.cloud.accountserver.Constants;

public class CertificateManager {
    private Map<String, Object> mResponsePayload  = new HashMap<>();

    public Map<String, Object> createPayload(byte[] cert, byte[] cacert) {
        mResponsePayload.put(Constants.RESP_CERT, cert);
        mResponsePayload.put(Constants.RESP_CACERT, cacert);
        return mResponsePayload;
    }


    public void addDeviceId(String di) {
        mResponsePayload.put(Constants.RESP_DEVICE_ID, di);
    }

}
