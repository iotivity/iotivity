//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef _IN_PROC_CLIENT_WRAPPER_H_
#define _IN_PROC_CLIENT_WRAPPER_H_

#include <thread>
#include <mutex>
#include <sstream>
#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <OCApi.h>
#include <ocstack.h>
#include <IClientWrapper.h>
#include <InitializeException.h>
#include <ResourceInitException.h>

namespace OC
{
    class InProcClientWrapper : public IClientWrapper
    {
    public:
        InProcClientWrapper(PlatformConfig cfg);
        virtual ~InProcClientWrapper();

        virtual int ListenForResource(const std::string& serviceUrl, const std::string& resourceType, std::function<void(OCResource::Ptr)>& callback);

    private:
        void listeningFunc();
        std::thread m_listeningThread;
        bool m_threadRun;
        std::mutex m_resourceListenerLock;
		std::mutex m_csdkLock;
        std::vector<std::function<void(OCClientResponse*)>> callbackList;
    };
}

#endif
