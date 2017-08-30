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

import java.security.InvalidParameterException;
import java.util.EnumSet;

public enum MVJustWorksOptionMask {
    NOT_APPLICABLE (0x0),
    DISPLAY_MUTUAL_VERIF_NUM  (0x1 << 0),
    CONFIRM_MUTUAL_VERIF_NUM  (0x1 << 1);

    private int value;

    private MVJustWorksOptionMask(int value) {
        this.value = value;
    }

    public int getValue() {
        return this.value;
    }

    public static EnumSet<MVJustWorksOptionMask> convertToEnumSet(int value) {
        EnumSet<MVJustWorksOptionMask> typeSet = null;

        for (MVJustWorksOptionMask v : values()) {
            if (0 != (value & v.getValue())) {
                if (null == typeSet) {
                    typeSet = EnumSet.of(v);
                } else {
                    typeSet.add(v);
                }
            }
        }

        if (null == typeSet || typeSet.isEmpty()) {
            throw new InvalidParameterException("Unexpected MVJustWorksOptionMask value:" + value);
        }

        return typeSet;
    }

}
