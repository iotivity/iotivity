/*
 *******************************************************************
 *
 * Copyright 2015 Intel Corporation.
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

/**
 * OcHeaderOption class allows to create instances which comprises optionId
 * and optionData as members. These are used in setting Header options.
 * After creating instances of OcHeaderOptions, use setHeaderOptions API
 * (in OcResource) to set header Options.
 * NOTE: optionId  is an integer value which MUST be within
 * range of 2048 to 3000 inclusive of lower and upper bound
 * except for a few options including If-Match with empty(num : 1),
 * If-None-Match(num : 5), Location-Path(num : 8),
 * Location-Query(num : 20) CoAP option.
 * HeaderOption instance creation fails if above condition is not satisfied.
 */
public class OcHeaderOption {

    public static final int MIN_HEADER_OPTION_ID = 2048;
    public static final int MAX_HEADER_OPTION_ID = 3000;
    public static final int IF_MATCH_OPTION_ID = 1;
    public static final int IF_NONE_MATCH_OPTION_ID = 5;
    public static final int LOCATION_PATH_OPTION_ID = 8;
    public static final int LOCATION_QUERY_OPTION_ID = 20;
    public static final int ACCEPT_OPTION_ID = 17;
    public static final int CONTENT_TYPE_OPTION_ID = 12;

    private int mOptionId;
    private String mOptionData;

    public OcHeaderOption(int optionId, String optionData) {
        if (!(optionId >= MIN_HEADER_OPTION_ID && optionId <= MAX_HEADER_OPTION_ID)
                && optionId != IF_MATCH_OPTION_ID
                && optionId != IF_NONE_MATCH_OPTION_ID
                && optionId != LOCATION_PATH_OPTION_ID
                && optionId != LOCATION_QUERY_OPTION_ID
                && optionId != ACCEPT_OPTION_ID
                && optionId != CONTENT_TYPE_OPTION_ID) {
            throw new InvalidParameterException("Option ID range is invalid");
        }

        this.mOptionId = optionId;
        this.mOptionData = optionData;
    }

    /**
     * API to get Option ID
     *
     * @return option ID
     */
    public int getOptionId() {
        return mOptionId;
    }

    /**
     * API to get Option data
     *
     * @return option data
     */
    public String getOptionData() {
        return mOptionData;
    }
}
