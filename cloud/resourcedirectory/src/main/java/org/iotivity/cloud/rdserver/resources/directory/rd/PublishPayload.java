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

public class PublishPayload {

    private PublishTags             tags  = new PublishTags();

    private ArrayList<PublishLinks> links = new ArrayList<>();

    public PublishPayload copy() {
        PublishPayload pubPayload = new PublishPayload();
        pubPayload.tags = this.tags.copy();

        pubPayload.links = new ArrayList<>();

        for (PublishLinks link : this.links) {
            pubPayload.links.add(link.copy());
        }

        return pubPayload;
    }

    public PublishTags getTags() {
        return tags;
    }

    public void setTags(PublishTags tags) {
        this.tags = tags;
    }

    public ArrayList<PublishLinks> getLinks() {
        return links;
    }

    public void setLinks(ArrayList<PublishLinks> links) {
        this.links = links;
    }
}
