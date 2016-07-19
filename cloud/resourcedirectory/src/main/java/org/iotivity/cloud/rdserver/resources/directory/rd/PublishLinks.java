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
package org.iotivity.cloud.rdserver.resources.directory.rd;

import java.util.ArrayList;
import java.util.HashMap;

import org.iotivity.cloud.rdserver.Constants;

public class PublishLinks {
    /** URI Reference */
    private String            href;
    /** Relation between target URI and context URI */
    private String            rel;
    /** Resource Types */
    private ArrayList<String> rt   = new ArrayList<String>();
    /** Resource interface */
    private ArrayList<String> itf  = new ArrayList<String>();
    /** policies that apply for resource */
    private Object            p;
    /** Title for the link relation */
    private String            title;
    /** This is used to override the context URI */
    private String            anchor;
    /** An ordinal number that is not repeated */
    private int               ins;
    /** Time to live for this link */
    private int               ttl;
    /**
     * A hint of the media type of the representation of the resource referenced
     * by the target URI.
     */
    ArrayList<String>         type = new ArrayList<String>();

    public PublishLinks copy() {
        PublishLinks links = new PublishLinks();
        links.href = this.href;
        links.rel = this.rel;
        links.rt.addAll(this.rt);
        links.itf.addAll(this.itf);
        links.p = this.p;
        links.title = this.title;
        links.anchor = this.anchor;
        links.ins = this.ins;
        links.ttl = this.ttl;
        return links;
    }

    public String getHref() {
        return href;
    }

    public void setHref(Object href) {
        this.href = href.toString();
    }

    public String getRel() {
        return rel;
    }

    public void setRel(Object rel) {
        this.rel = rel.toString();
    }

    public ArrayList<String> getRt() {
        return rt;
    }

    public void setRt(Object rt) {
        this.rt = (ArrayList<String>) rt;
    }

    public ArrayList<String> getItf() {
        return itf;
    }

    public void setItf(Object itf) {
        this.itf = (ArrayList<String>) itf;
    }

    public Object getP() {
        return p;
    }

    public void setP(Object p) {
        HashMap<Object, Object> bm = (HashMap<Object, Object>) p;
        this.p = bm.get(Constants.BITMAP);
    }

    public void changePType() {
        HashMap<Object, Object> bm = new HashMap<Object, Object>();
        bm.put(Constants.BITMAP, (int) p);
        this.p = bm;
    }

    public String getTitle() {
        return title;
    }

    public void setTitle(Object title) {
        this.title = title.toString();
    }

    public String getAnchor() {
        return anchor;
    }

    public void setAnchor(Object anchor) {
        this.anchor = anchor.toString();
    }

    public int getIns() {
        return ins;
    }

    public void setIns(Object ins) {
        this.ins = (int) ins;
    }

    public int getTtl() {
        return ttl;
    }

    public void setTtl(Object ttl) {
        this.ttl = (int) ttl;
    }

    public ArrayList<String> getType() {
        return type;
    }

    public void setType(Object type) {
        this.type = (ArrayList<String>) type;
    }

}
