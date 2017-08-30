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
public class OicSecAce {
    private String subjectID;
    private int permission;
    private List<OicSecResr> resources;
    private List<OicSecValidity> validities;

    public OicSecAce(String subjectID, int permission,
            List<OicSecResr> resources, List<OicSecValidity> validities) {
        super();
        this.subjectID = subjectID;
        this.permission = permission;
        this.resources = resources;
        this.validities = validities;
    }

    public String getSubjectID() {
        return subjectID;
    }

    public void setSubjectID(String subjectID) {
        this.subjectID = subjectID;
    }

    public int getPermission() {
        return permission;
    }

    public void setPermission(int permission) {
        this.permission = permission;
    }

    public List<OicSecResr> getResourcesList() {
        return resources;
    }

    public OicSecResr[] getResources() {
        return resources.toArray(new OicSecResr[resources.size()]);
    }
    public void setResources(List<OicSecResr> resources) {
        this.resources = resources;
    }

    public List<OicSecValidity> getValiditiesList() {
        return validities;
    }

    public OicSecValidity[] getValidities() {
        return validities.toArray(new OicSecValidity[validities.size()]);
    }


    public void setValidities(List<OicSecValidity> validities) {
        this.validities = validities;
    }
}
