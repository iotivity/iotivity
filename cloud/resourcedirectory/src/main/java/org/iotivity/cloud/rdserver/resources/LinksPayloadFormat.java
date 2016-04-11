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

public class LinksPayloadFormat {
    /** This is the target relative URI. */
    String            href;
    /**
     * Resource Type - A standard OIC specified or vendor defined resource type
     * of the resource referenced by the target URI.
     */
    ArrayList<String> rt;
    /**
     * Interface - The interfaces supported by the resource referenced by the
     * target URI.
     */
    ArrayList<String> itf;
    /**
     * The relation of the target URI referenced by the link to the context URI;
     * The default value is null.
     */
    String            rel;
    /**
     * Specifies if the resource referenced by the target URIis observable or
     * not.
     */
    boolean           obs;
    /**
     * A title for the link relation. Can be used by the UI to provide a
     * context.
     */
    String            title;
    /**
     * This is used to override the context URI e.g. override the URI of the
     * containing collection.
     */
    String            uri;
    /**
     * The instance identifier for this web link in an array of web links - used
     * in links.
     */
    int               ins;
    /**
     * A hint of the media type of the representation of the resource referenced
     * by the target URI.
     */
    ArrayList<String> mt;

    public LinksPayloadFormat() {
        rt = new ArrayList<String>();
        itf = new ArrayList<String>();
        mt = new ArrayList<String>();
    }

    public String getHref() {
        return href;
    }

    public void setHref(String href) {
        this.href = href;
    }

    public ArrayList<String> getRt() {
        return rt;
    }

    public void setRt(ArrayList<String> rt) {
        this.rt = rt;
    }

    public ArrayList<String> getItf() {
        return itf;
    }

    public void setItf(ArrayList<String> itf) {
        this.itf = itf;
    }

    public String getRel() {
        return rel;
    }

    public void setRel(String rel) {
        this.rel = rel;
    }

    public boolean isObs() {
        return obs;
    }

    public void setObs(boolean obs) {
        this.obs = obs;
    }

    public String getTitle() {
        return title;
    }

    public void setTitle(String title) {
        this.title = title;
    }

    public String getUri() {
        return uri;
    }

    public void setUri(String uri) {
        this.uri = uri;
    }

    public int getIns() {
        return ins;
    }

    public void setIns(int ins) {
        this.ins = ins;
    }

    public ArrayList<String> getMt() {
        return mt;
    }

    public void setMt(ArrayList<String> mt) {
        this.mt = mt;
    }
}