/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *		http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#include "ResourceProperties.h"

namespace RAML
{

    void ResourceProperties::readJson()
    {
        if (! m_cjson)
            return;
        cJSON *jsonrt = cJSON_GetObjectItem(m_cjson, "rt");
        m_rt = jsonrt->valuestring;

        cJSON *jsonif = cJSON_GetObjectItem(m_cjson, "if");
        m_if = jsonif->valuestring;

        cJSON *jsonProperties = cJSON_GetObjectItem(m_cjson, "properties");
        cJSON *childProperties = jsonProperties->child;

        while (childProperties)
        {
            std::string attName = childProperties->string;

            std::string attType = cJSON_GetObjectItem(childProperties, "type")->valuestring;
            if (attType == "string")
            {
                addAttribute(attName , std::string(cJSON_GetObjectItem(childProperties, "default")->valuestring));
                cJSON *allowedvalues = cJSON_GetObjectItem(childProperties, "values");
                if (allowedvalues)
                {
                    int size = cJSON_GetArraySize(allowedvalues);
                    int idx = 0;
                    std::vector<std::string> allwdValues;

                    do
                    {
                        allwdValues.push_back(cJSON_GetArrayItem(allowedvalues, idx)->valuestring);

                    }
                    while ( ++idx < size);

                    setAllowedValues(attName, allwdValues);
                }
            }
            else
            {
                addAttribute(attName , int(cJSON_GetObjectItem(childProperties, "default")->valueint));
                cJSON *allowedvalues = cJSON_GetObjectItem(childProperties, "values");
                if (allowedvalues)
                {
                    int size = cJSON_GetArraySize(allowedvalues);
                    int idx = 0;
                    std::vector<int> allwdValues;

                    do
                    {
                        allwdValues.push_back(cJSON_GetArrayItem(allowedvalues, idx)->valueint);

                    }
                    while ( ++idx < size);

                    setAllowedValues(attName, allwdValues);
                }
                cJSON *ranges = cJSON_GetObjectItem(childProperties, "range");
                if (ranges)
                {
                    int sizeRange = cJSON_GetArraySize(ranges);
                    if (sizeRange == 2)
                    {
                        setRange(attName , (cJSON_GetArrayItem(ranges, 0)->valueint) , (cJSON_GetArrayItem(ranges,
                                 1)->valueint));
                    }
                }

            }

            setUpdateInterval(attName , cJSON_GetObjectItem(childProperties, "update_frequency")->valueint);
            childProperties = childProperties->next;
        }

    }

}

