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
 * RemoteOpenLevelResource.
 */

#ifndef SMARTHOME_API_CLIENT_REMOTEOPENLEVEL_H_
#define SMARTHOME_API_CLIENT_REMOTEOPENLEVEL_H_

#include <SHBaseRemoteResource.h>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            /**
             * This class contains a set of functions for callback of
             * RemoteOpenLevelResource class.
             */
            class RemoteOpenLevelResourceDelegate
            {
            public:
                /**
                 * Virtual destructor
                 */
                virtual ~RemoteOpenLevelResourceDelegate() {}

                /**
                 * API to handle a response about get open level request.
                 *
                 * @param[in] currentLevel How open or ajar the entity is.
                 * @param[in] increment The step between possible values.
                 * @param[in] min Lower bound of open level.
                 * @param[in] max Upper bound of open level.
                 * @param[in] ret ResultCode about response.(SUCCESS OR FAIL)
                 */
                virtual void onGetLevel(int currentLevel, int increment,
                                        int min, int max, ResultCode ret) = 0;

                /**
                 * API to handle a response about change open level request.
                 *
                 * @param[in] currentLevel How open or ajar the entity is.
                 * @param[in] increment The step between possible values.
                 * @param[in] min Lower bound of open level.
                 * @param[in] max Upper bound of open level.
                 * @param[in] ret ResultCode about response.(SUCCESS OR FAIL)
                 */
                virtual void onChangeLevel(int currentLevel, int increment,
                                           int min, int max, ResultCode ret) = 0;
            };

            /**
             * This class contains a set of functions to control RemoteOpenLevelResource.
             */
            class RemoteOpenLevelResource: public SHBaseRemoteResource,
                                           public SHBaseRemoteResourceDelegate
            {
            friend class SHBaseRemoteResourceBuilder;
            public:
                RemoteOpenLevelResource();
                virtual ~RemoteOpenLevelResource();

                /**
                 * API to get the open level of remote device.
                 */
                void getLevel();

                /**
                 * API to change the open level of remote device.
                 *
                 * @param[in] level open level
                 */
                void changeLevel(int level);

                /**
                 * API to set delegate of RemoteOpenLevelResource
                 *
                 * @param[in] delegate  Instance that inherit RemoteOpenLevelResourceDelegate class.
                 */
                void setDelegate(RemoteOpenLevelResourceDelegate* delegate);

            private:
                virtual void onGet(PropertyBundle bundle, ResultCode ret);
                virtual void onSet(PropertyBundle bundle, ResultCode ret);
                virtual void onObserve(PropertyBundle bundle, const ObserveResponse obsType,
                        ResultCode ret);
                RemoteOpenLevelResourceDelegate *m_delegate;
            };
        }
    }
}

#endif /* SMARTHOME_API_CLIENT_REMOTEOPENLEVEL_H_ */
