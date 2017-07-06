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
 * This file contains the declaration of classes and its members related to OpenLevelResource.
 */

#ifndef SMARTHOME_API_SERVER_OPENLEVEL_H_
#define SMARTHOME_API_SERVER_OPENLEVEL_H_

#include <SHBaseResource.h>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            /**
             * This class contains a set of callback functions handling events and requests
             * for OpenLevelResource class
             */
            class OpenLevelResourceDelegate
            {
            public:
                virtual ~OpenLevelResourceDelegate() {}

                /**
                 * API to handle level change request .
                 *
                 * @return ResultCode about response.(SUCCESS OR FAIL)
                 */
                virtual ResultCode onChangeLevel(int level) = 0;
            };

            /**
             * This class contains a set of functions to change the properties of OpenLevel.
             */
            class OpenLevelResource: public SHBaseResource, public SHBaseResourceDelegate
            {
            public:
                OpenLevelResource();
                virtual ~OpenLevelResource();

                /**
                 * API to get the open level.
                 *
                 * @return How open or ajar the entity is.
                 */
                int getLevel();

                /**
                 * API to get the increment.
                 *
                 * @return The step between possible values.
                 */
                int getIncrement();

                /**
                 * API to get the range. If no range is provided then 0 to 100 is assumed.
                 * 0 means closed, 100 means fully open.
                 *
                 * @param[out] min Lower bound.
                 * @param[out] max Upper bound.
                 */
                bool getRange(int &min, int &max);

                /**
                 * API to set the open level.
                 *
                 * @param[in] level How open or ajar the entity is.
                 */
                void setLevel(int level);

                /**
                 * API to set the increment.
                 *
                 * @param[in] increment The step between possible values.
                 */
                void setIncrement(int increment);

                /**
                 * API to set the range.
                 *
                 * @param[in] min Lower bound.
                 * @param[in] max Upper bound.
                 */
                void setRange(int min, int max);

                /**
                 * API to set delegate of OpenLevelResource.
                 *
                 * @param[in] delegate Instance that inherit OpenLevelResourceDelegate class.
                 */
                void setDelegate(OpenLevelResourceDelegate* delegate);

            private:
                ResultCode onGet(RequestId requestId, const ResourceQuery& query);
                ResultCode onSet(RequestId requestId, const PropertyBundle& bundle,
                                 const ResourceQuery& query);

                OpenLevelResourceDelegate *m_delegate;
            };
        }
    }
}

#endif /* SMARTHOME_API_SERVER_OPENLEVEL_H_ */
