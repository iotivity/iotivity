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

package org.iotivity.base;

import java.io.Serializable;
import java.util.List;
import java.util.Arrays;

public class OicSecPdAcl implements Serializable {

    private int          mPermission;
    private List<String> mResources;
    private List<String> mPeriods;
    private List<String> mRecurrences;

    public OicSecPdAcl(List<String> recurrences, List<String> periods, int permission,
            List<String> resources) {
        this.mRecurrences = recurrences;
        this.mPeriods = periods;
        this.mPermission = permission;
        this.mResources = resources;
    }

    public List<String> getRecurrences() {
        return mRecurrences;
    }

    public void setRecurrences(List<String> recurrences) {
        this.mRecurrences = recurrences;
    }

    public List<String> getPeriods() {
        return mPeriods;
    }

    public void setPeriods(List<String> periods) {
        this.mPeriods = periods;
    }

    public int getPermission() {
        return this.mPermission;
    }

    public void setPermission(int permission) {
        this.mPermission = permission;
    }

    public List<String> getResources() {
        return mResources;
    }

    public void setResources(List<String> resources) {
        this.mResources = resources;
    }

    public int getResourcesCount() {
        return this.mResources.size();
    }

    public String getResources(int i) {
        return this.mResources.get(i);
    }

    public int getPeriodsCount() {
        return this.mPeriods.size();
    }

    public String getPeriods(int i) {
        return this.mPeriods.get(i);
    }

    public String getRecurrences(int i) {
        return this.mRecurrences.get(i);
    }
}
