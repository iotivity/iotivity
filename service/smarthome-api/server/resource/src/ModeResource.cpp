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
#include <ModeResource.h>
#include <PropertyBundle.h>
#include <ResourceQuery.h>
#include "logger.h"

#define TAG "SH_SERVER_MODE_RESOURCE"

const static std::string URI_MODE = "/mode";
const static std::string KEY_SUPPORTEDMODES = "supportedModes";
const static std::string KEY_MODES = "modes";

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            ModeResource::ModeResource() :
                    m_userDelegate(NULL), SHBaseResource(URI_MODE, RESOURCE_TYPE::MODE)
            {
                OIC_LOG(DEBUG, TAG, "Entered ctor");

                std::list< std::string > emptylist;
                PropertyBundle bundle;
                bundle.setValue(KEY_SUPPORTEDMODES, emptylist);
                bundle.setValue(KEY_MODES, emptylist);
                setPropertyBundle(bundle);
            }

            ModeResource::~ModeResource()
            {
            }

            void ModeResource::addSupportedMode(const std::list< std::string >& supportedMode)
            {
                OIC_LOG(DEBUG, TAG, "Entered addSupportedMode");

                if (supportedMode.empty())
                {
                    return;
                }

                PropertyBundle storedBundle = getPropertyBundle();

                std::list< std::string > curSupportedMode;
                storedBundle.getValue(KEY_SUPPORTEDMODES, curSupportedMode);

                std::list< std::string > newSupportedMode = curSupportedMode;

                if (curSupportedMode.empty())
                {
                    storedBundle.setValue(KEY_SUPPORTEDMODES, supportedMode);
                    setPropertyBundle(storedBundle);
                    return;
                }

                for (std::list< std::string >::const_iterator srcItr = supportedMode.begin();
                        srcItr != supportedMode.end(); srcItr++)
                {
                    bool isFound = false;
                    for (std::list< std::string >::iterator curItr = curSupportedMode.begin();
                            curItr != curSupportedMode.end(); curItr++)
                    {
                        if (0 == (*srcItr).compare((*curItr)))
                        {
                            isFound = true;
                            break;
                        }
                    }

                    if (!isFound)
                    {
                        newSupportedMode.insert(newSupportedMode.end(), *srcItr);
                    }
                }

                storedBundle.setValue(KEY_SUPPORTEDMODES, newSupportedMode);
                setPropertyBundle(storedBundle);
                return;
            }

            void ModeResource::removeSupportedMode(const std::string& removeMode)
            {
                OIC_LOG(DEBUG, TAG, "Entered removeSupportedMode");

                if (removeMode.empty())
                {
                    // TODO
                    // How about logging string is empty
                    return;
                }

                PropertyBundle storedBundle = getPropertyBundle();

                bool isFound = false;
                std::list< std::string > supportedMode;
                storedBundle.getValue(KEY_SUPPORTEDMODES, supportedMode);

                for (std::list< std::string >::iterator itr = supportedMode.begin();
                        itr != supportedMode.end(); itr++)
                {
                    if (0 == (*itr).compare(removeMode))
                    {
                        isFound = true;
                        supportedMode.erase(itr);
                        break;
                    }
                }

                if (isFound)
                {
                    storedBundle.setValue(KEY_SUPPORTEDMODES, supportedMode);
                    setPropertyBundle(storedBundle);
                }
                else
                {
                    // TODO
                    // How about logging given mode is not found in list.
                    return;
                }
                return;
            }

            std::list< std::string > ModeResource::getSupportedMode()
            {
                OIC_LOG(DEBUG, TAG, "Entered getSupportedMode");

                std::list< std::string > supportedMode;

                PropertyBundle storedBundle = getPropertyBundle();
                storedBundle.getValue(KEY_SUPPORTEDMODES, supportedMode);

                return supportedMode;
            }

            void ModeResource::setCurrentMode(const std::list< std::string >& currentMode)
            {
                OIC_LOG(DEBUG, TAG, "Entered setCurrentMode");

                PropertyBundle storedBundle = getPropertyBundle();
                storedBundle.setValue(KEY_MODES, currentMode);
                setPropertyBundle(storedBundle);
                return;
            }

            std::list< std::string > ModeResource::getCurrentMode()
            {
                OIC_LOG(DEBUG, TAG, "Entered getCurrentMode");

                std::list< std::string > currentMode;

                PropertyBundle storedBundle = getPropertyBundle();
                storedBundle.getValue(KEY_MODES, currentMode);
                return currentMode;
            }

            void ModeResource::setModeResourceDelegate(ModeResourceDelegate *modeDelegate)
            {
                OIC_LOG(DEBUG, TAG, "Entered setModeResourceDelegate");

                m_userDelegate = modeDelegate;
                setDelegate(this);
                return;
            }

            ResultCode ModeResource::onGet(RequestId requestId, const ResourceQuery& query)
            {
                OIC_LOG(DEBUG, TAG, "Entered onGet");

                m_userDelegate->getModeCallback();

                return SUCCESS;
            }

            ResultCode ModeResource::onSet(RequestId requestId, const PropertyBundle& bundle,
                                           const ResourceQuery& query)
            {
                OIC_LOG(DEBUG, TAG, "Entered onSet");

                // spec says request payload and success case required "modes" property
                // when failed case payload required "modes", "supportedModes"
                // but we will always send both property together to keep consistency on
                // client side delegate callback

                PropertyBundle storedBundle = getPropertyBundle();

                if (!bundle.contains(KEY_MODES))
                {
                    return FAIL;
                }

                if (!storedBundle.contains(KEY_MODES))
                {
                    return FAIL;
                }

                if (!storedBundle.contains(KEY_SUPPORTEDMODES))
                {
                    return FAIL;
                }

                std::list< std::string > requestList;
                bundle.getValue(KEY_MODES, requestList);
                ResultCode retCode = m_userDelegate->setModeCallback(requestList);

                switch (retCode)
                {
                    case SUCCESS:
                        storedBundle.setValue(KEY_MODES, requestList);
                        setPropertyBundle(storedBundle);
                        return SUCCESS;

                    case FAIL:
                        return FAIL;

                    case KEEP:
                        return KEEP;

                    default:
                        return KEEP;
                }

                return KEEP;
            }
        }
    }
}
