package org.ws4d.coap.messages;

/**
 * @author Christian Lerche <christian.lerche@uni-rostock.de>
 */

public enum CoapMediaType {
    text_plain (0),		//text/plain; charset=utf-8
    link_format (40),	//application/link-format
    xml(41),			//application/xml
    octet_stream (42),	//application/octet-stream
    exi(47),			//application/exi
    json(50),			//application/json
    cbor(60),
    UNKNOWN (-1);
    
    int mediaType;
    private CoapMediaType(int mediaType){
    	this.mediaType = mediaType;
    }
    
    public static CoapMediaType parse(int mediaType){
    	switch(mediaType){
    	case 0: return text_plain; 
    	case 40:return link_format; 
    	case 41:return xml;
    	case 42:return octet_stream;
    	case 47:return exi;
    	case 50:return json;
    	case 60:return cbor;
    		default: return UNKNOWN;
    	}
    }
    
    public int getValue(){
    	return mediaType;
    }
}