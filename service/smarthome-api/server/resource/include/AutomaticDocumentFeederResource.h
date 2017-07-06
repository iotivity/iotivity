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

/**
 * This file contains the declaration of classes and its members related to
 * AutomaticDocumentFeederResource.
 */

#ifndef SMARTHOME_API_SERVER_AUTOMATICDOCUMENTFEEDER_H_
#define SMARTHOME_API_SERVER_AUTOMATICDOCUMENTFEEDER_H_

#include <SHBaseResource.h>
#include <CommonApi.h>
#include <PropertyBundle.h>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            /**
             * This class contains a set of functions to change the properties of AutomaticDocumentFeeder.
             */
            class AutomaticDocumentFeederResource: public SHBaseResource,
                    public SHBaseResourceDelegate
            {
            public:
                AutomaticDocumentFeederResource();
                virtual ~AutomaticDocumentFeederResource();

                /**
                 * API to get the current state.
                 *
                 * @return current state
                 */
                std::string getCurrentState();

                /**
                 * API to get the possible state.
                 *
                 * @return possible states
                 */
                std::list<std::string> getPossibleStates();

                /**
                 * API to add the possible state.
                 *
                 * @return true if success
                 */
                bool addPossibleState(std::string state);

                /**
                 * API to set the current state.
                 *
                 * @return true if success
                 */
                bool setCurrentState(std::string state);

            private:
                ResultCode onGet(RequestId requestId, const ResourceQuery& query);
                ResultCode onSet(RequestId requestId, const PropertyBundle& bundle,
                        const ResourceQuery& query);
                std::list<std::string> possibleStates;
            };
        }
    }
}

#endif /* SMARTHOME_API_SERVER_AUTOMATICDOCUMENTFEEDER_H_ */
