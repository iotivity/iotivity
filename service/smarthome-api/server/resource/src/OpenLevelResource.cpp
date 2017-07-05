/* ****************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
 *
 *
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
 ******************************************************************/
#include <OpenLevelResource.h>
#include <ResourceQuery.h>
#include <PropertyBundle.h>
#include "logger.h"

#define TAG "SH_SERVER_OPENLEVEL_RESOURCE"

const static std::string URI_OPENLEVEL = "/OpenLevelResURI";
const static std::string KEY_OPENLEVEL = "openLevel";
const static std::string KEY_INCREMENT = "increment";
const static std::string KEY_RANGE = "range";

const static int DEFAULT_INCREMENT = 1;
const static int DEFAULT_MIN_VALUE = 0;
const static int DEFAULT_MAX_VALUE = 100;

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            OpenLevelResource::OpenLevelResource()
            : m_delegate(NULL), SHBaseResource(URI_OPENLEVEL, RESOURCE_TYPE::OPENLEVEL,
                RESOURCE_PROPERTY::DISCOVERABLE | RESOURCE_PROPERTY::OBSERVABLE)
            {
                std::list<int> range;
                range.push_back(DEFAULT_MIN_VALUE);
                range.push_back(DEFAULT_MAX_VALUE);

                PropertyBundle bundle;
                bundle.setValue(KEY_OPENLEVEL, DEFAULT_MIN_VALUE);
                bundle.setValue(KEY_INCREMENT, DEFAULT_INCREMENT);
                bundle.setValue(KEY_RANGE, range);

                setPropertyBundle(bundle);
            }

            OpenLevelResource::~OpenLevelResource()
            {
            }

            int OpenLevelResource::getLevel()
            {
                OIC_LOG(DEBUG, TAG, "Entered getLevel");

                PropertyBundle bundle = getPropertyBundle();
                int level = 0;
                bundle.getValue(KEY_OPENLEVEL, level);
                return level;
            }

            int OpenLevelResource::getIncrement()
            {
                OIC_LOG(DEBUG, TAG, "Entered getIncrement");

                PropertyBundle bundle = getPropertyBundle();
                int increment = 0;
                bundle.getValue(KEY_INCREMENT, increment);
                return increment;
            }

            bool OpenLevelResource::getRange(int &min, int &max)
            {
                OIC_LOG(DEBUG, TAG, "Entered getRange");

                PropertyBundle bundle = getPropertyBundle();
                std::list<int> range;
                bundle.getValue(KEY_RANGE, range);
                min = *(range.begin());
                max = *(range.rbegin());

                return true;
            }

            void OpenLevelResource::setLevel(int level)
            {
                OIC_LOG(DEBUG, TAG, "Entered setLevel");

                PropertyBundle bundle = getPropertyBundle();
                bundle.setValue(KEY_OPENLEVEL, level);
                setPropertyBundle(bundle);
            }

            void OpenLevelResource::setIncrement(int increment)
            {
                OIC_LOG(DEBUG, TAG, "Entered setIncrement");

                PropertyBundle bundle = getPropertyBundle();
                bundle.setValue(KEY_INCREMENT, increment);
                setPropertyBundle(bundle);
            }

            void OpenLevelResource::setRange(int min, int max)
            {
                OIC_LOG(DEBUG, TAG, "Entered setRange");

                std::list<int> range;
                range.push_back(min);
                range.push_back(max);

                PropertyBundle bundle = getPropertyBundle();
                bundle.setValue(KEY_RANGE, range);
                setPropertyBundle(bundle);
            }

            void OpenLevelResource::setDelegate(OpenLevelResourceDelegate* delegate)
            {
                OIC_LOG(DEBUG, TAG, "Entered setDelegate");

                this->m_delegate = delegate;
                SHBaseResource::setDelegate(this);
            }

            ResultCode OpenLevelResource::onGet(RequestId requestId, const ResourceQuery& query)
            {
                OIC_LOG(DEBUG, TAG, "Entered onGet");
                return SUCCESS;
            }

            ResultCode OpenLevelResource::onSet(RequestId requestId, const PropertyBundle& bundle,
                                                const ResourceQuery& query)
            {
                OIC_LOG(DEBUG, TAG, "Entered onSet");

                if (bundle.contains(KEY_INCREMENT))
                {
                    OIC_LOG(ERROR, TAG, "increment is readOnly property");
                    return FAIL;
                }

                if (bundle.contains(KEY_RANGE))
                {
                    OIC_LOG(ERROR, TAG, "range is readOnly property");
                    return FAIL;
                }

                if (!bundle.contains(KEY_OPENLEVEL))
                {
                    OIC_LOG(ERROR, TAG, "openLevel is mandatory property");
                    return FAIL;
                }

                PropertyBundle storedBundle = getPropertyBundle();
                std::list<int> range;
                storedBundle.getValue(KEY_RANGE, range);

                int min = *(range.begin());
                int max = *(range.rbegin());

                int level = 0;
                bundle.getValue(KEY_OPENLEVEL, level);
                if (level < min || max < level)
                {
                    // Error response will be sent by Resource Class.
                    OIC_LOG(ERROR, TAG, "Out of range property value for openLevel");
                    return FAIL;
                }

                ResultCode retCode = this->m_delegate->onChangeLevel(level);
                if (retCode == SUCCESS)
                {
                    // Update property with the requested value.
                    storedBundle.setValue(KEY_OPENLEVEL, level);
                    setPropertyBundle(storedBundle);
                }
                return retCode;
            }
        }
    }
}
