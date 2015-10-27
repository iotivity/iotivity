/* Copyright [2011] [University of Rostock]
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

package org.ws4d.coap.messages;

/**
 * Type-safe class for CoapPacketTypes
 * 
 * @author Nico Laum <nico.laum@uni-rostock.de>
 * @author Sebastian Unger <sebastian.unger@uni-rostock.de>
 * @author Christian Lerche <christian.lerche@uni-rostock.de>
 */
public enum CoapPacketType {
    CON(0x00),
    NON(0x01),
    ACK(0x02),
    RST(0x03);

    private int packetType;

    CoapPacketType(int packetType) {
    	if (packetType >= 0x00 && packetType <= 0x03){
    		this.packetType = packetType;
    	} else {
    		throw new IllegalStateException("Unknown CoAP Packet Type");
		}
    	
    }

    public static CoapPacketType getPacketType(int packetType) {
        if (packetType == 0x00)
            return CON;
        else if (packetType == 0x01)
            return NON;
        else if (packetType == 0x02)
            return ACK;
        else if (packetType == 0x03)
            return RST;
        else
        	throw new IllegalStateException("Unknown CoAP Packet Type");
    }
    
    public int getValue() {
        return packetType;
    }
}
