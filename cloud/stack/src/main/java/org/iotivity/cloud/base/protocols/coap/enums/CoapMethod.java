package org.iotivity.cloud.base.protocols.coap.enums;

public enum CoapMethod {
    GET(1), POST(2), PUT(3), DELETE(4);

    public static CoapMethod valueOf(int code) {
        switch (code) {
            case 1:
                return CoapMethod.GET;

            case 2:
                return CoapMethod.POST;

            case 3:
                return CoapMethod.PUT;

            case 4:
                return CoapMethod.DELETE;
        }

        throw new IllegalArgumentException("Invalid Method value");
    }

    private final int code;

    private CoapMethod(int code) {
        this.code = code;
    }

    public int getCode() {
        return code;
    }
}
