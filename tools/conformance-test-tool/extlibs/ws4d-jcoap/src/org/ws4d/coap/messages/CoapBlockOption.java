package org.ws4d.coap.messages;

/**
 * @author Christian Lerche <christian.lerche@uni-rostock.de>
 */


public class CoapBlockOption{
	private int number;
	private boolean more;
	private CoapBlockSize blockSize;
	
	
	public CoapBlockOption(byte[] data){
		if (data.length <1 || data.length > 3){
			throw new IllegalArgumentException("invalid block option : data length is "+data.length);
		}
		long val = AbstractCoapMessage.coapUint2Long(data);
		
		this.blockSize = CoapBlockSize.parse((int) (val & 0x7));
		if (blockSize == null){
			throw new IllegalArgumentException("invalid block options : block size is "+blockSize);
		}
		
		if ((val & 0x8) == 0){
			//more bit not set 
			more = false;
		} else {
			more = true;
		}
		number = (int) (val >> 4);
	}
	
	public CoapBlockOption(int number, boolean more, CoapBlockSize blockSize){
		if (blockSize == null){
			throw new IllegalArgumentException();
		}
		if (number < 0 || number > 0xFFFFFF ){
			//not an unsigned 20 bit value
			throw new IllegalArgumentException();
		}
		
		this.blockSize = blockSize;
		this.number = number;
		this.more = more;
	}

	public int getNumber() {
		return number;
	}

	public boolean isLast() {
		return !more;
	}


	public CoapBlockSize getBlockSize() {
		return blockSize;
	}
	
	public int getBytePosition(){
		return number << (blockSize.getExponent() + 4);
	}
	
	
	public byte[] getBytes(){
		int value = number << 4;
		value |= blockSize.getExponent();
		if (more){
			value |= 0x8;
		}
		return AbstractCoapMessage.long2CoapUint(value);
	}
	
	public enum CoapBlockSize {
	    BLOCK_16 (0),
	    BLOCK_32 (1),
	    BLOCK_64 (2),
	    BLOCK_128(3),
	    BLOCK_256 (4),
	    BLOCK_512 (5),
	    BLOCK_1024 (6);	    
	    int exp; 
	    
	    CoapBlockSize(int exponent){
	    	exp = exponent;
	    }
	    
	    public static CoapBlockSize parse(int exponent){
	    	switch(exponent){
	    	case 0: return BLOCK_16;
	    	case 1: return BLOCK_32;
	    	case 2: return BLOCK_64;
	    	case 3: return BLOCK_128;
	    	case 4: return BLOCK_256;
	    	case 5: return BLOCK_512;
	    	case 6: return BLOCK_1024;
	    	default : return null;
	    	}
	    	
	    }
	    
	    public int getExponent(){
	    	return exp;
	    }
	    
	    public int getSize(){
	    	return 1 << (exp+4);
	    }
	}

}
