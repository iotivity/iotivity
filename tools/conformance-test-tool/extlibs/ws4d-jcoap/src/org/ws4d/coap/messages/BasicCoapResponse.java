package org.ws4d.coap.messages;

import java.io.UnsupportedEncodingException;

import org.ws4d.coap.interfaces.CoapResponse;
import org.ws4d.coap.messages.AbstractCoapMessage.CoapHeaderOption;
import org.ws4d.coap.messages.AbstractCoapMessage.CoapHeaderOptionType;

/**
 * @author Christian Lerche <christian.lerche@uni-rostock.de>
 */

public class BasicCoapResponse extends AbstractCoapMessage implements
        CoapResponse {
    CoapResponseCode responseCode;

    public BasicCoapResponse(byte[] bytes, int length) {
        this(bytes, length, 0);
    }

    public BasicCoapResponse(byte[] bytes, int length, int offset) {
        serialize(bytes, length, offset);
        /*
         * check if response code is valid, this function throws an error in
         * case of an invalid argument
         */
        responseCode = CoapResponseCode
                .parseResponseCode(this.messageCodeValue);

        // TODO: check integrity of header options
    }

    /* token can be null */
    public BasicCoapResponse(CoapPacketType packetType,
            CoapResponseCode responseCode, int messageId, byte[] requestToken) {
        this.version = 1;

        this.packetType = packetType;

        this.responseCode = responseCode;
        if (responseCode == CoapResponseCode.UNKNOWN) {
            throw new IllegalArgumentException(
                    "UNKNOWN Response Code not allowed");
        }
        this.messageCodeValue = responseCode.getValue();

        this.messageId = messageId;

        setToken(requestToken);
    }

    @Override
    public void setToken(byte[] token) {
        /* this function is only public for a request */
        super.setToken(token);
    }

    @Override
    public CoapResponseCode getResponseCode() {
        return responseCode;
    }

    public String getResponseCodeString() {

        int responseValue = responseCode.getValue();

        return String.format("%d.%02d", responseValue / 32, responseValue % 32);
    }

    @Override
    public void setMaxAge(int maxAge) {
        if (options.optionExists(CoapHeaderOptionType.Max_Age)) {
            throw new IllegalStateException("Max Age option already exists");
        }
        if (maxAge < 0) {
            throw new IllegalStateException("Max Age MUST be an unsigned value");
        }
        options.addOption(CoapHeaderOptionType.Max_Age, long2CoapUint(maxAge));
    }

    @Override
    public String getHeaderOptions() {
        StringBuilder sb = new StringBuilder();
        StringBuilder sbv = new StringBuilder();
        int oldNr = -1;
        boolean list = false;
        String optionValue = "";

        sb.append('{');

        for (CoapHeaderOption option : options) {
            if (option.getOptionTypeValue() != oldNr) {
                if (oldNr != -1) {
                    if (list)
                        sbv.append(']');
                    sb.append(sbv.toString());
                    sbv = new StringBuilder();
                    sb.append(", ");
                } else {

                }
                list = false;
                sb.append('"');
                sb.append(option.getOptionType().getString());
                sb.append('"');
                sb.append(':');
            } else {
                if (!list)
                    sbv.insert(0, '[');
                list = true;
                sbv.append(",");
            }

            if (option.getOptionType().equals(CoapHeaderOptionType.Uri_Path)
                    || option.getOptionType().equals(
                            CoapHeaderOptionType.Uri_Query)) {
                try {
                    optionValue = new String(option.getOptionData(), "UTF-8");
                } catch (UnsupportedEncodingException e) {
                    e.printStackTrace();
                }
            }
            sbv.append("\"" + optionValue + "\"");
            oldNr = option.getOptionTypeValue();
        }
        if (list)
            sbv.append(']');
        sb.append(sbv.toString());
        sb.append('}');

        return sb.toString();
    }

    @Override
    public long getMaxAge() {
        CoapHeaderOption option = options
                .getOption(CoapHeaderOptionType.Max_Age);
        if (option == null) {
            return -1;
        }
        return coapUint2Long((options.getOption(CoapHeaderOptionType.Max_Age)
                .getOptionData()));
    }

    @Override
    public void setETag(byte[] etag) {
        if (etag == null) {
            throw new IllegalArgumentException("etag MUST NOT be null");
        }
        if (etag.length < 1 || etag.length > 8) {
            throw new IllegalArgumentException("Invalid etag length");
        }
        options.addOption(CoapHeaderOptionType.Etag, etag);
    }

    @Override
    public byte[] getETag() {
        CoapHeaderOption option = options.getOption(CoapHeaderOptionType.Etag);
        if (option == null) {
            return null;
        }
        return option.getOptionData();
    }

    @Override
    public boolean isRequest() {
        return false;
    }

    @Override
    public boolean isResponse() {
        return true;
    }

    @Override
    public boolean isEmpty() {
        return false;
    }

    @Override
    public String toString() {
        return packetType.toString() + ", " + responseCode.toString()
                + ", MsgId: " + getMessageID() + ", #Options: "
                + options.getOptionCount();
    }

    @Override
    public void setResponseCode(CoapResponseCode responseCode) {
        if (responseCode != CoapResponseCode.UNKNOWN) {
            this.responseCode = responseCode;
            this.messageCodeValue = responseCode.getValue();
        }
    }

    @Override
    public void setUriPath(String path) {
        if (path == null)
            return;

        if (path.length() > CoapHeaderOption.MAX_LENGTH) {
            throw new IllegalArgumentException("Uri-Path option too long");
        }

        /* delete old options if present */
        options.removeOption(CoapHeaderOptionType.Uri_Path);

        /* create substrings */
        String[] pathElements = path.split("/");
        /* add a Uri Path option for each part */
        for (String element : pathElements) {
            /* check length */
            if (element.length() < 0
                    || element.length() > CoapHeaderOption.MAX_LENGTH) {
                throw new IllegalArgumentException("Invalid Uri-Path");
            } else if (element.length() > 0) {
                /* ignore empty substrings */
                options.addOption(CoapHeaderOptionType.Uri_Path,
                        element.getBytes());
            }
        }

    }

    @Override
    public void setUriQuery(String query) {
        if (query == null)
            return;

        if (query.length() > CoapHeaderOption.MAX_LENGTH) {
            throw new IllegalArgumentException("Uri-Query option too long");
        }

        /* delete old options if present */
        options.removeOption(CoapHeaderOptionType.Uri_Query);

        /* create substrings */
        String[] pathElements = query.split("&");
        /* add a Uri Path option for each part */
        for (String element : pathElements) {
            /* check length */
            if (element.length() < 0
                    || element.length() > CoapHeaderOption.MAX_LENGTH) {
                throw new IllegalArgumentException("Invalid Uri-Path");
            } else if (element.length() > 0) {
                /* ignore empty substrings */
                options.addOption(CoapHeaderOptionType.Uri_Query,
                        element.getBytes());
            }
        }

    }
}
