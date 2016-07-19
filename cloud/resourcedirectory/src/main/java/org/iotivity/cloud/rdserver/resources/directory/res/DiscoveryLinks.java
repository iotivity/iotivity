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
package org.iotivity.cloud.rdserver.resources.directory.res;

import java.util.ArrayList;
import java.util.HashMap;

import org.iotivity.cloud.rdserver.Constants;

public class DiscoveryLinks {
    /** URI Reference */
    private String                  href;
    /** Resource Types */
    private ArrayList<String>       rt  = new ArrayList<>();
    /** Resource interface */
    private ArrayList<String>       itf = new ArrayList<>();
    /** policies that apply for resource */
    private HashMap<Object, Object> p   = new HashMap<>();

    public DiscoveryLinks() {

    }

    public String getHref() {
        return href;
    }

    public void setHref(Object href) {
        this.href = href.toString();
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
        int bm = (int) p;
        this.p.put(Constants.BITMAP, bm);
    }
}
