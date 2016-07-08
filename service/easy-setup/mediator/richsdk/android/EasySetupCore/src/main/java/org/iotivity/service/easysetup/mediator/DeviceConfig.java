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

public class DeviceConfig
{
    private final String mId;
    private final String mName;
    private final String mLanguage;
    private final String mCountry;

    public DeviceConfig(String id, String name, String language, String country)
    {
        mId = id;
        mName = name;
        mLanguage = language;
        mCountry = country;
    }

    public String getId()
    {
        return mId;
    }

    public String getName()
    {
        return mName;
    }

    public String getLanguage()
    {
        return mLanguage;
    }

    public String getCountry()
    {
        return mCountry;
    }
}
