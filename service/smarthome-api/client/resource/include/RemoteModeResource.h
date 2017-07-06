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
 * RemoteModeResource.
 */

#ifndef SMARTHOME_API_CLIENT_REMOTEMODE_H_
#define SMARTHOME_API_CLIENT_REMOTEMODE_H_

#include <list>
#include <SHBaseRemoteResource.h>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            /**
             * This class contains a set of functions for callback of
             * RemoteModeResource class.
             */
            class RemoteModeResourceDelegate
            {
            public:
                /**
                 * Virtual destructor.
                 */
                virtual ~RemoteModeResourceDelegate() {}

                /**
                 * API to handle a response about Mode get request.
                 *
                 * @param[in] supportedMode supported mode
                 * @param[in] currentMode current mode
                 * @param[in] ret ResultCode about response.(SUCCESS OR FAIL)
                 */
                virtual void onGetMode(const std::list< std::string >& supportedMode,
                        const std::list< std::string >& currentMode, ResultCode ret) = 0;

                /**
                 * API to handle a response about Mode set request.
                 *
                 * @param[in] supportedMode supported mode
                 * @param[in] currentMode current mode
                 * @param[in] ret ResultCode about response.(SUCCESS OR FAIL)
                 */
                virtual void onSetMode(const std::list< std::string >& supportedMode,
                        const std::list< std::string >& currentMode, ResultCode ret) = 0;
            };

            /**
             * This class contains a set of functions to control RemoteModeResource.
             */
            class RemoteModeResource: public SHBaseRemoteResource,
                    public SHBaseRemoteResourceDelegate
            {
            friend class SHBaseRemoteResourceBuilder;
            public:
                virtual ~RemoteModeResource();

                /**
                 * API to set a instance of delegate class.
                 */
                void setRemoteModeResourceDelegate(RemoteModeResourceDelegate *delegate);

                /**
                 * API to get current mode.
                 */
                void getMode();

                /**
                 * API to set current mode,
                 * @param[in] mode current modes
                 */
                void setMode(std::list< std::string > mode);

                virtual void onGet(PropertyBundle bundle, ResultCode ret);
                virtual void onSet(PropertyBundle bundle, ResultCode ret);
                virtual void onObserve(PropertyBundle bundle, const ObserveResponse obsType,
                        ResultCode ret);

            protected:
                RemoteModeResource();

            private:
                RemoteModeResourceDelegate *m_delegate;
            };
        }
    }
}

#endif /* SMARTHOME_API_CLIENT_REMOTEMODE_H_ */
