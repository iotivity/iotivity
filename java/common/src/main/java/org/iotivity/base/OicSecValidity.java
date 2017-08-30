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
public class OicSecValidity {
    private String period;
    private List<String> recurrences;
    private int recurrenceLen;

    public OicSecValidity(String period, List<String> recurrences,
            int recurrenceLen) {
        super();
        this.period = period;
        this.recurrences = recurrences;
        this.recurrenceLen = recurrenceLen;
    }

    public String getPeriod() {
        return period;
    }

    public void setPeriod(String period) {
        this.period = period;
    }

    public List<String> getRecurrencesList() {
        return recurrences;
    }

    public String getRecurrences(int i) {
        return this.recurrences.get(i);
    }

    public void setRecurrences(List<String> recurrences) {
        this.recurrences = recurrences;
    }

    public int getRecurrenceLen() {
        return recurrenceLen;
    }

    public void setRecurrenceLen(int recurrenceLen) {
        this.recurrenceLen = recurrenceLen;
    }

}
