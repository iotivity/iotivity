/*
 *******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

package org.iotivity.base;

import java.util.List;

public class OicSecAcl {
    private String rownerID;
    private List<OicSecAce> oicSecAces;

    public OicSecAcl (String rownerID, List<OicSecAce> oicSecAces) {
        super();
        this.rownerID = rownerID;
        this.oicSecAces = oicSecAces;
    }
    public String getRownerID() {
        return rownerID;
    }

    public void setRownerID(String rownerID) {
        this.rownerID = rownerID;
    }

    public List<OicSecAce> getOicSecAcesList() {
        return oicSecAces;
    }

    public OicSecAce[] getOicSecAces() {
        return oicSecAces.toArray(new OicSecAce[oicSecAces.size()]);
    }

    public void setOicSecAces(List<OicSecAce> oicSecAces) {
        this.oicSecAces = oicSecAces;
    }
}
