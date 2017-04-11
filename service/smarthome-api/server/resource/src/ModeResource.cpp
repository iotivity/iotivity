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
#include <ResourceQuery.h>
#include <iostream>

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
                    m_userDelegate(NULL), SHBaseResource(URI_MODE)
            {
                std::cout << "[ModeResource] constructor" << std::endl;

                std::list<std::string> types;
                types.push_back(RESOURCE_TYPE::MODE);
                setTypes(types);

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
                std::cout << "[ModeResource] addSupportedMode" << std::endl;

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
                std::cout << "[Mode] removeSupportedMode" << std::endl;

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
                std::cout << "[Mode] getSupportedMode" << std::endl;

                std::list< std::string > supportedMode;

                PropertyBundle storedBundle = getPropertyBundle();
                storedBundle.getValue(KEY_SUPPORTEDMODES, supportedMode);

                return supportedMode;
            }

            void ModeResource::setCurrentMode(const std::list< std::string >& currentMode)
            {
                std::cout << "[ModeResource] setCurrentMode" << std::endl;

                PropertyBundle storedBundle = getPropertyBundle();
                storedBundle.setValue(KEY_MODES, currentMode);
                setPropertyBundle(storedBundle);
                return;
            }

            std::list< std::string > ModeResource::getCurrentMode()
            {
                std::cout << "[ModeResource] getCurrentMode" << std::endl;

                std::list< std::string > currentMode;

                PropertyBundle storedBundle = getPropertyBundle();
                storedBundle.getValue(KEY_MODES, currentMode);
                return currentMode;
            }

            void ModeResource::setModeResourceDelegate(ModeResourceDelegate *modeDelegate)
            {
                std::cout << "[ModeResource] setModeResourceDelegate" << std::endl;
                m_userDelegate = modeDelegate;
                setDelegate(this);
                return;
            }

            ResultCode ModeResource::onGet(int requestId, const ResourceQuery& query)
            {
                std::cout << "[ModeResource] onGet" << std::endl;

                PropertyBundle storedBundle = getPropertyBundle();

                if (!storedBundle.contains(KEY_MODES))
                {
                    return FAIL;
                }

                if (!storedBundle.contains(KEY_SUPPORTEDMODES))
                {
                    return FAIL;
                }

                sendResponse(requestId, storedBundle);
                return KEEP;
            }

            ResultCode ModeResource::onSet(int requestId, const PropertyBundle& bundle,
                                           const ResourceQuery& query)
            {
                std::cout << "[ModeResource] onSet" << std::endl;

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
                        sendResponse(requestId, storedBundle);
                        return KEEP;

                    case FAIL:
                        sendErrorResponse(requestId, storedBundle);
                        return KEEP;

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
