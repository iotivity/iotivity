/**
 * ***************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * ****************************************************************
 */

package org.iotivity.service.easysetup.mediator;

/*
 * This class stores an Enrollee's configuration specific to device configuration
 * device name, language, and country information are stored
 */
public class DeviceConfig
{
    private final String mName;
    private final String mLanguage;
    private final String mCountry;

    /**
     * Constructor
     *
     * @param name Device's human-friendly name like device model name
     * @param language IETF language tag using ISO 639X
     * @param country ISO Country Code (ISO 3166-1 Alpha-2)
     */
    public DeviceConfig(String name, String language, String country)
    {
        mName = name;
        mLanguage = language;
        mCountry = country;
    }

    /**
     * Get a device name
     *
     * @return a device name
     */
    public String getName()
    {
        return mName;
    }

    /**
     * Get a language
     *
     * @return a language
     */
    public String getLanguage()
    {
        return mLanguage;
    }

    /**
     * Get a country
     *
     * @return a country
     */
    public String getCountry()
    {
        return mCountry;
    }
}
