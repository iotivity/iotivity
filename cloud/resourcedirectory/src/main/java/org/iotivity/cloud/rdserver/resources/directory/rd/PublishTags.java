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

public class PublishTags {
    /** An unique identifier of device */
    private String di;
    /** A human friendly name of device */
    private String n;
    /** Time (in seconds) to indicate how long RD should publish this item */
    private int    lt;

    public PublishTags copy() {
        PublishTags tags = new PublishTags();
        tags.di = this.di;
        tags.n = this.n;
        tags.lt = this.lt;
        return tags;
    }

    public String getDi() {
        return di;
    }

    public void setDi(Object di) {
        this.di = di.toString();
    }

    public String getN() {
        return n;
    }

    public void setN(Object n) {
        this.n = n.toString();
    }

    public int getLt() {
        return lt;
    }

    public void setLt(Object lt) {
        this.lt = (int) lt;
    }
}
