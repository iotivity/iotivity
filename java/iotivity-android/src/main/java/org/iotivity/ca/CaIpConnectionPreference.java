/*
 ********************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
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

package org.iotivity.ca;

import java.security.InvalidParameterException;
import java.util.EnumSet;

public enum CaIpConnectionPreference {

    /** Cloud TCP (default) */
    CA_CLOUD (0),
    /** local UDP */
    CA_LOCAL_UDP (1),
    /** local TCP */
    CA_LOCAL_TCP (2),
    ;

    private int value;

    private CaIpConnectionPreference(int value) {
        this.value = value;
    }

    public int getValue() {
        return this.value;
    }
}
