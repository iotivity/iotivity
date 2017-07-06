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
 * This file contains the declaration of classes and its members related to ModeResource.
 */

#ifndef SMARTHOME_API_SERVER_MODE_H_
#define SMARTHOME_API_SERVER_MODE_H_

#include <SHBaseResource.h>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            /**
             * This class contains a set of functions for callback of
             * ModeResource class.
             */
            class ModeResourceDelegate
            {
            public:
                virtual ~ModeResourceDelegate() {}

                /**
                 * API to handle a mode get request .
                 *
                 * @return ResultCode about response.(SUCCESS OR FAIL)
                 */
                virtual ResultCode getModeCallback() = 0;

                /**
                 * API to handle a mode set request .
                 *
                 * @param[in] mode mode value
                 * @return ResultCode about response.(SUCCESS OR FAIL)
                 */
                virtual ResultCode setModeCallback(std::list< std::string > mode) = 0;
            };

            /**
             * This class contains a set of functions to change the properties of Mode.
             */
            class ModeResource: public SHBaseResource, public SHBaseResourceDelegate
            {
            public:
                ModeResource();
                virtual ~ModeResource();

                /**
                 * API to add supported mode.
                 *
                 * @param[in] supportedMode supported modes
                 */
                void addSupportedMode(const std::list< std::string >& supportedMode);

                /**
                 * API to remove supported mode.
                 *
                 * @param[in] removeMode removed mode
                 */
                void removeSupportedMode(const std::string& removeMode);

                /**
                 * API to get supported mode.
                 *
                 * @return supported modes
                 */
                std::list< std::string > getSupportedMode();

                /**
                 * API to set current modes.
                 *
                 * @param[in] currentMode currment modes
                 */
                void setCurrentMode(const std::list< std::string >& currentMode);

                /**
                 * API to get current modes.
                 *
                 * @return current modes
                 */
                std::list< std::string > getCurrentMode();

                /**
                 * API to set a instance of delegate class.
                 *
                 * @param[in] modeDelegate delegate
                 */
                void setModeResourceDelegate(ModeResourceDelegate *modeDelegate);

            private:
                ResultCode onGet(RequestId requestId, const ResourceQuery& query);
                ResultCode onSet(RequestId requestId, const PropertyBundle& bundle,
                                 const ResourceQuery& query);

                ModeResourceDelegate *m_userDelegate;
            };
        }
    }
}

#endif /* SMARTHOME_API_SERVER_MODE_H_ */
