package org.iotivity.cloud.base.protocols.coap.enums;

public enum CoapStatus {
    // Success 2.xx
    CREATED(65), DELETED(66), VALID(67), CHANGED(68), CONTENT(69),
    // Client Error 4.xx
    BAD_REQUEST(128), UNAUTHORIZED(129), BAD_OPTION(130), FORBIDDEN(131),
    //
    NOT_FOUND(132), METHOD_NOT_ALLOWED(133), NOT_ACCEPTABLE(134),
    //
    PRECONDITION_FAILED(140), REQUEST_ENTITY_TOO_LARGE(141),
    //
    UNSUPPORTED_CONTENT_FORMAT(143),
    // Server Error 5.xx
    INTERNAL_SERVER_ERROR(160), NOT_IMPLEMENTED(161), BAD_GATEWAY(162),
    //
    SERVICE_UNAVAILABLE(163), GATEWAY_TIMEOUT(164), PROXY_NOT_SUPPORTED(165);

    public static CoapStatus valueOf(int code) {
        switch (code) {
            case 65:
                return CoapStatus.CREATED;

            case 66:
                return CoapStatus.DELETED;

            case 67:
                return CoapStatus.VALID;

            case 68:
                return CoapStatus.CHANGED;

            case 69:
                return CoapStatus.CONTENT;

            case 128:
                return CoapStatus.BAD_REQUEST;

            case 129:
                return CoapStatus.UNAUTHORIZED;

            case 130:
                return CoapStatus.BAD_OPTION;

            case 131:
                return CoapStatus.FORBIDDEN;

            case 132:
                return CoapStatus.NOT_FOUND;

            case 133:
                return CoapStatus.METHOD_NOT_ALLOWED;

            case 134:
                return CoapStatus.NOT_ACCEPTABLE;

            case 140:
                return CoapStatus.PRECONDITION_FAILED;

            case 141:
                return CoapStatus.REQUEST_ENTITY_TOO_LARGE;

            case 143:
                return CoapStatus.UNSUPPORTED_CONTENT_FORMAT;

            case 160:
                return CoapStatus.INTERNAL_SERVER_ERROR;

            case 161:
                return CoapStatus.NOT_IMPLEMENTED;

            case 162:
                return CoapStatus.BAD_GATEWAY;

            case 163:
                return CoapStatus.SERVICE_UNAVAILABLE;

            case 164:
                return CoapStatus.GATEWAY_TIMEOUT;

            case 165:
                return CoapStatus.PROXY_NOT_SUPPORTED;
        }

        throw new IllegalArgumentException("Invalid Status value");
    }

    private final int code;

    private CoapStatus(int code) {
        this.code = code;
    }

    public int getCode() {
        return code;
    }
}
