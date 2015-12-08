// ******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
//
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

package oic.ctt.network;

import org.ws4d.coap.interfaces.CoapRequest;
import org.ws4d.coap.interfaces.CoapServerChannel;

/**
 * The OICObserver class, contains various information on a single observer in
 * each of its instances.
 */
public class OICObserver {

    private int               mSequenceNumber;
    private CoapServerChannel mCoapServerChannel;
    private CoapRequest       mCoapRequest;

    /**
     * Initializes an OICObserver object
     */
    public OICObserver() {
        setCoapServerChannel(null);
        setCoapRequest(null);
        setSeqNumber(4);
    }

    /**
     * Gets the IP address of the observer
     * 
     * @return string value of the observer
     */
    public String getObserverIP() {
        return mCoapRequest.getSourceIP().split("/")[1];
    }

    /**
     * Gets the port of the observer
     * 
     * @return integer value indicating port address
     */
    public int getObserverPort() {
        return mCoapRequest.getSourcePort();
    }

    /**
     * Gets the notification QoS property of the observer
     * 
     * @return true if CON and false if NON
     */
    public boolean getReliablity() {
        return mCoapRequest.isReliable();
    }

    /**
     * Gets the sequence number
     * 
     * @return Sequence number
     */
    public int getSeqNumber() {
        return ++mSequenceNumber;
    }

    /**
     * Sets the sequence number
     * 
     * @param seqNumber
     *            Sequence number
     */
    public void setSeqNumber(int seqNumber) {
        this.mSequenceNumber = seqNumber;
    }

    /**
     * Gets the CoAP server channel reference
     * 
     * @return CoapServerChannel object
     */
    public CoapServerChannel getCoapServerChannel() {
        return mCoapServerChannel;
    }

    /**
     * Sets the CoAP server channel refercence
     * 
     * @param mCoapServerChannel
     *            CoapServerChannel object
     */
    public void setCoapServerChannel(CoapServerChannel mCoapServerChannel) {
        this.mCoapServerChannel = mCoapServerChannel;
    }

    /**
     * Gets the CoAP Request
     * 
     * @return CoapRequest object
     */
    public CoapRequest getCoapRequest() {
        return mCoapRequest;
    }

    /**
     * Sets the CoAP Request
     * 
     * @param mCoapRequest
     *            CoapRequest object
     */
    public void setCoapRequest(CoapRequest mCoapRequest) {
        this.mCoapRequest = mCoapRequest;
    }
}
