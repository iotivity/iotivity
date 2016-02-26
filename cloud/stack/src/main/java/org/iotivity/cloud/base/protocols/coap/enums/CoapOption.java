package org.iotivity.cloud.base.protocols.coap.enums;

public enum CoapOption {
    IF_MATCH(1), URI_HOST(3), ETAG(4), IF_NONE_MATCH(5), URI_PORT(
            7), LOCATION_PATH(8), URI_PATH(11), CONTENT_FORMAT(12), MAX_AGE(
                    14), URI_QUERY(15), ACCEPT(17), LOCATION_QUERY(
                            20), PROXY_URI(35), PROXY_SCHEME(39), SIZE1(
                                    60), OBSERVE(6);

    public static CoapOption valueOf(int option) {
        switch (option) {
            case 1:
                return CoapOption.IF_MATCH;

            case 3:
                return CoapOption.URI_HOST;

            case 4:
                return CoapOption.ETAG;

            case 5:
                return CoapOption.IF_NONE_MATCH;

            case 7:
                return CoapOption.URI_PORT;

            case 8:
                return CoapOption.LOCATION_PATH;

            case 11:
                return CoapOption.URI_PATH;

            case 12:
                return CoapOption.CONTENT_FORMAT;

            case 14:
                return CoapOption.MAX_AGE;

            case 15:
                return CoapOption.URI_QUERY;

            case 17:
                return CoapOption.ACCEPT;

            case 20:
                return CoapOption.LOCATION_QUERY;

            case 35:
                return CoapOption.PROXY_URI;

            case 39:
                return CoapOption.PROXY_SCHEME;

            case 60:
                return CoapOption.SIZE1;

            case 6:
                return CoapOption.OBSERVE;
        }

        throw new IllegalArgumentException("Invalid option value");
    }

    private final int value;

    private CoapOption(int value) {
        this.value = value;
    }

    public int getvalue() {
        return value;
    }
}
