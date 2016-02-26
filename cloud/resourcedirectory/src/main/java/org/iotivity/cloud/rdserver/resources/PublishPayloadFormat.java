/*
 * //******************************************************************
 * //
 * // Copyright 2016 Samsung Electronics All Rights Reserved.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 * //
 * // Licensed under the Apache License, Version 2.0 (the "License");
 * // you may not use this file except in compliance with the License.
 * // You may obtain a copy of the License at
 * //
 * //      http://www.apache.org/licenses/LICENSE-2.0
 * //
 * // Unless required by applicable law or agreed to in writing, software
 * // distributed under the License is distributed on an "AS IS" BASIS,
 * // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * // See the License for the specific language governing permissions and
 * // limitations under the License.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */
package org.iotivity.cloud.rdserver.resources;

import java.util.ArrayList;

public class PublishPayloadFormat {

    /** Name of tags. */
    String deviceName;
    /** Device identifier. */
    String di;
    /** The base URI where the resources are hold. */
    String baseUri;
    /** Bitmap holds observable, discoverable, secure option flag. */
    int    bitmap;
    /** Port set in case, the secure flag is set above. */
    int    port;
    /** Id for each set of links i.e. tag. */
    int    ins;
    /**
     * Defines the list of allowable resource types (for Target and anchors) in
     * links included in the collection; new links being created can only be
     * from this list.
     */
    String rts;
    /**
     * When specified this is the default relationship to use when an OIC Link
     * does not specify an explicit relationship with *rel* parameter.
     */
    String drel;
    /** Time to keep holding resource. */
    int    ttl;

    ArrayList<LinksPayloadFormat> links;

    public PublishPayloadFormat() {
        links = new ArrayList<LinksPayloadFormat>();
    }

    public String getDeviceName() {
        return deviceName;
    }

    public void setDeviceName(String deviceName) {
        this.deviceName = deviceName;
    }

    public String getDi() {
        return di;
    }

    public void setDi(String di) {
        this.di = di;
    }

    public String getBaseUri() {
        return baseUri;
    }

    public void setBaseUri(String baseUri) {
        this.baseUri = baseUri;
    }

    public int getBitmap() {
        return bitmap;
    }

    public void setBitmap(int bitmap) {
        this.bitmap = bitmap;
    }

    public int getPort() {
        return port;
    }

    public void setPort(int port) {
        this.port = port;
    }

    public int getIns() {
        return ins;
    }

    public void setIns(int ins) {
        this.ins = ins;
    }

    public String getRts() {
        return rts;
    }

    public void setRts(String rts) {
        this.rts = rts;
    }

    public String getDrel() {
        return drel;
    }

    public void setDrel(String drel) {
        this.drel = drel;
    }

    public int getTtl() {
        return ttl;
    }

    public void setTtl(int ttl) {
        this.ttl = ttl;
    }

    public ArrayList<LinksPayloadFormat> getLinks() {
        return links;
    }

    public void setLinks(ArrayList<LinksPayloadFormat> links) {
        this.links = links;
    }
}
