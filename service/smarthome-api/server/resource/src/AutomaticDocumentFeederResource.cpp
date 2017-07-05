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
#include <CommonApi.h>
#include <CommonException.h>
#include <ResourceQuery.h>
#include <AutomaticDocumentFeederResource.h>

const static std::string URI_AUTOMATICDOCUMENTFEEDER = "/AutomaticDocumentFeederResURI";
const static std::string KEY_CURRENT_STATE = "currentAdfState";
const static std::string KEY_STATES = "adfStates";

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            AutomaticDocumentFeederResource::AutomaticDocumentFeederResource() :
                SHBaseResource(URI_AUTOMATICDOCUMENTFEEDER,
                        RESOURCE_TYPE::OPERATIONAL_STATE,
                        RESOURCE_PROPERTY::DISCOVERABLE | RESOURCE_PROPERTY::OBSERVABLE)
            {
            }

            AutomaticDocumentFeederResource::~AutomaticDocumentFeederResource()
            {
            }

            std::string AutomaticDocumentFeederResource::getCurrentState()
            {
                PropertyBundle storedBundle;
                storedBundle = getPropertyBundle();

                std::string currentState;
                storedBundle.getValue(KEY_CURRENT_STATE, currentState);

                return currentState;
            }

            std::list<std::string> AutomaticDocumentFeederResource::getPossibleStates()
            {
                PropertyBundle storedBundle;
                storedBundle = getPropertyBundle();

                std::list<std::string> possibleStates;
                storedBundle.getValue(KEY_STATES, possibleStates);

                return possibleStates;
            }

            bool AutomaticDocumentFeederResource::addPossibleState(std::string state)
            {
                PropertyBundle storedBundle;
                storedBundle = getPropertyBundle();

                std::list<std::string> possibleStates;
                storedBundle.getValue(KEY_STATES, possibleStates);

                possibleStates.push_back(state);
                storedBundle.setValue(KEY_STATES, possibleStates);

                setPropertyBundle(storedBundle);

                return true;
            }

            ResultCode AutomaticDocumentFeederResource::onGet(RequestId requestId,
                    const ResourceQuery& query)
            {
                std::cout << "[AutomaticDocumentFeederResource] onGet" << std::endl;

                return SUCCESS;
            }

            ResultCode AutomaticDocumentFeederResource::onSet(RequestId requestId,
                    const PropertyBundle& bundle, const ResourceQuery& query)
            {
                std::cout << "[AutomaticDocumentFeederResource] onSet" << std::endl;
                std::cout << "[AutomaticDocumentFeederResource] No supported request.."
                        << std::endl;

                return FAIL;
            }
        }
    }
}
