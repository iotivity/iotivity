package org.ws4d.coap.messages;

/**
 * @author Christian Lerche <christian.lerche@uni-rostock.de>
 */

public enum CoapRequestCode {
	GET(1), POST(2), PUT(3), DELETE(4);

	private int code;

	private CoapRequestCode(int code) {
		this.code = code;
	}
	
	public static CoapRequestCode parseRequestCode(int codeValue){
		switch (codeValue) {
		case 1:
			return GET;
		case 2:
			return POST;
		case 3:
			return PUT;
		case 4:
			return DELETE;
		default:
			throw new IllegalArgumentException("Invalid Request Code");
		}
	}

	public int getValue() {
		return code;
	}
	
	@Override
	public String toString() {
		switch (this) {
		case GET:
			return "GET";
		case POST:
			return "POST";
		case PUT:
			return "PUT";
		case DELETE:
			return "DELETE";
		}
		return null;
	}
}

